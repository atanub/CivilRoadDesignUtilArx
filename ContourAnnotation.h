//ContourAnnotation.h
////////////////////////////////////////////////////////////////
#ifndef _CONTOURANNOTATION_H
#define _CONTOURANNOTATION_H
#include "Structures.h"


#endif // _CONTOURANNOTATION_H
class CContourAnnotation
{
private:
	CContourAnnotation() {}
	~CContourAnnotation() {}

public:
	
	static void SetContourAnnotationParam();
	static void AnnotateAllContours();
	static void AnnotateSelContours();
	static void ConvertSelPLinesAsContour();
	static void InstantAnnotatation();
	
	static BOOL AnnotateContourLines(const DWGPARAMS&);
private:
	static BOOL AnnotateContourLinesEx(const DWGPARAMS&);
	static BOOL AnnContLinesManually(const DWGPARAMS&);
	static BOOL ConvertToContourEntity(const DWGPARAMS&);
	static BOOL InstantAnnotation(const DWGPARAMS&, ANNTNPARAMS&);
	//
	static BOOL InsAnnText(const AcGePoint3dArray&, const AcGePoint3dArray&, const AcGeDoubleArray&, const double&, const char*);
	//Auto Annotation...........
	////////////////////////////////////////////////////////////////////////////////////////
	static BOOL AnnotateLine(AcDbLine*, const DWGPARAMS&, AcGePoint3dArray&);
	static void DrawLineFragments(const char*, const char*, const AcGePoint3dArray&);
	static AcDbObject* GetEntityForAnnotation(const DWGPARAMS&, const char*, AcGePoint3d* = 0L, ANNTNPARAMS* = 0L);

	static BOOL InstantAnnotationForLine(const DWGPARAMS&, ANNTNPARAMS&, AcDbObject*, const AcGePoint3d&);
	//static BOOL InstantAnnotationForPLine(const DWGPARAMS&, const ANNTNPARAMS&, AcDbObject*, const AcGePoint3d&, AcDbObjectIdArray* = 0L);
	static BOOL InsertElevTextOnPLine(const DWGPARAMS&, const ANNTNPARAMS&, AcDbObject*, const AcGePoint3d&);
	static BOOL IsPoint2dOnLine(const AcGePoint3d&, const AcGePoint3d&, const AcGePoint3d&, double* = 0L, double* = 0L);
	static BOOL AddXDataForLastAddedContourEnt(const char*, const double&, AcDbObjectIdArray* = 0L);
	//static BOOL AddXData(const char*, const double&, const AcDbObjectId&);

	static int IsLineOrPLine(const AcDbObjectId&, AcDbObject*&);
	static BOOL GetPLinesForConversion(AcDbObjectIdArray&);

	static void DividePolyline(const AcGePoint3dArray&, const double&, AcGePoint3dArray&, AcGeDoubleArray&);
	static BOOL CalcInsertionPtNAngle(const double&, const AcGePoint3dArray&, AcGePoint3d&, double&);

	static BOOL GetAnnotationPath(AcGePoint3dArray& Array);
	static BOOL GetAllContourEntities(AcDbObjectIdArray& Array, const DWGPARAMS& DwgParams);
	static BOOL AnnotatePolylineManually(AcDbObject* pEntity, const DWGPARAMS& DwgParams, const ANNTNPARAMS& AnnParam, const AcGePoint3dArray& PtArray, AcDbObjectIdArray& /*NewEntsArray*/);
	static BOOL AnnotateLineManually(const AcDbLine* pEntity, const DWGPARAMS& DwgParams, const ANNTNPARAMS& AnnParam, const AcGePoint3dArray& PtArray, AcDbObjectIdArray& NewEntsArray);
};
