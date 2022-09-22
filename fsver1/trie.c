#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define maxc 256

typedef struct trienode{
	struct trienode *arr[maxc];
	bool isend;
}trienode;


trienode *createnode(){
	
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
	
	
	for(i = 0; i<prefixlen; i++){
		if(temp->arr[prefix[i]] == NULL)
			temp->arr[prefix[i]] = createnode();
		
		
	
	temp = temp->arr[prefix[i]];
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
			newprefix[plen] = i;
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
	
	for(int i=0; i<plen; i++){
		if(tmp->arr[prefix[i]] == NULL){
			return false;
		}
		tmp = tmp->arr[prefix[i]];
		
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
	
	node->arr[prefix[0]] = deletestr_rec(node->arr[prefix[0]], prefix+1, deleted);
	
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

	insert(&root, "you");
	insert(&root, "suck");
	insert(&root, "can");
	insert(&root, "you");
	insert(&root, "dig");
	insert(&root, "it");
	insert(&root, "sucker");
	disptrienode(root);
	printf("search for su : %d\n", searchtrie(root, "su"));
	printf("search for sucker : %d\n", searchtrie(root, "sucker"));
	printf("search for dig : %d\n", searchtrie(root, "dig"));
	deletestr(&root, "dig");
	printf("search for dig : %d\n", searchtrie(root, "dig"));
	
	return 0;
}
