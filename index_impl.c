#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>

#include "core.h"
#include "index.h"
#include "defn_impl.h"
#include "index_impl.h"
#include "my_index_func.h"
#include "threads.h"

/*----------------------------CreatePayloadData-------------------------------*/
enum ErrorCode CreatePayloadData(Payload_data* *array_ptr,
								 int number_of_thresholds) {
	int i;
	*array_ptr = malloc(number_of_thresholds * sizeof (Payload_data));
	if (*array_ptr == NULL)
		return EC_FAIL;
	for (i = 0; i < number_of_thresholds; i++) {
		(*array_ptr)[i].q_id = 0;
		(*array_ptr)[i].word_position = 0;
		(*array_ptr)[i].next = NULL;
	}
	return EC_SUCCESS;
}

/*------------------------------CreatePayload---------------------------------*/
enum ErrorCode CreatePayload(Payload* *payload, int number_of_thresholds) {

	*payload = malloc(sizeof (Payload));
	if (*payload == NULL)
		return EC_FAIL;
	(*payload)->data_array_length = number_of_thresholds;
	(*payload)->found_dist = -1;
	enum ErrorCode error = CreatePayloadData(&((*payload)->payload_data_array),
											number_of_thresholds);
	if (error != EC_SUCCESS)
		return EC_FAIL;
	return EC_SUCCESS;
}

void CopyPayload(Payload* from_payload, Payload* *to_payload) {

	int i;
	Payload_data* temp1; /*periexei to from_payload_data*/
	Payload_data* temp2; /*periexei to to_payload_data*/
	(*to_payload)->data_array_length = from_payload->data_array_length;
	for (i = 0; i < from_payload->data_array_length; i++) {
		(*to_payload)->payload_data_array[i].q_id =
				from_payload->payload_data_array[i].q_id;
		(*to_payload)->payload_data_array[i].word_position =
				from_payload->payload_data_array[i].word_position;
		(*to_payload)->payload_data_array[i].next = NULL;
		temp1 = from_payload->payload_data_array[i].next;
		temp2 = &((*to_payload)->payload_data_array[i]);
		while (temp1 != NULL) {
			temp2->next = malloc(sizeof (Payload_data));
			temp2 = temp2->next;
			temp2->q_id = temp1->q_id;
			temp2->word_position = temp1->word_position;
			temp2->next = NULL;
			temp1 = temp1->next;
		}
	}
}

/*-------------------------------CreateEntry----------------------------------*/
enum ErrorCode CreateEntry(const Word* word, Entry* entry) {

	entry->payload = NULL;
	entry->next_entry = NULL;
	if ((entry->word = malloc(sizeof (Word))) == NULL)
		return EC_FAIL;
	entry->word->w_length = word->w_length;
	entry->word->next_word = NULL;
	entry->word->w_id = word->w_id;
	strcpy(entry->word->string, word->string);
	return EC_SUCCESS;
}

/*--------------------------------AddEntry------------------------------------*/
enum ErrorCode AddEntry(EntryList* entryList, const Entry* entry) {

	int i = 0;
	Entry* temp_entry;
	Payload_data* temp1;
	if ((temp_entry = malloc(sizeof (Entry))) == NULL) {
		printf("COULN'T ALLOCATE MEMORY (MALLOC)....\n");
		return EC_FAIL;
	}
	temp_entry->next_entry = NULL;
	//DHMIOURGW ARXIKA TO PAYLOAD OPWS PREPEI*/
	int wannabe_length = entry->payload->data_array_length;
	CreatePayload(&(temp_entry->payload), wannabe_length);
	temp_entry->payload->found_dist = entry->payload->found_dist;
	temp1 = entry->payload->payload_data_array;
	
	CopyPayload(entry->payload, &(temp_entry->payload));

	if ((temp_entry->word = malloc(sizeof (Word))) == NULL) {
		printf("COULN'T ALLOCATE MEMORY (MALLOC)....\n");
		return EC_FAIL;
	}
	temp_entry->word->w_length = entry->word->w_length;
	temp_entry->word->next_word = NULL;
	temp_entry->word->w_id = entry->word->w_id;
	strcpy(temp_entry->word->string, entry->word->string);

	if (entryList->head_entry == NULL) {
		entryList->head_entry = temp_entry;
		entryList->curr_entry = temp_entry;
		entryList->last_entry = temp_entry;
	}
	else {
		entryList->last_entry->next_entry = temp_entry;
		entryList->last_entry = temp_entry;
	}
	entryList->number_of_entries++;
	temp_entry = NULL;
	return EC_SUCCESS;
}

void DestroyPayload(Payload* payload) {

	int i;
	Payload_data * payload_data_pre;
	Payload_data * payload_data_cur;

	for (i = 0; i < payload->data_array_length; i++) {
		payload_data_cur = &(payload->payload_data_array[i]);
		payload_data_cur = payload_data_cur->next;
		while (payload_data_cur != NULL) {
			payload_data_pre = payload_data_cur;
			payload_data_cur = payload_data_cur->next;
			payload_data_pre->q_id = 0;
			payload_data_pre->word_position = 0;
			payload_data_pre->next = NULL;
			free(payload_data_pre);
			payload_data_pre = NULL;
		}
	}
	payload_data_cur = (payload->payload_data_array);
	free(payload_data_cur);
	free(payload);
}

/*-------------------------------DestroyEntry---------------------------------*/
enum ErrorCode DestroyEntry(Entry *entry) {
	/* prepei na sigoureutoume oti h kathe Entry tha exei
	 * MONO mia leksh (ara to word->next_word tha einai null, kai ara de tha
	 * thelei free)
	 */
	/*PROSOXH: H SYNARTHSH THEWREITAI STH MAIN POU MAS EXOUN DWSEI OTI PAIRNEI
	 * WS PARAMETRO ENA &entry. Opote auto pou mporw mono na kanw einai na kanw
	 * apla free ta periexomena tou entry. DE MPORW NA KANW free(entry) - tha
	 * petaksei seg.. EPOMENWS, an tyxon exw enan pointer se Entry (Entry*ptr)
	 * kai thelw na kanw free ton deikth, to mono pou mporw na kanw gia na
	 * xrhsimopoihsw th DestroyEntry einai na kanw DestroyEntry(ptr); kai meta
	 * apo katw free(ptr); wste na apodesmeusw kai to malloc pou xe ginei ston
	 * ptr - malloc(sizeof(Entry));
	 */
	if (entry->payload != NULL)
		DestroyPayload((entry->payload));
	free(entry->word);
	entry->word = NULL;
	return EC_SUCCESS;
}

/*-----------------------------CreateEntryList--------------------------------*/
enum ErrorCode CreateEntryList(EntryList* entryList) {

	entryList->head_entry = NULL;
	entryList->curr_entry = NULL;
	entryList->last_entry = NULL;
	entryList->number_of_entries = 0;
	return EC_SUCCESS;
}

/*--------------------------------GetFirst------------------------------------*/
Entry* GetFirst(EntryList* entryList) {

	if (entryList->head_entry != NULL)
		entryList->curr_entry = entryList->head_entry->next_entry;
	return entryList->head_entry;
}

/*---------------------------------GetNext------------------------------------*/
Entry* GetNext(EntryList* entryList) {

	Entry * temp_cur = entryList->curr_entry;
	if (temp_cur != NULL)
		entryList->curr_entry = temp_cur->next_entry;
	return temp_cur;
}

/*----------------------------DestroyEntryList--------------------------------*/
enum ErrorCode DestroyEntryList(EntryList* entryList) {

	Entry * curr;
	Entry * next_curr; /*o epomenos ap ton current kombo mas*/
	curr = entryList->head_entry;
	while (curr != NULL) {
		next_curr = curr->next_entry;
		DestroyEntry(curr); /*auto eixame ksexasei kai eixe kapoio memory leak*/
		free(curr);
		curr = next_curr;
	}
	return EC_SUCCESS;
}

/*---------------------------GetNumberOfEntries-------------------------------*/
unsigned int GetNumberOfEntries(const EntryList* entryList) {
	return entryList->number_of_entries;
}

/*---------------------------BuildEntryIndex-------------------------------*/
enum ErrorCode BuildEntryIndex(const EntryList* entries,
							   enum MatchType type,
							   Index* index) {

	/*an exoume ftasei sthn kenh entrylist, den uparxoun alla entries gia
	 * paidia sto dentro,opote ola ta stoixeia exoun eurethriastei*/
	if (entries->head_entry == NULL)
		return;
	/*arxikopoihsh twn timwn tou komvou*/
	index->radius = 0;
	index->match_type = type;
	index->left = NULL;
	index->right = NULL;

	if (entries->head_entry->next_entry == NULL) {
		/*exoume 1 mono entry opote to kanoume vp kateutheian k kanoume return*/
		CreateEntry(entries->head_entry->word, &(index->vp));
		CreatePayload(&(index->vp.payload), 3);
		CopyPayload(entries->head_entry->payload, &(index->vp.payload));
		return;
	}
	unsigned int noe = GetNumberOfEntries(entries);
	float * matrix_of_mesh_timh; /*mesh timh ka8e vpcandidate*/
	float * matrix_of_typikh_apoklish; /*typikh apoklish ka8e vpcandidate*/
	float max_typikh; /*h max typikh apoklish s ola ta vpcandidates*/
	int * matrix_of_vpcandidates; /* ta vpcandidates*/
	Entry ** Vpcandidates; /*Pinakas me deiktes se entries pou deixnoun*/
	/*ta  vpcandidate entries mesa sthn Entrylist 
	 * mas (entries auth t fora px)*/
	Entry ** Sample; /*Pinakas me deiktes se entries pou 8a paroume san deigma*/
	/*apo to entrylist mas gia na mas boh8hsoun
	 *  na epile3oume vp*/
	int meter = 0; /*ena counteraki gia epanalhpthkes diadikasies*/
	int thesh_vp; /*h 8esh tou oristikou vp mesa sthn entrylist mas*/
	int i, j; /*ta palia paradosiaka*/
	int N; /*o ari8mos twn upopshfiwn vp shmeiwn*/
	/*10% tou plh8usmou ein ikanopoihtiko deigma*/
	int ammount_of_sample = (int) ((10 * noe) / 100) + 1;
	int number_of_entries;
	Entry * temp; /*deikths se entry*/
	EntryList CloseNeighbour_list; /*lista me t kontinotera entries sto vp*/
	EntryList DistantNeighbout_list; /*antistoixa me ta makrunotera*/
	/*o parakatw einai enas dynamically allocated matrix ths domhs QSortStruct,
	 * pou einai mia domh pou periexei enan Entry*, to entrylist_location) pou
	 * krataei thn antistoixh 8esh sthn opoia brisketai ena entry sthn
	 *  entrylist pou prospa8oyme na sortaroume,kai enan int, to distance pou
	 * einai h MatchType distance autou tou entry apo to vp mas.
	 */
	QSortStruct * sort_matrix;
	/*painroume posa 8a einai ta vp candidates*/
	if (noe < 5)
		N = rand() % noe + 1;
	else
		N = rand() % 5 + 1;
	/*periexei tis N int theseis pou vriskontai sthn entrylist ta Vpcandidates*/
	matrix_of_vpcandidates = calloc(N, sizeof (int));
	/*mesh timh t antistoixou vpcandidate*/
	matrix_of_mesh_timh = calloc(N, sizeof (float));
	/*tupikh apoklish t vpcandidate*/
	matrix_of_typikh_apoklish = calloc(N, sizeof (float));
	/*deixnei t candidate mesa st entrylist*/
	Vpcandidates = calloc(N, sizeof (Entry *));
	/*antistoixa g ta Samples*/
	Sample = calloc(ammount_of_sample, sizeof (Entry *));
	/*bazoume unique akeraious ston pinaka me ta upopshfia vps*/
	unique_integer_generator(N, noe, matrix_of_vpcandidates);
	/*EDW gemisoume ton pinaka me random Samples (dn 3eroume an einai unique*/
	/*an ta 8eloume unique mporoume na kanoume tn diadikasia p akolou8hsame*/
	/*me to matrix_of_vpcandidates, alla prp n kanoume allocate allon enan*/
	/*integer pinaka mege8ous |Sample| */
	meter = 0;
	temp = entries->head_entry;
	for (i = 0; i < ammount_of_sample; i++) {
		int sample_entry = rand() % noe; /*8esh tou tuxaiou entry*/
		while (temp != NULL) {
			if (meter == sample_entry) {
				/*o deikths ts i 8eshs tou Sample deixnei sthn 8esh ths
				 * entylist pou exei ena entry p 8a paroume sa sample
				 */
				Sample[i] = temp;
				temp = entries->head_entry; /*deixnei pali sthn arxh*/
				meter = 0;
				break; /*gia na bgei ap thn while*/
			}/*if (meter == sample_entry) {*/
			meter++;
			temp = temp->next_entry;
			//getchar();
		}/*while (temp!=NULL) {*/

	}/*end of for( i=0 ; i < ammount_of_sample ; i++ ){*/

	/** EDW ftiaxnw ton pinaka Vpcandidates,na deixnei sta entries tou entrylist
	 * twn opoiwn oi 8eseis, briskontai ston int pinaka matrix_of_vpcandidates
	 * kai einai unique h ka8e mia apo autes.
	 */
	qsort(matrix_of_vpcandidates, N, sizeof (int), cmpfunc_VpCandidatePositions);
	meter = 0;
	temp = entries->head_entry;
	for (i = 0; i < N; i++) {
		int cur_position = matrix_of_vpcandidates[i] - meter;
		while (cur_position-- != 0) {
			temp = temp->next_entry;
			meter++;
		}
		Vpcandidates[i] = temp;
	}/*for(i=0; i<=N ; i++) {*/
	/*EXW ta Vpcandidates,to Sample mou kai twra paw na upologisw mesh timh,*/
	/*tupikh apoklish wste na epile3w to VP mou*/

	/*Briskw mesh timH*/
	for (i = 0; i < N; i++) {
		for (j = 0; j < ammount_of_sample; j++)
			matrix_of_mesh_timh[i] += matchword(Vpcandidates[i]->word,
												Sample[j]->word,
												type);
		/*to diairw dia tou mege8ous tou deigmatos*/
		matrix_of_mesh_timh[i] = (matrix_of_mesh_timh[i]
				/ (ammount_of_sample + 1));
	}
	/*BRiskw tupikh apoklish*/
	for (i = 0; i < N; i++) {
		for (j = 0; j < ammount_of_sample; j++)
			matrix_of_typikh_apoklish[i] += pow(matchword(Vpcandidates[i]->word,
														Sample[j]->word,
														type)
												- matrix_of_mesh_timh[i], 2);
		/*to diairw dia tou mege8ous tou deigmatos*/
		matrix_of_typikh_apoklish[i] = (matrix_of_typikh_apoklish[i])
				/ (ammount_of_sample + 1);
		/*briskw thn riza tou S^2*/
		matrix_of_typikh_apoklish[i] = sqrt(matrix_of_typikh_apoklish[i]);

	}
	/*BRiskw thn MAX tupikh apoklish*/
	/*Milame gia maximum 5 stoixeia opote de xreiazetai sort,
	 * arkei h aplh sygkrish kathe fora*/
	max_typikh = matrix_of_typikh_apoklish[0];
	thesh_vp = 0; /*h 8esh tou oristikou vp mesa sthn entrylist mas*/
	for (i = 0; i < N; i++) {
		if (max_typikh < matrix_of_typikh_apoklish[i]) {
			max_typikh = matrix_of_typikh_apoklish[i];
			thesh_vp = i;
		}
	}
	/*Twra exoume brei to Vp point pou 8a paroume kai einai to
	 * Vpcandidate[thesh_vp] to Vp cadidate exei antistoixia se 8esh me tn
	 * pinaka matrix_of_typikh_apoklish kai ton matrix_of_mesh_timh ,ktl ktl
	 */
	CreateEntry((Vpcandidates[thesh_vp])->word, &(index->vp));

	CreatePayload(&(index->vp.payload), 3);
	CopyPayload(Vpcandidates[thesh_vp]->payload, &(index->vp.payload));
	/*Twra pame Na upologisoume thn aktina! H aktina- radius,einai o median pou
	 * 8a paroume meta apo to sortarisma twn entries ws pros thn apostash tous
	 * (analoga to matchtype),apo to vp pou exoume brei. Gia na upologisoume
	 * loipon authn tn aktina, prepei na sortaroume me kapoion
	 * tropo ta entries mas.
	 * Auto pou kanoume einai:
	 * xrhsimopoioume enan pinaka domwn(sort_matrix) mege8ous oso to entrylist,
	 * o opoios einai mia domh pou periexei enan Entry*, to entrylist_location,
	 * pou krataei thn antistoixh 8esh sthn opoia brisketai ena entry sthn
	 * entrylist pou prospa8oyme na sortaroume,kai enan int, to distance, pou
	 * einai h MatchType distance autou tou entry apo to vp mas.
	 * Skopos mas einai na kalesoume thn quicksort ws pros ta distances
	 * na sortaristoun apo to mikrotero pros to megalutero distance,na paroume
	 * ton median autwn. Mazi me ta distances omws, exoume sortarei paketaki kai
	 * ta antistoixa entries pou 8eloume na baloume sto de3i 'h sto
	 * aristero paidi tou VP mas.
	 */
	/*kanoume allocate tn pinaka*/
	/*to valgrind edw leei oti me calloce'd noe theseis, yparxei "provlhma",
	 * enw me noe+1 desmeumenes oxi... giati?????
	 */
	sort_matrix = calloc(noe, sizeof (QSortStruct));
	temp = entries->head_entry;
	for (meter = 0; meter < noe; meter++) {
		/*bazw to distance ths meter le3hs apo to vp,analoga me to type*/
		sort_matrix[meter].distance = matchword(index->vp.word,temp->word,type);
		/*swzw th dieuthynsh tou antistoixou entry me toso distance*/
		sort_matrix[meter].entrylist_location = temp;
		temp = temp->next_entry;
	}
	/*twra exoume enan sort_matrix gemato me ts apostaseis twn le3ewn tou
	 * entrylist apo to vp mas,kai apo to antistoixo entry. Kai twra kanoume
	 * sort ws pros to distance wste na sortaristoun apo thn mikroterh sth
	 * megaluterh distance.
	 */
	counting_sort(sort_matrix, noe, MAX_WORD_LENGTH + 1);
	/*twra 8a exoume to sort_matrix sortarismeno apo to mikrotero distance sto
	 * megalutero kai o median einai sthn 8esh (int)(noe/2)
	 */
	unsigned int median_element = (int) (noe / 2);
	/*to distance tou median element einai auto pou ginetai to radius tou vp*/
	int median_distance = sort_matrix[median_element].distance;
	index->radius = sort_matrix[median_element].distance;
	/*TWRA ftiaxnoume 2 listes,h mia me tous kontinoterous geitones tou vp
	 *k h allh me tous makrunoterous basei ts aktinas,kai kanoume anadromika
	 *thn idia diadikasia mexri na xoun eurethriastei ola ta entries
	 */
	CreateEntryList(&CloseNeighbour_list);
	CreateEntryList(&DistantNeighbout_list);
	meter = 0;
	temp = entries->head_entry;
	/*gia ola ta stoixeia me mikrotero distance apo to radius (pou einai to
	 * distance tou median_element)
	 */
	for (i = 0; i <= median_element; i++) {
		/*an to distance einai 0 shmainei leksh prin 'h meta to vp einai
		 * diplotyph me to vp, ara thn prospername*/
		if (sort_matrix[i].distance == 0)
			continue;
		AddEntry(&CloseNeighbour_list, sort_matrix[i].entrylist_location);
	}
	/*gia ola ta stoixeia me distance iso me to radius (an yparxoun alla tetoia
	 * ektos apo to median_element) prepei na ta valoume kai auta mesa sthn
	 * CloseNeighbour.
	 */
	for (; i < noe && sort_matrix[i].distance == median_distance; i++)
		AddEntry(&CloseNeighbour_list, sort_matrix[i].entrylist_location);
	/*TWRA gemizw thn DISTANTNeigbourlist me thn idia logikh*/
	for (; i < noe; i++) {
		if (sort_matrix[i].distance == 0)
			continue;
		AddEntry(&DistantNeighbout_list, sort_matrix[i].entrylist_location);
	}
	/*kanoume free tous ynamically allocated pinakes pou xrhsimopoihsame*/
	free(Sample);
	free(Vpcandidates);
	free(sort_matrix);
	free(matrix_of_mesh_timh);
	free(matrix_of_typikh_apoklish);
	free(matrix_of_vpcandidates);
	/*kanoume anadromika ta idia*/
	number_of_entries = GetNumberOfEntries(&CloseNeighbour_list);
	if (number_of_entries != 0) {
		if ((index->left = calloc(1, sizeof (Index))) == NULL) {
			printf("COULN'T ALLOCATE MEMORY (MALLOC)....\n");
			return EC_FAIL;
		}
		BuildEntryIndex(&CloseNeighbour_list,type,index->left);
	}
	DestroyEntryList(&CloseNeighbour_list);
	number_of_entries = GetNumberOfEntries(&DistantNeighbout_list);
	if (number_of_entries != 0) {
		if ((index->right = calloc(1, sizeof (Index))) == NULL) {
			printf("COULN'T ALLOCATE MEMORY (MALLOC)....\n");
			return EC_FAIL;
		}
		BuildEntryIndex(&DistantNeighbout_list, type, index->right);
	}
	DestroyEntryList(&DistantNeighbout_list);
}

/*---------------------------DestroyEntryIndex--------------------------------*/
enum ErrorCode DestroyEntryIndex(Index* index) {
	DestroyEntryIndex2(index, 0);
}


//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

/*
enum ErrorCode PrintIndex(Index* index) {

	if(index == NULL)
		return EC_FAIL;
	printf("Trexon komvos: !%s!", index->vp.word->string);
	printf("Mpainw se aristero paidi...\n");
	PrintIndex(index->left);
	printf("Vghka apo aristero, Mpainw se deksi paidi...\n");
	PrintIndex(index->right);
	printf("Vghka kai apo deksi, epistrefw ena epipedo panw...\n");

}
 */
//////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

enum ErrorCode DestroyEntryIndex2(Index* index, int counter) {

	/*kai edw (opws paromoia ginetai me thn DestroyEntry) thewrw pws mou dinei
	 san orisma ena Index index to opoio to pernaei ws DestroyEntryIndex(&index)
	 * opote egw kanw kanonika free KAI tous pointers gia ta apokatw epipeda
	 * (count != 0) enw gia th riza den kanw free kapoion pointer thewrwntas oti
	 * den exw pointer na deixnei se auton (gt eipame oti mas pernaei san orisma
	 * th dieuthynsh enos struct Index kai oxi pointer se auto.
	 * EPOMENWS, an kapoia stigmh egw exw pointer sth riza tou dentrou kai
	 * kalesw th DestroyEntryIndex me orisma ton pointer, tote prepei na kserw
	 * oti de tha ginei free se auton ton pointer apo th sygkekrimenh synarthsh.
	 * Tha prepei na to kanw manually apeksw, dld:
	 * Index * index_ptr = &(index_root_struct);
	 * DestroyEntryIndex(index_ptr);
	 * free(index_ptr);
	 */
	if (index == NULL)
		return EC_FAIL;
	DestroyEntryIndex2(index->left, counter + 1);
	DestroyEntryIndex2(index->right, counter + 1);
	if (counter != 0) {
		DestroyEntry(&(index->vp));
		free(index);
	} else {
		DestroyEntry(&(index->vp));
		index->right = NULL;
		index->left = NULL;
		index = NULL;
	}
	return EC_SUCCESS;
}

/*----------------------------LookupEntryIndex--------------------------------*/
enum ErrorCode LookupEntryIndex(const Word* word,
								int threshold,
								const Index* index,
								EntryList* result) {
	/*h results entrylist apla thelei gemisma - einai dhmioughmenh hdh.*/
	enum MatchType match_type = index->match_type;
	int match_distance = matchword(word, index->vp.word, match_type);
	/* if d(Q,vp) <= threshold */
	if (match_distance <= threshold) {
		pthread_mutex_lock(&mtx_look_up);
		index->vp.payload->found_dist = match_distance;
		AddEntry(result, &(index->vp));
		index->vp.payload->found_dist = -1;
		pthread_mutex_unlock(&mtx_look_up);
	}
	/*if d(Q,vp) <= threshold + median */
	if (match_distance <= threshold + index->radius)
		if (index->left != NULL)
			LookupEntryIndex(word, threshold, index->left, result);
	/*if Median <= threshold + d(Q,vp)*/
	if (index->radius <= threshold + match_distance)
		if (index->right != NULL)
			LookupEntryIndex(word, threshold, index->right, result);
	return EC_FAIL;
}

int matchword(const Word* w1, const Word* w2, enum MatchType matchtype) {
	int result;
	switch (matchtype) {
		case MT_EDIT_DIST:
			result = EditDistance(w1, w2);
			break;
		case MT_HAMMING_DIST:
			result = HammingDistance(w1, w2);
			break;
		case MT_EXACT_MATCH:
			result = Equal(w1, w2);
			break;
		default:
			break;
	}
	return result;
}

int unique_integer_generator(int selected_items, int total_items, int *
							 matrix_vpcandidates) {
	/*Algori8mos tou Floyd, exei
	 * Time complexity = O(selected_items*log(total_items) )
	 * Space complexity= O(selected_items)
	 * phgh : ""http://stackoverflow.com/questions/1608181/
	 * unique-random-numbers-in-an-integer-array-in-the-c-programming-language""
	 */
	unsigned int * is_used = calloc(total_items, sizeof (unsigned int));
	int i, im, r;
	im = 0; /*h 8esh t pinaka p bazoume unique akeraio*/
	for (i = total_items - selected_items; i < total_items
			&& im < selected_items; ++i) {
		r = rand() % (i + 1); //pare random arithmo
		if (is_used[r])/*an exoume hdh r..*/
			r = i; /*..tote xrhsimopoihse to i, to opoio sigoura mas dinei
                    * mia thesh pou den exei xrhsimopoihthei.*/
		matrix_vpcandidates[im++] = r;
		is_used[r] = 1;
	}
	free(is_used);
	return 0;
}

int cmpfunc_SortMatrix(const void* p1, const void* p2) {
	return ( ((QSortStruct *) p1)->distance - ((QSortStruct *) p2)->distance);
}

int cmpfunc_VpCandidatePositions(const void* p1, const void* p2) {
	return ( *((int *) p1) - *((int *) p2));
}

void counting_sort(void * input_array, int input_length, int max_key_value) {

	int count[max_key_value];
	QSortStruct output_array[input_length];
	int i, total = 0;
	for (i = 0; i < max_key_value; i++)
		count[i] = 0;
	for (i = 0; i < input_length; i++)
		count[return_key(input_array, i)]++;
	for (i = 1; i < max_key_value; i++)
		count[i] += count[i - 1];
	for (i = 0; i < input_length; i++) {
		int count_position = return_key(input_array, i);
		output_array[count[count_position] - 1].distance = count_position;
		output_array[count[count_position] - 1].entrylist_location =
				((QSortStruct*) input_array)[i].entrylist_location;
		count[count_position]--;
	}
	for (i = 0; i < input_length; i++)
		set_key(input_array, i, output_array);
}

int return_key(void * input_array, int position) {
	return (((QSortStruct *) input_array)[position]).distance;
}

void set_key(void * input_array, int i, void * output_array) {
	Entry* temp_ptr;
	((QSortStruct *) input_array)[i].distance = 
			((QSortStruct *) output_array)[i].distance;
	((QSortStruct *) input_array)[i].entrylist_location = 
			((QSortStruct *) output_array)[i].entrylist_location;
}