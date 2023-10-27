/*
*  This program contains functions for macro unfold.
*/

#include "pre_data.h"

/* global operation and instructions lists names */

int is_directive(char *word);
int is_instruction(char *word);

/**
*   This function cleares all the characters in the string from the start to
*   the size which its value is the index of the end of the clearing. 
* 
*   @param word - The word in string
*   @param size - The size of the string that will be cleared
*/
void clear_string(char *word,int size)
{
    int i = 0;
    while(i < size) /* till size */
    {
    	word[i] = '\0'; /* placing '\0' in every cell */
        i++;
    }
} /* end of clear_string function */

/**
*   This function skips tabs and spaces in the file
*   until a non tab or space character found or end of file.
* 
*   @param pf - pointer of file   
*/
void skip_white (FILE *pf)
{
    char get;
    while (((get = fgetc(pf)) != EOF) && NEW_WORD); /* skipping the tabs and spaces (NEW_WORD macro) */

    if(get != EOF) /* when we reach EOF we dont will not move backwards */
    {
    	fseek(pf,-1,SEEK_CUR); /* moving one position backwards from current position */
    }
} /* end of skip_white function */

/**
*   This function gets a word in string in the file from the 
*   start which is the file pointer to the size which its value
*   is the index of the end the check.
*   returns the number of the characters the function has read
*   note: this function assumes that the wo
*
*   @param pf - The pointer of the file
*   @param word - The word which will be returned
*   @param size - The last index of the word  
*
*   @return  The word between the pointer to the last index
*/
int get_word (FILE *pf, char *word, int size)
{
    char get; /* to get every character */
    int i = 0;

    /* getting the word till end of file or new word (\r\n and space) or the received size */
    while ((get = fgetc(pf)) != EOF && !(NEW_WORD) && !(NEW_LINE) && i < size-1)
        {
            *(word+i) = get; /* inserting the character to the word */
            i++;
        }

    if (get != EOF) /* if not end of file we will go backward to get the character that the loop read and didn't used */
    {
        fseek(pf,-1,SEEK_CUR);    
    }

    *(word+i) = '\0'; /* end of string */
    return i; /* returning the number of the characters the function has read */
} /* end of get_word function */

/**
*   This function gets a word in string and a chart pointer,
*   checks if the word is equal to a macro name 
*   from the chart.
* 
*   @param word - The word which is checked if its a macro name
*   @param chart - The chart of declared macros
*
*   @return  The mcro or NULL if the chart is empty
*/
struct mcrobj* is_dec_mcro(char *word, struct mcrobj *chart)
{
    while(chart != NULL)
    {
        if(strcmp(chart->mcro_name, word) == 0) /* the same name */
        {
            return chart; /* returning the mcro */
        }

        chart = chart->next;
    }

    return NULL;
} /* end of is_dec_mcro function */

/**
*   This function gets a word in string and checks
*   if the word is a new macro definition.
*   returns 1 if the received word is "mcro", else returns 0;
* 
*   @param word - The word which is checked if its a valid new macro
*
*   @return - The value 1 if the received word is mcro, else returns 0
*/
int is_new_mcro (char *word)
{
    char start_mcro [5] = "mcro"; /* the word the define a new mcro */

    if(strcmp(start_mcro,word) == 0) /* the received word is "mcro" */
    {   
        return 1;   
    }

    return 0;
} /* end of is_new_mcro function */

/**
*   This function adds a new macro to the macro list
* 
*   @param pf - The pointer of the file
*   @param file_name - The name of the file
*   @param word - The word which is the new macro name
*   @param chart_tail - The last macro in the list
*   @param chart_head - The first macro in the list
*
*   @return  The value 1 for successful addition, else returns -1
*/
int add_mcro(FILE *pf, char *file_name, char *word, struct mcrobj **chart_tail, struct mcrobj **chart_head)
{
	char get; /* to read the characters */
	char word2 [MAX_LINE]; /* helping string to store the words */
	char end_mac [9] = "endmcro"; /* mcro end of definition */
	char **temp1; /* temporary pointer for reallocating */ 
	int i = strlen(end_mac);
	struct mcrobj *o = (struct mcrobj*)malloc(sizeof(struct mcrobj)); /* allocating mcro object */

	if (o == NULL) /* memory allocation failed */
	{
        	perror("memory allocation failed\n");
        	return ERROR;
	}

	if ((*chart_head) == NULL)
	{
		(*chart_head) = o; /* first mcro object */
	}

	if ((*chart_tail) != NULL) /* connecting the last object with the new object */
	{
    		(*chart_tail)->next = o;
	}

	(*chart_tail) = o; /* updating tail */

	/* updating mcro fields */
	strcpy(o->mcro_name, word);
	o->next = NULL;
	o->lines_number = 0;
    
	while(1) /* while the mcro definition has not end */ /* loop1 */
	{	/* reading the lines and allocating memory for them */
		clear_string(word2,MAX_LINE);
        	skip_white(pf);
        	get = fgetc(pf);

        	if (NEW_LINE)
        	{
            		continue;
        	}

        	fseek(pf,-1,SEEK_CUR); /* reading the unused character that has read */
        	fgets(word2, MAX_LINE, pf); /* reading the whole line */
				/***** note: assuming that the mcro lines are valid *****/
        	if(strncmp(end_mac,word2,7) == 0) /* we check if this is the macro definition */
        	{
     			while(i<strlen(word2)) /* checking extra text after the definition ending of the macro */ /* loop2 */
            		{
                		if(word2[i] != '\n' && word2[i] != '\t' && word2[i] != ' ' && word2[i] != '\r')
                		{
                    			printf("Error: extra text after the definition ending of the macro %s \n", word);

                    			if(remove(file_name) != 0)
                    			{
                        			printf("note: the file %s has been created and couldn't be removed, its content may be incorrect, please do not refer to it \n", file_name);
                    			}

                    			return ERROR;
                		}

                		i++;
            		} /* end of loop2 */
                
			return 1;
        	}

		if (o->lines_number != 0) /* not the first time we enter the loop */
        	{
            		temp1 = (char**)realloc(o->lines, sizeof(char*) * (o->lines_number+1)); /* reallocating memory for the line */

            		if (temp1 == NULL)
            		{
                		perror("memory reallocation failed\n");
                		return ERROR;
            		}

            		o->lines = temp1;
            		o->lines [o->lines_number] = (char*)malloc(sizeof(char)* MAX_LINE); /* allocating array to the new array cell */

            		if (o-> lines [o->lines_number] == NULL)
            		{
                		perror("memory allocation failed\n");
                		return ERROR;
            		}
        	}

        	else /* the first line to allocate memory for */
        	{
            		o->lines = (char**)malloc(sizeof(char*));

            		if (o->lines == NULL)
            		{
                		perror("memory allocation failed\n");
                		return ERROR;
            		}

            		o->lines [0] = (char*)malloc(sizeof(char) * MAX_LINE); /* pointer of the array points to array with size of 81 */

            		if (o->lines [0] == NULL)
            		{
                		perror("memory allocation failed\n");
                		return ERROR;
            		}

        	}

		strcpy(o->lines [o->lines_number],word2); /* copying the line to the allocated memory */
            	o->lines_number++;
    	} /* end of loop1 */
} /* end of add_mcro function */


/**
*   This function unfolds the declared macros from the file.
* 
*   @param f - The file
*   @param mcro_pointer - The pointer of a macro
*/
void unfold_dec_mcro(FILE *f, struct mcrobj *mcro_pointer)
{
    int i;
    int size = mcro_pointer->lines_number; /* the number of the lines to unfold */
    for(i=0; i<size; i++) /* till line_number */
    {
        fputs(mcro_pointer->lines [i],f); /* printing the mcro lines to the file with the .am ending */
    }
} /* end of unfold_dec_mcro function */

