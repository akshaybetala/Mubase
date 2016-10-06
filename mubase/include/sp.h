#ifndef SP_H
#define SP_H

class SlottedPage {
    private:
    
	// The underlying buffer block, to which this class is a wrapper.
    char *block;
	
	// Initial Offset to account for block pointers
	char* INTOFFSET;
	public:

    // Constructor.
    SlottedPage(char *buffPage);

    /**
     * Initialize (format) the block.
     */
    void initBlock();

    /**
     * Retrieve a record at the given slot number. Returns -1
     * if the slot number is invalid.
     */
    char *getRecord(short slotNum);

    /**
     * Stores a record if there is enough space.  Returns the 
     * slot number for the new record, and -1 if not enough space.
     */
     short storeRecord(char *rec , short recLength);

    /**
     * Delete a record given its slot number and returns the
     * space available.
     */
    short deleteRecord(short slotNum);

     /**
     * Swaps records stored at given slot id's.Returns true if swaped,else false
     */ 
	bool swapRecord(short slotNum1,short slotNum2);
	
	/**
     * Display the contents of the block
     */ 
    void display();
	
	/**
	 * Gets the Total number of Records.
	 */
	short getTotNoRec();
	
	/**
	 * Gets the Total number of Slots.
	 */
	short getTotNoSlots();

};
#endif	//SP_H

