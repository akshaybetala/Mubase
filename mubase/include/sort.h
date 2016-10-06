#ifndef SORT_H
#define SORT_H

#include "rm.h"
#include "bm.h"
#include "sp.h"
#include "heap.h"
#include <string>
using namespace std;


class Sort : public RIterator {
private:
	class RecElement;
	char* dbName;
	char* relName;
	Schema* schema;
	Heap<RecElement> *heap;
	RIterator** Iterator;
	int SizeOfIterator;
	BufferManager *bm;
	string* runName;

	/*
	*	Attributes on which soting is to be done.
	*/
	int attrIndex;
	/*
	*	true for sorting in ascending order.
	*	false for sorting in desscending order.
	*/
	bool sortOrder;
	
	/*
	*	Sort block in ascending order if sortOrder is true else in descending order.
	*/
	void sortBlock(char* blockPtr);
	/*
	*	if sortOrder is true
	* 		Return true id rec1 is greater or equal to rec2 else return false.
	*	if sortOrder is false
	* 		Return true id rec2 is greater or equal to rec1 else return false.
	*/
	bool recCompare(char* rec1, char* rec2);	
	int Partition(SlottedPage* sp,int left,int right,int pivot_index);
	void QuickSort(SlottedPage* sp,int left,int right);
	
	/*
	*	Iterator for sorted block
	*/
	class RecElement{
		Record *rec;
		Sort* parent;
		int Index;
	public:
		// code cpp file mein likhna
		RecElement(){;}
		RecElement(Record *rec,int Index,Sort* parent){
			this->rec=rec;
			this->Index=Index;
			this->parent=parent;
		}
		bool operator<(const RecElement&);
		bool operator>(const RecElement&);
		bool operator==(const RecElement&);
		Record* getRecord();
		int getIndex();
	};
	
public:
	Sort(char* dbName,char* relName, int attrIndex,bool order);
	virtual void open();
    virtual Record* next();
    virtual void close();
};

#endif	//SORT_H

