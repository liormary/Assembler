/*
 * second_reading.c
 * This file contains the second reading functions
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "flags.h"
#include "functions_for_commands_and_directives_reading.h"
#include "structs.h"
#include "global_varaibles.h"
#include "label_help.h"

/*function declarations*/
extern int handle_directive2(char**,symbol_p,int);/*can be found in directive_help.c*/
extern int handle_command2(char**,comm_ptr,symbol_p ,int,external_ptr*);/*can be found in command_help.c*/

/*local function declaration*/
int read_line2(FILE **, symbol_p ,int, comm_ptr ,external_ptr*);

/*This function does the second reading of the input file
 * gets:
 *      fd - pointer to the input file
 *      sp - pointer to the symbol table
 *      num_of_symb - number of the symbols saved already
 *      h_comm_code - pointer to the commands table
 *      h_external_table - pointer to the external labels table
 *
 * returns:
 *          ERROR  is error detected
 *          SUCCESS case complete second reading successfully
 */
int second_reading(FILE *fd ,symbol_p sp,int num_of_symb,comm_ptr h_comm_code ,external_ptr *h_external_table){
	int error_flag = SUCCESS,status;
	line_indx = 0;
	IC =100;
	while(! feof(fd) ){/*runs until we get to end of file*/
		line_indx++;
		status= read_line2(&fd,sp,num_of_symb,h_comm_code,h_external_table);/*read a line from file*/

		if((status == ERROR) && (error_flag == SUCCESS)){/*case we have an error in reading and error flag is off*/
			error_flag = ERROR;
		}
		if(status == END){/*case we reached to end of reading*/
			if(error_flag == ERROR)/*case we had an error*/
				return ERROR;
			else
				return SUCCESS;
		}

	}
	if(error_flag == ERROR )
		return ERROR;
	else
		return SUCCESS;
}

/*This function read a line in the second reading of the input file
 * gets :
 *      fd - pointer to the input file
 *      sp - pointer to the symbol table
 *      num_of_symb - number of the symbols saved already
 *      h_comm_code - pointer to the commands table
 *      h_external_table - pointer to the external labels table
 *
 * returns:
 *          ERROR if error detected
 *          SUCCESS case complete reading a line successfully
 */
int read_line2(FILE **fd , symbol_p sp,int num_of_symb, comm_ptr h_comm_ptr,external_ptr *h_external_table){
	char line[line_length] ,  *line_p;
	char *readerr;
	line_p = line ;
	if(!(readerr = fgets(line , line_length , *fd)))/*rcase reached to end of file*/
		return 	END;
	if(skip_space(&line_p)== END_OF_LINE)/*its a blank line - can be found in functions_for_commands_and_directives_reading.c*/
		return SUCCESS;
	if(*line_p == ';')/*its a note line*/
		return SUCCESS;
	skip_on_label(&line_p);/*skip on the label in case we have one - can be found in label_help.c*/
	skip_space(&line_p);/*get to the command/directive type*/
	if(*line_p=='.'){/*directive sentence*/
		if(handle_directive2(&line_p,sp,num_of_symb)==ERROR)/*finish reading and saving directive - can be found in directive_help.c*/
			return ERROR;
		return SUCCESS;
	}
	if(handle_command2(&line_p,h_comm_ptr,sp,num_of_symb,h_external_table)==ERROR){/* finish reading and saving command - can be found in command_help.c*/
		IC+=4;
		return ERROR;
	}
	else{
		IC+=4;
		return SUCCESS;
	}
}






