#include <stdio.h>

void swap(int arr[], int index1, int index2) {
    int tmp = arr[index1];
	arr[index1] = arr[index2];
	arr[index2] = tmp;
}

void heapAjust(int arr[], int index, int size) {
	int i = index;
    int max_child_index;
    for(; i <= size/2-1; i = max_child_index) {
	    if(2*i+2 > size-1) {
		    //only left child.
			if(arr[i] < arr[2*i+1]) {
			    swap(arr, i, 2*i+1);
			}
			break;
		}
		max_child_index = arr[2*i+1] > arr[2*i+2] ? 2*i+1 : 2*i+2;
		if(arr[i] < arr[max_child_index]) {
		    swap(arr, i, max_child_index);
		}
	} 
}

void heapSort(int arr[], int size) {
    int index = size/2-1;
   	for(; index >= 0; index--) {
	    heapAjust(arr, index, size);
		//debug
		int i = 0;
		printf("arr:");
		for(; i < size; i++) {
			printf(" %d ", arr[i]);
		}
		printf("\r\n");
	}
	printf("first traverse\n");
	for(index = size-1; index >= 1; index--) {
        swap(arr, 0, index);
		if(index == 1) {
		    break;
		}
		heapAjust(arr, 0, index);
		//debug
		int i = 0;
		printf("index:%d, after heapAjust arr:", index);
		for(; i < size; i++) {
			printf(" %d ", arr[i]);
		}
		printf("\r\n");
	}
}

int main() {
    int arr[] = {88,20,1,2,4,79,19,45,97,21,87,49,56,33,90,77};
	//debug
	int i = 0;
	printf("original arr:");
	for(; i < sizeof(arr)/sizeof(int); i++) {
		printf(" %d ", arr[i]);
	}
	printf("\r\n");

	heapSort(arr, sizeof(arr)/sizeof(int));
	
	//debug
	printf("result arr:");
	for(i = 0; i < sizeof(arr)/sizeof(int); i++) {
		printf(" %d ", arr[i]);
	}
	printf("\r\n");
	return 0;
}


