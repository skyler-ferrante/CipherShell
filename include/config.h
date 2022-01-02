/**
 * Get all lines from a file
**/

#ifndef CONFIG_H
#define CONFIG_H

#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define MAX_LINE_SIZE 128

struct node{
	char* line;
	struct node* next;
};

struct node* new_node();

void add_node(struct node* tail, char* line);

struct node* get_lines(char* filename);

void print_lines(struct node* ll);

#endif
