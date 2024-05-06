/*
 * first_reading.c
 * This file contains the first reading functions
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

/*local functions declaration*/
int read_line( FILE**,symbol_p*,int* ,data_ptr *,int*,comm_ptr* );
int illegal_length(char *);
void update_data_tab_address(data_ptr );
void update_symbol_tab(symbol_p f,int );
void skip_end_of_line(FILE **);
/*functions declaration*/
extern int handle_directive(char** ,symbol_p *,int ,int,char*,data_ptr*,int*);/*can be found in directive_help.c*/
extern int handle_command(char **,symbol_p *,int,int,char *,comm_ptr*);/*can be found in command_help.c*/


/*This function does the first reading of the input file
 * gets:
 *      fd - pointer to the input file
 *      sp - pointer to the symbol table
 *      num_of_symb - number of the symbols saved already
 *      data_table - pointer to the data table
 *      num_of_data- number of bytes used in data table
 *      h_comm_code - pointer to the commands table
 * returns:
 *          ERROR if illegal input detected
 *          SUCCESS case first reading was successful
*/
int first_reading(FILE *fd ,symbol_p *sp,int *num_of_symb,data_ptr *data_table,int *num_of_data,comm_ptr* h_comm_code){
	int error_flag = SUCCESS,status;
	line_indx = 0;
	IC =100;
	DC = 0;
	while(! feof(fd) ){/*runs until we reach to end of file*/
		line_indx++;
		status= read_line(&fd ,sp,num_of_symb,data_table,num_of_data,h_comm_code);/*reading a line from file*/
		if(status == LENGTH_ERR)/*case we had a longer than 80 letters line skip the rest of the line so we could contine reading next line*/
			skip_end_of_line(&fd);
		if((status == ERROR || status ==LENGTH_ERR) && (error_flag == SUCCESS)){/*case we had an error in reading line and error flag is off*/
			error_flag = ERROR;
		}
		if(status == END){/*Happens when tried to read a line but couldn't read because reached end of line*/
			if(error_flag == ERROR)
				return ERROR;
			else{/*we read the file with no error*/
				if(*data_table)
					update_data_tab_address(*data_table);/*update the addresses of the data table to be after the command table*/
				if(*sp)
					update_symbol_tab(*sp,*num_of_symb);/*update the data labels values*/
				return SUCCESS;
			}
		}

	}
	/* gets here only if in the last line of the file wasn't  with '\n' at the end and reached EOF*/
	if(error_flag == ERROR)
		return ERROR;
	else{/*we read file without errors*/
		if(*data_table)
			update_data_tab_address(*data_table);/*update the addresses of the data table to be after the command table*/
		if(*sp)
			update_symbol_tab(*sp,*num_of_symb);/*update the data labels values*/
		return SUCCESS;
	}
}
/*****************************************************functions for first_reading()****************************************************************/
/*This function reads a line in the first reading of the input file
 * gets:
 *     fd - pointer to the input file
 *     sp - pointer to the symbol table
 *     num_of_symb - number of the symbols saved already
 *     data_table - pointer to the data table
 *     num_of_data- number of bytes used in data table
 *     h_comm_code - poi
 * returns:
 *          ERROR if illegal input detected
 *          LENGTH_ERR if the line is too long
 *          SUCCESS case line reading was successful
*/
int read_line( FILE **fd ,symbol_p *sp,int *num_of_symb,data_ptr *data_table,int *num_of_data,comm_ptr *h_comm_code){

	char line[line_length] ,  *line_p,*start_p;
	char *readerr;
	int label_flag =NO ,direct_type;
	line_p = line ;
	/*read a line:*/
	if(!(readerr = fgets(line , line_length , *fd)))/*case reached to end of file*/
		return 	END;
	if(strlen(line)==line_length-1){/*case we read the maximum letters number*/
		if(illegal_length(line_p)==ERROR){/*check if line is too long*/
			printf("\n	error - line number %d :illegal length\n",line_indx);
			return LENGTH_ERR;
		}
	}
	if(skip_space(&line_p)== END_OF_LINE)/*case we have an empty line - can be found in functions_for_commands_and_directives_reading.c*/
		return SUCCESS;
	if(*line_p == ';')/*case we have a note line*/
		return SUCCESS;
	start_p = line_p;/*set line_p as the start of the line address(after start spaces)*/
	if((label_flag = check_for_label(&line_p,sp,*num_of_symb))==ERROR){/*case we illegal label - can be found in label_help.c*/
		return ERROR;
	}
	if(label_flag == YES)/*case we have a label*/
		if(skip_space(&line_p)== END_OF_LINE){/*case after label we dont have a directive/command*/
			printf("\n	error - line number %d :label insert without command/directive\n",line_indx);
			return ERROR;
		}
	if(*line_p=='.'){/*directive sentence*/
		if((direct_type = handle_directive(&line_p,sp,*num_of_symb,label_flag,start_p,data_table,num_of_data))==ERROR)/*finish reading the directive and get the directive type -can be found in directive_help.c*/
			return ERROR;
		else
			if(direct_type == ENTRY)/*case directive type is entry - we will complete everything in the seconed reading*/
				return SUCCESS;
			else
				if(direct_type == EXTERN)/*case directive type is extern add to symbols counter 1*/
					(*num_of_symb)++;
				else
					if(label_flag==YES)/*case we had a label in directive*/
						(*num_of_symb)++;
		return SUCCESS;
	}
	else{/*command sentence*/
		if(handle_command(&line_p,sp,*num_of_symb,label_flag,start_p,h_comm_code)==ERROR){/*finish reading and saving command by a designed function - can be found in command_help.c*/
			return ERROR;
		}
		else{
			if(label_flag==YES)/*case we had a label in command*/
				(*num_of_symb)++;
		}
		return SUCCESS;
	}

	return SUCCESS;
}
/*This function update the symbols address table after the first reading of the input file
 * gets :
 *     sp - pointer to the symbol table
 *     num_of_symb - number of the symbols saved already
 *
*/
void update_symbol_tab(symbol_p sp,int num_of_symb){
	int i;
	for(i=0;i<num_of_symb;i++){
		if(!strcmp(sp->attributes,"data"))/*case label is a data label update its value to after the command labels*/
			sp->value+=IC;
		sp++;
	}
}

/*This function updates the data table addresses after the first reading of the input file
 * gets data_table- pointer to the data table
*/
void update_data_tab_address(data_ptr data_table){
	int i;
	data_table->address+=IC;/*update the first data label value*/
	data_table++;
	for(i=1;i<DC;i++){
		if(data_table->address){/*if its a start of a directive address (if the address = 0 its in the middle of the directive code there for its doesnt have an address)*/
			data_table->address+=IC;/*update the data label value*/
		}
		data_table++;
	}
}

/***********************************************************functions for read_line()*************************************/
/*This function checks if the line length is legal in the first reading of the input file
 * gets line - pointer to the last character of the string that saves the line entered
 * returns:
 *          ERROR if the line is too long
 *          SUCCESS case the line length is legal
*/
int illegal_length(char *line){
	if (line[line_length-2] != '\n')/*case the last letter in the string is not a '\n' we have to many letters in line */
		return ERROR;
	return SUCCESS;

}

/*This function skips the end of the line in case the line was to long
 * gets fd - pointer to the input file (points to the characters in the end of the line)
*/
void skip_end_of_line(FILE **fd){
	int ch;
	while((ch =fgetc(*fd))!='\n' ){
		if(ch==EOF)/*we reached end of file-its also end of line so stop skipping*/
			break;
	}
	if(ch!=EOF)
		ch =fgetc(*fd);/*we are not in the end of file -skip on '\n'*/
}



