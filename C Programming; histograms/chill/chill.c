/*
 * chill.c
 * 
 * calculates wind chill based on temp and wind speed
 *
 * temp or T = temperature
 * speed or V = wind speed
 *
 * if no argument is given, gives auto table of temps and speeds and their calculations
 * if one argument given, print calc using that argument as temp, with speeds 5.0, 10.0, and 15.0
 * if two arguments, uses first as temp and second as speed, prints that calc
 *
 * results printed as table
 *
 * Jemely Robles, January 2023, cs50 23W
 */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* ***** function declarations ***** */

float windChill(float T, float V);

/* ******** main ******** */
int main(const int argc, char *argv[])
{
 float T = 0;
 float V = 0;
 float chill;

 if (argc == 1) { //if no arguments, print this table
   T = -10.0; 
   V = 5.0;  
   printf("  Temp   Wind  Chill\n -----  -----  ----- \n"); //table header
   while (T <= 40.0) { //until temp gets to 40, 
     for (V = 5; V <= 15.0; V+=5) { //go through speeds 5 through 15, up by 5 each time
       chill = windChill(T, V); //calculate wind chill with current temp and speed
       printf(" %5.1f  %5.1f  %5.1f \n", T, V, chill); //print this table out
     }  
     printf("\n");
     //when loop of speeds is done, increase temp by 10 and start over for new calculations
     T += 10.0;
   }  
 } 

 else if (argc == 2) { //if one argument is given, that is the temp
   if (50.0 > atof(argv[1]) && atof(argv[1]) > -99.0) { //if that argument is in the proper range
    T = atof(argv[1]); //update temp value
    V = 5.0; //start with speed of 5
    printf("  Temp   Wind  Chill\n -----  -----  ----- \n"); //header
    while (V <= 15.0) { 
      //do calculations using given temp and 3 diff speeds, 5, 10, and 15
      chill = windChill(T, V);
      printf(" %5.1f  %5.1f  %5.1f\n", T, V, chill); //print as table
      V += 5.0;
    } 
   }
   else{ //if given temp not within range, print error message
     printf("Wind velocity must be between 50.0 and -99.0 degrees Fahrenheit %s\n", argv[0]);
   }  
 } 
 
 else if (argc == 3) { //if two arguments, argument one is new temp and argument 2 is new speed
   if (50.0 > atof(argv[1]) && atof(argv[1]) > -99.0){ //can only update temp if given argument is within range
     if (0.5 <= atof(argv[2]) && atof(argv[2]) <= 231.0){ //only update speed if second argument in range
       T = atof(argv[1]);
       V = atof(argv[2]);  
       printf("  Temp   Wind  Chill\n -----  -----  ----- \n");//header
       chill = windChill(T, V);//calculate
       printf(" %5.1f  %5.1f  %5.1f \n", T, V, chill);//print as table
     }
   } 
   if(50.0 <= atof(argv[1]) || atof(argv[1]) <= -99.0){//if given temp not in range, print error message
     printf("Temperature must be less than 50.0 and greater than -99.0 degrees Fahrenheit %s\n", argv[0]);
   }
   if(0.5 > atof(argv[2]) || atof(argv[2])> 231.0){//if given speed not in range, print error message
     printf("Wind velocity must be greater than 0.4 and less than 232.0 mph %s\n", argv[0]);
     return 1;
   }  
 }  
 else{ // if more than 2 arguments given, print usage error message
  printf("usage: too many arguments %s\n", argv[0]);
  return 2; //exit with error status
 } 
  return 0;  // exit status
}


/* ************** windChill ************** */
/*
 * calculates wind chill of given values
 * T = temperature fahrenheiht
 * V = velocity of wind m/h
 *
 */
 
float
windChill(float T, float V)
{
  float WindChill;
  //formula for calculating wind chill, based on a temperature and wind speed
  WindChill = 35.74 + 0.6215*T - 35.75*(pow(V, 0.16)) + 0.4275*T*(pow(V, 0.16));

  return WindChill;
}



