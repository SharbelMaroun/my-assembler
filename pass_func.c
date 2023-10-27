/*
*  This program contains functions for first_pass .
*/

#include "data.h"


/*
*  function skip_white2 skips tabs and spaces in the file
*  till the next char that is not tab or space
* 
*  @param pf - received file pointer    
*/
void skip_white2 (FILE *pf)
{
	char get;
	while (((get = fgetc(pf)) != EOF) && NEW_WORD); /* skipping the tabs and spaces (NEW_WORD macro) */

	if(get != EOF) /* when we reach EOF we dont will not move backwards */
	{
		fseek(pf,-1,SEEK_CUR); /* moving one position backwards from current position */
	}
} /* end of skip_white2 function */


/*
*  function clear_string2 clears received string
*  till the received size (second parameter)
* 
*  @param word - received string
*  @param size - received string's size
*/
void clear_string2(char *word,int size)
{
    int i = 0;
    while(i < size) /* till size */
    {
        word[i] = '\0'; /* placing '\0' in every cell */
        i++;
    }
} /* end of clear_string2 function */


/*
*  function is_label checks if the received string have valid label syntax.
*  for valid label it returns 1, else returns ERROR
* 
*  @param word - received string
*  @param char - received indicator. 'o' for operand or 's' for starting of line
*  @return  The value 1 if valid label, else ERROR(-1)
*/
int is_label (char *word,char o)
{
    int i = 0;
    char word2[MAX_LABEL]; /* in case of receiving 's' in the second operand */
    
    clear_string2(word2, MAX_LABEL); /* clearing string from garbage characters */
    if(word == NULL || (!islower(word[i]) && !isupper(word[i]))) /* checking first character is not lowercase or uppercase */
        return ERROR; /* not valid so we return ERROR */
    
    if(strlen(word) > MAX_LABEL) /* MAX_LABEL = 31 */
        return ERROR;
    
    if(o == 's') /* if we receive 's' as second operand so after the label we have ':' */
    {
        strncpy(word2,word,strlen(word)-1); /* cutting ':' */
    }
    else
    {
        strncpy(word2,word,strlen(word)); /* if we receive 'o' so dont have ':' after the label */
    }
    
    if(word[strlen(word)-1] != ':' && o == 's') /* o = "start" or o = "operand"*/
    {
        return ERROR;
    }
    
    if(is_instruction(word2) != ERROR) /* label cant be name of instruction */
    {
        return ERROR;
    }
    if(is_directive(word2) != ERROR) /* label cant be name of directive */
    {
        return ERROR;
    }
        
    for(i=1;i<strlen(word2);i++) /* checking the whole word characters validity */
    {
        if(!islower(word[i]) && !isupper(word[i]) && !isdigit(word[i])) /* if not a number or A-Z or a-z, its invalid label */
        {
            return ERROR;
        }
    }

    return 1; /* valid label */
} /* end of is_label function */


/*
*  function is_num checks if the received string have valid integer.
*  for valid num it returns 1, else returns ERROR
* 
*  @param word - received string
*  @return  The value 1 for valid num, else return ERROR
*/
int is_num(char *word)
{
	int i=0;

	if(word[i] == '-' || word[i] == '+') /* checks if this is negative number so we can continue checking */
	{
		i++;
	}    

    	while(word[i] != '\0') /* number validity test */ /* loop1 */
    	{
        	if(!(isdigit(word[i]))) /* using isdigit() function */
		{
        		return ERROR; /* we give a sign that this is not valid number */
		}
        
        	i++;
    	} /* end of loop1 */
    
    	if (atoi(word) > MAX_NUM || atoi(word) < -MAX_NUM-1) /* if the number is not in the range of 10 bits (two other bits for A.R.E)*/
    	{
        	printf("Warning: the number can not be represented in 10 bits\n");
        	return 1;
    	}

    	return 1; /* we give a sign that this is valid number */
} /* end of is_num function */


/*
*  function twos_complement_value receives an integer, if this integer is greater or equal to zero
*  the function will return the received number, and if the received number is less than zero
*  it will reprensent the number in 2's complemnet and return its decimal representation
*  note: this function assumes that the received number is an integer.
* 
*  @param value - the received integer 
*
*  @return  The decimal representation in 2's complemnet
*/
int twos_complement_value(int value)
{
    
	int i = 0; /* bits counter */
	int mask = 1; /* mask to use bitwise with it */
	int result = 0; /* to store the result */
	int absValue = abs(value); /* storing the absolute value of teh received number */
    
	if(value >= 0) /* if the n=received number is not negatve so the received value will be returned */
	{
		return value;
	}
        
	while(i<sizeof(int)*8) /* till the last bit of integer */ /* loop1 */
	{ /* we are searching the first bits that contains 1 (right to left) */
		if((absValue & (mask<<i)) != 0) /* if the number after the & operation is not zero so the bits in this position is 1 */
		{
			result  += (mask<<i); /* we "turn on" only the bit that is in the current postion and add it to the result */

			while(i<sizeof(int)*8) /* till the last bit of integer */ /* loop2 */
			{
				if((absValue & (mask<<i)) == 0) /* now we do the opposite, and in every position of bits, we turn them over */
				{
					result  += mask<<i; /* adding to result */
				}

				i++;
			} /* end of loop2 */
        	}

        	i++;
	} /* end of loop1 */

	return result;
} /* end of twos_complement_value function */


/*
*  function is_instruction receives a string, and checks is this string is name 
*  of instruction, if so then return its opcode, else returns -1.
* 
*  @param word - the received string 
*  @return  The 
*/
int is_instruction(char *word)
{
    int i; /* counter */
    int size = 16; /* 16 opcodes (0-15) */
    for(i=0;i<size;i++) /* passing over all the array of instructions names */
    {
        if(strcmp(word, operation_list[i]) == 0) /* if we  find it we will return its opcode */
	{
        	return i;
	}
    }

    return ERROR;
} /* end of is_instruction function */


/*
*  function is_empty_line receives a string that contains a line, and checks is this string is empty.
*  return 1 for empty line, and 0 if there is a char than not tab or space.
* 
*  @param line - the received string 
*
*  @return  The value 1 if the received string is "empty line", else return 0
*/
int is_empty_line (char *line)
{
    int i = 0; /* counter */
    
    while(line[i] != '\0') /* till the last char in the string */
    {
        if(line[i] != '\t' || line[i] != ' ' || line[i] != '\r') /* not tab or space */
            return 0;
    }

    return 1; /* empty string */
} /* end of is_empty_line function */


/*
*  function add_label receives a pointer to the head of the labels list
*  and the label name and its address, to create a label node and to insert the received
*  information in the node's appropriate fields, and receives the parameter lineNum 
*  that indicates the line number.
*  in the labels list, the addresses that greater than 4000 means taht this label has declared as extern and their they apear as operand in these addresses.
*  and the addresses that equal to 4000 means that this label has declared as .extern and they have'nt defined till now.
*  addresses in the range (2000,4000), have declared as .entry and defined as label in the source code
*  addresses that equal to 2000 means that this label has declared as .entry and they have'nt defined till now.
*  addresses less than 2000 is defined as a normal label with its address, and may it will declared as .entry in the following code
* 
*  @param head - a pointer to the head (**)
*  @param label - label's name
*  @param dec_num - the address of the label
*  @param line_num - the current line number in the current source file 
*  @param def - if a definition of a label or operand
*
*  @return  The value 0 if the label has added successfully, else return ERROR
*/
int add_label(struct label_node **head, char* label, int dec_num, int line_num, int def) 
{  
	/* checks if the label already exists in the list */
	struct label_node* current = *head; /* pointer to current label node */
	struct label_node* last; /* last node */
	struct label_node* new_node; /* new node that may be created */

    	if(dec_num == ENTRY_START_ADDRESS) /* in the case of .entry */
    	{
        	while (current != NULL) /* till the last label node */ /* loop1 */
        	{
            		if ((strcmp(current->label, label) == 0)) /* label name found in the labels list */
            		{
                		if((current->dec_num) < ENTRY_START_ADDRESS) /* when .entry appears after declatation of the label */
                		{
                    			(current->dec_num) += ENTRY_START_ADDRESS; /* we will add 4000 to the exist address */
                		}

                		else if((current->dec_num) >= EXTERN_START_ADDRESS) /* when this label has declared as .extern before */
                		{
                    			printf("Error: Label from line %d has declared before as extern label, it can't be declared also as entry label\n", line_num);
                    			return ERROR;
                		}

                		return 0; /* may this label has updated by other .entry before so we put return outside the if() */
            		}

            		current = current->next; /* going to next label node */
        	} /* end of loop1 */

		goto option1; /* if label not found we will create for it a new label node */
	}
    
	if(dec_num == EXTERN_START_ADDRESS) /* in the case of .entry */
	{
        	while (current != NULL) /* loop2 */
        	{
            		if (strcmp(current->label, label) == 0) /* label found*/
            		{
                		if((current->dec_num) >= ENTRY_START_ADDRESS && (current->dec_num) < EXTERN_START_ADDRESS) /* if has defined before as entry or declared as .entry */
                		{
                    			printf("Error: Label from line %d has declared before as entry label, it can't be declared also as extern label\n", line_num);
                    			return ERROR;
                		}

                		if((current->dec_num) >= EXTERN_START_ADDRESS) /* it has declared, or updated */
                		{
                    			return 0; 
                		}

                		if((current->dec_num) < ENTRY_START_ADDRESS) /* normal label */
                		{
                    			printf("Error: Label from line %d has declared before, it can't be declared also as extern label\n", line_num);
                    			return ERROR;
                		}
           		    }

			current = current->next;
        	} /* end of loop2 */
        	
        	goto option1; /* if label not found we will create for it a new label node */
	}
        
	while (current != NULL) /* loop3 */
	{
        
		if (strcmp(current->label, label) == 0)
		{   /* the label has declared before as .entry or .extern and hasn't defined till now */
			if((current->dec_num) == ENTRY_START_ADDRESS) 
			{
                		if(def == 1)
				{
                    			(current->dec_num) += dec_num; /* adding the address from the instruction and directive buffer to it */
				}

                		return 0;
            		}

            		if((current->dec_num) == EXTERN_START_ADDRESS) /* has declared before as extern */
            		{
                		if(def == 1) /* definition request */
                		{
                    			printf("Error: Label from line %d has declared before as extern label, it can't be defined in this file\n", line_num);
                    			return ERROR;
                		}

                		(current->dec_num) += dec_num; /* adding the address from the memory image to it */

                		return 0;
            		}
            
            		if((current->dec_num) < EXTERN_START_ADDRESS) /* entry or normal Label can't be defined more than one time */
            		{
                		printf("Error: Label from line %d cannot be executed, Identical label already exists in the list\n", line_num);
                		return ERROR;
            		}

            		/* bigger than EXTERN_START_ADDRESS (4000) */
                	dec_num += EXTERN_START_ADDRESS; /* adding 4000 to the received address from the instruction and directive buffer */
                	while (current != NULL)  /* going to the last label node to create a new label node with extern address */ /* loop4 */
                	{
                		current = current->next;  
                	} /* end of loop4 */

                	goto option1; /* to create a new label node */
        	}

        	current = current->next;
    	} /* end of loop3 */

	if(def == 0) /* this is operand and not exist in the label list */
	{
		return 0;
	}
    
	/* creates new node */
    	option1:	
    	new_node = (struct label_node*) malloc(sizeof(struct label_node)); /* allocating memory for new label node */

    	if (new_node == NULL) /* if allocating memory failed */
    	{
        	printf("No enough memory to allocate in line %d, in function \"add_label\"\n", line_num);
        	return ERROR;
    	}
  
	/* adds data to the new node */
	strcpy(new_node->label, label); /* giving the name of the label to the new node */
    	new_node->dec_num = dec_num; /* giving the label address to the new node */
   	new_node->next = NULL; /* because it is the last node */

	/* the new node is the first node in the list */
	if (*head == NULL) 
	{ /* adds the node to the head of the list */
        	*head = new_node;
        	return 0;
    	}
	
	/* the new node isn't the first node in the list */
  	last = *head;
  	while (last->next != NULL) 
  	{  /* traverse to the end of the list */
  		last = last->next;
  	}

	last->next = new_node; /* adds the new node to the end of the list */
	return 0;
} /* end of add_label function */


/*
*  function type receives a string, and checks is this string is instruction or directive or valid label.
*  it returs a char: 'i' for instruction, 'd' for directive, 'l' for label,else it returs 'N'.
* 
*  @param word - the received string 
*
*  @return  The character: 'i' if the received word is instruction, 'd' if directive, 'l' if label else return 'N'.
*/
char type(char *word)
{
    /* calling the function to check and returns appropriate character */
	if (is_instruction(word) != ERROR)
	{
        	return 'i';
	}

	if((is_directive(word)) != ERROR)
	{
        	return 'd';
	}

	if (is_label(word,'s') != ERROR)
	{
        	return 'l';
	}

	return 'N'; /* */
} /* end of type function */


/*
*  function is_dec_label receives a struct label_node, and a string, it checks if the string
*  is a name of label that exists in the label list.
*  returns the label adrress if the label exists, else returns -1.
* 
*  @param head - pointer to the head of the label list
*  @param label - the received string 
*
*  @return  The address of the label if declared, else return ERROR (-1)
*/
int is_dec_label(struct label_node **head, char *label)
{ 
    struct label_node* current = *head; /* current label node to check them */
    while (current != NULL) 
    {
        if ((strcmp(current->label, label) == 0)) /* label name found */
        {
            return current->dec_num; /* returns the address */
        }
        current = current->next;
    }

    return ERROR; /* not found */
}

/*
*  function update_extern receives the labels list, and the name of the label that wanted to update
*  and the lines counter of the file. it checks if the received extern label has get an address as extern (before)
*  if so, then return 0, else it calls the add_label function to add the label if the addition is valid.
* 
*  @param head - the received label list
*  @param word - the received label name
*  @param dec_num - the received address
*  @param lineNum - the received lines counter
*
*  @return  The value 0 if the extern label has no need to be updated, else the returned value of the add_label function will be returned
*/
int update_extern(struct label_node **head, char* word, int dec_num, int lineNum)
{
    struct label_node *current = *head;

    while(current != NULL)
    {
        /* if exists (same name and address) */
        if((strcmp(current->label,word) == 0) && ((current->dec_num) == (dec_num + EXTERN_START_ADDRESS)))
	{
            return 0;
	}

        current = current->next;
    }

    return add_label(head,word,dec_num,lineNum, 0); /* add it to the tail of the label list */ 
}

