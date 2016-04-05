#include <stdio.h>

void heapAjust(int array[],int i,int length)
{
    int parent = i;
    int child;
    int temp; 
    child = 2 * i + 1;
    while(child < length - 1)
    {
        if(array[child] < array[child + 1])
        {
            child ++;
        }
        if(array[child] > array[parent])
        {
            temp = array[parent];
            array[parent] = array[child];
            array[child] = temp;
        }
        else
        {
            break;
        }
        parent = child;
        child = 2 * parent + 1;
    }
}

void heapSort(int array[],int length)
{
    int i;
    int temp;
    int n = length;
    i = (length - 2) / 2;
    while(i >= 0)
    {
        heapAjust(array,i,length);
        i --;
    }
    while(n > 1) 
    {
        temp = array[0]; 
        array[0] = array[n - 1]; 
        array[n - 1] = temp;
        heapAjust(array,0,n-1);
        n --;
    }
}

int main()
{
    int i;
    int array[12] = {12,24,48,22,5,0,7,99,100,53,67,88};
    heapSort(array,12); 
    printf(" \n");
    for(i = 0;i < 12;i ++)
    {
        printf("%d ",array[i]);
    }
    printf(" \n");
    return 0;
}
