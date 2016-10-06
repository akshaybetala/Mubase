#include "bm.h"
#include "sp.h"
#include "util.h"
#include "sm.h"
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
using namespace std;


void StorageManager::initDB(const char *dbName){
	BufferManager *bm = BufferManager::getInstance(dbName);
	int initialNumBlocks = bm->getTotNoBlocks();
	char *diskBlock;
	for(int i=0;i< initialNumBlocks-1;i++)
	{
		diskBlock = bm->pinPage(i);
		writeInt(diskBlock, 0, i+1);
		bm->unpinPage(i,true);
	}

	diskBlock = bm->pinPage(initialNumBlocks-1);
	writeInt(diskBlock, 0, -1);
	bm->unpinPage(initialNumBlocks-1,true);
	diskBlock = bm->pinPage(0);
	
	// create system objscts
	int SYSOBJ_ID = createObject(dbName, SYSOBJ);
	SlottedPage *SYSOBJ_SP = new SlottedPage(bm->pinPage(SYSOBJ_ID));
	SYSOBJ_SP->initBlock();
	delete SYSOBJ_SP;
	bm->unpinPage(SYSOBJ_ID,true);
	
	int SYSCOL_ID = createObject(dbName, SYSCOL);
	SlottedPage SYSCOL_SP( bm->pinPage(SYSCOL_ID));
	SYSCOL_SP.initBlock();
	bm->unpinPage(SYSCOL_ID,true);
	
	//initialise the value of nofObjects(see createObject) with 2
	writeInt(diskBlock, SIZE_INT, 2);
	bm->unpinPage(0,true);
}

int StorageManager::createObject(const char *dbName, const char *objectName){
	BufferManager *bm = BufferManager::getInstance(dbName);
	char *diskFirstBlock = bm->pinPage(0);
	int nofObjects = readInt(diskFirstBlock, SIZE_INT);
	
	//if(nofObjects >= (BLOCK_SIZE/22)-1 )
		//throw exception saying that not enough sapce 4 new object
		
    char name[OBJNAME_SIZE];
    memcpy(name, objectName, OBJNAME_SIZE);
    name[OBJNAME_SIZE-1] = 0;
	
	//objectName assumed to take not more than OBJNAME_SIZE characters
	writeBytes(diskFirstBlock, 
				2*SIZE_INT +nofObjects*(OBJ_SIZE), &name[0], OBJNAME_SIZE);
		
	//allocating new block
	int ObjectFirstBlockId = allocateBlock(dbName);
	char *firstFreeBlock = bm->pinPage(ObjectFirstBlockId);
	writeInt(firstFreeBlock, 0, 0);
	bm->unpinPage(ObjectFirstBlockId,true);
	
	// writing first blockId alloted to the created object 
	writeInt(diskFirstBlock, 
			2*SIZE_INT + nofObjects*(OBJ_SIZE) + 
			OBJNAME_SIZE, ObjectFirstBlockId);

	// writing  last  blockId alloted to the created object
	writeInt(diskFirstBlock, 
			2*SIZE_INT + nofObjects*(OBJ_SIZE) + 
			OBJNAME_SIZE+SIZE_INT, ObjectFirstBlockId);
	//updating the number of objects
	writeInt(diskFirstBlock, SIZE_INT, nofObjects+1);
	//cout<<diskFirstBlock+ 2*SIZE_INT +nofObjects*(OBJ_SIZE)<<endl;
	bm->unpinPage(0,true);	
	
	return ObjectFirstBlockId;
}

int StorageManager::getFirstBlockId(const char *dbName, const char *objectName){
	BufferManager *bm = BufferManager::getInstance(dbName);
	char *diskFirstBlock = bm->pinPage(0);
	int nofObjects = readInt(diskFirstBlock, SIZE_INT);
	char *objName;
	int ObjectFirstBlockId = -1;
	//~ cout<<" nofObjects "<<nofObjects<<endl;
	for(int i=0;i<nofObjects;i++){
		objName = readBytes(diskFirstBlock,	2*SIZE_INT + i*OBJ_SIZE, OBJNAME_SIZE);

		if( strcmp(objName,objectName) == 0 ){
			ObjectFirstBlockId = readInt(diskFirstBlock,2*SIZE_INT + i*OBJ_SIZE + OBJNAME_SIZE);
			bm->unpinPage(0,false);
			free(objName);	
			break;
		}
	}
	return ObjectFirstBlockId;
}

int StorageManager::getLastBlockId(const char *dbName, const char *objectName){
	BufferManager *bm = BufferManager::getInstance(dbName);
	 char *diskFirstBlock = bm->pinPage(0);
	 int nofObjects = readInt(diskFirstBlock, SIZE_INT);
	 char *objName;
	 int ObjectLastBlockId;
	 cout<<" nofObjects "<<nofObjects<<endl;
	 for(int i=0;i<nofObjects;i++){
	 	objName = readBytes(diskFirstBlock,	2*SIZE_INT + i*OBJ_SIZE, OBJNAME_SIZE);
	 	
	 	if( strcmp(objName,objectName) == 0 ){
	 		ObjectLastBlockId = readInt(diskFirstBlock,2*SIZE_INT + i*OBJ_SIZE + OBJNAME_SIZE+ SIZE_INT);
	 		bm->unpinPage(0,false);
	 		free(objName);	
	 		
	 		return ObjectLastBlockId;
	 	}
	 }	                       
}

int StorageManager::allocateBlock(const char *dbName){
	BufferManager *bm = BufferManager::getInstance(dbName);
	char *diskFirstBlock = bm->pinPage(0);
	int firstFreeBlockId = readInt(diskFirstBlock,0);		
	
	char *firstFreeBlock = bm->pinPage(firstFreeBlockId);
	int secondFreeBlockId =   readInt(firstFreeBlock,0);
	
	writeInt(diskFirstBlock, 0, secondFreeBlockId);
	writeInt(firstFreeBlock, 0, -1);
	
	bm->unpinPage(0,true);
	bm->unpinPage(firstFreeBlockId,false);
	return firstFreeBlockId;
}

int StorageManager::freeBlock(const char *dbName, int blockId){
	BufferManager *bm = BufferManager::getInstance(dbName);
	char *diskFirstBlock = bm->pinPage(0);
	char *newFreeBlock = bm->pinPage(blockId);
	
	int firstFreeBlockId = readInt(diskFirstBlock,0);
	
	writeInt(diskFirstBlock, 0, blockId);
	writeInt(newFreeBlock, 0, firstFreeBlockId);

	bm->unpinPage(0,true);
	bm->unpinPage(blockId,true);
}

void StorageManager::addBlock(const char *dbName, const char *objectName, int blockId){
	
	if(blockId < 1)
		return;
	BufferManager *bm = BufferManager::getInstance(dbName);
	char *diskFirstBlock = bm->pinPage(0);
	int nofObjects = readInt(diskFirstBlock, SIZE_INT);
	char *objName;
	int ObjectLastBlockId;
	cout<<" nofObjects "<<nofObjects<<endl;
	for(int i=0;i<nofObjects;i++){
		objName = readBytes(diskFirstBlock,	2*SIZE_INT + i*OBJ_SIZE, OBJNAME_SIZE);
		
	 	if( strcmp(objName,objectName) == 0 ){
	 		ObjectLastBlockId = readInt(diskFirstBlock,2*SIZE_INT + i*OBJ_SIZE + OBJNAME_SIZE + SIZE_INT);
			writeInt(diskFirstBlock,2*SIZE_INT + i*OBJ_SIZE + OBJNAME_SIZE + SIZE_INT,blockId);
	 		bm->unpinPage(0,true);
			free(objName);	
			char* lastBlock=bm->pinPage(ObjectLastBlockId);
	 		writeInt(lastBlock, 0, blockId);
	 		char* newBlock=bm->pinPage(blockId);
	 		writeInt(newBlock, 0,0);
	 		
	 	}
	}
}

void StorageManager::dropObject(const char *dbName, char *objectName){
	BufferManager *bm = BufferManager::getInstance(dbName);
	char *diskFirstBlock = bm->pinPage(0);
	int nofObjects = readInt(diskFirstBlock, SIZE_INT);
	char *objName;
	

	for(int i=0;i<nofObjects;i++){
		objName = readBytes(diskFirstBlock, 
							2*SIZE_INT + i*OBJ_SIZE, OBJNAME_SIZE);

		if( strcmp(objName,objectName) ){
			int ObjectFirstBlockId = readInt(diskFirstBlock,2*SIZE_INT + i*OBJ_SIZE + OBJNAME_SIZE);
			int ObjectLastBlockId = readInt(diskFirstBlock,2*SIZE_INT + i*OBJ_SIZE + OBJNAME_SIZE + SIZE_INT);
			char* ObjectLastBlock=bm->pinPage(ObjectLastBlockId);
			int firstFreeBlockId = readInt(diskFirstBlock,0);
			writeInt(ObjectLastBlock,0,firstFreeBlockId);
			writeInt(diskFirstBlock,0,ObjectFirstBlockId);
			bm->unpinPage(ObjectLastBlock,true);
			makeCompact(diskFirstBlock,
						2*SIZE_INT + i*OBJ_SIZE ,
						2*SIZE_INT + (i+1)*OBJ_SIZE, 
						(nofObjects-i-1)*OBJ_SIZE);
			writeInt(diskFirstBlock,SIZE_INT,nofObjects-1);
			
			bm->unpinPage(0,true);
			free(objName);
			break;
		}			
	}	
}

