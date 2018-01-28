// AutoUpdate.cpp : Defines the entry point for the console application.
//


#include "stdafx.h"
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <wininet.h>
#include <process.h>

#include "Autoupdate.h"
#include "UpdateServer.h"




//***************************************************************************************
FileRecord::FileRecord(int doid, char *doname)	 : DataObject(doid,doname)
{
	;
}

//***************************************************************************************
FileRecord::~FileRecord()
{
	;
}
//*********************************************************************


AutoUpdate::AutoUpdate(const UpdateServer& updateServer) 
	: newLauncher(false)
	, m_updateServer(updateServer)
{ 
}

void AutoUpdate::ProcessDirectory(DoublyLinkedList *recList, char* dir)
{
	char tempText[1024];
	char tmp[2048];

	sprintf_s(tempText, 1024, "%s\\*",dir);

	FileNameList *list = GetNameList(tempText, "");

	for (int i = 0; list && i < list->numOfFiles; ++i)
	{
		// looking for a period, which would indicate a file
		int isFile = FALSE;
		for (int j = 0; j < (int)strlen(list->nameList[i].name); ++j)
		{
			if ('.' == list->nameList[i].name[j])
				isFile = TRUE;
		}

		if (isFile)
		{
			if (!strncmp(".",list->nameList[i].name,1))
				;
			else if (!strncmp("..",list->nameList[i].name,2))
				;
			else if (!_strnicmp("auto",list->nameList[i].name,strlen("auto")))
				;
			else if (!_strnicmp("BMLauncher",list->nameList[i].name,strlen("BMLauncher")))
				;
			else
			{
				if(!_strnicmp("BMLauncher",list->nameList[i].name,strlen("BMLInstaller")))
					newLauncher = true;

				sprintf_s(tempText, 1024, "%s\\%s",dir,list->nameList[i].name);
				HANDLE tempFile = CreateFile(tempText,GENERIC_READ,0, NULL, OPEN_EXISTING,
							 FILE_ATTRIBUTE_NORMAL, NULL);

				if (!tempFile)
				{
					sprintf_s( tmp, 2048, "Unable to open %s to get its time.\r\n", tempText );
					UpdateTextBox( tmp );
					return;
				}
 
				FILETIME created, accessed, written;
				if (!GetFileTime(	tempFile, &created, &accessed, &written))
				{
					sprintf_s( tmp, 2048, "Unable to get time info for %s.\r\n", tempText );
					UpdateTextBox( tmp );
					return;
				}

				DWORD size;
				size = GetFileSize(tempFile,NULL);
				if (0xFFFFFFFF == size)
				{
					sprintf_s( tmp, 2048, "Unable to get size info for %s.\r\n", tempText );
					UpdateTextBox( tmp );
					return;
				}

				// close down file
				CloseHandle(tempFile);

				DWORD dwCrc32;
				DWORD crcResult = GetCRC(tempText, dwCrc32);
//				assert(!crcResult);

				FileRecord *fr = new FileRecord(0,tempText);
				fr->time = written;
				fr->size = dwCrc32; //size;
				recList->Append(fr);
			}
		}
		else
		{
			sprintf_s(tempText, 1024, "%s\\%s",dir,list->nameList[i].name);
			ProcessDirectory(recList,tempText);
		}


	}


	delete list;

}

//*********************************************************************
void AutoUpdate::ProcessIndexData(DoublyLinkedList *list, char *data, DWORD length)
{
	char *curPtr = data;
	char *endPtr = data + length;
	char tempText[1024];
	short size = 1;

	while (curPtr < endPtr)
	{
		memcpy(&size,curPtr,2);
		curPtr += 2;
		if (size != -1)
		{
			memcpy(&tempText,curPtr,size);
			tempText[size] = 0;
			tempText[3] -= 1;
			curPtr += size;
			FileRecord *fr = new FileRecord(0,tempText);
			memcpy(&(fr->time),curPtr,sizeof(FILETIME));
			curPtr += sizeof(FILETIME);
			memcpy(&(fr->size),curPtr,sizeof(DWORD));
			curPtr += sizeof(DWORD);
			list->Append(fr);
		}
	}
}

//*********************************************************************
int AutoUpdate::DownloadFile(char *fileName)
{
	char tmp[1024];
	sprintf_s( tmp, 1024, "Updating %s  ",fileName );
	UpdateTextBox( tmp );
	HINTERNET hInternetSession;   
	HINTERNET hURL;
	char cBuffer[4096];
	char tempText[1024];
	BOOL bResult;
	DWORD dwBytesRead;
	HANDLE hOutputFile;

	int gotFile = FALSE;

	// Make internet connection.
	hInternetSession = InternetOpen(
							"Microsoft Internet Explorer",   // agent
							INTERNET_OPEN_TYPE_PRECONFIG,      // access
							NULL, NULL, 0);            // defaults

	int openTries = 0;
	while (!hInternetSession && openTries < 20)
	{
		++openTries;
		Sleep(100);
		hInternetSession = InternetOpen(
								"Microsoft Internet Explorer",   // agent
								INTERNET_OPEN_TYPE_PRECONFIG,      // access
								NULL, NULL, 0);            // defaults
	}

	if (hInternetSession)
	{

		// Make connection to desired page.
		sprintf_s(tempText,1024,"%s/%s", m_updateServer.pszServerURL, fileName);
		hURL = InternetOpenUrl(
					hInternetSession,               // session handle
					tempText,   // URL to access
					NULL, 0, 0, 0);               // defaults

		int connectTries = 0;
		while (!hURL && connectTries < 20)
		{
			++connectTries;
			Sleep(100);
		}

		if (hURL)
		{
	//		DWORD fileSize;
	//		InternetQueryDataAvailable( hURL, &fileSize,0,0);

	//		cBuffer = new char[fileSize];

			// create output file
			sprintf_s(tempText,1024,"%s\\%s",m_updateServer.pszServerName, fileName);
			int index = strlen(tempText);
			while (index > 0 && '\\' != tempText[index])
				--index;
			if ('\\' == tempText[index])
				tempText[index] = 0;

			CreateDirectory(tempText, NULL);

			sprintf_s(tempText,1024,"%s\\%s",m_updateServer.pszServerName, fileName);
			hOutputFile = CreateFile(tempText,GENERIC_WRITE,0, NULL, CREATE_ALWAYS,
									 FILE_ATTRIBUTE_NORMAL, NULL);

			DWORD att = GetFileAttributes( tempText );

			if( ( att & FILE_ATTRIBUTE_READONLY ) == FILE_ATTRIBUTE_READONLY ) {
				UpdateTextBox( "ERROR: " );
				UpdateTextBox( tempText );
				UpdateTextBox( " is read only!\r\n" );
				return false;
			}

			do
			{
				// read page into memory buffer
				bResult = InternetReadFile(hURL, (LPSTR)cBuffer,
								(DWORD)4096, &dwBytesRead);

				// write out data
				DWORD bytesWritten;
				bResult = WriteFile(hOutputFile, cBuffer, dwBytesRead, &bytesWritten, NULL);

				UpdateTextBox( "." );

			} while( /*!bResult ||*/ dwBytesRead != 0 ); 
			// close down file
			CloseHandle(hOutputFile);
			gotFile = TRUE;

			// close down connections
			InternetCloseHandle(hURL);

		}

		// close down connections
		InternetCloseHandle(hInternetSession);
	}


	if (gotFile)
		UpdateTextBox( "DONE\r\n" );
	else
		UpdateTextBox( "*** ERROR\r\n" );

	return gotFile;

}

//*********************************************************************
void AutoUpdate::Update()
{
	char tmp[256];

	newLauncher = false;

	UpdateTextBox( "Updating Blade Mistress\r\n\r\n" );

	DoublyLinkedList *localList, *masterList;

	localList  = new DoublyLinkedList();
	masterList = new DoublyLinkedList();

	// fill local list
	sprintf_s(tmp, 256, ".\\%s", m_updateServer.pszServerName);
	ProcessDirectory(localList, tmp);


	// fill master list
	HINTERNET hInternetSession;   
	HINTERNET hURL;
	char cBuffer[1024 * 100];
	BOOL bResult;
	DWORD dwBytesRead;

	// Make internet connection.
	hInternetSession = InternetOpen(
							"Microsoft Internet Explorer",   // agent
							INTERNET_OPEN_TYPE_PRECONFIG,      // access
							NULL, NULL, 0);            // defaults

	char tempText[1024];
	sprintf_s(tempText,1024,"%s/index.dat", m_updateServer.pszServerURL);
	// Make connection to desired page.
	hURL = InternetOpenUrl(
				hInternetSession,               // session handle
				tempText,   // URL to access
				NULL, 0, 0, 0);               // defaults

	DWORD fileSize;
	InternetQueryDataAvailable( hURL, &fileSize,0,0);

	if (fileSize < 1)
	{
		UpdateTextBox("Unable to get index file from website.\r\n");
		return;
	}


	// read page into memory buffer
	bResult = InternetReadFile(hURL, (LPSTR)cBuffer,
					(DWORD)1024*100, &dwBytesRead);

	// close down connections
	InternetCloseHandle(hURL);
	InternetCloseHandle(hInternetSession);

	// parse downloaded index file
	ProcessIndexData(masterList, cBuffer, dwBytesRead);

	// for each master file

	int errorCount = 0;

	FileRecord *frMaster = (FileRecord *) masterList->First();
	while (frMaster)
	{

		// if there is a corresponding local file
		FileRecord *frLocal = (FileRecord *) localList->Find(frMaster->WhoAmI());
		if (frLocal)
		{
			//	if the local file is old
			sprintf_s( tmp, 256,"checking %s\r\n", frMaster->WhoAmI() );
			UpdateTextBox( tmp );

			if (frLocal->size != frMaster->size)
				if (!DownloadFile(frMaster->WhoAmI()))
					++errorCount;
/*
			txtBox += "checking %s (%ld vs remote %ld\n",
				frLocal->do_name, frLocal->time.dwHighDateTime, frMaster->time.dwHighDateTime);
			if (CompareFileTime(&(frLocal->time), &(frMaster->time)) < 0)
			{ 
				// copy the new version from the website
				if (!DownloadFile(frMaster->WhoAmI()))
					++errorCount;
			}
			else if (frLocal->size != frMaster->size)
				if (!DownloadFile(frMaster->WhoAmI()))
					++errorCount;
*/
		}
		else
		{
			// copy the new version from the website
			if (!DownloadFile(frMaster->WhoAmI()))
				++errorCount;
		}

		frMaster = (FileRecord *) masterList->Find(frMaster->WhoAmI());

		frMaster = (FileRecord *) masterList->Next();
	}


	// clean up

	delete masterList;
	delete localList;

	if (errorCount) 
	{
		sprintf_s( tmp, 256,"\r\n\r\n*** %d files could not be downloaded. **\r\n", errorCount );
		UpdateTextBox( tmp );
	}
}


void AutoUpdate::UpdateTextBox(char* txt)
{
	 SendMessage( dialog, WM_USER+1, (unsigned int)txt, NULL );
}

bool AutoUpdate::IsThereNewLauncher() { return newLauncher; }