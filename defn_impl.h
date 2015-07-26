#ifndef DEFN_IMPL_H
#define	DEFN_IMPL_H

#include "core.h"
#include "defn.h"

#ifdef	__cplusplus
extern "C" {
#endif

#define MIN3(a, b, c) ((a) < (b)? ((a) < (c)? (a): (c)): ((b) < (c)? (b): (c)))

	extern int word_id_counter;
	typedef struct Hash_bucket bucket;

	struct Word {
		char string[MAX_WORD_LENGTH + 1]; /*h leksh pou anaferomaste*/
		unsigned int w_id; /*to id tou sygkekrimenou word (to vala proairetika,
						 * isws xreiastei sto mellon)*/
		int w_length; /*word length*/
		Word* next_word; /*enas pointer se next word*/
	};

	struct Query {
		QueryID q_id; /*to id tou query*/
		/*mia lista apo ta words pou apoteleitai*/
		Word words_array[MAX_QUERY_WORDS];
		/*to plhthos twn leksewn pou apoteleitai, gia na mhn
		tsekarw kathe fora th words_list poso megalh einai
		(an autos o elegxos - tou poso megalh einai h
		words_list,ara poses lekseis exei to query - ginetai
		syxna tote kerdizei xrono o int "number_of_words")*/
		int number_of_words;
		/*ti typos match antistoixei se auto to query*/
		enum MatchType match_type;
		/*an typos match=Edit,Hamming, tote edw dhlwnetai to bound*/
		unsigned int match_dist;
	};

	struct Document {
		Word* words_list;
		char* doc_string; /*OLO to document*/
		DocID d_id; /*to id tou document*/
		int number_of_words;
		int number_of_duplicates;
	};

	struct Hash_bucket {
		char word[MAX_WORD_LENGTH + 1];
		bucket* next_bucket;
	};

#ifdef	__cplusplus
}
#endif

#endif	/* DEFN_IMPL_H */