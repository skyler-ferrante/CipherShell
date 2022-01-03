/**
 * Get all lines from a file
**/

#ifndef CONFIG_H
#define CONFIG_H

#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#define MAX_LINE_SIZE 128

struct node{
	char* line;
	struct node* next;
};

struct node* new_node();

void add_node(struct node* tail, char* line);

struct node* get_lines(char* filename);

void free_nodes(struct node* head);

void print_lines(struct node* ll);

#endif
