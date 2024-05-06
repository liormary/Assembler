assembler: assembler.o first_reading.o second_reading.o directive_help.o command_help.o label_help.o  functions_for_commands_and_directives_reading.o command_table.o createfiles.o free_memory.o
	gcc -ansi -Wall -pedantic assembler.o first_reading.o second_reading.o directive_help.o command_help.o label_help.o  functions_for_commands_and_directives_reading.o command_table.o createfiles.o free_memory.o -o assembler
	
assembler.o: assembler.c flags.h structs.h global_varaibles.h createfiles.h free_memory.h
	gcc -c -ansi -Wall -pedantic assembler.c -o assembler.o
	
first_reading.o: first_reading.c flags.h structs.h global_varaibles.h functions_for_commands_and_directives_reading.h label_help.h
	gcc -c -ansi -Wall -pedantic first_reading.c -o first_reading.o
	
second_reading.o: second_reading.c flags.h structs.h global_varaibles.h functions_for_commands_and_directives_reading.h label_help.h
	gcc -c -ansi -Wall -pedantic second_reading.c -o second_reading.o	

directive_help.o: directive_help.c flags.h structs.h global_varaibles.h functions_for_commands_and_directives_reading.h label_help.h
	gcc -c -ansi -Wall -pedantic directive_help.c -o directive_help.o
		
command_help.o: command_help.c flags.h structs.h global_varaibles.h functions_for_commands_and_directives_reading.h label_help.h
	gcc -c -ansi -Wall -pedantic command_help.c -o command_help.o
	
label_help.o: label_help.c flags.h structs.h global_varaibles.h functions_for_commands_and_directives_reading.h 
	gcc -c -ansi -Wall -pedantic label_help.c -o label_help.o
	
functions_for_commands_and_directives_reading.o: functions_for_commands_and_directives_reading.c flags.h
	gcc -c -ansi -Wall -pedantic functions_for_commands_and_directives_reading.c -o functions_for_commands_and_directives_reading.o
		
command_table.o: command_table.c flags.h structs.h 
	gcc -c -ansi -Wall -pedantic command_table.c -o command_table.o
		
createfiles.o: createfiles.c flags.h structs.h global_varaibles.h
	gcc -c -ansi -Wall -pedantic createfiles.c -o createfiles.o
		
free_memory.o: free_memory.c structs.h
	gcc -c -ansi -Wall -pedantic free_memory.c -o free_memory.o
	

	

	

