#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define max(a,b) (a > b ? a : b)

int cost[400][2000] = {0};
int items[400][2000] = {0};
int greedy_array[2000];
int num_elements;

// Comparison function for the quick sort function
int compare_function(const void *a, const void *b){
    return *(int*)b - *(int*)a;
}

int greedy_knapsack(int W, int n, int weights[])
{
    int i = 0;
    int sum = 0;

    // Sort the array of weights using the inbuilt qsort function
    qsort(weights,n,sizeof(int),compare_function);
    num_elements = 0;
    // iterate over all the elements
    while(i < n){
        // add that element to the knapsack
        sum += weights[i];
        num_elements++; // increase the number of elements
        // if that weight satisfies the constraint, add in the knapsack
        if((sum <= (float)W - log(num_elements))){
            greedy_array[num_elements-1] = weights[i];
        }
        // else, remove the element from knapsack and decrease the elements
        else{
            sum -= weights[i];
            num_elements--;
        }
        i++;
    }
    // return the sum
    return sum;
}

int dp_knapsack(int index, float size, int count, int weights[]){
    int take,dontTake;
    // the updated size  
    float size2 = size - log(count);
    take = dontTake = 0;

    // if value is precalculated
    if (cost[index][(int)size2]!=0)
        return cost[index][(int)size2];

    // once last element is reached, if weight constraint is not breached then include it
    if (index==0){
        if (weights[0]<=size2){
            items[index][(int)size2] = 1;
            cost[index][(int)size2] = weights[0];
            return weights[0];
        }
        else{
            items[index][(int)size2]=-1;
            cost[index][(int)size2] = 0;
            return 0;
        }
    }
    // if element under consideration is less than remaining weight
    if (weights[index]<=size2)
        take = weights[index] + dp_knapsack(index-1, size-weights[index], count+1, weights);

    dontTake = dp_knapsack(index-1, size, count, weights);
    // compare cost if element is taken vs not taken
    cost[index][(int)size2] = max (take, dontTake);
    // accordingly update matrix of taken/not taken
    if (take>dontTake)
    {
        items[index][(int)size2]=1;
    }
    else
        items[index][(int)size2]=-1;

    return cost[index][(int)size2];

}

void printItems(int item, float size, int count, int weights[]){

    float size2;
    while(item>=0)
    {
        size2 = size - log(count);
        // if item is included update item and weight remaining, else only item
        if(items[item][(int)size2]==1)
        {
            printf("%d ",weights[item]);
            size -= weights[item];
            item--;
            count++;
        }
        else
            item--;
    }

    printf("\n");

return;
}

int main(int argc, char **argv){
    int nItems = 0;
    float knapsackSize = 0;
    int weights[1000];
    int max_value;
    int i,j=0,acc=0;
    char str[2000];
    FILE *fp = fopen(argv[1],"r");
    if(fp == NULL)
    {
        printf("File not opened \n");
        exit(0);
    }
    // only reads the first set of knapsack size and weights in the input file
    while(fgets(str,2000,fp))
    {
        if(str[0] == '#')
            continue;
        else if(knapsackSize == 0)
            knapsackSize = atoi(str);
        else
        {
            for(i=0; str[i]!='\0'; i++)
            {
                if(str[i]>=48 && str[i]<=57) // is a number
                    acc = 10*acc + str[i]-48;
                else if(acc!=0 && (str[i]==' ' || str[i]=='\n'))
                {
                    weights[j] = acc;
                    acc = 0;
                    j++;
                }
            }
        }
    }
    nItems = j;
    
    max_value = dp_knapsack(nItems-1,knapsackSize,1,weights);
    printf("DP approach elements chosen:\n");
    printItems(nItems-1,knapsackSize, 1, weights);

    printf("Greedy algo: max = %d\nWeights chosen: ",greedy_knapsack(knapsackSize,nItems,weights));
    for(i=0; i<num_elements; i++)
        printf("%d ",greedy_array[i]);
    printf("\n");
    fclose(fp);

    // writing results
    fp = fopen("output1.dat","a");
    fprintf(fp, "Dynamic programming cost table: \n");
    for(i=0; i<nItems; i++)
    {
        for(j=0; j<=knapsackSize; j++)
        {
            fprintf(fp,"%d ", cost[i][j]);
        }
        fprintf(fp,"\n");
    }
    fprintf(fp,"\n");
    fprintf(fp, "Dynamic programming element selection table: \n");
    for(i=0; i<nItems; i++)
    {
        for(j=0; j<=knapsackSize; j++)
        {
            fprintf(fp,"%d ", items[i][j]);
        }
        fprintf(fp,"\n");
    }
    fclose(fp);
    return 0;
}