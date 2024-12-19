/* 
 * grid.c - Nuggest grid module
 * see grid.h for more information.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <math.h>
#include "players.h"
#include "gold.h"

/**************** file-local global variables ****************/
/* none */

/**************** local types ****************/

/**************** global types ****************/
typedef struct grid {
  char *gridStr;       // string representation of the grid
  int x;
  int y;
} grid_t;

/**************** local functions ****************/
/* not visible outside this file */
static int isVisible(grid_t* grid, int n, int px, int py);

/**************** global functions ****************/
/* that is, visible outside this file */
/* see grid.h for comments about exported functions */
int grid_getX(grid_t* grid) {return grid->x;}
int grid_getY(grid_t* grid) {return grid->y;}
char* grid_getStr(grid_t* grid) {return grid->gridStr;}

/**************** grid_new() ****************/
/* see grid.h for description */
grid_t*
grid_new(char* mapFile)
{
  grid_t* grid = malloc(sizeof(grid_t));

  if (grid == NULL) {
    return NULL;              // error allocating grid
  } else {
    FILE *fp;
    fp = fopen(mapFile, "r"); // ASSUME file follows map formats
    if (fp == NULL) {
      return NULL;            // error reading file
    }

    char line[1024];
    char* gridStr=malloc(100000);  // ASSUMPTION: max map size
    strncpy(gridStr, "", 100000);
    int y = 0;
    while (fgets(line, sizeof(line), fp)) {
        /* note that fgets don't strip the terminating \n, checking its
           presence would allow to handle lines longer that sizeof(line) */
        strcat(gridStr, line);
        y = y + 1;
    }
    int x = strlen(line);
    grid->x = x;
    grid->y = y;
    grid->gridStr = gridStr;

    fclose(fp);
    //printf("%s", gridStr);
    return grid;
  }
}


/**************** grid_get() ****************/
/* see grid.h for description */
char grid_get(char* map, int n) {
  if (map == NULL || n>= strlen(map) || n<0) {
    return '\0';
  }
  return *(map+n);
}


/**************** grid_hasSpace() ****************/
/* see grid.h for description */
int grid_hasSpace(char* map) {
  if (map != NULL) {
    return (strchr(map,'.') != NULL);
  } else {
    return 0;
  }
}

/**************** grid_randloc() ****************/
/* see grid.h for description */
int grid_randloc(char* map, int seed) {
  if (map == NULL) {
    return -1;
  }
  if (grid_hasSpace(map) == 0) {
    return -1;
  }
  srand(seed);
  int lower = 0, upper = strlen(map)-1;
  int num = (rand() % (upper - lower + 1)) + lower;
  while (map[num] != '.') {
    num = (rand() % (upper - lower + 1)) + lower;
  }
  return num;
}

/**************** grid_NtoX() ****************/
/* see grid.h for description */
int grid_NtoX(grid_t* grid, int n) {
  if (grid == NULL || grid->gridStr == NULL || n>=(grid->x*grid->y) || n<0) {
    return -1;
  }
  return n % grid->x;
}
/**************** grid_NtoY() ****************/
/* see grid.h for description */
int grid_NtoY(grid_t* grid, int n) {
  if (grid == NULL || grid->gridStr == NULL || n>=(grid->x*grid->y) || n<0) {
    return -1;
  }
  return n / grid->x;
}

/**************** grid_XYtoN() ****************/
/* see grid.h for description */
int grid_XYtoN(grid_t* grid, int x, int y) {
  if (grid == NULL || grid->gridStr == NULL || x>(grid->x) || x<0 || y>(grid->y)|| y<0) {
    return -1;
  }
  return (grid->x)*y +x;
}

/**************** grid_display() ****************/
/* see grid.h for description */
char* 
grid_display(grid_t* grid, players_t* players, gold_t* gold, 
             char player, char* vis) 
{
  if (grid == NULL || grid->gridStr == NULL) {
    return NULL;
  }

  // set base map
  char* map = grid->gridStr;
  char* output = malloc(strlen(map) + 1);
  strcpy(output, map);

  // add in players
  players_addToMap(players, output, player);
  // add in gold
  gold_addToMap(gold, output);

  // factor in visibility
  if (vis == NULL) {
    return output;
  } 
  
  for (int i = 0; i < strlen(map); i++) {
    char M = output[i];
    char V = vis[i];
    // make never-seen regions blank
    if (V == '0') {
      output[i] = ' ';
    // remove gold & players from not-currently but-previously seen regions
    } else if (V == '2' && (M == '*' || isalpha(M) != 0)) {
      output[i] = map[i];
    } 
  }
  
  return output;
}

/**************** grid_iterate() ****************/
/* see grid.h for description */
void 
grid_iterate(grid_t* grid, void* arg,
                void (*itemfunc)(void* arg, char* c) )
{
  if (grid != NULL && grid->gridStr != NULL && itemfunc != NULL) {
    char* gridStr = grid->gridStr;
    char c;
    int i = 0;
    while ((c = *(gridStr+i)) != EOF) {
      itemfunc(arg, (gridStr+i));
      i++;
    }

  }
}

/**************** grid_delete() ****************/
/* see grid.h for description */
void 
grid_delete(grid_t* grid)
{
  if (grid != NULL) {
    free(grid->gridStr);
    free(grid);
  }
}




/**************** visibility() ****************/
/* see grid.h for description */
char*
visibility(grid_t* grid, char* vis, int px, int py) {
  char* gridStr = grid_getStr(grid);
  int n = grid_XYtoN(grid,px,py);
  char c = gridStr[n];
  if (c != '.' && c != '#') {
    fprintf(stderr,"Invalid Player Location!\n");
    return vis;
  }
  // for point in gridStr, 
  for (int i = 0; i<strlen(gridStr); i++) {
    char point = gridStr[i];
    //printf("Current point: [%c], on visibility string: %c\n", point, vis[i]);
    if (point != ' ') {
      if (point == '\n') {
        vis[i] = '1';
      } else {
        int visible = isVisible(grid, i, px,py);
        //printf("Is it visible? [%d]\n", visible);
        
        if (visible == 0) {     // if currently not visible
          if (vis[i] == '1' || vis[i] == '2') {  // was visible, change to 2
            vis[i] = '2';       
          } else {              // wasn't visible, change to 0
            vis[i] = '0';       
          }
        } else {                // if currently visible
          vis[i] = '1';         // change to 1 regardless visible before
        }
      }
    } 
    //printf("Modified visibility string: [%c]\n", vis[i]);
  }
  //vis[n] = '1'; // player location is visible 
  return vis;
}


/*******************************/
/* local functions */
static int
isVisible(grid_t* grid, int n, int px, int py) {
  char* gridStr = grid_getStr(grid);
  int x = grid_NtoX(grid, n);
  int y = grid_NtoY(grid, n);
  //printf("inspecting string:(%d,%d)[%c]\n",x,y, gridStr[n]);


  if (px==x || px==x+1 || px==x-1) { 
    if (py>y) {
      for (int i = y+1; i<py; i++) {
        char new_point = gridStr[grid_XYtoN(grid,px,i)];
        if (new_point != '.'){// || new_point == '|'|| new_point == '#'|| new_point == ' ') {
          return 0;
        } 
      }
    } else {
      for (int i = py+1; i<y; i++) {
        char new_point = gridStr[grid_XYtoN(grid,px,i)];
        if (new_point != '.'){// || new_point == '|'|| new_point == '#'|| new_point == ' ') {
          return 0;
        } 
      }
    }

    return 1;
  }

  if (py==y || py==y+1 || py==y-1) { 
    if (px>x) {
      for (int i = x+1; i<px; i++) {
        char new_point = gridStr[grid_XYtoN(grid,i,py)];
        if (new_point != '.'){// || new_point == '|' || new_point == '#'|| new_point == ' ') {
          return 0;
        } 
      }
    } else {
      for (int i = px+1; i<x; i++) {
        char new_point = gridStr[grid_XYtoN(grid,i,py)];
        if (new_point != '.'){// || new_point == '|' || new_point == '#'|| new_point == ' ') {
          return 0;
        } 
      }
    }
    return 1;
  }
  // *********** CASE 1 ********
  // P . . .   P=player, x=point
  // . . . .
  // . . . x
  if (px<x && py<y) { 
    double tan = (double)(x-px) / (y-py);
    for (int i = px+1;i<x;i++) {
      double y_intercept = (double)(i-px) / tan + py;
      // check floor & ceil
      int y1 = (int)floor(y_intercept);
      int y2 = (int)ceil(y_intercept);
      if (y1 == py) {
        y1 = py+1;
      } else if (y2 == y) {
        y2 = y-1;
      }
      char new_point1 = gridStr[grid_XYtoN(grid,i,y1)];
      char new_point2 = gridStr[grid_XYtoN(grid,i,y2)];
      //printf("px<x&py<y, two points inspecting: (%d,%d)[%c], (%d,%d)[%c]\n",i,y1,new_point1,i,y2,new_point2);
      if (new_point1 == '-' || new_point1 == ' '|| new_point1 == '|' || new_point2 == '|' || new_point2 == ' '|| new_point1 == '#'|| new_point2 == '#'|| new_point1 == '+'|| new_point2 == '+') {
        //printf("two points inspecting: (%d,%d)[%c], (%d,%d)[%c]\n",i,y1,new_point1,i,y2,new_point2);
        //printf("hit a wall! Return 0 (not visible)\n");
        return 0;
      }
    }

    for (int i = py+1;i<y;i++) {
      double x_intercept = tan / (double)(i-py) + px;
      int x1 = (int)floor(x_intercept);
      int x2 = (int)ceil(x_intercept);
      if (x1 == px) {
        x1 = px+1;
      } else if (x2 == x) {
        x2 = x-1;
      }
      char new_point1 = gridStr[grid_XYtoN(grid,x1,i)];
      char new_point2 = gridStr[grid_XYtoN(grid,x2,i)];
      if (new_point1 == '-' || new_point1 == ' '|| new_point1 == '|' || new_point2 == '-' || new_point2 == ' '|| new_point1 == '#'|| new_point2 == '#'|| new_point1 == '+'|| new_point2 == '+') {
        return 0;
      }      
    }
    //printf("good\n");
  }

  // *********** CASE 2 ********
  // x . . .   P=player, x=point
  // . . . .
  // . . . P
  if (px>x && py>y) { 
    //printf("inspecting char: (%d, %d),[%c]\n",x,y,gridStr[n]);
    double tan = (double)(x-px) / (y-py);
    for (int i = x+1;i<px;i++) {
      double y_intercept = (double)(i-x) / tan + y;
      // check floor & ceil
      int y1 = (int)floor(y_intercept);
      int y2 = (int)ceil(y_intercept);

      if (y1 == y) {
        y1 = y+1;
      } else if (y2 == py) {
        y2 = py-1;
      }
      char new_point1 = gridStr[grid_XYtoN(grid,i,y1)];
      char new_point2 = gridStr[grid_XYtoN(grid,i,y2)];
      //printf("two points inspecting: (%d,%d)[%c], (%d,%d)[%c]\n",i,y1,new_point1,i,y2,new_point2);
      if (new_point1 == '|' || new_point1 == ' ' || new_point1 == '-' || new_point2 == '|' || new_point2 == ' '|| new_point1 == '#'|| new_point2 == '#'|| new_point1 == '+'|| new_point1 == '+') {
        //printf("Issue at CASE2: two points inspecting: (%d,%d)[%c], (%d,%d)[%c]\n",i,y1,new_point1,i,y2,new_point2);
        //printf("hit a wall! Return 0 (not visible)\n");
        return 0;
      }
    }
    
    //printf("inspecting char: (%d, %d),[%c]\n",x,y,gridStr[n]);
    double cot = (double)(y-py)/(x-px);
    for (int i = y+1;i<py;i++) {
      double x_intercept = (double)(i-y)/cot + x;
      int x1 = (int)floor(x_intercept);
      int x2 = (int)ceil(x_intercept);
      if (x1 == x) {
        x1 = x+1;
      } else if (x2 == px) {
        x2 = px-1;
      }
      char new_point1 = gridStr[grid_XYtoN(grid,x1,i)];
      char new_point2 = gridStr[grid_XYtoN(grid,x2,i)];
      //printf("two points inspecting: (%d,%d)[%c], (%d,%d)[%c]\n",x1,i,new_point1,x2,i,new_point2);
      if (new_point1 == '-' || new_point1 == ' '|| new_point2 == '|' || new_point2 == '-' || new_point2 == ' '|| new_point1 == '#'|| new_point2 == '#'|| new_point1 == '+'|| new_point2 == '+') {
        //printf("hit a wall! Return 0 (not visible)\n");
        return 0;
      }      
    }
  }

  // *********** CASE 3 ********
  // . . . P   P=player, x=point
  // . . . .
  // x . . .
  if (px>x && py<y) { 
    double tan = (double)(px-x) / (y-py);
    for (int i = x+1;i<px;i++) {
      double y_intercept = (double)(i-x) / tan + py;
      // check floor & ceil
      int y1 = (int)floor(y_intercept);
      int y2 = (int)ceil(y_intercept);
      if (y1 == py) {
        y1 = py+1;
      } else if (y2 == y) {
        y2 = y-1;
      }
      char new_point1 = gridStr[grid_XYtoN(grid,i,y1)];
      char new_point2 = gridStr[grid_XYtoN(grid,i,y2)];
      //printf("two points inspecting: (%d,%d)[%c], (%d,%d)[%c]\n",i,y1,new_point1,i,y2,new_point2);
      if (new_point1 == '|' || new_point2 == '|' || new_point1 == ' '|| new_point2 == ' '|| new_point1 == '#'|| new_point2 == '#' ||new_point2 == '-' || new_point2 == '+'|| new_point1 == '+') {
        //printf("Issue at CASE3: two points inspecting: (%d,%d)[%c], (%d,%d)[%c]\n",i,y1,new_point1,i,y2,new_point2);
        //printf("hit a wall! Return 0 (not visible)\n");
        return 0;
      }
    }
    //printf("good\n");

    for (int i = py+1;i<y;i++) {
      double x_intercept = tan / (double)(i-py) + x;
      int x1 = (int)floor(x_intercept);
      int x2 = (int)ceil(x_intercept);
      if (x1 == x) {
        x1 = x+1;
      } else if (x2 == px) {
        x2 = px-1;
      }
      char new_point1 = gridStr[grid_XYtoN(grid,x1,i)];
      char new_point2 = gridStr[grid_XYtoN(grid,x2,i)];
      if (new_point1 == '-' || new_point1 == ' '|| new_point2 == '|' || new_point2 == '-' || new_point2 == ' '|| new_point1 == '#'|| new_point2 == '#'|| new_point1 == '+'|| new_point2 == '+') {
        return 0;
      }      
    }
  }

  // *********** CASE 4 ********
  // . . . x   P=player, x=point
  // . . . .
  // P . . .
  if (px<x && py>y) { 
    double tan = (double)(x-px) / (py-y);
    for (int i = px+1;i<x;i++) {
      double y_intercept = (double)(i-px) / tan;
      // check floor & ceil
      int y1 = py - (int)floor(y_intercept);
      int y2 = py - (int)ceil(y_intercept);
      if (y1 == y) {
        y1 = y+1;
      } else if (y2 == py) {
        y2 = py-1;
      }
      char new_point1 = gridStr[grid_XYtoN(grid,i,y1)];
      char new_point2 = gridStr[grid_XYtoN(grid,i,y2)];
      //printf("two points inspecting: (%d,%d)[%c], (%d,%d)[%c]\n",i,y1,new_point1,i,y2,new_point2);
      if (new_point1 == '-' || new_point1 == '|' || new_point2 == '|' || new_point1 == ' '|| new_point2 == ' '|| new_point1 == '#'|| new_point2 == '#'|| new_point1 == '+'|| new_point2 == '+') {
        //printf("Issue at CASE4: two points inspecting: (%d,%d)[%c], (%d,%d)[%c]\n",i,y1,new_point1,i,y2,new_point2);
        //printf("hit a wall! Return 0 (not visible)\n");
        return 0;
      }
    }

    //printf("inspecting char: (%d, %d),[%c]\n",x,y,gridStr[n]);
    for (int i = y+1;i<py;i++) {
      double x_intercept = tan / (double)(i-y) + px;
      int x1 = (int)floor(x_intercept);
      int x2 = (int)ceil(x_intercept);
      if (x1 == px) {
        x1 = px+1;
      } else if (x2 == x) {
        x2 = x-1;
      }
      char new_point1 = gridStr[grid_XYtoN(grid,x1,i)];
      char new_point2 = gridStr[grid_XYtoN(grid,x2,i)];
      if (new_point1 == '-' || new_point1 == ' '|| new_point1 == '|' || new_point2 == '-' || new_point2 == ' '|| new_point1 == '#'|| new_point2 == '#'|| new_point1 == '+'|| new_point2 == '+') {
        //printf("Issue at CASE4: two points inspecting: (%d,%d)[%c], (%d,%d)[%c]\n",x1,i,new_point1,x2,i,new_point2);
        //printf("hit a wall! Return 0 (not visible)\n");
        return 0;
      }      
    }
  }
  

  return 1;
}
