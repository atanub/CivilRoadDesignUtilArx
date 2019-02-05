// CutFillCalculatorForXSection.h : header file
////////////////////////////////////////////////////////////////
#ifndef _CUTFILLCALCULATORFORXSECTION_H_
#define _CUTFILLCALCULATORFORXSECTION_H_


class CCutFillCalculatorForXSection  
{
public:
	CCutFillCalculatorForXSection(const AcGePoint3dArray& arrayExistingLevels, const AcGePoint3dArray& arrayProposedLevels);
	~CCutFillCalculatorForXSection();

protected:
	BOOL _Calculate();
	int _FindIntersectionPoint(int, const AcGePoint3d& ptStartParam, const AcGePoint3d& ptEndParam, AcGePoint3d& ptResultParam);

protected:
	const AcGePoint3dArray	 m_arrayExistingLevels;
	const AcGePoint3dArray	 m_arrayProposedLevels;
#ifdef _DEBUG
public:
	static void Test();
protected:
	static BOOL GetPLineSelection(const char* pszPrompt, AcDbObject*& pObj);
#endif//_DEBUG
};

/////////////////////////////////////////////////////////////////////////////
#endif //_CUTFILLCALCULATORFORXSECTION_H_

