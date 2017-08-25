/*
 * Soubor:      proj2.c
 * Datum:       27.04.2017
 * Autor:       David Endrych,xendry02@stud.fit.vutbr.cz, FIT
 * Projekt:     IOS 2. Úloha (2016/2017)
 * Prekladac:   gcc (Ubuntu 5.4.0-6ubuntu1~16.04.4) 5.4.0 20160609
 * Popis:       Implemetace synchronizacniho problumu Child Care
 */

#include <stdio.h>
#include "header.h"
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <time.h>

#define shmSIZE sizeof(int)
#define semSIZE sizeof(sem_t)

//Globalni promene, sdilena pamet, semafory, file descriptor
int actionID,curr_adultID,curr_childID,all_childID,all_adultID, rem_actualID;
int *action = 0, *curr_adult = 0, *curr_child = 0, *all_child = 0,*all_adult = 0, *rem_actual = 0;
sem_t *sem_finished, *sem_one;
FILE *fw;


int main(int argc, char *argv[])
{
    //Zachyceni signalu
    signal(SIGTERM, end);
    signal(SIGINT, end);
    //Ziskani argumentu
    parametrs par;
    if((getArg(argc,argv,&par)) == 1)
        return 1;
    int pid;   
    int dest = 0;
    //Nacteni souboru, nastaveni bufferu
    if((fw = fopen("proj2.out","w+")) == NULL)
        return 1;
    setbuf(fw,NULL);
    setbuf(stderr,NULL);
    //Inicializace pameti a semaforu, osetreni chyb
    int stat = init();
    if(stat == 1)
    {
        fprintf(stderr,"Nepodarilo se inicializovat sdilenou pamet\n");
        dest = destruct();
        if(dest == 1)
            fprintf(stderr,"Nepodarilo se uvolnit sdilenou pamet\n");
        else if(dest == 2)
            fprintf(stderr,"Nepodarilo se uvolnit semafory\n");
        return 2;
    }
    else if(stat == 2)
    {
        fprintf(stderr,"Nepodarilo se inicializovat semafory\n");
        destruct();
        return 2;
    }
    //Pole pidu pro cekani na procesy
    pid_t adultPID[par.A];
    pid_t childPID[par.C];
    *rem_actual = par.A + par.C;

    //Fork procesu a testovani uspechu
    if((pid=fork())<0)
    {
        fprintf(stderr,"Nepodarilo se vytvorit proces\n");
        dest = destruct();
        if(dest == 1)
            fprintf(stderr,"Nepodarilo se uvolnit sdilenou pamet\n");
        else if(dest == 2)
            fprintf(stderr,"Nepodarilo se uvolnit semafory\n");
        exit(2);
    }
    //Adult vetev
    if(pid == 0)
    {
        for(int i =0; i < par.A;i++)
        {
            //Uspani procesu
            srand(time(NULL));
            usleep((rand()%(par.AGT+1))*1000);
            //ulozeni pidu do pomocneho pole
            adultPID[i] = pid;
            //Forknuti procesu a test uspechu
            if((pid=fork())<0)
            {
                fprintf(stderr,"Nepodarilo se vytvorit proces\n");
                dest = destruct();
                if(dest == 1)
                    fprintf(stderr,"Nepodarilo se uvolnit sdilenou pamet\n");
                else if(dest == 2)
                    fprintf(stderr,"Nepodarilo se uvolnit semafory\n");
                exit(2);
            }
            if(pid == 0)
            {
                adult(&par);
            }
        }
    }
    //Child vetev
    else
    {
        for(int i=0;i<par.C;i++)
        {
            // Uspani procesuAno, je to tak. Jedine v pripade, ze odejde posledni adult a dalsi se jiz negeneruji, tak vstoupi do centra vsechny procesy child bez omezeni.

            srand(time(NULL));
            usleep((rand()%(par.CGT+1)*1000));
            //Ulozeni pidu do pomocneho pole
            childPID[i] = pid;
            //Fork procesu a testovani uspechu
            if((pid=fork())<0)
            {
                fprintf(stderr,"Nepodarilo se vytvorit proces\n");
                dest = destruct();
                if(dest == 1)
                    fprintf(stderr,"Nepodarilo se uvolnit sdilenou pamet\n");
                else if(dest == 2)
                    fprintf(stderr,"Nepodarilo se uvolnit semafory\n");
                exit(2);
            }
            if(pid == 0)
            {
                child(&par);
            }
        }
    }
    //Cekani na procesy
    for(int i = 0;i< par.A;i++)
        waitpid(adultPID[i],NULL,0);
    for(int i = 0;i< par.C;i++)
        waitpid(childPID[i],NULL,0);
    //Uvolnovani pameti
    dest = destruct();
    if(dest == 1)
        fprintf(stderr,"Nepodarilo se uvolnit sdilenou pamet\n");
    else if(dest == 2)
        fprintf(stderr,"Nepodarilo se uvolnit semafory\n");
    return 0;
}

/**
 * Simulace chovani adult procesu 
 * @param *par Ukazatel na strukturu obsahujici argumenty
**/
void adult(parametrs *par)
{
    int waiting = 1; // Promena, ktera zajisti pouze jedno vypsani cekani 
    //Start procesu
    sem_wait(sem_one);
    *action += 1;
    *all_adult += 1;
    int this_adult = *all_adult;
    fprintf(fw,"%d\t: A %d    : started\n",*action,this_adult);
    fflush(fw); 
    sem_post(sem_one);
    //Vstup procesu
    sem_wait(sem_one);
    *action +=1;
    *curr_adult += 1;
    fprintf(fw,"%d\t: A %d    : enter\n",*action,this_adult);
    fflush(fw);
    //Uspani procesu
    srand(time(NULL));
    int sleep = (rand()%(par->AWT+1))*1000;
    sem_post(sem_one);
    usleep(sleep);
    //Proces se pokusi opustit centrum
    sem_wait(sem_one);
    *action +=1;
    fprintf(fw,"%d\t: A %d    : trying to leave\n",*action,this_adult);
    fflush(fw);
    //Cekani na opusteni dokud neplati podminka
    while(1)
    {
         if(!waiting)
            sem_wait(sem_one);
         if(*curr_child <= (*curr_adult - 1) * 3)
         {
             *curr_adult -= 1;
             *action += 1;
             fprintf(fw,"%d\t: A %d    : leave\n",*action,this_adult);
             fflush(fw);
             break;
         }
         else
         {
             if(waiting)
             {
                *action += 1;
                fprintf(fw,"%d\t: A %d     : waiting : %d : %d\n",*action,this_adult,*curr_adult,*curr_child);
                fflush(fw);
             }
             waiting = 0;
         }
         
         sem_post(sem_one);
    }
    // Ubrani procesu a test jestli uz nejsou neodesli vsechny
    *rem_actual -=1;
    if(*rem_actual <= 0)
        sem_post(sem_finished);
    sem_post(sem_one);
    //Ukonceni procesu
    sem_wait(sem_finished);
    *action +=1;
    fprintf(fw, "%d\t: A %d    : finished\n",*action,this_adult);
    fflush(fw);
    sem_post(sem_finished);
    exit(0);
}

/**
 * Funkce simuluje proces child
 * @param *par Ukazatel na strukturu obsahujici argumenty
**/
void child(parametrs *par)
{
    int waiting = 1; // Promena, ktera zajisti pouze jedno vypsani cekani 
    //Start procesu
    sem_wait(sem_one);
    *action += 1;
    *all_child += 1;
    int this_child = *all_child;
    fprintf(fw,"%d\t: C %d    : started\n",*action,this_child);
    fflush(fw);
    sem_post(sem_one);
    // Cekani dokud neni splena podminka pro vstup
    while(1)
    { 
         sem_wait(sem_one);
         if(!(*curr_child +1 > *curr_adult * 3) || (*all_adult == par->A && *curr_adult == 0))
         {
             *curr_child += 1;
             *action += 1;
             fprintf(fw,"%d\t: C %d    : enter\n",*action,this_child);
             fflush(fw);
             sem_post(sem_one);
             break;
         }
         else
         {
             if(waiting)
             {
                *action += 1;
                fprintf(fw,"%d\t: C %d     : waiting : %d : %d\n",*action,this_child,*curr_adult,*curr_child);
                fflush(fw);
             }
             waiting = 0;
         }
         
         sem_post(sem_one);
    }
    sem_wait(sem_one);
    //Uspani procesu
    srand(time(NULL));
    int sleep = (rand()%(par->CWT+1))*1000;
    sem_post(sem_one);
    usleep(sleep);
    // Opusteni centra
    sem_wait(sem_one);
    *action +=1;
    fprintf(fw,"%d\t: C %d    : trying to leave\n",*action,this_child);
    fflush(fw);
    // Ubrani procesu a test jestli uz nejsou neodesli vsechny
    *rem_actual -= 1;
    if(*rem_actual <= 0)
        sem_post(sem_finished);
    *action += 1;
    *curr_child -= 1;
    fprintf(fw,"%d\t: C %d    : leave\n",*action,this_child);    
    sem_post(sem_one);
    //Ukonceni procesu
    sem_wait(sem_finished);
    *action +=1;
    fprintf(fw, "%d\t: C %d   : finished\n",*action,this_child);
    fflush(fw);
    sem_post(sem_finished);
    exit(0);
}  

/**
 * Funkce inicializuje sdilenou pamet a semafory
 * @return 1 V pripade problemu se sdilenou pameti
 * @return 2 V pripade problemu se semafory
 * @return 0 Pokud je vse v poradku
**/
int init()
{
    // Sdilena pamet
    // action je pořadové číslo prováděné akce
    // curr_adult aktualni pocet adultu v centru
    // curr_child aktualni pocet childu v centru
    // all_child celkovy pocet vygenerovanych childu
    // all_adult celkovy pocet vygenerovanych adultu
    // rem_actual urcuje kolik procesu ma jeste opustit centrum

    if((actionID = shmget(IPC_PRIVATE, shmSIZE, IPC_CREAT | IPC_EXCL| 0666)) == -1)
        return 1;
    if((action = (int*)shmat(actionID, NULL, 0)) == (void *) -1)
        return 1;
    if((curr_adultID = shmget(IPC_PRIVATE, shmSIZE, IPC_CREAT | IPC_EXCL| 0666)) == -1)
        return 1;
    if((curr_adult = (int*)shmat(curr_adultID, NULL, 0)) == (void *) -1)
        return 1;
    if((curr_childID = shmget(IPC_PRIVATE, shmSIZE, IPC_CREAT | IPC_EXCL| 0666)) == -1)
        return 1;
    if((curr_child = (int*)shmat(curr_childID, NULL, 0)) == (void *) -1)
        return 1;
    if((all_childID = shmget(IPC_PRIVATE, shmSIZE, IPC_CREAT | IPC_EXCL| 0666)) == -1)
        return 1;
    if((all_child = (int*)shmat(all_childID, NULL, 0)) == (void *) -1)
        return 1;
    if((all_adultID = shmget(IPC_PRIVATE, shmSIZE, IPC_CREAT | IPC_EXCL| 0666)) == -1)
        return 1;
    if((all_adult = (int*)shmat(all_adultID, NULL, 0)) == (void *) -1)
        return 1;
    if((rem_actualID = shmget(IPC_PRIVATE, shmSIZE, IPC_CREAT | IPC_EXCL| 0666)) == -1)
        return 1;
    if((rem_actual = (int*)shmat(rem_actualID, NULL, 0)) == (void *) -1)
        return 1;

    //Semafory
    // sem_one zapouzdruje prikazy
    // sem_finished urcuje jestli se muzou procesy ukoncovat
    if((sem_finished = mmap(NULL, semSIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED)
        return 2;
    if((sem_init(sem_finished, 1, 0)) == -1)
        return 2;
    if((sem_one = mmap(NULL, semSIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED)
        return 2;
    if((sem_init(sem_one, 1, 1 )) == -1)
        return 2;
    return 0;
}

/**
 * Funkce uvolnuje pamet, rusi semafory apod.
 * @return 1 V pripade problemu se sdilenout pameti
 * @return 2 V pripade problemu se semafory
 * @return 0 Pokud je vse v poradku
**/
int destruct()
{
    // Sdilena pamet
    if((shmctl(actionID,IPC_RMID,NULL)) == -1)
        return 1;
    if((shmctl(curr_adultID,IPC_RMID,NULL)) == -1)
        return 1;
    if((shmctl(curr_childID,IPC_RMID,NULL)) == -1)
        return 1;
    if((shmctl(all_childID,IPC_RMID,NULL)) == -1)
        return 1;
    if((shmctl(all_adultID,IPC_RMID,NULL)) == -1)
        return 1;
    if((shmctl(rem_actualID,IPC_RMID,NULL)) == -1)
        return 1;
    
    // Semafory
    if((sem_destroy(sem_finished)) == -1)
        return 2;
    if((munmap(sem_finished, semSIZE)) == -1)
        return 2;
    if((sem_destroy(sem_one)) == -1)
        return 2;
    if((munmap(sem_one,semSIZE)) == -1)
        return 2;

    fclose(fw);
    return 0;
}

/**
 * Funkce nacita argumenty
 * @param argc Pocet argumentu
 * @param *argv[] Jednotlive argumenty v poli
 * @param *par Ukazatel na strukturu pro parametry
 * @return 1 V Pripade chyby
 * @return 0 Pokud se argumenty nactou v poradku 
**/
int getArg(int argc,char *argv[],parametrs *par)
{
    char *endptr;

    if(argc == 7)
    {
        par->A = (int) strtol(argv[1],&endptr,0);
        if(*endptr != '\0' && par->A <= 0)
        {
            fprintf(stderr,"Spatny argument A\n");
            return 1;
        }
        par->C = (int) strtol(argv[2],&endptr,0);
        if(*endptr != '\0' && par->C <= 0)
        {
            fprintf(stderr,"Spatny argument C\n");
            return 1;
        }
        par->AGT = (int) strtol(argv[3],&endptr,0);
        if(*endptr != '\0' && par->AGT < 0 && par->AGT >= MAXTIME)
        {
            fprintf(stderr,"Spatny argument AGT\n");
            return 1;
        }
        par->CGT = (int) strtol(argv[4],&endptr,0);
        if(*endptr != '\0' && par->CGT < 0 && par->CGT >= MAXTIME)
        {
            fprintf(stderr,"Spatny argument CGT\n");
            return 1;
        }
        par->AWT = (int) strtol(argv[5],&endptr,0);
        if(*endptr != '\0' && par->AWT < 0 && par->AWT >= MAXTIME)
        {
            fprintf(stderr,"Spatny argument AWT\n");
            return 1;
        }
        par->CWT = (int) strtol(argv[6],&endptr,0);
        if(*endptr != '\0' && par->CWT < 0 && par->CWT >= MAXTIME)
        {
            fprintf(stderr,"Spatny argument CWT\n");
            return 1;
        }
    }
    else
    {
        fprintf(stderr,"Spatne argumenty");
        return 1;
    }
    return 0;
}
/**
 * Funkce uvolnuje zdroje a ukoncuje procesy, volana signaly
**/
void end()
{
    destruct();
    kill(getppid(),SIGTERM);
    exit(2);
}
