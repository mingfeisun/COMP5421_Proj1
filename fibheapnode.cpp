#include "fibheapnode.h"
#include "typedef.h"
#include <string.h>
#include <stdlib.h>
#include <iostream>

//***************************************************************************
//=========================================================
// FibHeapNode Constructor
//=========================================================
//***************************************************************************

FibHeapNode::FibHeapNode(pixel* pix)
{
     Left = Right = Parent = Child = NULL;
     Degree = Mark = NegInfinityFlag = 0;
     this->pix = pix;
}

//=========================================================
// FibHeapNode Destructor
//
// Body is empty, but declaration is required in order to
// force virtual.  This will ensure that FibHeap class
// calls derived class destructors.
//=========================================================

FibHeapNode::~FibHeapNode()
{
}

//=========================================================
// FHN_Assign()
//
// To be used as first step of an assignment operator in a
// derived class.  The derived class will handle assignment
// of key value, and this function handles copy of the
// NegInfinityFlag (which overrides the key value if it is
// set).
//=========================================================

void FibHeapNode::FHN_Assign(FibHeapNode& RHS)
{
     NegInfinityFlag = RHS.NegInfinityFlag;
}

//=========================================================
// FHN_Cmp()
//
// To be used as the first step of ALL comparators in a
// derived class.
//
// Compares the relative state of the two neg. infinity
// flags.  Note that 'this' is the left hand side.  If
// LHS neg. infinity is set, then it will be less than (-1)
// the RHS unless RHS neg. infinity flag is also set.
// Only if function returns 0 should the key comparison
// defined in the derived class be performed, e.g.
//
// For ==, if zero returned, then compare keys
//	   if non-zero X returned, then return 0
// For <,  if zero returned, then compare keys
//         if non-zero X returned, then return X<0?1:0
// For >,  if zero returned, then compare keys
//         if non-zero X returned, then return X>0?1:0
//=========================================================

int  FibHeapNode::FHN_Cmp(FibHeapNode& RHS)
{
     if (NegInfinityFlag)
     return RHS.NegInfinityFlag ? 0 : -1;
     return RHS.NegInfinityFlag ? 1 : 0;
}

//========================================================================
// We do, on occasion, compare and assign objects of type FibHeapNode, but
// only when the NegInfinityFlag is set.  See for example FibHeap::Delete().
//
// Also, these functions exemplify what a derived class should do.
//========================================================================

void FibHeapNode::operator =(FibHeapNode& RHS)
{
     FHN_Assign(RHS);
     // Key assignment goes here in derived classes
     this->Left = RHS.Left; this->Right = RHS.Right;
     this->Parent = RHS.Parent; this->Child = RHS.Child;
     this->Degree = RHS.Degree; this->Mark = RHS.Mark;

     this->pix = (pixel*)malloc(sizeof(pixel));
     this->pix->col = RHS.pix->col;
     this->pix->row = RHS.pix->row;
     this->pix->steps = RHS.pix->steps;
     this->pix->t_cost = RHS.pix->t_cost;
     this->pix->state = RHS.pix->state;
     this->pix->pre = RHS.pix->pre;
     memcpy(this->pix->cost, RHS.pix->cost, 8*sizeof(double));
}

int  FibHeapNode::operator ==(FibHeapNode& RHS)
{
     if (FHN_Cmp(RHS)) return 0;
     // Key compare goes here in derived classes
     return (this->pix->t_cost == (&RHS)->pix->t_cost);
}

int  FibHeapNode::operator <(FibHeapNode& RHS)
{
    int X;

    if ((X=FHN_Cmp(RHS)) != 0)
        return X < 0 ? 1 : 0;
    // Key compare goes here in derived classes
    return (this->pix->t_cost < (&RHS)->pix->t_cost) ? 1 : 0;
}

//=========================================================
// Print()
//=========================================================

void FibHeapNode::Print()
{
     if (NegInfinityFlag)
     std::cout << "-inf.";
}
