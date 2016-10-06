#ifndef GLOBAL
#define GLOBAL

#define BLOCK_SIZE 4096
#define DEFAULT_BUFFS 100
#define RESERVED_BUFFS 20
#define NUM_BUFFS  (DEFAULT_BUFFS + RESERVED_BUFFS)
#define KEY_SIZE 4
#define PTR_SIZE 4
#define PAIR_SIZE (KEY_SIZE + PTR_SIZE)
#define SIZE_INT 4
#define SIZE_SHORT 2
#define OBJNAME_SIZE 50
#define OBJ_SIZE (OBJNAME_SIZE+SIZE_INT+SIZE_INT)
#define MAX_ATTRS 10
#define SIZE_ATTR 15
#define RELNAME_SIZE OBJNAME_SIZE
#define MAX_INPUT_REC_SIZE (BLOCK_SIZE-SIZE_INT-4*SIZE_SHORT)

// System objscts
#define SYSOBJ "sysobj"
#define SYSCOL "sysCol"

// Data types
#define INTEGER (int)1
#define FLOAT   (int)2
#define STRING  (int)3

#define SIZE_INTEGER sizeof(int)
#define SIZE_FLOAT sizeof(float)


#endif	// GLOBAL

/**
 * Disk structure :
 * Block 1(DISK_BLOCK): |(SIZE_INT)next free block|(SIZE_INT)no of objects|(OBJ_SIZE)obj1|(OBJ_SIZE)obj2|...|(OBJ_SIZE)objN| (over flow may happen)
 * obj1 -> SYSOBJ
 * obj2 -> SYSCOL
 * 
 * 
 * (OBJ_SIZE)objx :
 * |(OBJNAME_SIZE)object name|(SIZE_INT)object first block|
 * 
 * Free list :
 * (SIZE_INT)next free block no, -1 if last
 * 
 * 
 * 
 */

