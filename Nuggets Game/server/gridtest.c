#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "grid.h"

#include <time.h>

int main() {
    printf("testing grid_new\n");
    grid_t* grid = grid_new("../maps/big.txt");
    //grid_t* grid = grid_new("../maps/contrib21s/sapphire.txt");

    printf("grid dimension: %d x %d\n", grid_getX(grid),grid_getY(grid));
    printf("grid map: \n%s\n",grid_getStr(grid));

    printf("testing grid_randloc\n");
    int n = grid_randloc(grid_getStr(grid),time(NULL));
    printf("%d\n", n);

    printf("testing grid_get at index grid_randloc\n");
    printf("%c\n", grid_get(grid_getStr(grid), n));

    printf("testing grid_NtoX\n");
    int x = grid_NtoX(grid, n);
    printf("%d\n", x);

    printf("testing grid_NtoY\n");
    int y = grid_NtoY(grid, n);
    printf("%d\n", y);

    printf("testing grid_XYtoN\n");
    printf("%d\n", grid_XYtoN(grid, x, y));

    int len = strlen(grid_getStr(grid));
    char* vis = malloc(len*sizeof(char));
    for (int i = 0; i < len; i++){
        vis[i] = '0';
    }
    //x=136;
    //y=35;
    //x=15;
    //y=6;
    printf("testing visibility with player @(%d,%d)\n",x,y);
    //printf("strlen(gridStr) = %d,strlen(vis) = %ld\n", len, strlen(vis));
    //printf("initialize vis string: \n%s\n", vis);
    //visibility(grid, vis, x,y);
    visibility(grid, vis, x,y);
    //for (int i = 0;i<10;i++) {
        //n = grid_randloc(grid_getStr(grid));
        //(1);
        //printf("n = %d\n", n);
        //x = grid_NtoX(grid, n);
        //y = grid_NtoY(grid, n);
        //visibility(grid, vis, x,y);
    //}
    printf("after visibility check: \n%s\n", vis);
    printf("testing grid_display: \n");
    printf("%s\n", grid_display(grid, NULL, NULL, '@', vis));

    printf("testing hasSpace");
    char map[100];
    printf("this should be 0: %d\n", grid_hasSpace(map));
    strcpy(map,"1010101");
    printf("testing grid_randloc\n");
    n = grid_randloc(map,1);
    printf("%d\n", n);
    printf("this should be 0: %d\n", grid_hasSpace(map));
    strcpy(map,"---.--+");
    printf("testing grid_randloc\n");
    n = grid_randloc(map,1);
    printf("%d\n", n);
    printf("this should be 1: %d\n", grid_hasSpace(map));
    printf("testing grid_randloc\n");
    n = grid_randloc(map,1);
    printf("%d\n", n);

    printf("testing grid_delete\n");
    grid_delete(grid);
}