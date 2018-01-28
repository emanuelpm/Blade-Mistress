:: The structure of the client directory is:
::
:: Root
::  * Dat
::      - Cooked data resides here
::  * BBOnline.exe
::  * readme.txt
::  * textColors.txt

@echo off

pushd ..

:: Delete any files already staged
del StagingFilesClient\* /F /S /Q >nul 2>&1

:: Make the directories up front and suppress if they can't (usually because they already exist)
mkdir StagingFilesClient >nul 2>&1
mkdir StagingFilesClient\Dat >nul 2>&1

:: Copy our files over with overwrite
xcopy Assets\ArtCooked\* StagingFilesClient\Dat\ /Y
xcopy Assets\readme.txt StagingFilesClient\ /Y
xcopy Assets\textColors.txt StagingFilesClient\ /Y

xcopy Build\Win32\Release\BBOnline\BBOnline.exe StagingFilesClient\ /Y

:: Delete old dat file
del ..\StagingFilesClient\index.dat

:: Run the index generator
Build\Win32\Release\IndexMaker\IndexMaker.exe %~dp0..\StagingFilesClient\ %~dp0..\index.dat

:: Move the dat file
move %~dp0..\index.dat %~dp0..\StagingFilesClient\index.dat

popd
