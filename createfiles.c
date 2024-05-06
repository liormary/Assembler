/*
 * createfiles.c
 * This file contains all the functions that responsible for creating output files
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "flags.h"
#include "structs.h"
#include "global_varaibles.h"

/*local functions declaration*/
char* set_ob_ex_en_file_name(char *file_name, int type, int file_name_length);
int get_file_name_length(char*);
/*declaration*/
extern void free_output_file_name(char *);/*can be found in free_memory.c*/

/******************************************* create files functions************************************************/
/*This function in charge of creating the ob file
 * gets :
 *       h_comm_code - pointer to the commands code table
 *       data_table - pointer to the data table
 *       num_of_data - the number of bytes used in data table
 *       file_name - pointer to the input file name
 */
void create_ob_file(comm_ptr h_comm_code, data_ptr data_table, int num_of_data,char* file_name) {
	comm_ptr hptr;
	data_ptr dptr;
	int i;
	int count = 0;
	int adcount = 100;
	FILE* f;
	int file_name_length;
	char *ob_file;
	file_name_length = get_file_name_length(file_name);/*get thr file name length (until the '.')*/
	ob_file = set_ob_ex_en_file_name(file_name, OB, file_name_length);/*set the output file name*/
	f = fopen(ob_file, "a"); /*open file ps.ob */
	if (!f) {
		fprintf(stderr, "Error: could not open file!\n"); /*there is not enough memory to create the file*/
	} else { /*open file was successful*/
		fprintf(f, "\n     %d %d", (IC - 100), DC); /*print to the file the number of the lines of the code and the number of the lines of the data*/
		for (hptr = h_comm_code; hptr != NULL; hptr = hptr->next) { /*pass on the command table and convert the binary code to hexadecimal numbers from right to left */
			fprintf(f, "\n%04d", adcount); /*print to file the address of the binary command*/
			adcount = adcount + 4;
			for (i = 0; i < 4; i++) {
				fprintf(f, " %02X", (unsigned char) hptr->code[i]); /*print to file the hexadecimal numbers*/
			}
		}
		dptr = data_table;
		while (count < num_of_data) { /*pass on the data table and convert the binary data to hexadecimal numbers from right to left */
			fprintf(f, "\n%04d", adcount);
			for (i = 0; i < 4; i++) {
				if (count == num_of_data) {
					break;
				}
				fprintf(f, " %02X", (unsigned char) dptr->data);
				dptr++;
				count++;
			}
			adcount = adcount + 4;
		}

	}
	fclose(f); /*close the file*/
	free_output_file_name(ob_file); /*free the name of the file*/
}

/*This function in charge of creating the ext file
 * gets:
 *    h_external_table - pointer to the external labels
 *    file_name - pointer to the input file name
 */
void create_ext_file(external_ptr h_external_table, char* file_name) {
	external_ptr exptr;
	int file_name_length;
	FILE* f1;
	char *ex_file;
	file_name_length = get_file_name_length(file_name);/*get thr file name length (until the '.')*/
	ex_file = set_ob_ex_en_file_name(file_name, EX, file_name_length);/*set the output file name*/
	for (exptr = h_external_table; exptr != NULL; exptr = exptr->next) {  /*check the external table and if she is not null, print to the file the name of the external label*/
		f1 = fopen(ex_file, "a");
		if (!f1) {
			fprintf(stderr, "Error: could not open file!\n");
		} else {
			fprintf(f1, "\n");
			fputs(exptr->name, f1);
			fprintf(f1, " %04d ", exptr->address);
			fclose(f1); /*close the file*/
		}
	}
	free_output_file_name(ex_file); /*free the name of the file*/
}

/*This function in charge of creating  the ent file
 * gets:
 *      sp - pointer to the symbols table
 *      num_of_symb - number of symbols un the symbols table
 *      file_name - pointer to the input file name
 */
void create_ent_file(symbol_p sp, int num_of_symb, char* file_name) {
	int file_name_length;
	symbol_p sptr;
	int count = 0;
	int first = YES;
	FILE* f2 = NULL;
	char *en_file;
	file_name_length = get_file_name_length(file_name);/*get thr file name length (until the '.')*/
	en_file = set_ob_ex_en_file_name(file_name, EN, file_name_length);/*set the output file name*/
	sptr = sp;
	for (count = 0; count < num_of_symb; count++) { /*check the symbol table and if there is entry label, the function print the name of the label to the file */
		if (strncmp(sptr->attributes, "data,entry", 10) == 0) {
			if (first == YES) {
				f2 = fopen(en_file, "a");
				if (!f2) {
					fprintf(stderr, "Error: could not open file!\n");
				} else {
					fprintf(f2, "\n");
					fputs(sptr->name, f2);
					fprintf(f2, " %04d ", sptr->value);
					first = NO;
				}
			} else {
				fprintf(f2, "\n");
				fputs(sptr->name, f2);
				fprintf(f2, " %04d ", sptr->value);
			}
		}
		sptr++;
	}
	if(f2)
		fclose(f2); /*close the file*/
	free_output_file_name(en_file); /*free the name of the file*/
}

/**************************************** create files help functions************************************************/
/*This function check the file name length
 * gets :
 *       file_name - pointer to the file name
 * returns:
 *          the file name length
 */
int get_file_name_length(char* file_name) {
	int counter = 0;
	while (*file_name != '.') {
		file_name++;
		counter++;
	}
	return counter;
}

/*This function sets the file name for each output file
 * gets :
 *      file_name -pointer to the input file name
 *      type - output file type
 *      file_name_length - the length of the input file name without the .as ending
 * returns:
 *          pointer to the output file name
 */
char* set_ob_ex_en_file_name(char *file_name, int type, int file_name_length) {
	char *output_file_name;
	if (type == OB) {
		if(!(output_file_name = (char*) calloc(sizeof(char), file_name_length + 4))){/*allocatimg memory for output file name - file_name_length+4 for file name + the ending ".ob"  */
		   printf("error-coudnt allocate memory");
		   exit (1);
		}
		strncpy(output_file_name, file_name, file_name_length + 1);/*copy to the output file name the input file name and the dot after*/
		strcat(output_file_name, "ob");/*adds to the file name the ending of the output file*/
		return output_file_name;
	}
	if (type == EX) {
		if(!(output_file_name = (char*) calloc(sizeof(char), file_name_length + 5))){/*allocatimg memory for output file name - file_name_length+5 for file name + the ending ".ext"  */
		   printf("error-coudnt allocate memory");
		   exit (1);
		}
		strncpy(output_file_name, file_name, file_name_length + 1);/*copy to the output file name the input file name and the dot after*/
		strcat(output_file_name, "ext");/*adds to the file name the ending of the output file*/
		return output_file_name;
	}
	if (type == EN) {
		if(!(output_file_name = (char*) calloc(sizeof(char), file_name_length + 5))){/*allocatimg memory for output file name - file_name_length+5 for file name + the ending ".ent"  */
		   printf("error-coudnt allocate memory");
		   exit (1);
		}
		strncpy(output_file_name, file_name, file_name_length + 1);/*copy to the output file name the input file name and the dot after*/
		strcat(output_file_name, "ent");/*adds to the file name the ending of the output file*/
		return output_file_name;
	}
	return NULL;

}

