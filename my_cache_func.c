#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "index.h"
#include "index_impl.h"
#include "defn_impl.h"
#include "core.h"
#include "cache_structures.h"
#include "my_index_func.h"
#include "my_cache_func.h"


enum ErrorCode create_and_initialize_cache(Cache *Cache) {

	Cache->cache_hash = calloc(CACHE_SIZE, sizeof (CacheElement*));
	if (Cache->cache_hash == NULL)
		return EC_FAIL;
	Cache->LRU_element = NULL;
	Cache->MRU_element = NULL;
	Cache->number_of_words_in_cache = 0;
	return EC_SUCCESS;
}

void delete_cache_hash_and_list(Cache* cache) {
	/*Gia na kanw delete to cache hash,dedomenou oti ola ta stoixeia tou
	 * sundeontai me mia dipla sundedemenh lista,me kefalh to most recently
	 * used,kai telos to least recently used,arkei na sbhsoume authn tn lista
	 * k 8a xoume sbhsei ola ta stoixeia p exei mesa to cache_hash
	 */
	int i;
	int NumberOfWordsInCache = cache->number_of_words_in_cache;
	CacheElement * temp_pre;
	CacheElement * temp_cur;

	temp_pre = cache->MRU_element;
	temp_cur = cache->MRU_element;

	for (i = 0; i < NumberOfWordsInCache; i++) {
		temp_cur = temp_cur->next_cache_element;
		DestroyEntryList(temp_pre->result_list);
		free(temp_pre->result_list);
	//	free(&(temp_pre->word));
		free(temp_pre);
		temp_pre = temp_cur;
	}
	free(cache->cache_hash);
}


enum ErrorCode delete_cache(Cache* cache) {
	/*den exoume Entry hashtable gia na kalesoume tn destroy hashtable*/
	delete_cache_hash_and_list(cache); 
}


enum ErrorCode my_insert_to_cache(  Cache* cache_ptr, 
									EntryList * resultlist_ptr, 
									Word * word_ptr) {
	/*ean exei fullarei h cache mas...*/
	if (cache_ptr->number_of_words_in_cache == MAX_CACHE_ITEMS) {
		/*..tote prepei (prin ginei h opoiadhpote eisagwgh na kanoume delete
		 * to LRU element!*/
		delete_LRU_element(cache_ptr);
	}
	/*twra se auto to shmeio, sigoura yparxei xwros sthn cache mas, ara ginetai
	 * kanonika h eisagwgh pou theloume!*/
	/*vres to katallhlo bucket gia na ginei h eisagwgh*/
	unsigned int appropriate_bucket;
	appropriate_bucket = (unsigned int) hash((word_ptr->string)) % CACHE_SIZE;
	/*kane eisagwgh sto katallhlo bucket kai update thn LRU lista*/
	insert_and_update_LRU_list( cache_ptr,
								appropriate_bucket,
								resultlist_ptr,
								word_ptr);
	return EC_SUCCESS;
}

void delete_LRU_element(Cache* cache_ptr) {
	/*SYMVASH: TO MAX_ITEMS POU THA XWRAEI H CACHE DE THA EINAI  P O T E  KATW
	 APO TO CACHE_SIZE THS CACHE, ME ALLA LOGIA THA PREPEI NA GEMISEI H CACHE
	 * ME TOUL CACHE_SIZE TO PLHTHOS WORDS WSTE META NA PREPEI NA KANW REMOVE.
	 * KAI FYSIKA TO CACHE_SIZE DE THA PESEI POTE KATW APO 3 SIGOURA (KAI TO 2
	 * THA ME KALUPTE VEVAIA ALLA OK, AS TO THEWROUME ETSI)
	 */
	/*pointer sto LRU element*/
	CacheElement* LRU_element = cache_ptr->LRU_element;
	/*to bucket pou einai mesa to LRU element*/
	int bucket_of_LRU = LRU_element->bucket;
	int position_LRU_in_bucket = LRU_element->position_in_bucket;
	/*pointer sto prwto element tou bucket pou anhkei to LRU*/
	CacheElement* element_ptr = cache_ptr->cache_hash[bucket_of_LRU];
	int i;
	/*phgaine ton element_pointer na deixnei sto element arkivws prin to LRU
	  (thymizw oti to position_in_bucket einai 0 gia to 1o element tou bucket)
	 */
	/*an einai sthn prwth thesh tou bucket (apotelei mia ksexwristh periptwsh
	 giati den yparxei previous element sto bucket)
	 */
	if (position_LRU_in_bucket == 0) {
		/*vale ton cache pointer tou bucket autou na deixnei sto 2o element
		  ('h tespa sto NULL an den ypaxei 2o element)
		 */
		cache_ptr->cache_hash[bucket_of_LRU] = element_ptr->next_bucket_element;
	}
	else { /*alliws (den einai sthn prwth thesh) vazw ton pointer sto previous*/
		for (i = 0; i < position_LRU_in_bucket - 1; i++)
			element_ptr = element_ptr->next_bucket_element;
		/*kanw to previous na "prospernaei" to LRU element - pou einai akrivws
		 to epomeno,kai to vazw na deixnei epomenws sto methepomeno tou bucket
		 */
		element_ptr->next_bucket_element =
				element_ptr->next_bucket_element->next_bucket_element;
	}
	/*(se auto to shmeio to xw afairesei apo th lista tou bucket sto opoio
	 *anhkei.)*/
	/*Afou afairesa ena stoixeio apo to bucket, prepei na allaksw tis times
	  sta positions twn epomenwn apo auto pou molis afairesa stoixeiwn*/
	element_ptr = LRU_element->next_bucket_element;
	while (element_ptr != NULL) {
		element_ptr->position_in_bucket--;
		element_ptr = element_ptr->next_bucket_element;
	}
	cache_ptr->LRU_element = cache_ptr->LRU_element->prev_cache_element;
	cache_ptr->LRU_element->next_cache_element = NULL;
	
	/*kai twra kanw delete to Element*/
	DestroyEntryList(LRU_element->result_list);
	free(LRU_element);

	/*twra pou molis ekana kai free afairw 1 apo to plhthos twn leksewn*/
	cache_ptr->number_of_words_in_cache--;
}

void insert_and_update_LRU_list(Cache* cache_ptr,
								unsigned int appropriate_bucket,
								EntryList* resultlist_ptr,
								Word* word_ptr) {
	/*dhmiourgw se mia temp_metavlhth to neo komvo/neo element*/
	CacheElement* temp_element_ptr = malloc(sizeof (CacheElement));
	temp_element_ptr->next_bucket_element = NULL;
	temp_element_ptr->bucket = appropriate_bucket;
	temp_element_ptr->position_in_bucket = 0;
	temp_element_ptr->next_cache_element = NULL;
	temp_element_ptr->prev_cache_element = NULL;
	/*(ftiaxnw to word tou)*/
	temp_element_ptr->word.w_id = word_ptr->w_id;
	temp_element_ptr->word.w_length = word_ptr->w_length;
	temp_element_ptr->word.next_word = NULL;
	strcpy(temp_element_ptr->word.string, word_ptr->string);
	/*vazw ton entrylist pointer tou element na deixnei sto ekswteriko
	 *result (den ksanakanw malloc kai to copiarw ekei mesa klp). Auto
	 * douleuei giati to ekswteriko resultlist to kanw malloc (ara den einai
	 * topikh metavlhth) kai den to kanw free pote kai ara synexizei na
	 * na einai egkyrh h prosvash tou!
	 */
	temp_element_ptr->result_list = resultlist_ptr;
	
	/*ean to bucket einai teleiws adeio*/
	if (cache_ptr->cache_hash[appropriate_bucket] == NULL)
		cache_ptr->cache_hash[appropriate_bucket] = temp_element_ptr;
	else { /*phgaine sto teleutaio element*/
		CacheElement* cur_element_ptr=cache_ptr->cache_hash[appropriate_bucket];
		int position=1;
		while (cur_element_ptr->next_bucket_element != NULL){
			position++;
			cur_element_ptr = cur_element_ptr->next_bucket_element;
		}
		temp_element_ptr->position_in_bucket = position;
		cur_element_ptr->next_bucket_element = temp_element_ptr;
	}
	/*se auto to shmeio exw valei to neo komvo/element sto hash table kai mou
	 menei na kanw update thn LRU lista, kanontas MRU to neo auto element.
	 */
	if (cache_ptr->number_of_words_in_cache == 0) {
		cache_ptr->MRU_element = temp_element_ptr;
		cache_ptr->LRU_element = temp_element_ptr;
	} else {
		cache_ptr->MRU_element->prev_cache_element = temp_element_ptr;
		temp_element_ptr->next_cache_element = cache_ptr->MRU_element;
		cache_ptr->MRU_element = temp_element_ptr;
	}
	cache_ptr->number_of_words_in_cache++;
}

enum ErrorCode my_search_cache(Cache* cache_ptr, 
							   Word * word_ptr,
							   EntryList* *resultlist_ptr) {
	/*vres to katallhlo bucket gia na ginei h eisagwgh*/
	unsigned int appropriate_bucket;
	appropriate_bucket = (unsigned int) hash((word_ptr->string)) % CACHE_SIZE;
	/*an einai teleiws adeio to bucket apeutheias return EC_FAIL*/
	if (cache_ptr->cache_hash[appropriate_bucket] == NULL)
		return EC_FAIL;
	/*(alliws) skanarw ola ta elements tou bucket psaxnontas gia idia leksh*/
	CacheElement* cur_element_ptr = cache_ptr->cache_hash[appropriate_bucket];
	while (cur_element_ptr != NULL) {
		/*an kanei match h leksh mou me to curr element tote search succeeded*/
		if ( strcmp(cur_element_ptr->word.string, word_ptr->string) == 0) {
			/*steile "apeksw" th resultlist*/
			*resultlist_ptr = cur_element_ptr->result_list;
			
			/*kai twra prepei na kanw update sthn lista kai na to valw ws MRU
			 (an einai hdh MRU element de xreiazetai na kanw tpt)
			 */
			if(cur_element_ptr != cache_ptr->MRU_element) {
				/*ean einai to LRU element*/
				if (cur_element_ptr == cache_ptr->LRU_element) {
					cur_element_ptr->prev_cache_element->next_cache_element = 
																		   NULL;
					cache_ptr->LRU_element=cur_element_ptr->prev_cache_element;
				}
				/*alliws(ean dld den einai oute LRU - kai oute MRU fysika)*/
				else {
					cur_element_ptr->prev_cache_element->next_cache_element = 
											cur_element_ptr->next_cache_element;
					cur_element_ptr->next_cache_element->prev_cache_element = 
											cur_element_ptr->prev_cache_element;
				}
				cur_element_ptr->next_cache_element = cache_ptr->MRU_element;
				cur_element_ptr->prev_cache_element = NULL;
				cache_ptr->MRU_element->prev_cache_element = cur_element_ptr;
				cache_ptr->MRU_element = cur_element_ptr;
			}
			return EC_SUCCESS;
		}
		cur_element_ptr = cur_element_ptr->next_bucket_element;
	}
	return EC_FAIL;
}