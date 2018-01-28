# Microsoft Developer Studio Project File - Name="InfoRipper" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=InfoRipper - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "InfoRipper.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "InfoRipper.mak" CFG="InfoRipper - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "InfoRipper - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "InfoRipper - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "InfoRipper - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "InfoRipper - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\BBOnline\src" /I "..\..\puma\src" /I "..\..\helper\src" /I "..\..\network" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib wininet.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "InfoRipper - Win32 Release"
# Name "InfoRipper - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE="..\Src\BBO-Savatar.cpp"
# End Source File
# Begin Source File

SOURCE="..\Src\BBO-Smob.cpp"
# End Source File
# Begin Source File

SOURCE="..\Src\BBO-Snpc.cpp"
# End Source File
# Begin Source File

SOURCE=..\..\Helper\Src\BStream.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Helper\Src\crc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Helper\Src\Dataobje.cpp
# End Source File
# Begin Source File

SOURCE="..\Src\dungeon-map.cpp"
# End Source File
# Begin Source File

SOURCE=..\..\Helper\Src\fileFind.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\FileTool.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\FileTool.h
# End Source File
# Begin Source File

SOURCE=.\InfoRipper.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Helper\Src\Linklist.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\mapList.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\questSystem.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\staffData.cpp
# End Source File
# Begin Source File

SOURCE=..\NameRipper\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=..\NameRipper\stubBBOserver.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\tokenManager.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\traderGoods.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Helper\Src\UniqueNames.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE="..\Src\BBO-Savatar.h"
# End Source File
# Begin Source File

SOURCE="..\Src\BBO-Smob.h"
# End Source File
# Begin Source File

SOURCE=..\NameRipper\StdAfx.h
# End Source File
# Begin Source File

SOURCE=..\NameRipper\stubBBOserver.h
# End Source File
# Begin Source File

SOURCE=..\..\Helper\Src\UniqueNames.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE="..\Src\BBO-Smonster.cpp"
# End Source File
# Begin Source File

SOURCE="..\Src\BBO-Smonster.h"
# End Source File
# Begin Source File

SOURCE=..\Src\BBO.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\BBO.h
# End Source File
# Begin Source File

SOURCE=..\..\Helper\Src\GeneralUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Helper\Src\GeneralUtils.h
# End Source File
# Begin Source File

SOURCE=..\Src\inventory.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\inventory.h
# End Source File
# Begin Source File

SOURCE=..\Src\longtime.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\longtime.h
# End Source File
# Begin Source File

SOURCE=..\Src\monsterData.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\monsterData.h
# End Source File
# Begin Source File

SOURCE=..\NameRipper\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=..\Src\sharedSpace.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\sharedSpace.h
# End Source File
# Begin Source File

SOURCE=..\Src\totemData.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\totemData.h
# End Source File
# End Target
# End Project
