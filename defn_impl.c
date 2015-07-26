#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "defn.h"
#include "defn_impl.h"
#include "hashing_func.h"
#include "data_structures.h"

int word_id_counter = 0;

void CreateWord(const char* word_str, Word* word) {

	strcpy(word->string, word_str);
	word->w_id = word_id_counter; /*global metablhth */
	word->w_length = strlen(word_str);
	word_id_counter++;
	word->next_word = NULL;
}

////////////////////////////////////////////////////////////////////////////////

void CreateQuery(QueryID query_id,
				 const char* query_str,
				 enum MatchType match_type,
				 unsigned int match_dist,
				 Query* query) {
	int i = 0;
	int number_of_words = 0; /*poses le3eis einai to query*/
	int current_word = 0; /*h 8esh pou 8a mpei h ka8e le3h ston pinaka me le3eis tou Query*/
	char word[MAX_WORD_LENGTH + 1]; /*metablhth p xwraei 1 le3h*/
	(*query).q_id = query_id;
	(*query).match_type = match_type;
	(*query).match_dist = match_dist;

	/*upologizw to posa words exei to query*/
	while (query_str[i] != '\0') {
		if (query_str[i] < 'a' || query_str[i] > 'z') {
			i++;
			continue; /*den einai le3h sunexizoume*/
		} else {/*brhkame le3h*/
			int j = 0;
			while (query_str[i] >= 'a' && query_str[i] <= 'z') {
				word[j] = query_str[i]; /*antigrafoume ena ena ta grammata*/
				j++;
				i++;
			}
			word[j] = '\0';
			/*exoume grapsei thn le3h sto words
			kai pame na thn baloume sthn lista me
			ta words tou query*/
			CreateWord(word, &((*query).words_array[current_word]));
			current_word++;
			number_of_words++;
		}/*end of else{/*brhkame le3h*/
	}/*end of while(str[i]!='\0')*/
	(*query).number_of_words = number_of_words;
}

////////////////////////////////////////////////////////////////////////////////

int GetNumQueryWords(const struct Query* query) {
	return (*query).number_of_words;
}

////////////////////////////////////////////////////////////////////////////////

const struct Word* getQueryWord(unsigned int word, const Query* query) {
	return &((*query).words_array[word]);
}

////////////////////////////////////////////////////////////////////////////////

void CreateDocument(DocID doc_id, const char* doc_str, Document* document) {

	int i = 0;
	int number_of_words = 0;
	char temp_word[MAX_WORD_LENGTH + 1];

	(*document).d_id = doc_id;
	(*document).words_list = NULL;
	(*document).number_of_duplicates = 0;
	document->doc_string = malloc((strlen(doc_str) + 1) * sizeof (char));
	strcpy(document->doc_string, doc_str);
	return;
}

////////////////////////////////////////////////////////////////////////////////

int GetNumDocumentWords(const struct Document* doc) {
	return (*doc).number_of_words;
}

////////////////////////////////////////////////////////////////////////////////

const struct Word* getDocumentWord(unsigned int word, const Document* doc) {
	//den tsekarw an to int word einai moufa arithmos opote tote tha exw seg......//
	//h arithmhsh twn leksewn einai: 0h, 1h, 2h, 3h klp..(periexei kai to mhden)//
	Word* temp_ptr = doc->words_list;
	while (word-- > 0)
		temp_ptr = temp_ptr->next_word;
	return temp_ptr;
}

////////////////////////////////////////////////////////////////////////////////

int Equal(const Word* w1, const Word* w2) {
	if (strcmp(w1->string, w2->string) == 0)
		return 0; /*epistrefw mhden an einai isa*/
	return 1;
}

////////////////////////////////////////////////////////////////////////////////

int HammingDistance(const Word* w1, const Word* w2) {

	int s1len, s2len, dist = 0, i = 0;
	s1len = strlen(w1->string);
	s2len = strlen(w2->string);
	if (s1len != s2len)
		return -1;
	while (s1len-- > 0) {
		dist += ((w1->string[i]) != (w2->string[i]));
		i++;
	}
	return dist;
}

////////////////////////////////////////////////////////////////////////////////

int EditDistance(const Word* w1, const Word* w2) {
	unsigned int s1len, s2len, x, y, lastdiag, olddiag;
	s1len = strlen(w1->string);
	s2len = strlen(w2->string);
	unsigned int column[s1len + 1];
	for (y = 1; y <= s1len; y++)
		column[y] = y;
	for (x = 1; x <= s2len; x++) {
		column[0] = x;
		for (y = 1, lastdiag = x - 1; y <= s1len; y++) {
			olddiag = column[y];
			column[y] = 
					MIN3(column[y]+1, 
					column[y-1]+1,
					lastdiag+((w1->string)[y-1] == (w2->string)[x-1] ? 0 : 1));
			lastdiag = olddiag;
		}
	}
	return (column[s1len]);
}

////////////////////////////////////////////////////////////////////////////////

void RemoveDuplicates(struct Document* doc) {

	Word *temp_pre, *temp_curr; /*-*/
	unsigned int index;
	int number_of_duplicates;
	int i = 0;
	int number_of_words = 0;
	char temp_word[MAX_WORD_LENGTH + 1];
	Word* temp_current = NULL; // deixnei sto neo komvo pou molis ekana malloc
	Word* temp_previous = NULL; // deixnei ston prhgoumeno komvo pou prepei na
	// syndesw me to neo pou dhmiourghsa kathe fora.
	/*PARSAROUME TO STRING SE LE3EIS*/
	//edw twra tha prepei na kanoume parsing kai se kathe vhma
	//na apothhkeuoume th leksh pou brhkame
	while (doc->doc_string[i] != '\0') {
		if (doc->doc_string[i] < 'a' || doc->doc_string[i] > 'z') {
			i++;
			continue; /*den einai le3h sunexizoume*/
		} 
		else {/*brhkame le3h*/
			int j = 0;
			while (doc->doc_string[i] >= 'a' && doc->doc_string[i] <= 'z') {
				/*antigrafoume ena ena ta grammata*/
				temp_word[j] = doc->doc_string[i];
				j++;
				i++;
			}
			temp_word[j] = '\0';
			/*exoume grapsei thn le3h sto words
			kai pame na thn baloume sthn lista me
			ta words tou doc*/
			if (doc->words_list == NULL) {
				doc->words_list = malloc(sizeof (Word));
				CreateWord(temp_word, doc->words_list);
				temp_previous = doc->words_list;
				number_of_words++;
				continue; //den paei sta apo katw - anti gia if-else dhladh...
			}
			temp_current = malloc(sizeof (Word));
			CreateWord(temp_word, temp_current);
			temp_previous->next_word = temp_current;
			temp_previous = temp_current;
			temp_current = NULL;
			number_of_words++;
		}/*end of else{/*brhkame le3h*/
	}/*end of while(str[i]!='\0')*/
	doc->number_of_words = number_of_words;

	int now = doc->number_of_words;
	int remaining_words = now;
	temp_pre = doc->words_list;
	temp_curr = temp_pre;
	/*ftiaxnoume ena pinaka mege8ous iso me to mege8os tn le3ewn t doc mas*/
	bucket* bucket_array = calloc(now, sizeof (bucket));
	/*arxikopoioume tis metavlhtes pou tha xrhsimopoihsoume*/
	/*oso exoume lekseis gia na tsekaroume tha ginetai h olh diadikasia*/
	while (remaining_words > 0) {
		/*pairnoume tn 8esh t pinaka p prp na mpei h le3h!*/
		index = hash(temp_curr->string) % now;
		/*an einai adeio to bucket..*/
		if (bucket_array[index].word[0] == '\0') {
			/*..apla to xwnoume mesa!*/
			strcpy(bucket_array[index].word, temp_curr->string);
			temp_pre = temp_curr;
			temp_curr = temp_curr->next_word;
			--remaining_words;
			continue;
		}/*else, ean to bucket dn einai adeio..*/
		else {
		/*...shmainei oti,1) eite exei tn idia le3h mesa ara brhkame duplicate,
							  2) eite egine collision k prp n tn xwsoume tn le3h
									mesa stn lista t next_bucket*/
			/*ean idia le3h*/
			if (strcmp(bucket_array[index].word, temp_curr->string) == 0) {
				/*edw kanoume free ton kombo tou doc p eimaste!*/
				/*o prohgoumenos deixnei sto neo epomeno*/
				temp_pre->next_word = temp_curr->next_word;
				free(temp_curr);
				temp_curr = temp_pre->next_word;
				++(doc->number_of_duplicates);
				--(doc->number_of_words);
				--remaining_words;
				continue;
			}/*alliws (ean den einai idies le3eis) exoume collision!*/
			else {
				bucket * temp_buck;
				/* exei k h lista sigoura toul 1 le3h mesa,opte prp n
				tsekaroume k edw g duplicate*/
				bucket * temp_buckpre = &bucket_array[index];
				temp_buck = bucket_array[index].next_bucket;
				int found = 0;
				/*oso yparxoun akomh lekseis sth lista pou diasxizoume*/
				while (temp_buck != NULL) {
					/*elegxw ena ena ta buckets an einai idia le3h auth p
					eisagoume m authn pou einai mesa!*/
					if (strcmp(temp_buck->word, temp_curr->string) == 0) {
						/*ein idio brhkam duplicate*/
						/*edw kanoume free ton kombo tou doc p eimaste!*/
						/*an einai ola fysiologika, diagrafoume ton
						komvo kai synexizoume me epomenh leksh tou doc*/
						/*o prohgoumenos deixnei sto neo epomeno*/
						temp_pre->next_word = temp_curr->next_word; 
						free(temp_curr);
						temp_curr = temp_pre->next_word;
						++(doc->number_of_duplicates);
						--(doc->number_of_words);
						--remaining_words;
						found = 1;
						break; /*gia na vgei apo th while thn teleutaia*/
					}
					/* <else...> - den einai idia h leksh me auth tou trexontos
					bucket ths listas ara pame sto epomeno bucket*/
					temp_buckpre = temp_buck;
					temp_buck = temp_buck->next_bucket;
				}/*while(temp_buck!=NULL)*/
				/*an vrethhke h leksh ws duplicate phgaine sthn epomenh leksh*/
				if (found == 1)
					continue;
				/*an ftasei edw to programm tote exoume sarwsei olh th lista kai
				oles oi lekseis einai diaforetikes, ara xwnoume kai auth thn
				trexousa leksh sth lista epishs*/
				/*ftasame sto telos ts listas xwris duplicate
				prp na tn swsoume k autn tn le3h ara sto hash mas*/
				temp_buck = malloc(sizeof (bucket));
				temp_buckpre->next_bucket = temp_buck;
				strcpy(temp_buck->word, temp_curr->string);
				temp_buck->next_bucket = NULL;
				temp_pre = temp_curr;
				temp_curr = temp_curr->next_word;
				--remaining_words;
				continue;
				/*kai an dn exei to xwnoume sto telos ths*/
			}/*}else{/*den einai idies le3eis exoume collision!*/
		}/*end of }else{/*to bucket dn einai adeio*/
	}/*while(remaining_words>0){*/
	/*free everything from memory*/
	for (i = 0; i < now; i++) {
		bucket* curr_buck;
		bucket* temp_buck = NULL;
		curr_buck = bucket_array[i].next_bucket;
		while (curr_buck != NULL) {
			temp_buck = curr_buck;
			curr_buck = curr_buck->next_bucket;
			free(temp_buck);
		}
	}
	free(bucket_array);
}

unsigned long hash(const char *str) {

	unsigned long hash = 5381;
	int c;
	while (c = *str++)
		hash = ((hash << 5) + hash) + c;
	return hash;
	//	unsigned h = 0;
	//    while (*str)
	//        h = (unsigned int) (h * 101 + (unsigned char*) *(str++));
	//    return h;
};