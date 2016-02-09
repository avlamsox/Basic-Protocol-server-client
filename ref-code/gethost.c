#include <stdio.h>
#include <unistd.h>

int main()
{
	char name[10];
	gethostname(name,20);
	printf("Host name is %s\n",name);
	
	return 0;
}
