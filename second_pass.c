/*
*  This program is the second pass on the input file after the first pass.
*  The program executed by the first pass after memory allocation and adress assignments and gets their values for
*  binary machine code creation.
*/

#include "data.h"

int second_pass (FILE *file_ptr, char *filename, unsigned int buffer_data[BUFFER_MAX_SIZE][1] , unsigned int buffer_inst[BUFFER_MAX_SIZE][1], struct label_node **head, int IC, int DC)
{
	char get; /* to get a single char */
	char line [MAX_LINE+2]; /* to store the whole line */
	char *word; /* to tokenize the line */
	int errInd = 0; /* error indicator */
	int value1; /* funtions returning values */
	int value2;
	int lineNum = 1; /* lines counter */
	int dec_num = BUFFER_START_INDEX; /* memory image counter */
	int inst_code; /* to store instruction representation */
    
	clear_string2(line,MAX_LINE+2);

	while(1) /* loop1 */
	{
		clear_string2(line,MAX_LINE+2);
		get = fgetc(file_ptr); /* cheking first character in the line */

		if (get == '\n' || get == ';') /* new line or comment line */
		{
			if(get == ';') /* if this is a comment line */
			{
		        	fgets(line, MAX_LINE+2,file_ptr); /* reading the rest of the line */
			}

		    	lineNum++;
		    	continue;
		}

		if(get == EOF) /* end of file */
		{
		    break;
		}

		fseek(file_ptr,-1,SEEK_CUR); /* moving backward on character */
        	fgets(line, MAX_LINE+2,file_ptr); /* reading the whole line */
        
        	if(is_empty_line(line)) /* if empty line */
        	{
            		lineNum++;
		    	continue;    
        	}
    
		word = strtok(line," \r\n\t,");  /* tokenize first word */

		if(type(word) == 'l') /* if this is a label we will tokenize the next word */
		{
		    word = strtok(NULL," \r\n\t,"); 
		}
		
		if((value1 = is_instruction(word)) != ERROR) /* instruction case */
	    	{
	        	inst_code = buffer_inst[dec_num][0]; /* the instruction 12 bits */
	        	dec_num++;
            		word = strtok(NULL," \r\n\t,"); /* tokenize the first operand*/

            		if(value1 < 14) /* instructions with operands(1 or 2) */
            		{
                		if(is_label(word, 'o') == 1) /* if the first operand is a label */
                		{
                    			if((value2 = is_dec_label(head, word)) == ERROR) /* getting its address */
                    			{
                        			printf("Error in line %d, the label in first operand has not defined\n", lineNum);
                        			errInd = ERROR;
                    			}

                    			else
                    			{  
                       				if(value2 >= EXTERN_START_ADDRESS) /* if this is extern label */
                        			{
                            				update_extern(head, word, dec_num, lineNum); /* adding to label list if haven't inserted yet */
                        			}

                        			if(value2 == ENTRY_START_ADDRESS) /* when the address is 2000 so the label has declared as entry but not defined */
                        			{
                            				printf("Error in line %d, the label in first operand has declared as entry but not defined\n", lineNum);
                            				errInd = ERROR;
                        			}

                        			/* checking if the address has not inserted yet */
                        			if(buffer_inst[dec_num][0] == 1 || buffer_inst[dec_num][0] == 2)
                        			{
                            				LABEL_CODING(value2, dec_num) /* inserting the address to memory image */
                        			}
                    			}
                		}

                		dec_num ++;

                		switch(value1) /* instructions with two operands */ /* switch1 */
                		{ 
                			case(0): case(1): case(2): case(3): case(6):
                    			word = strtok(NULL," \r\n\t,"); /* tokenize the second operand */

                    			if(*word == ',') /* we dont need the comma for operand test */
                    			{
                        			*word = '\0';
                        			word ++;
                    			}
                    
                    			if(is_label(word, 'o') == 1) /* second operand is a label */
                    			{
                       				if((value2 = is_dec_label(head, word)) == ERROR) /* if haven't defined */
                        			{
                            				printf("Error in line %d, the label in first operand has not defined\n", lineNum);
                            				errInd = ERROR;
                        			}

                        			else
                        			{  
                            				if(value2 >= EXTERN_START_ADDRESS) /* extern label */
                            				{
                                				update_extern(head, word, dec_num, lineNum); /* inserting extern label if declared as extern but not exists */
                            				}

                            				if(value2 == ENTRY_START_ADDRESS) /* entry label */
                            				{
                                				printf("Error in line %d, the label in first operand has declared as entry but not defined\n", lineNum);
                                				errInd = ERROR;
                            				}

                            				/* checking if the address has not inserted yet */
                            				if(buffer_inst[dec_num][0] == 1 || buffer_inst[dec_num][0] == 2)
                            				{
                                				LABEL_CODING(value2, dec_num) /* inserting the address to memory image */
                            				}
                        			}
                    			}

                    			/* checking if two operands are rigesters */
                    			if(((20 & inst_code) != 20) || ((2560 & inst_code) != 2560)) /* 20 in binary = 101 in target operand bits, and 2560 in binary = 101 in source operand bits */
                    			{
                        			dec_num++;
                    			}

                			break;
                		} /* end of switch1 */
            		}

	    	}

	    	else if(strcmp(word,".entry") == 0) /* entry directive */
	    	{
	        	word = strtok(NULL," \r\n\t,");
	        
	        	if(is_dec_label(head,word) == ENTRY_START_ADDRESS) /* decalred as entry but not defined */
	        	{
	            		printf("Error in line %d, label '%s' has not defined for entry\n", lineNum, word);
	            		errInd = ERROR;
	        	}
	    	}

	    	else if(strcmp(word,".extern") == 0) /* extern directive */
	    	{
	        	word = strtok(NULL," \r\n\t,");
	        	if(is_dec_label(head,word) == EXTERN_START_ADDRESS) /* decalred as entry but not used */
	        	{
	            		printf("Warning: in line %d, extern label '%s', has decalred as extern but not used\n", lineNum, word);
	        	}
	    	}

	    	/* skip the directives indexes */
	    	while(buffer_inst[dec_num][0] == 3 && dec_num < (IC+DC+BUFFER_START_INDEX)) /* loop2 */
	    		{
	        	dec_num ++;
	    	} /* end of loop2 */

	    	lineNum ++;
	} /* end of loop1 */

	/* if there is no error the outputs functions will be called */
    	if(errInd != ERROR)
    	{
        	print_base_64(filename, buffer_data, buffer_inst, IC, DC);
        	label_to_files(head, filename);
    	}

	filename = strtok(filename,".");
    	strcat(filename,".am"); /* preparing the file with the .am ending for closing in the main */
    	return errInd; /* returning error indicator */
} /* end of second_pass */
