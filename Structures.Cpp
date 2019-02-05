//Structures.cpp
#include "Stdafx.h"
#include "Structures.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <adslib.h>
#include <adsdlg.h>

#define EPS	0.000001

////////////////////////////////////////////////////////////////////////////////////
//CVersionOfStructs
const int	CVersionOfStructs::m_iVersionXSecData		= 50;
const int	CVersionOfStructs::m_iVersionProfileData	= 50;
const int	CVersionOfStructs::m_iVersionAnnotationData = 50;
const int	CVersionOfStructs::m_iVersionDwgParamData	= 50;

////////////////////////////////////////////////////////////////////////////////////
//
int CompareAcPt3D_X(const void *pArg1, const void *pArg2)
{
   if(((AcGePoint3d*)pArg1)->x > ((AcGePoint3d*)pArg2)->x)
	   return 1;
   else if(((AcGePoint3d*)pArg1)->x < ((AcGePoint3d*)pArg2)->x)
	   return -1;
   return 0;
}
int CompareAcPt3D_Y(const void *pArg1, const void *pArg2)
{
   if(((AcGePoint3d*)pArg1)->y > ((AcGePoint3d*)pArg2)->y)
	   return 1;
   else if(((AcGePoint3d*)pArg1)->y < ((AcGePoint3d*)pArg2)->y)
	   return -1;
   return 0;
}
int CompareAcPt3D_Z(const void *pArg1, const void *pArg2)
{
   if(((AcGePoint3d*)pArg1)->z > ((AcGePoint3d*)pArg2)->z)
	   return 1;
   else if(((AcGePoint3d*)pArg1)->z < ((AcGePoint3d*)pArg2)->z)
	   return -1;
   return 0;
}
void SortAcPoint3DArray(AcGePoint3dArray& Array, const short& iWRT)
{
	switch(iWRT)
	{
	case 0://X
		qsort((void*)(Array.asArrayPtr()), (size_t)(Array.length()), sizeof(AcGePoint3d), CompareAcPt3D_X);
		return;
	case 1://Y
		qsort((void*)(Array.asArrayPtr()), (size_t)(Array.length()), sizeof(AcGePoint3d), CompareAcPt3D_Y);
		return;
	case 2://Z
		qsort((void*)(Array.asArrayPtr()), (size_t)(Array.length()), sizeof(AcGePoint3d), CompareAcPt3D_Z);
		return;
	default:
		return;
	}
}

