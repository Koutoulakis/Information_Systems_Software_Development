/* 
 * File:   cache_structures.h
 * Author: john
 *
 * Created on March 10, 2014, 1:11 AM
 */

#ifndef CACHE_STRUCTURES_H
#define	CACHE_STRUCTURES_H

#define CACHE_SIZE 100000
#define MAX_CACHE_ITEMS 200000


#ifdef	__cplusplus
extern "C" {
#endif

	typedef struct cache_element CacheElement;
	typedef struct Cache Cache;

	struct cache_element {
		/* H logikh einai oti exoume ena ena pointer se CacheElement,pou deixnei
		 *  mesa sto idio bucket se periptwsh overflow(next_bucket_element) kai
		 * enan Pointer se cacheelement next_cache_element opou deixnei pio
		 * mphke meta ap auto sto hash (DHmiourgoume ousiastika mia kuklikh
		 * lista me ta stoixeia analoga me tn xronologikh seira p exoun mpei
		 * mesa ta stoixeia,kai tn tropopoioume analoga me to pws allazei to
		 * Most recently used k less recently used element ts lista mas
		 */
		Word word;
		/*lista me t apotelesmata gia thn le3h*/
		EntryList *result_list;
		/*to epomeno stoixeio sto idio bucket t hash(gia overflow*/
		CacheElement* next_bucket_element;
		/*to epomeno stoixeio p mphke stn cache,me xronikh seira p mphkan*/
		CacheElement* next_cache_element;
		CacheElement* prev_cache_element;
		/*se poio bucket einai sto hash to item*/
		int bucket;
		/*se poia 8esh sto bucket akribws (mporei na exei overflow)*/
		int position_in_bucket;
	};

	struct Cache {
		/*pinakas m deiktes s oles ts 8eseis ts cache */
		CacheElement** cache_hash;
		/*most_recently_used_element*/
		CacheElement* MRU_element;
		/*less_recently_used_element*/
		CacheElement* LRU_element;
		/*Exoume ena maximum wste n arxisoume n bgazoume to LRU_element ktl*/
		int number_of_words_in_cache;
	};

#ifdef	__cplusplus
}
#endif

#endif	/* CACHE_STRUCTURES_H */

