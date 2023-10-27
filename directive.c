/*
  This program checks if the input is a valid directive word with valid parameters.
  For '.data' checks valid numbers and comma.
  For '.string' checks valid string with quotation marks.
  For '.entry' checks valid label without colon which exists in the input file.
  For '.extern' checks valid label without colon which isn't exists in the input file.
  Else returns error - not a valid directive word.
  If a label has been declared before '.entry' or '.extern' a warning message will be printed and this label will be removed.
*/


#include "data.h"

/* global directive list */
char directive_list[4][8] = {".data", ".string", ".entry", ".extern"};


/*
*  Function directive_word receives a word in string which includes a word and the whole line.
*  The function checks if the first word in the line is a valid directive word and checks 
*  the parameters after the word.
*  For '.entry' and '.extern' uses the parameter head for label removement and IC and DC for decimal number
*  of removed label check.
*  Eventually returns 1 for a valid directive word and parameters, else returns -1
* 
*  @param buffer_data - the data memory buffer
*  @param buffer_inst - the instruction memory buffer
*  @param IC - the instruction counter
*  @param DC - the data counter
*  @param lineNum - the line number of this line in the input
*  @param word - the input directive word which includes the whole line
*  @param head - the label list
*
*  @return  The value 1 for valid directive word and parameters, else returns -1
*/
int directive_word(unsigned int buffer_data[BUFFER_MAX_SIZE][1], unsigned int buffer_inst[BUFFER_MAX_SIZE][1], int *IC, int *DC, int lineNum, char *word, struct label_node** head) 
{

	/* general variables */
	int i = 0;
	int result = ERROR;
	char *param;
	char temp[MAX_LINE];

	/* variables of .data case */
	int comCount = 0;
	int numCount = 0;
	int digCount = 0;
	int oprCount = 0;

	/* .data check */
	if (strcmp(word, ".data") == 0) 
	{
        
		word = strtok(NULL,"\r\n");
		param = strtok(word,"\r\n");

		if (word == NULL) /* no parameters after directive word */
		{ 
			printf("Error: in line %d: Missing parameters\n",lineNum);
			return result;
		}
        
        	/* skips whitespaces from the start of the string */
        	while (word[i] == '\t' || word[i] == ' ' || word[i] == '\r') 
		{
            		i++;
        	}
    
        	/* no parameters after directive word */
        	if (word[i] == '\0') 
		{
            		printf("Error: in line %d: Missing parameters\n",lineNum);
			return result;
        	}
        
		while (word[i] != '\0') /* checks the parameters after directive word */ /* loop1 */
		{ 
    
    			/* invalid number */
    			if (word[i] != ' ' && word[i] != '\t' && word[i] != '\r' && word[i] != '+' && word[i] != '-' && word[i] != ',' && !isdigit(word[i])) 
			{		
						printf("Error in line %d: Invalid number\n",lineNum);
						return result;
      			}
    	
      			/* current character is a number or an operand of a number */
      			if (word[i] == '-' || word[i] == '+' || isdigit(word[i])) 
			{
      
      				if (numCount - comCount > 1) /* missing comma */
				{ 		
					printf("Error in line %d: Missing comma between parameters\n",lineNum);
            				return result;
          			}
      
		    		if (word[i] == '-' || word[i] == '+') /* current character is a operand of a number */
				{ 		
					oprCount++;
          			}
          
            			if ((word[i] == '-' || word[i] == '+') && isdigit(word[i-1])) /* operand after digit */
				{ 		
					printf("Error in line %d: Invalid number\n",lineNum);
					return result;
          			}
          
            			else if (isdigit(word[i])) /* current character is a number */
				{ 		
					digCount++;
          			}
        	
        			if (digCount > 0 && oprCount > 1) /* multiple operands */
				{ 		
					printf("Error in line %d: Multiple operands\n",lineNum);
					return result;
          			}
          
			} 
      
      			/* a valid number counted */
      			else if (digCount > 0) 
			{   
      	    			numCount++;
            			oprCount = 0;
            			digCount = 0;
      			}
      
        		/* operand without a valid number */
        		else if (oprCount != digCount)
			{   
      				printf("Error in line %d: Invalid number after operand\n",lineNum);
				return result;
        		}
      
        		/* operand without a valid number */
			 if (!isdigit(word[i]) && oprCount > 1) 
			{ 		
				printf("Error in line %d: Operand without a number\n",lineNum);
				return result;
			}
			
        		/* multiple operands */
    			if ((word[i] == '-' || word[i] == '+') && oprCount > 1) 
			{ 		
				printf("Error in line %d: Multiple operands\n",lineNum);
				return result;
			}
			
      	
      			/* comma before first number */
    			if (word[i] == ',' && numCount == 0) 
			{ 		
				printf("Error in line %d: Comma before first parameter\n",lineNum);
				return result;
			}
      	
      			/* multiple commas */
      			if (word[i] == ',' && numCount - comCount == 0) 
			{    
      				printf("Error in line %d: Multiple commas\n",lineNum);
				return result;
      			}
      	
      			/* a valid comma counted */
      			else if (word[i] == ',') 
			{     
      				comCount++;
      			}
	
			i++;
		} /* end of loop1 */
    
		/* operand without number after last number */
        	if (oprCount != 0 && digCount == 0) 
		{
			printf("Error in line %d: Operand without a number\n",lineNum);
			return result;
		}
    
		/* comma after last number */
		if (numCount == comCount && digCount == 0) 
		{
			printf("Error in line %d: Comma after last parameter\n",lineNum);
			return result;
		}
        
        	param = strtok(param," \t\r\n,");

	    	while (param != NULL) 
		    {
		        if (atoi(param) > MAX_NUM || atoi(param) < -MAX_NUM-1) /* if the number is not in the range of 10 bits (two other bits for A.R.E)*/
    	        {
        	        printf("Warning: in line %d the number can not be represented in 10 bits\n",lineNum);
            	}
    			buffer_data[CURRENT_INDEX][0] = twos_complement_value(atoi(param));
            		buffer_inst[CURRENT_INDEX][0] = 3; /* marks empty in instruction buffer (there is no valid instruction with the binary code 11 so we can use it as empty)*/
	    		(*DC)++;
			param = strtok(NULL," \t\r\n,");
	    	}
        result = 1;
		return result;
	} /* end of .data check */

	/* .string check */
	else if (strcmp(word, ".string") == 0) { 

		param = strtok(NULL,""); /* reading the parameter (the string) */

        	if (param == NULL) /* no parameters after directive word */
		{ 
			printf("Error: in line %d: Missing parameter\n",lineNum);
			return result;
		}

        	if(param[0] != '"') /* quotation have to be at the first of the string */
        	{
            		printf("Error in line %d: Missing quotation marks before string\n",lineNum);
			return result;
        	}
        
        	i = 1;
        	while(i<strlen(param) && param[i] != '"') /* finding the quotation marks index */
        	{
            		i++;
       		}
        
        	if(param[i] != '"') /* quotation have to be at the end of the string */
       		{
            printf("Error in line %d: Missing quotation marks after string\n",lineNum);
			return result;
        	}
        	
        	i = 1;
        	while(i<strlen(param) && param[i] != '"') /* checking if all the String characters are printable */
        	{
            		if(param[i] < FIRST_PRINTABLE_CHAR || param[i] > LAST_PRINTABLE_CHAR) /* range og printable characters by ascii code */
            		{
                	printf("Error in line %d: String contains unprintable char\n",lineNum);
			    	return result;
            		}

            		i++;
       		}

        	i++;

        	while(i<strlen(param)) /* checking if there is any extra text */
        	{
            		if(param[i] != '\t' && param[i] != ' ' && param[i] != '\r' && param[i] != '\n')
            		{
                		printf("Error in line %d: Extra text after the parameter\n",lineNum);
			    	return result;
            		}

            		i++;
        	}

        	param = strtok(param," \t\r\n\""); /* reading the parameter (the string) */
        
		if(param == NULL)
		{
			buffer_data[CURRENT_INDEX][0] = 0 ;
            		buffer_inst[CURRENT_INDEX][0] = 3; /* marks empty in instruction buffer (there is no valid instruction with the binary code 11 so we can use it as empty)*/
		    	(*DC)++;
		}

		else
		{
			i = 0;

    	    		while (param[i] != '\0')
    	    		{
    	    			buffer_data[CURRENT_INDEX][0] = param[i]; /* the ascii code will be inserted */
                		buffer_inst[CURRENT_INDEX][0] = 3; /* marks empty in instruction buffer (there is no valid instruction with the binary code 11 so we can use it as empty)*/
    	    			(*DC)++;
    	    			i++;
	        	}

	        	/* null have to be at the end of the string */
	        	buffer_data[CURRENT_INDEX][0] = 0;
            		buffer_inst[CURRENT_INDEX][0] = 3;
            		(*DC)++;
        	}
        result = 1;
		return result; /* valid directive line */
	} /* end of .string check */

	/* .entry or .extern check */  
  	else if (strcmp(word, ".entry") == 0 || strcmp(word, ".extern") == 0) 
	{
            
        	i = word[2]; /* n if entry, x if extern */

        	/* a label declared before .entry */
		if (remove_label(head,(*IC+*DC+BUFFER_START_INDEX)) == (*IC+*DC+BUFFER_START_INDEX)) 
		{
            		printf("Warning in line %d: Label declaration before .entry won't be included\n",lineNum);
        	}

		word = strtok(NULL,"\r\n");
		if(word != NULL)
            	strcpy(temp,word);
        
        	/* no parameters after directive word */
		if (word == NULL) 
		{
			printf("Error: in line %d: Missing parameter\n",lineNum);
			return result;
		}
        
        	/* colon after label */
        	if(word[strlen(word)-1] == ':') 
		{
            		printf("Error in line %d: Colon after label\n",lineNum);
			return result;
        	}
        
        	word = strtok(temp,"\n \t\r");

        	if((word = strtok(NULL,"\n \t\r")) != NULL)
        	{
            		printf("Error in line %d: Extra text after label\n",lineNum);
           	 	return result;
        	}   
        
        	word = strtok(temp,"\n \t\r");

        	/* invalid label */
		if (is_label(word, 's') != ERROR) 
		{
			printf("Error in line %d: Invalid label\n",lineNum);
			return result;
		}
            
        	if(i == 110) /* ascii code of 'n' */
		{
			result = add_label(head, word ,ENTRY_START_ADDRESS ,lineNum, 0); /* note - check if the label exists in the label list in the end */
		}

        	if(i == 120) /* ascii code of 'x' */
		{
            		result = add_label(head, word ,EXTERN_START_ADDRESS ,lineNum, 0); /* note - check if the label exists in the label list in the end */
		}
        result = 1;
		return result;
	}  /* end of .entry or .extern check */

    return ERROR;    
} /* end of directive_word function */


/*
*  Function is_directive receives a string, and checks if this string is name 
*  of directive, if so then return its position in the directive_list, else returns -1.
* 
*  @param word - the received word in string
*
*  @return  The value 1 if the word is directive, else returns -1 
*/
int is_directive(char *word)
{
    int i;
    int size = 4; /* size of the directive_list */
    for(i=0;i<size;i++) /* passing over all the list */
    {
        if(strcmp(word, directive_list[i]) == 0) /* found */
	{
        	return i;
	}
    }

    return ERROR;
} /* end of is_directive function */

/*
*  Function remove_label, to remove labels before .extern and .entry.
*  This function receives an address, and checks is this address has defined,
*  if so, then removes the label with the received address, and returns the received address.
*  If the label has removed successfully, else returns ERROR (-1).
* 
*  @param head - the label list
*  @param decNum - the decimal number of the removed label
*
*  @return  The decimal number of the removed label, else returns -1
*/
int remove_label(struct label_node **head, int decNum) 
{
    
	struct label_node* current = *head;
	struct label_node* previous = NULL;
	int result = ERROR;
	
  	/* checks if an equal label from the specifed parameters is exist in the list */
  	while (current != NULL) /* loop1 */
	{ 
  
  		/* found the label which will be removed */
  		if (current->dec_num == decNum) 
		{ 
        		result = current->dec_num; /* label has been removed successfully, add warnning or successful removement */
        
    			if (previous == NULL) /* removed label is head */
			{ 
      				*head = current->next; 
      			}

      			else 
			{
    				previous->next = current->next; /* removed label isn't head */
      			}
      
      			free(current); /* free memory */
      			break;
  		}
    
    		/* checks the next element in the list */
    		previous = current;
  		current = current->next;
  	} /* end of loop2 */
  
	return result;
} /* end of remove_label function */
