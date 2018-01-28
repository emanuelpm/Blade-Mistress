
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "uidbanlist.h"
#include ".\helper\GeneralUtils.h"

//***************************************************************************************
//***************************************************************************************
UidBanRecord::UidBanRecord(int doid, char *doname) : DataObject(doid, doname)
{
	uid = 0;
}

//***************************************************************************************
UidBanRecord::~UidBanRecord()
{
}

//******************************************************************
//******************************************************************
UIDBanList::UIDBanList()
{
	Load();
}

//******************************************************************
UIDBanList::~UIDBanList()
{
	Save();
}

//******************************************************************
int UIDBanList::IsBanned(int uid)
{
	UidBanRecord *br = (UidBanRecord *) bannedUIDRecords.First();
	while (br)
	{
		if (uid == br->uid)
		{
			return TRUE;
		}

		br = (UidBanRecord *) bannedUIDRecords.Next();
	}

	return FALSE;
}

//******************************************************************
void UIDBanList::addBannedUID(int uid, char *name)
{
	if (!name[0])
		return;

	UidBanRecord *br = new UidBanRecord(0,name);

	br->uid = uid;

	_strdate( br->dateString );

	bannedUIDRecords.Append(br);

}

//******************************************************************
void UIDBanList::Load(void)
{
	char tempText[1024];

	FILE *fp;
	fp = fopen("serverdata\\bannedUIDs.txt","r");
	if (fp)
	{
		LoadLineToString(fp, tempText);
		while (strcmp(tempText,"DONEJULIAAAAAARD"))
		{
			UidBanRecord *br = new UidBanRecord(0,tempText);
			LoadLineToString(fp, br->dateString);
			fscanf(fp, "%d\n", &(br->uid));

			bannedUIDRecords.Append(br);

			LoadLineToString(fp, tempText);
		}

		fclose(fp);
	}
}

//******************************************************************
void UIDBanList::Save(void)
{
	FILE *fp;
	fp = fopen("serverdata\\bannedUIDs.txt","w");

	UidBanRecord *br = (UidBanRecord *) bannedUIDRecords.First();
	while (br)
	{
		fprintf(fp,"%s\n%s\n%d\n", br->WhoAmI(), br->dateString, br->uid);
		br = (UidBanRecord *) bannedUIDRecords.Next();
	}

	fprintf(fp, "DONEJULIAAAAAARD\n");
	fclose(fp);
}




/* end of file */



