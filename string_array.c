#include <stdio.h>

int main(int argc, char *argv[])
{
    int i = 0;
    printf("argument count: %d\n", argc);
    printf("argument 0: %s\n", argv[0]);
    for (i = 1; i < argc; i++) {
        printf("arg %d: %s\n", i, argv[i]);
    }

    char *states[] = {
        "California", "Oregon", "Washington",
        "Texas", "North Carolina"
    };

    int num_states = 5;

    for (i = 0; i < num_states; i++) {
        printf("state %d: %s\n", i, states[i]);
    }

    return 0;
}
