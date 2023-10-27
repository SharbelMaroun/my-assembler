/*
*  This program is the first pass on the input file after pre-assembler.
*  The program allocates memory for the input and checks if the input is valid, recognizes the labels and assigns an adress value to them.
*  The adresses values are transferred to the second pass for binary code machine creation.
*  Every line of the input will be identified by a decimal number and a binary code machine number.
*  Eventually, frees the memory which has been allocated.
*/

#include "data.h"

int first_pass (FILE *file_ptr, char *filename) 
{
	/* note: According to page 36, line 9 in the C book of Brian W. Kernighan
	Dennis M. Ritchie, integer have at least 16 bits so integer can store 12 bits. */
             
	unsigned int buffer_inst[BUFFER_MAX_SIZE][1]; /* the memory image of instructions  */
	unsigned int buffer_data[BUFFER_MAX_SIZE][1]; /* the memory image of instructions  */
	int i; /* conter to initialize memory images */
	int lineNum = 1; /* line number counter */
	int IC = 0; /* instructions counter */
	int DC = 0; /* data counter */
	int errInd = 0; /* erroer indicator */
	int result = 0; /* function return values results */
	char *word; /* word to tokenize the the lines */
    	char line [MAX_LINE+2]; /* to read the line */
	char get; /* to read first character to recognize EOF and new lines */
	struct label_node *head = NULL; /* lable list pointer */
	struct label_node *current = NULL; /* lable list current pointer  */ 
	
    
	for (i = 0; i < BUFFER_MAX_SIZE; i++)  /* initialize the memory image of instructions to zero */
	{
		buffer_inst[i][0] = 0;
	}

	for (i = 0; i < BUFFER_MAX_SIZE; i++) /* initialize the memory image of data to zero */
	{
		buffer_data[i][0] = 0;
	}

	/* while the file didn't reach the end but imaginary computer memory reached the maximum  */
	while ((IC+DC+BUFFER_START_INDEX) < BUFFER_MAX_SIZE) /* loop1 */
	{
		clear_string2(line, MAX_LINE+2); /* clearing string every itiration */
		skip_white2 (file_ptr); /* skipping white before the sentence */
		get = fgetc(file_ptr); /* getting the first character int the sentece to check */

		if (get == '\n' || get == ';') /* if new line or comment we will ignore */
		{
			if(get == ';')
			{
				while ((get = fgetc(file_ptr)) != EOF && get != '\n'); /* reading the rest of the line */
			}

			lineNum++; /* going to next line */
			continue; /* to next iteration */
		}

		if(get == EOF)
		{
			break;
		}

		fseek(file_ptr,-1,SEEK_CUR); /* we read the first character so want to move one character backwards */
        fgets(line, MAX_LINE+2,file_ptr); /* read all the line */

        if(is_empty_line(line)) /* this func returns 1 if the line is empty */
    	{
        		lineNum++;
		continue;    
    	}

		/* if line length more than 81 including \n */
		if (((strlen(line) > MAX_LINE)) || ((strlen(line) > MAX_LINE-1) && ((line[strlen(line)-1] != '\n') || (line[strlen(line)-1] != '\r'))))
		{
			printf("Error: invalid line: line %d is too long\n", lineNum);
			while((get = fgetc(file_ptr)) != EOF && get != '\n'); /* skipping the rest of the line */
			errInd = ERROR; /* error indicator now will be ERROR (-1) */
		}

		else /* valid line length */
		{
			word = strtok(line," \r\n\t"); /* taking the word in the current line till new line or tab or space */

			switch(type(word)) /* we are checking if the word in this position is according to line structure*/
			{/* because label is optional so we have to handle the case with label and without label */
				case('l'):  /* valid label syntax */
				word[strlen(word)-1] = '\0';

                        	if((result = add_label(&head, word, (IC+DC+BUFFER_START_INDEX), lineNum, 1)) == ERROR) /* we will add it to label list */
				{
                       			errInd = ERROR;
                            	}

			       	word = strtok(NULL," \r\n\t"); /* next part of the line till \n or \t or space */

			       	if(word == NULL) /* label with empty line is undefined sentence */
			            {  
			                printf("Error: invalid line: line %d doesn't contain any defined sentence\n", lineNum);
			                errInd = ERROR;
			                lineNum++;
			                continue;
			            }

				switch(type(word)) /* because label is optional so we have to handle the case without label */
				{
			        	case('l'): /* declaring more than one label in a row   */
			                printf("Error in line %d: nested labels\n", lineNum);
			                errInd = ERROR;
			                break;

			                case('i'):  /* instruction */
			                /* calling instruction function and giving returned value to result */
        			            result = inst_word(word, &head, buffer_inst, &IC, &DC, lineNum); 
		        	            break;

			                case('d'): /* directive */ 
			                /* calling directive function and giving returned value to result */
			                    result = directive_word(buffer_data, buffer_inst, &IC, &DC, lineNum, word, &head);
        			            break;

		        	        default:
		        	        /* any other word is undefined command */
			                    printf("Error in line %d: undefined command\n", lineNum);
			                    errInd = ERROR;
        			        break;
				}

				break;

			        /* case without optional label at the beginning of the lien */
			        case('i'):  /* instruction */
			            result = inst_word(word, &head, buffer_inst, &IC, &DC, lineNum);
			            break;

			        case('d'):  /* directive_word */
			            result = directive_word(buffer_data, buffer_inst, &IC, &DC, lineNum, word, &head);
			            break;

			        default:    /* any other word is undefined command */
			            printf("Error in line %d: undefined command\n", lineNum);
			            errInd = ERROR;
			 	    break;
			}
		
		}

        	if(errInd != ERROR) /* if no error detected till now, update if there is an error */
		{
			errInd = result;
		}

		lineNum++; /* to next line */
	} /* end of loop1 */

	if ((get = fgetc(file_ptr)) != EOF) /* checking if there any text after we reach maximum memory image */
	{
		printf("Warning: reached to the maximum memory, the assembler won't check the text from line %d\n",lineNum);
	}

	if(errInd != ERROR) /* if there no errors in the first pass we enter the second pass */
	{
		rewind(file_ptr); /* returns the file position indicator to the beginning of the file to read it again */
		errInd = second_pass(file_ptr, filename, buffer_data, buffer_inst, &head, IC, DC); /* calling second_pass */
	}
	
	/* free label list */
	while (head != NULL)  /* loop2 */
    	{
        	current = head->next;
        	free(head);
        	head = current;
    	} /* end of loop2 */

	return errInd;	
} /* end of first_pass method */

