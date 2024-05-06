/*
 * flags.h
 * This file contains flags and macros for the project
 */

enum error_codes {ERROR=-1,LENGTH_ERR=-2,SUCCESS=-3,RESUME=-4 ,END=-5,EXTERN_DUPLICATE=-6};
typedef enum {DW,DH,DB,ASCIZ,ENTRY,EXTERN,UNKNOWN,COMM_OP}dir__type;
enum {IM =7,REG =8,LABEL = 9};
enum {NO =-100,YES = -200};
enum {DIRECTIVE=100 ,COMMAND=200};
enum command_tab_places {move_place = 5 ,addi_place = 8,bne_place =13,lb_place=17,jmp_place =23,stop_place=26};
enum registers {RS =0,RT = 1 ,RD= 2};
enum files_types{OB=1,EX=2,EN=3};


#define END_OF_LINE -1
#define line_length 81
#define DB_MIN -128
#define DH_MIN -32768
#define DW_MIN -2147483648
#define DB_MAX 127
#define DH_MAX 32767
#define DW_MAX 2147483647
#define IM_MIN -32768
#define IM_MAX 32767
#define REG_MIN 0
#define REG_MAX 31
#define DH_SIZE 2
#define DW_SIZE 4
#define BYTE_SIZE 8
#define command_op_code 63


