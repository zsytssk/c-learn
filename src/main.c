#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct point
{
    int x;
    int y;
};

struct line
{
    struct point start;
    struct point end;
};

int main()
{
    struct line l1;
    printf("x: %d, y: %d", l1.start.x, l1.start.y);
}