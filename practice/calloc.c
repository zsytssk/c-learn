#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct line
{
    int x;
    int y;
};

int main()
{
    struct line *l1 = calloc(1, sizeof(*l1));
    l1->x = 10;
    l1->y = 10;
    printf("x: %d, y: %d\n", l1->x, l1->y);
    free(l1);

    // 获取当前时间戳
    time_t current_time = time(NULL);

    // 打印时间戳
    if (current_time == ((time_t)-1))
    {
        printf("Failed to obtain the current time.\n");
        return 1;
    }

    printf("Current time: %ld\n", (long)current_time);

    return 0;
}