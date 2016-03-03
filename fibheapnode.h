#ifndef FIBHEAPNODE_H
#define FIBHEAPNODE_H
#include "typedef.h"

class FibHeapNode
{
friend class FibHeap;

    FibHeapNode *Left, *Right, *Parent, *Child;
    short Degree, Mark, NegInfinityFlag;

public:
    pixel* pix;

    FibHeapNode(pixel* pix);

    virtual ~FibHeapNode();

    void operator =(FibHeapNode& RHS);

    int operator ==(FibHeapNode& RHS);

    int operator <(FibHeapNode& RHS);

    virtual void Print();


protected:

    int  FHN_Cmp(FibHeapNode& RHS);
    void FHN_Assign(FibHeapNode& RHS);

};

#endif // FIBHEAPNODE_H
