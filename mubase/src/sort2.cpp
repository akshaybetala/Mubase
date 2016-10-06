#include "sort.h"
#include "bm.h"

Sort::Sort(const char* dbName,const char* relName, int attrIndex,bool order){
	this->dbName=dbName;
	this->relName=relName;
	Relation relation(dbName,relName,false,NULL);
	this->schema=relation.getSchema();		
	this->attrIndex=attrIndex;
	this->sortOrder=order;
	this->heap=NULL;
	this->Iterator=NULL;
	this->bm=NULL;
	this->SizeOfIterator = 0;
}

void Sort::open(){
	bm = BufferManager::getInstance(this->dbName);
	RelBlockIterator relBlockIterator(this->dbName,this->relName);
	relBlockIterator.open();
	int CurrentRunNo=0;
	int TotalRun=0;
	bool rel=true;
	while(rel){
		int FreeFrames=bm->getTotFreeFrame()-1;
		cout<<FreeFrames<<endl;
		SizeOfIterator = FreeFrames;
		char* blockPtr[FreeFrames];
		Iterator = new RIterator*[FreeFrames];
		heap=new Heap<RecElement>(FreeFrames,this->sortOrder);
		for(int i =0;i < FreeFrames;i++){
			
			int blockId =relBlockIterator.next();
			cout<<blockId<<endl;
			if(blockId <= 0 ){
				relBlockIterator.close();
				rel=false;
				SizeOfIterator = i;
				cout<<"toatl run :" << TotalRun<<endl;
				if(TotalRun == 0 )
				 return;
				break;
			}
			blockPtr[i] = bm->getTempFrame( bm->pinPage(blockId));
			bm->unpinPage(blockId,false);
			sortBlock(blockPtr[i]);
			Iterator[i] = (RIterator*)new BlockRecIterator(blockPtr[i],this->schema);
			Iterator[i]->open();
			Record* record = Iterator[i]->next();
			if(record!=NULL){
				cout<<"rocrd eneterd in heap"<<endl;
				RecElement* recElement = new RecElement(record,i,this);
				heap->Insert(recElement);
			}
			
		}
		
		CurrentRunNo++;
		TotalRun++;
		string runName= "run1";
		Relation run(dbName,&runName[0],true,schema);
		while(heap->Status() > 0)
		{
			RecElement* recElement = heap->Extract();
			Record* nextRecord=recElement->getRecord();
			run.addRecord(*nextRecord);
			delete nextRecord ;
			int Index = recElement->getIndex();
			Record *record = Iterator[Index]->next();
			if(record!=NULL){
				recElement = new RecElement(record,Index,this);
				heap->Insert(recElement);
			}
			else{
				bm->releaseTempFrame(blockPtr[Index]);
				Iterator[Index]->close();
				delete Iterator[Index];
			}
			
		}
		delete heap;
		delete Iterator ;
	}

	while(true){
		int FreeFrames = bm->getTotFreeFrame()-1;
		heap=new Heap<RecElement>(FreeFrames,this->sortOrder);
		runName = new string[FreeFrames];
		Iterator = new  RIterator*[FreeFrames];
		for(int i =0; i < TotalRun && i < FreeFrames  ; i++){
		
			runName[i] = "run" + (char)(CurrentRunNo-TotalRun+i+1);
			Iterator[i] = new BaseRelIterator(dbName,&runName[i][0]);
			Iterator[i]->open();
			Record* record = Iterator[i]->next();
			if(record!=NULL){
				RecElement recElement(record,i,this);
				heap->Insert(&recElement);
			}
			
		}
		if(TotalRun <= FreeFrames )
			return;
		TotalRun = TotalRun-FreeFrames+1;
		CurrentRunNo++;
		string objname= "run" + (char)(CurrentRunNo);
		Relation curRun(dbName,&objname[0],false,schema);
		while(heap->Status() > 0)
		{
			RecElement* recElement = heap->Extract();
			Record* record=recElement->getRecord();
			curRun.addRecord(*record);
			delete record ;
			int Index = recElement->getIndex();
			delete recElement;
			record = Iterator[Index]->next();
			if(record!=NULL){
				recElement = new  RecElement(record,Index,this);
				heap->Insert(recElement);
			}
			else{
				Iterator[Index]->close();
				delete Iterator[Index];
				Relation run(dbName,&runName[Index][0],false,schema);
				run.deleteRelation();
			}
			
		}
		
		delete heap;
		delete Iterator;
		delete runName;
	}

}

Record* Sort::next(){
	if(heap->Status() > 0){
		RecElement* recElement = heap->Extract();
		Record* resultRec=recElement->getRecord();
		int Index = recElement->getIndex();
		delete recElement;
		Record* record = Iterator[Index]->next();
		if(record!=NULL){
			recElement = new RecElement(record,Index,this);
			heap->Insert(recElement);
		}
		else{
			Iterator[Index]->close();
			delete Iterator[Index];
			//cout<<runName[Index];
			//Relation run(dbName,&runName[Index][0],false,schema);
			//run.deleteRelation();
		}
		return resultRec;	
	}
	return NULL;
}

void Sort::close(){
        dbName=  NULL;
	schema=NULL;
	relName=NULL;
	delete runName;
	delete heap;
	delete Iterator;
	bm = NULL;
	
}


int Sort::Partition(SlottedPage* sp,int left,int right,int pivot_index){
	char* pivot_value = sp->getRecord(pivot_index);
	sp->swapRecord(pivot_index,right);
	int store_index=left;
	
	for(int i=left;i<right;i++){
	
		char* temp=sp->getRecord(i);
		if( recCompare(temp, pivot_value) ){
			sp->swapRecord(store_index,i);
            store_index+=1;
        }
      
	}
	sp->swapRecord(right,store_index);
	return store_index;

}

void Sort::QuickSort(SlottedPage* sp,int left,int right){
	if(right>left){
		srand(time(NULL));
        int pivot_index=1+left+rand()%(right-left);
		int newpivot_index=Partition(sp,left,right,pivot_index);
		QuickSort(sp, left, newpivot_index-1);
		QuickSort(sp,newpivot_index+1, right);
	}
}

void Sort::sortBlock(char* blockPtr){
	
	SlottedPage *sp = new SlottedPage(blockPtr);
	int n=sp->getTotNoSlots();
	int i=0,j=n-1;
	while(true)
	{
		while(sp->getRecord(i)!=NULL){
			i++;
		}
		while(sp->getRecord(j)==NULL){
			j--;
		}
		if(i>=j)
			break;
		sp->swapRecord(i,j);
	}
	QuickSort(sp,0,i-1);

}

bool Sort::recCompare(char* rec1, char* rec2){
	Record record1(rec1,schema);
	Record record2(rec2,schema);
	
	if(this->sortOrder) return !( *rec1 > *rec2);
	else return !( *rec1 < *rec2 );
}

bool Sort::RecElement::operator<(const RecElement& b){
	return  (*( this->rec->recBytes) <  *(b.rec->recBytes) );
}

bool Sort::RecElement::operator>(const RecElement& b){
	return  (*( this->rec->recBytes) > *(b.rec->recBytes) );
}

bool Sort::RecElement::operator==(const RecElement& b){
	return  (*( this->rec->recBytes) ==  *(b.rec->recBytes) );
}

int Sort::RecElement::getIndex(){
  	return this->Index;
}

Record* Sort::RecElement::getRecord(){
  	return this->rec;
}

