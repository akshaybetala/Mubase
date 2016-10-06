#include "iter.h"


class Sort : public RIterator {
    private:
	//
	char *dbName;
	char *obName;
	int attrIndex;
	public:
    virtual void open();
    virtual Record next();
    virtual void close();
	void sortBlock(int frameId);

};


