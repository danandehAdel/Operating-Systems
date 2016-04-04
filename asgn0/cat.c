#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
     int counter;
     int fileOpen;
     char buffer[101];
     
     // Check to see if there is any input
     if(argc < 2){
	perror("Not enough arguments!");
        exit(1);
     }
     // There exists input, therefore loop through them
     else{

	int i; 
	for(i = 1; i < argc; i++){
	    fileOpen = open(argv[i], O_RDONLY);
	    if(fileOpen < 0){
   		perror("Not able to open the file!");
            }
	    // read input file
	    while ((counter = read(fileOpen, buffer, 100)) > 0){
		write(1, buffer, counter);// write the read file to stdout
	    }
            close(fileOpen);
        }
     }
     return 0;
   }

