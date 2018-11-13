

#include "threadpool.h"
#include <stdlib.h>
#include <stdio.h>

#define TRUE 1
#define FALSE 0


//#define DO_DEBUG

void work_t_add(threadpool* pool, work_t *work);
work_t *work_t_pop(threadpool* pool);
void print(char *str);

threadpool* create_threadpool(int num_threads_in_pool) {
    if(num_threads_in_pool < 1 || num_threads_in_pool > MAXT_IN_POOL) {
        return NULL;
    }
    
    threadpool *pool = (threadpool *)malloc(sizeof(threadpool));
    if(!pool) {
        return NULL;
    }
    
    pool->num_threads = num_threads_in_pool;
    pool->qsize = 0;
    pool->shutdown = FALSE;
    pool-> dont_accept = FALSE;
    pool->qhead = NULL;
    pool->qtail = NULL;
    pool->threads = (pthread_t *)malloc(sizeof(pthread_t) * num_threads_in_pool);
    if(!pool->threads) {
        return NULL;
    }
    
    pthread_mutex_init(&pool->qlock, NULL);
    pthread_cond_init(&pool->q_not_empty, NULL);
    pthread_cond_init(&pool->q_empty, NULL);
    int i;
    for(i = 0; i< num_threads_in_pool; i++) {
        pthread_create(pool->threads+i, NULL, do_work, pool);
    }
    
    return pool;
}

void* do_work(void* p) {
    threadpool *pool = (threadpool *)p;
    while(TRUE) {
        print("at do_work, try lock qlock\n");
        pthread_mutex_lock(&pool->qlock);
        print("at do_work, lock qlock\n");
        while(pool->qsize == 0 && pool->dont_accept == FALSE) { // if the pool is empty
            print("at do_work, wait q_empty\n");
            pthread_cond_wait(&pool->q_empty, &pool->qlock);
            print("at do_work, wake up from q_empty\n");
            
            if(pool->shutdown == TRUE) {
                print("at do_work, shutdown\n");
                goto endloop;
            }
        }
        
        if(pool->qsize == 0 && pool->dont_accept == TRUE) {
            print("at do_work, signal q_not_empty\n");
            pthread_cond_signal(&pool->q_not_empty);
            goto endloop;
        }
        
        print("at do_work, take new work\n");
        work_t *work = work_t_pop(pool);
        print("at do_work, unlock qlock\n");
        pthread_mutex_unlock(&pool->qlock);
        work->routine(work->arg);
        free(work);
    }
    
endloop:
    print("at do_work, end\n");
    pthread_mutex_unlock(&pool->qlock);
    //pthread_exit(NULL);
    return NULL;
}

void dispatch(threadpool* pool, dispatch_fn dispatch, void *arg) {
    if(!pool) {
        return;
    }
    
    if(pool->dont_accept == TRUE) {
        print("at dispatch, not accepted\n");
        return;
    }
    
    work_t *work = (work_t *)malloc(sizeof(work_t));
    if(!work) {
        return;
    }
    
    work->arg = arg;
    work->routine = dispatch;
    work->next = NULL;
    print("at dispatch, add\n");
    pthread_mutex_lock(&pool->qlock);
    work_t_add(pool, work);
    print("at dispatch, signal q_empty\n");
    pthread_cond_signal(&pool->q_empty);
    pthread_mutex_unlock(&pool->qlock);
}

void work_t_add(threadpool* pool, work_t *work) {
    if(pool->dont_accept == TRUE) {
        print("at work_t_add, not accepted\n");
        return;
    }
    
    if(pool->qsize == 0) { // the queue is empty
        pool->qhead = work;
        pool->qtail = work;
    }
    
    else {
        pool->qtail->next = work;
        pool->qtail = pool->qtail->next;
    }
    
    pool->qsize++;
}

work_t *work_t_pop(threadpool* pool) {
    work_t *work = pool->qhead;
    if(pool->qhead) { // queue not empty
        if(pool->qsize == 1) { // only one argument in the queue
            print("at work_t_pop, one argument\n");
            pool->qhead = NULL;
            pool->qtail = NULL;
        }
        
        else {
            print("at work_t_pop, more then one argument\n");
            pool->qhead = pool->qhead->next;
        }
        
        pool->qsize--;
    }
    
    else {
        printf("ERROR: at work_t_pop, return null\n");
    }
    
    return work;
}

void destroy_threadpool(threadpool* pool) {
    if(!pool) {
        return;
    }
    
    print("at destroy, try lock qlock\n");
    pthread_mutex_lock(&pool->qlock);
    print("at destroy, lock qlock\n");
    pool->dont_accept = TRUE;
    print("at destroy, wait q_not_empty\n");
    if(pool->qsize != 0) {
        pthread_cond_wait(&pool->q_not_empty, &pool->qlock);
    }
    print("at do_work, wake up from q_not_empty\n");
    pool->shutdown = TRUE;
    print("at destroy, unlock qlock\n");
    pthread_mutex_unlock(&pool->qlock);
    print("at destroy, broadcast\n");
    pthread_cond_broadcast(&pool->q_empty);
    int i;
    for (i = 0; i < pool->num_threads; i++)
        pthread_join(pool->threads[i], NULL);
    free(pool->threads);
    free(pool);
}

void print(char *str) {
#ifdef DO_DEBUG
    printf("%s", str);
#endif
}


