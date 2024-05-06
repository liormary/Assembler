/*
 * free_memory.c
 * This file contains the functions that free all the dynamic memory
 */

#include <stdlib.h>
#include "structs.h"

/*local functions declaration*/
void free_h_comm_code(comm_ptr*);
void free_external_table(external_ptr);

/*This function free all the tables
 * gets:
 *     sp - pointer to the symbols table
 *     data_table - pointer to the data table
 *     h_comm_code - pointer to the command code table
 *     h_external_table - pointer to the external labels table
 */
void free_all_tables(symbol_p sp,data_ptr data_table,comm_ptr *h_comm_code,external_ptr h_external_table){
	if(sp)/*case we have symbol table*/
		free(sp);
	if(data_table)/*case we have data table*/
		free(data_table);
	if(*h_comm_code)/*case we have command table*/
		free_h_comm_code(h_comm_code);/*free linked list*/
	if(h_external_table)/*case we have external labels table*/
		free_external_table(h_external_table);/*free linked list*/
	return;
}

/*This function free the command code table(linked list)
 * gets h_comm_code - pointer to the commands table
 */
void free_h_comm_code(comm_ptr *h_comm_code){
	comm_ptr temp;
	while(*h_comm_code){
		temp = *h_comm_code;
		*h_comm_code = (*h_comm_code)->next;

			free(temp);

	}

}

/*This function free the external table(linked list)
 * gets h_external_table- pointer to the external labels table
 */
void free_external_table(external_ptr h_external_table){
	external_ptr temp;
	while(h_external_table->next!=NULL){
		temp = h_external_table;
		h_external_table = h_external_table->next;
		free(temp);

	}

}

/*This function free the output file name
 * gets file - pointer to the file name
 */
void free_output_file_name(char *file){
	free(file);
}
