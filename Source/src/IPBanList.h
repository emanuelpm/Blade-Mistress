#ifndef IPBANLIST_H
#define IPBANLIST_H

#include "./helper/linklist.h"
#include "BBO.h"
#include "inventory.h"

class BBOSNpc;

//***************************************************************************************
class BanRecord : public DataObject
{
public:

	BanRecord(int doid, char *doname);        // mesh
	virtual ~BanRecord();

	char ip[4];
	char dateString[64];
};

//******************************************************************
class IPBanList
{
public:

	IPBanList();
	virtual ~IPBanList();

	int IsBanned(unsigned char *ip);
	void addBannedIP(unsigned char *ip, char *name);

	void Load(void);
	void Save(void);

	DoublyLinkedList bannedIPRecords;

};

#endif
