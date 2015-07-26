#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#include "defn_impl.h"
#include "core.h"
#include "data_structures.h"
#include "my_core_func.h"
#include "job_structs.h"
#include "thread_func.h"

/*-----------------------------InitializeIndex--------------------------------*/
enum ErrorCode InitializeIndex() {

	int i;
	/*Initialize HashTable Pointers*/
	GlobalStruct.NumberOfBuckets = MAX_QUERY_WORDS * 2 * 1000 + 7;
	unsigned int buckets = GlobalStruct.NumberOfBuckets;
	GlobalStruct.HashTableEditDistance = malloc(buckets * sizeof (Entry*));
	GlobalStruct.HashTableHammingDistance = malloc(buckets * sizeof (Entry*));
	GlobalStruct.HashTableExactMatch = malloc(buckets * sizeof (Entry*));
	for (i = 0; i < buckets; i++) {
		GlobalStruct.HashTableEditDistance[i] = NULL;
		GlobalStruct.HashTableHammingDistance[i] = NULL;
		GlobalStruct.HashTableExactMatch[i] = NULL;
	}
	/*Initialize Hamming Indexes*/
	GlobalStruct.EditDistanceIndex = NULL;
	/*Initialize Hamming Indexes*/
	int hamming_indexes = MAX_WORD_LENGTH - MIN_WORD_LENGTH + 1;
	GlobalStruct.HammingDistanceIndexes = malloc(hamming_indexes * sizeof (Index*));
	for (i = 0; i < hamming_indexes; i++)
		GlobalStruct.HammingDistanceIndexes[i] = NULL;
	/*Initialize rest*/
	GlobalStruct.DocList = malloc(sizeof (DocumentList));
	GlobalStruct.DocList->head = NULL;
	GlobalStruct.DocList->last_position = NULL;

	GlobalStruct.QueryTempInfoList = malloc(sizeof (QueryTempInfoNode));
	GlobalStruct.QueryTempInfoList->head_node = NULL;
	GlobalStruct.QueryTempInfoList->last_node = NULL;

	GlobalStruct.number_of_queries = 0;

	GlobalStruct.CurrentMatchingQidsList = malloc(sizeof (FinalQidList));
	GlobalStruct.CurrentMatchingQidsList->head_qid = NULL;
	GlobalStruct.CurrentMatchingQidsList->last_qid = NULL;

	pthread_barrier_init(&our_barrier, NULL, NUM_WORKER_THREADS + 1);
	initialize_pool(&pool);

	pthread_mutex_init(&mtx_set_one, 0);
	pthread_mutex_init(&mtx_look_up, 0);
	pthread_mutex_init(&mtx_search_hash_exact, 0);
	pthread_mutex_init(&mtx, 0);
	pthread_mutex_init(&mtx_nonempty, 0);
	pthread_mutex_init(&mtx_nonfull, 0);
	pthread_cond_init(&cond_nonempty, 0);
	pthread_cond_init(&cond_nonfull, 0);
	create_threads(NUM_WORKER_THREADS, &cons);

	return EC_SUCCESS;
}

/*-------------------------------DestroyIndex---------------------------------*/
enum ErrorCode DestroyIndex() {
	int i;
	FreeGlobalVariables();

	SchedulerSendFinishedJobs();
	for (i = 0; i < NUM_WORKER_THREADS; i++)
		pthread_join(cons[i], 0);
	free(cons);

	pthread_barrier_destroy(&our_barrier);
	pthread_cond_destroy(&cond_nonempty);
	pthread_cond_destroy(&cond_nonfull);
	pthread_mutex_destroy(&mtx_nonempty);
	pthread_mutex_destroy(&mtx_nonfull);
	pthread_mutex_destroy(&mtx);

	pthread_mutex_destroy(&mtx_set_one);
	pthread_mutex_destroy(&mtx_look_up);
	pthread_mutex_destroy(&mtx_search_hash_exact);

	return EC_SUCCESS;
}

/*--------------------------------StartQuery----------------------------------*/
enum ErrorCode StartQuery(QueryID query_id,
						  const char* query_str,
						  enum MatchType match_type,
						  unsigned int match_dist) {

	char word[MAX_WORD_LENGTH + 1]; /*metablhth p xwraei 1 le3h*/
	int i = 0;
	unsigned int word_counter = 0;
	GlobalStruct.number_of_queries++;
	/*Apothhkeuw tis info (q_id kai number of words) sthn TempInfoList*/
	if (GlobalStruct.QueryTempInfoList->head_node == NULL) {
		GlobalStruct.QueryTempInfoList->head_node =
				malloc(sizeof (QueryTempInfoNode));
		GlobalStruct.QueryTempInfoList->head_node->q_id = query_id;
		GlobalStruct.QueryTempInfoList->head_node->query_word_count = 0;
		GlobalStruct.QueryTempInfoList->head_node->next_node = NULL;
		GlobalStruct.QueryTempInfoList->last_node =
				GlobalStruct.QueryTempInfoList->head_node;
	} else {
		GlobalStruct.QueryTempInfoList->last_node->next_node =
				malloc(sizeof (QueryTempInfoNode));
		GlobalStruct.QueryTempInfoList->last_node =
				GlobalStruct.QueryTempInfoList->last_node->next_node;
		GlobalStruct.QueryTempInfoList->last_node->q_id = query_id;
		GlobalStruct.QueryTempInfoList->last_node->query_word_count = 0;
		GlobalStruct.QueryTempInfoList->last_node->next_node = NULL;
	}
	/*parsarw to query me skopo na valw tis lekseis tou sta hash_tables */
	while (query_str[i] != '\0') {
		if (query_str[i] < 'a' || query_str[i] > 'z') {
			i++;
			continue; /*den einai le3h sunexizoume*/
		} else {/*brhkame le3h*/
			word_counter++;
			int j = 0;
			Word word_object;
			while (query_str[i] >= 'a' && query_str[i] <= 'z') {
				word[j] = query_str[i]; /*antigrafoume ena ena ta grammata*/
				j++;
				i++;
			}
			word[j] = '\0';
			CreateWord(word, &word_object);
			/*s'auto to shmeio exoume krathsei mia leksh tou query sto word*/
			/*Bhma 2: kanoume insert th leksh sto swsto hash_table
			 * (swsto me vash to matchtype)*/
			if (match_type == MT_HAMMING_DIST && match_dist != 0)
				insert_entry_hash_table(GlobalStruct.HashTableHammingDistance,
										GlobalStruct.NumberOfBuckets,
										word_object,
										match_type,
										match_dist,
										&word_counter,
										query_id);
			else if (match_type == MT_EDIT_DIST && match_dist != 0) {

				insert_entry_hash_table(GlobalStruct.HashTableEditDistance,
										GlobalStruct.NumberOfBuckets,
										word_object,
										match_type,
										match_dist,
										&word_counter,
										query_id);

			} else /* if (match_type == MT_EXACT_MATCH)*/
				insert_entry_hash_table(GlobalStruct.HashTableExactMatch,
										GlobalStruct.NumberOfBuckets,
										word_object,
										match_type,
										match_dist,
										&word_counter,
										query_id);
		}/*end of else{/*brhkame le3h*/
	}/*end of while(str[i]!='\0')*/
	GlobalStruct.QueryTempInfoList->last_node->query_word_count = word_counter;
	return EC_SUCCESS;
}

/*------------------------------MatchDocument---------------------------------*/
enum ErrorCode MatchDocument(DocID doc_id,
							 const char* doc_str) {
	DocumentListNode* cur_doc_node_ptr;
	static char first_call = 'Y';

	if (first_call == 'Y') {
		first_call = 'N';

		build_all_indexes();

		/*twra ftiaxnw ena PINAKA 2 Diastasewn NxM opou to N einai to plh9os twn 
		 * queries pou exoume kai M ka8e fora einai to plh9os twn le3ewn pou
		 * exei ena query + 2 opou to +2 sumbolizei 2 extra 8eseis stis opoies
		 * kratame 1on) to id tou query kai 2on) to plh8os twn le3ewn pou exei
		 * to sugkekrimeno query wste na to skanaroume swsta otan 8a 8eloume 
		 * na doume an oles oi le3eis tou kanoun match me to document to opoio
		 * eiani isodunamo me to na exei olo Assous o pinakasNxM mas sthn 8esh 
		 * pou exoume balei auto to query */
		/*********************************************/
		GlobalStruct.FinalMatchingQueryMatrix =
				malloc((GlobalStruct.number_of_queries) * sizeof (int *));
		GlobalStruct.FinalMatchingQueryMatrix_to_match =
				malloc((GlobalStruct.number_of_queries) * sizeof (char));
		if (GlobalStruct.FinalMatchingQueryMatrix == NULL || GlobalStruct.FinalMatchingQueryMatrix_to_match == NULL) {
			printf("There is not enough memory.\n");
			exit(EXIT_FAILURE);
		}
		// Here we make each row of it's own, individual length. which is 2 in the byte Matrix version 
		int i;
		GlobalStruct.QueryTempInfoList->last_node =
				GlobalStruct.QueryTempInfoList->head_node;
		for (i = 0; i < GlobalStruct.number_of_queries; i++) {
			/*Desmeuw oses 8eseis oses einai oi le3eis tou i query
			 * (skanarwntas tn lista pou kratame mesa to id tou query k 
			 * tis le3eis pou exei (Qid_list) */
			GlobalStruct.FinalMatchingQueryMatrix[i] = malloc((2) * sizeof (int));
			if (GlobalStruct.FinalMatchingQueryMatrix[i] == NULL) {
				printf("There is not enough memory.\n");
				exit(EXIT_FAILURE);
			}
			/*Apo8hkeuuw to id kai to poses le3eis exei to query*/
			GlobalStruct.FinalMatchingQueryMatrix[i][0] =
					GlobalStruct.QueryTempInfoList->last_node->q_id;
			GlobalStruct.FinalMatchingQueryMatrix[i][1] =
					GlobalStruct.QueryTempInfoList->last_node->query_word_count;
			GlobalStruct.QueryTempInfoList->last_node =
					GlobalStruct.QueryTempInfoList->last_node->next_node;
			/*adeiazw tn lista mias pou dn mas xreiazetai pia*/
			free(GlobalStruct.QueryTempInfoList->head_node);
			GlobalStruct.QueryTempInfoList->head_node =
					GlobalStruct.QueryTempInfoList->last_node;
			GlobalStruct.FinalMatchingQueryMatrix_to_match[i] = 0;
		}
		/**********************************************/
		/*EDW Sortarw ton apo panw pinaka ws pros id wste pio meta na kanw 
		 * duadikh anazhthsh gia to query p prp na tsekarw*/
		qsort(  GlobalStruct.FinalMatchingQueryMatrix,
				GlobalStruct.number_of_queries,
				sizeof (int *),
				compare_FinalMatchingQueryMatrix);
	}
	if (GlobalStruct.DocList->head == NULL) {
		/*EDW BAZW TO PRWTO DOCUMENT STHN DOCUMENT LIST MOU*/
		GlobalStruct.DocList->head = malloc(sizeof (DocumentListNode));
		cur_doc_node_ptr = GlobalStruct.DocList->head;
		//CreateDocument(doc_id, doc_str, &(GlobalStruct.DocList->head->doc));
		GlobalStruct.DocList->head->next_doc = NULL;
		GlobalStruct.DocList->last_position = GlobalStruct.DocList->head;
	}
	else { /*vazw to DOC sto telos ths documentList mas*/
		/*Kanoume malloc neo kombo sto telos tou doc*/
		GlobalStruct.DocList->last_position->next_doc =
				malloc(sizeof (DocumentListNode));
		/*To last position na deixnei sto Neo kombo pou dhmiourghsame parapanw*/
		GlobalStruct.DocList->last_position =
				GlobalStruct.DocList->last_position->next_doc;
		cur_doc_node_ptr = GlobalStruct.DocList->last_position;
		GlobalStruct.DocList->last_position->next_doc = NULL;
	}
	Job* current_job_ptr;
	//	StructCreateDocJob* create_doc_job_ptr = malloc(sizeof(StructCreateDocJob));
	//	create_doc_job_ptr->doc_id = doc_id;
	//	create_doc_job_ptr->doc_str = malloc(MAX_DOC_LENGTH*sizeof(char));
	//	strcpy(create_doc_job_ptr->doc_str, doc_str);
	//	create_doc_job_ptr->global_list_position = cur_doc_node_ptr;
	//	CreateJob(CREATE_DOC, (void *) create_doc_job_ptr, &current_job_ptr);
	//	SchedulerAddJob(current_job_ptr);
	//sleep(5);
	CreateDocument(doc_id, doc_str, &(cur_doc_node_ptr->doc));

	Document* cur_doc = &(cur_doc_node_ptr->doc);
	CreateJob(DEDUPLICATE, (void *) cur_doc, &current_job_ptr);
	SchedulerAddJob(current_job_ptr);
	return EC_SUCCESS;
}

/*-----------------------------GetNextAvailRes--------------------------------*/
enum ErrorCode GetNextAvailRes(DocID *p_doc_id,
							   unsigned int *p_num_res,
							   QueryID* *p_query_ids) {
	int i, j;
	static char first_call = 'Y';
	SchedulerSendBarrierJobs();
	pthread_barrier_wait(&our_barrier);

	DocumentListNode* cur_list_node = GlobalStruct.DocList->head;

	if (cur_list_node != NULL) {

		*p_doc_id = cur_list_node->doc.d_id;
		*p_num_res = 0;
		/* Tha xrhsimopoihsw to cur_list_node->doc gia na to kanw search kai
		 * na "gemisw" ta orismata ths GetNextAvRes me thn swsth plhroforia kai
		 * meta tha paw sto epomeno doc na kanw to idio, mexri na teleiwsoun 
		 * ta docs ths listas*/
		EntryList results;
		Word * cur_word;
		Document * cur_doc = &(cur_list_node->doc);
		/*remove ta duplicates, apothhkeush unique leksewn sth doc.words_list*/

		//RemoveDuplicates(&(cur_list_node->doc));

		cur_word = cur_doc->words_list;
		while (cur_word != NULL) {
			Job* current_job_ptr;
			Word* for_search_word;
			for_search_word = malloc(sizeof (Word));
			strcpy(for_search_word->string, cur_word->string);
			for_search_word->w_id = cur_word->w_id;
			for_search_word->w_length = strlen(cur_word->string);
			for_search_word->next_word = NULL;
			CreateJob(SEARCH_WORDS, (void *) for_search_word, &current_job_ptr);
			SchedulerAddJob(current_job_ptr);

			cur_word = cur_word->next_word;
		}
		SchedulerSendBarrierJobs();
		pthread_barrier_wait(&our_barrier);
		FindMatchingQids(p_num_res, p_query_ids);
		/*Edw sbhnoume to 1o document ths lista mas k bazoume to next gia 1o*/
		DestroyHeadDoc();
	} else {
		return EC_NO_AVAIL_RES;
	}
	return EC_SUCCESS;
}



