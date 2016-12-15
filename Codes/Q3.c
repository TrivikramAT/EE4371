
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct person 
{
	int index;
	char name[20];
	int death_age;
	int parent_age;
	struct person *next; // next person in adjacency list
	struct person *child; // pointer to list of children
	struct person *child_next; // pointer to next child
}person;

person *start = NULL; // pointer to start of adjacency list
 // to store number of descendents and generations after them data
int desc_table[50][2] = {[0 ... 49][0 ... 1] = -1};

// helper function to search for the address of node with given name
person* search(person *node, char name[])
{
	while(node!=NULL)
	{
		if(strcmp(node->name, name)==0)
			return node;
		node = node->next;
	}
	printf("Person not found\n");
	return NULL;
}

// function to add a new person to the list 
person* add_to_list(person *node, int index, char n1[], char n2[])
{
	person *temp = (person *)malloc(sizeof(person));
	strcpy(temp->name, n1);
	temp->index = index;
	if(n2[0] == '-') // not dead yet
		temp->death_age = -1;
	else
		temp->death_age = atoi(n2);
	temp->parent_age = 0;
	temp->next = NULL;
	temp->child = NULL;
	temp->child_next = NULL;

	if(node == NULL)
		return temp;
	else
	{
		node->next = temp;
		node->child = NULL;
		node->child_next = NULL;
	}
	return NULL;
}

// function to add a child connection in the already built list
// helper function searches for the node's child in the list
void add_child(person *node, char n2[], int age)
{
	person *temp = search(node, n2);
	temp->parent_age = age;

	// if it is this node's first child
	if(node->child == NULL)
		node->child = temp;
	// children already exist for this node
	else
	{
		node = node->child;
		while(node->child_next != NULL)
			node = node->child_next;
		node->child_next = temp;
	}
}

int get_descendents(person *node)
{
	person *temp, *temp2;
	int sum = 0, max = 0, min = 1000;

	// if the descendents array already has a value calculated for this node
	if(desc_table[node->index][0] != -1)
		return desc_table[node->index][0];
	// no child = no descendent. Base case for recursive call
	else if(node->child == NULL)
	{
		desc_table[node->index][1] = 0;
		return 0;
	}
	else
	{
		temp = node->child;
		// loop through all the children and get total number of descendents
		while(temp != NULL)
		{
			sum += get_descendents(temp)+1; // node's descendents plus itself
			if(desc_table[temp->index][1] > max)
				max = desc_table[temp->index][1];
			temp = temp->child_next;
		}
		desc_table[node->index][0] = sum;
		desc_table[node->index][1] = max + 1;
	}
	return sum;
}

int check_ggf(person *node, int count)
{
	int min = 1000; //some arbitrarily large number
	int total_age;

	// great grand child generation reached
	if(count == 3)
		return node->parent_age;

	person *temp = node->child;
	while(temp != NULL)
	{
		// check if this node has enough generations after it to produce a great grand child
		if(desc_table[temp->index][1] >= 3-count-1) 
		{	
			total_age = check_ggf(temp, count+1);	
		}
		else 
			total_age = 1000; // some arbitrarily number such that this child is ignored

		// required - minimum number of years after which great grand child was born
		if(min > total_age)
			min = total_age;
		temp = temp->child_next;
	}
	return (min + node->parent_age);
}

void main(int argc, char **argv)
{
	FILE *fp = fopen(argv[1],"r");
	char s[100], n1[20], n2[20];
	int age = -1, index = 0; 
	int ggf_res;
	person *temp, *temp_desc; // beginning of the list

	// building the adjacency list
	while(fgets(s,100,fp))
	{
		if(s[0] == '#')
		{	
			continue;
		}
		else
		{
			sscanf(s, "%s %s %d", n1, n2, &age);
			if(!(isalpha(n2[0]))) // only one name is present, person data
			{
				if(start == NULL)
				{
					start = add_to_list(start, index, n1, n2);
					index++;
				}
				// names present
				else
				{
					temp = start;
					while(temp->next != NULL)
						temp = temp->next;
					add_to_list(temp, index, n1, n2);
					index++;
				}			
			}
			else // two names present, children data
			{
				temp = start;
				while(temp != NULL)
				{
					if(strcmp(temp->name, n1) == 0)
					{	
						add_child(temp, n2, age);
						break;
					}
					else
						temp = temp->next;
				}
			}
		}
	}	

	// printing out adjacency list
	temp = start;
	while(temp != NULL)
	{
		temp_desc = temp->child;
		while(temp_desc != NULL)
		{
			temp_desc = temp_desc->child_next;
		}
		temp = temp->next;
	}

	// finding number of descendents
	temp = start;
	printf("Descendents data - name = number of descendents\n");
	while(temp != NULL)
	{
		printf("%s = %d\n",temp->name, get_descendents(temp));
		temp = temp->next;
	}
	printf("\n");

	// find who lived long enough to see great grand children
	temp = start;
	printf("People who lived to see their great grand children\n");
	while(temp !=NULL)
	{
		if(desc_table[temp->index][1] < 3)
			temp = temp->next;
		else
		{
			ggf_res = check_ggf(temp, 0);
			if((temp->parent_age == -1) && (temp->death_age >= ggf_res+1)) // special case of first ancestor
				printf("%s\n",temp->name);
			else if(temp->death_age >= ggf_res-temp->parent_age)
				printf("%s\n",temp->name);
			temp = temp->next;
		}
	}
}