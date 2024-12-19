/*
 * words.c program
 * breaks input from stdin or files into a series of words, one per line
 *
 * Jemely Robles, January 2023, cs50 23W
 */

#include <stdio.h>
#include <strings.h>
#include <ctype.h>

/* function declarations */
int retrieval(FILE *fp);

/* ******** main ******** */
int
main(const int argc, char* argv[])
{

  FILE *fp;

  //read from stdin if no arguments given
  if(argc == 1) {
    retrieval(stdin);
  }  

  else{ //for arguments, go through them and read the files
    for(int i = 1; i < argc; i++){
      if (*argv[i] != '-'){ //if the argument isn't -
        if(fp != NULL){ //and the file has content
          fp = fopen(argv[i], "r"); //open file
          retrieval(fp);//read it
          fclose(fp);//close file
        }
        else{ //if the file is null, print and error to stderr
          fprintf(stderr, "File is unreadable");
          return 1;
        }
      }
      else{ //if the argument is -, start reading from stdin
          retrieval(stdin);
      }
    }
  }  
return 0;
}  


/* ********** retrieval ********** */
/*
 * reads files character by character, prints out word by word
 *
 */

int retrieval(FILE *fp){
  
  char c; //current character
  char back = '\n'; //back output

   //while the current character isn't the end of file, read character by character and print it
   while((c = fgetc(fp)) != EOF){
    if(!isalpha(c)){ //if its not a letter, ignore it
      if(back == '\n'){ //if back output was a newline, 
        continue; //continue so no more new lines print
      }
        else{ //if a newline didn't print yet, do that and update back
          back = '\n';
          printf("\n");
        }  
      }
      else{ //if current character is a letter
        back = c; //update back to current character
        putchar(c);//print character
      }
    }  
  return 0;
}  



