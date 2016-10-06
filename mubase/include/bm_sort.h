#ifndef BM_H
#define BM_H

#include "global.h"

/**
 * Queue for LRU replacement policy.
 */
class lruNode{
	public:	
		int frameId;
		lruNode *nextPtr;
		lruNode *prevPtr;
};

/**
 * Buffer Manager for MuBase. Implements LRU replacement policy.
 */
class BufferManager {

    private:
    /**
     * BufferManager will have a singleton class (direct 
     * instantiation is prohibited - so the constructor is 
     * private.
     */
	static BufferManager *loneInstance;

    /**
     * The db name.
     */
    char *dbName;

    /**
     * The actual buffer pool, which stores the pages.
     */
    char *buffer;

	/**
	 * Pincount of the blocks managed by the BufferManager.
	 */
 	int pinCount[NUM_BUFFS];

	/**
	 * Flag to indicate writes on the blocks
	 * managed by the BufferManager.
	 */
    bool dirtyFlag[NUM_BUFFS];

	/**
	 * Array for maintaining the mapping between 
	 * the frames of the BufferPool and the blocks of the File. 
	 */
    int blockInFrame[NUM_BUFFS];

	/**
	 * Count of filled default_frames
	 */
    short filledDefaultFrames;
	
	/**
	 * Count of filled reserved_frames
	 */
    short filledReservedFrames;
	
	/**
	 * Pointers to LRU Queue for DEFAULT frames
	 */
	lruNode *defaultLruHead;
	lruNode *defaultLruTail;
	
	
	/**
	 * Pointers to LRU Queue for Reserved frames
	 */
	lruNode *reservedLruHead;
	lruNode *reservedLruTail;

    /**
     * Private constructor.
     */
    BufferManager(const char *dbName);
        // Open the db and allocate memory for the buffer pool
        // to hold NUM_BUFFS number of blocks.
        
    public:

    static BufferManager *getInstance(const char *dbName);     

	/**
	 * Gets the Total number of blocks.
	 */
	int getTotNoBlocks();

	/**
	 * Function to insert a node in the Queue
	 */
	void insertNode(int frameId);

	/**
	 * Function to delete a node in the Queue when
	 * the Pincount increases above zero.
	 */
	void deleteNode(int frameId);
	
	/**
	 * Function to evict a node in the Queue when
	 * there is not enough space in the BufferPool.
	 */
	int evict(  bool inReserve = false);

    /**
     * Fetch the block in memory (if not already present), increment
     * its pinCount and return its address.
     */
    char *pinPage(int blockId , bool inReserve = false);

    /**
     * Reduce the pinCount of the buffer block. If the pinCount 
     * becomes 0, the buffer block will be a candidate for being
     * evicted (eviction happens lazily when the need arises).
     */
    void unpinPage(int blockId, bool isDirty,bool inReserve = false);
	void unpinPage(char* blockPtr, bool isDirty);
	
    /**
     * Forcibly write a given buffer block to the disk.
     */
    void forcePage(int frameId);
	
	/**
	 * Gives the block ID stored in the Frame 
	 */	 
	int getBlockInFrame(int blockId,  bool inReserve = false);
	
	/**
	 * Gives the next Free Frame 
	 */
	int getFreeFrame( bool inReserve = false);
	
	
	/**
	 * Gives the Free Frame for temproray purpose. 
	 */
	char* getTempFrame(char* blockPtr , bool inReserve = false);
	
	void releaseTempFrame(char* blockPtr);
	
	/**
	 * Gives the total no of Free Frame 
	 */
	int getTotFreeFrame(bool inReserve = false);
	
	int getDiskBlockId(char* blockPtr);
    /**
     * Destructor. Write all dirty pages to disk.
     */
     ~BufferManager();
};


#endif
