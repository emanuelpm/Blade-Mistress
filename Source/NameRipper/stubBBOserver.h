#ifndef STUBBBOSERVER_H
#define STUBBBOSERVER_H

#include "linklist.h"
#include "sharedSpace.h"

class BBOSAvatar;
class BBOSMonster;
class BBOSMob;
class BBOSNpc;
class DungeonMap;
class TowerMap;

//***********************************************************************************
class BBOServer
{
public:

	int  FindAvatarInGuild(char *name, SharedSpace **sp);
	void UpdateInventory(BBOSAvatar *avatar);
	BBOSAvatar * FindAvatar(BBOSMob *mobPtr, SharedSpace **sp);

};
/*

	BBOServer(int useIOCP = TRUE);
	virtual ~BBOServer();

	void Tick();
	void HandleMessages(void);
	void TransferAvatar(int intoWorld, int handle);
	BBOSAvatar * FindAvatar(int id, SharedSpace **sp);
	BBOSAvatar * FindAvatar(char *name, char *password, SharedSpace **sp);
	BBOSAvatar * FindAvatarByAvatarName(char *avatarName, SharedSpace **sp);
	BBOSAvatar * FindIncomingAvatar(int id);
	BBOSMonster * FindMonster(BBOSMob *mobPtr, SharedSpace **sp);
	void BuildInventoryInfoStruct(
		         MessInventoryInfo *info, Inventory *inv, int index, int isPlayerData);
	int  TransferItem(BBOSAvatar *avatar, 
									  MessInventoryTransferRequest *transferRequestPtr,
									  long amount = 1, int isGiving = TRUE);
	int  ShiftItem(BBOSAvatar *avatar, MessInventoryChange *inventoryChangePtr);
	void TransferAmount(InventoryObject *io, Inventory *inv, Inventory *partner, long amount);
	void SetWield(int isWielding, InventoryObject *iObject, Inventory *inventory);
	void HandleCombine(BBOSAvatar *avatar, char *skillName);
//	void DoMonsterDrop(Inventory *inv, BBOSMonster *monster);

	void TellClientAboutInventory(BBOSAvatar *avatar, int type);

	void TryToMoveAvatar(int fromSocket, MessAvatarMoveRequest *avMoveReqPtr);

	void AddDungeonSorted(DungeonMap *dm);
	void HandleChatLine(int fromSocket, char *chatText);
	void ProcessWho(int fromSocket, SharedSpace *ss);
	void HandleTeleport(BBOSAvatar *ca, BBOSMob *townmage, 
										 SharedSpace *ss, int forward);
	void HandleKickoff(BBOSAvatar *ca, SharedSpace *sp);
	void HandleTreeTalk(BBOSAvatar *curAvatar, SharedSpace *sp, MessTalkToTree *mt);
	void HandleWordOfPower(BBOSAvatar *ca, SharedSpace *sp);
	void HandleDeadGate(int x, int y, SharedSpace *sp);
	void HandlePetFeeding(MessFeedPetRequest *mess, BBOSAvatar *srcAvatar,
											SharedSpace *ss);
	void HandleAdminMessage(MessAdminMessage *mess, BBOSAvatar *curAvatar,
											SharedSpace *ss);
	void HandleContMonstString(char *input, BBOSAvatar *curAvatar,
											SharedSpace *ss);

	void ProcessFriend(BBOSAvatar *srcAvatar, int type, 
		                SharedSpace *ss, char *targetName = NULL);
	void TellBuddiesImHere(BBOSAvatar *srcAvatar);
	void MaintainIncomingAvatars(void);

	void HandleSellingAll(BBOSAvatar *curAvatar, BBOSNpc *curNpc, int type);
	void HandleExtendedInfo(BBOSAvatar *avatar, 
									 MessExtendedInfoRequest *requestPtr);

	void SendToEveryGuildMate(SharedSpace *sp, 
												 TowerMap *guild, int size, const void *dataPtr);
	void ListGuild(BBOSAvatar *curAvatar, TowerMap *guild);
	void SaltThisMeat(InventoryObject *io);

	NetWorldRadio *	lserver;
	std::vector<TagObjectConnection> tagObjectList;

//	DoublyLinkedList *mobs, *avatars, *incoming, *generators;
	DoublyLinkedList *incoming;
	DoublyLinkedList *spaceList;
	DoublyLinkedList *userMessageList;


	DWORD testTime, dungeonUpdateTime, lastConnectTime;
	int errorContactVal;
	int IOCPFlag;

//	GroundMap *map;

};
*/

extern BBOServer *bboServer;


#endif
