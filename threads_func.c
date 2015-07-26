#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "core.h"
#include "index.h"
#include "hashing_func.h"
#include "defn_impl.h"
#include "index_impl.h"
#include "data_structures.h"
#include "hash_table_func.h"
#include "threads.h"
#include "thread_func.h"
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "job_structs.h"
#include  "cache_structures.h"

void create_threads(unsigned int number_of_threads, pthread_t* *thread_ids) {
	int i;
	*thread_ids = malloc(number_of_threads * sizeof (pthread_t));
	if ((*thread_ids) == NULL) {
		printf("Couldn't create threads, program is terminating...\n");
		exit(1);
	}
	for (i = 0; i < NUM_WORKER_THREADS; i++)
		pthread_create(&((*thread_ids)[i]), 0, consumer, 0);
}

void CreateJob(enum JobType job_type, void * job_parameters, Job* *job_ptr) {
	*job_ptr = malloc(sizeof (Job));
	if (*job_ptr == NULL) {
		printf("Couldn't create job, program is terminating...\n");
		exit(1);
	}
	(*job_ptr)->job_type = job_type;
	(*job_ptr)->job_params.arguments = job_parameters;
}

void initialize_pool(pool_t * pool) {
	/*tous jobs pointers den tous arxikopoiw, gt panta tha prepei na diavazoun
	 ta threads apo to pool otan toul 1 pointer deixnei se kanoniko job*/
	pool->start = 0;
	pool->end = -1;
	pool->count = 0;
}

Job* obtain(pool_t * pool) {
	Job* obtained_job_ptr;
	pthread_mutex_lock(&mtx_nonempty);
	while (pool->count <= 0) {
		//printf("\n>> Found Buffer Empty \n");
		pthread_cond_wait(&cond_nonempty, &mtx_nonempty);
	}
	obtained_job_ptr = pool->jobs[pool->start];
	pool->start = (pool->start + 1) % POOL_SIZE;
	pthread_mutex_lock(&mtx);
	pool->count--;
	pthread_mutex_unlock(&mtx);
	pthread_mutex_unlock(&mtx_nonempty);
	return obtained_job_ptr;
}

void * consumer(void * ptr) {

	Cache cache;
	create_and_initialize_cache(&cache);
	while (1) {
		/*termatizei MONO otan kanei execute sygkekrimenh job, thn "FINISHED"*/
		Job* current_job_ptr = obtain(&pool);
		pthread_mutex_lock(&mtx_nonfull);
		pthread_cond_signal(&cond_nonfull);
		pthread_mutex_unlock(&mtx_nonfull);
		JobExecute(current_job_ptr, &cache);
	}
}

/*thelw na valw to job ths parametrou sto pool*/
void place(pool_t * pool, Job* job) {
	pthread_mutex_lock(&mtx_nonfull);
	while (pool->count >= POOL_SIZE) {
		//printf(">> Found Buffer Full \n");
		pthread_cond_wait(&cond_nonfull, &mtx_nonfull);
	}
	pool->end = (pool->end + 1) % POOL_SIZE;
	pthread_mutex_lock(&mtx);
	pool->jobs[pool->end] = job;
	pool->count++;
	pthread_mutex_unlock(&mtx);
	pthread_mutex_unlock(&mtx_nonfull);
}

void SchedulerAddJob(Job* job) { /*h parametros exei 
							  * ginei malloced ( created ) apeksw*/
	place(&pool, job);
	pthread_mutex_lock(&mtx_nonempty);
	pthread_cond_signal(&cond_nonempty);
	pthread_mutex_unlock(&mtx_nonempty);
}

void SchedulerSendFinishedJobs() {
	int i;
	Job* current_job_ptr;
	for (i = 0; i < NUM_WORKER_THREADS; i++) {
		current_job_ptr = malloc(sizeof (Job));
		if (current_job_ptr == NULL) {
			printf("FAAAAAAAAAAAAAAALSE and exiting...\n");
			exit(1);
		}
		current_job_ptr->job_type = FINISHED;
		SchedulerAddJob(current_job_ptr);
	}
}

void SchedulerSendBarrierJobs() {
	int i;
	Job* current_job_ptr;
	for (i = 0; i < NUM_WORKER_THREADS; i++) {
		current_job_ptr = malloc(sizeof (Job));
		if (current_job_ptr == NULL) {
			printf("FAAAAAAAAAAAAAAALSE and exiting...\n");
			exit(1);
		}
		current_job_ptr->job_type = BARRIER;
		SchedulerAddJob(current_job_ptr);
	}
}

void JobExecute(Job* job, Cache* cache) {
	int * arguments;
	switch (job->job_type) {
		case CREATE_DOC:
		{
			StructCreateDocJob* arguments =
					(StructCreateDocJob*) job->job_params.arguments;
			CreateDocument(arguments->doc_id,
						arguments->doc_str,
						&(arguments->global_list_position->doc));
			free(arguments->doc_str);
			free(arguments);
			free(job);
			break;
		}
		case DEDUPLICATE:
		{
			Document* doc_ptr = (Document*) job->job_params.arguments;
			RemoveDuplicates(doc_ptr);
			free(job);
			break;
		}
		case SEARCH_WORDS:
		{
			EntryList* results;
			Word* cur_word = (Word*) job->job_params.arguments;
			/*An h cache mas den exei to stoixeio*/

			if (my_search_cache(cache, cur_word, &results) == EC_FAIL) {
				/*lookup sta 3 katallhla eurethria*/
				results = malloc(sizeof (EntryList));
				CreateEntryList(results);

				if (GlobalStruct.EditDistanceIndex != NULL)
					LookupEntryIndex(cur_word,
									MAX_MATCH_DIST,
									GlobalStruct.EditDistanceIndex, results);
				/* cur_word_lenth = {MIN_WORD_LENGTH ... MAX_WORD_LENGTH}
				 * hamming_indGlobalStruct.FinalMatchingQueryMatrix[i][1]exes = 
				 * {0 ... MAX_WORD_LENGTH - MIN_WORD_LENGTH + 1}
				 * ara respective_position = cur_word_length - MIN_WORD_LENGTH;
				 */

				int resp_hamming_index = cur_word->w_length - MIN_WORD_LENGTH;
				if (GlobalStruct.HammingDistanceIndexes[resp_hamming_index] !=
																		   NULL)
					LookupEntryIndex(
						cur_word,
						MAX_MATCH_DIST,
						GlobalStruct.HammingDistanceIndexes[resp_hamming_index],
						results);
				search_entry_exact_hash_table(
											cur_word,
											GlobalStruct.HashTableExactMatch,
											GlobalStruct.NumberOfBuckets,
											results);
				/*edw pleon tha prepei to entrylist results na exei oles tis 
				 * entries pou ekanan match me to trexon document.
				 * Prepei na ta epeksergastw(na symplhrwsw tn global pinaka klp)
				 * gia na tsekarw poia queries ekanan match kai na ta epistrepsw
				 * sto 3o orisma ths synarthshs.
				 */
				my_insert_to_cache(cache, results, cur_word);
			}
			Fill_FinalMatchingQueryMatrix(results);
			//DestroyEntryList(&results);
			free(job->job_params.arguments);
			free(job);
			break;
		}
		case BARRIER:
			free(job);
			pthread_barrier_wait(&our_barrier);
			break;
		case FINISHED:
			free(job);
			/*edw to delete cache...*/
			delete_cache(cache);
			pthread_exit(0);
			break;
		default:
			break;
	}
}


