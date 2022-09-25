#ifndef __AUTOUPDATE_H
#define __AUTOUPDATE_H


#include "stdafx.h"

#include "..\src\helper\linklist.h"
#include "..\src\helper\fileFind.h"
#include "..\src\helper\crc.h"
#include "BMLauncher.h"
#include "BMUpdater.h"

#include <chrono>
#include <vector>

struct UpdateServer;

//***************************************************************************************

class FileRecord : public DataObject
{
public:

	FileRecord(int doid, char *doname);
	virtual ~FileRecord();

    std::time_t time;
	DWORD size;
};


class AutoUpdate
{
public:
	AutoUpdate(const UpdateServer& updateServer);
	
	~AutoUpdate() { dialog = 0; }

	void Update();
	void setDialog(HWND dlg) { dialog = dlg; }
	bool IsThereNewLauncher();


private:
	HWND dialog;
	bool newLauncher;
	const UpdateServer& m_updateServer;

    std::vector<FileRecord> ProcessIndexData(std::istream* inStream);
	int DownloadFile(char *fileName);
	void UpdateTextBox(char* txt);
};

#endif
