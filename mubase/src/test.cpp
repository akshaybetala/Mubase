#include "dm.h"
#include "sm.h"
#include "rm.h"
#include <cstdlib>
#include "global.h"
#include <iostream>
using namespace std;

int main ()  {
	const char* dbName = "mydb.txt";
	DiskManager::createDB(dbName, 4000);
 	StorageManager::initDB(dbName);
	
	Schema schema1;
	schema1.addAttribute(INTEGER, (char*)"a");
	schema1.addAttribute(INTEGER, (char*)"b");
	
	const char* relName = "Rel1";
	Relation rel(dbName, relName, true, &schema1);
//	cout << "here" << endl;
/**	
	Relation rel1(dbName, relName, false, NULL);
//	cout << "CHECK2"<<endl;
	Schema *s = rel1.getSchema();

	int numAttrs = s->getNumOfAttrs();
	cout << "numAttrs " << numAttrs <<endl;
	for (int i = 0; i < numAttrs; i++)
	{
		cout << s->getAttrType(i)<< "  " << s->getAttrName(i)<<endl;
	}
*/
	
	int i=1000;
	int a;
	while(i>0){
		a = rand();
		char* r1 = (char*)malloc(2*SIZE_INTEGER);
		memcpy(r1+0, &a, SIZE_INTEGER);
		memcpy(r1 + SIZE_INTEGER, &a, SIZE_INTEGER);
		Record rec1(r1, &schema1, 2*SIZE_INTEGER);
		rel.addRecord(rec1);
		free(r1);
		i--;
	}
	char* r1 = (char*)malloc(2*SIZE_INTEGER);
	memcpy(r1+0, &(a=4), SIZE_INTEGER);
	memcpy(r1 + SIZE_INTEGER, &(a=400), SIZE_INTEGER);
	Record rec1(r1, &schema1, 2*SIZE_INTEGER);
	rel.addRecord(rec1);
	free(r1);
	r1 = (char*)malloc(2*SIZE_INTEGER);
	memcpy(r1+0, &(a=4), SIZE_INTEGER);
	memcpy(r1 + SIZE_INTEGER, &(a=500), SIZE_INTEGER);
	Record rec2(r1, &schema1, 2*SIZE_INTEGER);
	rel.addRecord(rec2);
	
	
	
	RelBlockIterator relIterartor(dbName,relName);
	i=0;
	relIterartor.open();
	i= relIterartor.next();
	cout<<i;
	while(i!=-1){
		cout<<"XX"<<i<<"YY"<<endl;
		BlockRecIterator block(i,&schema1);
		block.open();
		while(block.next()!=NULL)
			cout<<i << "   block" <<endl; 
		i= relIterartor.next();
	}
	cout<<"end"<<endl;
	
	
	Equal eq((char*)"a", (char*)"4");
	PredIterator pIt(dbName, relName, &eq, NULL, 0);
	pIt.open();
	Record* r;
	while((r=pIt.next()) != NULL)
	{cout << "found"<<endl;
		cout << r->getInt(0) << " " << r->getInt(1)<<endl;
	}
	pIt.close();
	
	
	
	return 0;
}

