/******************************************************************
 * The Main program with the two functions. A simple
 * example of creating and using a thread is provided.
 ******************************************************************/

#include "helper.h"

void *producer (void *id);
void *consumer (void *id);

unsigned short  N;//size of queue, assigned value with command line input
unsigned short num_job;//number of job per producer, assigned with command line input
unsigned short num_pro;//number of producer, assigned with command line input
unsigned short  num_con;//number of consumer, assigned with command line input

unsigned short  in=0;//index of position in producer queue
unsigned short out=0;//index of position in consumer queue

int* buf_ptr;//pointer to critical array

  int producer_id = 0; //producer id
  int consumer_id = 0; //consumer id

  int cnt_con;//total number of jobs to consume
  int semid;//semaphore set id

//sleep parameter
int item;

int main (int argc, char **argv)
{
 
   
  //check parameters and assigned to global  varibles
  if(check_arg(argv[1])==-1) {cerr<<"Wrong size of queue input!(0-9)"<<endl; exit(1);}
  else   {
    N=check_arg(argv[1]);
    buf_ptr=new int[N];
    for(int i=0;i<N;i++) buf_ptr[i]=0;
  }//dynamically allocate critical buffer, each element initialize to 0

  if(check_arg(argv[2])==-1) {cerr<<"Wrong number of jobs input!(0-9)"<<endl; exit(1);}
  else   num_job=check_arg(argv[2]); 

  if(check_arg(argv[3])==-1) {cerr<<"Wrong number of producers input!(0-9)"<<endl; exit(1);}
  else   num_pro=check_arg(argv[3]);
    
  if(check_arg(argv[3])==-1) {cerr<<"Wrong number of  consumerss input!(0-9)"<<endl; exit(1);}
  else  num_con=check_arg(argv[4]);
		
  //set up and initialize semaphores

  key_t key=SEM_KEY;
  cnt_con=num_job*num_pro;  

  semid=sem_create(key, 3);//semaphore set containing 3 semaphores

  if(semid==-1) {cout<<"Failure to create semaphore set!"<<endl;exit(2);}
  
//s0:mutex
  if(sem_init(semid,0,1)==-1) {cerr<<"Failure to initialize mutex semaphore!"<<endl;exit(3);}
  
  //s1:spaces
  if(sem_init(semid,1,N)==-1) {cerr<<"Failure to initialize space semaphore!"<<endl;exit(3);}

  //s2:P,item
  if(sem_init(semid,2,0)==-1) {cerr<<"Failure to initialize item semaphore!"<<endl;exit(3);}
  
   pthread_t producerid[num_pro];//producer thread
   pthread_t consumerid[num_con];//consumer thread

   item=N;
   int parameter=N;

  for(int i=0;i<num_pro;i++){
    if(pthread_create (&producerid[i], NULL, producer, (void *) &parameter)!=0)
      {cerr<<"Failure to create producer thread!"<<endl; exit(4);}
  }
    
  for(int i=0;i<num_con;i++){
    if(pthread_create (&consumerid[i], NULL, consumer, (void *) &parameter)!=0)
      {cerr<<"Failure to create consumer thread!"<<endl; exit(4);}
  }
  
 
  for(int i=0;i<num_pro;i++){
    if(pthread_join (producerid[i], NULL)!=0) {cerr<<"Failure to join producer thread!"<<endl; exit(5);}
  }

  for(int i=0;i<num_con;i++){
    if(pthread_join (consumerid[i], NULL)!=0) {cerr<<"Failure to join consumer thread!"<<endl; exit(5);}
  }

  if(sem_close(semid)==-1) {cerr<<"Failure to destroy semaphore set!"<<endl; exit(6);}
  
  delete []buf_ptr;//delete ptr to heap memory

  return 0;
}

void *producer (void *parameter)
{
  int cnt_pro=num_job;
  int pro=++producer_id;
  int duration; 
  
  while(1){

    if(cnt_pro<=0){printf("producer (%d): No more jobs to generate\n",pro);
       pthread_exit(0);}
     }
    
     duration=rand()%10+1;
     --cnt_pro;//produce job

     
     /*  
     item--;     
     if(item<=0) sleep(20);
     if(item<=0) {
       printf("producer (%d): No more jobs to generate\n",pro);
       pthread_exit(0);}
     */
     
     sem_wait(semid,1);//block when buffer is full
     
     sem_wait(semid,0);//enter critical section
     
     buf_ptr[in]=duration;
     printf("Producer(%d): job id %d duration %d\n",pro,in,duration);
     
     in=(in+1)%N;//deposit job
     
     sem_signal(semid,0);//leave critical section
     sem_signal(semid,2);
    
     sleep(rand()%5+1);//sleep for 1-5s after producing a job
        
    
   pthread_exit(0);
}

void *consumer (void *parameter)
{ 
  int con=++consumer_id;
  int duration;
  int job_id;
  
  while(1){

    if(cnt_con<=0){printf("Consumer(%d): No more jobs left\n",con);
      pthread_exit(0);}
     
    /* item++;
     if(item>=N) sleep(20);
     if(item>=N) {
       printf("Consumer(%d): No more jobs left\n",con);
       pthread_exit(0);}
    */
     
    sem_wait(semid,2);//block when buffer is empty
    sem_wait(semid,0);//enter critical section
    
    duration=buf_ptr[out];
    printf("Consumer(%d): job id %d executing time %d\n",con,out,duration);
    job_id=out;
    out=(out+1)%N;   
    buf_ptr[job_id]=0;//fetch job
      
    sem_signal(semid,0);//leave critical section         
    sem_signal(semid,1);   
   
    sleep(duration);//sleep for duration, consuming a job
    printf("Consumer(%d): job id %d completed\n",con,job_id);
    
    --cnt_con;//consume a job
   
  }
  
   pthread_exit (0);
}

/***********************************

***** sleep for 20s when blocked (not done) modify in main, substituting pthread_join API:

struct timespec ts;
int s;
if(clock_gettime(CLOCK_REALTIME, &ts))==-1){cerr<<"Failure to get real time!"<<endl; exit();}

ts.tv_sec+=20;

******  do separately for producer and consumer


s=pthread_timedjoin_np(thread,NULL, &ts);
if(s==ETIMEDOUT){cout<<"No position in the que to deposit a job after waiting for 20s!"<<endl;}
else if(s!=0){cout<<"Failure to join thread"<<endl;}


 **********************************/
