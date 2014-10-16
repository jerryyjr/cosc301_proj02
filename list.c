#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include <string.h>

/* your list function definitions */

void list_clear(struct node *list) {
    while (list != NULL) {
        struct node *tmp = list;
        list = list->next;
        free(tmp);
    }
}

void list_print(const struct node *list) {
    int i = 0;
    printf("***List Contents Begin***\n");
    while (list != NULL) {
        printf("List item %d: %s\n", i++, list->num);
        list = list->next;
    }
	printf("***List Contents End***\n");
}

void list_append(char *num, struct node **head) {   //append num in order
    struct node *tmp=*head;
	struct node *new=malloc(sizeof(struct node));
	
	strcpy(new->num,num);
	new->next=NULL;
	if(tmp==NULL){
		
		*head=new;
	}else {
		while(tmp->next!=NULL){
			
			tmp=tmp->next;
		}
		tmp->next=new;
	}
	
}

/*char *findme(struct node **head){
	struct node *tmp=*head;
	
	if(tmp=NULL){
		return NULL;
	}

	while(tmp->next!=NULL){
		if (strcmp(tmp
	}
}*/



/*int main(int argc, char **argv){
	struct node *list=NULL;
	list_appendsort(3,&list);
	list_appendsort(2,&list);
	list_appendsort(5,&list);
	list_appendsort(4,&list);
	list_print(list);
	list_clear(list);
	char a[5]="     ";
	char b[3]="a b";
	strlcp(a,b,3);
	char *c;
	c=strdup_letter(a);
	printf("%s\n",a);
	printf("%s",c);
	printf("%s",b);
	
}*/
