/*#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defn.h"
#include "defn_impl.h"
#include "core.h"
#include "index.h"
#include "index_impl.h"

#define NUMBER_OF_WORDS 1
#define NUMBER_OF_LOOKUPS 1
#define SHOW_ONLY_ERRORS 0 //1 for true statement, otherwise 0

#define CHECK(text, actual, expected)                         \
  if (actual != expected) {                                   \
    printf("%-30s: Failed | actual = %3d, expected = %3d\n",  \
           text,                                              \
           actual,                                            \
           expected);                                         \
  } else if(SHOW_ONLY_ERRORS != 1){                           \
    printf("%-30s: Success\n", text);                         \
  }

#define LOG(text, msg)  \
  printf("%-30s: %s\n", text, msg);


char *contents[] = {"wideman"};

extern Index* global_index;

void search(Word *search_word,
        Word words[],
        enum MatchType match_type,
        unsigned int match_dist,
        Word ***results,
        int *number_of_results);

int equalSets(Word* words[], int number_of_words, EntryList *entry_list);

void checkIndex(Word *search_word,
        enum MatchType type,
        unsigned int match_dist,
        EntryList *entry_list,
        Word dataset[]);

int main(int argc, char** argv) {

    int i;
    char logBuffer[31];
    Word words[NUMBER_OF_WORDS];

    CreateWord("basketball", &words[0]);

    Entry entry;
    CreateEntry(&words[0], &entry);
    CHECK("Check Entry", Equal(&words[0], entry.word), 0);

    DestroyEntry(&entry);
    EntryList entry_list;
    CreateEntryList(&entry_list);

    for (i = 0; i < NUMBER_OF_WORDS; ++i) {
        CreateWord(contents[i], &words[i]);
        CreateEntry(&words[i], &entry);
        AddEntry(&entry_list, &entry);
        DestroyEntry(&entry);
    }

    CHECK("Check EntryList Length", GetNumberOfEntries(&entry_list), NUMBER_OF_WORDS);

    Entry *entry_ptr = GetFirst(&entry_list);
    for (i = 0; i < NUMBER_OF_WORDS; ++i) {
        sprintf(logBuffer, "Check EntryList Entry: %d", i);
        if (entry_ptr != NULL && entry_ptr->word != NULL) {
            CHECK(logBuffer, Equal(&(words[i]), entry_ptr->word), 0);
        } else {
            LOG(logBuffer, "Should not be NULL");
        }
        entry_ptr = GetNext(&entry_list);
    }

    char *search_words[] = {"diocman"};
    int match_dist[] = {2, 2, 0, 3, 1, 1, 3};
    enum MatchType match_type[] = {MT_HAMMING_DIST, MT_EDIT_DIST, MT_EDIT_DIST, MT_EDIT_DIST, MT_EDIT_DIST, MT_EDIT_DIST, MT_EDIT_DIST};

    Word search_word;
    for (i = 0; i < NUMBER_OF_LOOKUPS; ++i) {
        CreateWord(search_words[i], &search_word);
        checkIndex(&search_word, match_type[i], match_dist[i], &entry_list, words);
    }
    DestroyEntryList(&entry_list);

    return (EXIT_SUCCESS);
}

// Return an array with the correct results

void search(Word *search_word,
        Word words[],
        enum MatchType match_type,
        unsigned int match_dist,
        Word ***results,
        int *number_of_results) {
    int i, res;
    int results_counter = 0;
    Word * current_results[NUMBER_OF_WORDS];
    for (i = 0; i < NUMBER_OF_WORDS; ++i) {
        int match_flag = 0;
        if (match_type == MT_EXACT_MATCH) {
            if (!Equal(search_word, &(words[i])))
                match_flag = 1;
        } else if (match_type == MT_HAMMING_DIST) {
			printf("exw hammingggggg\n");
            res = HammingDistance(search_word, &(words[i]));
			printf("res=!%d!\n",res);
			printf("match_dist=!%d!\n", match_dist);
            if (res >= 0 && res <= match_dist)
                match_flag = 1;
        } else if (match_type == MT_EDIT_DIST) {
            res = EditDistance(search_word, &(words[i]));
            if (res <= match_dist && res >= 0)
                match_flag = 1;
        }

        if (match_flag) {
            current_results[results_counter] = &(words[i]);
            ++results_counter;
        }
    }

    *results = malloc(results_counter * sizeof (Word*));
    memcpy(*results, current_results, results_counter * sizeof (Word*));
    *number_of_results = results_counter;
}

// Checks if two sets are equal

int equalSets(Word* words[], int number_of_words, EntryList *entry_list) {
    if (number_of_words != GetNumberOfEntries(entry_list)) {
        CHECK("Num of results", GetNumberOfEntries(entry_list), number_of_words);
        return 0;
    }

    int i, j;
    Entry* entry = GetFirst(entry_list);
	if (entry == NULL)
		printf("POULO KATEUTHEIAN, EINAI NULL (XWRIS APOTELESMATA)\n");
    for (i = 0; i < GetNumberOfEntries(entry_list); ++i) {
        for (j = 0; j < number_of_words; ++j) {
            if (!Equal(entry->word, words[j])) {
                break;
            }
        }
        if (j == number_of_words) {
            LOG("Search", "Not found!");
            return 0;
        }
        entry = GetNext(entry_list);
    }
    LOG("Search", "succeeded");
    return 1;
}

// Checks an Index

void checkIndex(Word *search_word,
        enum MatchType type,
        unsigned int match_dist,
        EntryList *entry_list,
        Word dataset[]) {

    Index index;
    BuildEntryIndex(entry_list, type, &index);

    EntryList results;
    CreateEntryList(&results);

    LookupEntryIndex(search_word, match_dist, &index, &results);

    Word **correct_results;
    int number_of_results;
    search(search_word, dataset, type, match_dist, &correct_results, &number_of_results);

    equalSets(correct_results, number_of_results, &results);

    free(correct_results);
    DestroyEntryList(&results);
    DestroyEntryIndex(&index);
}*/