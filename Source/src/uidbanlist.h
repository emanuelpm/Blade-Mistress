#ifndef UIDBANLIST_H
#define UIDBANLIST_H

#include "./helper/linklist.h"
#include "BBO.h"
#include "inventory.h"

class BBOSNpc;

//***************************************************************************************
class UidBanRecord : public DataObject
{
public:

	UidBanRecord(int doid, char *doname);        // mesh
	virtual ~UidBanRecord();

	int uid;
	char dateString[64];
};

//******************************************************************
class UIDBanList
{
public:

	UIDBanList();
	virtual ~UIDBanList();

	int IsBanned(int uid);
	void addBannedUID(int uid, char *name);

	void Load(void);
	void Save(void);

	DoublyLinkedList bannedUIDRecords;

};

#endif
