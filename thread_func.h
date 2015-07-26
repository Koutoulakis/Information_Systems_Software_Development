/* 
 * File:   thread_func.h
 * Author: panagiotis
 *
 * Created on March 19, 2014, 9:09 AM
 */

#ifndef THREAD_FUNC_H
#define	THREAD_FUNC_H

#include "threads.h"
#include "cache_structures.h"

#ifdef	__cplusplus
extern "C" {
#endif

	void create_threads(unsigned int number_of_threads, pthread_t* *thread_ids);
	void CreateJob(enum JobType job_type, void * job_parameters, Job* *job_ptr);
	void initialize_pool(pool_t * pool);
	Job* obtain(pool_t * pool);
	void * consumer(void * ptr);
	void place(pool_t * pool, Job* job);
	void SchedulerAddJob(Job* job);
	void SchedulerSendFinishedJobs();
	void SchedulerSendBarrierJobs();
	void * producer(void * ptr);
	void JobExecute(Job* job, Cache *cache);

#ifdef	__cplusplus
}
#endif

#endif	/* THREAD_FUNC_H */

