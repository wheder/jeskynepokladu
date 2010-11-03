#include <iostream>
#include <cstdio>
#include <sstream>
#include <cstring>
#include <fstream>
#include <exception>
#include <cctype>
#include <iomanip>
#include <math.h>

#include "mpi.h"

#include <stdlib.h>

//#define __TEST 1


#define DESKRIPCNI_MESSAGE 1
    #define JOB_FINISHED 1
    #define JOB_RUNNING_AND_GIVE 2
    #define JOB_RUNNING_CANT_GIVE 3
#define CO_DELAT 2
    #define POSILAM_UKOL 1
    #define DEJ_PULKU 2
    #define POKRACUJ 3
    #define VRAT_NEJLEPSI 4

using namespace std;

struct Predmet {
    double C;
    double O;
};

int mpi_count = 0;
int mpi_nr = 0;

int idle = 0;

double * C = NULL;
double * O = NULL;
int pocet = 0;
double max_objem = 0;
int * max_arr = NULL;
double max_sum = 0;
int max_cnt = 0;

bool * send_array(int * arr, int cnt) {
    bool * out = new bool[pocet];
    for (int i =0;i<pocet;i++) out[i]=false;
    for (int i=0;i<cnt;i++)out[arr[i]]=true;
    return out;
}

int get_cnt_rec_arr(bool * in) {
    int cnt = 0;
    for (int i = 0; i < pocet; i++) if (in[i]) cnt++;
    return cnt;
}
int * recieve_array(bool * in) {
    int position = 0;
    int * arr = new int[pocet];
    for (int i =0; i< pocet; i++) {
        if (in[i]) {
            arr[position]=i;
            position++;
        }
    }
    return arr;
}

void porovnej_nejlepsi(bool * in) {
    int * arr = recieve_array(in);
    int pocet_prvku = get_cnt_rec_arr(in);
    double sum = 0;
    for (int i =0; i< pocet_prvku; i++) {
        sum += C[arr[i]];
    }
    if (sum > max_sum) {
        max_sum = sum;
        max_cnt = pocet_prvku;
        for (int i = 0; i<pocet_prvku; i++) max_arr[i] = arr[i];
    }


    delete [] arr;
}

int compare (const void * i1, const void * i2)
{
    struct Predmet *a = (struct Predmet *)i1;
    struct Predmet *b = (struct Predmet *)i2;
    double i =  ( (((struct Predmet*)a)->O) - (((struct Predmet*)b)->O) );
    if (i != 0) return ceil(i);
    return ceil  ( (((struct Predmet*)a)->C) - (((struct Predmet*)b)->C) );

}

void init() {
    Predmet * vals = new Predmet[pocet];
    for (int i=0;i<pocet;i++) {
        vals[i].C = C[i];
        vals[i].O = O[i];
    }
    qsort ( vals, pocet, sizeof(struct Predmet), compare );
    for (int i=0;i<pocet;i++) {
        C[i] = vals[i].C;
        O[i] = vals[i].O;
    }
    delete [] vals;
}

double spocitej(int filled, int * temp_arr, int new_index , double V_so_far, double C_so_far) {

    double ret = 0;

    temp_arr[filled] = new_index;
    //filled++;

    V_so_far += O[new_index];
    C_so_far += C[new_index];

    ret = C_so_far;
    if (V_so_far > max_objem) return -1.0;

    if (C_so_far > max_sum) {
        ret = C_so_far;
        max_sum = C_so_far;
        max_cnt = filled+1;
        for (int g =0; g<pocet; g++) max_arr[g]=temp_arr[g];
    }

    int index = new_index+1;

    while (index < pocet) {
        int * temp_array_test = new int[pocet];

        for (int g =0; g<pocet; g++) temp_array_test[g]=temp_arr[g];

        double max = spocitej(filled+1, temp_array_test, index, V_so_far, C_so_far);
        if (max > max_sum) {
            ret = max;
            max_sum = max;
            max_cnt = filled+2;
            //if (max_arr != NULL) delete [] max_arr;
            for (int g =0; g<pocet; g++) max_arr[g]=temp_array_test[g];
        }
        delete  [] temp_array_test;


        index++;
    }

    return C_so_far;
}

void posli_procesu_ukol(int proces_nr, int * arr, int cnt_given) {
    int posilam = POSILAM_UKOL;
    MPI_Send(&posilam, 1, MPI_INT, proces_nr, CO_DELAT, MPI_COMM_WORLD);
    posilam = cnt_given;
    MPI_Send(&posilam, 1, MPI_INT, proces_nr, CO_DELAT, MPI_COMM_WORLD);
    bool * pole = send_array(arr, cnt_given);
    MPI_Send(&pole, pocet, MPI_CHAR, proces_nr, CO_DELAT, MPI_COMM_WORLD);
    //verme ze to vsechno proslo ;-)
    delete [] pole; // snad je uklid korektni
}

int main(int argc, char *argv[])
{
    MPI_Init( &argc, &argv );

    /* find out process rank */
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_nr);

    /* find out number of processes */
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_count);
    if (mpi_count == 1) {
        cout << "Cannot run with just one process!" << endl;
        return 7;
    }

    if (argc != 2) {
        cout << "usage: " <<argv[0]<< " <inputfile>>" << endl;
        return 1;
    }

    ifstream inf;
    inf.open( argv[1], ios::in );
    if ( !inf.good () ) {
            cerr << "Chyba cteni z '" << argv[1] << "'." << endl;
            return 2;
    }
    else {
        //if (mpi_nr == 0) cout << "Oteviram soubor '" << argv[1] << "' ke cteni." << endl;
    }
    inf>>max_objem;
    if (!inf.good()) return 5;
    inf>>pocet;
    if (!inf.good()) return 6;
    if (pocet > 256) {
        cerr << "Pocet prvku nemuze byt vetsi nez 256!" << endl;
        return 8;
    }
    C = new double[pocet]; // cost
    O = new double[pocet]; // objem
    for ( int i=0 ; i<pocet ; i++ ) {
        inf>>C[i];
        if (!inf.good()) return 3;
        inf>>O[i];
        if (!inf.good()) return 4;
    }
    inf.close();


    /*for (int i=0;i<pocet;i++) {
        cout << "| ";
        printf("%.12f",O[i]);
        cout << " | ";
        printf("%.12f",C[i]);
        cout << " |" << endl;
    }
    cout <<endl;*/
    init(); //sortime pole, to se bude hodit - verime ze se vzdy vysorti identicky... a proc by taky ne, co?
    /*for (int i=0;i<pocet;i++) {
        cout << "| ";
        printf("%.12f",O[i]);
        cout << " | ";
        printf("%.12f",C[i]);
        cout << " |" << endl;
    }*/

    if (mpi_nr == 0) cout << "max_objem"<< max_objem<<endl;

    max_arr = new int[pocet];

    if (mpi_nr == 0) { /// ridici proces
        //rozdame zakladni praci
        int cislo = 0;
        int process_nr = 1;
        for (; cislo < pocet && cislo < (mpi_count-1); cislo++,process_nr++) {
            //poslat pole nejakymu troubovi
            //vytvorime nove pole k poslani - musime pak killnout!
            int * arr = new int[pocet];
            arr[0] = cislo;
            //posilame dalsi zadani!
            posli_procesu_ukol(process_nr, arr, 1);
            delete [] arr; // uklid poslaneho pole;

        }
        if (cislo == pocet && cislo == (mpi_count-1)) { // vyslo nam to akorat, procesy a cisla byla rovnomerne rozdelena.
            //asi pujdeme dal ;-)
        }
        else if (cislo == pocet) {//dodelali jsme  frontu cisel (je jich malo?), a zbyly nam procesy
            //chceme aby se to nekdy stalo?
            /// @TODO
        }
        else if (cislo == (mpi_count-1)) {//dosly nam procesy, cisel je jeste spousta. musime si na ne vytvorit frontu
            // nebo lepe budeme cekat nez nekdo skonci, a pak mu to soupneme ;-)
            // a znova a znova ;-)
            int process_nr = 1;
            while (cislo < pocet) {

                MPI_Status status;
                int flag = 0;
                MPI_Iprobe ( process_nr, DESKRIPCNI_MESSAGE, MPI_COMM_WORLD, &flag, &status );

                if (flag) {//ha, ma volno!
                    void * zprava_potomka = malloc(sizeof(int*));
                    MPI_Recv(zprava_potomka, 1, MPI_INT, process_nr, DESKRIPCNI_MESSAGE, MPI_COMM_WORLD, &status); // mozna by slo dat ANY_PROCESS, ale to by se clovek posral, ztratil by prehled. stejne nejspis budou nejdriv hotovy ty nejposlednejsi ;-)
                    //zpravu cist nepotrebujeme, neni duvod aby prislo cokoli jineho, nez ze proces/ukol skoncil.
                    free(zprava_potomka);

                    //vytvorime nove pole k poslani - musime pak killnout!
                    int * arr = new int[pocet];
                    arr[0] = cislo;
                    //posilame dalsi zadani!
                    posli_procesu_ukol(process_nr, arr, 1);
                    delete [] arr; // uklid poslaneho pole;

                    //povedlo se nam zadat dalsi job, posouvame cislo
                    cislo++;
                }


                //posuneme se na dalsi proces, a koukneme jestli uz nahodou neskoncil
                process_nr++;
                if (process_nr == mpi_count) process_nr = 1;
            }



        }

        process_nr = 1;
        MPI_Status status;
        while (true) {
            int flag = 0;
            MPI_Iprobe ( process_nr, DESKRIPCNI_MESSAGE, MPI_COMM_WORLD, &flag, &status );
            void * zprava_potomka = malloc(sizeof(int));
            if (flag) {//proces neco chce, nejspis ma hotovo - tohle by melo kontrolovat pouze hotove useky
                MPI_Recv(zprava_potomka, 1, MPI_INT, process_nr, DESKRIPCNI_MESSAGE, MPI_COMM_WORLD, &status);
                //zpravu potomka ignorujeme, on sam zadnou zpravu nevyvola, krome JOB_FINISHED

                //nicmene musime se zeptat jednotlivych procesu, jestli nahodou nemaji nejaky volny job, ktery bychom dali soucasnemu flakaci
                //otazkou je, odkud zacit. zkusme to odzadu. bude nejspise nejmene cekani

                idle = 0;

                // i je cislo zkoumaneho procesu
                for (int i = (mpi_count-1); i>0;i--)  {
                    if (i == process_nr) {
                        idle ++;
                        continue;
                    }
                    int nic = 0;
                    MPI_Send(&nic, 1, MPI_INT, i, DESKRIPCNI_MESSAGE, MPI_COMM_WORLD);
                    MPI_Recv(zprava_potomka, 1, MPI_INT, process_nr, DESKRIPCNI_MESSAGE, MPI_COMM_WORLD, &status);
                    int zprava = (*(int *)zprava_potomka);
                    if (zprava == JOB_RUNNING_AND_GIVE) {//tahle mrska nam muze dat kus vlastni prace, tak si o ni rekneme. taky by mohla dat rovnou, ale prdime na to ;-)
                        int nic = DEJ_PULKU;
                        MPI_Send(&nic, 1, MPI_INT, i, CO_DELAT, MPI_COMM_WORLD);
                        MPI_Recv(zprava_potomka, pocet, MPI_CHAR, i, CO_DELAT, MPI_COMM_WORLD, &status);
                        //tady vlastne obdrzime jen jednu vetev, ne nejaky interval. co s tim? vratit start a end?
                        MPI_Send(zprava_potomka, pocet, MPI_CHAR, process_nr, CO_DELAT, MPI_COMM_WORLD);



                        //nasli jsme darce, predali reseni, koncime s touhle srandou, at si pokracuje ten while ;-)
                        break;
                    }
                    else if (zprava == JOB_FINISHED) idle++;
                    else if (zprava == JOB_RUNNING_CANT_GIVE) {
                        //int nic = POKRACUJ;
                        //MPI_Send(&nic, 1, MPI_INT, i, CO_DELAT, MPI_COMM_WORLD);
                        /// `pokracuj` bude defaultni chovani kdyz nemuze poskytnout

                    }

                }
                if (idle == (mpi_count-1)) {
                    free(zprava_potomka);
                    break; //uz mame vsechny flakace
                }

            }



            /*
            if (flag) {//potomek neco chce, nejspis ma hotovo
                MPI_Recv(zprava_potomka, 1, MPI_INT, process_nr, DESKRIPCNI_MESSAGE, MPI_COMM_WORLD, &status);
                int zprava = (*(int *)zprava_potomka);
                switch (zprava) {
                    case JOB_FINISHED: //rutina na nalezeni nejakeho jadra, co ma moc prace.
                        if ()
                        //
                        //
                        //
                    break;
                    case JOB_RUNNING_AND_GIVE: //muze se to vlastne stat?

                    break;
                    case JOB_RUNNING_CANT_GIVE: //muze se to vlastne stat?

                    break;
                }

            }*/



            process_nr++;
            process_nr = process_nr%mpi_count;
            if (process_nr == 0) process_nr++;
            /*
            if (!flag) cout << "nic" << endl;
            else {
                printf("zkouska\n");
                fflush(stdout);
                void * nic = malloc(sizeof(int*));
                MPI_Recv(nic, 1, MPI_INT, process_nr, tag, MPI_COMM_WORLD, &status);
                cout << "mam ";
                cout << (*(int *)nic);
                cout << endl;
                if ((*(int *)nic) == 19) break;
            }
            */
            free(zprava_potomka);
        }
        //prej uz se teda vsichni flakaji -> rekneme si o nejlepsi reseni ;-)
        for (int i = 1; i<mpi_count; i++) {
            int nic = VRAT_NEJLEPSI;
            MPI_Status status;
            MPI_Send(&nic, 1, MPI_INT, i, CO_DELAT, MPI_COMM_WORLD);
            void * vysledek = malloc(sizeof(bool)*pocet);
            MPI_Recv(vysledek, pocet, MPI_CHAR, i, CO_DELAT, MPI_COMM_WORLD, &status);


            bool * vysled =  ((bool *)vysledek);
            porovnej_nejlepsi(vysled);

            free (vysledek);
        }

        /* FINALNI VYPIS VYSLEDKU */
        printf("%.12f",max_sum);
        cout  << endl;

        cout << "max_cnt " << max_cnt <<endl;

        cout << "array:" <<endl;
        for ( int q = 0; q< max_cnt; q++) {
            printf("%d",max_arr[q]);
            printf(" ");
            printf("%.12f",C[max_arr[q]]);
            printf(" ");
            printf("%.12f",O[max_arr[q]]);
            cout <<endl;
        }

    }
    else { /// podrizene procesy
        /// @TODO

        //zacneme poslouchat, az nam sef neco zada
        void * vysledek = malloc(sizeof(int));
        MPI_Status status;
        MPI_Recv(vysledek, 1, MPI_INT, i, CO_DELAT, MPI_COMM_WORLD, &status);
        int message = (*(int*) vysledek);
        if (message != POSILAM_UKOL) {
            printf("%d, neco se pokazilo - %d\n",mpi_nr, message);
            fflush(stdout);
            return 9;
        }

        //tady nacteme pocet cisel, ktera budeme pocitat.
        MPI_Recv(vysledek, 1, MPI_INT, i, CO_DELAT, MPI_COMM_WORLD, &status);
        int message = (*(int*) vysledek);
        if (message != POSILAM_UKOL) {
            printf("%d, neco se pokazilo - %d\n",mpi_nr, message);
            fflush(stdout);
            return 9;
        }

        /*
        for (int  nic = 0; (nic) < 20; (nic)++) {
            //MPI_Send(&nic, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
            //MPI_Send (param, 2, MPI_INT, source, tag1, MPI_COMM_WORLD);

            //usleep(1);
        }*/

    }



/*
    for (int f=0;f<pocet;f++) {
        int * temp_arr = new int[pocet];
        spocitej(0,  temp_arr, f , 0, 0);
        delete [] temp_arr;
    }
*/


    delete [] max_arr;

    MPI_Finalize();

    printf("%d koncim!\n", mpi_nr);
    fflush(stdout);

    return 0;
}
