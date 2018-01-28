# Microsoft Developer Studio Project File - Name="BMLauncher" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=BMLauncher - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "BMLauncher.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "BMLauncher.mak" CFG="BMLauncher - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "BMLauncher - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "BMLauncher - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "BMLauncher - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL" /d "_BM_LAUNCHER"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 wininet.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "BMLauncher - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL" /d "_BM_LAUNCHER"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 wininet.lib /nologo /subsystem:windows /debug /machine:I386 /out:"C:\Program Files\Blade Mistress\BMLauncher.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "BMLauncher - Win32 Release"
# Name "BMLauncher - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AutoUpdate.cpp
# End Source File
# Begin Source File

SOURCE=.\BMAboutDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\BMLauncher.cpp
# End Source File
# Begin Source File

SOURCE=.\BMLauncher.rc
# End Source File
# Begin Source File

SOURCE=.\BMLauncherDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\BMReadmeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\BMUpdater.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Helper\crc.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Helper\Dataobje.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Helper\fileFind.cpp
# End Source File
# Begin Source File

SOURCE=.\LauncherConfig.cpp
# End Source File
# Begin Source File

SOURCE=..\src\Helper\Linklist.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AutoUpdate.h
# End Source File
# Begin Source File

SOURCE=.\BMAboutDlg.h
# End Source File
# Begin Source File

SOURCE=.\BMLauncher.h
# End Source File
# Begin Source File

SOURCE=.\BMLauncherDlg.h
# End Source File
# Begin Source File

SOURCE=.\BMReadmeDlg.h
# End Source File
# Begin Source File

SOURCE=.\BMUpdater.h
# End Source File
# Begin Source File

SOURCE=..\Src\Helper\crc.h
# End Source File
# Begin Source File

SOURCE=..\Src\Helper\Dataobje.h
# End Source File
# Begin Source File

SOURCE=..\Src\Helper\fileFind.h
# End Source File
# Begin Source File

SOURCE=.\LauncherConfig.h
# End Source File
# Begin Source File

SOURCE=..\Src\Helper\Linklist.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\BMLauncher.ico
# End Source File
# Begin Source File

SOURCE=.\res\BMLauncher.rc2
# End Source File
# Begin Source File

SOURCE=.\res\Logo.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
