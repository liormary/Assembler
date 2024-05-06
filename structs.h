/*
 * structs.h
 * This file contains all the tables structs
 */


#define label_max_length 31
#define label_attributes_length 15


/*data table*/
typedef struct data_byte* data_ptr;
typedef struct data_byte{
	char data;
	int address;
}data_byte;



/*symbols table*/
typedef struct{
	char name[label_max_length+1];
	int value;
	char attributes[label_attributes_length];
}symbol;

typedef symbol* symbol_p ;


/* command table*/
typedef struct{
	char name[5];
	char type;
	char opcode;
	char func;

}command;





/*command machine code linked list pointer*/
typedef struct comm_node* comm_ptr;

typedef struct comm_node{
	char code[4];
	int address;
	comm_ptr next;
}comm_node;



/*external labels linked list pointer*/
typedef struct external_node* external_ptr;



/*external labels by command linked list*/
typedef struct external_node{
	char name[label_max_length];
	int address;
	external_ptr next;
}external_node;


