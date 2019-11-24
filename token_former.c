#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "verification_functions.h"
#include "painter.h"
#include "token_former.h"

void check(char *buff, FILE *finput){							// check for memory allocation
	if (buff == NULL){
		fclose(finput);
		perror(NULL);
		exit(0);
	}
}

char *check_block(reader buffer, FILE *finput){				//check for memory allocation
	if (buffer.block == NULL ){
		fprintf(stderr, "Error: select memory\n");
		free(buffer.block);
		if (buffer.word != NULL){
			free(buffer.word);
		}
		fclose(finput);
		exit(0);
	}
	return 0;
}

/* read block of N size from the input file */
char *read_block( FILE *finput){			
	char *buffer = malloc(N * sizeof(char));
	check(buffer, finput);
	buffer[0] = '\0';
	char *cur = buffer;
	char *tmp = fgets(cur, N, finput);
	if (tmp == NULL){
		return buffer;
	}
	buffer = tmp;
	return buffer;
}

/* update block, if you must refer to the following characters */
reader block_updater(reader buffer, FILE *finput){
	if (buffer.num_symb_in_block >= strlen(buffer.block) ){
		free(buffer.block);
		buffer.num_symb_in_block = 0;
		buffer.block = read_block(finput);
		check_block(buffer, finput);
	}
	return buffer;
}

int special_flag_updater(int special_flag, int num, char *buf){
	if (special_flag == 1){
		if (num >= strlen(buf) ){
			special_flag = 0;			
		}
	}	
	return special_flag;
}

/* adding a character to a word */
char *word_getter(char *buff, char symb, int n){
	if (n != 0){
		buff = realloc(buff, (n + 1) * sizeof(char));
		//check(buff, finput);
	}
	buff[n] = symb;
	return buff;
}

/* taking the next character */
reader new_symbol(reader buffer, int *symb, FILE *finput){
	*symb = buffer.block[buffer.num_symb_in_block];
	buffer.num_symb_in_block++;
	return buffer;
}

/*  the formation of the end of the word */
char *end_of_word(char *word, int count){
	word = realloc(word, (count + 1) * sizeof(char));
	word[count] = '\0';
	return word;
}

/*  update word and block
	increase character number in read word */
reader next_symbol_in_word_type1(reader buffer, int symb, FILE *finput){
	buffer.word = word_getter(buffer.word, symb, buffer.count);
	buffer.count++;
	buffer = block_updater(buffer, finput);
	return buffer;
}

/*  update word and block
	increase character number in read word and block
	type_1 and type_2 different in relation to the block */
reader next_symbol_in_word_type2(reader buffer, int symb, FILE *finput){
	buffer.word = word_getter(buffer.word, symb, buffer.count);
	buffer.count++;
	buffer.num_symb_in_block++;
	buffer = block_updater(buffer, finput);
	return buffer;
}

/* composing token - identifier in variable "word" */
reader identifiers(reader buffer, int *symb, FILE *finput){
	*symb = buffer.block[buffer.num_symb_in_block];
	while ((isalnum(*symb) || ((*symb) == '_'))
			&& (*symb != '\0') && (ferror(finput) == 0)){
		buffer = next_symbol_in_word_type2(buffer, *symb, finput);
		*symb = buffer.block[buffer.num_symb_in_block];
	}
	return buffer;
}

/*  choose between identifier and keywords
	print token */
reader chooser_of_ident_keyw(reader buffer, token *tok){
	buffer.word = end_of_word(buffer.word, buffer.count);	
	if (keywords_checker(buffer.word)){
		tok->word = buffer.word;
		tok->type = 0;
	} else{
		tok->word = buffer.word;
		tok->type = 1;
	}
	return buffer;
}

/*  composing token - string literals
	if the closing symbol is not met, then this word goes
	to a new cycle and is painted in accordance with the token classes,
	the closing symbol remains without color
	suffix printed like a identifier */
reader reaction_double_commas(reader buffer, int *special_flag, int *symb, token *tok, FILE *finput){
	buffer = next_symbol_in_word_type1(buffer, *symb, finput);                             
	*symb = buffer.block[buffer.num_symb_in_block];
	int condition = 0;
	while ((condition < 2) && (*symb != '\n')
			&& (*symb != '\0') && (ferror(finput) == 0)){
		if (*symb == '\\'){
			if (condition == 1){
				condition = 0;
			} else{
				condition = 1;
			}
		} else{
			if (*symb == '"'){
				if (condition == 1){
					condition = 0;
				} else{
					condition = 2;
				}
			} else{
				if (condition == 1){
					condition = 0;
				}
			}
		}
		buffer = next_symbol_in_word_type2(buffer, *symb, finput);
		*symb = buffer.block[buffer.num_symb_in_block];
	}
	if (condition != 2){
		buffer.word = end_of_word(buffer.word, buffer.count);
		tok->word = buffer.word;
		tok->type = -1;
		buffer.num_symb_in_block--;
	} else{
		buffer.word = end_of_word(buffer.word, buffer.count);
		tok->word = buffer.word;
		tok->type = 3;
		buffer.num_symb_in_block--;
	}
	buffer.num_symb_in_block++;
	return buffer;
}

/*  case with a word beginning with prefix,: 
	U, L, u or u8 */
reader string_literal_with_prefix(reader buffer, int *special_flag, int *symb, token *tok, FILE *finput){
	buffer = new_symbol(buffer, symb, finput);
	switch(*symb){
		case '"':
			buffer = reaction_double_commas(buffer, special_flag, symb, tok, finput);
			break;
		default:
			buffer.num_symb_in_block--;
			buffer = identifiers(buffer, symb, finput);
			buffer.word = end_of_word(buffer.word, buffer.count);	
			tok->word = buffer.word;
			tok->type = 1;
			break;
	}
	return buffer;
}

/*  check on belonging to punctuators
	lots of punctuators for my variant devided on 2 groups.
	the word is composed of 2 characters and 
	is checked for the identity of the token - punctuator.
	if this is true - checking to >>= or <<=
	otherwise check first symbol to single token , second symbol go to new cycle */
reader punctuation(reader buffer, int *symb, token *tok, FILE *finput){
	if (separators_checker(*symb) && (buffer.count == 0)){
		if (*symb == '\n'){
			tok->new_line = 1;
		}
		buffer = next_symbol_in_word_type1(buffer, *symb, finput);
		buffer.word = end_of_word(buffer.word, buffer.count);
		tok->word = buffer.word;
		tok->type = -1;
	} else{
		while ((*symb != '\0') && (ferror(finput) == 0) && (buffer.count < 2)){
			if (buffer.count < 1){
				buffer = next_symbol_in_word_type1(buffer, *symb, finput);
				buffer = new_symbol(buffer, symb, finput);
			} else{
				buffer.word = word_getter(buffer.word, *symb, buffer.count);
				buffer.count++;
			}
		}
		if (buffer.count == 2){
			buffer.word = end_of_word(buffer.word, buffer.count);		
			if (double_punctuators_checkers (buffer.word)){
				if (!(strcmp(buffer.word, "<<")) || !(strcmp(buffer.word, ">>"))){
					buffer = block_updater(buffer, finput);
					buffer = new_symbol(buffer, symb, finput);
					if (*symb == '='){
						buffer.word = word_getter(buffer.word, *symb, buffer.count);
						buffer.count++;
						buffer.word = end_of_word(buffer.word, buffer.count);
						tok->word = buffer.word;
						tok->type = 4;	
					} else{
						buffer.num_symb_in_block--;	
						tok->word = buffer.word;
						tok->type = 4;
					}
				} else{	
					tok->word = buffer.word;
					tok->type = 4;
				}
			} else{
				buffer.num_symb_in_block--;
				buffer.word[1] = '\0';
				tok->word = buffer.word;
				tok->word = end_of_word(tok->word, 1);
				if (single_punctuation_checker(tok->word[0])){
					tok->type = 4;	
				} else{
					tok->type = -1;
				}
			}
		} else{
			buffer.word = end_of_word(buffer.word, buffer.count);
			tok->word = buffer.word;
			if (single_punctuation_checker(tok->word[0])){
				tok->type = 4;
			} else{
				tok->type = -1;
			}
		}
	}
	return buffer;
}

/* print a one line comment until the end of the line is encountered
	or, if it's the last line, // print in NC, next symbols go to new cycle and coloring 
	according to type of token  */
reader one_line_comment(reader buffer, int *special_flag, int *symb, token *tok, FILE *finput){
	buffer = next_symbol_in_word_type1(buffer, *symb, finput);
	while ( ((*symb = buffer.block[buffer.num_symb_in_block]) != '\0') && (ferror(finput) == 0)
			&& ((*symb = buffer.block[buffer.num_symb_in_block]) != '\n')){
		buffer = next_symbol_in_word_type2(buffer, *symb, finput);
	}
	buffer.word = word_getter(buffer.word, *symb, buffer.count);
	buffer.word = end_of_word(buffer.word, buffer.count + 1);
	if (*symb == '\n'){
		tok->word = buffer.word;
		tok->type = 5;
		tok->new_line = 1;
	} else{
		free(buffer.block);
		buffer.block = malloc(strlen(buffer.word) + 1 );
		strcpy(buffer.block, buffer.word);
		buffer.num_symb_in_block = 1;
		*special_flag = 1;
		free(buffer.word);
		buffer.word = malloc(3* sizeof(char));
		buffer.word[0] = '/';
		buffer.word[1] = '/';
		buffer.word[2] = '\0';
		tok->word = buffer.word;
		tok->type = -1;
	}
	buffer.num_symb_in_block++;
	return buffer;
}

/*  print a multiline comment until the end of the comment is encountered
	or the end of file */
reader multiline_comment(reader buffer, int *symb, token *tok, FILE *finput){
	buffer = next_symbol_in_word_type1(buffer, *symb, finput);
	int condition = 0;
	while ((condition != 2) && (*symb != '\0') && (ferror(finput) == 0)){
		*symb = buffer.block[buffer.num_symb_in_block];
		buffer.word = word_getter(buffer.word, *symb, buffer.count);
		buffer.count++;
		if (*symb == '*'){
			condition = 1;
		} else{
			if (condition == 1){
				if (*symb == '/'){
					condition = 2;
				} else {
					condition = 0;
				}
			}								 
		}
		buffer.num_symb_in_block++;
		buffer = block_updater(buffer, finput);
	}
	buffer.word = end_of_word(buffer.word, buffer.count);
	tok->word = buffer.word;
	if (condition != 2){
		tok->type = -1;
	} else{
		tok->type = 5;
	}
	return buffer;
}

/*  formation of the end
	suffix analysis
	print */
reader number_end_and_print(reader buffer, char *suffix, int suf_count,
		int start_read_suffix, int no_suffix_flag, token *tok){
	if (no_suffix_flag != 3){
		buffer.word = end_of_word(buffer.word, buffer.count);	
		buffer.word = realloc(buffer.word, strlen(buffer.word) + strlen(suffix) + 1);
		strcat(buffer.word, suffix);
		buffer.count += strlen(suffix);
		buffer.word = end_of_word(buffer.word, buffer.count);	
		tok->word = buffer.word;
		tok->type = 2;
	} else{
		buffer.word = end_of_word(buffer.word, buffer.count);
		tok->word = buffer.word;
		tok->type = 2;			
	}
	return buffer;
}

reader octal_and_hexadec_digit(reader buffer, int *symb, token *tok, FILE *finput){
	char *suffix = malloc(sizeof(char));		// buffer for suffix of digit
	check(suffix, finput);
	int start_read_suffix = 0;		// if that flag == 1, then the suffix begins to form
	int suf_count = 0;				// character number in suffix
	int no_suffix_flag = 0;			// if that flag == 2, than this suffix is ok
	buffer = block_updater(buffer, finput);
	*symb = buffer.block[buffer.num_symb_in_block];
	/* case of hexadecimal-constant */
	if ((*symb == 'x') || (*symb == 'X')){
		buffer = next_symbol_in_word_type2(buffer, *symb, finput);
		*symb = buffer.block[buffer.num_symb_in_block];
		if ((isdigit(*symb)) || (letters_of_16_bit_number(*symb))){
			while (((isalnum(*symb)) || (*symb == '_')) && (no_suffix_flag < 2)
					&& (*symb != '\0') && (ferror(finput) == 0)){
				buffer = block_updater(buffer, finput);
				buffer.num_symb_in_block++;
				if ((isalpha(*symb)) || (*symb == '_')){
					if (!letters_of_16_bit_number(*symb)){
						start_read_suffix = 1;
					}
				}
				if (start_read_suffix){
					suffix = word_getter(suffix, *symb, suf_count);
					suf_count++;
					suffix = word_getter(suffix, '\0', suf_count);
					if (!suffix_checker(suffix)){
						if (no_suffix_flag == 1){
							suffix[suf_count - 1] = '\0';
							buffer.num_symb_in_block--;
							no_suffix_flag = 2;
						} else{
							no_suffix_flag = 3;
							buffer.num_symb_in_block--;
						}
					} else{
						no_suffix_flag = 1;
					}
				} else{
					buffer.word = word_getter(buffer.word, *symb, buffer.count);
					buffer.count++;
				}
				if (no_suffix_flag < 2){
					buffer = block_updater(buffer, finput);
					*symb = buffer.block[buffer.num_symb_in_block];
				}
			}
			if (no_suffix_flag != 2){
				suffix = end_of_word(suffix, suf_count);
			}
			buffer = number_end_and_print(buffer, suffix, suf_count, start_read_suffix, no_suffix_flag, tok);
		} else{
			free(buffer.word);
			buffer.word = malloc(2 * sizeof(char));
			buffer.word[0] = '0';
			buffer.word[1] = '\0';
			tok->word = buffer.word;
			tok->type = 2;
			fseek(finput, -2 * sizeof(char), SEEK_CUR);
			buffer.num_symb_in_block = N;
		}

	/* case of octal-constant */	
	} else{
	while (((isalnum(*symb)) || (*symb == '_')) && (no_suffix_flag < 2)
			&& (*symb != '8') && (*symb != '9')
			&& (*symb != '\0') && (ferror(finput) == 0)){
		buffer = block_updater(buffer, finput);
		buffer.num_symb_in_block++;
			if ((isalpha(*symb))  || (*symb == '_')){
				start_read_suffix = 1;
			}
			if (start_read_suffix){
				suffix = word_getter(suffix, *symb, suf_count);
				suf_count++;
				suffix = word_getter(suffix, '\0', suf_count);
				if (!suffix_checker(suffix)){
					if (no_suffix_flag == 1){
						suffix[suf_count - 1] = '\0';
						buffer.num_symb_in_block--;
						no_suffix_flag = 2;
					} else{
						no_suffix_flag = 3;
						buffer.num_symb_in_block--;
					}
				} else{
					no_suffix_flag = 1;
				}
			} else{
				buffer.word = word_getter(buffer.word, *symb, buffer.count);
				buffer.count++;
			}
			if (no_suffix_flag < 2){
				buffer = block_updater(buffer, finput);
				*symb = buffer.block[buffer.num_symb_in_block];
			}
		}
		if (no_suffix_flag != 2){
			suffix = end_of_word(suffix, suf_count);
		}
		buffer = number_end_and_print(buffer, suffix, suf_count, start_read_suffix, no_suffix_flag, tok);
	}
	free(suffix);
	return buffer;
}

/*  check on belonging to decimal digit
	and formation suffix of this digit */
reader decimal_digit(reader buffer, int *symb, token *tok, FILE *finput){
	char *suffix = malloc(sizeof(char));		// buffer for suffix of digit
	check(suffix, finput);
	int start_read_suffix = 0;		// if that flag == 1, then the suffix begins to form
	int suf_count = 0;				// character number in suffix
	int no_suffix_flag = 0;			// if that flag == 2, than suffix is ok
	buffer = block_updater(buffer, finput);
	*symb = buffer.block[buffer.num_symb_in_block];
	while (((isalnum(*symb)) || (*symb == '_')) && (no_suffix_flag < 2)
			&& (*symb != '\0') && (ferror(finput) == 0)){
		buffer = block_updater(buffer, finput);
		buffer.num_symb_in_block++;
		if ((isalpha(*symb)) || (*symb == '_')) {
			start_read_suffix = 1;
		}
		if (start_read_suffix){
			suffix = word_getter(suffix, *symb, suf_count);
			suf_count++;
			suffix = word_getter(suffix, '\0', suf_count);
			if (!suffix_checker(suffix)){
				if (no_suffix_flag == 1){
					suffix[suf_count - 1] = '\0';
					buffer.num_symb_in_block--;
					no_suffix_flag = 2;
				} else{
					no_suffix_flag = 3;
					buffer.num_symb_in_block--;
				}
			} else{
				no_suffix_flag = 1;
			}
		} else{
			buffer.word = word_getter(buffer.word, *symb, buffer.count);
			buffer.count++;
		}
		if (no_suffix_flag < 2){
			buffer = block_updater(buffer, finput);
			*symb = buffer.block[buffer.num_symb_in_block];
		}
	}
	if (no_suffix_flag != 2){
		suffix = end_of_word(suffix, suf_count);
	}
	buffer = number_end_and_print(buffer, suffix, suf_count, start_read_suffix, no_suffix_flag, tok);
	free(suffix);
	return buffer;
}

/* the total sample for the 8, 10 and 16-bit integer const */
reader digit_hendler(reader buffer, int *symb, token *tok, FILE *finput){
	buffer = next_symbol_in_word_type1(buffer, *symb, finput);
	if (*symb == '0'){
		buffer = octal_and_hexadec_digit(buffer, symb, tok, finput);
	} else{
		buffer = decimal_digit(buffer, symb, tok, finput);
	}
	return buffer; 
}