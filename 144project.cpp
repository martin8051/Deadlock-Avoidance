
//Name: Martin Pantoja-Saldana
//Class: CSCI 144
//Deadlock Avoidance Project

#include <iostream>
#include <chrono>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <string>
#include <vector>
#include "unistd.h"
#include <stdlib.h>
#include <time.h>
#include <sstream>

using namespace std;

// GLOBAL VARIABLES 
condition_variable cv;              //used to create lock
mutex muxx;                         // used for lock
vector <int> capacity(10,10);       //what is the total amount of a particular resource
vector <int> availible;             //how much of each resource is currently availible
vector <vector <int>> maxx;         // what is the thread's (max/total goal) amount of a certain resource nessesary to fufill their task
vector <vector <int>> allocation;   // what threads hold what amount of resources
vector <vector <int>> request;      //what is the request of a thread to fufill their task (maxx - allocation)
vector <thread> threads;            //holds threads to run
vector <int> timescompleted;        //used to keep track of how many times a thread has completed

int numOfThreads, numOfResources;

void threadFunc(int tid);                           //used to repeatedly make a thread run, process and then restart
void Requestfunc(int resourceID, int threadID);     //requests a certain resource to a certain thread
bool wouldBeSafe(int resourceID, int threadID);     //hypothetically gives resource to a thread and runs a safe test
bool isSafe();                                      //checks to see if the current state of program will reach deadlock or not
void displaymatrix(vector<vector <int> > matrix);

int main(int argc, char *argv[])
{
    stringstream conv1,conv2;                           //get amount of threads and resources though command line
    conv1 << argv[1];
    conv1 >> numOfThreads;

    conv2 << argv[2];
    conv2 >> numOfResources;

    cout <<"Number of Threads : "<<numOfThreads << endl;
    cout <<"Number of Resources : "<<numOfResources << endl;   
     
    // srand(time(NULL));                          //randomize seed for random number generators
    srand(69);
    for(int x = 0; x < numOfThreads; x++)       //initilize all vector sizes according to num of threads and resources
    {
        vector<int> mRow;
        vector<int> aRow;
        vector<int> rRow;
        for(int i = 0; i <numOfResources;i++)   //initilize 2D vectors (maxx, allocation, request)
        {
            mRow.push_back(0) ;
            aRow.push_back(0);
            rRow.push_back(0);
        }
        maxx.push_back(mRow);
        allocation.push_back(aRow);
        request.push_back(rRow);

        timescompleted.push_back(0);        //initilize times completed (starts at 0 for all)
    }

    for(int w = 0; w < numOfResources; w++)
    {
        availible.push_back(capacity[w]);       //initilize vector availible (currently all resources are availible)

    }
    for(int tid = 0; tid < numOfThreads; tid++) // Create threads and run thread functions
    {  
        printf("thread number: %i has been created\n",tid);
        threads.push_back(thread(threadFunc,tid));
        sleep(1);
    }

    system("pause");
    return 0;
}

void threadFunc(int tid)         //used to repeatedly make a thread run, process and then restart
{
    int randtime;
    int nloops;
    while(true)                 //thead runs forever until tester chooses to terminate program
    {
         for(int j = 0; j < numOfResources; j++)        //generating random Max (total goal) and request (max-allocation) for thread
        {
            maxx[tid][j] = rand() % 7 + 3;
            request[tid][j] = maxx[tid][j];
        }
        printf("thread: %i is requesting all resources\n",tid);
        displaymatrix(request);

        for(int j = 0; j < numOfResources ; j++)        //request all of the recources needed for this thread
        {
            nloops = request[tid][j];
            for(int k = 0; k <nloops; k++)     // loop nloops times where nloops is amount of each specific resource being requested
            {
                Requestfunc(j,tid);                     //request one piece of resource
            }
        }
        printf("thread: %i has been granted all resources\n",tid);
        displaymatrix(allocation);
        timescompleted[tid]++;
        
        for(int j = 0; j < numOfResources; j++)         //thread's request has been fufilled, return resources to availible and reset it's allocation to 0
        {
            availible[j] += allocation[tid][j]; 
            allocation[tid][j] = 0;
        }

        printf("thread: %i has now finished a total of %d times, releasing and restarting thread\n",tid,timescompleted[tid]);
        displaymatrix(allocation);

        randtime = rand() % 3 + 2;                      // relase lock, sleep for randtime amount (2-4 seconds), and then repeat loop
        cv.notify_all();                                // works with sleeping for 1-2 seconds, but too fast for proper output to be displayed 
        sleep(randtime); 
        
    }
}

void Requestfunc(int resourceID, int threadID)   //requests a certain resource to a certain thread
{
    unique_lock<mutex>aLock(muxx);               // locking function to current thread, so only one thread can request at at time

    while(!wouldBeSafe(resourceID, threadID))    // check if granting request is safe, if not safe, then keep wait and release lock
    {                                            // note: when awoken it will check again 
        cv.wait(aLock);
    }
    
    request[threadID][resourceID]--;            // now that it would be safe, grant request and notify all other threads (gives up lock)
    allocation[threadID][resourceID]++;
    availible[resourceID]--;
    cv.notify_all();
}

bool wouldBeSafe(int resourceID, int threadID) //hypothetically grant request to see if result would be safe, returns true or false
{
    bool result = false;

    availible[resourceID]--;                   //granting request here for testing purposes
    allocation[threadID][resourceID]++;
    request[threadID][resourceID]--;

    if(isSafe())                               //test if current state will reach deadlock, if not then return true (safe)
    {
        request[threadID][resourceID]++;       // undoing the grant request, since this was only for testing
        availible[resourceID]++;
        allocation[threadID][resourceID]--;

        return true;
    }

    request[threadID][resourceID]++;           // current stae will reach deadlock, undoing the grant and returning false
    availible[resourceID]++;
    allocation[threadID][resourceID]--;
    return result; 
}


bool isSafe()       //checks to see if the current state of program will reach deadlock or not. returns true or false
{
    vector <int> toBeAvailble = availible;                  //make a copy of availible
    vector <bool> finish(numOfThreads,false);               //used to test if all the threads have finished, default set to all false
    int foundId = -1;                                       //flag set to -1, default value.
    vector< vector<int> > need = request;                   //make a copy of request

    while(true)
    {
        foundId = -1;                                       //reset not found flag
        for(int j = 0; j < numOfThreads; j++)               // check each thread to see if their needs (requests) are possible to grant from availible
        {
            for(int w = 0; w <numOfResources; w++)          
            {
                if(need[j][w] <= toBeAvailble[w] && finish[j]==0)   //if the thread hasn't finished, and current resource is possible 
                {
                    foundId = j;                                    // set flag to current threadID
                } else          // this thread's current resource need isn't possible, stop checking this thread (break) and reset flag to default -1
                  {
                    foundId = -1;
                    break;
                  }
            }

            if(foundId != -1) //after checking each thread, check to see if flag has default value, if not -> a thread whos possible needs has been found, break
            {
                break;
            }
        }
            if(foundId == -1) //at this point, it's searching all possible threads, if a thread wasn't found check to see if maybe all the threads are finished
            {
                for(int x = 0; x <numOfThreads; x++)
                {
                    if(finish[x] == 0)          //if any thread isn't finished, then that means deadlock would occur. Return false
                    {
                        return false; 
                    }
                }
                return true;                    //all threads finished,no deadlock will occur. return true
             } else //  a Thread who isn't finished, and whos needs are possible has been found
                {
                    finish[foundId] = 1;        // set thread to finished
                    for(int x = 0; x <numOfResources; x++)      //assumes thread would get resources and complete tasks so give up resources to tobeavailible
                    {
                        toBeAvailble[x] = toBeAvailble[x] + allocation[foundId][x]; 
                    }
                    foundId = -1;               //reset flag to default value -1
                }
    }
    cout <<"somethings wrong!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<endl; // if program reaches here, an error has occured
    return false;
}

void displaymatrix(vector<vector <int> > matrix)    //used to display 2D vectors, note doesn't display 0's to make tables easier to read
{
    for(int x = 0; x < numOfThreads; x++)
    {
        for(int w = 0; w < numOfResources; w++)
        {
           if(matrix[x][w] != 0)
           {
                printf("[ %i ] ",matrix[x][w]);
           }else
            {
                printf("[   ] ");
            }
        }
        printf("\n");
    }
}
