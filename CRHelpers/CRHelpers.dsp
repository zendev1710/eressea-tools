# Microsoft Developer Studio Project File - Name="CRHelpers" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=CRHelpers - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "CRHelpers.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CRHelpers.mak" CFG="CRHelpers - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CRHelpers - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "CRHelpers - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CRHelpers - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\crtools" /I "..\eressea" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x40c /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x40c /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "CRHelpers - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\crtools" /I "..\eressea" /D "WIN32" /D "_DEBUG" /D "_AFXDLL" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x40c /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x40c /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "CRHelpers - Win32 Release"
# Name "CRHelpers - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CR.cpp
# End Source File
# Begin Source File

SOURCE=.\CRBuilding.cpp
# End Source File
# Begin Source File

SOURCE=.\CRBuildings.cpp
# End Source File
# Begin Source File

SOURCE=.\CRContainer.cpp
# End Source File
# Begin Source File

SOURCE=.\CRFaction.cpp
# End Source File
# Begin Source File

SOURCE=.\CRFactions.cpp
# End Source File
# Begin Source File

SOURCE=.\CRRace.cpp
# End Source File
# Begin Source File

SOURCE=.\CRRaces.cpp
# End Source File
# Begin Source File

SOURCE=.\CRRegion.cpp
# End Source File
# Begin Source File

SOURCE=.\CRRegions.cpp
# End Source File
# Begin Source File

SOURCE=.\CRReport.cpp
# End Source File
# Begin Source File

SOURCE=.\CRShip.cpp
# End Source File
# Begin Source File

SOURCE=.\CRShips.cpp
# End Source File
# Begin Source File

SOURCE=.\CRUnit.cpp
# End Source File
# Begin Source File

SOURCE=.\CRUnits.cpp
# End Source File
# Begin Source File

SOURCE=.\MapiAdmin.cpp
# End Source File
# Begin Source File

SOURCE=.\registrykey.cpp
# End Source File
# Begin Source File

SOURCE=.\SortableObList.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=.\Token.cpp
# End Source File
# Begin Source File

SOURCE=.\wToDir.cpp
# End Source File
# Begin Source File

SOURCE=.\wToFile.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\CR.h
# End Source File
# Begin Source File

SOURCE=.\CRBuilding.h
# End Source File
# Begin Source File

SOURCE=.\CRBuildings.h
# End Source File
# Begin Source File

SOURCE=.\CRContainer.h
# End Source File
# Begin Source File

SOURCE=.\CREresseaFile.h
# End Source File
# Begin Source File

SOURCE=.\CRFaction.h
# End Source File
# Begin Source File

SOURCE=.\CRFactions.h
# End Source File
# Begin Source File

SOURCE=.\CRFile.h
# End Source File
# Begin Source File

SOURCE=.\CRRace.h
# End Source File
# Begin Source File

SOURCE=.\CRRegion.h
# End Source File
# Begin Source File

SOURCE=.\CRRegions.h
# End Source File
# Begin Source File

SOURCE=.\CRReport.h
# End Source File
# Begin Source File

SOURCE=.\CRShip.h
# End Source File
# Begin Source File

SOURCE=.\CRTypes.h
# End Source File
# Begin Source File

SOURCE=.\CRUnits.h
# End Source File
# Begin Source File

SOURCE=.\MapiAdmin.h
# End Source File
# Begin Source File

SOURCE=.\registrykey.h
# End Source File
# Begin Source File

SOURCE=.\SortableObList.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Token.h
# End Source File
# Begin Source File

SOURCE=.\wLizard.h
# End Source File
# Begin Source File

SOURCE=.\wToDir.h
# End Source File
# Begin Source File

SOURCE=.\wToFile.h
# End Source File
# End Group
# End Target
# End Project
