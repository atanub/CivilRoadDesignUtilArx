# Microsoft Developer Studio Project File - Name="ContourArx" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ContourArx - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ContourArx.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ContourArx.mak" CFG="ContourArx - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ContourArx - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ContourArx - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
F90=df.exe
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /nologo /MD /W3 /Gi /GX /Od /I "GridControlSource\GridCtrl_src" /I "GridControlSource\NewCellTypes" /I "..\..\LicenseKeyGenerator\License" /D "NDEBUG" /D "RADPACK" /D "ACRXAPP" /D "STRICT" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Fr /Yu"stdafx.h" /FD /Gx /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 acad.lib rxapi.lib acedapi.lib Libacge.lib Acgiapi.lib License.lib /nologo /base:"0x1c000000" /entry:"DllEntryPoint@12" /subsystem:windows /dll /incremental:yes /machine:I386 /nodefaultlib:"libc.lib" /out:".\Release\ContourArx.arx" /libpath:"..\..\LicenseKeyGenerator\License\Release"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
F90=df.exe
# ADD BASE CPP /nologo /MD /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /nologo /MD /W4 /Gm /GX /Zi /I "..\..\LicenseKeyGenerator\License" /I "GridControlSource\GridCtrl_src" /I "GridControlSource\NewCellTypes" /D "_DEBUG" /D "_MBCS" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Fr /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /X /u
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 acad.lib rxapi.lib acedapi.lib Libacge.lib Acgiapi.lib License.lib /nologo /base:"0x1c000000" /entry:"DllEntryPoint@12" /subsystem:windows /dll /debug /machine:I386 /nodefaultlib:"libc.lib" /out:".\Debug\ContourArx.arx" /libpath:"..\..\LicenseKeyGenerator\License\Debug"
# SUBTRACT LINK32 /map

!ENDIF 

# Begin Target

# Name "ContourArx - Win32 Release"
# Name "ContourArx - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Group "GridControlSource"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GridControlSource\GridCtrl_src\GridCell.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GridControlSource\GridCtrl_src\GridCellBase.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GridControlSource\NewCellTypes\GridCellCheck.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GridControlSource\NewCellTypes\GridCellCombo.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GridControlSource\NewCellTypes\GridCellDateTime.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GridControlSource\GridCtrl_src\GridCtrl.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GridControlSource\GridCtrl_src\GridDropTarget.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GridControlSource\NewCellTypes\GridURLCell.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GridControlSource\GridCtrl_src\InPlaceEdit.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GridControlSource\GridCtrl_src\TitleTip.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# End Group
# Begin Group "EntitySelector"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DbObjectEx.cpp
# End Source File
# Begin Source File

SOURCE=.\DbObjectEx.h
# End Source File
# Begin Source File

SOURCE=.\EntitySelector.cpp
# End Source File
# Begin Source File

SOURCE=.\EntitySelector.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\AboutDlg.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ActivateDlg.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\AnnotationParamDlg.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ContourAnnotation.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ContourApp.Cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ContourArx.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ContourArx.def
# End Source File
# Begin Source File

SOURCE=.\ContourARX.rc
# End Source File
# Begin Source File

SOURCE=.\ContourParamDlg.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ContourToolbar.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CoOrdMarker.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CoOrdMarkerPropDlg.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CrossSectionProfileDefinition.cpp
# End Source File
# Begin Source File

SOURCE=.\CurveCalculator.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CutFillCalculatorForXSection.cpp
# End Source File
# Begin Source File

SOURCE=.\DefineBoundaryDlg.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DrawContourLines.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DrawUtil.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DwgParamObj.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\edreact.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GenGridData.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GenGridDataDlg.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GenPtsDlg.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\GenPtsFrMultiSegDlg.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\HAlignment.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\HorzCurveDataDlg.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ImportPts.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\LicenseKeySelectionDlg.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Line2PLine.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ModifyAnnotationDlg.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NEAnnotationEntity.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\NEEntPropDlg.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\objreact.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\PLineDataExtractor.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Profile.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ProfileDataDlg.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ProfileDwgAttrs.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Protection.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ProtectionInterface.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Purge.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\QProfile.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\RailwayDsgnTemplateDlg.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\RailXSectionTemplate.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ReadPtData.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\resourcehelper.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ResultDlg.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\RoadDesignDataDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\stdafx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Structures.Cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SubRoutines.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SystemRegistryManager.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\TIN.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\TINData.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\TINEntity.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\VAlignment.cpp
# End Source File
# Begin Source File

SOURCE=.\VAlignment.h
# End Source File
# Begin Source File

SOURCE=.\Volume.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\XData.Cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\XSecDataDlg.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\XSection.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\XSectionDwg.cpp

!IF  "$(CFG)" == "ContourArx - Win32 Release"

# ADD CPP /Yu"stdafx.h"

!ELSEIF  "$(CFG)" == "ContourArx - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\ActivateDlg.h
# End Source File
# Begin Source File

SOURCE=.\AnnotationParamDlg.h
# End Source File
# Begin Source File

SOURCE=.\GridControlSource\GridCtrl_src\CellRange.h
# End Source File
# Begin Source File

SOURCE=.\ContourAnnotation.h
# End Source File
# Begin Source File

SOURCE=.\ContourApp.h
# End Source File
# Begin Source File

SOURCE=.\ContourParamDlg.h
# End Source File
# Begin Source File

SOURCE=.\ContourToolbar.h
# End Source File
# Begin Source File

SOURCE=.\CoOrdMarker.h
# End Source File
# Begin Source File

SOURCE=.\CoOrdMarkerPropDlg.h
# End Source File
# Begin Source File

SOURCE=.\DefineBoundaryDlg.h
# End Source File
# Begin Source File

SOURCE=.\DrawContourLines.h
# End Source File
# Begin Source File

SOURCE=.\DrawUtil.h
# End Source File
# Begin Source File

SOURCE=.\DwgParamObj.h
# End Source File
# Begin Source File

SOURCE=.\edreact.h
# End Source File
# Begin Source File

SOURCE=.\GenGridDataDlg.h
# End Source File
# Begin Source File

SOURCE=.\GenPtsDlg.h
# End Source File
# Begin Source File

SOURCE=.\GenPtsFrMultiSegDlg.h
# End Source File
# Begin Source File

SOURCE=.\GridControlSource\GridCtrl_src\GridCell.h
# End Source File
# Begin Source File

SOURCE=.\GridControlSource\GridCtrl_src\GridCellBase.h
# End Source File
# Begin Source File

SOURCE=.\GridControlSource\NewCellTypes\GridCellCheck.h
# End Source File
# Begin Source File

SOURCE=.\GridControlSource\NewCellTypes\GridCellCombo.h
# End Source File
# Begin Source File

SOURCE=.\GridControlSource\NewCellTypes\GridCellDateTime.h
# End Source File
# Begin Source File

SOURCE=.\GridControlSource\GridCtrl_src\GridCtrl.h
# End Source File
# Begin Source File

SOURCE=.\GridControlSource\GridCtrl_src\GridDropTarget.h
# End Source File
# Begin Source File

SOURCE=.\GridControlSource\NewCellTypes\GridURLCell.h
# End Source File
# Begin Source File

SOURCE=.\HorzCurveDataDlg.h
# End Source File
# Begin Source File

SOURCE=.\ImportPts.h
# End Source File
# Begin Source File

SOURCE=.\GridControlSource\GridCtrl_src\InPlaceEdit.h
# End Source File
# Begin Source File

SOURCE=.\Line2PLine.h
# End Source File
# Begin Source File

SOURCE=.\GridControlSource\GridCtrl_src\MemDC.h
# End Source File
# Begin Source File

SOURCE=.\NEAnnotationEntity.h
# End Source File
# Begin Source File

SOURCE=.\NEEntPropDlg.h
# End Source File
# Begin Source File

SOURCE=.\objreact.h
# End Source File
# Begin Source File

SOURCE=.\Purge.h
# End Source File
# Begin Source File

SOURCE=.\QProfile.h
# End Source File
# Begin Source File

SOURCE=.\RailwayDsgnTemplateDlg.h
# End Source File
# Begin Source File

SOURCE=.\ReadPtData.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\resourcehelper.h
# End Source File
# Begin Source File

SOURCE=.\ResultDlg.h
# End Source File
# Begin Source File

SOURCE=.\RoadDesignDataDlg.h
# End Source File
# Begin Source File

SOURCE=.\stdafx.h
# End Source File
# Begin Source File

SOURCE=.\Structures.h
# End Source File
# Begin Source File

SOURCE=.\TIN.h
# End Source File
# Begin Source File

SOURCE=.\TINEntity.h
# End Source File
# Begin Source File

SOURCE=.\GridControlSource\GridCtrl_src\TitleTip.h
# End Source File
# Begin Source File

SOURCE=.\Util.h
# End Source File
# Begin Source File

SOURCE=.\Volume.h
# End Source File
# Begin Source File

SOURCE=.\XData.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\idr_cont.ico
# End Source File
# Begin Source File

SOURCE=.\res\idr_main.ico
# End Source File
# Begin Source File

SOURCE=.\toolbar1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\toolbar_.bmp
# End Source File
# End Group
# End Target
# End Project
