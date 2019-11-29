#include<pthread.h>
#include "common.h"

/*TODO:  
     Insert approproate synchronization statements
     to make it work with multiple threads
*/
void *hashit(void *arg)
/*Argument is the end pointer*/
{
   char *cptr;
   unsigned long *chash;
   char *endptr = (char *)arg;   // END pointer


   while(1){
        pthread_mutex_lock(&lock);
        if(dataptr >= endptr){
            pthread_mutex_unlock(&lock);
            break;
        }

        // pthread_mutex_lock(&lock);

        cptr = dataptr;
        dataptr += BLOCK_SIZE;
        // pthread_mutex_unlock(&lock);
        chash = optr;
        optr++;
        pthread_mutex_unlock(&lock);
        /*   Perform the real calculation. The following line should not be inside any locks*/
        
        *chash = calculate_and_store_hash(cptr, endptr); 
        // dataptr += BLOCK_SIZE;
        // pthread_mutex_unlock(&lock);
  }
  pthread_exit(NULL); 
}
