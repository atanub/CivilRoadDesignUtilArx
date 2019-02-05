//Structures.h
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _STRUCTURES_H_
#define _STRUCTURES_H_

#include <afxtempl.h>
#include <acgi.h>

#define PI 3.1415926535897932384626433832795

typedef struct tagHOLEDEF
{
	char				szName[32];
	AcGePoint3dArray	arrayBoundary;//
} HOLEDEF;

typedef CTypedPtrArray<CPtrArray, HOLEDEF*> CHoleDefArray;

typedef struct tagBOUNDARYDEF
{
	AcGePoint3dArray	arrayBoundary;//
	CHoleDefArray		arrayHoleDef;//
} BOUNDARYDEF;


typedef struct tagTRIANGLE
{
	Adesk::Int32	p1,
					p2,
					p3;
} TRIANGLE;

typedef struct tagPointData
{
	AcGePoint3d	pt;
	char		szDesc[_MAX_PATH];

} POINTDATA;

typedef struct tagPointPair3D
{
	AcGePoint3d	pt01,
				pt02;
} POINTPAIR3D;

class EDGE
{
public:
	int p1,
		p2;
    EDGE() : p1(0), p2(0)
	{
	}
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//CVersionOfStructs - Version info of current application for the structures which are being saved in DWG file
class CVersionOfStructs
{
protected:
	CVersionOfStructs(){;}
	~CVersionOfStructs(){;}
public:
	const static int	m_iVersionXSecData;
	const static int	m_iVersionProfileData;
	const static int	m_iVersionAnnotationData;
	const static int	m_iVersionDwgParamData;
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
typedef struct tagDWGPARAMS
{
	double	fContourInterval;
	long	iContourColorBase;
	double	fTxtHeight,
			fSpotLevTxtHt, 
			fSpotLevRotAngle, 
			fSpotLevBlkRelScale, 
			fTxtSpaMF;

	char	szAppName[_MAX_PATH],
			szPointLyr[_MAX_PATH],
			szTriangleLyr[_MAX_PATH],
			szElevLyrPrefix[_MAX_PATH-1],
			chDotSeparator,
			szSpotLevLyr[_MAX_PATH],
			szSpotLevBlock[_MAX_PATH];
	
	char	szPointXDFlag[_MAX_PATH];
	BOOL	bIsRegistered;
} DWGPARAMS;

typedef struct tagANNTNPARAMS
{
	double	fTxtHeight;
	
	BOOL	bForceAnnotation;
	double	fRotAngle;
	short	iRotAngleType;	//0 = Along Contour Line(s);
							//1 = Along the Path Specified;
							//2 = User;

	char	szLyr[_MAX_PATH];
	short	iLyrType;		//0 = Current Layer;
							//1 = Respective Elev. Layer;
							//2 = User;
} ANNTNPARAMS;

typedef struct tagProfileData
{
	double	dXScale;
	double	dYScale;
	
	double	dDatum;
	double	dStartChg;
	double	dSexonInterval;
	double	dSexonIntervalMinor;

	BOOL	bDrawNERows;

	double	dNumAnnSize;
	double	dRowAnnSize;
	double	dPflLabelSize;
	double	dRowHt;
	//
	char	szNorthingTag[_MAX_PATH];
	char	szEastingTag[_MAX_PATH];
	char	szElevTag[_MAX_PATH];
	char	szDistanceTag[_MAX_PATH];
	char	szCumDistanceTag[_MAX_PATH];
	char	szDatumTag[_MAX_PATH];
	char	szProfileTag[_MAX_PATH];
	
	char	szLyrAnnotation[_MAX_PATH];
	char	szLyrTopSurface[_MAX_PATH];
	char	szLyrOrdinate[_MAX_PATH];
	char	szLyrAxis[_MAX_PATH];
} PROFILEDATA;
typedef struct tagProfileDataEx
{
	//
	int				iVersion;
	PROFILEDATA		data;
	double			fProfilePathWidth;
	//
	double			fExtraSpaceForFuture[10];
	int				iExtraSpaceForFuture[10];
	//
} PROFILEDATAEX;

typedef struct tagXSecData
{
	double	dXScale;
	double	dYScale;
	double	dNumAnnSize;
	double	dRowAnnSize;
	double	dRowHt;
	double	dCSLabelAnnSize;

	double	dSheetWid;
	double	dSheetHeight;
	double	dNPlateWid;
	double	dNPlateHeight;
	
	double	dStartChg;
	double	dCSInterval;
	double	dOrdInterval;
	double	dMaxCSOffset;

	BOOL	bAutoDatumFlag;
	double	dAutoDatumDenominator;

	char	szOffsetTag[_MAX_PATH];
	char	szElevTag[_MAX_PATH];
	char	szNorthingTag[_MAX_PATH];
	char	szEastingTag[_MAX_PATH];
	char	szDatumTag[_MAX_PATH];
	char	szXSecLabelTag[_MAX_PATH];
	
	char	szLyrAnnotation[_MAX_PATH];
	char	szLyrTopSurface[_MAX_PATH];
	char	szLyrOrdinate[_MAX_PATH];
	char	szLyrAxis[_MAX_PATH];
	
} XSECDATA;
typedef struct tagXSecDataEx
{
	//
	int			iVersion;								//Extra Field
	//
	XSECDATA	data;
	BOOL		bDrawSectionsAtAllIntersectionPoints;	//Extra Field
	BOOL		bDrawWithNorthingEastingData;			//Extra Field
	//
	double		dAutoDatumMargin;
	double		dCSIntervalMinor;
	//
	double		fExtraSpaceForFuture[10];				//Extra Field
	int			iExtraSpaceForFuture[10];				//Extra Field
	//
} XSECDATAEX;

typedef CTypedPtrArray<CPtrArray, TRIANGLE*> CTrianglePtrArray;
typedef CTypedPtrArray<CPtrArray, POINTDATA*> CPointDataPtrArray;
typedef CTypedPtrArray<CPtrArray, POINTPAIR3D*> CPointPair3DPtrArray;

void SortAcPoint3DArray(AcGePoint3dArray&, const short&);



#endif //_STRUCTURES_H_
