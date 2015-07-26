//#include <stdio.h>
//#include <stdlib.h>
//#include "core.h"
//#include "defn.h"
//#include "defn_impl.h"
//
//#define CHECK(text, actual, expected)                         \
//  if (actual != expected) {                                   \
//    printf("%-30s: Failed | actual = %3d, expected = %3d\n",  \
//           text,                                              \
//           actual,                                            \
//           expected);                                         \
//  } else {                                                    \
//    printf("Check %2s: Success\n", text);                     \
//  }
//
//int main(int argc, char** argv) {
//  // Create a query
//  Query query;
//  CreateQuery(0, "hello query", MT_HAMMING_DIST, 1, &query);
//
//  int numQueryWords = GetNumQueryWords(&query);
//  CHECK("Num Query Words", numQueryWords, 2);
//
//  // Create a document
//  Document doc;
//  CreateDocument(0, "hello document hellw querie hello", &doc);
//
//  int numDocWords = GetNumDocumentWords(&doc);
//  CHECK("Num Doc Words", numDocWords, 5);
//
//  // Remove duplicates
//  RemoveDuplicates(&doc);
//  numDocWords = GetNumDocumentWords(&doc);
//  CHECK("Num Doc Words No Duplicates", numDocWords, 4);
//
//  Word w1;
//  CreateWord("hello", &w1);
//  Word w2;
//  CreateWord("query", &w2);
//
//  // Exact distance
//  CHECK("eq(hello, hello)", Equal(&w1, getQueryWord(0, &query)), 0);
//  CHECK("eq(query, query)", Equal(&w2, getQueryWord(1, &query)), 0);
//  CHECK("eq(hello, query)", Equal(&w1, getQueryWord(1, &query)), 1);
//  CHECK("eq(hello, hello)", Equal(getDocumentWord(0, &doc), getQueryWord(0, &query)), 0);
//  CHECK("eq(hello, hellw)", Equal(getDocumentWord(0, &doc), getDocumentWord(2, &doc)), 1);
//  CHECK("eq(document, document)", Equal(getDocumentWord(1, &doc), getDocumentWord(1, &doc)), 0);
//
//  // Hamming distance
//  CHECK("ham(hello, hello)", HammingDistance(&w1, getQueryWord(0, &query)), 0);
//  CHECK("ham(hello, query)", HammingDistance(&w1, getQueryWord(1, &query)), 5);
//  CHECK("ham(hello, hellw)", HammingDistance(getDocumentWord(0, &doc), getDocumentWord(2, &doc)), 1);
//  CHECK("ham(hellw, hell0)", HammingDistance(getDocumentWord(2, &doc), getDocumentWord(0, &doc)), 1);
//  CHECK("ham(hello, document)", HammingDistance(getDocumentWord(0, &doc), getDocumentWord(1, &doc)), -1);
//  CHECK("ham(document, hello)", HammingDistance(getDocumentWord(1, &doc), getDocumentWord(0, &doc)), -1);
//
//  // Edit distance
//  CHECK("edit(hello, hello)", EditDistance(&w1, getQueryWord(0, &query)), 0);
//  CHECK("edit(hello, hellw)", EditDistance(getDocumentWord(0, &doc), getDocumentWord(2, &doc)), 1);
//  CHECK("edit(query, querie)", EditDistance(&w2, getDocumentWord(3, &doc)), 2);
//  CHECK("edit(querie, query)", EditDistance(getDocumentWord(3, &doc), &w2), 2);
//
//  return (EXIT_SUCCESS);
//}