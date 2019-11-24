Program2: main.o verification_functions.o token_former.o painter.o
	gcc main.o verification_functions.o token_former.o painter.o -o Program2 -Wall -fno-common
main.o: main.c
	gcc main.c -c -g -Wall -fno-common
verification_functions.o: verification_functions.c 
	gcc verification_functions.c -c -g -Wall -fno-common
token_former.o: token_former.c 
	gcc token_former.c -c -g -Wall -fno-common
painter.o: painter.c 
	gcc painter.c -c -g -Wall -fno-common
clean: 
	rm main.o verification_functions.o token_former.o painter.o Program2
