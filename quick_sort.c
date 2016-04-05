#include <stdio.h>

void Qsort(int a[],int low,int high){
    if(low >= high){
        return;
    }
    int first = low;
    int last = high;
    int key = a[first];
    while(first < last){
        while(first < last && a[last] >= key){
            last--;
        }
        a[first] = a[last];
        while(first < last && a[first] <= key){
            first++;
        }
        a[last] = a[first];
    }
    a[first] = key;
    Qsort(a,low,first-1);
    Qsort(a,first+1,high);
}

int main(void){
    int a[] = {2,6,9,7,20,44,33,20,2,88,99,56,43,7,0,69,100,47,55,83};
    Qsort(a,0,sizeof(a)/sizeof(int));
    int i;
    printf("a[%d]:\n",sizeof(a)/sizeof(int));
    for(i = 0;i < sizeof(a)/sizeof(int);i++){
        printf("%d ",a[i]);
    }
    printf("\nendsort!\n");
    return 0;
}
