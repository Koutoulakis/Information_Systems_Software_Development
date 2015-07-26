/* 
 * File:   scheduler.h
 * Author: panagiotis
 *
 * Created on March 17, 2014, 5:19 PM
 */

#ifndef SCHEDULER_H
#define	SCHEDULER_H

#include <pthread.h>

#define POOL_SIZE 7
#define NUM_WORKER_THREADS 4
#define NUM_JOBS 50
#define THREAD_SEARCH_WORDS 1

#ifdef	__cplusplus
extern "C" {
#endif

	enum JobType {
		SUM,
		CREATE_DOC,
		DEDUPLICATE,
		SEARCH_WORDS,
		BARRIER,
		FINISHED
	};

	typedef struct JobParams JobParams;

	struct JobParams {
		void* arguments;
	};

	typedef struct Job Job;

	struct Job {
		enum JobType job_type;
		JobParams job_params;
	};

	typedef struct {
		Job* jobs[POOL_SIZE];
		int start;
		int end;
		int count;
	} pool_t;

	/*GLOBAL variables pou aforoun sthn parallhlia*/
	pthread_barrier_t our_barrier;
	pthread_mutex_t mtx2;
	pthread_mutex_t mtx1;
	pthread_mutex_t mtx_set_one;
	pthread_mutex_t mtx_look_up;
	pthread_mutex_t mtx_search_hash_exact;
	pthread_mutex_t mtx;
	pthread_mutex_t mtx_nonempty;
	pthread_mutex_t mtx_nonfull;
	pthread_cond_t cond_nonempty;
	pthread_cond_t cond_nonfull;
	pool_t pool;
	pthread_t* cons;
	pthread_barrier_t our_barrier;

#ifdef	__cplusplus
}
#endif

#endif	/* SCHEDULER_H */

