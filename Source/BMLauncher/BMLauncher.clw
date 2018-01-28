; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CBMUpdater
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "BMLauncher.h"

ClassCount=5
Class1=CBMLauncherApp
Class2=CBMLauncherDlg
Class3=CBMUpdater

ResourceCount=6
Resource1=IDR_MAINFRAME
Resource2=IDD_BMLAUNCHER_DIALOG
Resource3=IDD_ABOUT_DIALOG
Resource4=IDD_UPDATE_DIALOG
Resource5=IDD_README_DIALOG
Class4=CBMAboutDlg
Class5=CBMReadmeDlg
Resource6=IDR_MENU

[CLS:CBMLauncherApp]
Type=0
HeaderFile=BMLauncher.h
ImplementationFile=BMLauncher.cpp
Filter=N

[CLS:CBMLauncherDlg]
Type=0
HeaderFile=BMLauncherDlg.h
ImplementationFile=BMLauncherDlg.cpp
Filter=D
LastObject=CBMLauncherDlg
BaseClass=CDialog
VirtualFilter=dWC

[DLG:IDD_BMLAUNCHER_DIALOG]
Type=1
Class=CBMLauncherDlg
ControlCount=21
Control1=IDC_STATIC,static,1342179342
Control2=IDC_STATIC,button,1342177287
Control3=IDC_RADIO_PRIMARY,button,1342308361
Control4=IDC_RADIO_TEST,button,1342177289
Control5=IDC_STATIC,button,1342177287
Control6=IDC_RADIO_FULLSCREEN,button,1342308361
Control7=IDC_RADIO_WINDOWED,button,1342177289
Control8=IDC_STATIC,static,1342308352
Control9=IDC_RADIO_640,button,1342308361
Control10=IDC_RADIO_800,button,1342177289
Control11=IDC_RADIO_1024,button,1342177289
Control12=IDC_RADIO_1600,button,1342177289
Control13=IDC_STATIC,button,1342177287
Control14=IDC_STATIC,static,1342308353
Control15=IDC_STATIC,static,1342308352
Control16=IDC_EDIT_ACCOUNT,edit,1350631552
Control17=IDC_STATIC,static,1342308352
Control18=IDC_EDIT_PASSWORD,edit,1350631584
Control19=IDC_BUTTON_LAUNCH,button,1342242816
Control20=IDC_BUTTON_EXIT,button,1342242816
Control21=IDC_BUTTON_UPDATE,button,1208025088

[DLG:IDD_UPDATE_DIALOG]
Type=1
Class=CBMUpdater
ControlCount=2
Control1=ID_CLOSE,button,1476460545
Control2=IDC_TEXT,RICHEDIT,1352730692

[CLS:CBMUpdater]
Type=0
HeaderFile=BMUpdater.h
ImplementationFile=BMUpdater.cpp
BaseClass=CDialog
Filter=D
LastObject=CBMUpdater
VirtualFilter=dWC

[MNU:IDR_MENU]
Type=1
Class=CBMLauncherDlg
Command1=ID_MENU_LAUNCH
Command2=ID_MENU_EXIT
Command3=ID_MENU_BLOG
Command4=ID_MENU_WIKI
Command5=ID_MENU_FORUM
Command6=ID_MENU_WEBSITE
Command7=ID_MENU_README
Command8=ID_MENU_ABOUT
CommandCount=8

[DLG:IDD_README_DIALOG]
Type=1
Class=CBMReadmeDlg
ControlCount=2
Control1=IDOK,button,1342242817
Control2=IDC_TEXT,RICHEDIT,1352728644

[DLG:IDD_ABOUT_DIALOG]
Type=1
Class=CBMAboutDlg
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDC_LAUNCHER_TEXT,static,1342308352
Control3=IDC_STATIC,static,1342308352

[CLS:CBMAboutDlg]
Type=0
HeaderFile=BMAboutDlg.h
ImplementationFile=BMAboutDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=CBMAboutDlg
VirtualFilter=dWC

[CLS:CBMReadmeDlg]
Type=0
HeaderFile=BMReadmeDlg.h
ImplementationFile=BMReadmeDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=CBMReadmeDlg
VirtualFilter=dWC

