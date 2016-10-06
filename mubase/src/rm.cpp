#include "rm.h"
#include "iter.h"
#include "global.h"
#include "bm.h"
#include "sm.h"
#include "util.h"
#include "sp.h"
#include "bt.h"
#include <cassert>
#include <cstdio>
#include <string>
#include <sstream>
#include <cstring>
#include <iostream>
using namespace std;

/** Schema */
Schema::Schema(const char* dbName, int blockId, short slot){
	BufferManager *bm = BufferManager::getInstance(dbName);
	char* block = bm->pinPage(blockId);
	SlottedPage sp(block);
	char* rec = sp.getRecord(slot);
//	cout<<"reading frm bl"<<blockId<<"slot Id : "<<slot<<endl;
	char* r = rec;
//	cout << "Schema:"<<*r;
	while(*r!=0){
		cout<<(int)(*r);
		r++;
	}
	cout << endl;
	
	int numAttrs = readInt(rec, 0);

//	Schema(rec+SIZE_INT, numAttrs);

	char* schema1 = rec+SIZE_INT;
	this->numAttrs = numAttrs;
	for (int i = 0; i < numAttrs; i++)  {
		memcpy(&(attrType[i]), schema1, SIZE_INT);
		memcpy(&(attrName[i][0]), schema1+SIZE_INT, SIZE_ATTR);
		schema1 += (SIZE_ATTR+SIZE_INT);
//		cout << attrType[i] << "   " << attrName[i][0] << endl;
	}
	free(rec);
	bm->unpinPage(blockId, false);
}
Schema::Schema(char* schema, int numAttrs){
	this->numAttrs = numAttrs;
	for (int i = 0; i < numAttrs; i++){
		memcpy(&(attrType[i]), schema, SIZE_INT);
		memcpy(&(attrName[i][0]), schema+SIZE_INT, SIZE_ATTR);
		schema += (SIZE_ATTR+SIZE_INT);
//		cout << attrType[i] << "   " << attrName[i][0] << endl;
	}
}
Schema::Schema(){
	this->numAttrs = 0;
}
int Schema::getString(char* &schema){
	int size = SIZE_INT + numAttrs*(SIZE_ATTR+SIZE_INT);
	char* s = (char*)malloc(size);
	int offset = 0;
	cout << "creating schema string"<<endl;
	
	cout << "numAttrs "<<numAttrs<<endl;
	memcpy(s+offset,&numAttrs, SIZE_INT);
	offset += SIZE_INT;
	
	for (int i = 0; i < numAttrs; i++){
		cout << attrType[i]<<" " << attrName[i][0]<<endl;
		memcpy(s+offset, &(attrType[i]), SIZE_INT);
		offset += SIZE_INT;
		memcpy(s+offset, &(attrName[i][0]), SIZE_ATTR);
		offset += SIZE_ATTR;
	}
	cout << endl;
	
	offset = 0;
	cout << "Schema is:";
	while(offset > size){
		cout << (int)(*(s+offset)) << " ";
		offset++;
	}
	cout <<"XX"<< endl;
	
	schema = s;	
	return size;
}
bool Schema::addAttribute(int attrType, char* attrName){
	if (numAttrs == MAX_ATTRS)
		return false;
	
	this->attrType[numAttrs] = attrType;
	memcpy(&(this->attrName[numAttrs][0]), attrName, SIZE_ATTR);
	numAttrs++;
	return true;
}
bool Schema::operator==(const Schema &schema){
	if (this->numAttrs == schema.numAttrs){
		for (int i = 0; i < numAttrs; i++){
			if (attrType[i]!=schema.attrType[i] || strcmp(attrName[i], schema.attrName[i])!=0)
				return false;
		}
		return true;
	}
	return false;
}
int Schema::getAttrType(char* attrName){
	int type = -1;
	for (int i = 0; i < numAttrs; i++){
		if (strcmp(attrName, this->attrName[i]) == 0){
			type = this->attrType[i];
			break;
		}
	}
	return type;
}

/** Record */
Record::Record(char *recBytes, Schema *schema, int length){
	assert(recBytes!=NULL);
	assert(schema!= NULL);
	this->recBytes = recBytes;
	this->schema = schema;
	if (length == -1) length=strlen(recBytes);
	this->length = length;
}
char* Record::getAttrStarting(int attrIndex){
	char* attr = this->recBytes;
	for (int i = 0; i < attrIndex; i++){
		if (this->schema->getAttrType(i) == INTEGER){
			attr += SIZE_INTEGER;
		} else if (this->schema->getAttrType(i) == FLOAT){
			attr += SIZE_FLOAT;
		} else if (this->schema->getAttrType(i) == STRING){
			int length = readInt(attr, SIZE_INT);
			attr += (SIZE_INT + length);
		} else {
			assert(false);	// Unsupported dataType found
		}
	}
	return attr;
}
int Record::getInt(int attrIndex){
	assert(this->schema->getAttrType(attrIndex) == INTEGER);
	char* attr = getAttrStarting(attrIndex);
	int i;
	memcpy(&i, attr, SIZE_INTEGER);
	return i;
}
float Record::getFloat(int attrIndex){
	assert(this->schema->getAttrType(attrIndex) == FLOAT);
	char* attr = getAttrStarting(attrIndex);
	float f;
	memcpy(&f, attr, SIZE_FLOAT);
	return f;
}
char* Record::getString(int attrIndex){
	assert(this->schema->getAttrType(attrIndex) == FLOAT);
	char* attr = getAttrStarting(attrIndex);
	int length;
	memcpy(&length, attr, SIZE_INT);
	char* c = (char*)malloc(length+1);
	memcpy(c, attr+SIZE_INT, length);
	c[length] = '\0';
	return c;
}
int Record::compareAttr(Record record, int attrIndex){
	assert(this->schema->getAttrType(attrIndex) == record.schema->getAttrType(attrIndex));
	
	int attrType = this->schema->getAttrType(attrIndex);
	char* attr1 = getAttrStarting(attrIndex);
	char* attr2 = record.getAttrStarting(attrIndex);
	
	if (attrType == INTEGER){
		int i1, i2;
		memcpy(&i1, attr1, SIZE_INTEGER);
		memcpy(&i2, attr2, SIZE_INTEGER);
		if (i1==i2)
			return 0;
		else if (i1<i2)
			return -1;
		else
			return 1;
	} else if (attrType == FLOAT){
		float f1, f2;
		memcpy(&f1, attr1, SIZE_FLOAT);
		memcpy(&f2, attr2, SIZE_FLOAT);
		if (f1==f2)
			return 0;
		else if (f1<f2)
			return -1;
		else
			return 1;
	} else if (attrType == STRING){
		int l1, l2;
		memcpy(&l1, attr1, SIZE_INT);
		memcpy(&l2, attr2, SIZE_INT);
		char* c1 = (char*)malloc(l1+1);
		char* c2 = (char*)malloc(l2+1);
		memcpy(c1, attr1+SIZE_INT, l1);
		memcpy(c2, attr2+SIZE_INT, l2);
		return strcmp(c1, c2);
	} else {
		assert(false);	// Unsupported dataType found
	}
}
bool Record::lessThan(Record record,int attrIndex){
	int cmp = compareAttr(record, attrIndex);
	return cmp<0?true:false;
}
bool Record::greaterThan(Record record,int attrIndex){
	int cmp = compareAttr(record, attrIndex);
	return cmp>0?true:false;
}
bool Record::equalTo(Record record,int attrIndex){
	int cmp = compareAttr(record, attrIndex);
	return cmp==0?true:false;
}

/** Relation */
RecID Relation::writeToObj(BufferManager* bm, int firstBlockId, 
			char* bytes, int length, bool isSysObj, 
			char* sysObjname){
	//~ if (isSysObj)
		//~ cout << sysObjname<<endl;
	int blockId = firstBlockId;
	char* block = bm->pinPage(blockId);
	SlottedPage* sp = new SlottedPage(block);
	short slot = sp->storeRecord(bytes, length);

	while(slot < 0) {
	//~ cout << "here " << blockId << " " << slot<<endl;
		int nextBlockId = readInt(block, 0);
		bm->unpinPage(blockId, false);
		delete sp;
		if (nextBlockId < 0){
			int freeBlockId = StorageManager::allocateBlock(dbName);
			// update last block's nextBlockID
			if (isSysObj)
				StorageManager::addBlock(dbName, sysObjname, freeBlockId);
			else{
				block = bm->pinPage(blockId);
				writeInt(block, 0, freeBlockId);
				bm->unpinPage(blockId, true);
			}
			
			// initialize newFreeBlock
			block = bm->pinPage(freeBlockId);
			writeInt(block, 0, -1);
			sp = new SlottedPage(block);
			sp->initBlock();
			delete sp;
			bm->unpinPage(freeBlockId, true);
			nextBlockId = freeBlockId;
		}
		blockId = nextBlockId;
		block = bm->pinPage(blockId);
		sp = new  SlottedPage(block);
		slot = sp->storeRecord(bytes, length);
	}
	bm->unpinPage(blockId, true);
	delete sp;
	RecID recId;
	recId.blockId = blockId;
	recId.slot = slot;
	return recId;
}
Relation::Relation(const char *dbName, const char *relName, bool create,
					Schema *schema) {
	this->dbName  = dbName;
	this->relName = relName;
	
	BufferManager *bm = BufferManager::getInstance(dbName);
	
	if(create) {
		assert(schema != NULL);
		
		/** allocate block for relation */
		char *block = NULL;
		SlottedPage* sp = NULL;
		this->firstBlockId = StorageManager::allocateBlock(dbName);
		block = bm->pinPage(this->firstBlockId);
		sp = new SlottedPage(block);
		sp->initBlock();
		delete sp;
		bm->unpinPage(this->firstBlockId, true);
		
		/** add schema to SYSCOL */
		char* schemaStr = NULL;
		int schemaLength = schema->getString(schemaStr);
/**		cout << "schema getString has been called. check if it is working properly??"<<endl;
		//////////////////////////////////////////////
		//////////CHECK HERE/////////////////////
		///////////////////////////////////
		cout << "-------------CHECK------------" << endl;
		
		int offset = 0;
		
		char* A_name = (char*)malloc(SIZE_ATTR);
		char* num = (char*)malloc(SIZE_INT);
		memcpy(num,schemaStr,SIZE_INT);
		int numAttrs = (int)*num;
		cout << "numAttrs " << numAttrs << endl;
		offset += SIZE_INT;

		for (int i = 0; i < numAttrs; i++)  {
			memcpy(num,schemaStr+offset, SIZE_INT);
			offset += SIZE_INT;
			memcpy(A_name,schemaStr+offset, SIZE_ATTR);
			offset += SIZE_ATTR;

			cout << (int)*num<<" " << *A_name<<endl;
		}
		cout << endl;
		//////////////////////////////////////////////////////////	
		//////////////////////////////////////////////////////////	
		//////////////////////////////////////////////////////////	
*/		int SYSCOL_firstBlockId = StorageManager::getFirstBlockId(dbName, (const char*)SYSCOL);
		schemaRecId = writeToObj(bm, SYSCOL_firstBlockId, schemaStr, schemaLength, true, (char*)SYSCOL);

		/**
		 * create rel record to insert into slotted page
		 * relation slot entry format in SYSOBJ:
		 * (chars)relation name, (int)relation first block id, (int)col_blockid, (short)col_slot
		 */
		char* rel;
		int relLength = RELNAME_SIZE + SIZE_SHORT + SIZE_INT*2;
		rel = (char*)malloc(relLength);
		memcpy(rel, relName, RELNAME_SIZE);
		memcpy(rel+RELNAME_SIZE, &(this->firstBlockId), SIZE_INT);
		memcpy(rel+RELNAME_SIZE+SIZE_INT, &(schemaRecId.blockId), SIZE_INT);
		memcpy(rel+RELNAME_SIZE+SIZE_INT*2, &(schemaRecId.slot), SIZE_SHORT);

		/** write relation to SYSOBJ */
		int SYSOBJ_firstBlockId = StorageManager::getFirstBlockId(dbName, (const char*)SYSOBJ);
		relRecId = writeToObj(bm, SYSOBJ_firstBlockId,rel, relLength, true, (char*)SYSOBJ);
		free(rel);
	} else {
		int blockId = StorageManager::getFirstBlockId(dbName, SYSOBJ);
		char* block = bm->pinPage(blockId);
		SlottedPage* sp = new SlottedPage(block);
		bool found = false;
		
		while(!found) {
			int slots = sp->getTotNoSlots();
			char* rel;
			for (int i = 0; i < slots; i++){
				rel = sp->getRecord(i);
				if (rel != NULL){
					if (strcmp(relName, rel) == 0) {
						memcpy(&(this->firstBlockId), rel+RELNAME_SIZE, SIZE_INT);
						memcpy(&(schemaRecId.blockId), rel+RELNAME_SIZE+SIZE_INT, SIZE_INT);
						memcpy(&(schemaRecId.slot), rel+RELNAME_SIZE+SIZE_INT*2, SIZE_SHORT);
						relRecId.blockId = blockId;
						relRecId.slot = i;
						free(rel);
						found = true;
						break;
					}
					free(rel);
				}
			}
			int nextBlockId = readInt(block, 0);
			delete sp;
			bm->unpinPage(blockId, false);
			
			if (found) break;
			if (nextBlockId < 0){
				schemaRecId.blockId = schemaRecId.slot = -1;
				relRecId.blockId = relRecId.slot = -1;
				break;
			}
			blockId = nextBlockId;
			block = bm->pinPage(blockId);
			sp = new  SlottedPage(block);
		}
		cout << "opening existing relation: "<<schemaRecId.blockId<< " " << schemaRecId.slot<<endl;
		cout << "opening existing relation: "<<relRecId.blockId<< " " << relRecId.slot<<endl;
	}
	
	this->schema = new Schema(dbName, schemaRecId.blockId, schemaRecId.slot);
	
/**	char* schemaStr = NULL;
	int schemaLength = this->schema->getString(schemaStr);
	cout << "-------------CHECK IN FALSE------------" << endl;
	
	int offset = 0;
	
	char* A_name = (char*)malloc(SIZE_ATTR);
	char* num = (char*)malloc(SIZE_INT);
	memcpy(num,schemaStr,SIZE_INT);
	int numAttrs = (int)*num;
	cout << "numAttrs " << numAttrs << endl;
	offset += SIZE_INT;

	for (int i = 0; i < numAttrs; i++)  {
		memcpy(num,schemaStr+offset, SIZE_INT);
		offset += SIZE_INT;
		memcpy(A_name,schemaStr+offset, SIZE_ATTR);
		offset += SIZE_ATTR;

		cout << (int)*num<<" " << *A_name<<endl;
	}
	cout << endl;
*/	
}
RecID Relation::addRecord(Record rec){
	RecID recId;
	recId.blockId = -1;
	recId.slot = -1;
	// TODO: check schema valodity of relation and record
	//~ if (*schema == *(rec.getSchema())){
		BufferManager* bm = BufferManager::getInstance(dbName);
		recId = writeToObj(bm, this->firstBlockId, rec.getBytes(), 
					rec.getLength(), false, NULL);
	//~ }
	return recId;
}
void Relation::deleteRecord(RecID recId){
	BufferManager *bm = BufferManager::getInstance(dbName);
	char* block = bm->pinPage(recId.blockId);
	SlottedPage sp(block);
	sp.deleteRecord(recId.slot);
	bm->unpinPage(recId.blockId, true);
}
void Relation::deleteRelation(){
	BufferManager *bm = BufferManager::getInstance(dbName);
	int blockId = relRecId.blockId;
	char* block = bm->pinPage(blockId);
	SlottedPage* sp = new SlottedPage(block);
	short slot = sp->deleteRecord(relRecId.slot);
	delete sp;
	if (slot < 0)	// relation does not exist
	{
		bm->unpinPage(blockId, false);
		return;
	}else{
		bm->unpinPage(blockId, true);
		
		// remove schema
		blockId = schemaRecId.blockId;
		block = bm->pinPage(blockId);
		sp = new SlottedPage(block);
		sp->deleteRecord(schemaRecId.slot);
		delete sp;
		bm->unpinPage(blockId, true);
		
		// delete relation
		while(firstBlockId >= 0){
			block = bm->pinPage(firstBlockId);
			memcpy(&blockId, block, SIZE_INT);
			bm->unpinPage(firstBlockId, false);
			StorageManager::freeBlock(dbName, firstBlockId);
			firstBlockId = blockId;
		}
	}
}

/** BlockRecIterator */
BlockRecIterator::BlockRecIterator(int blockId, Schema* s)  {
	assert(blockId > 0);
	
	this->BlockId = blockId;
	this->RecSchema = s;
	bm = NULL;
	SlotCount = 0;
	TotalNumOfSlots = 0;

	bm = BufferManager::getInstance(dbName); 	
	block = bm->pinPage(BlockId);
	isOpen = false;
}
BlockRecIterator::BlockRecIterator(char *block, Schema* s){
	assert(block!=NULL);
	
	this->block = block;
	this->RecSchema = s;
	bm = NULL;
	SlotCount = 0;
	TotalNumOfSlots = 0;
}
void BlockRecIterator::open()  {
	sp = new SlottedPage(block);
	TotalNumOfSlots = sp->getTotNoSlots();
	isOpen = true;
}
Record* BlockRecIterator::next()  {
	if(isOpen)   {
		char* RecBytes = NULL;
		do  {
			RecBytes = sp->getRecord(SlotCount++);
		}while(RecBytes == NULL && SlotCount < TotalNumOfSlots);

		if(RecBytes==NULL)
			return NULL;
			
		Record* Rec = new Record(RecBytes,RecSchema);
		return Rec;
	}
	else
		cerr << "BlockRec Iterator not initialised." << endl;
	return NULL;
}
void BlockRecIterator::close()	{
	isOpen = false;
	SlotCount = 0;
	RecSchema = NULL;
	delete sp;
}

/** RelBlockIterator */
RelBlockIterator::RelBlockIterator(const char *dbName, const char *relName)	 {
	this->dbName  = dbName;	
	this->relName = relName;
	nextBlockId = -1;
	bm = NULL;
	isOpen = false;
}
void RelBlockIterator::open()  {
	Relation rel(dbName, relName, false, NULL);
	nextBlockId = rel.getFirstBlockId();
	isOpen = true;
}
int RelBlockIterator::next()  {
	if(isOpen)   {
		if(nextBlockId <= 0)
			return -1;

		bm = BufferManager::getInstance(dbName);
		char* block = bm->pinPage(nextBlockId);
		int nextRec = nextBlockId;
		nextBlockId = readInt(block,0);
		bm->unpinPage(nextRec, false);	
		
		return nextRec;
	}
	else
		cerr << "RelBlock Iterator not initialised." << endl;
	return -1;
}
void RelBlockIterator::close()	{
	isOpen = false;
	nextBlockId = -1;
}

/** BaseRelIterator */
BaseRelIterator::BaseRelIterator(const char *dbName, const char *relName){
	this->dbName = dbName;
	this->relName = relName;
	BlockIterator = NULL;
	RecIterator = NULL;
	RecSchema = NULL;
	isOpen = false;
}
void BaseRelIterator::open()  {
	BlockIterator = new RelBlockIterator(dbName, relName);
	BlockIterator->open();
	Relation RelObj(dbName, relName, false, NULL);		
	RecSchema = RelObj.getSchema();
	isOpen = true;	
}
Record* BaseRelIterator::next()  {
	if(isOpen)   {
		if(RecIterator == NULL)  {
			int BlockId = BlockIterator->next();
			if(BlockId<0)
				return NULL;
			RecIterator = new BlockRecIterator(BlockId, RecSchema);
			RecIterator->open();
		}

		while(true)  {
			Record* record = RecIterator->next();
			if(record!=NULL)
				return record;
			delete RecIterator;
			int BlockId = BlockIterator->next();
			if(BlockId < 0)
				return NULL;
			RecIterator = new BlockRecIterator(BlockId, RecSchema);
			RecIterator->open();
		}
	}
	else
		cerr << "BaseRel Iterator not initialised." << endl;
	return NULL;
}
void BaseRelIterator::close()  {
	isOpen = false;
	delete RecIterator;
	delete BlockIterator;
	RecSchema = NULL;
}

//gets the column number for the char* on which the condition is given
void BaseCon::findColumn(char* s, Schema* schemaPtr,int& i)  {
	i=-1;
	int numOfAttrs=schemaPtr->getNumOfAttrs();

	for(int count=0;count<numOfAttrs;count++)  {
		if(strcmp(s,schemaPtr->getAttrName(count))==0)  {
			i=count;
			break;
		}
	}
}
char* BaseCon::getLeftVal()  {
	if(!useIndexIfExists)
		return NULL;
	return l;
}
char* BaseCon::getRightVal()  {
	return r;
}

bool Equal::evaluate(Record rc)  {

	int leftPredColNo;
	int rightPredColNo;
	Schema* schemaPtr=rc.getSchema();
	findColumn(l,schemaPtr,leftPredColNo);
	findColumn(r,schemaPtr,rightPredColNo);

	if(leftPredColNo!=-1)   {
		if(rightPredColNo!=-1)   {
			int dataType1=schemaPtr->getAttrType(leftPredColNo);
			int dataType2=schemaPtr->getAttrType(rightPredColNo);
		
			if( (dataType1==INTEGER && dataType2==INTEGER) || ( dataType1==FLOAT && dataType2==FLOAT) )   {
				float a;float b;	
				if(dataType1==INTEGER)
					a=(float)rc.getInt(leftPredColNo);
				else if(dataType1==FLOAT)
					a=rc.getFloat(leftPredColNo);
				if(dataType2==INTEGER)
					b=(float)rc.getInt(rightPredColNo);
				else if(dataType2==FLOAT)
					b=rc.getFloat(rightPredColNo);
				return (a==b);
			}			
			else if(dataType1==STRING && dataType2==STRING)   {
				char *a,*b;
				a=rc.getString(leftPredColNo);					
				b=rc.getString(rightPredColNo);
				return (a==b);
			}
			else
				cout<<"Invalid Comparison"<<endl;			
		}
		else   {
			if(schemaPtr->getAttrType(leftPredColNo)==STRING )
				return (rc.getString(leftPredColNo)==r);
			else if(schemaPtr->getAttrType(leftPredColNo)==INTEGER)  {
				int a,b;	
				a=rc.getInt(leftPredColNo);
				stringstream ss;
				ss<<r;
				ss>>b;
				//b=atoi(r);
				return (a==b);					
			}
			else   {
				float a,b;
				a=rc.getFloat(leftPredColNo);
				stringstream ss;
				ss<<r;
				ss>>b;
				//b=stof(r);
				return(a==b);
			}
		}
	}
	else	
		cout<<"L.H.S should contain some attribute name"<<endl;
	return false;
}
bool Greater::evaluate(Record rc)   {

	int leftPredColNo;
	int rightPredColNo;
	Schema* schemaPtr=rc.getSchema();
	findColumn(l,schemaPtr,leftPredColNo);
	findColumn(r,schemaPtr,rightPredColNo);

	if(leftPredColNo!=-1)   {
		if(rightPredColNo!=-1)   {
			int dataType1=schemaPtr->getAttrType(leftPredColNo);
			int dataType2=schemaPtr->getAttrType(rightPredColNo);
		
			if( (dataType1==INTEGER && dataType2==INTEGER) || (dataType1==FLOAT && dataType2==FLOAT) )   {
				float a,b;	
				if(dataType1==INTEGER)
					a=(float)rc.getInt(leftPredColNo);
				else if(dataType1==FLOAT)
					a=rc.getFloat(leftPredColNo);
				if(dataType2==INTEGER)
					b=(float)rc.getInt(rightPredColNo);
				else if(dataType2==FLOAT)
					b=rc.getFloat(rightPredColNo);
				return (a>b);
			}
			
			else if(dataType1==STRING && dataType2==STRING)   {
				char *a,*b;
				a=rc.getString(leftPredColNo);					
				b=rc.getString(rightPredColNo);
				return (a>b);
			}
			else
				cout<<"Invalid Comparison"<<endl;			
		}
		else   {
			if(schemaPtr->getAttrType(leftPredColNo)==STRING )
				return (rc.getString(leftPredColNo)>r);
			else if(schemaPtr->getAttrType(leftPredColNo)==INTEGER)   {
				int a,b;	
				a=rc.getInt(leftPredColNo);
				stringstream ss;
				ss<<r;
				ss>>b;
				//b=atoi(r);
				return (a>b);					
			}
			else
			{
				float a,b;
				a=rc.getFloat(leftPredColNo);
				stringstream ss;
				ss<<r;
				ss>>b;
				//b=stof(r);
				return(a>b);
			}
		}
	}
	else	
		cout<<"L.H.S should contain some attribute name";
}	
bool GreaterEq::evaluate(Record rc)  {
	char* l1 = l;
	char* r1 = r;
	
	Less ob(l1,r1);
	return !(ob.evaluate(rc));
}	
bool Less::evaluate(Record rc)  {
	
	int leftPredColNo;
	int rightPredColNo;
	Schema* schemaPtr=rc.getSchema();
	findColumn(l,schemaPtr,leftPredColNo);
	findColumn(r,schemaPtr,rightPredColNo);
	
	if(leftPredColNo!=-1)   {
		if(rightPredColNo!=-1)   {
			int dataType1=schemaPtr->getAttrType(leftPredColNo);
			int dataType2=schemaPtr->getAttrType(rightPredColNo);
		
			if( (dataType1==INTEGER && dataType2==INTEGER) || (dataType1==FLOAT && dataType2==FLOAT) )   {
				float a,b;	
				if(dataType1==INTEGER)
				{	
					a=(float)rc.getInt(leftPredColNo);
				}
				else if(dataType1==FLOAT)
				{	
					a=rc.getFloat(leftPredColNo);
				}
				if(dataType2==INTEGER)
				{	
					b=(float)rc.getInt(rightPredColNo);
				}
				else if(dataType2==FLOAT)
				{	
					b=rc.getFloat(rightPredColNo);
				}
				return (a<b);
			}
			
			else if(dataType1==STRING && dataType2==STRING)   {
				char *a,*b;
				a=rc.getString(leftPredColNo);					
				b=rc.getString(rightPredColNo);
				return (a<b);
			}
			else
				cout<<"Invalid Comparison"<<endl;		
		}
		else   {
			if(schemaPtr->getAttrType(leftPredColNo)==STRING )
				return (rc.getString(leftPredColNo)<r);
			else if(schemaPtr->getAttrType(leftPredColNo)==INTEGER)   {
				int a,b;	
				a=rc.getInt(leftPredColNo);
				stringstream ss;
				ss<<r;
				ss>>b;
				//b=atoi(r);
				return (a<b);					
			}
			else   {
				float a,b;
				a=rc.getFloat(leftPredColNo);
				stringstream ss;
				ss<<r;
				ss>>b;
				//b=stof(r);
				return(a<b);
			}
		}
	}
	else
		cout<<"L.H.S should contain some attribute name";
}
bool LessEq::evaluate(Record rc)  {
	char* l1 = l;
	char* r1 = r;
	
	Greater ob(l1,r1);
	return !(ob.evaluate(rc));
}
bool NotEq::evaluate(Record rc)  {
	char* l1 = l;
	char* r1 = r;

	Equal ob(l1,r1);
	return !(ob.evaluate(rc));
} 

/** Predicate Scan Iterator */
PredIterator::PredIterator(const char *dbName, const char *relName, Predicate* pred, BPlusTree* BPT, int key)  {
	
	assert(pred!=NULL);
	this->dbName = dbName;
	this->relName = relName;
	this->pred = pred;
	this->BPT = BPT;
	this->keyVal = key;
	isOpen = false;
	baseIterator = NULL;
	IndexedKeyFound = false;
	RecSchema = NULL;
}
void PredIterator::open()  {
	baseIterator = new BaseRelIterator(dbName,relName);
	baseIterator->open();
	Relation RelObj(dbName, relName, false, NULL);		
	RecSchema = RelObj.getSchema();
	this->isOpen = true;
}
Record* PredIterator::next()  {
	if(isOpen)   {
		Record* rec = NULL;

		if(BPT == NULL)   {
			do{
				rec = baseIterator->next();
			}while(rec!= NULL && !pred->evaluate(*rec));
			
			return rec;
		}
		else   {
			if(!IndexedKeyFound)   {
				int RecId = BPT->lookUp(keyVal);
				int* RecIdPtr = &RecId;

				short BlockID;
				short *shortPtr = &BlockID;			
				memcpy(shortPtr,RecIdPtr,0);
				
				short slotNo;
				shortPtr = &slotNo;
				memcpy(shortPtr,RecIdPtr,2);
				
				BufferManager* bm = BufferManager::getInstance(dbName);
				char* block = bm->pinPage(BlockID);
				
				SlottedPage* sp = new SlottedPage(block);
				char* RecBytes = sp->getRecord(slotNo);
				delete sp;
				
				Record* record = new Record(RecBytes,RecSchema);
				
				IndexedKeyFound = true;
				return record;
			}		
			return NULL;
		}
	}
	else
		cerr << "Pred_Iterator not initialised." << endl;
	return NULL;
}
void PredIterator::close()  {
	delete baseIterator;
	isOpen = false;
}

RIterator* Relation::openScan(Predicate* p)  {
	int key = 0;
	BPlusTree* BPT;
	
	if(!p->useIndexIfExists)
		BPT = NULL;
	
	else   {
		char* indexName;
		strcpy(indexName, relName);	
		strcat(indexName, "_");	
		strcat(indexName, p->getLeftVal());

		BPT = checkIndex(indexName);

		if(BPT!=NULL)   {
			if(schema->getAttrType(p->getLeftVal()) != INTEGER)
				return 0;
			else   {
				stringstream ss;
				ss << p->getRightVal();
				ss >> key;
			}
		}
	}
	
	return new PredIterator(dbName, relName, p, BPT, key);
}

BPlusTree* Relation::checkIndex(char* indexName)  {

	int rootBlockID = StorageManager::getFirstBlockId(dbName, indexName);
	
	if(rootBlockID != -1)	{
		return new BPlusTree(dbName, indexName, false);
	}	
	return NULL;
}
