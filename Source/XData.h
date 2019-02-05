//XData.h
//***************************************************************************/
#ifndef _XDATA_H

#include <acgi.h>


void PflAndCSecnXDQuery();
void ModifyPflAndCSecnAnn();



//Field data.......
class CXDataPflAndCS
{
public:
	CXDataPflAndCS();
	~CXDataPflAndCS();

	static BOOL RegApp(const char*);
	static BOOL AttachXData(ads_name, const char*, const double&);
	static BOOL InquireXData(ads_name, const CString&, double&);
	static BOOL ModifyXData(ads_name, const CString&, const double&);
	
	static BOOL ModifyText(ads_name, const char*);
	static void PrintResBuf(struct resbuf*);
private:

public:
	const static char* m_pszAppName;
	const static char* m_pszChgTag;
	const static char* m_pszEastTag;
	const static char* m_pszNorthTag;
	const static char* m_pszElevTag;
};

#define _XDATA_H
#endif /*  _XDATA_H */