#include "sort.h"
#include "rm.h"


int Partition(int frameId,int left,int right,int pivot_index)
{
   
   	int pivot_value = 
	int temp;
	{	
		temp=array[right];
		array[right]=array[pivot_index];
		array[pivot_index]=temp;
	}
	int store_index=left;

    for(int i=left;i<right;i++)
	{
		if(array[i]<pivot_value)
		{
			temp=array[store_index];
			array[store_index]=array[i];
			array[i]=temp;
            store_index+=1;
        }
      
	}
	
	temp=array[store_index];
	array[store_index]=array[right];
	array[right]=temp;

	return store_index;
}

template <class T>
void QuickSort(T* array,int left,int right)
{
	if(right>left)
	{
		srand(time(NULL));
        int pivot_index=1+left+rand()%(right-left);
		int newpivot_index=Partition(array,left,right,pivot_index);
		QuickSort(array, left, newpivot_index-1);
		QuickSort(array, newpivot_index+1, right);
	}
}


void Sort::sortBlock(int frameId){





}