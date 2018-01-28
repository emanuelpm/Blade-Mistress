# Microsoft Developer Studio Project File - Name="SkinViewer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=SkinViewer - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SkinViewer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SkinViewer.mak" CFG="SkinViewer - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SkinViewer - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "SkinViewer - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SkinViewer - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\BBOnline\src" /I "..\..\puma\src" /I "..\..\helper\src" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 dsound.lib wininet.lib ws2_32.lib d3dx8.lib d3d8.lib winmm.lib dxguid.lib dxerr8.lib dinput8.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "SkinViewer - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\BBOnline\src" /I "..\..\puma\src" /I "..\..\helper\src" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "NOSOUND" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 dsound.lib wininet.lib ws2_32.lib d3dx8.lib d3d8.lib winmm.lib dxguid.lib dxerr8.lib dinput8.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib LIBCMTD.LIB LIBCPMTD.LIB /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "SkinViewer - Win32 Release"
# Name "SkinViewer - Win32 Debug"
# Begin Source File

SOURCE=..\..\Helper\Src\AutoLog.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Helper\Src\AutoLog.h
# End Source File
# Begin Source File

SOURCE=..\..\Puma\src\d3dapp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Puma\src\d3dapp.h
# End Source File
# Begin Source File

SOURCE=..\..\Puma\src\d3dfont.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Puma\src\d3dfont.h
# End Source File
# Begin Source File

SOURCE=..\..\Puma\src\d3dutil.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Puma\src\d3dutil.h
# End Source File
# Begin Source File

SOURCE=..\..\Helper\Src\Dataobje.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Helper\Src\Dataobje.h
# End Source File
# Begin Source File

SOURCE=..\..\Puma\src\dsutil.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Puma\src\dsutil.h
# End Source File
# Begin Source File

SOURCE=C:\mssdk\include\dxfile.h
# End Source File
# Begin Source File

SOURCE=..\..\Puma\src\dxutil.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Puma\src\dxutil.h
# End Source File
# Begin Source File

SOURCE=..\..\Helper\Src\fileFind.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Puma\src\gamemode.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Puma\src\gamemode.h
# End Source File
# Begin Source File

SOURCE=..\..\Helper\Src\GeneralUtils.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Helper\Src\GeneralUtils.h
# End Source File
# Begin Source File

SOURCE=..\..\Helper\Src\Linklist.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Helper\Src\Linklist.h
# End Source File
# Begin Source File

SOURCE=..\..\Puma\src\puma.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Puma\src\puma.h
# End Source File
# Begin Source File

SOURCE=..\..\Puma\src\pumaAnim.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Puma\src\pumaAnim.h
# End Source File
# Begin Source File

SOURCE=..\..\Puma\src\pumaMesh.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Puma\src\pumaMesh.h
# End Source File
# Begin Source File

SOURCE=..\..\Puma\src\pumaSound.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Puma\src\pumaSound.h
# End Source File
# Begin Source File

SOURCE=..\..\Puma\src\pumaVideoData.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Puma\src\pumaVideoData.h
# End Source File
# Begin Source File

SOURCE=..\Src\skinViewerMain.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Puma\src\UIR_ArtTextButton.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Puma\src\UIR_ArtTextButton.h
# End Source File
# Begin Source File

SOURCE=..\..\Puma\src\UIR_DragBar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Puma\src\UIR_DragBar.h
# End Source File
# Begin Source File

SOURCE=..\..\Puma\src\UIR_DragEdge.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Puma\src\UIR_DragEdge.h
# End Source File
# Begin Source File

SOURCE=..\..\Puma\src\UIR_EdgeBar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Puma\src\UIR_EdgeBar.h
# End Source File
# Begin Source File

SOURCE=..\..\Puma\src\UIR_EditLine.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Puma\src\UIR_EditLine.h
# End Source File
# Begin Source File

SOURCE=..\..\Puma\src\UIR_ScrollWindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Puma\src\UIR_ScrollWindow.h
# End Source File
# Begin Source File

SOURCE=..\..\Puma\src\UIR_TextBox.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Puma\src\UIR_TextBox.h
# End Source File
# Begin Source File

SOURCE=..\..\Puma\src\UIR_TextButton.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Puma\src\UIR_TextButton.h
# End Source File
# Begin Source File

SOURCE=..\..\Puma\src\UIR_Window.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Puma\src\UIR_Window.h
# End Source File
# Begin Source File

SOURCE=..\..\Puma\src\UIRect.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Puma\src\UIRect.h
# End Source File
# Begin Source File

SOURCE=..\Src\viewSkinMode.cpp
# End Source File
# Begin Source File

SOURCE=..\Src\viewSkinMode.h
# End Source File
# End Target
# End Project
