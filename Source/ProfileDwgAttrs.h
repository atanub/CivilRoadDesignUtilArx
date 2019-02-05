//ProfileDwgAttrs.h
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _PROFILEDWGATTRS_H_
#define _PROFILEDWGATTRS_H_

#include "Structures.h"

class CProfileDwgAttrs;
class CThePointsOnAlignment;
//
//CProfileDwgAttrs
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CProfileDwgAttrs
{
public:
	CProfileDwgAttrs();
	CProfileDwgAttrs(const PROFILEDATA&, const BOOL);

	virtual ~CProfileDwgAttrs();
public:
	
	virtual int			GetRowCount() const { return m_arrayRowTags.GetSize();}
	virtual const char* GetLabelAtRow(int iIndex) const { assert((iIndex >= 0) && (iIndex < m_arrayRowTags.GetSize())); return (LPCSTR)(m_arrayRowTags[iIndex]);}
	virtual AcGePoint3d GetYForTxtAnnAtDataRow(int iRowIndex) const;
	void				AddLabel(const char* pszLabel) { m_arrayRowTags.Add(pszLabel);}

	const char*			GetDatumTag() const {return m_pszDatumTag;}
	const char*			GetProfileTag() const {return m_pszProfileTag;}

	const char*			GetLyrLyrAnnotation() const {return m_pszLyrAnnotation;}
	const char*			GetLyrTopSurface() const {return m_pszLyrTopSurface;}
	const char*			GetLyrOrdinate() const {return m_pszLyrOrdinate;}
	const char*			GetLyrAxis() const {return m_pszLyrAxis;}
	
	double		GetXScale() const {return m_dXScale;}
	double		GetYScale() const {return m_dYScale;}
	double		GetDatum() const {return m_dDatum;}
	double		GetStartChg() const {return m_dStartChg;}
	double		GetSexonIntervalMajor() const {return m_dSexonInterval;}
	double		GetSexonIntervalMinor() const {return m_dSexonIntervalMinor;}
	double		GetNumAnnSize() const {return m_dNumAnnSize;}
	double		GetRowAnnSize() const {return m_dRowAnnSize;}
	double		GetPflLabelSize() const {return m_dPflLabelSize;}
	double		GetRowHeight() const { return m_dRowHt;}
	double		GetRowBorderHeight() const { return m_dRowBorderThk;}
public:
	virtual AcGePoint3d DrawAxis(const AcGePoint3d&, const double&) const;

protected:
	double	m_dXScale;
	double	m_dYScale;
	
	double	m_dDatum;
	double	m_dStartChg;
	double	m_dSexonInterval;
	double	m_dSexonIntervalMinor;

	double	m_dNumAnnSize;
	double	m_dRowAnnSize;
	double	m_dPflLabelSize;
	double	m_dRowHt;
	double	m_dRowBorderThk;

	char*	m_pszDatumTag;
	char*	m_pszProfileTag;
	
	char*	m_pszLyrAnnotation;
	char*	m_pszLyrTopSurface;
	char*	m_pszLyrOrdinate;
	char*	m_pszLyrAxis;
	
	CStringArray	m_arrayRowTags;
};
//
//COrdinateData
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class COrdinateData
{
public:
	typedef enum {NoAnn = 0, OrdAnn = 1, TextAnn = 2, BlockAnn = 4} AnnBitCode;
public:
	COrdinateData();
	virtual ~COrdinateData();
public:
	AcGePoint3d Draw(const CProfileDwgAttrs*);
protected:
	AnnBitCode		m_iAnnBitCode;
	double			m_fXOrd;			//After applying scale...The distance from Axis Origin
	double			m_fYOrd;			//After applying scale...The distance Above Axis Origin
	CStringArray	m_arrayAttribs;		//Valuse of Elev/Northing/Easting...etc.
	CStringArray	m_arrayExtraAnnData;//Text/Block Name...may contain both
};
//
//COrdinateCollection
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class COrdinateCollection
{
public:
	COrdinateCollection();
	virtual ~COrdinateCollection();
public:
	AcGePoint3d Draw(const AcGePoint3d&, const CProfileDwgAttrs*);
protected:
	CTypedPtrArray<CPtrArray, COrdinateData*> m_arrayOrds;
};

//
//CProfileDwgGenerator
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CProfileDwgGenerator
{
public:
	CProfileDwgGenerator();
	virtual ~CProfileDwgGenerator();
public:
	void Draw(const AcGePoint3d&, const CProfileDwgAttrs*, const CThePointsOnAlignment*);
protected:
	COrdinateCollection		m_collectionOrds;
};

#endif //_PROFILEDWGATTRS_H_
