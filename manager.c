/*
 ============================================================================
 Name        : manager.c
 Author      : Tye Borden A00310101
 Version     :1
 ============================================================================
This purpose of this project is to create a train junction with four
entry/exit points. These are seen North, West,South, and East (N,W,S,E). The
direction of where the train is from is read in from a textfile (sequence.txt)
. Each train will be then spawned using the fork process. Then for the process
to complete the train must cross the junction. This will done by aquiring 3
locks. One for its own direction its comming from, second for direction to its
right, and finally a junction lock. Once all all obtained it may cross the
junction safely. A matrix (matrix.txt) is kept to keep the status all trains
that are spawned and the locks (N,W,S,E) they have or are requesting
(2 = have, 1 = request, 0 = do not have). If deadlock  is found in the matrix,
 then then program will stop, print the cycle causing the deadlock, and kill
 the remaining processes.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <ctype.h>
#include <unistd.h>

#define  DIRSEMS 4 //Number of locks
#define  NSEM "/semN" //Name of North semaphore
#define  WSEM "/semW" //Name of West semaphore
#define  SSEM "/semS" //Name of South semaphore
#define  ESEM "/semE" //Name of East semaphore
#define  JSEM "/semJuction"//Name of Junction semaphore
#define  MSEM "/semMatrix" //Name of Matrix semaphore

char txtSeq[1000]; //Buffer to read in sequence.txt
int  **txtMatrix;
int *northArr,*westArr,*southArr, *eastArr; //Arrays of each lock
int countSeq; //Number of trains
sem_t *northLock, *southLock,*eastLock,*westLock, *junctLock, *mxtLock;

/*
This function is used to simulate crocssing a junction.
@param void
*/
void crossTheJunction()
{
        sleep(2);
}
/*
This function is used to display the matrix on screen
@param void
*/
void showmatrix()
{   printf("   N  W  S  E\n");
    for(int i = 0;i < countSeq;i++)
    {
        for(int j =0;j<DIRSEMS;j++)
        {
          if( j == 0)
          {
            printf("%cT ",txtSeq[i]);
          }
          printf("%d  ",txtMatrix[i][j]);

        }
        printf("\n");
    }
    printf("\n");
}
/*
This function writes the txtMatrix to the matrix.txt file.
@param
*/
void writeMatrix()
{

        FILE * f = fopen("matrix.txt","w");

        for(int row = 0; row < countSeq; row++)
        {
                for(int col = 0; col < DIRSEMS; col++)
                {
                        fprintf(f,"%d ",txtMatrix[row][col]);
                }
                fprintf(f,"\n");
        }
        fclose(f);
}

/*
This function reads the data from the matrix.txt and updates the matrix in
a given process.
@param void
*/
void readMatrix()
{

        FILE * f = fopen("matrix.txt","r");
        char buf[10],*token;
        for(int i =0; i<countSeq; i++)
        {
                fgets(buf,sizeof(buf),f);
                token = strtok(buf," ");
                for(int j =0; j<DIRSEMS; j++)
                {
                        txtMatrix[i][j] = atoi(token);
                        token = strtok(NULL," ");
                }
        }
        fclose(f);
}
/*
This function is used to update both the process matrix and the matrix.txt
of any lock changes that happen by a given train while locking other process
from updating it until its finished.
@param int m the lock being changed
@param int n the train
@ param int value the lock status
*/
void updateMxt(int m, int n, int value)
{
        mxtLock = sem_open(MSEM,0);
        // sleep(1);
        sem_wait(mxtLock);
        readMatrix();
        txtMatrix[n][m] = value;
        writeMatrix();
        sem_post(mxtLock);

}
/*
This function checks to see if there are any cycles that may cause a deadLock
@param void
@return 1  or 0, 1 = yes , 0
*/
int checkDeadLock()
{
        mxtLock = sem_open(MSEM,0); // Make sure the matrix isn't updated
        int checkFor2 = 0, checkFor1 = 0;
        int result = 0;

        sem_wait(mxtLock);
        readMatrix();
        //Store all current lock date in own array
        for(int i = 0; i < countSeq; i++)
        {
                northArr[i] = txtMatrix[i][0];
                westArr[i] = txtMatrix[i][1];
                southArr[i] = txtMatrix[i][2];
                eastArr[i] = txtMatrix[i][3];
        }
        //Check for the cycle
        for(int i =0; i < countSeq; i++)
        {
                if(northArr[i] == 2 && westArr[i] != 2 )
                {
                        checkFor2++;
                }
                if(northArr[i] == 1)
                {
                        checkFor1++;
                }
                if(westArr[i] == 2 && southArr[i] != 2)
                {
                        checkFor2++;
                }
                if(westArr[i] == 1)
                {
                        checkFor1++;
                }
                if(southArr[i] == 2 && eastArr[i] != 2)
                {
                        checkFor2++;
                }
                if(southArr[i] == 1)
                {
                        checkFor1++;
                }
                if(eastArr[i] == 2 && northArr[i] != 2)
                {
                        checkFor2++;
                }
                if(eastArr[i] == 1)
                {
                        checkFor1++;
                }
        }
        if(checkFor2 == 4 && checkFor1 >= 4) result = 1;
        sem_post(mxtLock);
        return result;

}
/*
This function is to act as the train arriving and trying to cross the junction
@param char direc is the direction the trains comming from
@param int mxtNum is the trains corrisponding row in the matrix
*/
void train(char direc, int mxtNum){
        northLock = sem_open(NSEM,0);
        westLock = sem_open(WSEM,0);
        southLock = sem_open(SSEM,0);
        eastLock = sem_open(ESEM,0);

        junctLock = sem_open(JSEM,0);

        switch (direc) {
        case 'N':

                printf("N Train <%d>: North train started\n",getpid());

                printf("N Train <%d>: requests for North-Lock\n",getpid());
                updateMxt(0,mxtNum,1);
                sem_wait(northLock);
                printf("N Train <%d>: Acquires North-Lock\n",getpid());
                updateMxt(0,mxtNum,2);

                printf("N Train <%d>: requests for West-Lock\n",getpid());
                updateMxt(1,mxtNum,1);
                sem_wait(westLock);
                printf("N Train <%d>: Acquires West-Lock\n",getpid());
                updateMxt(1,mxtNum,2);

                printf("N Train <%d>: requests for Junction-Lock\n",getpid());
                sem_wait(junctLock);
                printf("N Train <%d>: Acquires Junction-Lock Passes through "
                       "the jucntion\n",getpid());

                crossTheJunction();

                printf("N Train <%d>: Releases Junction-Lock\n",getpid());
                sem_post(junctLock);

                printf("N Train <%d>: Releases West-Lock\n",getpid());
                updateMxt(1,mxtNum,0);
                sem_post(westLock);

                printf("N Train <%d>: Releases North-Lock\n",getpid());
                updateMxt(0,mxtNum,0);
                sem_post(northLock);
                break;

        case 'W':

                printf("W Train <%d>: West train started\n",getpid());

                printf("W Train <%d>: requests for West-Lock\n",getpid());
                updateMxt(1,mxtNum,1);
                sem_wait(westLock);
                printf("W Train <%d>: Acquires West-Lock\n",getpid());
                updateMxt(1,mxtNum,2);

                printf("W Train <%d>: requests for South-Lock\n",getpid());
                updateMxt(2,mxtNum,1);
                sem_wait(southLock);
                printf("W Train <%d>: Acquires South-Lock\n",getpid());
                updateMxt(2,mxtNum,2);

                printf("W Train <%d>: requests for Junction-Lock\n",getpid());
                sem_wait(junctLock);
                printf("W Train <%d>: Acquires Junction-Lock Passes through "
                       "the jucntion\n",getpid());

                crossTheJunction();

                printf("W Train <%d>: Releases Junction-Lock\n",getpid());
                sem_post(junctLock);

                printf("W Train <%d>: Releases South-Lock\n",getpid());
                updateMxt(2,mxtNum,0);
                sem_post(southLock);

                printf("W Train <%d>: Releases West-Lock\n",getpid());
                updateMxt(1,mxtNum,0);
                sem_post(westLock);
                break;

        case 'S':

                printf("S Train <%d>: South train started\n",getpid());

                printf("S Train <%d>: requests for South-Lock\n",getpid());
                updateMxt(2,mxtNum,1);
                sem_wait(southLock);
                printf("S Train <%d>: Acquires South-Lock\n",getpid());
                updateMxt(2,mxtNum,2);

                printf("S Train <%d>: requests for East-Lock\n",getpid());
                updateMxt(3,mxtNum,1);
                sem_wait(eastLock);
                printf("S Train <%d>: Acquires East-Lock\n",getpid());
                updateMxt(3,mxtNum,2);

                printf("S Train <%d>: requests for Junction-Lock\n",getpid());
                sem_wait(junctLock);
                printf("S Train <%d>: Acquires Junction-Lock Passes through "
                       "the jucntion\n",getpid());

                crossTheJunction();

                printf("S Train <%d>: Releases Junction-Lock\n",getpid());
                sem_post(junctLock);

                printf("S Train <%d>: Releases East-Lock\n",getpid());
                updateMxt(3,mxtNum,0);
                sem_post(eastLock);

                printf("S Train <%d>: Releases South-Lock\n",getpid());
                updateMxt(2,mxtNum,0);
                sem_post(southLock);
                break;

        case 'E':

                printf("E Train <%d>: East train started\n",getpid());

                printf("E Train <%d>: requests for East-Lock\n",getpid());
                updateMxt(3,mxtNum,1);
                sem_wait(eastLock);
                printf("E Train <%d>: Acquires East-Lock\n",getpid());
                updateMxt(3,mxtNum,2);

                printf("E Train <%d>: requests for North-Lock\n",getpid());
                updateMxt(0,mxtNum,1);
                sem_wait(northLock);
                printf("E Train <%d>: Acquires North-Lock\n",getpid());
                updateMxt(0,mxtNum,2);

                printf("E Train <%d>: requests for Junction-Lock\n",getpid());
                sem_wait(junctLock);
                printf("E Train <%d>: Acquires Junction-Lock Passes through "
                       "the jucntion\n",getpid());

                crossTheJunction();

                printf("E Train <%d>: Releases Junction-Lock\n",getpid());
                sem_post(junctLock);

                printf("E Train <%d>: Releases North-Lock\n",getpid());
                updateMxt(0,mxtNum,0);
                sem_post(northLock);

                printf("E Train <%d>: Releases East-Lock\n",getpid());
                updateMxt(3,mxtNum,0);
                sem_post(eastLock);
                break;
        }
}
/*
This function reads in the sequence from sequence.txt and stores it in txtSeq
@param void
*/
void readSeq(){
        FILE *file;
        countSeq =0;
        file = fopen("sequence.txt", "r");
        int stop =0;

        if(file != NULL)
        {
                fgets(txtSeq, sizeof(txtSeq),file);
                while(stop != 1)
                {
                        if (!isalpha(txtSeq[countSeq++]))
                        {
                                stop = 1;
                        }
                }
        }

        countSeq = countSeq -1;

        txtMatrix = (int **)malloc(countSeq * sizeof(int *));
        for(int i = 0; i< countSeq; i++)
        {
                txtMatrix[i] = (int *)malloc(DIRSEMS * sizeof(int));
        }
}
/*
This function is used to zero out the matrix intially
@param void
*/
void zeroMatrix()
{
        for(int row = 0; row < countSeq; row++)
        {
                for(int col = 0; col < DIRSEMS; col++)
                {
                        txtMatrix[row][col] = 0;
                }
        }
}



/*
Main function that runs the program
@param int agrc number of cmd parameters
@param char const *argv[] cmd arrguments
@return 0 when program completes
*/
int main(int argc, char const *argv[]) {

        //Unlink any previous named semaphores that have the same name
        sem_unlink(NSEM);
        sem_unlink(WSEM);
        sem_unlink(SSEM);
        sem_unlink(ESEM);
        sem_unlink(JSEM);
        sem_unlink(MSEM);

        //Intiallize all the named semaphores
        northLock = sem_open(NSEM, O_CREAT, 0644, 1);
        westLock = sem_open(WSEM, O_CREAT, 0644, 1);
        southLock = sem_open(SSEM, O_CREAT, 0644, 1);
        eastLock = sem_open(ESEM, O_CREAT, 0644, 1);
        mxtLock = sem_open(MSEM, O_CREAT, 0644, 1);
        junctLock = sem_open(JSEM, O_CREAT, 0644, 1);

        readSeq();

        //Intiallize all the lock arrays
        northArr =  (int *) malloc(countSeq * sizeof(int));
        westArr =  (int *) malloc(countSeq * sizeof(int));
        southArr =  (int *) malloc(countSeq * sizeof(int));
        eastArr =  (int *) malloc(countSeq * sizeof(int));

        zeroMatrix();
        writeMatrix();
        // printf("parrent %d\n",getpid()); **Uncommnet to check parrent pid

        //Read in and convert the commandline  deadlock probabilty p
        double probs = atof(argv[1]);

        //Instantiate all child processes
        pid_t trains[countSeq],dlock;
        int i = 0;
        double ran;
        int deadLock = 0; //Deadlock checking value
        while(countSeq > i)//Spawn children
        {
                ran = ((double)(rand() % 101))/100.00; // See child spawns
                if(ran > (1.00 - probs)) {
                        if((trains[i] = fork()) == 0)
                        {
                                train(txtSeq[i],i);
                                exit(1);
                        }
                        i++;
                }
                if(ran > probs) // See if deadlock check is done
                {
                        deadLock = checkDeadLock();
                        if (deadLock == 1) {
                                break;
                        }
                }
        }

        //If no deadlock occures while spawning come here to keep checking
        if(deadLock != 1 && (dlock = fork()) == 0)
        {
                int value = 0;
                while(value != 1)
                {
                        sleep(1);
                        value = checkDeadLock();
                        if(value == 1 ) {
                                deadLock = value;
                        }
                }
        }

        // If deadlock occures come here and print the cycle and kill program
        if(deadLock == 1)
        {
                int n=0,s=0,e=0,w=0;
                printf("\n\n-----SYSTEM DEADLOCKED-----\n\n");
                for(int i = 0; i <countSeq; i++)
                {
                        if(northArr[i] == 2)
                        {
                                n = i;
                        }
                        if(westArr[i] == 2)
                        {
                                w = i;
                        }
                        if(southArr[i] == 2)
                        {
                                s=i;
                        }
                        if(eastArr[i] == 2)
                        {
                                e =i;
                        }
                }

                printf("Train <%d> from the North is waiting"
                       "for Train <%d> from the West ---->\n"
                       ,trains[n],trains[w]);
                printf("Train <%d> from the West is waiting"
                       "for Train <%d> from the South ---->\n"
                       ,trains[w],trains[s]);
                printf("Train <%d> from the South is waiting"
                       "for Train <%d> from the East ---->\n"
                       ,trains[s],trains[e]);
                printf("Train <%d> from the East is waiting"
                       "for Train <%d> from the North ---->\n\n"
                       ,trains[e],trains[n]);
                    showmatrix();

                kill(getppid(),SIGKILL);
        }


        // If no cycle occures let all process finish then exit the program
        int processNum = countSeq;
        pid_t pid;
        while(processNum > 0)
        {
                pid = wait(NULL);
                --processNum;
        }
        kill(dlock,SIGKILL);



        sem_unlink(NSEM);
        sem_unlink(WSEM);
        sem_unlink(SSEM);
        sem_unlink(ESEM);
        sem_unlink(JSEM);
        sem_unlink(MSEM);


        free(txtMatrix);
        free(northArr);
        free(westArr);
        free(southArr);
        free(eastArr);

        return 0;
}
