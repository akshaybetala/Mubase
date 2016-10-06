#ifndef UTIL_H
#define UTIL_H

/**
 * Returns length of a record.  Assume records are NULL 
 * terminated (not true in practice, but a shortcut).
 */
int length(char *record);

/** Returns 2 bytes starting at block + offset as an int. */
short readShort(char *block, short offset);
/** Writes a 2 byte short value at block + offset */
void writeShort(char *block, short offset, short value);

/** Reads a 4 byte int value at block + offswt */
int readInt(char *block, short offset);
/** Writes a 4 byte int value at block + offset */
void writeInt(char *block, short offset, int value);

/** Returns a fragment of the byte array */
char *readBytes(char *block, short recOffset, short recLength);
/** Writes a fragment in the byte array */
void writeBytes(char *block, short offset, char *rec, short recLength);
/** Writes a fragment in the byte array */
void writeBytes(char *block, short offset, char *rec);

/** Move a chunk of bytes */
void moveBytes(char *block, short destOffset, short sourceOffest, 
               short chunkLength);
/** moves chunks of bytes from src char* to destination char* */ 
void moveBytes(char *offsetInSrcLeaf, 
				char *offsetInDestLeaf, 
				short chunkLength);
/** makes the block compact after something in middle is removed */ 
void makeCompact(char *diskFirstblock, short destOffset, 
				short sourceOffset, short chunkLength);

#endif	// UTIL_H