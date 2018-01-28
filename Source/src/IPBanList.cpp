
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "ipBanList.h"
#include ".\helper\GeneralUtils.h"

//***************************************************************************************
//***************************************************************************************
BanRecord::BanRecord(int doid, char *doname) : DataObject(doid, doname)
{
	ip[0] = ip[1] = ip[2] = ip[3] = 0;
}

//***************************************************************************************
BanRecord::~BanRecord()
{
}

//******************************************************************
//******************************************************************
IPBanList::IPBanList()
{
	Load();
}

//******************************************************************
IPBanList::~IPBanList()
{
	Save();
}

//******************************************************************
int IPBanList::IsBanned(unsigned char *ip)
{
	BanRecord *br = (BanRecord *) bannedIPRecords.First();
	while (br)
	{
		if (ip[0] == br->ip[0] &&
			 ip[1] == br->ip[1] &&
			 ip[2] == br->ip[2] &&
			 ip[3] == br->ip[3])
		{
			return TRUE;
		}

		br = (BanRecord *) bannedIPRecords.Next();
	}

	return FALSE;
}

//******************************************************************
void IPBanList::addBannedIP(unsigned char *ip, char *name)
{
	if (!name[0])
		return;

	BanRecord *br = new BanRecord(0,name);

	for (int i = 0; i < 4; ++i)
		br->ip[i] = ip[i];

	_strdate( br->dateString );

	bannedIPRecords.Append(br);

}

//******************************************************************
void IPBanList::Load(void)
{
	char tempText[1024];
	int tempInt[4];

	FILE *fp;
	fp = fopen("serverdata\\bannedIPs.txt","r");
	if (fp)
	{
		LoadLineToString(fp, tempText);
		while (strcmp(tempText,"DONEJULIAAAAAARD"))
		{
			BanRecord *br = new BanRecord(0,tempText);
			LoadLineToString(fp, br->dateString);
			fscanf(fp, "%d %d %d %d\n", &tempInt[0], &tempInt[1], &tempInt[2], &tempInt[3]);
			for (int i = 0; i < 4; ++i)
				br->ip[i] = tempInt[i];

			bannedIPRecords.Append(br);

			LoadLineToString(fp, tempText);
		}

		fclose(fp);
	}
}

//******************************************************************
void IPBanList::Save(void)
{
	FILE *fp;
	fp = fopen("serverdata\\bannedIPs.txt","w");

	BanRecord *br = (BanRecord *) bannedIPRecords.First();
	while (br)
	{
		fprintf(fp,"%s\n%s\n%d %d %d %d\n", br->WhoAmI(), br->dateString,
			(int)br->ip[0], (int)br->ip[1], (int)br->ip[2], (int)br->ip[3]);
		br = (BanRecord *) bannedIPRecords.Next();
	}

	fprintf(fp, "DONEJULIAAAAAARD\n");
	fclose(fp);
}




/* end of file */



