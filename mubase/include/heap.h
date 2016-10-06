#ifndef HEAP_H
#define HEAP_H


#include<iostream>
#include<cstdlib>
using namespace std;

template <class T>
class Heap{
	
	int size;
	int current;
	T** array;
	bool order;

	public:
	
	Heap(int hsize, bool order){
		this->size=hsize;
		this->array=new T*[hsize];
		this->current=-1;
		this->order=order;
	}
	
	void Insert(T*);
	T* Extract();
	void Heapify(int index);
	int Status();
};

template <class T>
void Heap<T>::Insert(T* e)
{
	if(current==size-1)
	return;
	T* temp1=e;
	current++;
	array[current]=temp1;
	int temp=current;
	
    while(temp>0)
	{
		int i;
		temp%2==0 ? i=1+temp/2 : i=temp/2;
		if( order ? (*array[i]>(*array[temp])) : (*array[i]<(*array[temp])) ){
			T* t;
			t=array[i];
			array[i]=array[temp];
			array[temp]=t;
			Heapify(i);
			temp=i;
		}
		else
			return;
	}
	return;
}

template <class T>
void Heap<T>::Heapify(int index)
{
	T* temp;
    if(index < 0 || index > current )
	return;
	
	int index_left=2*index+1;
	int index_right=index_left+1;
	if(current==index || current < index_left)
	return;
	
	else
	if(current>=index_right)
	{
		
		int i;
		if(order)
			*array[index_left] < *array[index_right] ? i=index_left : i=index_right;
		else
			*array[index_left] > *array[index_right] ? i=index_left : i=index_right;
		
		if( order ? (*array[index]>(*array[i])) : (*array[index]<(*array[i]))){
			temp=array[index];
			array[index]=array[i];
			array[i]=temp;
			Heapify(i);
		}
	}
	
	else
	if(current==index_left)
	{
		if(order ? (*array[index]>(*array[index_left])) : (*array[index]<(*array[index_left])))
		{
			temp=array[index];
			array[index]=array[index_left];
			array[index_left]=temp;
		}
	}
}

template <class T>
T* Heap<T>::Extract(){
    T* value;
    if(current==-1)
        return NULL;
	else{
		
		value=array[0];
		array[0]=array[current];
		current--;
		Heapify(0);
	}	
	return value;
 }

template <class T>
int Heap<T>::Status(){
	return current+1;
}


#endif	//HEAP_H

