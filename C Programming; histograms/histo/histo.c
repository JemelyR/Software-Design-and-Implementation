/*
 *
 * histo.c
 *
 * reads positive intergers from stdin, prints out a histogram
 * dynamically scales based on inputs
 * histogram must remain having 16 bins
 *
 * Jemely Robles, January 2023, cs50, 23W
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* ******** main ******** */
int main(){
  
  int bins = 16;
  int range = bins;
  int histo[range];
  int binSize = 1;
  int left = 0;
  int right = 0;
  int c;
  int tracker;

  for(int i=0; i<range;i++){ //initialize a way to keep track of inputs
    histo[i] = 0;
  }

  printf("%d bins of size %d for range [0, %d)\n", bins, binSize, range);

  while(scanf("%d", &c) == 1){ //scan the inputs
    if(c >= range){ //if the input is or is bigger than the range, increase range
      while(c >= range){ 
        //while input is still bigger or equal to, increase range and bin size by 2
        range = range * 2;
        binSize = binSize * 2;
        right = binSize -1;//update the right side of histogram
        for (int i=0; i < 8; i++){ //scale histo, first 8 get joined together 2 by 2
          histo[i] = histo[2*i] + histo[(2*i)+1];
        }
        for (int i=8; i < bins; i++){//next 8 get initialized
          histo[i] = 0;
        }   
      }
      printf("%d bins of size %d for range [0,%d)\n", bins, binSize, range);
    } 
    tracker = floor(c/binSize);//divide c by binsize to see which bin it would fall into
    histo[tracker]+=1;//add one to that index(representsbin) for * printing later
  } 
  for(int i=0;i < range; i+=binSize) { //for all the bins
    printf("[%2d:%2d] ", left, right);//print the bins with their left and right value, left will always start at 0
    int track = floor(i/binSize);//access index that coresponds to the bin
    for (int i=0; i<histo[track]; i++){
      printf("*");  //print * depending on value in that index
    }  
    left += binSize;//update left by binsixe
    right += binSize;//update right
    printf("\n");
  }

return 0;
}





