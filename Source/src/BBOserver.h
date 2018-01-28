#ifndef BBOSERVER_H
#define BBOSERVER_H

#include ".\helper\linklist.h"
#include ".\network\server.h"
#include ".\network\IOCPServer.h"
#include "BBO.h"
#include "inventory.h"
#include ".\network\NetWorldMessages.h"
#include "ground-map.h"
#include "tower-map.h"

class BBOSAvatar;
class BBOSMonster;
class BBOSMob;
class BBOSNpc;
class DungeonMap;
class TowerMap;
class Quest;
class BBOSTree;
class QuestPart;

//***********************************************************************************
struct TagObjectConnection
{
	long id;
//	NetWorldObject * object;
};

//*********************************************************************
class UserMessage : public DataObject
{
public:

	UserMessage(unsigned long connectionID, char *name);
	virtual ~UserMessage();

	char password[128];
	char avatar[128];
	char message[128];
	DWORD color, age;
	long value1,value2,value3;

};

//***********************************************************************************
class BBOServer
{
public:

	BBOServer(int useIOCP = TRUE);
	virtual ~BBOServer();

	void Tick();
	void HandleMessages(void);
	void TransferAvatar(int intoWorld, int handle);
	BBOSAvatar * FindAvatar(int id, SharedSpace **sp);
	BBOSAvatar * FindAvatar(BBOSMob *mobPtr, SharedSpace **sp);
	BBOSAvatar * FindAvatar(char *name, char *password, SharedSpace **sp);
	BBOSAvatar * FindAvatarByAvatarName(char *avatarName, SharedSpace **sp);
	BBOSAvatar * FindAvatarByStringName(char *string, int &length, SharedSpace **retSpace);
	BBOSAvatar * FindAvatarByPartialName(char *avatarName, SharedSpace **sp);
	BBOSAvatar * FindIncomingAvatar(int id);
	BBOSMonster * FindMonster(BBOSMob *mobPtr, SharedSpace **sp);
	void BuildInventoryInfoStruct(
		         MessInventoryInfo *info, Inventory *inv, int index, int isPlayerData);
	void UpdateInventory(BBOSAvatar *avatar);
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
	void ProcessWho(BBOSAvatar *ca, SharedSpace *ss);
	void HandleTeleport(BBOSAvatar *ca, BBOSMob *townmage, 
										 SharedSpace *ss, int forward);
	void HandleKickoff(BBOSAvatar *ca, SharedSpace *sp);
	void HandleTreeTalk(BBOSAvatar *curAvatar, SharedSpace *sp, MessTalkToTree *mt);
	void HandleWordOfPower(BBOSAvatar *ca, SharedSpace *sp);
	void HandleDeadGate   (int x, int y, SharedSpace *sp);
	void HandleSpiritGate (int x, int y, SharedSpace *sp);
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

	// GUILD STUFF
	int  FindAvatarInGuild   (char *name, SharedSpace **sp);
	MemberRecord* GetRecordForGuildMember( char *name, SharedSpace **sp );
	int  FindGuild(DWORD nameCRC, SharedSpace **sp);
	int  DeleteNameFromGuild (char *name, SharedSpace **sp);
	int  ChangeAvatarGuildName(char *name, char *newName);
	void SendToEveryGuildMate(char *senderName, SharedSpace *sp, 
												 TowerMap *guild, int size, const void *dataPtr);
	void ListGuild(BBOSAvatar *curAvatar, TowerMap *guild);
	void SaltThisMeat(InventoryObject *io);
	void CheckGraveyard(SharedSpace *ss, int gX, int gY);

	void HandleKeyCode(BBOSAvatar *avatar, MessKeyCode *keyCodePtr);
	void HandleKarmaText(char *string, BBOSAvatar *curAvatar, BBOSAvatar *targetAv);

	void ListVotes(                     BBOSAvatar *curAvatar, TowerMap *guild);
	void DetailVote(             int i, BBOSAvatar *curAvatar, TowerMap *guild);
	void VoteOnBill(int voteVal, int i, BBOSAvatar *curAvatar, TowerMap *guild);

	void AttemptToStartVote(BBOSAvatar *curAvatar, TowerMap *guild, 
		                     char *subjectName, char *type);

	void CreateTreeQuest(Quest *quest, BBOSAvatar *curAvatar, 
	    									  SharedSpace *sp,BBOSTree *t, int forcedType = -1);
	int  ResolveTreeQuests(BBOSAvatar *curAvatar, 
											  SharedSpace *sp,BBOSTree *t);
	void CreateDemonPrince(SharedSpace *ss, BBOSAvatar *curAvatar, QuestPart *qt, char *tempText2);
	void HandleEarthKeyUse(BBOSAvatar *ca, InvEarthKey *iek, SharedSpace *ss);

	void AddWarpPair(SharedSpace *s1, int x1, int y1,
		  				  SharedSpace *s2, int x2, int y2, 
		  				  int allUse = TRUE);
	NetWorldRadio *	lserver;
	std::vector<TagObjectConnection> tagObjectList;

//	DoublyLinkedList *mobs, *avatars, *incoming, *generators;
	DoublyLinkedList *incoming;
	DoublyLinkedList *spaceList;
	DoublyLinkedList *userMessageList;

	DWORD testTime, dungeonUpdateTime, lastConnectTime, lastGraveyardTime, 
		   tenSecondTime, dayTimeCounter, smasherCounter, labyMapTime;
	int errorContactVal;
	int IOCPFlag;

//	GroundMap *map;
	LongTime countDownTime;
	int pleaseKillMe, weatherState;

};

extern BBOServer *bboServer;


#endif
