cat :   cat.o 
	gcc -o cat cat.c
cat.o: cat.c
	gcc -c -std=c99 -Wall cat.c
clean:
	rm -f cat.o cat
