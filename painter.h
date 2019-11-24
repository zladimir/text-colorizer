#define RED "\033[0;31m"
#define BLUE "\033[1;36m"
#define ORANGE "\033[0;33m"
#define PINK "\033[1;35m"
#define GREEN "\033[0;32m"
#define GRAY "\033[1;30m"
#define NC "\033[0m"

typedef struct{			// structure with information about token
    char *word;			
    int type;
    int new_line;		// variable responsible for newline
} token;

char *chooser_parenthesis(int);					/* selecting the framing brackets for the token */
char *color_conventer(int);						/* match the token type to its color */
void token_printer_2(int, char *, int *);		/* coloring of the word - token
													this function with framing brackets */
void token_printer_1(int, char *, int *);		/* coloring of the word - token */
