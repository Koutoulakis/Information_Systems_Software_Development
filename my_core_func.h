/* 
 * File:   my_core_func.h
 * Author: panagiotis
 *
 * Created on February 28, 2014, 5:43 PM
 */

#ifndef MY_CORE_FUNC_H
#define	MY_CORE_FUNC_H

#ifdef	__cplusplus
extern "C" {
#endif

	void build_all_indexes();
	void Fill_FinalMatchingQueryMatrix(EntryList *);
	void FindMatchingQids(unsigned int *, QueryID**);
	void DestroyHeadDoc(void);
	void set_one(char * matrix, int i, int position_of_word_in_query);
	void FreeHash(Entry ** *HashTablePointer);
	void FreeGlobalVariables(void);
	/*Gia na kanw qsort ton MxN dusdiastato*/
	int compare_FinalMatchingQueryMatrix(const void *, const void *);
	int binary_search(int **, int, int, int);

#ifdef	__cplusplus
}
#endif

#endif	/* MY_CORE_FUNC_H */

