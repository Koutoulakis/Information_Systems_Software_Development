/* 
 * File:   job_structs.h
 * Author: panagiotis
 *
 * Created on March 19, 2014, 12:07 PM
 */

#ifndef JOB_STRUCTS_H
#define	JOB_STRUCTS_H

#ifdef	__cplusplus
extern "C" {
#endif

	typedef struct create_doc_job StructCreateDocJob;

	struct create_doc_job {
		DocID doc_id;
		char * doc_str;
		DocumentListNode* global_list_position;
	};

#ifdef	__cplusplus
}
#endif

#endif	/* JOB_STRUCTS_H */

