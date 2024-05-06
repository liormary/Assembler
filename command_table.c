/*
 * command_table.c
 * This file contains the information of all the commands in a table
 */


#include <string.h>
#include "flags.h"
#include "structs.h"
#include "global_varaibles.h"



/*This function sets all the commands in a command table */
void set_command_table (){
	int i=0,j;
	strcpy(command_table[i++].name,"add");
	strcpy(command_table[i++].name,"sub");
	strcpy(command_table[i++].name,"and");
	strcpy(command_table[i++].name,"or");
	strcpy(command_table[i++].name,"nor");
	strcpy(command_table[i++].name,"move");
	strcpy(command_table[i++].name,"mvhi");
	strcpy(command_table[i++].name,"mvlo");
	strcpy(command_table[i++].name,"addi");
	strcpy(command_table[i++].name,"subi");
	strcpy(command_table[i++].name,"andi");
	strcpy(command_table[i++].name,"ori");
	strcpy(command_table[i++].name,"nori");
	strcpy(command_table[i++].name,"bne");
	strcpy(command_table[i++].name,"beq");
	strcpy(command_table[i++].name,"blt");
	strcpy(command_table[i++].name,"bgt");
	strcpy(command_table[i++].name,"lb");
	strcpy(command_table[i++].name,"sb");
	strcpy(command_table[i++].name,"lw");
	strcpy(command_table[i++].name,"sw");
	strcpy(command_table[i++].name,"lh");
	strcpy(command_table[i++].name,"sh");
	strcpy(command_table[i++].name,"jmp");
	strcpy(command_table[i++].name,"la");
	strcpy(command_table[i++].name,"call");
	strcpy(command_table[i++].name,"stop");
	for(i=0 ,j=1;i<comm_tab_len ;i++){
		if(i<move_place){/* R arithmetics commands*/
			command_table[i].type = 'R';
			command_table[i].opcode = 0;
			command_table[i].func = j++;
			if(i==4)/*last place of op_code = 0 so func code strat from 1 again*/
				j=1;

		}
		else
			if(i<addi_place){/*R copy commands*/
				command_table[i].type = 'R';
				command_table[i].opcode = 1;
				command_table[i].func = j++;
				if(i==7)/*last place of R command so op code start from 10*/
					j=10;
			}
			else
				if(i<jmp_place){ /* I commands*/
					command_table[i].type = 'I';
					command_table[i].opcode = j++;
					if(i==22)/*last place of I command so op code start from 30*/
						j=30;

				}
				else{
					if(i<stop_place){/* J commands*/
						command_table[i].type = 'j';
						command_table[i].opcode = j++;
					}
					else{/*stop command*/
						command_table[i].type = 'j';
						command_table[i].opcode = command_op_code;

					}
				}
	}


}
