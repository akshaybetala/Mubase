/** ----------------------------------------------------
 * CS3010: Introduction to Database Systems
 * Department of Computer Science and Engineering
 * IIT Hyderabad
 *
 * File: spimpl.cpp
 * Implementation of the methods provided by class SlottedPage.
 * Implementation to be provided by students. 
 * -----------------------------------------------------
 */

#include<iostream>
#include<cstring>
#include<cstdlib>
#include<cstdio>

#include "sp.h"
#include "sputil.h"

using namespace std;

// Initializes (formats) the slotted page
void SlottedPage::initBlock() {
    writeShort(INTOFFSET, (short)0, (short)0);
    writeShort(INTOFFSET, (short)2, (short)BLOCKSIZE);
}

// Shows the contents of the page
void SlottedPage::display() {
    short n    = readShort(INTOFFSET, (short) 0);
    short eofs = readShort(INTOFFSET, (short) 2);
   
    short availableSpace = eofs - (n + 1)*4;
 
    cout << "Number of slot entries = " << n << endl
         << "Available free space = " << availableSpace << endl;

    for (short i = 0; i < n; i++) {
        cout << "Record at slot " << i << endl;
        char *rec = getRecord(i);
        if (rec == NULL)
            cout << "None"<<endl; 
        else
            cout << rec<<endl;
    }
    cout << endl;
}

char *SlottedPage::getRecord(short slotNum) {
    
	short n    = readShort(INTOFFSET, (short) 0);
	short recLength=readShort( INTOFFSET,(slotNum+(short)1 )*(short)4 );
	short offset=readShort( INTOFFSET,(slotNum+(short)1 )*(short)4 +(short)2);
	if(offset == -1 || n <= slotNum)
		return NULL;
	char * rec = readBytes(INTOFFSET,offset,recLength);
	return rec;
}

short SlottedPage::storeRecord(char *record) {

	short n = readShort(INTOFFSET,(short)0);
    short eofs = readShort(INTOFFSET,(short)2);
    short availableSpace = eofs - (n + 1)*4;
 	short recLength=strlen(record);
	if(availableSpace < recLength + 4)
		return (short)-1;
	eofs= eofs - recLength;
	writeBytes(INTOFFSET,eofs+1,record);
	writeShort(INTOFFSET,(short)0,n+1);
	writeShort(INTOFFSET,(short)2,eofs);
	
	for(int i=0;i<n;i++)
	{
		short offset=readShort( INTOFFSET,(i+(short)1 )*(short)4 +(short)2 );
		if(offset == -1)
		{
			writeShort(INTOFFSET,(i+1)*4,recLength);
			writeShort(INTOFFSET,(i+1)*4 + 2 , eofs+1);
			return i;
		}
	
	}
	
	writeShort(INTOFFSET,(n+1)*4,recLength);
	writeShort(INTOFFSET,(n+1)*4 + 2 , eofs+1);
	return n;
}

short SlottedPage::deleteRecord(short slotNum) {
    short n    = readShort(INTOFFSET, (short)0);
    short eofs = readShort(INTOFFSET, (short)2);
    short offset=readShort( INTOFFSET,(slotNum+(short)1 )*(short)4  + (short)2 );
	if( n <= slotNum || offset == -1)
		return (short)(-1); 
	
	short recLength=readShort(INTOFFSET,(slotNum+(short)1 )*(short)4 );
    
	moveBytes(INTOFFSET,eofs+recLength+1,eofs+1,offset-eofs-1);
	writeShort(INTOFFSET, (short)2, eofs+recLength);

	for(short i=0;i<n;i++){
		short temp_offset=readShort( INTOFFSET,( i+(short)1 )*(short)4  + (short)2 );
		if(temp_offset!= -1 && temp_offset < offset){
			writeShort(INTOFFSET, ((i+(short)1 )*(short)4 + (short)2), temp_offset+recLength);
		}
	}	
	writeShort(INTOFFSET,(slotNum+(short)1 )*(short)4  + (short)2, (short)(-1));
	
	return eofs-(n+1)*2+recLength;
}

bool SlottedPage::swapRecord(short slotNum1,short slotNum2){
	
	short n    = readShort(INTOFFSET, (short)0);
	if( n <= slotNum1 || n <= slotNum2)
		return false; 
	if(slotNum1==slotNum2)
		return true;
	
	short recLength1=readShort( INTOFFSET,(slotNum1+(short)1 )*(short)4 );
	short offset1=readShort( INTOFFSET,(slotNum1+(short)1 )*(short)4 +(short)2 );
	
	moveBytes(INTOFFSET,(slotNum1+1)*4,(slotNum2+1)*4,4);
	writeShort(INTOFFSET,(slotNum2+1)*4,recLength1);
	writeShort(INTOFFSET,(slotNum2+1)*4 + 2 ,offset1);
	
	return true;
}

short SlottedPage::getTotNoRec(){
	short n    = readShort(INTOFFSET, (short)0);
    short count = 0;
	for (short i=0; i < n; i++) {
		short offset=readShort( INTOFFSET,(i+(short)1 )*(short)4 +(short)2 );
		if(offset != -1)
			count++;
	}
    return count;
}
	
short SlottedPage::getTotNoSlots(){
	return readShort(INTOFFSET, (short)0);
}


