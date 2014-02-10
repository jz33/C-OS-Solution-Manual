#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

/*
Operating System Concepts Edition 9th Project 5.1
Test on Macintosh 10.8.4 POSIX named pthread, semaphore
Auther: junzhengrice@gmail.com
*/

//A custom log with precise timespec
void initLog(void);
void pushLog(char*);
void printLog(void);
struct timespec getTimespec(void);

void* func_teach(void*);
void* func_learn(void*);

#define LOG_NUM 100 //Number of logs
#define LOG_BITS 3 //The char bits of LOG_NUM
#define LOG_MAX_LEN 50 //Max chars of log content
#define LOG_CLK_LEN 12 //Max chars of log clock time (timespec)

#define TA_SLEEP_INTERVAL 3 //TA sleep time interval
#define TA_RETRY_INTERVAL 1 //TA check if there are students
#define TA_RETRY_TIMES 8 //TA retry times

#define ST_NUM 4 //Student number
#define ST_WAY_TIME 6 //Student on the way to TA time (init)
#define ST_LEARN_INTERVAL 3 //ST learn from TA time interval
#define ST_RETRY_INTERVAL 4 //ST retry reaching TA time interval
#define ST_RETRY_TIMES 5 //ST retry times

static char* log[LOG_NUM+1];//Global log
static int st_current; //Current comed student number

int main(int argc, char *argv[]){

/************INIT LOG, THREADS, SEMAPHORE, ATTRIBUTE, CONDITION**************/

    initLog();
    
    sem_t* semaphore;
    semaphore = sem_open("/semaphore", O_CREAT, 0755, 1);
    if(semaphore == SEM_FAILED){
        perror("sem_open");
        exit(EXIT_FAILURE);
    }
    
    pthread_attr_t attr;
    pthread_mutexattr_t mutexattr;
    pthread_mutex_t mutex;
    
    pthread_attr_init(&attr);
    pthread_mutexattr_init(&mutexattr);
    pthread_mutex_init(&mutex, &mutexattr);
    
    pthread_t students[ST_NUM];
    pthread_t teacher;
  
/************MAIN**************/

    pthread_create(&teacher,&attr,func_teach,(void*)(&mutex));
    for(int i=0;i<ST_NUM;i++)
        pthread_create(&students[i],&attr, func_learn,(void*)(semaphore));
      
    for(int i=0;i<ST_NUM;i++)
        pthread_join(students[i],NULL);
    pthread_join(teacher,NULL);
    
/************CLOSE, UNLINK, DESTORY, LOG**************/

    sem_close(semaphore);
    sem_unlink("/semaphore");
    
    printLog();
    exit(EXIT_SUCCESS);
    
}

/************THREADS FUNCTIONS**************/
void* func_teach(void* param){
    //pthread_mutex_t mutex = ((PARAM_TA*)param)->mutex;
    pthread_mutex_t mutex = *((pthread_mutex_t*)param);
    
    struct timespec ts = getTimespec();
    long long int t_start = (long long)ts.tv_sec;
    
    //TA initally sleep
    int i;
    if(pthread_mutex_lock(&mutex)==0){
        pushLog("TA starts her TA session by sleeping...");
    }
  
    //TA check if any student comes
    for(i=0;i<TA_RETRY_TIMES;i++){
        if(st_current >0){
            pthread_mutex_unlock(&mutex);
            pushLog("First student is always welcome!");
            break;
        }
        else sleep(TA_RETRY_INTERVAL);
    }
    
    if(i >=TA_RETRY_TIMES){
        pthread_mutex_unlock(&mutex);
        pushLog("No one comes today");
    }
    else{ //Check if students are all done
        for(i=0;i<TA_RETRY_TIMES;i++){
            if(st_current <=0){
                pushLog("TA has done her teaching");
                break;
            }
            else sleep(ST_RETRY_INTERVAL);
        }
    }
    
    ts = getTimespec();
    char buffer[LOG_MAX_LEN];
    sprintf(buffer,"TA has done her TA session, totally time is %lld",
                (long long)ts.tv_sec-t_start);
    pushLog(buffer);
    pthread_exit(0);
}

void* func_learn(void* param){
    sem_t* semaphore = (sem_t*)param;
    
    //Students are coming to TA    
    sleep(ST_WAY_TIME);
    
    struct timespec ts = getTimespec();
    long long int t_start = (long long)ts.tv_sec;
    
    //Try to enter room        
    int i;
    int sem_res = sem_wait(semaphore);
    if(sem_res == 0){
        st_current++;
        char buffer[LOG_MAX_LEN];
        sprintf(buffer,"Student %08lu has entered the TA room",
                (unsigned long)pthread_self());
        pushLog(buffer);
    }
        
    //Teaching & done teaching
    sleep(ST_LEARN_INTERVAL);
    st_current--;
    
    //Leave room
    sem_res = sem_post(semaphore);
    if(sem_res == 0){
        char buffer[LOG_MAX_LEN];
        sprintf(buffer,"Student %08lu and TA has done flirting",
                (unsigned long)pthread_self());
        pushLog(buffer);
    }    
    pthread_exit(0);
}

/************AUXILIARY FUNCTIONS**************/
void initLog(void){
    log[0] = (char*)calloc((LOG_BITS+1),sizeof(char));
    log[0][0] = '0';
    log[0][1] = '\0';
}

void pushLog(char* cont){
    int base = 10; //printf("log[0]: %s\n",log[0]);
    long int counter = strtol(log[0],NULL,base); // to long int
    if((int)counter==LOG_NUM){
       printf("Log is full of size %d\n",LOG_NUM);
       return;
    }
    else{
       counter++;
       char buffer[LOG_BITS];
       int n = sprintf(buffer,"%ld",counter);// to string
       strncpy(log[0],buffer,n);
       log[0][n] = '\0'; 
    }
    
    int len = strlen(cont);
    if(len < 1) return;
    
    //Manage timespec
    struct timespec ts = getTimespec();
    char buffer[LOG_CLK_LEN];
    int n = sprintf(buffer,"%lld:", (long long)ts.tv_sec);
    
    log[counter] = (char*)malloc((n+len+1)*sizeof(char));
    strncpy(log[counter],buffer,n);
    strncpy(log[counter]+n, cont, len);
    log[counter][n+len] = '\0';
}

void printLog(void){
    int base = 10;
    long int counter = strtol(log[0],NULL,base);
    printf("Number of logs: %ld\n",counter);
    for(int i=1;i<=counter;i++) printf("%2d:%s\n",i, log[i]);
    printf("\n");
}

struct timespec getTimespec(void){
   struct timeval tv;
   struct timespec ts;
   gettimeofday(&tv, NULL);
   ts.tv_sec = tv.tv_sec +0;
   ts.tv_nsec = 0;
   return ts;
}