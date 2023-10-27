/*
*  This program responsable for the final output of the project.
*  The program contains two functions which creates files and prints in it data - base 64 and extern and entry labels (if exist).
*/

#include "data.h"

/* list of constant numbers of base 64 */
const char encoding_table[] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                '4', '5', '6', '7', '8', '9', '+', '/'
                              };


/*
*   Creates a file and prints the values of instruction and data tables
*   
*   The function assumes the buffer data and instruction data are valid tables,
*   creates a file and prints the values of the tables.
*   
*   @param filname  - The name of the file which will be created
*   @param buffer_data  - The data table
*   @param buffer_inst  - The instruction table
*   @param IC  - The instruction counter
*   @param DC  - The data counter
*/
void print_base_64 (char *filename, unsigned int buffer_data[BUFFER_MAX_SIZE][1], unsigned int buffer_inst[BUFFER_MAX_SIZE][1], int IC, int DC)
{
	unsigned int digit1 = 63; /* in binary the 12 bits (000000111111). so this can deduce the 6 bits on the right*/
	unsigned int digit2 = 4032; /* in binary the 12 bits (111111000000). so this can deduce the 6 bits on the left*/
	int code; /* storing the number in the current memory image index */
	int i = BUFFER_START_INDEX; /* counter */
	int j = 0; /* counter 2 */
	
	FILE *fp;
	int len = strlen(filename)+1; /* calculating the length of the file name with the ending .am */

	if (IC+DC == 0) /* memory image is empty */
	{
		printf("Warning: the memory image is empty, there are no instructions or directives in the file ");
        PRINT_FILE_NAME
		printf("\n");
		return;
	}

	OB_ENDING /* changing .ob ending */
	fp = fopen (filename, "w");

	if (fp == NULL)
	{
		printf ("Can't open a file for 64 base output\n");
		return;
	}

	fprintf (fp, "          \n");	/* 9 spaces for the object file title */
	j = IC; /* j will count the number of the instructions that have to be printed */

	while (j > 0) /* printing in the object file the instructions before */ /* loop2 */
	{
		/* marked 3 as empty(there is no valid instruction with the binary code 11 so we can use it as empty) */
		if (buffer_inst[i][0] != 3)
		{
			code = buffer_inst[i][0]; /* current memory image of the instruction */

			/* getting the 6 bits on the left and moving it 6 bits to the right and this will be the index in the encoding_table*/
			fprintf (fp, "%c", encoding_table[(code & digit2) >> BASE_64_MOVE]); 
			fprintf (fp, "%c\n", encoding_table[code & digit1]); /* getting the 6 bits on the right, and this will be the index in the encoding_table */
			j--; /*  */
		}

		i++;
	} /* end of loop2 */

	i = BUFFER_START_INDEX;
	j = DC; /* j will count the number of the directives that have to be printed */

	while (j > 0) /* 3 marks empty in instruction buffer and there is a number in the data buffer */ /* loop3 */
	{ 
		if (buffer_inst[i][0] == 3)
		{
			code = buffer_data[i][0]; /* current memory image of the directive */
			fprintf (fp, "%c", encoding_table[(code & digit2) >> BASE_64_MOVE]); /* printing in base 64 by using the index in the encoding_table */
			fprintf (fp, "%c\n", encoding_table[code & digit1]);
			j--;
		}

		i++;
	} /* end of loop3 */

	/* prints the title */
	rewind (fp);
	fprintf (fp, "%d ", IC);
	fprintf (fp, "%d", DC);
	fclose (fp);
} /* end of print_base_64 function */


/*
*   Creates files for labels of .entry and .extern
*
*   The function checks if a label of .entry or .extern exist
*   For labels which exist it creates a file for labels of .entry
*   and a file for labels of .extern .
*   In entry file - prints the labels of entry and the line of their declaration
*   In extern file - prints the labels of extern and the line of their .extern declaration.
*
*   @param label_node  - The list of labels from the input
*   @param filename   - The name of file which will be created
*/
void label_to_files (struct label_node **head, char *filename)
{
	int len; /* lenght of the file name */
	int i = 0;
	struct label_node *current = *head; /* label list current node */
	FILE *x_ptr = NULL; /* pointer to the extern labels */
	FILE *n_ptr = NULL; /* pointer to the entry label */

	len = strlen (filename) + 1;
	EXT_ENDING  /* changing file name to .ext ending */

	while (current != NULL && i != ERROR) /* till the last label node, and if the file can not be opened */ /* loop1 */
	{
		if ((current->dec_num) > EXTERN_START_ADDRESS) /* extern label */
		{
			if (i == 0) /* first extern label will open the file and will be added to it */
			{
				x_ptr = fopen (filename, "w");
				i++;

				if (x_ptr == NULL)
				{
					printf ("Can't open a file for extern labels\n");
					i = ERROR;
				}

				else
				{
					fprintf (x_ptr, "%s   ", current->label); /* label name */
					fprintf (x_ptr, "%d\n", current->dec_num - EXTERN_START_ADDRESS); /* label address */
				}
			}

			else /* not the first extern label */
			{
				fprintf (x_ptr, "%s   ", current->label); /* label name */
				fprintf (x_ptr, "%d\n", current->dec_num - EXTERN_START_ADDRESS); /* label address */
			}
		}

		current = current->next;
	} /* end of loop1 */

	if (x_ptr != NULL)
	{
		fclose (x_ptr);
	}

	ENT_ENDING /* changing file name to .ent ending */
	i = 0;
	current = *head; /* back to the head of the label list */

	while (current != NULL && i != ERROR) /* loop2 */
	{
		if ((current->dec_num) > ENTRY_START_ADDRESS && (current->dec_num) < EXTERN_START_ADDRESS) /* entry addresses between 2000-4000 */
		{
			if (i == 0) /* first entry label will open the entries file and will be added to it */
			{
				n_ptr = fopen (filename, "w");
				i++;

				if (n_ptr == NULL)
				{
					printf ("Can't open a file for extern labels\n");
					i = ERROR;
				}

				else
				{
					fprintf (n_ptr, "%s   ", current->label); /* label name */
					fprintf (n_ptr, "%d\n", current->dec_num - ENTRY_START_ADDRESS); /* label address */
				}
			}

	  		else /* not the first entry label */
	    		{
	      			fprintf (n_ptr, "%s   ", current->label); /* label name */
	      			fprintf (n_ptr, "%d\n", current->dec_num - ENTRY_START_ADDRESS); /* label address */
	    		}
		}

		current = current->next;
	} /* end of loop2 */

	if (n_ptr != NULL)
	{
        	fclose (n_ptr);
	}

} /* end of label_to_files function */

