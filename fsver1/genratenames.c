#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>

#define maxval 100

char name[maxval];

char *numtoname[10] = { "zero",	"one", "two", "three", "four", "five", "six", "seven", "eight", "nine"};

void *getname(){

	int tm = (int)clock();
	int temp = 0;
	name [0] = '\0';
	while(tm){
		temp = tm%10;
		strcat(name, numtoname[temp]);
		tm = tm/10;	
	}

}



int main(int argc, char *argv[]){
	
	int num = atoi(argv[1]);
	clock_t tm;
	int i;

	FILE *fp = fopen("filenames.txt", "w+");
	char *nm = malloc(sizeof(char) * maxval);
	for(i = 0; i<num; i++){
		getname();
		strcpy(nm, name);
		strcat(nm, "\n\0");
		fwrite(nm, strlen(nm), 1, fp);	
	}
	
	fclose(fp);
	return 0;
}
