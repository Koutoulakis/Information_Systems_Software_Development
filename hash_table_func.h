/* 
 * File:   hash_table_func.h
 * Author: panagiotis
 *
 * Created on February 28, 2014, 4:58 PM
 */

#ifndef HASH_TABLE_FUNC_H
#define	HASH_TABLE_FUNC_H

#include "core.h"
#include "index.h"

#ifdef	__cplusplus
extern "C" {
#endif

void create_entry_hash_table(Entry ** HashTablePointer,
							 enum MatchType matchtype,
							 unsigned int NumberOfBuckets);

void insert_entry_hash_table(Entry ** HashTablePointer,
							 unsigned int NumberOfBuckets,
							 Word word_object,
							 enum MatchType match_type,
							 unsigned int match_dist,
							 unsigned int *word_position,
							 QueryID query_id);

void insert_entry_here( Word word_object,
						enum MatchType match_type,
						unsigned int match_dist,
						unsigned int word_position,
						QueryID query_id,
						unsigned int payload_data_pos,
						Entry* *entry_ptr);

void search_entry_exact_hash_table(Word* cur_word,
							 Entry** HashTablePointer,
							 unsigned int NumberOfBuckets,
							 EntryList *results);

void hash_table_to_entrylist (  Entry ** HashTablePointer,
								unsigned int NumberOfBuckets,
								EntryList* entrylist);


#ifdef	__cplusplus
}
#endif

#endif	/* HASH_TABLE_FUNC_H */

