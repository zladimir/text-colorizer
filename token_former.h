#define N 2			// reading from a file occurs in blocks of N-1 character

/*  the main variable of the program
		block - variable in which the read block is stored from the file
		num_symb_in_block - character number in read block
		word - a variable that is assembled from blocks. it is checked for a token
		count - character number in read word
*/
typedef struct{			
	char *block;
	int num_symb_in_block;
	char *word;
	int count;
} reader;

//extern FILE *finput;						// the name of the input file

void check(char *, FILE *);								// check for memory allocation
char *check_block(reader, FILE *);						//check for memory allocation

/* read block of N size from the input file */
char *read_block( FILE *);						

/* update block, if you must refer to the following characters */
reader block_updater(reader, FILE *);			

int special_flag_updater(int , int , char *);

/* adding a character to a word */
char *word_getter(char *, char , int);

/* taking the next character */
reader new_symbol(reader , int *, FILE *);

/*  the formation of the end of the word */
char *end_of_word(char *, int);

/*  update word and block
	increase character number in read word */
reader next_symbol_in_word_type1(reader , int, FILE *);

/*  update word and block
	increase character number in read word and block
	type_1 and type_2 different in relation to the block */
reader next_symbol_in_word_type2(reader , int, FILE *);

/* composing token - identifier in variable "word" */
reader identifiers(reader, int *, FILE *);

/*  choose between identifier and keywords
	print token */
reader chooser_of_ident_keyw(reader, token*);

/*  composing token - string literals
	if the closing symbol is not met, then this word goes
	to a new cycle and is painted in accordance with the token classes,
	the closing symbol remains without color
	suffix printed like a identifier */
reader reaction_double_commas(reader, int *, int *, token *, FILE *);

/*  case with a word beginning with prefix,: 
	U, L, u or u8 */
reader string_literal_with_prefix(reader, int *s, int *, token*, FILE *);

/*  check on belonging to punctuators
	lots of punctuators for my variant devided on 2 groups.
	the word is composed of 2 characters and 
	is checked for the identity of the token - punctuator.
	if this is true - checking to >>= or <<=
	otherwise check first symbol to single token , second symbol go to new cycle */
reader punctuation(reader, int *, token *, FILE *);

/* print a one line comment until the end of the line is encountered
	or, if it's the last line, // print in NC, next symbols go to new cycle and coloring 
	according to type of token  */
reader one_line_comment(reader, int *, int *, token*, FILE *);

/*  print a multiline comment until the end of the comment is encountered
	or the end of file */
reader multiline_comment(reader, int *, token*, FILE *);

/*  check on belonging to octal and hexadecimal digit
	and formation suffix of this digit */
reader octal_and_hexadec_digit(reader, int *, token *, FILE *);

/*  check on belonging to decimal digit
	and formation suffix of this digit */
reader decimal_digit(reader, int *, token *, FILE *);

/* the total sample for the 8, 10 and 16-bit integer const */
reader digit_hendler(reader, int *, token *, FILE *);

/*  formation of the end
	suffix analysis
	print */
reader number_end_and_print(reader, char *, int, int, int, token *);