/*
 ============================================================================
 Name        : mmn14
 Author      : itay peretz and lior mary
 Copyright   : Your copyright notice
 Description : assembler code conversion to machine code
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "flags.h"
#include "structs.h"
#include "global_varaibles.h"
#include "createfiles.h"
#include "free_memory.h"

/*local functions declarations*/
void proccess_file(char *);
int check_end_of_file(char*);



int main(int argc, char *argv[]) {
	void set_command_table();/*declaration - can be found in command_table.c */
	int i;
	int error = RESUME;
	if (argc == 1){
		printf("Error - no files entered");
	}
	set_command_table();/*save the commands information in the command table*/
	for (i = 1; i < argc; i++) {
		error = check_end_of_file(argv[i]);
		if (error == ERROR) {
			continue;
		}
		proccess_file(argv[i]);/*read the file by a designed function*/

	}

	return EXIT_SUCCESS;
}
/*This function check for illegal ending for the assembler file
 * gets the file name
 * returns:
 *          ERROR in case of illegal ending
 *          RESUME in case of a legal ending
 */
int check_end_of_file(char *file) {
	char *name_p = file;
	while (*name_p != '.' && *name_p != 0)
		name_p++;
	if (strcmp(name_p, ".as")) {/*if the ending of the file is not legal for assembler file*/
		printf("\n coudn't open %s - illegal file ending name\n", file);
		return ERROR;
	}
	return RESUME;
}

/*This function in charge of processing the assembler file and create the output files
 *   gets the file name */
void proccess_file(char *file_name) {
	/*declarations:*/
	int first_reading(FILE *, symbol_p *, int*, data_ptr*, int*, comm_ptr*); /* function that do the first reading of the text -can be found in first_reading.c */
	int second_reading(FILE *, symbol_p, int, comm_ptr, external_ptr*); /* function that do the second reading of the text -can be found in second_reading.c */
	FILE *fd;
	symbol_p sp = NULL;/*pointer to the symbol table*/
	external_ptr h_external_table = NULL;/*pointer to the external labels table*/
	data_ptr data_table = NULL;/*pointer to the data table*/
	comm_ptr h_comm_code = NULL;/*pointer to the command code table*/
	int error;
	int num_of_symb = 0;
	int num_of_data = 0;

	if (!(fd = fopen(file_name, "r"))) {
		fprintf(stderr, "\n coudn't open %s - file is not accessible\n",
				file_name);
		return;
	}
	printf("\n file name: %s\n", file_name);
	if ((error = first_reading(fd, &sp, &num_of_symb, &data_table, &num_of_data,&h_comm_code)) == ERROR) {/*process first reading*/
		free_all_tables(sp, data_table, &h_comm_code, h_external_table);/*case there was an error free all dynamic allocated memory - can be found in free_memory.c*/
		return;
	} else {/*first reading was successful*/
		rewind(fd);
		if ((error = second_reading(fd, sp, num_of_symb, h_comm_code,&h_external_table)) == ERROR) {/*process second reading*/
			free_all_tables(sp, data_table, &h_comm_code, h_external_table);/*case there was an error free all dynamic allocated memory - can be found in free_memory.c*/
			return;
		}
		else {/*case we read file successfully create output files - can be found in creatfiles.c*/
			printf("\n	complete reading-creating files\n");
			create_ob_file(h_comm_code, data_table, num_of_data, file_name);
			create_ext_file(h_external_table, file_name);
			create_ent_file(sp, num_of_symb, file_name);
			/*after we created all the files, we need to free the tables space in the memory*/
			free_all_tables(sp, data_table, &h_comm_code, h_external_table);
		}
	}
	return;
}
