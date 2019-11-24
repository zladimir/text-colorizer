#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include  <ctype.h>
#include "verification_functions.h"
#include "painter.h"
#include "token_former.h"

//FILE *finput;

reader cycle(reader buf, token *tok, int answer, FILE *finput){
	void (*token_printer)(int, char *, int *);
	if (answer != 2){								// choice the function of painting
		token_printer = token_printer_1;			// usual mode
	} else{
		token_printer = token_printer_2;			// mode with framing brackets for the token
	}
	int symbol;
	int special_flag = 0;
	buf.block = read_block(finput);		// variable in which the read block is stored from the file
	if (buf.block == NULL ){
		fprintf(stderr, "Error: select memory\n");
		free(buf.block);
		fclose(finput);	
	} 
	buf.num_symb_in_block = 0;			// character number in read block
	
	while ((( feof(finput) == 0) && (ferror(finput) == 0)) || (special_flag)){
		buf.count = 0;					// character number in read word
		buf.word = malloc(sizeof(char));
		check(buf.word, finput);
		buf = block_updater(buf, finput);		// update block, if char number in read block > possible
		buf = new_symbol(buf, &symbol, finput);			// symdol = current character in block
		switch(symbol){
			case '"':
				buf = reaction_double_commas(buf, &special_flag, &symbol, tok, finput);		// proccesing string literals
				token_printer(tok->type, tok->word, &tok->new_line);
				break;
			case '/':
				buf = next_symbol_in_word_type1(buf, symbol, finput);
				buf = new_symbol(buf, &symbol, finput);
				switch(symbol){
					case '*':
						buf = multiline_comment(buf, &symbol, tok, finput);			// proccesing multi-line comment
						token_printer(tok->type, tok->word, &tok->new_line);
						break;
					case '/':
						buf = one_line_comment(buf, &special_flag, &symbol, tok, finput);			// proccesing one-line comment
						token_printer(tok->type, tok->word, &tok->new_line);
						break;
					default:
						buf = punctuation(buf, &symbol, tok, finput);
						token_printer(tok->type, tok->word, &tok->new_line);
						break;
				}
				break;
			default :
				if ((isalpha(symbol)) || (symbol == '_')){					
					buf = next_symbol_in_word_type1(buf, symbol, finput);
					switch(symbol){
						case 'U':
							buf = string_literal_with_prefix(buf, &special_flag, &symbol, tok, finput);		// check on belonging to
							token_printer(tok->type, tok->word, &tok->new_line);
							break;									// string literals
						case 'L':
							buf = string_literal_with_prefix(buf, &special_flag, &symbol, tok, finput);		// check on belonging to
							token_printer(tok->type, tok->word, &tok->new_line);
							break;									// string literals
						case 'u':
							buf = new_symbol(buf, &symbol, finput);
							switch(symbol){	
								case '"':
									buf.num_symb_in_block--;
									buf = string_literal_with_prefix(buf, &special_flag, &symbol, tok, finput);
									token_printer(tok->type, tok->word, &tok->new_line);
									break;					
								case '8':							// check to u8-prefix
									buf = next_symbol_in_word_type1(buf, symbol, finput);
									buf = string_literal_with_prefix(buf, &special_flag, &symbol, tok, finput);			
									token_printer(tok->type, tok->word, &tok->new_line);					
									break;
								default:
									buf.num_symb_in_block--;		// identifier/keyword
									buf = identifiers(buf, &symbol, finput);			// because start at u
									buf = chooser_of_ident_keyw(buf, tok);
									token_printer(tok->type, tok->word, &tok->new_line);
									break;
							}
							break;	
						default:
							buf = identifiers(buf, &symbol, finput);				// processing identifier/keyword
							buf = chooser_of_ident_keyw(buf, tok);	// because start at alpha
							token_printer(tok->type, tok->word, &tok->new_line);
							break;
					}
					break;
				} else{					
					if (isdigit(symbol)){										// proccesing integer const
						buf = digit_hendler(buf, &symbol, tok, finput);
					} else{
						buf = punctuation(buf, &symbol, tok, finput);									// proccesing punctuators
					}
					token_printer(tok->type, tok->word, &tok->new_line);
				}			
		}
		special_flag = special_flag_updater(special_flag, buf.num_symb_in_block, buf.block);
		free(tok->word);
	}	
	return buf;
}

int main (int argc, char **argv){
	if ( argc != 3){
		fprintf(stderr, "Error: wrong number of parameters\n");
		return 0;
	}

	FILE *finput;

	char *file_name_in = argv[1];
	if ((finput = fopen( file_name_in, "r")) == 0){
		fprintf(stderr, "Error: unable to open file input\n");
		return 0;
	}

	int answer = atoi(argv[2]);
	token tok;
	tok.type = -1;
	tok.word = '\0';
	tok.new_line = 1;
	reader buf = cycle(buf, &tok, answer, finput);	// start of the checking and coloring
	printf("\n");
	free(buf.block);

	if (feof(finput) != 1 ){
		fprintf(stderr, "Error: is a directory\n");
	}

	fclose(finput);
	return 0;	
}