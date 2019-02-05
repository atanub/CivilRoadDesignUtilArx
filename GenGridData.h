// GenGridData.h
/////////////////////////////////////////////////////////
#ifndef _GENGRIDDATA_H_
#define _GENGRIDDATA_H_

#include "Structures.h"

class CGenGridPoints
{
private:
	CGenGridPoints() {}
	~CGenGridPoints() {}

public:
	static void GenGridPoints();
private:
	static BOOL GenGridData(const DWGPARAMS&);
	static BOOL GetSelClosedPLineVertexes(AcGePoint3dArray&, BOOL* = 0L);
	static BOOL DrawGrid(const double&, const double&, const double&, const double&, const double&, const double&, const CString&);
	static void CalculateRectXtent(const AcGePoint3dArray&, const double&, const double&, double&, double&, double&, double&);
};


#endif // _GENGRIDDATA_H_

