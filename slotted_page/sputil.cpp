/** ----------------------------------------------------
 * CS3010: Introduction to Database Systems
 * Department of Computer Science and Engineering
 * IIT Hyderabad
 *
 * File: sputil.c
 * Implementation of helper functions for the slotted page.
 * To be provided by the students.
 * -----------------------------------------------------
 */

#include<iostream>
#include<cstring>
#include<cstdlib>
#include<cstdio>

#include "sputil.h"

using namespace std;

// Returns length of a record.  Assume records are NULL 
// terminated (not true in practice, but a shortcut).
inline int length(char *record) {
    return strlen(record);
}

// Returns 2 bytes starting at block + offset as a short.
short readShort(char *block, short offset) {
    short value;
    short *shortPtr = &value;
    
    memcpy(shortPtr,block+offset,2); 
    return value;
}

// Writes a 2 byte short value at block + offset
void writeShort(char *block, short offset, short value) {
    short *shortPtr = &value;
    memcpy(block+offset,shortPtr,2);
}

// Returns a fragment of the byte array
char *readBytes(char *block, short recOffset, short recLength) {
    char *rec = (char *) malloc((size_t) (recLength+1));
	//scaning d record from left end
    for(short i =0;i<recLength;i++){
		    rec[i] = block[recOffset + i]; 
    }
	rec[recLength]='\0';
    return rec;
}

// Writes a fragment in the byte array
void writeBytes(char *block, short offset, char *rec) {
	memcpy(block+offset,rec,strlen(rec));
}

// Move a chunk of bytes. Use memcpy to implement this.
void moveBytes(char *block, short destOffset, short sourceOffest, 
               short chunkLength) {
      memmove( block + destOffset, block + sourceOffest, chunkLength);
}

