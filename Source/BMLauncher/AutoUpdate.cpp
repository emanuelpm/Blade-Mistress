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

std::vector<FileRecord> AutoUpdate::ProcessDirectory(char* dir)
{
	char tempText[1024];
	char tmp[2048];

    auto filePaths = GetFilePaths(std::filesystem::path(dir));

    std::vector<FileRecord> records;
    for (const auto& path : filePaths) {
		if (!_strnicmp("auto",path.filename().string().c_str(), strlen("auto")))
			;
		else if (!_strnicmp("BMLauncher", path.filename().string().c_str(),strlen("BMLauncher")))
			;
		else
		{
			if(!_strnicmp("BMLauncher", path.filename().string().c_str(),strlen("BMLInstaller")))
				newLauncher = true;

            // get relative path from the directory we're processing
            std::filesystem::path relativeFilePath = std::filesystem::relative(path, std::filesystem::path(dir));

            // get the last modified time of the file
            using namespace std::chrono;
            auto sctp = time_point_cast<system_clock::duration>(std::filesystem::last_write_time(path) - std::filesystem::file_time_type::clock::now() + system_clock::now());
            std::time_t writtenTime = system_clock::to_time_t(sctp);

            // generate cyclical redundancy check
            unsigned long crc32;
            unsigned long crcResult = GetCRC(path.string(), crc32);
            assert(!crcResult);

            {
                FileRecord fr(0, (char*)relativeFilePath.string().c_str());
                fr.time = writtenTime;
                fr.size = crc32;
                records.push_back(fr);
            }
		}
	}
    return records;
}

//*********************************************************************
std::vector<FileRecord> AutoUpdate::ProcessIndexData(char *data, DWORD length)
{
	char *curPtr = data;
	char *endPtr = data + length;
    char tempText[1024];
    short size{ 1 };

    // for writing of this see IndexMaker.cpp :: ProcessDirectory
    std::vector<FileRecord> records;
	while (curPtr < endPtr)
	{
        /* ---- FORMAT ----
        *  - relative path length
        *  - "tagged" relative path
        *  - last modified time
        *  - cyclical redundancy check
        */
		memcpy(&size,curPtr,2);
		curPtr += 2;
		if (size != -1)
		{
			memcpy(&tempText,curPtr,size);
            tempText[size] = 0; // null char termination
			tempText[3] -= 1;
			curPtr += size;
            FileRecord fr(0, tempText);
            memcpy(&(fr.time), curPtr, sizeof(std::time_t));
            curPtr += sizeof(std::time_t);
            memcpy(&(fr.size), curPtr, sizeof(unsigned long));
            curPtr += sizeof(unsigned long);
            records.push_back(fr);
		}
        else {
            break;
        }
	}

    return records;
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
	auto localRecords = ProcessDirectory(tmp);


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
	DWORD fileSize;
	InternetQueryDataAvailable( hURL, &fileSize, INTERNET_FLAG_NO_CACHE_WRITE,0);
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
    auto remoteRecords = ProcessIndexData(cBuffer, dwBytesRead);
	int errorCount = 0;
    for(auto& remoteRecord : remoteRecords) {
        bool hasLocalCopy = false;
        for (auto& localRecord : localRecords) {
            if (strcmp(remoteRecord.WhoAmI(), localRecord.WhoAmI()) == 0) {
                //	if the local file is old
                sprintf_s(tmp, 256, "checking %s\r\n", remoteRecord.WhoAmI());
                UpdateTextBox(tmp);

                if (localRecord.size != remoteRecord.size)
                    if (!DownloadFile(remoteRecord.WhoAmI()))
                        ++errorCount;

                hasLocalCopy = true;
                break;
            }
        }

        if (!hasLocalCopy) {
			// copy the new version from the website
			if (!DownloadFile(remoteRecord.WhoAmI()))
				++errorCount;
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
