#include <stdio.h>

/* define the union here */

union data
{
    int data[6];
    char chars[21];
};

int main()
{

    // initializer lists like this are assigned to the first member of the union/struct!
    // There are 6 ints here so...
    union data intCharacters;
    intCharacters.data[0] = 1853169737;
    intCharacters.data[1] = 1936876900;
    intCharacters.data[2] = 1684955508;
    intCharacters.data[3] = 1768838432;
    intCharacters.data[4] = 561213039;
    intCharacters.data[5] = 0;
    // {{1853169737, 1936876900, 1684955508, 1768838432, 561213039, 0}}

    /* testing code */
    printf("[");
    // only go to 18 because 1 byte is for the terminating 0 and we don't print the last in the loop
    for (int i = 0; i < 19; ++i)
        printf("%c, ", intCharacters.chars[i]);
    printf("%c]\n", intCharacters.chars[19]);

    printf("%s\n", intCharacters.chars);
}