#ifndef INDEX_IMPL_H
#define    INDEX_IMPL_H

#include "index.h"

#ifdef    __cplusplus
extern "C" {
#endif

	/*struct to opoio xrhsimopoioume gia na kanoume quicksort*/
	struct QSortStruct {
		/*kai na broume ton median,to radius ktl ktl*/
		int distance; /*to distance ths le3hs apo to vp mas*/
		Entry* entrylist_location; /*h 8esh ths le3hs mas sthn Entrylist*/
	};

	struct Index {
		Entry vp;
		int radius;
		enum MatchType match_type;
		/*tha thewroume oti phgainoun ola ta entries me distance mikrotero 'h
		 * iso apo to vp pou epilege*/
		Index * left;
		/*tha thewroume oti phgainoun ola ta entries me distance >
		 * apo to vp pou epilegetai se kathe gyro*/
		Index * right;
	};

	struct EntryList {
		Entry* head_entry;
		/*to current entry gia thn Entry* GetNext(const EntryList* entryList)*/
		Entry* curr_entry;
		Entry* last_entry;
		int number_of_entries;
	};

#ifdef    __cplusplus
}
#endif

#endif    /* INDEX_IMPL_H */