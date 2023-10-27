/*
  This program checks if the input is a valid instruction word with valid operands.
  If valid - register the operands to a binary machine code, else returns error.
*/


#include "data.h"

/* global instructions list */
char operation_list[16][5] = {"mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop"};


/*
*  Function inst_word receives a word in string which includes a word and the whole line.
*  Eventually returns 1 for a valid instruction word and operands, else returns -1.
* 
*  @param inst_name - the instruction name
*  @param head - the label list
*  @param buffer_inst - the instruction memory buffer
*  @param IC - the instruction counter
*  @param DC - the data counter
*  @param lineNUm - the line number of this line in the input 
*
*  @return  The value 1 for valid instruction word and operands, else returns -1
*/
int inst_word(char *inst_name,struct label_node **head, unsigned int buffer_inst[BUFFER_MAX_SIZE][1], int *IC, int *DC, int lineNum)
{
    int errInd = 0; /* error indicator */
    char *word = NULL; /* word to tokenize */
    char word2[MAX_LINE]; /* word2 to store the operands */
    struct instruction inst; /* instruction object to store ARE, and opcode and the operands (if exist) */
    
    inst.op = is_instruction(inst_name); /* getting opcode by calling is_instruction function */

    switch(inst.op) /* cases with operands(one or two) */ /* switch1 */
    {   
    	case(0): case(1): case(2): case(3): case(4): case(5): case(6): case(7):
    	case(8): case(9): case(10): case(11): case(12): case(13):

        	word = strtok(NULL,"\r\n"); /* tokenize operands */

        	if(word != NULL)
		{
                	strcpy(word2,word); /* saving all the operands in a string*/
		}
            
            	inst.ARE = 0;
            	errInd = get_operands(head, buffer_inst, IC, DC, lineNum, &inst, word); /* to read the rest of operands */

            	if (errInd != ERROR)
            	{
                	word = strtok(word2,", \r\t"); /* first operand only in word*/
                	word = strtok(NULL," \t\n\r"); /* second operand only in word */

                	if(word != NULL && strcmp(word,",") == 0)
			{
                    		word = strtok(NULL," \t\n\r"); /* second operand only in word */
			}

                	switch(inst.op) /* instructions with 2 operands */ /* switch2 */
                	{   
                    		case(0): case(1): case(2): case(3): case(6):
                        		word = strtok(NULL,"\n\r \t"); /* tokenize after second operand to check if there any extra text*/
                    			break;
                	} /* end of switch2 */

                	if(word != NULL)
                	{
                    		printf("Error in line %d, extra text after command\n",lineNum);
                    		errInd = ERROR;
                	}   
            	}

            	break;

        case(14): case(15): 
            	/* one time less tokenizing in case of instruction with one operand */
            	if((word = strtok(NULL,"\n\r")) != NULL)
            	{
                	printf("Error in line %d, extra text after command\n",lineNum);
                	errInd = ERROR;
            	}

            	else /* instruction with one operand */
            	{   
                	inst.ARE = 0;
                	inst.source = 0;
               		inst.target = 0;
               		buffer_inst[CURRENT_INDEX][0] += inst.ARE+(inst.target<<TARGET_MOVE)+(inst.op<<OP_MOVE)+(inst.source<<SOURCE_MOVE);
               		(*IC) ++;
       		}

	break;
    } /* end of switch1 */

    return errInd; /* returning error indicator */
} /* end of inst_word function */


/*
*  Function get_operands receives the first operand and checks if its valid and if registered.
*  Eventually returns non negative value for valid operand, else returns -1.
*
*  @param head - the label list
*  @param buffer_inst - the instruction memory buffer
*  @param IC - the instruction counter 
*  @param DC - the data counter
*  @param lineNum - the number line of this line 
*  @param inst - the instruction list
*  @param word - the recieved first operand in string
*
*  @return  The non negative value for valid operand, else returns -1
*/
int get_operands(struct label_node **head,  unsigned int buffer_inst[BUFFER_MAX_SIZE][1], int *IC, int *DC, int lineNum, struct instruction *inst, char *word)
{
	char *operand1 = '\0'; /* for first operand */
	char *operand2 = '\0'; /* for second operand */
	int i = 0;
	int comma = 0; /* commas counter between the operands */
    
	switch((*inst).op) /* instructions with one operand will two operands */ /* switch1 */
	{   
		case(0): case(1): case(2): case(3): case(6):

            		operand1 = strtok(word,"");

            		if(operand1 == NULL)
            		{
                		printf("Error in line %d: Missing operands\n",lineNum);
                		return ERROR;
            		}

            		while(*(operand1+i) != '\0') /* passing over the paramters to count commas */ /* loop1 */
            		{
            			if(*(operand1+(i++)) == ',') /* first comma has found */
                		{
                			if(*(operand1+(i++)) == ',') /* second comma in a row */
                    			{
                        			printf("Error in line %d: more than one comma after first operand\n",lineNum);
                        			return ERROR;
                    			}

                    			comma ++;
                    			break; /* only one comma after first operand */
				}
			} /* end of loop1 */

            		if(comma == 0) /* no commas */
            		{
                		printf("Error in line %d: Missing comma between first operand and second operand \n",lineNum);
                		return ERROR;
            		}

            		operand1 = strtok(operand1," \t\n\r,"); /* tokenizing 1st operand */
        		break;
        		default: /* the rest of cases with one operands */
            		operand1 = strtok(word," \t\n\r"); /* tokenizing 1st operand in the case of instruction that don't have 2 operands (without commas) */
        		break;
    	} /* end of switch1 */

	/* first operand is register */
	if(is_register(operand1) != ERROR)
	{
		switch((*inst).op) /* encoding per opcode */ /* switch2 */
		{   
			case(0): case(1): case(2): case(3): case(6):
				(*inst).source = 5;
				break;

			case(4): case(5): case(7): case(8): case(9): case(10): case(11): case(12): case(13):
				(*inst).target = 5;
                		(*inst).source = 0;
            			break;
        	} /* end of switch2 */
    	}

    	else /* first operand is valid number */
    	{   
        	if(is_num(operand1) != ERROR)
        	{
           		switch((*inst).op) /* switch3 */
        		{   /* encoding per opcode */
            			case(0): case(1): case(2): case(3): case(6):
                			(*inst).source = 1;
            				break;
            			case(4): case(5): case(7): case(8): case(9): case(10): case(11): case(12): case(13):
                			(*inst).target = 1;
                			(*inst).source = 0;
            				break;
        		} /* end of switch3 */
        	}

        	else
        	{   /* first operand is valid label */
            		if(is_label(operand1, 'o') != ERROR)
            		{
                		switch((*inst).op) /* encoding per opcode */ /* switch4 */
                		{   
                    			case(0): case(1): case(2): case(3): case(6):
                        			(*inst).source = 3;
                    				break;
                    			case(4): case(5): case(7): case(8): case(9): case(10): case(11): case(12): case(13):
                        			(*inst).target = 3;
                        			(*inst).source = 0;
                    				break;
                		} /* end of switch4 */
            		}

            		else
            		{
                		printf("Error in line %d: invalid first operand\n",lineNum);
                		return ERROR;
            		}
        	}
    	}

	switch((*inst).op) /* continue to read the 2nd operand of instructions with two operands */ /* switch5 */
        {   
        	case(0): case(1): case(2): case(3): case(6):
                	if((get_operand2(&operand2, buffer_inst, IC, DC, lineNum, inst)) == ERROR)
                	{
                    		return ERROR;
                	}

            		break;
        } /* end of switch5 */

    	/* inserting to memory image by calling the function operands_to_binary */
    	return operands_to_binary(buffer_inst, head, inst, operand1, operand2, IC, DC, lineNum); /* checking if the operands are appropriate to this instruction */
} /* end of get_operand function */


/*
*  Function get_operand2 receives the second operand and checks if its valid and if its registered.
*  Eventually returns non negative value for valid operand, else returns -1.
*
*  @param operand2 - the received second operand 
*  @param buffer_inst - the instruction memory buffer
*  @param IC - the instruction counter 
*  @param DC - the data counter
*  @param lineNum - the number line of this line 
*  @param inst - the instruction list
*
*  @return  The non negative value for valid operand, else returns -1
*/
int get_operand2(char **operand2 , unsigned int buffer_inst[BUFFER_MAX_SIZE][1], int *IC, int *DC, int lineNum, struct instruction *inst)
{
	char *word;
    
	/* reading next operand */
	word = strtok(NULL," \r\t\n");

	if(word != NULL && strcmp(word,",") == 0)
	{
        	word = strtok(NULL," \r\t\n");
	}

   	if(word == NULL)
    	{
        	printf("Error in line %d: Missing second operand\n",lineNum);
		return ERROR;                
    	}

    	if(*word == ',') /* removing the comma for operand test */
    	{
        	*word = '\0'; /* placing NULL to remove the comma in the first cell*/
        	word ++;
    	}

    	*operand2 = strtok(word,""); /* tokenizing the whole word */
    
    	if(operand2 == NULL)
    	{
        	printf("Error in line %d: Missing second operand\n",lineNum);
        	return ERROR;
    	}
    
    	/* second operand is register */
    	if(is_register(*operand2) != ERROR)
    	{
        	(*inst).target = 5; /* to encode the appropriate address */
    	}

    	else /* second operand is a valid number */
    	{   
        	if(is_num(*operand2) != ERROR)
        	{
            		(*inst).target = 1;
        	}

        	else /* second operand is a valid label */
        	{       
                	if(is_label(*operand2, 'o') != ERROR)
                	{
                    		(*inst).target = 3;
                	}

                	else
                	{
                    		printf("Error in line %d: invalid second operand\n",lineNum);
                    		return ERROR;
                	}
        	}
    	}

    	return 0; /* no errors */
} /* end of get_operand2 function */


/*
*  Function operands_to_binary receives two operands and check if they are valid for instruction word.
*  Eventually returns non negative value for valid operands for instruction word, else returns -1.
* 
*  @param buffer_inst - the instruction memory buffer
*  @param head - the label list
*  @param inst - the instruction list
*  @param operand1 - the received first operand 
*  @param operand2 - the received second operand 
*  @param IC - the instruction counter 
*  @param DC - the data counter
*  @param lineNum - the number line of this line 
*
*  @return  The non negative value for valid operands for instruction word, else returns -1
*/
int operands_to_binary(unsigned int buffer_inst[BUFFER_MAX_SIZE][1], struct label_node **head, struct instruction *inst, char *operand1, char *operand2, int *IC, int *DC, int lineNum)
{
	int address; /* to store labels address */
	int errInd = 0; /* error indicator */

	switch((*inst).op) /* checking if the operands types are appropriate to the current instruction */ /* switch1 */
	{   
		case(0): 
            		if((*inst).target == 1) /* mov instruction can't have a number in second operand */
            		{
                		printf("Error in line %d: for this instruction, target operand can not be a number\n",lineNum);
                		errInd = ERROR;
            		}

        		break;

        	case(6):
            		if((*inst).source != 3) /* lea instruction's first operand have to be only a label */
            		{
                		printf("Error in line %d: for this instruction, source operand have to be an address\n",lineNum);
                		errInd = ERROR;
            		}

            		if((*inst).target == 1) /* lea instruction can't have a number in second operand */
            		{
                		printf("Error in line %d: for this instruction, target operand can not be a number\n",lineNum);
                		errInd = ERROR;
            		}

        		break;

        	case(5):
            		if((*inst).target == 1) /* clr instruction can't have a number in second operand */
            		{
                		printf("Warning in line %d: for this instruction, there is no meaning to initialize unstored value to zero\n",lineNum);
            		}

        		break;

       		case(7): 
            		if((*inst).target == 1) /* inc instruction can't have a number in second operand */
            		{
                		printf("Warning in line %d: for this instruction, there is no meaning to increase unstored value\n",lineNum);
            		}

        		break;

        	case(8): 
            		if((*inst).target == 1) /* dec instruction can't have a number in second operand */
            		{
                		printf("Warning in line %d: for this instruction, there is no meaning to decrease unstored value\n",lineNum);
            		}

        		break;

        	case(11): 
            		if((*inst).target == 1) /* red instruction can't have a number in second operand */
            		{
                		printf("Error in line %d: for this instruction, target operand can not be a number\n",lineNum);
                		errInd = ERROR;
            		}

        		break;

    	} /* end of switch1 */

        if(errInd == ERROR) /* stopping encoding if there is any error */
	{
        	return errInd;
        }

        if((((*inst).op >= 0 && (*inst).op <= 3) || (*inst).op == 6)) /* instructions with two operands */
        {   /* inserting the binary code with using bitwise and moving to proper position according to instruction memory distribution */
        	buffer_inst[CURRENT_INDEX][0] += (*inst).ARE+((*inst).target<<TARGET_MOVE)+((*inst).op<<OP_MOVE)+((*inst).source<<SOURCE_MOVE);
            	(*IC) ++;
            
            	switch((*inst).source) /* switch2 */
            	{   /* first operand encoding */
                	case(1): /* the operand is a number */
                    		buffer_inst[CURRENT_INDEX][0] = twos_complement_value(atoi(operand1)); /* using atoi to convert the string to integer */
                    		buffer_inst[CURRENT_INDEX][0] <<= TARGET_MOVE; /* to move 2 bits left we use TARGET_MOVE */
                    		(*IC)++;
                		break;

                	case(3): /* the operand is a label */
                    		address = is_dec_label(head, operand1); /* getting label address if declared*/

                    		if(address != ERROR && address != ENTRY_START_ADDRESS) /* if the label exists and not equal to 2000 (defined and not only declared) */
                    		{
                        		if(address >= EXTERN_START_ADDRESS) /* if declared as extern so we will add the label with the current index of memory image */
                        		{
                            			add_label(head, operand1, CURRENT_INDEX, lineNum, 0);
                        		}

                        		LABEL_CODING(address, CURRENT_INDEX) /* macro to encode the label */
                    		}

                    		else /* label not defined yet, so insert to ARE 2 (the coding is absolute) */
                    		{
                        		buffer_inst[CURRENT_INDEX][0] = 2;
                    		}

                    		(*IC)++;
                		break;

                	case(5): /* the operand is a register */
                    		buffer_inst[CURRENT_INDEX][0] = (operand1[2] - '0'); /* inserting register number */
                    		buffer_inst[CURRENT_INDEX][0] <<= 7; /* moving it to bits 7-11 (it's a valid register so it's position will be in bits 7-11) */

                    		if((*inst).target == 5)
                    		{   /* if the second operand also a register so the register number will be inserted in the same memory image index */
                       			 buffer_inst[CURRENT_INDEX][0] += ((operand2[2] - '0')<<TARGET_MOVE); /* inserting to bits 2-6 */
                    		}

                    		(*IC)++;
                		break;
            	} /* end of switch2 */

            	/* second operand encoding */
            	switch((*inst).target) /* switch3 */
            	{
                	case(1): /* the operand is a number */
                    		buffer_inst[CURRENT_INDEX][0] = twos_complement_value(atoi(operand2));
                    		buffer_inst[CURRENT_INDEX][0] <<= TARGET_MOVE; /* to move 2 bits left we use TARGET_MOVE */
                    		(*IC) ++;
                		break;

                	case(3): /* the operand is a label */
                    		address = is_dec_label(head, operand1);

                    		if(address != ERROR && address != ENTRY_START_ADDRESS)
                    		{
                        		if(address >= EXTERN_START_ADDRESS)
                        		{
                            			add_label(head, operand1, CURRENT_INDEX, lineNum, 0);
                        		}

                        		LABEL_CODING(address, CURRENT_INDEX) /* macro to encode the label */
                    		}

                    		else
                    		{
                        		buffer_inst[CURRENT_INDEX][0] = 2;
                    		}

                    		(*IC)++;
                		break;

                	case(5): /* the operand is a register */
                    		if((*inst).source != 5) /* we finished the case when source = target = 5 */
                    		{
                       	 		buffer_inst[CURRENT_INDEX][0] = (operand2[2] - '0');
                        		buffer_inst[CURRENT_INDEX][0] <<= TARGET_MOVE; /* inserting to bits 2-6 */
                        		(*IC) ++;
                    		}

                		break;
            	} /* end of switch3 */
            
        }

        else /* instructions with one operand */
        {   /* encoding the collected opcode and operands types and ARE to the memory image */
        	buffer_inst[CURRENT_INDEX][0] += (*inst).ARE+((*inst).target<<TARGET_MOVE)+((*inst).op<<OP_MOVE)+((*inst).source<<SOURCE_MOVE);
            	(*IC) ++;
            	switch((*inst).target) /* switch4 */
            	{
                	case(1): /* the operand is a number */
                    		buffer_inst[CURRENT_INDEX][0] = twos_complement_value(atoi(operand1));
                    		buffer_inst[CURRENT_INDEX][0] <<= TARGET_MOVE; /* to move 2 bits left we use TARGET_MOVE */
                    		(*IC)++;
                		break;

                	case(3): /* the operand is a label */
                    		address = is_dec_label(head, operand1);

                    		if(address != ERROR && address != ENTRY_START_ADDRESS)
                    		{
                        		if(address >= EXTERN_START_ADDRESS)
                        		{
                            			add_label(head, operand1, CURRENT_INDEX, lineNum, 0);
                        		}

                        		LABEL_CODING(address, CURRENT_INDEX) /* macro to encode the label */
                    		}

                    		else
                    		{
                        		buffer_inst[CURRENT_INDEX][0] = 2;
                    		}

                    		(*IC)++;
                		break;

                	case(5): /* the operand is a register */
                    		buffer_inst[CURRENT_INDEX][0] = (operand1[2] - '0');
                    		buffer_inst[CURRENT_INDEX][0] <<= TARGET_MOVE;
                    		(*IC)++;
                		break;
            	} /* end of switch4 */
        }

    	return errInd;
} /* end of operands_to_binary function */


/*
*  Function is_register receives a string, it checks if the string is name of register.
*  Eventually returns the register number when the string is a name of register, else returns -1.
* 
*  @param word - the received string 
*
*  @return  The value of register, else returns -1
*/
int is_register(char *word)
{
    /* register name is about @r with a number 0-7 (3 characters)*/
    if(strlen(word) == 3)
    {
        if(word[0] == '@' && word[1] == 'r' && word[2]>= '0' && word[2] <= '7')
	{
            return atoi(&word[2]); /* returns the register number by using atoi function to return it as an integer */
	}
    }    

    return ERROR; /* not a  name of register */
} /* end of is_register function */
