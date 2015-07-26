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

void create_entry_hash_table(Entry ** HashTablePointer,
							 enum MatchType matchtype,
							 unsigned int NumberOfBuckets) {
	HashTablePointer = calloc(NumberOfBuckets, sizeof (Entry));
}

void insert_entry_hash_table(Entry ** HashTablePointer,
							 unsigned int NumberOfBuckets,
							 Word word_object,
							 enum MatchType match_type,
							 unsigned int match_dist,
							 unsigned int *word_position,
							 QueryID query_id) {

	/*pairnoume tn 8esh t pinaka p prp na mpei h le3h!*/
	unsigned int index = hash(word_object.string) % NumberOfBuckets;
	unsigned int payload_data_pos;
	/*an to match_dist pairnei mono timh mhden, tote auth thn timh thelw*/
	if (match_type == MT_EXACT_MATCH || match_dist == 0)
		payload_data_pos = match_dist;
		/*an to match_dist pairnei times {1,2,3} tote thelw to match_dist-1*/
	else
		payload_data_pos = match_dist - 1;
	/*an einai adeio to bucket ginetai apeutheias eisagwgh..*/
	if (HashTablePointer[index] == NULL) {
		insert_entry_here(word_object,
						match_type,
						match_dist,
						*word_position,
						query_id,
						payload_data_pos,
						&(HashTablePointer[index]));
	}		/*else, ean to bucket dn einai adeio => uparxei toul 1 entry*/
	else {
		/*...shmainei oti,1) eite exei tn idia le3h mesa ara brhkame duplicate,
						  2) eite egine collision k prp n tn xwsoume tn le3h
							 mesa stn lista t next_bucket*/
		Entry * entry_ptr = HashTablePointer[index];
		Entry * previous_entry = entry_ptr;
		/*diasxizoume th lista tou bucket pou vriskomaste*/
		while (entry_ptr != NULL) {
			int is_duplicate = 0;
			/*an einai h idia le3h*/
			if (strcmp(entry_ptr->word->string, word_object.string) == 0) {

				Payload_data * ptr =
						&(entry_ptr->payload->payload_data_array[payload_data_pos]);
				Payload_data * previous_payl_data_node = ptr;
				/*h parakatw while ginetai gia na dw mhpws to trexon q_id
				 * einai hdh sth lista (ara AN h trexousa leksh yparxei hdh &&
				 * to trexon q_id yparxei hdh => exoume diplotyph leksh mesa
				 * sto idio query, opote apla thn agnooume san na mhn yphrxe!)
				 */
				while (ptr != NULL) {
					if (ptr->q_id == query_id) {
						is_duplicate = 1;
						(*word_position)--;
						break;
					}
					previous_payl_data_node = ptr;
					ptr = ptr->next;
				}
				if (!is_duplicate) {
					if (previous_payl_data_node->q_id != 0) {
						/*An dn eimaste sto prwto bucket p einai hdh allocated*/
						CreatePayloadData(&(previous_payl_data_node->next), 1);

						previous_payl_data_node->next->q_id = query_id;
						previous_payl_data_node->next->word_position = 
																 *word_position;
					} else {
						previous_payl_data_node->next = NULL;
						previous_payl_data_node->q_id = query_id;
						previous_payl_data_node->word_position = *word_position;
					}
				}
				break; /*vrhkame idia leksh, ara den pame parakatw sth lista
						* tou bucket*/
			}
			previous_entry = entry_ptr;
			entry_ptr = entry_ptr->next_entry;
		}
		/*ean diasxisame olh th lista tou bucket xwris na vroume idia leksh*/
		if (entry_ptr == NULL) {
			insert_entry_here(word_object,
							match_type,
							match_dist,
							*word_position,
							query_id,
							payload_data_pos,
							&(previous_entry->next_entry));
		}
	}
}

void insert_entry_here(Word word_object,
					   enum MatchType match_type,
					   unsigned int match_dist,
					   unsigned int word_position,
					   QueryID query_id,
					   unsigned int payload_data_pos,
					   Entry* *entry_ptr) {
	*entry_ptr = malloc(sizeof (Entry));
	CreateEntry(&word_object, *entry_ptr);
	if (match_type == MT_EXACT_MATCH || match_dist == 0)
		/*yparxei mono 1 timh gia to threshold, h mhden*/
		CreatePayload(&((*entry_ptr)->payload), 1);
	else
		/*yparxoun 3 times gia to threshold, oi 1,2,3 */
		CreatePayload(&((*entry_ptr)->payload), 3);
	Payload * ptr = (*entry_ptr)->payload;
	ptr->payload_data_array[payload_data_pos].q_id = query_id;
	ptr->payload_data_array[payload_data_pos].word_position = word_position;
}

void search_entry_exact_hash_table(Word* cur_word,
								   Entry** HashTablePointer,
								   unsigned int NumberOfBuckets,
								   EntryList *results) {
	/*pairnoume tn 8esh t pinaka p prp na mpei h le3h!*/
	unsigned int index = hash(cur_word->string) % NumberOfBuckets;
	Entry* cur_entry = HashTablePointer[index];
	while (cur_entry != NULL) {
		if (strcmp(cur_entry->word->string, cur_word->string) == 0) {
			pthread_mutex_lock(&mtx_search_hash_exact);
			cur_entry->payload->found_dist = 0;
			AddEntry(results, cur_entry);
			cur_entry->payload->found_dist = -1;
			pthread_mutex_unlock(&mtx_search_hash_exact);
			break;
		}
		cur_entry = cur_entry->next_entry;
	}
}

void hash_table_to_entrylist(Entry ** HashTablePointer,
							 unsigned int NumberOfBuckets,
							 EntryList* entrylist) {
	int cur_pos = 0, number_of_entries = 0, first_notnull_bucket = 0;
	Entry* last_entry_ptr;
	/*psaxnw thn prwth not NULL thesh*/
	while (cur_pos < NumberOfBuckets && HashTablePointer[cur_pos] == NULL)
		cur_pos++;
	/*an oloi oi kadoi einai adeioi tote apla return*/
	if (cur_pos == NumberOfBuckets)
		return;
	first_notnull_bucket = cur_pos;
	number_of_entries++;
	last_entry_ptr = HashTablePointer[cur_pos];
	while (cur_pos < NumberOfBuckets) {
		while (last_entry_ptr->next_entry != NULL) {
			number_of_entries++;
			last_entry_ptr = last_entry_ptr->next_entry;
		}
		/*twra tha xw ftasei sthn teleutaia entry tou apo panw ""epipedou""*/
		int i;
		for (i = cur_pos + 1; i < NumberOfBuckets; i++)
			/*me to pou vrw mia thesh pou den einai NULL*/
			if (HashTablePointer[i] != NULL) {
				last_entry_ptr->next_entry = HashTablePointer[i];
				last_entry_ptr = HashTablePointer[i];
				number_of_entries++;
				cur_pos = i;
				break;
			}
		if (i == NumberOfBuckets) /*=> htan NULL oi theseis meta to cur_pos*/
			break;
	}
	(*entrylist).head_entry = HashTablePointer[first_notnull_bucket];
	(*entrylist).curr_entry = (*entrylist).head_entry;
	(*entrylist).last_entry = last_entry_ptr;
	(*entrylist).number_of_entries = number_of_entries;
}