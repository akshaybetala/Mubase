#ifndef RM_H

#define RM_H

#include "global.h"
#include "iter.h"
#include <cstring>
#include <cstdlib>
using namespace std;
class SlottedPage;
class BPlusTree;
class TreeNode;
class BufferManager;
#include <iostream>

class Schema {
    int numAttrs;
    int attrType[MAX_ATTRS];
    char attrName[MAX_ATTRS][SIZE_ATTR];	// assume there is no '0' in attribute name
public:
	Schema();
	Schema(const char* dbName, int schemaBlockId, short slot);
	Schema(char* schema, int numAttrs);
	bool addAttribute(int attrType, char* attrName);

	int getString(char* &schema); //Format: numAttrs,datatype,colname,...
	int getNumOfAttrs(){return numAttrs;}
	int getAttrType(int attrIndex) {return attrType[attrIndex];}
	char* getAttrName(int attrIndex) {return attrName[attrIndex];}
	int getAttrType(char* attrName);
	bool operator==(const Schema &schema);
};

class Record {
    char *recBytes;
    Schema *schema;
	int length;
	char* getAttrStarting(int attrIndex);
public: 
    Record(char *recBytes, Schema *schema, int length=-1);
	Schema* getSchema(){return schema;}
	char* getBytes(){return recBytes;}
	int getLength(){return length;}
	
    int getInt(int attrIndex);
    char *getString(int attrIndex);
    float getFloat(int attrIndex);
	int compareAttr(Record record, int attrIndex);
	bool lessThan(Record record,int attrIndex);
    bool greaterThan(Record record,int attrIndex);
    bool equalTo(Record record,int attrIndex);
};

struct RecID{
	int blockId;
	short slot;
};

// Iterates a block record by record
class BlockRecIterator : public RIterator  {
	SlottedPage* sp;
	BufferManager* bm;
	short TotalNumOfSlots;
	short SlotCount;
	int BlockId;
	char* block;
	bool isOpen;
	Schema* RecSchema;
	char* dbName;
	char* relName;
public:
	//Constructor
	BlockRecIterator(int blockId, Schema* s);
	BlockRecIterator(char *block, Schema* s);
    // Opens the iterator, initializing its state
    void open();
    // Returns the next record, NULL if no more records.
    Record* next();
    // Closes the iterator
    void close();
};

// Iterates a relation block by block
class RelBlockIterator {
	int nextBlockId;
	const char* dbName;
	const char* relName;
	BufferManager* bm;
	bool isOpen;
public:
	//Constructor
    RelBlockIterator(const char *dbName, const char *relName);
	// Open the iterator, initializing its state
    void open();
    // Returns the next record, NULL if no more records.
    int next();
    // Close the iterator
    void close();
};

// Iterator for a relation record by record
class BaseRelIterator : public RIterator  {
private:
	const char* dbName;
	const char* relName;
	bool isOpen;
	RelBlockIterator *BlockIterator;
	BlockRecIterator *RecIterator;
	Schema* RecSchema;
public:
	//Constructor
    BaseRelIterator(const char *dbName, const char *relName);
    // Opens the iterator, initializing its state
    void open();
    // Returns the next record, NULL if no more records.
    Record* next();
    // Closes the iterator
    void close();
};

// Predicate Class Definition
class Predicate {
public:
	bool useIndexIfExists;						// Flag to be set if predicate is simple
	virtual bool evaluate(Record rc) = 0;		// pure virtual function
	virtual char* getLeftVal(){}
	virtual char* getRightVal(){}
};

class AndOp:public Predicate   {	// And Op. Pred Class Definition
private:
	Predicate* lx;					// Pointer to Left Predicate 
	Predicate* rx;					// Pointer to Right Predicate
public:
	AndOp(Predicate* l, Predicate* r)	{	// Constructor
		lx = l;
		rx = r;	
	}
	bool evaluate(Record rc)   {		// Evaluates the record with the predicate.
		return (lx->evaluate(rc) && rx->evaluate(rc));	
	}
};
class OrOp:public Predicate   {		// Or  Op. Pred Class Definition
private:
	Predicate* lx;					// Pointer to Left Predicate	
	Predicate* rx;					// Pointer to Right Predicate	
public:
	OrOp(Predicate* l, Predicate* r)   {	// Constructor
		lx = l;
		rx = r;
	}
	bool evaluate(Record rc)   {		// Evaluates the record with the predicate.
		return (lx->evaluate(rc) || rx->evaluate(rc));
	}
};
class NotOp:public Predicate   {	// Not Op. Pred Class Definition
private:
	Predicate* x;					// Pointer to Predicate
public:
	NotOp(Predicate* p)   {			// Constructor
		x = p;
	}
	bool evaluate(Record rc)   {		// Evaluates the record with the predicate.
		return !(x->evaluate(rc));
	}
};
class BaseCon:public Predicate   {	// Base Condition Pred Class Definition	
protected:
	char *l,*r;
public:	
	BaseCon(char* l1,char* r1)	{	// Constructor
		l = l1;
		r = r1;
	}
	void findColumn(char* s, Schema* schemaPtr,int& i);
	char* getLeftVal();
	char* getRightVal();
};

class Equal:public BaseCon   {
public:
	Equal(char* l1,char* r1) : BaseCon(l1,r1)  {}	// Constructor
	bool evaluate(Record rc);
};
class Greater:public BaseCon   {
public:
	Greater(char* l1,char* r1) : BaseCon(l1,r1) {}  // Constructor
	bool evaluate(Record rc);
};
class Less:public BaseCon   {
public:
	Less(char* l1,char* r1) : BaseCon(l1,r1)   {}
	bool evaluate(Record rc);
};	
class LessEq:public BaseCon   {
public:
	LessEq(char* l1,char* r1) : BaseCon(l1,r1)   {}
	bool evaluate(Record rc);
};	
class GreaterEq:public BaseCon   {
public:
	GreaterEq(char* l1,char* r1) : BaseCon(l1,r1)   {}
	bool evaluate(Record rc);
};	
class NotEq:public BaseCon   {
public:
	NotEq(char* l1,char* r1) : BaseCon(l1,r1)   {}
	bool evaluate(Record rc);
};	


// Iterator for a relation record by record with Predicate checking
class PredIterator : public RIterator   {

private:
	const char* dbName;
	const char* relName;
	BaseRelIterator* baseIterator;
	bool isOpen;
	Predicate* pred;
	BPlusTree* BPT;
	int keyVal;
	TreeNode *leafNode;
	Schema* RecSchema;
	bool IndexedKeyFound;

public:
	//Constructor
    PredIterator(const char *dbName, const char *relName, Predicate* pred, BPlusTree* BPT, int key);
    // Open the iterator, initializing its state
    void open();
    // Returns the next record which satisfies the predicate, NULL if no more records.
    Record* next();
    // Closes the iterator
    void close();
};

class Relation {
private:
    const char* dbName;
    const char* relName;
	Schema *schema;
	int firstBlockId;
	RecID schemaRecId, relRecId;
	RecID writeToObj(BufferManager* bm, int firstBlockId, 
			char* bytes, int length, bool isSysObj, 
			char* sysObjname);
public:
    Relation(const char *dbName, const char *relName, bool create, Schema *schema);
    RecID addRecord(Record rec);
    void deleteRecord(RecID recId);
	void deleteRelation();

	Schema* getSchema() {return schema;}
	int getFirstBlockId(){return firstBlockId;}

    RIterator *openScan() {
        return new BaseRelIterator(dbName, relName);
    }
	// Checks if index is available	
	BPlusTree* checkIndex(char* indexName);		

    // Predicated scan. Should make use of the B+ tree index
    // when an index is available on the required column(s).
    RIterator *openScan(Predicate* p);
};

#endif	//RM_H


