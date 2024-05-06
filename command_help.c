/*
 * command_help.c
 * This file contains all the functions that helps to process the commands
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
void skip_2_operands(char **);
int check_comm_type(char ** );
comm_ptr create_mem_node();
int check_and_read_operands(char **,int ,comm_ptr );
void add_to_command_code_tab(comm_ptr* ,comm_ptr );
long is_legal_immed(char **);
void save_immed_code(long,comm_ptr );
void save_op_func_code_(int ,comm_ptr );
int search_for_comma(char **);
void save_reg_address_code(long,int,comm_ptr );
void save_reg_code(char,int,comm_ptr);
int is_legal_reg(char **);
int check_for_reg(char **);

/*This function reads and process a command - first reading
 * gets:
 *      command - pointer to the command
 *      sp - pointer to the symbol table
 *      num_of_symb - number of the symbols saved already
 *      label_flag - indicates if there is a label in the start of the command
 *      start_p -pointer to the label in the start of the command
 *      h_comm_code - pointer to the commands table
 * returns:
 *          ERROR in case its not a legal command
 *          SUCCESS in case the command added to the command table
 */
int handle_command(char **command,symbol_p *sp,int num_of_symb,int label_flag,char *start_p,comm_ptr *h_comm_code){
	comm_ptr code_p;/*for saving a command code*/
	int error;
	int type,label_length =0;
	code_p = create_mem_node();/*creates a new command code space */
	if((type=check_comm_type(command))==ERROR){/* case its not a legal command*/
		free(code_p);
		return ERROR;
	}
	save_op_func_code_(type,code_p);/*save the op and func part in the command code*/
	if(type!=stop_place){/*if its not stop we need to read parameters */
		if(!isspace(**command)){/*if we dont have space between the command type to the first parameter print an error massage*/
			if(**command!=','){
				free(code_p);
				printf("\n	error - line number %d :missing space after command type\n",line_indx);
			}
			else{/*case we have a comma after the the command type*/
				free(code_p);
				printf("\n	error - line number %d :illegal comma after command type\n",line_indx);
			}
			return ERROR;
		}
		if(skip_space(command) == END_OF_LINE){/*case there is no parameters after the command type - can be found in functions_for_commands_and_directives_reading.c*/
			free(code_p);
			printf("\n	error - line number %d :missing operands to command\n",line_indx);
			return ERROR;
		}
		if(check_and_read_operands(command,type,code_p)==ERROR){/*we do have parameters so read them by a designed function*/
			free(code_p);
			return ERROR;
		}
	}
	else{/*command type is stop*/
		if(is_illegal_end(*command)==YES){/*we check for illegal text after command type -can be found in functions_for_commands_and_directives_reading.c*/
			free(code_p);
			printf("\n	error - line number %d :extraneous text after command/directive\n",line_indx);
			return ERROR;
		}
	}
	if(label_flag == YES){/*now completing the label in the start of the command */
		if(num_of_symb){/*case we already have labels we need to check if we dont already have same label in another command*/
			while(start_p[label_length]!=':')/*gwt label length*/
				label_length++;
			if((error = exists_label(start_p,*sp,num_of_symb,label_length,UNKNOWN))==YES){/*check if the label already exists int symbol table - can be foundin label_help.c*/
				free(code_p);
				printf("\n	error - line number %d :label already exists\n",line_indx);
				return ERROR;
			}
			if(error == EXTERN){
				free(code_p);
				printf("\n	error - line number %d :label already exists as an external can't save it as a local\n",line_indx);
				return ERROR;
			}
		}
		save_label(&start_p,sp,num_of_symb,COMMAND);/*save the label in the symbol table - can be found in label_help.c*/
	}
	add_to_command_code_tab(h_comm_code,code_p);/*add the command code to the command code table*/
	return SUCCESS;
}

/*This function  complete reading and processing a command - second reading
 * gets :
 *      command - pointer to the command
 *      h_comm_code - pointer to the commands table
 *      sp - pointer to the symbol table
 *      num_of_symb - number of the symbols saved already
 *      h_external_table -pointer to the external labels table
 * returns:
 *          ERROR in case its not a legal command
 *          SUCCESS in case of completing process of command
 */
int handle_command2(char **command,comm_ptr h_comm_ptr,symbol_p sp,int num_of_symb,external_ptr *h_external_table){
	int type;
	int label_type;
	int label_length;
	int i;
	int address = 0;
	type = check_comm_type(command);/*check for the command type*/
	skip_space(command);/*get to first operand -can be found in functions_for_commands_and_directives_reading.c*/
	if(type>=bne_place && type<lb_place){/*case the command type is a branch we need to complete the immed code*/
		skip_2_operands(command);/*skip first to registers because we already read them un the firt reading*/
		label_length = get_label_length(*command);/*can be found in label_help.c*/
		if((label_type=exists_label(*command,sp,num_of_symb,label_length,COMM_OP))==EXTERN){/*case the label entered is an external label is illegal label -can be found in label_help.c*/
			printf("\n	error - line number %d :entered an external label for conditional branching command \n",line_indx);
			return ERROR;
		}
		if(label_type==NO){/*case we didnt find the label in the symbol table we cant branch to that address*/
			printf("\n	error - line number %d :label wasn't found in assembler code \n",line_indx);
			return ERROR;
		}
		/*case we found the label in symbol table*/
		save_label_diff_code_in_immed(command,label_length,h_comm_ptr,sp,num_of_symb);/*save the difference between the current command address to the label address - can be found in label_help.c*/
	}
	if(type >=jmp_place && type <stop_place){/*case the command type is a J type command */
		if(check_for_reg(command)==NO){/*if we have a label as a parameter*/
			label_length = get_label_length(*command);
			if((label_type = exists_label(*command,sp,num_of_symb,label_length,COMM_OP))==NO){/*case we coudnt find the label in the symbol table -can be found in label_help.c*/
				printf("\n	error - line number %d :label wasn't found in assembler code \n",line_indx);
				return ERROR;
			}
			else{/*we found the label in symbol table*/
				/*if its an external label address should be with 0 so we don't need to enter address value to address in table there for we skip saving label code*/
				if(label_type!=EXTERN){/*case it is not an external label we need to enter its address to the immed part of the command*/
					while(h_comm_ptr->address!=IC)/*get to the place of the current command in the command table*/
						h_comm_ptr=h_comm_ptr->next;
					for(i=0;i<num_of_symb;i++){/*search for the label in the symbols table*/
						if(!strncmp(*command,sp->name,label_length)){
							if(sp->name[label_length]==0){
								address =sp->value;/*save the value of the label*/
								break;
							}
						}
						sp++;
					}
					save_reg_address_code((long)address,LABEL,h_comm_ptr);/*save the value of the label in the address part of the command */
				}
				else{/*case we used an external label save it in the externals table*/
					if(label_type==EXTERN){
						save_label_in_external_table(h_external_table,*command,label_length);/*can be found in label_help.c*/
					}

				}
			}
		}

	}
	return SUCCESS;
}



/********************************functions for handle_command() *******************************/
/*This function skips on first 2 operands
 * gets a pointer to the start of the operands part of a command
 * */
void skip_2_operands(char **command){
	int comm_counter=0;
	while(comm_counter<2){/*every time we get to a comma we skipped an operand there for run 2 times*/
		if(**command == ',')
			comm_counter++;
		(*command)++;
	}
	skip_space(command);/*case we have space skip the space after the comma -can be found in functions_for_commands_and_directives_reading.c*/
}

/*This function creates another command node
 * returns:
 *          nothing in case of error
 *          pointer to the node
 */
comm_ptr create_mem_node(){
	comm_ptr command_p;
	if(!(command_p = (comm_ptr)calloc(sizeof(comm_node),1))){/*check if memory is successful allocated*/
		printf("error-coudnt allocate memory");
		exit (1);
	}
	return command_p;
}

/*This function checks the command type
 * gets pointer to the command type
 * returns:
 *          ERROR in case we didnt find the command type
 *          integer the presents the command type
 */
int check_comm_type(char **command){
	char *command_p = *command;/*save in command_p the start address of the command*/
	int counter=0;
	int i;
	while(isalpha(*command_p)){/*advance the command_p to the end of the command type and counts the number of the letters*/
		counter++;
		command_p++;
	}
	for(i=0;i<comm_tab_len;i++){/*compares the type of the command with all the command types in the command table to find the command type*/
		if(!strncmp(*command,command_table[i].name,counter)){
			if(command_table[i].name[counter]==0){
				*command = command_p;
				return i;/*returns the command type*/
			}
		}
	}
	printf("\n	error - line number %d :undefined command\n",line_indx);/*case we didnt find the command type */
	return ERROR;
}

/*This function checks and read the operands in the input file - case all legal saves in the command node
 * gets :
 *        operands -pointer to the operands part of the command
 *        type - command type
 *        code_p - pointer to the command node
 * returns:
 *          ERROR in case of illegal operand
 *          SUCCESS in case all operands are legal -saves the operands code in the command node
 */
int check_and_read_operands(char **operands, int type ,comm_ptr code_p){
	int i;
	int reg;
	long num;
	if(**operands==','){/*case we have illegal comma after command type*/
		printf("\n	error - line number %d :illegal comma after command type\n",line_indx);
		return ERROR;
	}
	if(type< move_place){/*case we need to read 3 registers*/
		for(i=0;i<3;i++){
			if(i!=0)/*case we are not in the first operand there for we are looking for aa comma*/
				if(search_for_comma(operands)==ERROR)
					return ERROR;
			if((reg = is_legal_reg(operands))==NO)/*check if register is legal*/
				return ERROR;
			save_reg_code((char)reg,i,code_p);/*case register is legal saves it in its designed place*/
		}
	}
	else{
		if(type < addi_place){/*case we need to read 2 operands*/
			for(i=0;i<2;i++){
				if(i!=0)/*case we are not in the first operand there for we are looking for aa comma*/
					if(search_for_comma(operands)==ERROR)
						return ERROR;
				if((reg = is_legal_reg(operands))==NO)/*check if register is legal*/
					return ERROR;
				if(i == 0)
					save_reg_code((char)reg,RS,code_p);/*save the register in its designed place*/
				if(i == 1)
					save_reg_code((char)reg,RD,code_p);/*save the register in its designed place*/
			}

		}
		else{
			if(type < jmp_place){
				if(type< bne_place || (type>=lb_place &&type<jmp_place)){/*I command type but not bne,beq,blt,bgt there for we need to read register number and register*/
					for(i=0;i<3;i++){
						if(i!=0)
							if(search_for_comma(operands)==ERROR)
								return ERROR;
						if(i == 0||i == 2){/*read register*/
							if((reg = is_legal_reg(operands))==NO)
								return ERROR;
							if(i == 0)
								save_reg_code((char)reg,RS,code_p);/*save the register in its designed place*/
							if(i == 2)
								save_reg_code((char)reg,RT,code_p);/*save the register in its designed place*/
						}
						else{/*read immed for I command*/
							if((num = is_legal_immed(operands))==NO)
								return ERROR;
							save_immed_code(num,code_p);/*save the immed in its designed place*/
						}
					}
				}
				else{/*its bne,beq,blt,bgt*/
					for(i=0;i<2;i++){/*we are going to save only the registers code the immed code is going to be saved on the second pass*/
						if(i!=0)
							if(search_for_comma(operands)==ERROR)
								return ERROR;
						if((reg = is_legal_reg(operands))==NO)
							return ERROR;
						if(i == 0)
							save_reg_code((char)reg,RS,code_p);/*save the register in its designed place*/
						if(i == 1)
							save_reg_code((char)reg,RT,code_p);/*save the register in its designed place*/
					}
					if(search_for_comma(operands)==ERROR)/*check for a comma between the second operand to the third*/
						return ERROR;
					if(is_legal_label(*operands,NULL,0,COMM_OP)==NO)/*check if label is legal (dont read it just check for a legal input)-can be found in label_help.c*/
						return ERROR;
					while(!isspace(**operands))/*advance pointer to after the label*/
						(*operands)++;
				}
			}
			else{/*its a J command type*/
				if(type <stop_place){
					if(type==jmp_place){/*its the jump command - we need to read only one register or label*/
						if(check_for_reg(operands)==YES){ /*case we have register to read*/
							if((reg = is_legal_reg(operands))!=NO)/*check if register is legal*/
								save_reg_address_code((long)reg,REG,code_p);/*save register in the address part of the command*/
						}
						else{/*we got a label as an operand*/
							if(is_legal_label(*operands,NULL,0,COMM_OP)==NO){/*check if label entered as an operand is legal -can be found in label_help.c*/
								return ERROR;
							}
							while(!(isspace(**operands)))/*advance the pointer to after the label*/
								(*operands)++;
						}
					}
					else{/*its either la or call commands*/
						if(is_legal_label(*operands,NULL,0,COMM_OP)==NO){/*check if label entered as an operand is legal -can be found in label_help.c*/
							return ERROR;
						}
						while(!(isspace(**operands)))/*advance the pointer to after the label*/
							(*operands)++;

					}
				}
			}
		}
	}
	if(is_illegal_end(*operands)==YES){/*check for extraneous text after command - can be found in functions_for_commands_and_directives_reading.c*/
		printf("\n	error - line number %d :extraneous text after command/directive\n",line_indx);
		return ERROR;
	}
	return SUCCESS;
}

/*This function adds the command to the command code table
 * gets:
 *      h_comm_code - pointer to the command table
 *      code_p - pointer to the command code
 */
void add_to_command_code_tab(comm_ptr *h_comm_code,comm_ptr code_p){
	comm_ptr p1;
	code_p->address =IC;/*set the address of the command*/
	IC+=4;
	p1 = *h_comm_code;/*set a pointer to run on the table*/
	if(*h_comm_code == NULL){/*case we dont have any commands in table already-save the command in the start of the table*/
		*h_comm_code = code_p;
	}
	else{/*we already have commands in table - adds command to the end of the table*/
		while(p1->next!=NULL){/*get to the last place in the table*/
			p1=p1->next;
		}
		p1->next = code_p;/*set the new command in the last place of the table*/
	}
}

/*This function saves the binary code of the opcode and the func of the command
 * gets :
 *        tab_place - the command place in the commands table
 *        code_p - pointer to the command node
 */
void save_op_func_code_(int tab_place,comm_ptr code_p){
	code_p->code[3]|=(command_table[tab_place].opcode<<2);/*save the op code in the 26-31 bits*/
	if(command_table[tab_place].type == 'R'){/*case we are in R type command*/
		code_p->code[0]|=(command_table[tab_place].func<<6);/*save the first two bits of func code in the 6-7 bits*/
		code_p->code[1]|=(((unsigned)command_table[tab_place].func)>>2);/*save the last bits of the func code in the 8-10 bits*/
	}
}
/*********************************functions for check_and_read_operands()****************************************/


/*This function search for a comma between operands in the input file
 * gets operands - pointer to the character that after the operand
 * returns:
 *          ERROR in case we have something illegal
 *          SUCCESS case found a comma
 */
int search_for_comma(char **operands){
	if(skip_space(operands) == END_OF_LINE){/*case we searched for a comma and got to the end of line - we are missing an operand*/
		printf("\n	error - line number %d :missing operands to command\n",line_indx);
		return ERROR;
	}
	if(**operands!=','){/*case we didnt find a comma - we missing one*/
		printf("\n	error - line number %d :missing comma between command operands\n",line_indx);
		return ERROR;
	}
	(*operands)++;/*if we found a comma skip it*/
	if(skip_space(operands) == END_OF_LINE){/*check if we have a missing parameter*/
		printf("\n	error - line number %d :missing operands to command\n",line_indx);
		return ERROR;
	}
	if(**operands == ','){/*check for multiple commas between operands*/
		printf("\n	error - line number %d :multiple commas between command operands\n",line_indx);
		return ERROR;
	}
	return SUCCESS;


}

/*This function checks if the register is legal
 * gets reg- pointer to the register
 * returns:
 *          NO in case we have something illegal
 *          YES in case the register is legal
 */
int is_legal_reg(char **reg){
	int num=0;
	if(**reg!='$'){/*cae we didnt find $ in the start of the register  - illegal register*/
		printf("\n	error - line number %d :illegal register name\n",line_indx);
		return NO;
	}
	(*reg)++;/*case we did find a $ skip it*/
	if((num = atoi(*reg))||(is_zero(*reg)==YES)){/*case we found a  number of  register by atoi and check if the number is zero (because atoi returns 0 also if it didnt find a number in the string*/
		while(isdigit(**reg))/*skip the number */
			(*reg)++;
		if(**reg!=','&& !isspace(**reg)){/*case we reached end of number but there is an illegal letter after*/
			if(**reg!='$'){/*register is illegal - illegal letter in register*/
				printf("\n	error - line number %d :illegal register name\n",line_indx);
				return NO;
			}
			else{/*we started the next register without a comma between*/
				printf("\n	error - line number %d :missing comma between command operands\n",line_indx);
				return NO;
			}

		}
		if(legal_value((long)num,REG)==YES)/*check if register value is legal-can be found in functions_for_commands_and_directives_reading.c*/
			return num;/* case its a legal register returning register number*/
		else{/*case illegal value - error*/
			printf("\n	error - line number %d :illegal value register\n",line_indx);
			return NO;
		}
	}
	else{/*atoi couldnt get a number- there is no digit after $*/
		printf("\n	error - line number %d :illegal register name\n",line_indx);
		return NO;
	}

}

/*This function saves the binary register code
 * gets:
 *      reg - register value
 *      type - command type(R,I,J)
 *      code_p - pointer to the command node
 */
void save_reg_code(char reg ,int type ,comm_ptr code_p){
	if(type==RS){
		code_p->code[2]|=((unsigned)reg)<<5;/*save the first 3 bits of rs in the 21-23 bits*/
		code_p->code[3]|=((unsigned)reg)>>3;/*save the last 2 bits od rs in the 24-25 bits*/
	}
	if(type == RT){
		code_p->code[2]|=reg;/*save the rt bits in the 16-20 bits*/
	}
	if(type==RD){
		code_p->code[1]|=((unsigned)reg)<<3;/*save the rd bits in the 11-15 bits*/
	}

}

/*This function saves the binary code of the address part of the J commands
 * gets:
 *      address - the address value to enter as a binary code
 *      type - command type
 *      code_p - pointer to the command node
 */
void save_reg_address_code(long address,int type,comm_ptr code_p){
	char reg_flag =1;
	if(type == REG){/*case we save register as the address*/
		code_p->code[3]|=(reg_flag<<1);/*set the 25th bit as 1 */
		code_p->code[0]|=((unsigned char)address);/*save the register bits in 0-4 bits -all other bit are 0*/
	}
	else{/*case we save a label value as address*/
		if(type ==LABEL){
			code_p->code[0]|=((unsigned char)address);/*save the first 8 bits of label in the 0-7 bits*/
			code_p->code[1]|=((char)(((unsigned)address)>>BYTE_SIZE));/*save the middle 8 bits of label in the 8-15 bits*/
			code_p->code[2]|=((char)(((unsigned)address)>>(BYTE_SIZE*2)));/*save the middle 8 bits of label in the 16-23 bits*/
			code_p->code[3]|=((char)(((unsigned)address)>>(BYTE_SIZE*2+1)));/*save the last bit of label in the 24th bit*/
		}

	}
}
/*This function checks if the immed is legal
 * gets:
 *      immed - pointer to the immed part of the command
 */
long is_legal_immed(char **immed){
	long num =0;
	if(((num = atol(*immed)))||(is_zero(*immed)==YES)){/*get the immed value and check if its 0 because atol return 0 also when it coudnt find a number*/
		while(**immed == '-'||**immed == '+'||isdigit(**immed))/*skip the number*/
			(*immed)++;

		if(!isspace(**immed) && **immed != ','){/*reached to illegal letter after the number*/
			printf("\n	error - line number %d :illegal immed\n",line_indx);
			return NO;
		}
		else{/*its a number with a comma or a space after*/
			if(legal_value(num,IM)==YES){/*can be found in functions_for_commands_and_directives_reading.c*/
				return num;
			}
			else{/*number insert as immed has illegal value*/
				printf("\n	error - line number %d :immed illegal value\n",line_indx);
				return NO;
			}
		}

	}
	else{/*atol couldnt find a number*/
		printf("\n	error - line number %d :illegal immed\n",line_indx);
		return NO;
	}
}
/*This function saves immed binary code
 * gets:
 *      num - the immed value
 *      code_p - pointer to the command node
 */
void save_immed_code(long num,comm_ptr code_p){
	code_p->code[0]|=((unsigned char)num);/*save the first 8 bits of immed in the 0-7 bits*/
	code_p->code[1]|=(char)(((unsigned )num)>>8);/*save the last 8 bits of immed in the 8-15 bits*/
}
/*This function search for a register
 * gets:
 *      reg - pointer to the registers part of the command
 * returns:
 *      YES case found one
 *      NO case didnt
 */
int check_for_reg(char **reg){
	if(**reg=='$')/*check for the start of the register sign */
		return YES;
	return NO;
}
