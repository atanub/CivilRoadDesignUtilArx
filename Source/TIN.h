//TIN.h

#ifndef _TIN_H
#define _TIN_H

#include "Structures.h"




class TIN
{
private:
	TIN() {}
	~TIN() {}

public:
	static int Triangulate(AcGePoint3dArray&, CTrianglePtrArray&);
private:
	static int CircumCircle(const double&, const double&, const double&, const double&, const double&, const double&, const double&, const double&, double&, double&, double&);
	static int TriangulateCore(const int& nv, AcGePoint3d*&, TRIANGLE*&, int*);
};



#endif //_TIN_H
