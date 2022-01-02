#include "config.h"

struct node* new_node(){
	return (struct node*) malloc(sizeof(struct node));
}

void strip_newline(char* string){
        // Remove trailing newline
        // https://stackoverflow.com/questions/2693776/removing-trailing-newline-character-from-fgets-input
        string[strcspn(string, "\n")] = 0;
}

void add_node(struct node* tail, char* line){
	tail->line = strdup(line);
	strip_newline(tail->line);
	tail->next = NULL;
}

struct node* get_lines(char* filename){
	FILE* file;

	//If given file is readable
	if( (file = fopen(filename, "r")) ){
		char line[MAX_LINE_SIZE];

		struct node* head = new_node();
		struct node* tail = head;

		if( !fgets(line, MAX_LINE_SIZE, file) ){
			fprintf(stderr, "Cannot read first line from %s\n", filename);
			exit(-1);
		}

		add_node(tail, line);
	
		//While able to read line
		while( fgets(line, MAX_LINE_SIZE, file) ){
			tail->next = new_node();
			tail = tail->next;

			add_node(tail, line);
		}

		return head;
	}else{
		fprintf(stderr, "Can't read file\n");
		fprintf(stderr, "Exiting\n");
		exit(-1);
	}	
}

void print_lines(struct node* list){
	for(struct node* curr = list; curr; curr = curr->next){
		printf("%s\n", curr->line);
	}
}
