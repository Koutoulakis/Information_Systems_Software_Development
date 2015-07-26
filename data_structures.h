/* 
 * File:   data_structures.h
 * Author: panagiotis
 *
 * Created on February 18, 2014, 5:48 PM
 */

#ifndef DATA_STRUCTURES_H
#define	DATA_STRUCTURES_H

#include "index.h"
#include "index_impl.h"

#define MAX_MATCH_DIST 3

#ifdef	__cplusplus
extern "C" {
#endif

	/*DHMIOURGOUME ena global struct pou periexei mesa 3 hash tables
	- 1 gia ta queries pou erxontai gia EditDistance search sta Documents
	- 1 gia Hamming kai 1 gia Exact antistoixa.
	- Epishs exoume Enan Pinaka MatrixForQIds,opou edw 8a kratame sthn 8esh 1 
	  to id tou prwtou query,stn 8esh 2 to id tou deuterou query ktl ktl ktl
	- Telos exoume Enan pinaka NxM me akeraious,mege8ous oso o ari8mos twn
	  queries kai ka8e 8esh tou exei mhdenika(0) pou ka8e ena apo auta
	  anaparistoun mia le3h apo to query, px an exoume to prwto query
	  me id=id1 ,kai le3eis q=(q1,q2,q3) tote sthn prwth 8esh tou MatrixForQids
	  8a exoume MatrixForQIds a; a[0]=id1 kai antistoixa
	  FinalMatchingQueryMatrix b; b[0][0]=0;b[0][1]=0;b[0][2]=0; kai h logikh
	  einai oti otan kanei match h le3h me to document trexoume ston
	  FinalMatchingQueryMatrix  kai apo 0 bazoume thn timh 1,an einai ola 1 ,
	  tote kanei match to id ,opte pame sto MatrixForQIds kai pairnoume to id
	  na to tupwsoume k tetoia. Auth einai h arxikh logikh apo ekei k pera
	  ta xwnoume k ola ston idio pinaka kai ta ids stn prwth 8esh,ta 0,1 stn
	  deuterh oti mas er8ei,to blepoume auto.
	 */
	
	/*Lista pou tha periexei ta q_ids pou ekanan match gia ena trexon doc*/
	typedef struct FinalQidList FinalQidList;
	typedef struct FinalQidNode FinalQidNode;
	struct FinalQidList {
		FinalQidNode * head_qid;
		FinalQidNode * last_qid;
	};
	struct FinalQidNode {
		int q_id;
		FinalQidNode * next_q_id;
	};

	/*lista pou tha periexei ta docs pou tha mas exoun stalei*/
	typedef struct DocumentListNode DocumentListNode;
	typedef struct DocumentList DocumentList;
	struct DocumentList {
		DocumentListNode * head;
		DocumentListNode * last_position;
	};
	struct DocumentListNode {
		Document doc;
		DocumentListNode* next_doc;
	};

	/*lista opou tha apothhkeuontai temp info gia ta queries pou mas stelnontai
	 * arxika mesw ths StartQuery (me eksairesh oti sto query exoun diagrafei
	 * tyxon duplicates)
	 */
	typedef struct QueryTempInfoList QueryTempInfoList;
	typedef struct QueryTempInfoNode QueryTempInfoNode;
	struct QueryTempInfoList {
		QueryTempInfoNode * head_node;
		QueryTempInfoNode * last_node;

	};
	struct QueryTempInfoNode {
		int q_id;
		int query_word_count;
		QueryTempInfoNode * next_node;

	};

	/*global struct me oles tis aparaithtes metavlhtes*/
	struct GlobalStruct {
		
		Entry ** HashTableEditDistance;
		Entry ** HashTableHammingDistance;
		Entry ** HashTableExactMatch;
		unsigned int NumberOfBuckets;
		
		/* Lista pou exei mesa to id,KAI tis 
		 * le3eis pou periexei to antistoixo query*/
		QueryTempInfoList * QueryTempInfoList; 
		int number_of_queries; /*plh9os apo queries p exoume*/
		
		/*einai oi lista me ola ta ids pou kanoun match sto document*/
		FinalQidList * CurrentMatchingQidsList;
		int ** FinalMatchingQueryMatrix;
                char * FinalMatchingQueryMatrix_to_match;
		
		DocumentList * DocList;

		Index* EditDistanceIndex;
		Index** HammingDistanceIndexes;
	} GlobalStruct;

#ifdef	__cplusplus
}
#endif

#endif	/* DATA_STRUCTURES_H */
