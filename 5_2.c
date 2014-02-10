#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

/*
Operating System Concepts Edition 9th Project 5.2
Test on Macintosh 10.8.4 POSIX named pthread
Auther: junzhengrice@gmail.com
*/
#define PHIL_NUM 5 //Philosopher number
#define PHIL_THINK_INTERVAL 5 //phil think time interval, use for rand()
#define PHIL_LUNCH_INTERVAL 3 //phil eat time interval, const
#define PHIL_RETRY_INTERVAL 2 //phil retry to approach to chopsticks time, const
#define PHIL_MAX_MEAL 2 //Max meal eaten individually

//Philosophers ID
static const int phils[PHIL_NUM]={0,1,2,3,4};

//Accumulated meals consumed by each philosopher
static int meals[PHIL_NUM]={0,0,0,0,0}; 

//Chopsticks availability indicator. 1 is available, 0 is not    
static int chopsticks[PHIL_NUM]={1,1,1,1,1};

//Mutexes for each philosopher. Only 1 lock
static pthread_mutex_t mutex;

//Conditions for each chopstick
static pthread_cond_t conditions[PHIL_NUM];

//Child threads param struct
struct THREAD_PARAMETERES{
    int ind;
};
typedef struct THREAD_PARAMETERES TP;

//Thread function
void* func_phil(void*);

int main(int argc, char *argv[]){
    
    pthread_mutex_init(&mutex,NULL);
    for(int i=0;i<PHIL_NUM;i++) pthread_cond_init(conditions+i,NULL);
        
    pthread_t phils[PHIL_NUM];
  
    TP* tps[PHIL_NUM];
    for(int i=0;i<PHIL_NUM;i++){
        tps[i] = (TP*)malloc(sizeof(TP*));
        tps[i]->ind = i;
    }
    
    for(int i=0;i<PHIL_NUM;i++)
        pthread_create(phils+i, NULL, func_phil, (void*)(tps[i]));
    for(int i=0;i<PHIL_NUM;i++)
        pthread_join(phils[i],NULL);
        
    exit(EXIT_SUCCESS);
}

void* func_phil(void* param){
    int pid = ((TP*)param)->ind; //printf("Phil pid: %d\n",pid);
    
    //Every phil starts by pre thinking
    int pre_think = rand()%PHIL_THINK_INTERVAL+1;
    sleep(pre_think);
    printf("Phil %d of pthread_id %08lu: After pre think %d s Ready to eat\n",
            pid, (unsigned long)pthread_self(),pre_think);
                 
    while(meals[pid]<PHIL_MAX_MEAL){
        /*
          Enter lock. The mutex lock covers from 1 phil picks left chopstick,
          ends after pick right chopstick
        */
        pthread_mutex_lock(&mutex); 
        
        //Left chopstick is available. Assume chopsticks[pid] is the left chopstick
        if(chopsticks[pid]==1){
            printf("Phil %d of pthread_id %08lu: Got left chopstick\n",
                    pid, (unsigned long)pthread_self());
            
            chopsticks[pid]=0;
            
            //Try pick up right chopstick.Assume chopsticks[pid+1] is the left chopstick
            while(chopsticks[(pid+1)%PHIL_NUM]==0){
                printf("Phil %d of pthread_id %08lu: Right chopstick is missing! Wait\n",
                        pid, (unsigned long)pthread_self());
                pthread_cond_wait(conditions+(pid+1)%PHIL_NUM, &mutex);
            }
            
            printf("Phil %d of pthread_id %08lu: Got right chopstick\n",
                    pid, (unsigned long)pthread_self());
             
            //Reset chopstick, unlock mutex
            chopsticks[(pid+1)%PHIL_NUM]=0;
            pthread_mutex_unlock(&mutex); 
                
            //Eat
            sleep(PHIL_LUNCH_INTERVAL);
            meals[pid]++;
            printf("Phil %d of pthread_id %08lu: %d lunch is finished\n",
                    pid, (unsigned long)pthread_self(),meals[pid]);
                
            //Put down chopsticks, signal
            pthread_mutex_lock(&mutex); 
            chopsticks[pid]=1;
            chopsticks[(pid+1)%PHIL_NUM]=1;
            pthread_cond_signal(conditions+pid);
            pthread_cond_signal(conditions+(pid+1)%PHIL_NUM);
            pthread_mutex_unlock(&mutex); 
            printf("Phil %d of pthread_id %08lu: Put down 2 chopstickes. Time to think\n",
                    pid, (unsigned long)pthread_self());
                    
            //Think       
            sleep(PHIL_THINK_INTERVAL);
        }
        //Left chopstick is not available
        else{
            printf("Phil %d of pthread_id %08lu: Left chopstick is missing! I'll retry\n", 
                    pid, (unsigned long)pthread_self());
            
            //Unlock, retry
            pthread_mutex_unlock(&mutex); 
            sleep(PHIL_RETRY_INTERVAL);
        }
    }
    printf("Phil %d of pthread_id %08lu: Lunch time is done. I have eaten %d meals\n",
            pid, (unsigned long)pthread_self(), meals[pid]);
    pthread_exit(0);
}