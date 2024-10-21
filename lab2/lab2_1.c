#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

extern char** environ;

int main(int argc, char *argv[]) {
    char** envp = environ;
    char str[30];
    char name[20];
    char value[20];

    printf("Enter a string: ");
    scanf("%s", str);

    if (putenv(str) < 0) {
        fprintf(stderr, "Couldn't add the string \"%s\" to the enviroment", str);
        return 1;
    };

    printf("Enter a name: ");
    scanf("%s", name);

    printf("Value of %s: %s\n", name, getenv(name));

    printf("Enter new value: ");
    scanf("%s", value);
    setenv(name, value, 1);

    printf("Value of %s: %s\n", name, getenv(name));

    return 0;
}