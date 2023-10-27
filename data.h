/*
*  The header file of the first and second pass
*/


#ifndef DATA_H
#define DATA_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


/***********************************Macros**********************************/
#define ERROR -1
#define TARGET_MOVE 2 /* for bitwise, to move 2 bits to "target operands" */
#define OP_MOVE 5 /* to move 5 bits to "opcode" */
#define SOURCE_MOVE 9 /* to move 9 bits to "source operand" */
#define BASE_64_MOVE 6 /* to convert every 6 bits to base 64*/
#define EXTERN_START_ADDRESS 4000 /* the extern label's address will be the address+4000 to recognize extern label*/
#define ENTRY_START_ADDRESS 2000 /* the entry label's address will be the address+4000 to recognize entry label*/
#define BUFFER_MAX_SIZE 1024 /* max memory*/
#define BUFFER_START_INDEX 100 /* the first memory word */
#define CURRENT_INDEX (BUFFER_START_INDEX + (*IC) + (*DC)) /* number of instruction + directives + 100 */
#define MAX_LABEL 31
#define MAX_NUM 511 /* the maximum number that can 10 bits contains (2 bits for A.R.E)*/
#define MAX_LINE 81
#define NEW_WORD (get == '\t' || get == ' ' || get == '\r')
#define NEW_LINE (get == '\n')

/* printable characters from the ascii table */
#define FIRST_PRINTABLE_CHAR 32
#define LAST_PRINTABLE_CHAR 126

/* printing file name without endig, using a loop */
#define PRINT_FILE_NAME for (j = 0; filename[j] != '\0'; j++) \
		                { \
			                if (filename[j] == '.') \
			                { \
				                break; \
			                } \
			                printf("%c",filename[j]); \
		                }\
		                printf(".as\n");

/* exntern file ending */
#define EXT_ENDING filename[len - 3] = 'e'; \
    filename[len - 2] = 'x'; \
    filename[len - 1] = 't'; \
    filename[len] = '\0';
    
/* entry file ending */
#define ENT_ENDING filename[len - 3] = 'e'; \
    filename[len - 2] = 'n'; \
    filename[len - 1] = 't'; \
    filename[len] = '\0';
    
/* object file ending */
#define OB_ENDING filename[len - 3] = 'o'; \
        filename[len - 2] = 'b'; \
        filename[len - 1] = '\0';

/* inserting the binary code of label address by decimal numbers.
*  this macro gets two paramters, the first to insert label address to the memory image
*  the second paramter the index to insert.
*
*  @param value: to insert to the memory image
*  @param value: the index to insert
*
*  case(0) for normal label, case(2,3) entry label, case(4,5) extern label.
*/
#define LABEL_CODING(value, dec_num) \
    buffer_inst[dec_num][0] = 0; \
    switch (value / 1000) \
    { \
        case (0): \
            buffer_inst[dec_num][0] = value; \
            buffer_inst[dec_num][0] <<= TARGET_MOVE; \
            buffer_inst[dec_num][0] += 2; \
            break; \
        case (2): \
        case (3):\
            buffer_inst[dec_num][0] = value - ENTRY_START_ADDRESS; \
            buffer_inst[dec_num][0] <<= TARGET_MOVE; \
            buffer_inst[dec_num][0] += 2; \
            break; \
        case (4): \
        case (5):\
            buffer_inst[dec_num][0] = 0; \
            buffer_inst[dec_num][0] += 1; \
            break; \
    }


/******************************Data Structures******************************/

/* a list of directive words and a list of operation words */
extern char operation_list[16][5];
extern char directive_list[4][8];


/* 
   Structure of label linked list

   the list stores the name of the labels from the input
   and the decimal number of the line of their declaration.
   note: entry labels will be in the range (2100-3124)and extern label will be in the range (4100-5124)
   after that it will be reduced to the appropriate address.
*/
struct label_node 
{
    char label[MAX_LABEL]; /* label name */
    int dec_num; /* label address */
    struct label_node* next; /* next label node */
};


/* 
   Structure of instruction

   the list stores the adresses of instruction operands 
   which translated to a binary machine code.
*/
struct instruction
{
  unsigned int ARE:2;/* A.R.E code */
  unsigned int target:3;/* target operand */
  unsigned int op:4;/* opcode */
  unsigned int source:3;/* source operand */
};


/**********************************Functions********************************/

/*-----functions of first_pass-----*/

/* skips whitespaces */
void skip_white2 (FILE*);

/* clears the string */
void clear_string2 (char *, int);

/* checks if the string is empty */
int is_empty_line (char*);

/* checks if the string is a number */
int is_num (char*);

/* converts negative number to two's complement */
int twos_complement_value(int);

/* checks if the word is a label, instruction or directive */
char type(char*);

/* checks if the word is a valid label */
int is_label (char*, char);

/* adds the label to the label list */
int add_label(struct label_node**, char*, int, int, int);

/* removes the label from the label list */
int remove_label(struct label_node**, int);

/* checks if the word is a valid instruction word */
int is_instruction(char*);

/* sets the instruction word */
int inst_word(char *,struct label_node**, unsigned int [BUFFER_MAX_SIZE][1], int*, int*, int);

/* checks if the word is a valid directive word */
int is_directive(char*);

/* sets the directive word */
int directive_word(unsigned int [BUFFER_MAX_SIZE][1], unsigned int [BUFFER_MAX_SIZE][1], int*, int*, int, char*, struct label_node**); 

/* gets first operand after instruction word */
int get_operands(struct label_node**, unsigned int [BUFFER_MAX_SIZE][1], int*, int*, int, struct instruction*, char*);

/* gets second operand after instruction word */
int get_operand2(char**,unsigned int [BUFFER_MAX_SIZE][1], int*, int*, int, struct instruction*);

/* converts operand to a binary machine code */
int operands_to_binary(unsigned int [BUFFER_MAX_SIZE][1], struct label_node**, struct instruction*, char*, char*, int*, int*, int);

/* checks if all the labels in the label list has been declared */
int is_dec_label(struct label_node**, char*);

/* checks if the word is a name of a register  */
int is_register(char*);

/* adds a label from extern to the label list */
int update_extern(struct label_node**,char*,int,int);


/*----functions of final_prints----*/

/* creates files for labels */
void label_to_files(struct label_node**, char*);

/* prints files int 64 base */
void print_base_64(char*, unsigned int [BUFFER_MAX_SIZE][1] , unsigned int [BUFFER_MAX_SIZE][1], int, int);


/*-----functions of second_pass----*/

/* second pass */
int second_pass (FILE*, char*, unsigned int [BUFFER_MAX_SIZE][1], unsigned int [BUFFER_MAX_SIZE][1], struct label_node**, int, int);

#endif

