#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <time.h>

#define BUFLEN 20560
#define maxc 26
#define FSIZE 10000

typedef struct trienode{
	struct trienode *arr[maxc];
	bool isend;
}trienode;

int ndcnt;
trienode *createnode(){
	ndcnt += 1;
	int i;
	trienode *newnode = malloc(sizeof(*newnode));
	
	for(i = 0; i <maxc ; i++)
		newnode->arr[i] = NULL;
	
	newnode->isend = false;

	return newnode;
}


bool insert(trienode **root, char *pprefix){
	
	
	if(*root == NULL){
		*root = createnode();
	}

	
	int i;
	unsigned char *prefix = (unsigned char *)pprefix;
	int prefixlen = strlen(pprefix);
	trienode *temp = *root;
	int ascii = 0;
	
	for(i = 0; i<prefixlen; i++){
		ascii = tolower(prefix[i]) - 97;
		if(temp->arr[ascii] == NULL)
			temp->arr[ascii] = createnode();
		
	temp = temp->arr[ascii];
	}

	if(temp->isend)
		return false;
	else{
		temp->isend = true;
		return true;
	}
}


void disptrienode_rec(trienode *node, char *prefix, int plen){
	
	int i;
	char newprefix[plen + 2];
	memcpy(newprefix, prefix, plen);
	newprefix[plen + 1] = 0;
	
	

	if(node->isend)
		printf("word %s\n", prefix);

	for(i = 0; i <maxc; i++){
		if(node->arr[i] != NULL){
			newprefix[plen] = i+97;
			disptrienode_rec(node->arr[i], newprefix, plen+1);
		}
	}

}


void disptrienode(trienode *root){
	
	if(root == NULL)
		return;
	
	disptrienode_rec(root, NULL, 0);

}


bool searchtrie(trienode *root, char *pprefix){
	unsigned char *prefix = (unsigned char *) pprefix;
	int plen = strlen(pprefix);
	trienode *tmp = root;
	int ascii = 0;

	for(int i=0; i<plen; i++){
		ascii = prefix[i] - 97;
		if(tmp->arr[ascii] == NULL){
			return false;
		}
		tmp = tmp->arr[ascii];
		
	}
	return tmp->isend;
}


bool node_has_children(trienode *node){
	if(node == NULL) return false;
	
	for(int i=0; i<maxc; i++){
		if(node->arr[i] != NULL){
			return true;
		}
	}
	return false;
}

 
trienode *deletestr_rec(trienode *node, unsigned char *prefix, bool *deleted){
	if(node == NULL) return node;


	int ascii = 0;	
	if(*prefix == '\0'){
		if(node->isend){
			node->isend = false;
			*deleted = true;		
		
			if(node_has_children(node) == false){
				free(node);
				node = NULL;
			}
		}
		return node;
	}
	
	ascii = prefix[0] - 97;
	node->arr[ascii] = deletestr_rec(node->arr[ascii], prefix+1, deleted);
	

	if(*deleted && node_has_children(node) == false && 
		node->isend == false) {
			free(node);
			node = NULL;
		}
	return node;	
}


bool deletestr(trienode **root, char *pprefix){
	unsigned char * prefix = (unsigned char *) pprefix;
	bool result = false;
	
	if(*root == NULL)
		return false;
	
	*root = deletestr_rec(*root, prefix, &result);
	return result;
}


int main(void){

	trienode *root = NULL;

	int fd = open("filenames.txt", O_RDONLY, 777);
	if(fd){
	long int filelen1 = lseek(fd, 0, SEEK_END);
	long int filelen = filelen1;

	//printf("%ld\n", filelen);
	lseek(fd, 0, SEEK_SET);
	int dataread = 0;
	char buffer[BUFLEN];
	int i, j;
	char *filename = malloc(sizeof(char) *FSIZE);
	int unfnfllen = 0;
	
	clock_t start, end;
	start = clock();
	while(filelen>0){
	 	dataread = read(fd, buffer, BUFLEN);
		if(dataread <= 0)
			break;
		j = 0;
		for(i=0; i<dataread; i++){
			if(buffer[i] == '\n'){
				filename[j] = '\0';
				insert(&root, filename);
				filename[0] = '\0';
				j = 0;
			}
			else{
				filename[j++] = buffer[i];
			}			
		}
		
		if(filename[0] != '\0'){
			//printf("%c\n", filename[j]);
			filename[j] = '\0';
			unfnfllen = strlen(filename);
			//printf("%s\n", filename);
			//printf("%d\n", j);
			dataread = i-j-2;
			lseek(fd,  unfnfllen-1, SEEK_CUR);
		}
		//printf("filelen befor %ld\n", filelen);
		filelen = filelen - dataread;
		//printf("dataraed %d\n", dataread);
	//	printf("after filelen %ld\n", filelen);
		dataread = 0;
	}
	end = clock();
	printf("time for insertion %lf\n", ((double)end - start) / CLOCKS_PER_SEC);
	
/*	

	insert(&root, "you");
	insert(&root, "suck");
	insert(&root, "can");
	insert(&root, "you");
	insert(&root, "dig");
	insert(&root, "it");
	insert(&root, "sucker");
//	disptrienode(root);
	printf("search for sucker : %d\n", searchtrie(root, "sucker"));
	printf("search for dig : %d\n", searchtrie(root, "dig"));
	deletestr(&root, "dig");*/
	filelen = filelen1;
	lseek(fd, 0, SEEK_SET);
	start = clock();
	while(filelen>0){
	 	dataread = read(fd, buffer, BUFLEN);
		if(dataread <= 0)
			break;
		j = 0;
		for(i=0; i<dataread; i++){
			if(buffer[i] == '\n'){
				filename[j] = '\0';
		//		printf("search for %s : %d\n", filename, searchtrie(root, filename));
				filename[0] = '\0';
				j = 0;
			}
			else{
				filename[j++] = buffer[i];
			}			
		}
		
		if(filename[0] != '\0'){
		//	printf("%c\n", filename[j]);
			filename[j] = '\0';
			unfnfllen = strlen(filename);
	//		printf("%s\n", filename);
//			printf("%d\n", j);
			dataread = i-j-2;
			lseek(fd,  unfnfllen-1, SEEK_CUR);
		}
		//printf("filelen befor %ld\n", filelen);
		filelen = filelen - dataread;
		//printf("dataraed %d\n", dataread);
		//printf("after filelen %ld\n", filelen);
		dataread = 0;
	}
	close(fd);
	
	end = clock();
	printf("time for search %lf\n", ((double)end - start)/CLOCKS_PER_SEC);
	printf("node count %d\n", ndcnt);
	}
	return 0;
}