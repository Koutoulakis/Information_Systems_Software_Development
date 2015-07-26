/* 
 * File:   cache_func.h
 * Author: panagiotis
 *
 * Created on March 21, 2014, 9:23 AM
 */

#ifndef CACHE_FUNC_H
#define	CACHE_FUNC_H

#include "defn_impl.h"
#include "index_impl.h"
#include "cache_structures.h"

#ifdef	__cplusplus
extern "C" {
#endif

enum ErrorCode create_and_initialize_cache(Cache *Cache);
void delete_cache_hash_and_list(Cache* cache);	
enum ErrorCode delete_cache(Cache *  cache);
enum ErrorCode insert_to_cache(Cache *  cache ,EntryList * resultlist, Word * word);
enum ErrorCode search_cache(Cache* cache,Word* word,EntryList *result);
void CopyResultList(EntryList * from_entrylist, EntryList * to_entrylist);


enum ErrorCode my_search_cache(Cache* cache, Word* word, EntryList * result);

#ifdef	__cplusplus
}
#endif

#endif	/* CACHE_FUNC_H */

