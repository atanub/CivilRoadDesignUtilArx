// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//


// Check if the build is DEBUG version and it's intended
// to be used with Non-DEBUG AutoCAD.
// In this case, for MFC header files, we need to undefine
// _DEBUG symbol
// Read this project readme.txt for more detail
#if defined( _DEBUG) && !defined(DEBUG_AUTOCAD)
    #pragma message("Building debug version of ContourArx.arx to be used with non-debug/Prod AutoCAD")
    #define _DEBUG_WAS_DEFINED
    #undef _DEBUG
#endif

#include <afxwin.h>

// Turn on the _DEBUG symbol if it was defined, before including
// non-MFC header files.
//
#ifdef _DEBUG_WAS_DEFINED
    #define _DEBUG
    #undef _DEBUG_WAS_DEFINED
#endif

/*
#ifndef _ATANU_BANIK
    #pragma message("Building ContourArx.ARX for Self use")
	#define _ATANU_BANIK
#endif
*/


#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxdisp.h>        // MFC OLE automation classes
#include <afxtempl.h>
//Grid ....
#include "GridCtrl.h"
#include "GridURLCell.h"
#include "GridCellCombo.h"
#include "GridCellCheck.h"
#include "GridCellDateTime.h"

//ACAD ....
#include "acdb.h"             // acdb definitions
#include "adslib.h"           // ads defs
#include "aced.h"             // aced stuff
#include "dbmain.h"           // object and entity reactors
#include "dbdict.h"           // dictionaries
#include "actrans.h"          // transaction manager
#include "rxmfcapi.h"         // ACAD MFC stuff
#include "rxregsvc.h"		  // ACAD stuff
#include <Adscodes.h>
#include <DbEnts.h>
#include <GeAssign.h>
#include <Math.h>

#include <DbSol3d.h>
#include <DbRegion.h>

#include "Structures.h"
#include <LicenseAll.h>


