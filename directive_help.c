/*
 * directive_help.c
 * This file contains the functions that proccess the directives
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
int check_direct_type(char **);
int finish_ext_ent(char **,symbol_p* ,int );
int finish_dw_dh_db_as(char**,int,symbol_p *,int,int,char*,data_ptr*,int*);
void* check_read_dir_arg_to_arr(char **, int,int* );
int illegal_string(char **,int *);
int search_for_legal_comma(char **);
void save_direct_in_data_tab(void*,int,int,data_ptr*);

/*This function read and process the directives in the first reading
 * gets:
 *     direct_p - pointer to the directive
 *     sp -pointer to the symbol table
 *     num_of_symb - number of symbols in the symbols table
 *     label_flag - indicates for a flag in the start of the directive
 *     start_p - pointer to the label in the start of the directive
 *     data_table - pointer to the data table
 *     num_of_data- number of bytes used in data table
 * returns:
 *          ERROR in case something is wrong
 *          in case of success - a flag presents the directive type
 */
int handle_directive(char** direct_p,symbol_p *sp,int num_of_symb,int label_flag,char* start_p,data_ptr *data_table,int *num_of_data){
	int extern_error;
	switch( check_direct_type(direct_p)){/*check for the directive type*/
	case UNKNOWN :
		printf("\n	error - line number %d :unknown directive type\n",line_indx);
		return ERROR;
	case ENTRY:
		if(is_legal_label(*direct_p,sp,num_of_symb,ENTRY)==NO)/*check if label is legal - can be found in label_help.c*/
			return ERROR;
		return ENTRY ;/*cant handle yet will handle in second pass*/
	case EXTERN:
		if((extern_error = finish_ext_ent(direct_p ,sp,num_of_symb))==ERROR)/*finish reading  and saving the extern directive by a designed function*/
			return ERROR;
		if(extern_error == EXTERN_DUPLICATE)
			return EXTERN_DUPLICATE;
		else
			return EXTERN;
	case DW:
		if(finish_dw_dh_db_as(direct_p,DW,sp,num_of_symb,label_flag,start_p,data_table,num_of_data) == ERROR){/*finish reading  and saving the DW directive by a designed function*/
			return ERROR;
		}
		return DW;
	case DH:
		if(finish_dw_dh_db_as(direct_p,DH,sp,num_of_symb,label_flag,start_p,data_table,num_of_data) == ERROR){/*finish reading  and saving the DH directive by a designed function*/
			return ERROR;
		}
		return DH;
	case DB:
		if(finish_dw_dh_db_as(direct_p,DB,sp,num_of_symb,label_flag,start_p,data_table,num_of_data) == ERROR){/*finish reading  and saving the DB directive by a designed function*/
			return ERROR;
		}
		return DB;
	case ASCIZ:
		if(finish_dw_dh_db_as(direct_p,ASCIZ,sp,num_of_symb,label_flag,start_p,data_table,num_of_data) == ERROR){/*finish reading  and saving the asciz directive by a designed function*/
			return ERROR;
		}
		return ASCIZ;

	}
	return SUCCESS;
}

/*This function complete reading and processing the directives in the second reading
 * gets :
 *     direct - pointer to the directive
 *     symbol_p - pointer to the symbols table
 *     num_of_symb - number of symbols in symbols table
 * * returns:
 *          ERROR in case something is wrong
 *          SUCCESS
 */
int handle_directive2(char **direct ,symbol_p sp,int num_of_symb){
	int type;
	if((type = check_direct_type(direct))!= ENTRY)/*case directive type is not entry - we dont need to updated anything*/
		return SUCCESS;
	else{/*direct type is .entry*/
		if(updt_symb_tab_with_entry(direct,sp,num_of_symb)==ERROR)/*update the symbol table attributes with entry */
			return ERROR;

	}
	return SUCCESS;
}
/**********************************************functions for handle directive*******************************************/

/*This function checks the directive type
 * gets :
 *      type - pointer to the type part of the directive
 * returns:
 *          UNKNOWEN in case something is wrong
 *          flag presents the directive type
 */
int check_direct_type(char **type){
	char *type_p = *type;/*set type_p as the start address of the directive type*/
	int counter = 0;
	while(!isspace(*type_p)&& *type_p!=','){/*count the number of letters of the directive type*/
		type_p++;
		counter++;
	}
	if(!strncmp(*type,".dw",counter)){/*case directive type is dw*/
		*type = type_p;
		return DW;
	}
	if(!strncmp(*type,".dh",counter)){/*case directive type is dh*/
		*type = type_p;
		return DH;
	}
	if(!strncmp(*type,".db",counter)){/*case directive type is db*/
		*type = type_p;
		return DB;
	}
	if(!strncmp(*type,".asciz",counter)){/*case directive type is asciz*/
		*type = type_p;
		return ASCIZ;
	}
	if(!strncmp(*type,".entry",counter)){/*case directive type is entry*/
		*type = type_p;
		return ENTRY;
	}
	if(!strncmp(*type,".extern",counter)){/*case directive type is extern*/
		*type = type_p;
		return EXTERN;

	}

	return UNKNOWN;
}

/*This function finish reading and processing the extern and entry directives
 * gets :
 *       label - pointer to the label part of the directive
 *       symbol_p - pointer to the symbols table
 *       num_of_symb - number of symbols in symbols table
 * returns:
 *          ERROR in case something is wrong
 *          SUCCESS case directive is legal - saves the label in the symbols table
            EXTERN_DEPLICATE casw of duplicate external label as a sign to egnore the duplicate external label
 */
int finish_ext_ent(char **label,symbol_p *sp,int num_of_symb){/*reading label of the extern directive*/
	char *label_p ;
	int is_legal;
	if(skip_space(label)== END_OF_LINE){/*if we are missing a label- error*/
		printf("\n	error - line number %d :missing label to .extern \n",line_indx);
		return ERROR;
	}
	label_p = *label;/*set label_p as the start address of the label*/
	if((is_legal = is_legal_label(label_p,sp,num_of_symb,EXTERN)) == NO)/*check if label is legal- can be found in label_help.c*/
		return ERROR;
	while((!isspace(*label_p))&& (*label_p!=0)){/*skip the label - can be found in functions_for_command_and_directives_reading.c*/
		label_p++;
	}

	if(is_illegal_end(label_p)==YES){/*check for extraneous text in the end of the directive - can be found in functions_for_command_and_directives_reading.c*/
		printf("\n	error - line number %d :extraneous text after command/directive\n",line_indx);
		return ERROR;
	}
	if(is_legal == YES){
		save_label(label,sp,num_of_symb,EXTERN);/* save the label value in the symbol table*/
		return SUCCESS;
	}
	if(is_legal == EXTERN_DUPLICATE)/*case we have same label as an external already egnore it*/
		return EXTERN_DUPLICATE;

	return SUCCESS;

}

/*This function finish reading and processing the dw dh db as directives
 * gets :
 *       argu - pointer to the arguments part of the directive
 *       type - which directive it is
 *       symbol_p - pointer to the symbols table
 *       num_of_symb - number of symbols in symbols table
 *       label_flag - indicates if there is a label in the start of the directive
 *       start_p - pointer to the label in the start of the directive
 *       data_table - pointer to the data table
 *       num_of_data - number of bytes used in data table
 * returns:
 *          ERROR in case something is wrong
 *          SUCCESS case directive is legal - saves the directive in the data table
 */
int finish_dw_dh_db_as(char** argu,int type,symbol_p *sp,int num_of_symb,int label_flag,char* start_p,data_ptr *data_table,int *num_of_data){
	void *data_argus;
	int argus_counter=0,label_length =0,error;
	if(!isspace(**argu)){/*if we dont have space after the directive type- can be found in functions_for_command_and_directives_reading.c*/
		if(**argu!=','){/*if its a comma -illegal comma*/
			printf("\n	error - line number %d :missing space after directive type\n",line_indx);
		}
		else{/*its not a comma - missing space after directive type*/
			printf("\n	error - line number %d :illegal comma after directive type\n",line_indx);
		}
		return ERROR;
	}
	if(skip_space(argu) == END_OF_LINE){/*if we reached and of line- missing arguments to directive*/
		printf("\n	error - line number %d :missing arguments to directive \n",line_indx);
		return ERROR;
	}
	if(**argu == ','){/*we got a comma between directive type to first argument*/
		printf("\n	error - line number %d :illegal comma after directive type\n",line_indx);
		return ERROR;
	}
	if(!(data_argus = (check_read_dir_arg_to_arr(argu,type,&argus_counter))))/*save all the numbers in an array*/
		return ERROR;
	else{
		if(label_flag == YES){/* case we got a label */
			if(num_of_symb){/* case we already have labels in symbol table- check if this label already in the symboltable*/
				while(start_p[label_length]!=':')
					label_length++;
				if((error =exists_label(start_p,*sp, num_of_symb,label_length ,UNKNOWN))==YES){/*case we already have the same label in symbol table - can be found in label_help.c*/
					printf("\n	error - line number %d :label already exists\n",line_indx);
					return ERROR;
				}
				if(error == EXTERN){/*case the label exisits as an extenal label */
					printf("\n	error - line number %d :label already exists as an external label can't save label as a local\n",line_indx);
					return ERROR;
				}

			}
			save_label(&start_p,sp,num_of_symb,type);/* save the label in symbol table -can be found in label_help.c*/
		}
		save_direct_in_data_tab(data_argus,argus_counter,type,data_table);/* save the numbers insert in data table*/
		if(data_argus!=NULL){
			if(type == ASCIZ)
				free((char*)data_argus);/*free the arguments array*/
			else/*case directive is dw/dh/db*/
				free((long*)data_argus);/*free the arguments array*/


		}
		if(type ==DW)
			(*num_of_data)+=DW_SIZE*argus_counter;/*add to the bytes data counter the number of bytes we saved*/
		else
			if(type ==DH){
				(*num_of_data)+=DH_SIZE*argus_counter;/*add to the bytes data counter the number of bytes we saved*/
			}
			else{/*directive type is db or asciz*/
				(*num_of_data)+=argus_counter;/*add to the bytes data counter the number of bytes we saved*/
			}
	}

	return SUCCESS;
}
/*****************************************functions for finish_dw_dh_db_as()***********************************************/

/*This function checks and reads the arguments of a directive and saves them in an array
 * gets :
 *       argu - pointer to the arguments part of the directive
 *       type - which directive it is
 *       counter - number of the elements in the array - updates it in the function
 * returns:
 *          NULL in case of illegal text
 *          pointer to the array created in function the contains all the arguments
 */
void* check_read_dir_arg_to_arr(char **argu, int type,int *counter ){
	*counter=0;
	if(type == ASCIZ){
		char *string= NULL;/* pointer to an array of characters*/
		if(illegal_string(argu,counter)==YES)/*check if the string entered as an argument is a legal string*/
			return NULL;
		if(!(string = (char*)calloc(sizeof(char),*counter))){/*check for successful allocating*/
			printf("error-couldnt allocate memory");
			exit (1);
		}
		strncpy(string,*argu,*counter);/*copy the string to the array*/
		string[*counter]=0;/* set the end of the array set it as a string*/
		(*counter)++;
		return (void*)string;/*return the string pointer as void pointer*/
	}
	else{/*case dw ,dh,db */
		int first = YES;
		long num=0;
		long *numbers,*temp;
		int comma_flag;
		while(**argu != 0){/*read until we get to the end of line - because we dont know the number of the arguments we get*/
			if(first == NO){/*if its not the first number so we already read a number and now we are looking for a comma*/
				if((comma_flag = search_for_legal_comma(argu))== END)/*case we got to end of line return the array pointer as a void pointer*/
					return (void*)numbers;
				if(comma_flag == ERROR){
					free(numbers);
					return NULL;
				}
			}
			/*either if its the first number or not checking for a legal number and save it in array: */
			if((num = atol(*argu))||(is_zero(*argu)==YES)){/*atol could read a number there for we'll check if its legal*/
				while((**argu == '-')||(**argu == '+')||isdigit(**argu)){/*skip number*/
					(*argu)++;
				}
				if(!isspace(**argu) && **argu != ','&&**argu!=0){/*reached to illegal letter after the number*/
					if(first==NO)
						free(numbers);
					printf("\n	error - line number %d :illegal argument\n",line_indx);
					return NULL;
				}
				else{/*its a number with a comma or a space after*/
					if(legal_value(num,type)==YES){/*case number has a legal value - can be found in functions_for_commands_and_directives_reading.c*/
						if(first == NO){/*case we already have numbers in the a array-extend the array*/
							(*counter)++;
							if(!(temp = (long*)realloc(numbers,sizeof(long)*(*counter)))){
								printf("error-couldnt allocate memory");
								exit (1);
							}
							numbers = temp;
						}
						if(first == YES){/* case its the first number reading- create an array */
							(*counter)++;
							if(!(numbers = (long*)calloc(sizeof(long),(*counter)))){
								printf("error-couldnt allocate memory");
								exit (1);
							}
							first = NO;
						}
						numbers[*counter-1]=num;/*save the number in the array*/
					}
					else{/*illegal value number*/
						printf("\n	error - line number %d : illegal argument value\n",line_indx);
						return NULL;

					}
				}
			}
			else{/* atol couldnt read a number - illegal argument*/
				if(first==NO)
					free(numbers);
				printf("\n	error - line number %d :illegal argument\n",line_indx);
				return NULL;
			}
		}
		return (void*)numbers;/*return the array address as a void pointer*/
	}
}

/*This function saves the directive in the data table
 * gets:
 *      data_argus - pointer to the arguments array
 *      argus_counter - the number of arguments in the array
 *      type- the type of the directive
 *      data_table - pointer to the data table
 */
void save_direct_in_data_tab(void* data_argus,int argus_counter,int type,data_ptr* data_table){
	static int data_bytes_counter = 0;
	int i,j;
	data_ptr temp,table_p;
	if(*data_table ==NULL)/*we are in a start of a new assembler file so data table is a new one - set data_bytes_counter to 0  */
		data_bytes_counter =0;
	if(type == ASCIZ||type == DB)
		data_bytes_counter+=argus_counter;/* add to the data bytes counter the number of bytes we used*/
	if(type == DH)
		data_bytes_counter+=(argus_counter*DH_SIZE);/* add to the data bytes counter the number of bytes we used*/
	if(type == DW)
		data_bytes_counter+=(argus_counter*DW_SIZE);/* add to the data bytes counter the number of bytes we used*/
	if(*data_table == NULL){/*case we didnt have a  data table  because there was no data before -set a data table*/
		if(!(*data_table=(data_ptr)calloc(sizeof(data_byte),data_bytes_counter))){
			printf("error-couldnt allocate memory");
			exit (1);
		}
	}
	else{/*we already have data in table*/
		if(!(temp=(data_ptr)realloc(*data_table,sizeof(data_byte)*data_bytes_counter))){
			printf("error-couldnt allocate memory");
			exit (1);
		}
		else
			*data_table = temp;/*set data_table to the new memory allocated*/
	}

	if(type == ASCIZ){
		table_p = (*data_table)+data_bytes_counter-argus_counter;/*advance to the correct place in data table*/
		table_p->address = DC;/*set the address of the current directive as DC*/
		DC+=argus_counter;/*add to DC the number of bytes used*/
		for(i=0;i<argus_counter;i++){
			table_p->data = ((char*)data_argus)[i];/*save in data table the letter */
			table_p++;
		}
	}
	if(type == DB){
		table_p = (*data_table)+data_bytes_counter-argus_counter;/*advance to the correct place in data table*/
		table_p->address = DC;/*set the address of the current directive as DC*/
		DC+=argus_counter;/*add to DC the number of bytes used*/
		for(i=0;i<argus_counter;i++){
			table_p->data = (char)(((long*)data_argus)[i]);/*save in data table the number*/
			table_p++;
		}
	}
	if(type == DH){
		table_p = (*data_table)+data_bytes_counter-(argus_counter*DH_SIZE);/*advance to the correct place in data table*/
		table_p->address = DC;/*set the address of the current directive as DC*/
		DC+=(argus_counter*DH_SIZE);/*add to DC the number of bytes used*/
		for(i=0;i<argus_counter;i++){
			for(j=0;j<DH_SIZE;j++){
				table_p->data = (char)(((unsigned)(((long*)data_argus)[i]))>>(j*BYTE_SIZE));/*save the number in data table  by its bytes*/
				table_p++;
			}
		}
	}
	if(type == DW){
		table_p = (*data_table)+data_bytes_counter-(argus_counter*DW_SIZE);/*advance to the correct place in data table*/
		table_p->address = DC;/*set the address of the current directive as DC*/
		DC+=(argus_counter*DW_SIZE);/*add to DC the number of bytes used*/
		for(i=0;i<argus_counter;i++){
			for(j=0;j<DW_SIZE;j++){
				table_p->data = (char)(((unsigned)(((long*)data_argus)[i]))>>(j*BYTE_SIZE));/*save the number in data table  by its bytes*/
				table_p++;
			}
		}
	}
}

/***************************************************functions for check_read_dir_arg_to_arr()**************************************************/
/*This function checks if the string entered in the asciz directive is legal
 * gets:
 *      argus - pointer to the string part of the asciz directive
 * returns:
 *          NO if the string is illegal
 *          YES if the string is legal
 */
int illegal_string(char **argu,int *counter){
	char *argu_p ;
	if(**argu != '\"'){/*case we are missing a " in the start of the string*/
		printf("\n	error - line number %d : .asicz argument passed is not a string\n",line_indx);
		return YES;
	}
	(*argu)++;/*skip the "*/
	argu_p = *argu;
	while(*argu_p != '\"'){
		if(!isprint(*argu_p)){/*case the letter in the string is not printable*/
			printf("\n	error - line number %d : .asicz argument passed has an unprintable character\n",line_indx);
			return YES;
		}
		if(*argu_p == 0 ){/*reached end of line - missing a " in the end of string*/
			printf("\n	error - line number %d : .asicz argument passed is not a string\n",line_indx);
			return YES;

		}
		(*counter)++;/*counts the string letters*/
		argu_p++;
	}

	return NO;
}

/*This function searches for legal comma between the directive arguments
 * gets :
 *      argu - pointer to the arguments part of the directive
 * returns:
 *          ERROR case was a problem with finding a comma
 *          RESUME case found a legal comma
 */
int search_for_legal_comma(char **argu){

	if(skip_space(argu) == END_OF_LINE)/* can be found in functions_for_commands_and_directives_reading.c*/
		return END;/*reached end successfully*/
	if(**argu != ','){/*case was a space after number and after the space a comma is missing*/
		printf("\n	error - line number %d :missing comma between arguments\n",line_indx);
		return ERROR;
	}
	if(**argu == ','){
		(*argu)++;/*skip comma*/
		if(skip_space(argu) == END_OF_LINE){/*case we have illegal comma in the end of directive*/
			printf("\n	error - line number %d :illegal comma in the end\n",line_indx);
			return ERROR;
		}
		if(**argu == ','){/*case we have multiple commas*/
			printf("\n	error - line number %d :multiple commas between arguments\n",line_indx);
			return ERROR;
		}
		return RESUME;
	}

	return RESUME;

}
