/*
 * label_help.c
 * This file contains all the functions that helps with the labels
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "flags.h"
#include "functions_for_commands_and_directives_reading.h"
#include "structs.h"
#include "global_varaibles.h"

/*local functions declaration*/
int check_for_label(char**,symbol_p*,int);
void save_label(char**,symbol_p * ,int,int);
int exists_label(char * ,symbol_p  ,int ,int,int);
int compare_reserved_words(char*);
int reserved_words_check(char *,int);
int is_legal_label(char *,symbol_p *,int,int);
int get_label_length(char*);

/*This function checks if there is a label in the start of the command\directive
 * gets :
 *       name - pointer to the label part in the sentence
 *       sp - pointer to the symbol table
 *       num_of_symn - number of symbols in the table
 * returns:
 *          ERROR its an illegal label
 *          YES case its a legal label
 *          NO case there is no label
 */
int check_for_label(char** name,symbol_p *sp,int num_of_symb){
	char *name_p = *name;
	while(*name_p != ':'){
		name_p++;
		if(*name_p == 0)/*no label*/
			return NO;
	}

	if(isspace(*--name_p)){/*case we have a space between label to ':'*/
		printf("\n	error - line number %d :illegal label- space before ':'\n",line_indx);
		return ERROR;
	}
	if(is_legal_label(*name,sp,num_of_symb,UNKNOWN)==YES){/*check if label is legal*/
		while(**name != ':')/*skip on label*/
			(*name)++;
		(*name)++;/*skip on ':'*/
		return YES ;
	}

	return ERROR;
}

/*This function saves the binary code of the difference between the command address and the label entered in the immed
 * gets :
 *       label - pointer to the label
 *       length - the length of the label
 *       h_comm_code - pointer to the commands table
 *       sp - pointer to the symbol table
 *       num_of_symn - number of symbols in the table
 */
void save_label_diff_code_in_immed(char **label,int length,comm_ptr h_comm_ptr,symbol_p sp,int num_of_symb){
	void save_immed_code(long ,comm_ptr );
	int i;
	long immed;
	while(h_comm_ptr->address!=IC)/*gets to the current command in table*/
		h_comm_ptr=h_comm_ptr->next;
	for(i=0;i<num_of_symb;i++){/*search for the label in symbols table*/
		if(!strncmp(*label,sp->name,length)){/*case found label in table*/
			if(sp->name[length]==0){
				immed =sp->value-IC;/*set immed as the difference between this command and the label value*/
				save_immed_code(immed,h_comm_ptr);/*save the difference in the immed part of the command*/
				return;
			}
		}
		sp++;
	}
	return;
}

/*This function saves the label in the symbols table
 * gets:
 *       name - pointer to the label part in the sentence
 *       sp - pointer to the symbol table
 *       num_of_symn - number of symbols in the table
 *       type - the type of the label(for the attributes filed)
 *
 */
void save_label(char** name,symbol_p *sp ,int num_of_symb,int type ){
	symbol_p sp1;
	int counter=0;
	char *name_p =*name;
	if(!num_of_symb){/*case its the first label to save in symbol table-create table*/
		if(!(*sp = (symbol_p)calloc(sizeof(symbol),1))){
			printf("error-coudnt allocate memory");
			exit (1);
		}
	}

	if(num_of_symb){/*case we already have labels in symbol table- extend table*/
		if(!(sp1 = (symbol_p)realloc(*sp,sizeof(symbol)*(num_of_symb+1)))){
			printf("\nerror-could'nt allocate memory\n");
			exit (1);
		}
		*sp =sp1;/*set sp as the new symbol table pointer*/
	}
	while(isalpha(*name_p)||isdigit(*name_p)){/*get the label length*/
		counter++;
		name_p++;
	}
	strncpy(((*sp)+num_of_symb)->name,*name,counter);/*copy the label to the symbol table*/
	((*sp)+num_of_symb)->name[counter]=0;/*set the character after the label to 0 as a sign for end of string*/

	if(type == COMMAND){/*case label is a command label*/
		((*sp)+num_of_symb)->value = IC;/*set label value to current command counter*/
		strcpy(((*sp)+num_of_symb)->attributes,"code");/*set the attributes of the symbol to "code"*/
	}
	else
		if(type == EXTERN){/*case label is an external label*/
			strcpy(((*sp)+num_of_symb)->attributes,"data,external");/*set the attributes of the symbol to "data,external"*/
			((*sp)+num_of_symb)->value = 0;/*set label value to 0*/
		}
		else{
			if(type == ENTRY){/*case label is an entry label*/
				strcpy(((*sp)+num_of_symb)->attributes,"data,entry");/*set the attributes of the symbol to "data,entry"*/
			}
			else/*case label is a data label*/
				strcpy(((*sp)+num_of_symb)->attributes,"data");/*set the attributes of the symbol to "data"*/
			((*sp)+num_of_symb)->value = DC;/*set label value to current data counter*/
		}
	return;
}

/*This function saves the external label in the external table
 * gets :
 *      h_external_table - pointer to the external labels table
 *      label - pointer to the label
 *      label_length - the length of the label
 */
void save_label_in_external_table (external_ptr *h_external_table,char *label,int label_length){
	external_ptr temp ;
	external_ptr i= *h_external_table;
	temp = (external_ptr)calloc(sizeof(external_node),1);/*set a new external label node in memory*/
	strncpy(temp->name,label,label_length);/*save the name of the label in the external node*/
	temp->address = IC;/*save its value in the external node*/

	if(*h_external_table == NULL){/*case there is not labels in external labels table save the node as the head of the linked list*/
		*h_external_table = temp;
	}
	else{/*case we already have labels in external table add the label node the the end of the linked list*/
		while(i->next!=NULL)
			i=i->next;
		i->next = temp;

	}
}

/*This function updates the symbol table if there is an entry label - in the second reading
 * gets:
 *       label - pointer to the label
 *       sp - pointer to the symbol table
 *       num_of_symn - number of symbols in the table
 *
 * returns:
 *          ERROR if there is no such label in the symbol table
 *          SUCCESS case updetad attribute successfully
 */
int updt_symb_tab_with_entry(char **label,symbol_p sp,int num_of_symb){
	int counter=0,i;
	char *label_p ;
	skip_space(label);/*get to the label - can be found in functions_for_commands_and_directives_reading.c*/
	label_p = *label;
	counter = get_label_length(label_p);/*get label length*/
	for(i=0;i<num_of_symb;i++){
		if(!strncmp(sp->name,label_p,counter)){/*case we found the label in the symbol table*/
			if(sp->name[counter]==0){
				strcpy( sp->attributes,"data,entry");/*update label attributes to "data,entry"*/
				return SUCCESS;
			}
		}
		sp++;
	}
	/*case we didnt find the label in symbol table*/
	printf("\n	error - line number %d :entry label was'nt found in assembler code \n",line_indx);
	return ERROR;
}

/*This function skips on a label in the line
 * gets line - pointer to the label part in the line
 */
void skip_on_label(char **line){
	char *line_p = *line;
	while(*line_p != ':'){
		line_p++;
		if(*line_p == 0)/*no label*/
			return;
	}
	line_p++;/*skip on ':' */
	*line=line_p;/*advance the pointer to after the label*/
	return;
}

/******************************************************functions for check_for_label()*******************************************/
/*This function checks if the label is legal
 * gets :
 *       label - pointer to the label
 *       sp - pointer to the symbol table
 *       num_of_symn - number of symbols in the table
 *       type - type of label (to know what do check )
 * returns:
 *          NO case illegal
 *          YES case legal
 *          EXTERN_DUPLICATE case of deuplicate external label
 */
int is_legal_label(char *label,symbol_p *sp,int num_of_symb,int type){
	int counter = 0 ;
	int i;
	int error;
	if(type == UNKNOWN){/*case it a label in a start of a command/directive*/
		while(label[counter]!=':')/*get label length*/
			counter++;
	}
	else{/*case it is a label used as an operand in commands like extern/entry/branch */
		counter = get_label_length(label);/*get label length*/
	}

	if (counter > label_max_length){/*case illegal label length*/
		printf("\n	error - line number %d :illegal label-label name is too long\n",line_indx);
		return NO;
	}
	for(i=0 ;i< counter; i++){
		if(i==0)/*first letter of label*/
			if(!isalpha(label[i])){/*check if the first letter is an alphabetic letter*/
				printf("\n	error - line number %d :illegal label-illegal start letter\n",line_indx);
				return NO;
			}
		if(!isalpha(label[i]) && !isdigit(label[i])){/*check if the label written only from letters and digits*/
			printf("\n	error - line number %d :illegal label-illegal letter\n",line_indx);
			return NO;

		}
	}
	if(reserved_words_check(label ,counter)==ERROR){/* check if the label is not an  assembler reserved word*/
		printf("\n	error - line number %d :label name is a saved word\n",line_indx);
		return NO;
	}

	if((type != UNKNOWN) && (type!=COMM_OP)&&(type!=ENTRY))/*case its not a start of command or command operand label */
		if(num_of_symb){/*if there is already labels in the symbol table*/
			if((error = exists_label(label,*sp,num_of_symb,counter ,type))==YES){/* case the label is already exists as a local -error*/
				printf("\n	error - line number %d :label already exists as a local -can't set as an external\n",line_indx);
				return NO;
			}
			else
				if(error == EXTERN_DUPLICATE)
					return EXTERN_DUPLICATE;
		}
	return YES;

}



/******************************************************functions for is_legal_label()********************************************************/
/*This function checks of the label entered is a reserved assembler word
 * gets:
 *     name - pointer to the label
 *     length - the label length
 * returns:
 *          ERROR if it a reserved word
 *          SUCCESS its a legal label
 */
int reserved_words_check(char *name,int length){
	char label[label_max_length];/*set an array to save the label*/
	strncpy(label,name,length);/*save the label in the array*/
	label[length]=0;/*set the last place of the array to 0 as a sign of end of string*/
	if(compare_reserved_words(label)==NO)/*compare with reserved words*/
		return SUCCESS;
	return ERROR ;

}


/*This function checks if the label exists in the symbol table
 * gets:
 *     name_p - pointer to the label
 *       sp - pointer to the symbol table
 *       num_of_symn - number of symbols in the table
 *       length - label length
 *       type - label type
 * returns:
 *          NO case its already exists in the symbol table
 *          YES case it is not exists
 *          EXTERN_DUPLICATE case external label already saved as an external label
 *          EXTERN case local label saved already as an external
 */
int exists_label(char *name_p ,symbol_p sp,int num_of_symb ,int length,int type){
	int i;
	for(i=0;i<num_of_symb;i++){
		if(!strncmp(name_p,sp->name,length)){/*case we found the label in the symbol table*/
			if(sp->name[length]==0){/*check if labels actually the same to avoid  edge cases like comparing label1 and label11*/
				if(type == EXTERN){/*case the label type is extern*/
					if(!strcmp(sp->attributes,"data,external"))/*case the label in the symbol table is also external*/
						return EXTERN_DUPLICATE;/*its legal because its already set as an external label  there for stop compare and return EXTERN as a sign for not saving it again in symbol table*/
				}
				else/*the label type is not extern*/
					if(type == COMM_OP||UNKNOWN)/*case its a command operand label*/
						if(!strcmp(sp->attributes,"data,external")){/*case the label in the symbol table is an external*/
							return EXTERN;/*return EXTERN flag as a sign of extern error because we used an external label as a command operand*/
						}
				return YES;
			}
		}
		sp++;
	}
	return NO;
}
/***************************************functions for reserved_words()*********************************************************/



/*This function compares the label with all the reserved words of the assembler
 * gets:
 *     name - pointer to the label name
 * returns:
 *          YES case its a reserved word
 *          NO case not
 */
int compare_reserved_words(char* name){
	int i;
	for(i=0;i<comm_tab_len;i++)
		if(!strcmp(name,command_table[i].name))/*check if the label is a reserved word*/
			return YES;
	return NO;
}

/*This function checks the label length
 * gets label - pointer to the label
 * returns:
 *          integer of the label length
 */
int get_label_length(char *label){
	int label_length =0;
	while(!isspace(*label)&& *label!=0){/*runs until gets to space or end of line */
		label_length++;
		label++;
	}
	return label_length;
}
