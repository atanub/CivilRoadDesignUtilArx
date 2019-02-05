// ReadPtData.cpp : implementation file
//
/////////////////////////////////////////////////////////////////////////////
	//ASCII TXT File format....
	
	//First Line for definition of file format
	//Line #1		<Integer> ->for file fmts are as follows
	//1. X, Y, Z
	//2. N, E, Z
	//3. Pt#, X, Y, Z
	//4. Pt#, N, E, Z
	
	//Second Line for separator type
	//Line #2		<Integer> ->for separator type->> 1 = space; 2 = comma; 3 = semicolon;
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ReadPtData.h"

static BOOL ReadFileFmt(CStdioFile&, int&, int&, int&);
static BOOL ReadNextRecord(CStdioFile&, POINTDATA&, const int&, const int&, int&);
static BOOL FetchPointData(const CString&, double&, double&, double&, const int&);
static BOOL FetchPointDesc(const CString&, CString&, CString&, const int&);
static int FindSimmilarPoint(const POINTDATA&, const CPointDataPtrArray&);

static char szCommentMk[] = "//";
/////////////////////////////////////////////////////////////////////////////
// for Importing points within drawing 
BOOL ReadPointsFromTXTFile(CPointDataPtrArray& Array)
{
	static char BASED_CODE szFilter[] = "Text Files (*.txt)|*.txt||";
	CString strFile;
	
	if(Array.GetSize() != 0)
		return FALSE;
	
	CFileDialog Dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter);
	if(Dlg.DoModal() != IDOK)
		return FALSE;
	strFile = Dlg.GetPathName();
	//Start Reading
	{//for ASCII Txt file
		CStdioFile FileInp;
		CFileException e;

		if(!FileInp.Open(strFile, CFile::modeRead|CFile::typeText, &e))
		{
			CString strMsg;

			strMsg.Format("File \"%s\" could not be opened (Error# = %d)", strFile, e.m_cause);
			AfxMessageBox(strMsg, MB_ICONSTOP);
			return FALSE;
		}
		POINTDATA Rec;
		POINTDATA* pRec;
		int iLineCount, iFileType, iSepType;
		
		iLineCount = 0;
		if(!ReadFileFmt(FileInp, iFileType, iSepType, iLineCount))
		{
			AfxMessageBox("ERROR: Failed to read input file <1>", MB_ICONSTOP);
			return FALSE;
		}
		for(;;)
		{
			if(!ReadNextRecord(FileInp, Rec, iFileType, iSepType, iLineCount))
				break;
			if(FindSimmilarPoint(Rec, Array) < 0)
			{
				pRec = new POINTDATA;
				*pRec = Rec;
				Array.Add(pRec);
			}
			else
			{
				CString strMsg;

				strMsg.Format("Failed to import duplicate point (X=%f, Y=%f, Z=%f), line #%d", Rec.pt.x, Rec.pt.y, Rec.pt.z, iLineCount);
				ads_printf("\nERROR: %s", strMsg);
				continue;
				AfxMessageBox(strMsg, MB_ICONSTOP);
				{//Deallocate...
					int i;
					POINTDATA* pRec;
					for(i = 0; i < Array.GetSize(); i++)
					{
						pRec = Array[i];
						delete pRec;
					}
					if(Array.GetSize() > 0)
						Array.RemoveAll();
				}
				return FALSE;
			}
		}
	}
	return TRUE;
}
static BOOL ReadFileFmt(CStdioFile& FileInp, int& iFileType, int& iSepType, int& iLineCount)
{
	//ASCII TXT File format....
	//1. X, Y, Z
	//2. N, E, Z
	//3. Pt#, X, Y, Z
	//4. Pt#, N, E, Z
	//First Line for definition of file format
	//Line #1		<Integer> ->for above file fmts 
	//Line #2		<Integer> ->for separator type->> 1 = space; 2 = comma; 3 = semicolon;
	CString strLine;

	iFileType = iSepType = -1;
	for(;;)
	{
		if(!FileInp.ReadString(strLine))
		{
			AfxMessageBox("ERROR: Failed to read input file, data format is incorrect", MB_ICONSTOP);
			return FALSE;
		}
		iLineCount++;
		ads_printf("\rPlease wait... Reading line #%d", iLineCount);
		strLine.TrimLeft();
		strLine.TrimRight();
		if((strLine.Find(szCommentMk) == 0) || (strlen(strLine) <= 0))
		{
			continue;//Commented line
		}
		else
		{
			if(iFileType > 0)
			{//'file format type' read successfully now get the 'separator type'
				iSepType = atoi(strLine);
				if(iSepType <= 0)
				{
					AfxMessageBox("ERROR: Invalid separator type identifier", MB_ICONSTOP);
					return FALSE;//Invalid separator type
				}
				return TRUE;
			}
			iFileType = atoi(strLine);
			if(iFileType <= 0)
			{
				AfxMessageBox("ERROR: Invalid file format type identifier", MB_ICONSTOP);
				return FALSE;//Invalid file format type
			}
		}
	}
	return FALSE;
}
static BOOL ReadNextRecord(CStdioFile& FileInp, POINTDATA& Rec, const int& iFileType, const int& iSepType, int& iLineCount)
{
	//ASCII TXT File format....
	//1. X, Y, Z
	//2. N, E, Z
	//3. Pt#, X, Y, Z
	//4. Pt#, N, E, Z
	//First Line for definition of file format
	//Line #1		<Integer> ->for above file fmts 
	//Line #2		<Integer> ->for separator type->> 1 = space; 2 = comma; 3 = semicolon;
	CString strLine;

	for(;;)
	{
		if(!FileInp.ReadString(strLine))
			return FALSE;//Invalid file
		
		iLineCount++;
		ads_printf("\rPlease wait... Reading line #%d", iLineCount);
		strLine.TrimLeft();
		strLine.TrimRight();
		if((strLine.Find(szCommentMk) == 0) || (strlen(strLine) <= 0))
		{
			continue;//Commented line
		}
		else
		{
			CString strDesc;
			double d1, d2, d3;
			
			strcpy(Rec.szDesc, "");
			if((iFileType == 3) || (iFileType == 4))
			{//Get Point Description....
				if(!FetchPointDesc(strLine, strDesc, strLine, iSepType))
				{
					AfxMessageBox("ERROR: Failed to extract point description", MB_ICONSTOP);
					return FALSE;
				}
				
				if(strlen(strDesc) >= _MAX_PATH)
					strDesc = strDesc.Mid(0, _MAX_PATH - 2);//Ignore string lengthier than '_MAX_PATH'
				
				strcpy(Rec.szDesc, strDesc);
			}
			//
			if(!FetchPointData(strLine, d1, d2, d3, iSepType))
				return FALSE;
			switch(iFileType)
			{
			case 1://X, Y, Z
			case 3://Pt#, X, Y, Z
				Rec.pt.x = d1;
				Rec.pt.y = d2;
				Rec.pt.z = d3;
				return TRUE;
				break;
			case 2://N, E, Z
			case 4://Pt#, N, E, Z
				Rec.pt.x = d2;
				Rec.pt.y = d1;
				Rec.pt.z = d3;
				return TRUE;
			default:
				{//Invalid
					CString strMsg;
					
					strMsg.Format("ERROR: Invalid file format ID \"%d\"", iFileType);
					AfxMessageBox(strMsg, MB_ICONSTOP);
				}
				return FALSE;
			}
		}
	}
	return FALSE;
}
static BOOL FetchPointData(const CString& strLine, double& d1, double& d2, double& d3, const int& iSepType)
{
	CString strSeparator, strLineDummy, strTmp;
	int iIndex, iVarCount;
	
	switch(iSepType)
	{
	case 1: //space/tab;
		strSeparator = " \t";
		break;
	case 2: //comma;
		strSeparator = ",";
		break;
	case 3: //semicolon;
		strSeparator = ";";
		break;
	default:
		{//
			CString strMsg;
			
			strMsg.Format("ERROR: Invalid separator ID \"%d\"", iSepType);
			AfxMessageBox(strMsg, MB_ICONSTOP);
		}
		return FALSE;//Invalid separator ID
	}
	strLineDummy = strLine;
	iVarCount = 0;
	d1 = d2 = d3 = 0.0;
	for(;;)
	{
		iIndex = strLineDummy.FindOneOf(strSeparator);
		if(iIndex < 0)
		{//
			CString strMsg;
			
			strMsg.Format("ERROR: Invalid data line...Pl. check the following line\n\"%s\"", strLine);
			AfxMessageBox(strMsg, MB_ICONSTOP);
			return FALSE;//Abnormal data....
		}

		strTmp = strLineDummy.Mid(0, iIndex);
		strLineDummy = strLineDummy.Mid(iIndex + 1);
		strLineDummy.TrimLeft();
		strLineDummy.TrimRight();
		switch(iVarCount)
		{
			case 0:
				d1 = atof(strTmp);
				break;
			case 1:
				d2 = atof(strTmp);
				d3 = atof(strLineDummy);
				return TRUE;
			default:
				AfxMessageBox("ERROR: Abnormal data <9>", MB_ICONSTOP);
				return FALSE;
		}
		iVarCount++;
	}
	return FALSE;
}
static BOOL FetchPointDesc(const CString& strLine, CString& strDesc, CString& strRest, const int& iSepType)
{
	CString strSeparator, strLineDummy;
	int iIndex;
	
	switch(iSepType)
	{
	case 1: //space/tab;
		strSeparator = " \t";
		break;
	case 2: //comma;
		strSeparator = ",";
		break;
	case 3: //semicolon;
		strSeparator = ";";
		break;
	default:
		{//Invalid
			CString strMsg;
			strMsg.Format("ERROR: Invalid separator ID \"%d\"", iSepType);
			AfxMessageBox(strMsg, MB_ICONSTOP);
		}
		return FALSE;//Invalid separator ID
	}
	strLineDummy = strLine;
	strDesc = "";
	for(;;)
	{
		iIndex = strLineDummy.FindOneOf(strSeparator);
		if(iIndex < 0)
		{
			CString strMsg;
			strMsg.Format("ERROR: Invalid data line...Pl. check the following line\n\"%s\"", strLine);
			AfxMessageBox(strMsg, MB_ICONSTOP);
			return FALSE;//Abnormal data....
		}

		strDesc = strLineDummy.Mid(0, iIndex);
		strDesc.TrimLeft();
		strDesc.TrimRight();
		strRest = strLineDummy.Mid(iIndex + 1);
		return TRUE;
	}
	return FALSE;
}
static int FindSimmilarPoint(const POINTDATA& Rec, const CPointDataPtrArray& Array)
{
	int i;
	for(i = 0; i < Array.GetSize(); i++)
	{
		if((Rec.pt.x == (Array[i])->pt.x) && (Rec.pt.y == (Array[i])->pt.y))
			return i;
	}
	return -1;
}
