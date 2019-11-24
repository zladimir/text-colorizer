#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "verification_functions.h"

const char *keywords[] = {				
		"auto", "break", "case", "char",
		"const", "continue", "default",
		"do", "double", "else", "enum",
		"extern", "float", "for", "goto"
	};

	const char *suf[] = {							// correct suffix at the integer constants
		"u", "U", "l", "L", "ll", "LL", "Ul", "UL",
		"ul", "uL", "LU", "Lu", "lU", "lu",
		"ULL", "Ull", "uLL", "ull", "LLU", "LLu", "llU", "llu"
	};

	const char *single_punctuation[] = {	// parenthesis, which can be repeated
		"[", "]", "(", ")", "{", "}", "/",				// one after another repeatedly
		"<", ">", "=", ".", "%",  "^", "|"

	};

	const char *separators[] = {					// word separators, other than punctuators
		" ", "\t", "\n", "\0"
	};

	const char *double_punctuators[] = {				// punctuators in my variant
		"<<", ">>", "<=", ">=", "==", "!=",
		"&&", "||", "->", "&=", "^=", "|=", 
		"*=", "/=", "%=", "+=", "-="
	};

int keywords_checker(char *token){
	for (int i = 0; i < KW; ++i)
	{
		if (!(strcmp(keywords[i],token))){
			return 1;
		}
	}
	return 0;
}

/* check on belonging to the lot of "other_punctuators" */
int double_punctuators_checkers(char *token){
	for (int i = 0; i < DP; ++i)
	{
		if (!(strcmp(double_punctuators[i],token))){
			return 1;
		}
	}
	return 0;
}

/* check on belonging to the lot of "suffix at the integer constants" */
int suffix_checker(char *token){
	for (int i = 0; i < SUF; ++i)
	{
		if (!(strcmp(suf[i],token))){
			return 1;
		}
	}
	return 0;
}

/* check on belonging to the lot of "separators" */
int separators_checker(int c){
	for (int i = 0; i < SEP; ++i){
		if (c == *separators[i]){
			return 1;
		}
	}
	return 0;
}

/* check on belonging to the lot of "punctuation_of_parenthesis" */
int single_punctuation_checker(int c){
	for (int i = 0; i < SP; i++){
		if (c == *single_punctuation[i]){
			return 1;
		}
	}
	return 0;
}

/* check on belonging to the lot of letters of 16 bit number */
int letters_of_16_bit_number(int c){
	if ((c == 'a') || (c == 'b') || (c == 'c') || (c == 'd') || (c == 'e') || (c == 'f') ||
		(c == 'A') || (c == 'B') || (c == 'C') || (c == 'D') || (c == 'E') || (c == 'F')){
		return 1;
	}
	return 0;
}