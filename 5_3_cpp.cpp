#include <stdlib.h>
#include <stdio.h>
#include <queue>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
using namespace std;

/*
Operating System Concepts Edition 9th Project 5.3
Test on Debian 7.0 64 bits
Author: junzhengrice@gmail.com
Compile: g++ -lpthread [file]
*/

void* func_producer(void*);
void* func_consumer(void*);
void printLog(void);

#define LOG_NUM 5 //Max num of logs
#define PRODUCER_THREAD_NUM 5 //Num of producer threads
#define CONSUMER_THREAD_NUM 3 //Num of consumer threads
#define PRODUCE_NUM 2 //Num of logs to be written
#define CONSUME_NUM 2 //Num of logs to be removed

/***************PARAMETERS***************/
struct THREAD_PARAMETERES{
    int pid;
};
typedef struct THREAD_PARAMETERES PP;

pthread_attr_t attr;
pthread_mutex_t mutex;
sem_t sem_empty;
sem_t sem_full;

/*
The buffer is implemented as a C++ <queue>
*/
queue<int> mylog;

int main(int argc, char *argv[]){
/***************PARSING, INITIALIZING**************/    
    
    pthread_attr_init(&attr);
    pthread_mutex_init(&mutex, NULL);
    
    int res;
    res = sem_init(&sem_empty, 0, LOG_NUM);if(res==-1) printf("sem_init: sem_empty\n");
    res = sem_init(&sem_full,0,0);if(res==-1) printf("sem_init: sem_full\n");
    
    /*
    In macintosh, the named semaphore initialization syntax is like:
    sem_t* sem_empty = sem_open("/sem_empty", O_CREAT, S_IROTH | S_IWOTH, PRODUCER_THREAD_NUM);
    sem_t* sem_full  = sem_open("/sem_full",  O_CREAT, S_IROTH | S_IWOTH, 0);
    */
    
    PP* prod_param[PRODUCER_THREAD_NUM];
    for(int i=0;i<PRODUCER_THREAD_NUM;i++){
        prod_param[i] = (PP*)malloc(sizeof(PP));
        prod_param[i]->pid   = i;
    }
    
    PP* cons_param[CONSUMER_THREAD_NUM];
    for(int i=0;i<CONSUMER_THREAD_NUM;i++){
        cons_param[i] = (PP*)malloc(sizeof(PP));
        cons_param[i]->pid   = i;
    }
    
/***************THREADING**************/    
    pthread_t producer[PRODUCER_THREAD_NUM];
    pthread_t consumer[CONSUMER_THREAD_NUM];
    
    for(int i=0;i<PRODUCER_THREAD_NUM;i++)
        pthread_create(producer+i,&attr,func_producer,(void*)(prod_param[i]));
    
    for(int i=0;i<CONSUMER_THREAD_NUM;i++)
        pthread_create(consumer+i,&attr,func_consumer,(void*)(cons_param[i]));
    
    for(int i=0;i<PRODUCER_THREAD_NUM;i++) pthread_join(producer[i],NULL);
    for(int i=0;i<CONSUMER_THREAD_NUM;i++) pthread_join(consumer[i],NULL);
    
/***************CLEANING**************/
    
    for(int i=0;i<PRODUCER_THREAD_NUM;i++) {free(prod_param[i]);prod_param[i]=NULL;}
    for(int i=0;i<CONSUMER_THREAD_NUM;i++) {free(cons_param[i]);cons_param[i]=NULL;}
    
    printLog();
    exit(EXIT_SUCCESS);
}

/***************THREADS FUNCTIONS**************/
void* func_producer(void* param){
    int pid = ((PP*)param)->pid;//printf("%d\n",pid);
    
    for(int i=0;i<PRODUCE_NUM;i++){

        sem_wait(&sem_empty);
        pthread_mutex_lock(&mutex); 
        
        int cont = pid*10+i;        
        printf("PRODUCER: pid: %d, i: %d, cont: %d\n",pid,i, cont);
        mylog.push(cont);
        
        pthread_mutex_unlock(&mutex); 
        sem_post(&sem_full);
    }
    pthread_exit(0);
}

void* func_consumer(void* param){
    int pid = ((PP*)param)->pid;//printf("%d\n",pid);

    for(int i=0;i<CONSUME_NUM;i++){
                
        sem_wait(&sem_full);
        pthread_mutex_lock(&mutex); 
        
        printf("CONSUMER: pid: %d, i:%d\n", pid,i);
        mylog.pop();
        
        pthread_mutex_unlock(&mutex); 
        sem_post(&sem_empty);
    }   
    pthread_exit(0);
}

void printLog(void){
    int i=0;
    int cont = 0;
    while (!mylog.empty()){
        cont = mylog.front();
        printf("LOG %d: %d\n", i,cont);
        mylog.pop();
        i++;
    }
}