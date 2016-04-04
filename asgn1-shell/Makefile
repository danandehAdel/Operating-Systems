myshell: myshell.c lex.yy.c
	gcc -g -ansi -o myshell myshell.c lex.yy.c -lfl

lex.yy.c : lex.l
	/usr/bin/flex lex.l

clean: 
	rm myshell lex.yy.c
