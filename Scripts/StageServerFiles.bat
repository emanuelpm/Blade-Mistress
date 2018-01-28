:: The structure of the server directory is:
::
:: Root
::  * Dat
::      - Cooked data resides here
::  * ServerData
::      - bannedIPs.txt
::      - bannedUIDs.txt
::      - dungeons.dat
::      - items.txt
::      - petDragonData.txt
::      - questScripts.dat
::      - treeText.txt
::  * ServerWrapper.exe
::  * index.dat (this file is the index file for the Dat directory)
::  * readme.txt
::  * textColors.txt

@echo off

pushd ..

:: Delete any files already staged
del StagingFilesServer\* /F /S /Q >nul 2>&1

:: Make the directories up front and suppress if they can't (usually because they already exist)
mkdir StagingFilesServer >nul 2>&1
mkdir StagingFilesServer\Dat >nul 2>&1
mkdir StagingFilesServer\ServerData >nul 2>&1

:: Copy our files over with overwrite
xcopy Assets\ArtCooked\* StagingFilesServer\Dat\ /Y
xcopy Assets\ServerData\* StagingFilesServer\ServerData\ /Y
xcopy Assets\readme.txt StagingFilesServer\ /Y
xcopy Assets\textColors.txt StagingFilesServer\ /Y

xcopy Build\Win32\Release\ServerWrapper\ServerWrapper.exe StagingFilesServer\ /Y

popd
