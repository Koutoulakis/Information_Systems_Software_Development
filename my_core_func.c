#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "core.h"
#include "index.h"
#include "hashing_func.h"
#include "defn_impl.h"
#include "index_impl.h"
#include "data_structures.h"
#include "my_core_func.h"
#include "threads.h"


/*----------------------------build_all_indexes-------------------------------*/

/* ---- ftiaxnei ola ta indexes pou xreiazontai --- */
void build_all_indexes() {
	int i, hamming_indexes = MAX_WORD_LENGTH - MIN_WORD_LENGTH + 1;
	Entry* cur_entry;
	EntryList entrylist_edit;
	EntryList entrylists_hamming[hamming_indexes];
	EntryList * entrylist_pointer;
	for (i = 0; i < hamming_indexes; i++)
		CreateEntryList(&(entrylists_hamming[i]));
	CreateEntryList(&entrylist_edit);
	hash_table_to_entrylist(GlobalStruct.HashTableEditDistance,
							GlobalStruct.NumberOfBuckets,
							&entrylist_edit);
	for (i = 0; i < GlobalStruct.NumberOfBuckets; i++) {
		cur_entry = GlobalStruct.HashTableHammingDistance[i];
		while (cur_entry != NULL) {
			int cur_word_length = cur_entry->word->w_length;
			int respective_entrylist = cur_word_length - MIN_WORD_LENGTH;
			AddEntry(&entrylists_hamming[respective_entrylist], cur_entry);
			cur_entry = cur_entry->next_entry;
		}
	}
	/*se auto to shmeio prepei na exw ftiaksei oles tis aparaithtes entrylists*/
	/*Epomenws xtizw ta antistoixa indexes*/
	if (entrylist_edit.head_entry != NULL) {
		GlobalStruct.EditDistanceIndex = malloc(sizeof (Index));
		BuildEntryIndex(&entrylist_edit,
						MT_EDIT_DIST,
						GlobalStruct.EditDistanceIndex);
	}
	for (i = 0; i < hamming_indexes; i++)
		if ((entrylists_hamming[i]).head_entry != NULL) {
			GlobalStruct.HammingDistanceIndexes[i] = malloc(sizeof (Index));
			BuildEntryIndex(&(entrylists_hamming[i]),
							MT_HAMMING_DIST,
							GlobalStruct.HammingDistanceIndexes[i]);
		}
	entrylist_pointer = &entrylist_edit;
	DestroyEntryList(entrylist_pointer);
	for (i = 0; i < hamming_indexes; i++) {
		entrylist_pointer = &(entrylists_hamming[i]);
		DestroyEntryList(entrylist_pointer);
	}
}


/*----------------------Fill_FinalMatchingQueryMatrix-------------------------*/

/*Filling MxN Matrix Function*/
void Fill_FinalMatchingQueryMatrix(EntryList * results) {
	int i, j;
	/*trexw ena ena ta entries kai apo to payload tous,pairnw to id
	 * thn 8esh tous sto query kai ta xwnw ston global MxN pinaka*/
	Entry* curr_entry;
	curr_entry = GetFirst(results);
	int q_id;
	/*pou einai to id ston Global pinaka MxN=FinalMatchingQueryMatrix*/
	int matrix_location;
	int position;
	/*EDW PSAXNW THN ENTRYLIST RESULTS NA XWNW POIA STO MATRIX(MxN) 0,1 STA 
	 * QUERIES KANOUN MATCH STO DOCUMENT*/
	while (curr_entry != NULL) {
		int found_distance = curr_entry->payload->found_dist;
		if (found_distance == 0)
			i = found_distance;
		else
			i = found_distance - 1;
		for (; i < curr_entry->payload->data_array_length; i++) {
			/*an exei gia to threshold apotelesmata..*/
			if (curr_entry->payload->payload_data_array[i].word_position != 0) {
				/*DEIKTH SE PAYYLOAD DATA GiA NA TREXW ENA ENA TA 
				 * BUCKES NA PIANW IDS KAI POSITIONS*/
				Payload_data * temp_payload_data;
				temp_payload_data=&(curr_entry->payload->payload_data_array[i]);
				int counter = 0;
				while (temp_payload_data->word_position != 0
						|| temp_payload_data->next != NULL) {
					counter++;
					/*Trexw OLh thn lista tou threshold se ka8e payload*/
					/*pairnoume to id tou apotelesmatos*/
					q_id = temp_payload_data->q_id;
					/*pairnoume to position*/
					position = temp_payload_data->word_position;
					/*briskoume thn 8esh ston pinaka pou exei to id p 8elome*/
					matrix_location = binary_search(
										GlobalStruct.FinalMatchingQueryMatrix,
										0,
										GlobalStruct.number_of_queries - 1,
										q_id);
					/*Bazoume ston pinaka sthn 8esh pou bre8hke h le3h 1*/

					set_one(GlobalStruct.FinalMatchingQueryMatrix_to_match, 
							matrix_location, 
							position - 1);
					temp_payload_data = temp_payload_data->next;
					if (temp_payload_data == NULL)
						/*BREAK GIATI AMA EINAI NULL KAI ANEBEI STHN WHILE KAI 
						 * TSEKAREI WORD POSITION TRWEI SEG*/
						break;
				}
			}
		}
		/*Sunexizoume na diasxizoume thn EntryList*/
		curr_entry = GetNext(results);
	}
}

/*----------------------------FindMatchingQids--------------------------------*/
void FindMatchingQids(unsigned int *p_num_res, QueryID** p_query_ids) {

	int i, j;
	/*EDW FTIAXNW LISTA NA XWNW POIA QUERYS KANOUN MATCH STO DOCUMENT*/
	int number_of_queries = GlobalStruct.number_of_queries;
	int** matching_matrix_ptr = GlobalStruct.FinalMatchingQueryMatrix;
	FinalQidList* matching_qid_list_ptr = GlobalStruct.CurrentMatchingQidsList;
	for (i = 0; i < number_of_queries; i++) {
		/*o char gia na elegxw an einai olo to query matched*/
		char temp_char /*= ~0*/; 
		//ta kanw ola 1, k meta kanw shift de3ia kata 8-number_of_words gia na 
		//exw posa prp na einai ta 1 g na kanei match to query
		temp_char = 255 >> (8 - matching_matrix_ptr[i][1]);

		/*Meta kanw Xor me to pws einai akribws to byte mas ,k prp na
		 * epistrefei 0 alliws kapoio word sto query mas dn exei ginei match*/
		if ((GlobalStruct.FinalMatchingQueryMatrix_to_match[i] ^ temp_char)==0){
			/*Kanei Match to query mas opte to pros8etoume
			sthn lista mas me ta matching ids.*/
			if (matching_qid_list_ptr->head_qid == NULL) {
				/*einai to prwto query pou exei ginei match*/
				matching_qid_list_ptr->head_qid = malloc(sizeof (FinalQidNode));
				/*bazw to id tou sthn lista*/
				matching_qid_list_ptr->head_qid->q_id=matching_matrix_ptr[i][0];
				matching_qid_list_ptr->head_qid->next_q_id = NULL;
				matching_qid_list_ptr->last_qid=matching_qid_list_ptr->head_qid;
				(*p_num_res)++;
			} else {/*exoume hdh ids mesa*/
				/*kanoume malloc neo kombo*/
				matching_qid_list_ptr->last_qid->next_q_id =
						malloc(sizeof (FinalQidNode));
				/*deixnoume ston neo kombo*/
				matching_qid_list_ptr->last_qid =
						matching_qid_list_ptr->last_qid->next_q_id;
				/*bazoume sto id to id toiu query pou kanei match*/
				matching_qid_list_ptr->last_qid->q_id =
						matching_matrix_ptr[i][0];
				matching_qid_list_ptr->last_qid->next_q_id = NULL;
				(*p_num_res)++;
			}
		}
	}
	FinalQidNode * tempnode;
	matching_qid_list_ptr->last_qid = tempnode=matching_qid_list_ptr->head_qid;
	if ((*p_num_res) > 0) {
		if ((*p_query_ids = malloc((*p_num_res) * sizeof (QueryID))) == NULL) {
			printf("There is not enough memory.\n");
			exit(EXIT_FAILURE);
		}
		for (i = 0; i < (*p_num_res); i++)
			(*p_query_ids)[i] = 0;
		i = 0;
		while (matching_qid_list_ptr->last_qid != NULL && i < (*p_num_res)) {
			tempnode = matching_qid_list_ptr->last_qid;
			(*p_query_ids)[i] = tempnode->q_id;
			matching_qid_list_ptr->last_qid =
					matching_qid_list_ptr->last_qid->next_q_id;
			free(tempnode);
			i++;
		}
		matching_qid_list_ptr->head_qid = matching_qid_list_ptr->last_qid=NULL;
		/*ksanaarxikopoiw ton FinalMatchingQueryMatrix_to_match */
		for (i = 0; i < number_of_queries; i++) {
			GlobalStruct.FinalMatchingQueryMatrix_to_match[i] = 0;
		}
	}
}

/*-----------------------------DestroyHeadDoc---------------------------------*/
void DestroyHeadDoc() {

	DocumentList* doc_list_ptr = GlobalStruct.DocList;
	if (doc_list_ptr->head != NULL) {
		DocumentListNode* next_list_node = doc_list_ptr->head->next_doc;
		Word * temp_wordlistcur;
		Word * temp_wordlistpre;
		temp_wordlistcur= temp_wordlistpre= doc_list_ptr->head->doc.words_list;
		/*Sbhnoume oles ts le3eis tou doc*/
		while (temp_wordlistcur != NULL) {
			temp_wordlistpre = temp_wordlistcur->next_word;
			free(temp_wordlistcur);
			temp_wordlistcur = temp_wordlistpre;
		}
		free(doc_list_ptr->head->doc.doc_string);
		free(doc_list_ptr->head);
		doc_list_ptr->head = next_list_node;
	}
}

void set_one(char * matrix, int i, int position_of_word_in_query) {
	pthread_mutex_lock(&mtx_set_one);
	(matrix[i]) = ((matrix[i]) | (1 << position_of_word_in_query));
	pthread_mutex_unlock(&mtx_set_one);
}

/*--------------------------------FreeHash------------------------------------*/
void FreeHash(Entry ** *HashTablePointer) {
	int i;
	int NumberOfBuckets = GlobalStruct.NumberOfBuckets;
	int cur_pos = 0;

	for (i = 0; i < NumberOfBuckets; i++) {
		Entry* cur_entry_ptr = (*HashTablePointer)[i];
		Entry* temp_entry_ptr;
		while (cur_entry_ptr != NULL) {
			temp_entry_ptr = cur_entry_ptr->next_entry;
			DestroyEntry(cur_entry_ptr);
			free(cur_entry_ptr);
			cur_entry_ptr = temp_entry_ptr;
		}
	}
	free(*HashTablePointer);
}

/*---------------------------FreeGlobalVariables------------------------------*/
void FreeGlobalVariables(void) {
	int i;
	/*********************** FREE STA HASH TREES **********************/
	free(GlobalStruct.HashTableEditDistance);
	FreeHash(&(GlobalStruct.HashTableExactMatch));
	FreeHash(&(GlobalStruct.HashTableHammingDistance));
	/**********************FREE SE BOH8HTIKES LISTES **************************/
	DocumentListNode * temp1;
	GlobalStruct.DocList->last_position = temp1 = GlobalStruct.DocList->head;
	while (GlobalStruct.DocList->last_position != NULL) {
		Word * temp_wordlistcur;
		Word * temp_wordlistpre;
		temp_wordlistcur = GlobalStruct.DocList->last_position->doc.words_list;
		temp_wordlistpre = GlobalStruct.DocList->last_position->doc.words_list;
		temp1 = GlobalStruct.DocList->last_position->next_doc;
		while (temp_wordlistcur != NULL) {
			temp_wordlistpre = temp_wordlistcur->next_word;
			free(temp_wordlistcur);
			temp_wordlistcur = temp_wordlistpre;
		}
		GlobalStruct.DocList->last_position = temp1;
	}
	/******************************************************/
	QueryTempInfoNode * temp2;
	QueryTempInfoNode * temp22;
	temp2 = GlobalStruct.QueryTempInfoList->head_node;
	temp22 = GlobalStruct.QueryTempInfoList->head_node;
	while (temp2 != NULL) {
		temp2 = temp22->next_node;
		free(temp22);
		temp22 = temp2;
	}
	/******************************************************/
	FinalQidNode * temp3;
	FinalQidNode * temp33;
	temp3 = GlobalStruct.CurrentMatchingQidsList->head_qid;
	temp33 = GlobalStruct.CurrentMatchingQidsList->head_qid;
	while (temp3 != NULL) {
		temp3 = temp33->next_q_id;
		free(temp33);
		temp33 = temp3;
	}
	/************FREE STOUS MxN boh8htiko Pinaka**********/
	for (i = 0; i < GlobalStruct.number_of_queries; i++) {
		free(GlobalStruct.FinalMatchingQueryMatrix[i]);
	}
	free(GlobalStruct.FinalMatchingQueryMatrix);
	free(GlobalStruct.FinalMatchingQueryMatrix_to_match);
	/**********FREE TA INDEXES*******************/
	DestroyEntryIndex(GlobalStruct.EditDistanceIndex);
	free(GlobalStruct.EditDistanceIndex);

	int hamming_indexes = MAX_WORD_LENGTH - MIN_WORD_LENGTH + 1;
	for (i = 0; i < hamming_indexes; i++) {
		DestroyEntryIndex(GlobalStruct.HammingDistanceIndexes[i]);
		free(GlobalStruct.HammingDistanceIndexes[i]);
	}
	free(GlobalStruct.HammingDistanceIndexes);
	
	free(GlobalStruct.DocList);
	free(GlobalStruct.CurrentMatchingQidsList);
	free(GlobalStruct.QueryTempInfoList);
}


/*---------------------compare_FinalMatchingQueryMatrix-----------------------*/

/*Compare function*/
int compare_FinalMatchingQueryMatrix(const void *pa, const void *pb) {
	/*apo edw to "empneusthka"*/
	/*http: //stackoverflow.com/questions/
	 * /17202178/c-qsort-with-dynamic-n-by-2-multi-dimensional-array*/
	static int counter = 1;
	counter++;
	const int *a = *(const int **) pa;
	const int *b = *(const int **) pb;
	if (a[0] == b[0])
		return a[1] - b[1];
	else
		return a[0] - b[0];
}


/*---------------------------binary_search------------------------------------*/

/*Binary Search: matrix einai o MxN ,q_id_to match eiani to id
 * pou psaxnoume na broume s auton tn matrix*/
int binary_search(int ** array, int first, int last, int q_id_to_match) {

	int middle = (int) ((first + last) / 2);
	while (first <= last) {
		if (array[middle][0] < q_id_to_match)
			first = middle + 1;
		else if (array[middle][0] == q_id_to_match) {
			return middle;
			break;
		}
		else
			last = middle - 1;
		middle = (int) ((first + last) / 2);
	}
	if (first > last)
		return -1;
}


