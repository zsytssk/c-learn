#include <stdio.h>
#include <string.h>

int main()
{
    char str1[] = "Hello";
    char str2[] = "World";
    printf("%d\n", strlen(str1));

    char str3[100];
    strncat(str3, str1, strlen(str1));
    strncat(str3, ", ", 2);
    strncat(str3, str2, strlen(str2));

    printf("%s\n", str3);

    char str4[100];
    sprintf(str4, "%s %s", str1, str2);
    printf("%s\n", str4);
}