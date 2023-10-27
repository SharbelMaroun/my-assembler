#ifndef PRE_DATA_H
#define PRE_DATA_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

/* macros */

#define ERROR -1
#define MAX_LINE 81
#define NEW_WORD (get == '\t' || get == ' ' || get == '\r') /* word separating */
#define NEW_LINE (get == '\n')
#define ENLARGE_MCRO_LINES 3

/* macros to change the file ending */
#define AS_ENDING file_name[len - 4] = 'a'; \
        file_name[len - 3] = 's'; \
        file_name[len - 2] = '\0';

#define AM_ENDING file_name[len - 4] = 'a'; \
        file_name[len - 3] = 'm'; \
        file_name[len - 2] = '\0';

/* struct to store the mcro properties */
struct mcrobj{ 
    char mcro_name [MAX_LINE]; /* mcro name */
    struct mcrobj *next; /* points to the next obj */
    char **lines; /* mcro lines */
    int lines_number; /* the number of lines of the mcro */
};

/*-----functions of macro_unfold-----*/

/* skips whitespaces - also used in data.h */
void skip_white (FILE *);

/* clears the string - also used in data.h */
void clear_string(char *,int);

/* returns a word */
int get_word (FILE *, char *, int);

/* checks if the macro has been declared */
struct mcrobj * is_dec_mcro(char *, struct mcrobj *);

/* skips whitespaces */
int is_new_mcro (char *word);

/* add a macro to the macro list */
int add_mcro(FILE *, char*, char*, struct mcrobj**, struct mcrobj**);

/* unfolds the line of the declaration number of the macro */
void unfold_dec_mcro(FILE *f, struct mcrobj *);

/* checks if the word is a valid directive word - also used in data.h */
int is_directive(char*);

/* checks if the word is a valid instruction word  - also used in data.h */
int is_instruction(char*);

/* macro_unfold */
int unfold_mcro(FILE *, char *);


/*-----functions of first_pass-----*/

/* first pass */
int first_pass(FILE *, char*);

#endif
