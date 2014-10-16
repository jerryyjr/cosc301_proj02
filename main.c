#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <poll.h>
#include <signal.h>
#include <errno.h>

struct node{
	char num[128];
	struct node *next; 
};

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
	(new->num)[strlen(num)-1]='\0';
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

void remove_num_sign(char *buffer){
	for(int i =0;i<strlen(buffer);i++){
		if (buffer[i]=='#'){
			buffer[i]='\0';
			return;
		}	
	}
}

char** tokenify(const char *s) {

	
	char *str;
	str=strdup(s);
	char *t="  ";
	int index=0;
	char **array = malloc(strlen(str)*sizeof(t));
	
	t=strtok(str," \t\n");
	if (t!=NULL){
		array[0]=strdup(t);
		
	}
	for (int i=1;t!=NULL;i++){
		t=strtok(NULL," \t\n");	
		if (t!=NULL){	
		array[i]=strdup(t);
		
		}
		index=i;
    }
	array[index]=NULL;
	free(t);
	free(str);
	return array;

}

char** separate(const char *s) {

	
	char *str;
	str=strdup(s);
	char *t="  ";
	int index=0;
	char **array = malloc(strlen(str)*sizeof(t));
	
	t=strtok(str,";");
	if (t!=NULL){
		array[0]=strdup(t);
	}
	for (int i=1;t!=NULL;i++){
		t=strtok(NULL,";");	
		if (t!=NULL){	
		array[i]=strdup(t);
		}
		index=i;
    }
	array[index]=NULL;
	free(t);
	free(str);
	return array;

}

int change_mode(char *token, int *mode){
	
	if(strcmp(token,"s")==0 || strcmp(token,"sequential")==0){
		return 0;
	}else if (strcmp(token,"p")==0||strcmp(token,"parallel")==0){
		return 1;
	}else{
		printf("invalid mode\n");
		return *mode;
	}
}

void print_mode(int *mode){
	

	if (*mode==0){
		printf("sequential\n");
	}else{
		printf("parallel\n");
	}

	
}

int count_com(char **commands){
	
	int i=0;
	
	while(commands[i]!=NULL){
		i++;
	}
	
	return i;
}

struct node *process_conf(FILE *input_file) {
   
	char input[100];
	
	struct node *head=NULL;//malloc(sizeof(struct node));
	
	while(fgets(input,100,input_file)!=NULL){
		
		
		list_append(input,&head);
	}
	
	
	return head;
	
}

char * check(struct node **head,char *token){
	struct node *tmp=*head;
	while(tmp!=NULL){
		char command[128];
		char *com=command;
		strcpy(command,tmp->num);
		//command[strlen(command)]="/";
		strcat(command,"/");
		strcat(command,token);	
		
		
		struct stat fileStat;
   		if(stat(command,&fileStat) == 0){
			//printf("%s",command);
			return com;
		}
		tmp=tmp->next;
	}
	return NULL;
}

int sequential(char *buffer, int *mode){
	int eXit=0;             // indicate exit or not
	int m=*mode;
	int childrv;
	int i=0;
	char *chec=NULL;
	FILE *datafile = NULL;
	datafile = fopen("shell-config", "r");
    if (datafile == NULL) {
    	printf("Unable to open file shell-config: %s\n", strerror(errno));
    	exit(-1);
    }
	struct node *head=NULL;
	head=process_conf(datafile);
	
	char **commands=separate(buffer);
	while(commands[i]!=NULL){
		char **tokens=tokenify(commands[i]);
		

		if(tokens[0]!=NULL){
			if(strcmp(tokens[0],"mode")==0){
				if(tokens[1]!=NULL){
					m=change_mode(tokens[1],mode);
				}else{
					print_mode(mode);
				}
			}else if (strcmp(tokens[0],"exit")==0){				
				eXit=1;
			}else{
				chec=check(&head,tokens[0]);
				if(chec!=NULL){
					pid_t pid=fork();
					if (pid==0){
									
						if(execv(chec, tokens) < 0) {
	  	    	     		fprintf(stderr, "execv failed: %s\n", strerror(errno));
	  	  		    	}
					}else{
						wait(&childrv);
					}
				}else{
					struct stat fileStat;
   					if(stat(tokens[0],&fileStat) == 0){
						pid_t pid=fork();
						if (pid==0){
							if(execv(tokens[0], tokens) < 0) {
	  	    	     			fprintf(stderr, "execv failed: %s\n", strerror(errno));
	  	  		    		}
						}else{
							wait(&childrv);
						}
						
					}else{
						printf("Invalid command: %s\n",tokens[0]);
					}

				}
			}
		}
			
		free(tokens);
		i++;
	}
	free(commands);
	*mode=m;
	list_clear(head);
	return eXit;
	
}


int parallel(char *buffer, int *mode){
	int eXit=0;             // indicate exit or not
	int m=*mode;
	int childrv;
	int i=0;
	int count=0;
	pid_t pid;
	char *chec=NULL;
	FILE *datafile = NULL;
	datafile = fopen("shell-config", "r");
    if (datafile == NULL) {
    	printf("Unable to open file shell-config: %s\n", strerror(errno));
    	exit(-1);
    }
	struct node *head=NULL;
	head=process_conf(datafile);

	char **commands=separate(buffer);
	count=count_com(commands);
	while(commands[i]!=NULL){
		
		char **tokens=tokenify(commands[i]);
		
		
		if(tokens[0]!=NULL){
			if(strcmp(tokens[0],"mode")==0){
				if(tokens[1]!=NULL){
					m=change_mode(tokens[1],mode);
				}else{
					print_mode(mode);
				}
				count--;
			}else if (strcmp(tokens[0],"exit")==0){				
				eXit=1;
				count--;
			}else{
				chec=check(&head,tokens[0]);
				if(chec!=NULL){
					pid=fork();	
					if (pid==0){					
						if(execv(chec, tokens) < 0) {
	    	   		  		fprintf(stderr, "execv failed: %s\n", strerror(errno));
	  		   		 	}
					}
				
				}else{
					struct stat fileStat;
   					if(stat(tokens[0],&fileStat) == 0){
						pid_t pid=fork();
						if (pid==0){
							if(execv(tokens[0], tokens) < 0) {
	  	    	     			fprintf(stderr, "execv failed: %s\n", strerror(errno));
	  	  		    		}
						}
						
					}else{
						printf("Invalid command: %s\n",tokens[0]);
					}
				}
			}			
			free(tokens);
		}		
		i++;
		
	}
	for(int i=0; i<count;i++){
		waitpid(-1,NULL,0);
	}
	free(commands);
	*mode=m;
	list_clear(head);
	return eXit;
	
}

int main(int argc, char **argv) {
	int eXit=0;
	int mode=0;        //default mode: sequential
    while(1){
		int eof=0;
		
		int *p=NULL;
		p=&mode;
		char prompt[]="prompt> ";
		printf("%s", prompt);
		fflush(stdout);  

		char buffer[1024];
		while (fgets(buffer, 1024, stdin) != NULL) {
    		int len=strlen(buffer);
			eof=1;
			buffer[len-1]='\0';
			break;
		}
		
		remove_num_sign(buffer);
		if(strlen(buffer)==0){
			
			strcpy(buffer,"  ");
		}

		if (mode==0){
			eXit=sequential(buffer,p);
		}else{
			eXit=parallel(buffer,p);
		}
		


		if(eof==0){
			printf("\n");
		}	
		
		if(eXit==1){exit(0);}
	}

    return 0;
}

