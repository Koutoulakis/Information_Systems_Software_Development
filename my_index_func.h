/* 
 * File:   my_index_func.h
 * Author: paok k as mn gamhsw pote!
 *
 * Created on January 30, 2014, 4:09 PM
 */

#ifndef MY_INDEX_FUNC_H
#define	MY_INDEX_FUNC_H

#include "core.h"
#include "defn.h"


#ifdef	__cplusplus
extern "C" {
#endif

	int matchword(const Word* w1, const Word* w2, enum MatchType matchtype);
	int cmpfunc_VpCandidatePositions(const void* p1, const void* p2);
	int cmpfunc_SortMatrix(const void* p1, const void* p2);
	int unique_integer_generator(int max_matrix_size,
								 int max_int_to_be_generated,
								 int *matrix_vpcandidates);
	void counting_sort(void * p1, int n, int);
	int return_key(void * input_array, int position);
	void set_key(void * input_array, int i, void * output_array);
	void CopyPayload(Payload* from_payload, Payload* *to_payload);
	enum ErrorCode DestroyEntryIndex2(Index* index, int counter);

#ifdef	__cplusplus
}
#endif

#endif	/* MY_INDEX_FUNC_H */
