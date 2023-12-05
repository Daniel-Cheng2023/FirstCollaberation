#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

int main(){
    char *s, *a;
    int i;

    free(s);
    s = NULL;
    a = s;
    printf( "%s\n", s );
    printf( "%s\n", a );
    s = NULL;
    free(s);
    s = NULL;
    s = "123456";
    printf( "%s\n", s );
    return 0;
}