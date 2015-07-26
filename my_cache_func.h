/* 
 * File:   my_cache_func.h
 * Author: panagiotis
 *
 * Created on March 23, 2014, 6:20 PM
 */

#ifndef MY_CACHE_FUNC_H
#define	MY_CACHE_FUNC_H

#ifdef	__cplusplus
extern "C" {
#endif


	enum ErrorCode create_and_initialize_cache(Cache *Cache);
	void delete_cache_hash_and_list(Cache* cache);
	enum ErrorCode delete_cache(Cache * cache);
	enum ErrorCode my_insert_to_cache(  Cache* cache_ptr,
										EntryList * resultlist_ptr,
										Word * word_ptr);
	void delete_LRU_element(Cache* cache_ptr);
	void insert_and_update_LRU_list(Cache* cache_ptr,
									unsigned int appropriate_bucket,
									EntryList* resultlist_ptr,
									Word* word_ptr);
	enum ErrorCode my_search_cache( Cache* cache_ptr,
									Word * word_ptr,
									EntryList* *resultlist_ptr);

#ifdef	__cplusplus
}
#endif

#endif	/* MY_CACHE_FUNC_H */

