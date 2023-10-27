/*
*  This project is an assembler for assembley language.
*  The program gets a file which contains a program in assembley language, checks its correctness and translates the program
*  to a binary code machine which will be eventually printed in an output file.
*
*  Authors: Sharbel Maroun , Eddi Vilinets
*  Date: 16.08.2023
*/

#include "pre_data.h"


int main(int argc, char *argv[])
{
	int result = 1; /* result of the value that the functios return */
	int i; /* index counter */
	int len; /* the argv[i] length */
	FILE *fp; /* pointer of the file */
	char ending [4] = ".as\0"; /* to add to the received file from the command line the ending .as */
	char *file_name; /* to store the file name */
    
	if (argc<2) /* no received files */
	{
		printf("There are no files to open\n");
	}
    
	for(i=1; i<argc; i++) /* passing over all the files that received from the command line */
	{
	    printf("The file %s Errors and Warnings(if exist):\n",argv[i]);
		len = strlen(argv[i]) + 5; /* adding 5 for the endings */
		file_name = (char*) malloc (sizeof(char) * len); /* allocating memory for the file name */

		if (file_name == NULL)
		{
			printf("can't allocate memory for file name");
			return 0;
		}

		/* placing the file name with the ending */
		clear_string(file_name,len);
		strcpy(file_name, argv[i]); 
		strcat(file_name, ending);
		fp = fopen(file_name,"r");

		if (fp == NULL)
		{
			if(file_name != NULL)
			free(file_name);
			printf("the file %s, can not be opened\n", argv[i]);
			continue;
		}

		AM_ENDING /* changing to .am ending */
        
		result = unfold_mcro(fp,file_name); /* making expanded file by unfoolding the mcros */
		AS_ENDING /* changing to .as ending */
		if(fp != NULL) /* closing the file with .as ending */
		fclose(fp);
            
		if (result == ERROR) /* error in mcros */
		{

			if(file_name != NULL)
			{
				free(file_name);
			}

		continue;
		}
        
		AM_ENDING /* changing to .am ending */
		fp =  fopen(file_name,"r"); /* open for reading */

		if (fp == NULL)
		{
			if(file_name != NULL)
			free(file_name);
			printf("the file %s, can not be opened\n", file_name);
			continue;
		}
        
		rewind(fp);
		first_pass(fp,file_name); /* second pass called from first pass */
    
		if (file_name != NULL) /* free the allocated memory of the file name */
		{
			free(file_name);
		}

		if (fp != NULL)
		{
			fclose(fp);
		}
	} /* end of loop */
    
	return 0;
} /* end of main */

