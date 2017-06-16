// testallocator.cpp : 定义控制台应用程序的入口点。
//

#include <vector>
#include <ctime>
#include <iostream>
#include <cstdlib>
#include "Allocator.h"

#define INT 1
#define FLOAT 2
#define DOUBLE 3
#define CLASS  4

class vecWrapper
{
public:
	vecWrapper() {
		m_pVec = NULL;
		m_type = INT;
	}
	virtual ~vecWrapper()	{
	}
public:
	void setPointer(int type, void *pVec) { m_type = type; m_pVec = pVec; }
	virtual void visit(int index) = 0;
	virtual int size() = 0;
	virtual void resize(int size) = 0;
	virtual bool checkElement(int index, void *value) = 0;
	virtual void setElement(int idex, void *value) = 0;
protected: 
	int m_type;
	void *m_pVec;
};

template<class T>
class vecWrapperT : public vecWrapper
{
public:
	vecWrapperT(int type, std::vector<T, Allocator<T> > *pVec) 
	{ 
		m_type = type;
		m_pVec = pVec;
	}
	virtual ~vecWrapperT() {
		if (m_pVec)
			delete ((std::vector<T, Allocator<T> > *)m_pVec);
	}
public:
	virtual void visit(int index)
	{
		T temp = (*(std::vector<T, Allocator<T> > *)m_pVec)[index];
	}
	virtual int size()
	{
		return ((std::vector<T, Allocator<T> > *)m_pVec)->size();
	}
	virtual void resize(int size)
	{
		((std::vector<T, Allocator<T> > *)m_pVec)->resize(size);
	}
	virtual bool checkElement(int index, void *pValue)
	{
		T temp = (*(std::vector<T, Allocator<T> > *)m_pVec)[index];
		if (temp == (*((T *)pValue)))
			return true;
		else
			return false;
	}

	virtual void setElement(int index, void *value)
	{
		(*(std::vector<T, Allocator<T> > *)m_pVec)[index] = *((T *)value);
	}
};

class myObject
{
public:
	myObject() : m_X(0), m_Y(0) {}
	myObject(int t1, int t2) :m_X(t1), m_Y(t2) {}
	myObject(const myObject &rhs) { m_X = rhs.m_X; m_Y = rhs.m_Y;}
	bool operator == (const myObject &rhs)
	{
		if ((rhs.m_X == m_X) && (rhs.m_Y == m_Y))
			return true;
		else
			return false;
	}
protected:
	int m_X;
	int m_Y;
};

#define TESTSIZE 10000

int main()
{
    std::clock_t start;
    double duration;
    Allocator<int> a;
    for(int i = 0; i < LAYER; i++)
        a.Mount(i);


    start = std::clock();

	vecWrapper **testVec;
	testVec = new vecWrapper*[TESTSIZE];

	int tIndex, tSize;
	//test allocator
	for (int i = 0; i < TESTSIZE - 4; i++)
	{
		tSize = (int)((float)rand()/(float)RAND_MAX * TESTSIZE);
		vecWrapperT<int> *pNewVec = new vecWrapperT<int>(INT, new std::vector<int, Allocator<int> >(tSize));
		testVec[i] = (vecWrapper *)pNewVec;
	}


	for (int i = 0; i < 4; i++)
	{
		tSize = (int)((float)rand() / (float)RAND_MAX * TESTSIZE);
		vecWrapperT<myObject> *pNewVec = new vecWrapperT<myObject>(CLASS, new std::vector<myObject, Allocator<myObject> >(tSize));
		testVec[TESTSIZE - 4 + i] = (vecWrapper *)pNewVec;
	}
	
	//test resize
	for (int i = 0; i < 100; i++)
	{
		tIndex = (int)((float)rand() / (float)RAND_MAX * TESTSIZE);
		tSize = (int)((float)rand() / (float)RAND_MAX * TESTSIZE);
		testVec[tIndex]->resize(tSize);
	}

	//test assignment
	tIndex = (int)((float)rand() / (float)RAND_MAX * (TESTSIZE - 4 - 1));
	int tIntValue = 10;
	testVec[tIndex]->setElement(testVec[tIndex]->size()/2, &tIntValue);
	if (!testVec[tIndex]->checkElement(testVec[tIndex]->size()/ 2, &tIntValue))
		std::cout<< "incorrect assignment in vector %d\n" << tIndex << std::endl;

	tIndex = TESTSIZE - 4 + 3;
	myObject tObj(11,15);
	testVec[tIndex]->setElement(testVec[tIndex]->size()/2, &tObj);
	if (!testVec[tIndex]->checkElement(testVec[tIndex]->size()/2, &tObj))
		std::cout << "incorrect assignment in vector %d\n" << tIndex << std::endl;
	
	for (int i = 0; i < TESTSIZE; i++)
		delete testVec[i];

	delete []testVec;
	duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
    std::cout<<"printf: "<< duration <<'\n';
    for(int i = 0; i < LAYER; i++)
        a.Free(i);

    return 0;
}


