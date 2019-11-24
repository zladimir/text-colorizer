#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "painter.h"

int control[6] = {0, 0, 0, 0, 0, 0};

/* selecting the framing brackets for the token */
char *chooser_parenthesis(int color){
	char *parenthesis = malloc(3 * sizeof(char));
	switch(color){
		case 0:
			parenthesis[0] = '[';
			parenthesis[1] = ']';
			break;
		case 1:
			parenthesis[0] = '(';
			parenthesis[1] = ')';
			break;
		case 2:
			parenthesis[0] = '#';
			parenthesis[1] = '#';
			break;
		case 3:
			parenthesis[0] = '{';
			parenthesis[1] = '}';
			break;
		case 4:
			parenthesis[0] = '<';
			parenthesis[1] = '>';
			break;
		case 5:
			parenthesis[0] = '|';
			parenthesis[1] = '|';
			break;
		default:
			parenthesis[0] = '\0';
			return parenthesis;
	}
	parenthesis[2] = '\0';
	return parenthesis;
}

/* match the token type to its color */
char *color_conventer(int color){
	switch(color){
		case 0:
			return RED;
		case 1:
			return BLUE;
		case 2:
			return ORANGE;
		case 3:
			return PINK;
		case 4:
			return GREEN;
		case 5:
			return GRAY;
		default:
			return NC;
	}
}

/* coloring of the word - token
	this function with framing brackets */
void token_printer_2(int color, char *word, int *nl){
	char *clr = color_conventer(color);
	printf("%s", clr);
	if (*nl == 1){
		*nl = 0;
		for (int i = 0; i < 6; ++i){
			control[i] = 0;
		}
	}
	if (color != -1){
		control[color]++;
		if (control[color] == 1){
			char *parenthesis = chooser_parenthesis(color);
			putchar(parenthesis[0]);
			printf("%s", word);
			putchar(parenthesis[1]);
			free(parenthesis);
		} else{
			printf("%s", word);
		}
	} else{
		printf("%s", word);
	}
	printf("%s", NC);
	return;
}

/* coloring of the word - token */
void token_printer_1(int color, char *word, int *nl){
	char *clr = color_conventer(color);
	printf("%s", clr);
	printf("%s", word);
	printf("%s", NC);
	return;
}