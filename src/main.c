#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct line;

int main()
{
    struct line l1;
    printf("x: %d, y: %d", l1.start.x, l1.start.y);
}