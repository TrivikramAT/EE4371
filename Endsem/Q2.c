#include <stdio.h>
#include <stdlib.h>

#define N 500 // the given order of matrix

typedef struct element
{
	int i,j;
	double val;
	struct element *next;
}node;

node *head = NULL; // head of linked list
node *current = NULL;
float X[N];
float Y1[N]; // Y1 = A*X
float Y2[N]; // Y2 = XT*A

void add_element(int i, int j, float val)
{
	node *temp = (node *)malloc(sizeof(node));
	temp->i = i;
	temp->j = j;
	temp->val = val;
	temp->next = NULL;

	if(head == NULL)
	{
		head = temp;
		current = temp;
	}
	else
	{
		current->next = temp;
		current = temp;
	}

}

int main(int argc, char **argv)
{
	int i,j,count=0;
	float val;
	FILE *fp = fopen(argv[1],"r");
	char s[100];

	while(fgets(s,100,fp))
	{
		if(s[0] == '#')
			continue;
		j = -1;
		sscanf(s, "%d %d %f",&i,&j,&val);
		if(j != -1) // line being read is matrix data
		{
			add_element(i,j,val);
		}
		else // line being read is vector data
		{
			sscanf(s, "%f", &X[count++]);
		}
	}

	// multiplying
	current = head;
	while(current != NULL)
	{
		// A*X
		Y1[current->i] += current->val*X[current->j];
		// XT*A
		Y2[current->j] += current->val*X[current->i];
		current = current->next;
	}

	fclose(fp);

	fp = fopen("output2.dat","w");

	fprintf(fp, "Vector A*X =>\n");
	for(i=0; i<N; i++)
		fprintf(fp, "%f\n", Y1[i]);
	fprintf(fp,"\n");
	fprintf(fp, "Vector XT*A =>\n");
	for(i=0; i<N; i++)
		fprintf(fp, "%f\n", Y2[i]);

	fclose(fp);

}