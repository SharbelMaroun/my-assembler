/* Preprocessor file, to unfold the macros */
/* The preprocessor assumes that the macros body are valid, and only checks macros starting and ending definition */

#include "pre_data.h"

int unfold_mcro(FILE *pf, char *file_name) /* (filename from the command line)
the file that we want to read from it is already opened*/
{
	char get = '\0';
	int i = 0; /* for counting */
	int j = 0; /* to free allocated memory */
	int errInd = 0; /* error indicator */
	struct mcrobj *chart_head = NULL; /* the head of chart of macros lines*/
	struct mcrobj *chart_tail = NULL; /* tail of macros chart */
	struct mcrobj *mcro_pointer = NULL;
	char word [MAX_LINE]; /* array to check every single word */
	FILE *f; /* temporary file pointer to write where it points */
	f = fopen(file_name,"w"); /* opening the file that we want write in it */
    
	if (f == NULL) /* the file to write cannot be opened */
	{
		printf("the file %s, to unfold the macros can not be opened\n", file_name);

        	if(remove(file_name) != 0)
        	{
            		printf("note: the file %s has been created and couldn't be removed, its content may be incorrect, please do not refer to it \n", file_name);
        	}

        	return ERROR;
    	}
    
	clear_string(word,MAX_LINE);

	while(get != EOF) /* reads the first character iin the line */ /* loop1 */
	{	
        	clear_string(word,MAX_LINE);
        	skip_white(pf);
        	get = fgetc(pf);

        	if (get == EOF) /* end of file */
        	{
            		break;
        	}

        	if (NEW_LINE) /* new line */
        	{
            		fputc('\n',f);
            		continue;
        	}

        	else
        	{
            		fseek(pf,-1,SEEK_CUR); /* going backward to read also the first character */
        	}

        	get_word (pf, word, MAX_LINE); /* reading the first word in the line*/
        	
        	/* .string is a directive that its parameter will be inserted to the memory, so the white spaces will not be ignored */
        	if(strcmp(word,".string") == 0)
        	{
        	    fputs(word,f);
        	    fputc(' ',f);
        	    skip_white(pf);
        	    while ((get = fgetc(pf)) != EOF && get != '\n') /* reading the whole line */
        	    {
        	        fputc(get,f);
        	    }
        	    if(get != EOF)
        	        fputc('\n',f);
        	    
        	    continue;
        	}
        
	    	if (chart_head != NULL)
	    	{
            		mcro_pointer = is_dec_mcro(word, chart_head); /* declared macro to unfold it */

	    		if (mcro_pointer != NULL)
            		{
                		unfold_dec_mcro(f,mcro_pointer);
                		continue;
            		}
		}

            	if(is_new_mcro (word)) /* mcro new definition */
            	{
			/* reading the mcro name */
                	clear_string(word,MAX_LINE);
                	skip_white(pf); /* there is seperation between "mcro" word and mcro definition */
                	get_word(pf, word, MAX_LINE);

                	if((is_instruction(word) != ERROR) || (is_directive(word) != ERROR)) /* mcro name can't be reserved word (instruction or directive) */
                	{
                    		printf("ERROR: in macro \"%s\": name of macro can't be a name of instruction or name of directive\n", word);
                    		if(remove(file_name) != 0)
                    		{
                        	printf("note: the file %s has been created and couldn't be removed, its content may be incorrect, please do not refer to it \n", file_name);
                    		}
                    		return ERROR;
                	}
                	if(is_new_mcro (word) || (strcmp(word,"endmcro") == 0)) /* mcro name can't be reserved word("mcro" or "endmcro") */
                	{
                    		printf("ERROR: in macro \"%s\": name of macro can't be mcro definition words\n", word);
                    		if(remove(file_name) != 0)
                    		{
                        	printf("note: the file %s has been created and couldn't be removed, its content may be incorrect, please do not refer to it \n", file_name);
                    		}
                    		return ERROR;
                	}

			if(is_dec_mcro(word, chart_head) != NULL) /* mcro name can't be defined more than one time */
                	{
                    		printf("ERROR: multiple definition of the mcro \"%s\"\n", word);
                    		if(remove(file_name) != 0)
                    		{
                        		printf("note: the file %s has been created and couldn't be removed, its content may be incorrect, please do not refer to it \n", file_name);
                    		}

                    		return ERROR;
                	}

                	skip_white(pf);
                	get = fgetc(pf); /* checking extra text after mcro definition */

                	if(get != '\n') /* it can only be new line */
                	{
                    		printf("ERROR: extra text after the definition of the macro %s\n", word);
                    		if(remove(file_name) != 0)
                    		{
                        		printf("note: the file %s has been created and couldn't be removed, its content may be incorrect, please do not refer to it \n", file_name);
                    		}

                    		return ERROR;
                	}

                	errInd = add_mcro (pf, file_name, word, &chart_head, &chart_tail); /* adding to the mcro list with its lines (till endmcro) */

			/* note: we assume that endmcro is exists according to mman 14 */
                	if (errInd == ERROR)
			{
                    		return ERROR;
                    	}
            	}

            	else /* every different line will be printed in the expanded file */
            	{
                	fputs(word,f);
                	fputc(' ',f);
            	}
    	} /* end of loop1 */

	/* free the mcros objects including it's lines */
	i = 0;
	while(chart_head != NULL) /* loop2 */
	{
        	mcro_pointer = chart_head;
        	chart_head = chart_head->next;
        	j = (mcro_pointer)->lines_number; /* for loop limit  */

        	for(i=0; i<j; i++) /* till lines_number */
        	{
        		free ((mcro_pointer)->lines[i]); /* free every line */
        	}

        	free ((mcro_pointer)->lines); /* free the lines "array" */
        	free (mcro_pointer); /* free the mcro object */
    	} /* end of loop2 */

	fclose(f);
	return 1;
} /* end of macro_unfold */
