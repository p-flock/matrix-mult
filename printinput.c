#include <stdio.h>
#include <stdlib.h>

int main(){
	char args[1024];
	fgets(args,BUFSIZ, stdin);
	printf("%s\n", args);
}
