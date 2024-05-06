/*
 * functions_for_commands_and_directives_reading.c
 * This file contains all the general functions that helps reading both the commands and the directives  - those functions used in the directive_help.c the commands_help.c and the first and second reading files
 */

#include <limits.h>
#include<ctype.h>
#include "flags.h"



/*This function checks if the char is zero
 * gets num- pointer for the character
 * returns:
 *          YES - case character is 0
 *          NO - case its not 0
*/
int is_zero(char *num){
	while(*num == '-'||*num == '+'||isdigit(*num)){
		if(*num == '0')
			return YES;
		num++;
	}
	return NO;
}

/*This function checks for a legal value of arguments/operands/registers
 * gets :
 *     num - the value to check for
 *     type - the type of operand/argument/register
 * returns:
 *          YES case legal
 *          NO case illegal
*/
int legal_value(long num ,int type){
	if (type == DW){
		if(num <= INT_MAX && num >= INT_MIN)
			return YES;
		return NO;
	}
	if (type == DH){
		if(num <= DH_MAX && num >= DH_MIN)
			return YES;
		return NO;
	}
	if (type == DB){
		if(num <= DB_MAX && num >= DB_MIN)
			return YES;
		return NO;
	}
	if(type == IM){
		if((num <= IM_MAX) && (num >= IM_MIN))
			return YES;
		return NO;
	}
	if(type == REG){
		if(num <= REG_MAX && num >= REG_MIN)
			return YES;
		return NO;
	}
	return YES;
}

/*This function checks if there is an extraneous text in the end of the line
 * gets line_p -pointer to the end of the line
 * returns:
 *          YES case there is
 *          NO case the is not
*/
int is_illegal_end(char *line_p){
	while(*line_p != 0){/*run until end of line*/
		if(!isspace(*line_p))
			return YES;
		line_p++;
	}
	return NO;
}

/*This function skip spaces in the input file
 * gets ch - pointer a character
 * returns:
 *          END_OF_LINE case after skipping space reached to the end of the line
 *          RESUME - after skipping space - a flag that indicates to continue reading
*/
int skip_space(char** ch){
	while(isspace(**ch))
		(*ch)++;
	if(**ch == 0)
		return END_OF_LINE;/*case we reached end of line return end of line flag*/
	return RESUME;

}
