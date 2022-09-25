// AutoUpdate.cpp : Defines the entry point for the console application.
//


#include "stdafx.h"
#include "../src/Helper/files.h";

#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <wininet.h>
#include <process.h>
#include <chrono>
#include <iostream>
#include <istream>
#include <streambuf>
#include <string>
#include <sstream>

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

	DoublyLinkedList *masterList;

	masterList = new DoublyLinkedList();

	// fill local list
	sprintf_s(tmp, 256, "%s", m_updateServer.pszServerName);
    auto localRecords = GetFileDetails(std::filesystem::path(tmp));


	// fill remote list
	HINTERNET hInternetSession;   
	HINTERNET hURL;
	char cBuffer[1024 * 100];
	BOOL bResult;
	DWORD dwBytesRead;

	// Make internet connection.
	hInternetSession = InternetOpen(
							"Microsoft Internet Explorer",   // agent
							INTERNET_OPEN_TYPE_PRECONFIG,      // access
							NULL, NULL, INTERNET_FLAG_NO_CACHE_WRITE);            // defaults

	char tempText[1024];
	sprintf_s(tempText,1024,"%s/index.dat", m_updateServer.pszServerURL);
	// Make connection to desired page.
	hURL = InternetOpenUrl(
				hInternetSession,               // session handle
				tempText,   // URL to access
				NULL, 0, INTERNET_FLAG_NO_CACHE_WRITE, 0);               // defaults

    // Closely related to IndexMaker.cpp :: ProcessDirectory
    unsigned long fileSize;
	InternetQueryDataAvailable( hURL, &fileSize, INTERNET_FLAG_NO_CACHE_WRITE,0);
	if (fileSize < 1)
	{
		UpdateTextBox("Unable to get index file from website.\r\n");
		return;
	}

	// read page into memory buffer
	bResult = InternetReadFile(hURL, (LPSTR)cBuffer,
					(unsigned long)1024*100, &dwBytesRead);

	// close down connections
	InternetCloseHandle(hURL);
	InternetCloseHandle(hInternetSession);

	// parse downloaded index file
    std::string indexDataBuffer(cBuffer, cBuffer + dwBytesRead);
    std::stringstream indexDataStream(indexDataBuffer);
    auto remoteRecords = ReadFileDetailsFromStream(&indexDataStream);

	int errorCount = 0;
    for(auto& remoteRecord : remoteRecords) {
        bool hasLocalCopy = false;
        for (auto& localRecord : localRecords) {
            if (!_strnicmp("auto", (char*)localRecord.relativePath.string().c_str(), strlen("auto")))
                continue;
            else if (!_strnicmp("BMLauncher", (char*)localRecord.relativePath.string().c_str(), strlen("BMLauncher")))
                continue;
            else if (!_strnicmp("BMLauncher", (char*)localRecord.relativePath.string().c_str(), strlen("BMLInstaller")))
                    newLauncher = true;

            if (remoteRecord.relativePath == localRecord.relativePath) {
                //	if the local file is old
                sprintf_s(tmp, 256, "checking %s\r\n", remoteRecord.relativePath.string().c_str());
                UpdateTextBox(tmp);

                if (localRecord.crc32 != remoteRecord.crc32) {
                    if (!DownloadFile((char*)remoteRecord.relativePath.string().c_str())) {
                        ++errorCount;
                    }
                }

                hasLocalCopy = true;
                break;
            }
        }

        if (!hasLocalCopy) {
			// copy the new version from the website
            if (!DownloadFile((char*)remoteRecord.relativePath.string().c_str())) {
                ++errorCount;
            }
		}

	}


	// clean up
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
