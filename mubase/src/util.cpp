#include "util.h"
#include "global.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
using namespace std;

short readShort(char *block, short offset) {
    short value;
    memcpy(&value, block+offset, SIZE_SHORT); 
    return value;
}
void writeShort(char *block, short offset, short value) {
    memcpy(block+offset, &value, SIZE_SHORT);
}

int readInt(char *block, short offset) {
    int value;
    memcpy(&value, block+offset, SIZE_INT);
    return value;
}
void writeInt(char *block, short offset, int value) {
    memcpy(block+offset, &value, SIZE_INT);
}

char *readBytes(char *block, short recOffset, short recLength) {
    char* rec = (char*)malloc(recLength);
	memcpy(rec, block+recOffset, recLength);
    return rec;
}
void writeBytes(char *block, short offset, char *rec, short recLength) {
	memcpy(block+offset, rec, recLength);
}
void writeBytes(char *block, short offset, char *rec) {
    memcpy(block+offset, rec, strlen(rec));
}

void moveBytes(char *block, short destOffset, short sourceOffest, short chunkLength) {
      memmove( block + destOffset, block + sourceOffest, chunkLength);
}
void moveBytes(char *offsetInSrcLeaf, char *offsetInDestLeaf, short chunkLength){
    memmove( offsetInDestLeaf, offsetInSrcLeaf, chunkLength);
}
void makeCompact(char *diskFirstblock, short destOffset, short sourceOffset, 
               	short chunkLength){
    memmove(diskFirstblock + destOffset, diskFirstblock + sourceOffset, 
				chunkLength);
}

