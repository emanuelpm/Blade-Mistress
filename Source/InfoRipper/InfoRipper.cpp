// NameRipper.cpp : Defines the entry point for the console application.
//

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//#include "stdafx.h"
#include "fileFind.h"

#include "BBO-SAvatar.h"
#include "BBO.h"
#include "GeneralUtils.h"
#include "UniqueNames.h"
#include "version.h"

DoublyLinkedList accountList;

DoublyLinkedList dataList, tempList;

//***************************************************************************************
class NameRecord : public DataObject
{
public:

	NameRecord(int doid, char *doname);

	DataObject *userFile;
	int index;

};

//***************************************************************************************
NameRecord::NameRecord(int doid, char *doname)	 : DataObject(doid,doname)
{
}

long avatarLifeArray[50];
long accountLifeArray[50];

//***************************************************************************************
//***************************************************************************************
int main(int argc, char* argv[])
{

	//*********** load EVERY account into memory
	char tempText[1024];

	FileNameList *list = GetNameList("users\\temp\\*.use", "");

	for (int i = 0; i < list->numOfFiles; ++i)
	{
		// looking for a period, which would indicate a file
		int isFile = FALSE;
		for (int j = 0; j < (int) strlen(list->nameList[i].name); ++j)
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
			else
			{
				sprintf(tempText,"users\\temp\\%s", list->nameList[i].name);

				BBOSAvatar *curAvatar = new BBOSAvatar();
				curAvatar->LoadAccount(tempText, NULL, FALSE, TRUE);
				accountList.Append(curAvatar);
			}
		}

		if (!(i % 100))
			printf(".");
	}

	for (i = 0; i < 50; ++i)
	{
		avatarLifeArray[i] = 0;
		accountLifeArray[i] = 0;
	}


	// *************    for each account
	BBOSAvatar *curAvatar = (BBOSAvatar *) accountList.First();
	while (curAvatar)
	{
		int totalLifeTime = 0;

		// for each avatar
		for (int i = 0; i < NUM_OF_CHARS_PER_USER; ++i)
		{
			if (curAvatar->charInfoArray[i].topIndex > -1)
			{
				// collect pertinant data: name, userfile, index, and age
				NameRecord *nr = new NameRecord(
						curAvatar->charInfoArray[i].lifeTime, 
						curAvatar->charInfoArray[i].name);

				nr->do_name[28] = 0; // truncate a bit less than 32 for numbers
				nr->index = i;
				nr->userFile = curAvatar;

				totalLifeTime += curAvatar->charInfoArray[i].lifeTime;

				if (curAvatar->charInfoArray[i].lifeTime < 10000)
					++(avatarLifeArray[curAvatar->charInfoArray[i].lifeTime / (10000/50)]);

				// add data to name list
				dataList.Append(nr);
			}
		}

		if (totalLifeTime < 10000)
			++(accountLifeArray[totalLifeTime/ (10000/50)]);

		curAvatar = (BBOSAvatar *) accountList.Next();
	}

/*
	FILE *fp = fopen("DustAmounts.txt","w");

	fprintf(fp,"Account Name, Character Name, dust amounts\n");

	curAvatar = (BBOSAvatar *) accountList.First();
	while (curAvatar)
	{
//		long totalValue = 0;

		for (int i = 0; i < NUM_OF_CHARS_PER_USER; ++i)
		{
			if (curAvatar->charInfoArray[i].topIndex > -1)
			{
				CleanString(curAvatar->name);
				fprintf(fp,"%s, %s, ", curAvatar->name, curAvatar->charInfoArray[i].name);

				long value = 0;

				InventoryObject *io = (InventoryObject *) 
					       curAvatar->charInfoArray[i].inventory.objects.First();
				while (io)
				{
					if (INVOBJ_INGREDIENT == io->type)
						value += io->amount;

					io = (InventoryObject *) curAvatar->charInfoArray[i].inventory.objects.Next();
				}

				io = (InventoryObject *) curAvatar->charInfoArray[i].wield.objects.First();
				while (io)
				{
					if (INVOBJ_INGREDIENT == io->type)
						value += io->amount;

					io = (InventoryObject *) curAvatar->charInfoArray[i].wield.objects.Next();
				}

				io = (InventoryObject *) curAvatar->charInfoArray[i].workbench.objects.First();
				while (io)
				{
					if (INVOBJ_INGREDIENT == io->type)
						value += io->amount;

					io = (InventoryObject *) curAvatar->charInfoArray[i].workbench.objects.Next();
				}

				fprintf(fp,"%ld\n", value);
			}
		}
		curAvatar = (BBOSAvatar *) accountList.Next();
	}

	fclose(fp);
*/
/*
	FILE *fp = fopen("playerAgeInfo.txt","w");
	if (fp)
	{
		for (i = 0; i < 49; ++i)
		{
			fprintf(fp,"%d, ", i * 5);
			fprintf(fp,"%d, %d\n", avatarLifeArray[i], accountLifeArray[i]);
		}

		fclose(fp);
	}
*/



/*
	FILE *fp = fopen("CharKarmaInfo.txt","w");

	fprintf(fp,"User Name, Character Name, Character Age, ");

#if 0	
			if (sheGave && !iGave)
			{
				partnerAvatar->charInfoArray[partnerAvatar->curCharacterIndex].
					  karmaGiven[herRelationship][SAMARITAN_TYPE_GIFT] += 1;
				charInfoArray[curCharacterIndex].
					  karmaReceived[myRelationship][SAMARITAN_TYPE_GIFT] += 1;

				partnerAvatar->LogKarmaExchange(
					       this, herRelationship, myRelationship, SAMARITAN_TYPE_GIFT);

				if (IsAGuildMate(partnerAvatar))
				{
					partnerAvatar->charInfoArray[partnerAvatar->curCharacterIndex].
						  karmaGiven[SAMARITAN_REL_GUILD][SAMARITAN_TYPE_GIFT] += 1;
					charInfoArray[curCharacterIndex].
						  karmaReceived[SAMARITAN_REL_GUILD][SAMARITAN_TYPE_GIFT] += 1;

					partnerAvatar->LogKarmaExchange(
					     this, SAMARITAN_REL_GUILD, SAMARITAN_REL_GUILD, SAMARITAN_TYPE_GIFT);
				}
			}
#endif	
	fprintf(fp,"Thanks Given as Elder, Thanks Recieved as Elder, "); 
	fprintf(fp,"Please Given as Elder, Please Recieved as Elder, "); 
	fprintf(fp,"Welcome Given as Elder, Welcome Recieved as Elder, "); 
	fprintf(fp,"Gift Given as Elder, Gift Recieved as Elder, "); 
	fprintf(fp,"Cash Given as Elder, Cash Recieved as Elder, "); 

	fprintf(fp,"Thanks Given as Peer, Thanks Recieved as Peer, "); 
	fprintf(fp,"Please Given as Peer, Please Recieved as Peer, "); 
	fprintf(fp,"Welcome Given as Peer, Welcome Recieved as Peer, "); 
	fprintf(fp,"Gift Given as Peer, Gift Recieved as Peer, "); 
	fprintf(fp,"Cash Given as Peer, Cash Recieved as Peer, "); 

	fprintf(fp,"Thanks Given as Younger, Thanks Recieved as Younger, "); 
	fprintf(fp,"Please Given as Younger, Please Recieved as Younger, "); 
	fprintf(fp,"Welcome Given as Younger, Welcome Recieved as Younger, "); 
	fprintf(fp,"Gift Given as Younger, Gift Recieved as Younger, "); 
	fprintf(fp,"Cash Given as Younger, Cash Recieved as Younger, "); 

	fprintf(fp,"Thanks Given as Guildmate, Thanks Recieved as Guildmate, "); 
	fprintf(fp,"Please Given as Guildmate, Please Recieved as Guildmate, "); 
	fprintf(fp,"Welcome Given as Guildmate, Welcome Recieved as Guildmate, "); 
	fprintf(fp,"Gift Given as Guildmate, Gift Recieved as Guildmate, "); 
	fprintf(fp,"Cash Given as Guildmate, Cash Recieved as Guildmate, "); 

	fprintf(fp,"END\n"); 

	curAvatar = (BBOSAvatar *) accountList.First();
	while (curAvatar)
	{
		for (int i = 0; i < NUM_OF_CHARS_PER_USER; ++i)
		{
			if (curAvatar->charInfoArray[i].topIndex > -1)
			{
				CleanString(curAvatar->name);
				fprintf(fp,"%s, %s, ", 
					         curAvatar->name, curAvatar->charInfoArray[i].name);

//				float minutes = curAvatar->charInfoArray[i].lifeTime * 5.0f + 1;
				fprintf(fp,"%ld, ", curAvatar->charInfoArray[i].lifeTime * 5);

				for (int j = 0; j < SAMARITAN_REL_MAX; ++j)
				{
					for (int k = 0; k < SAMARITAN_TYPE_MAX; ++k)
					{
						fprintf(fp,"%4.6f, %4.6f, ", 
						  (float)curAvatar->charInfoArray[i].karmaGiven[j][k], // / minutes * 100,
						  (float)curAvatar->charInfoArray[i].karmaReceived[j][k]); // / minutes * 100);
					}
				}

				fprintf(fp,"END\n");
			}
		}
		curAvatar = (BBOSAvatar *) accountList.Next();
	}

	fclose(fp);

*/




	/*
	// for each item in name list
	NameRecord *nr = (NameRecord *) dataList.First();
	while (nr)
	{
		int resetFlag = FALSE;

		// find every other item with the same name, moving them into the holding list
		NameRecord *nr2 = (NameRecord *) dataList.First();
		while (nr2)
		{
			if (nr2 != nr && IsCompletelyVisiblySame(nr->WhoAmI(), nr2->WhoAmI()))
			{
				dataList.Remove(nr2);
				tempList.Append(nr2);
				nr2 = (NameRecord *) dataList.First();
			}
			else
				nr2 = (NameRecord *) dataList.Next();
		}

		dataList.Find(nr);

		// if there's anything in the holding list
		nr2 = (NameRecord *) tempList.First();
		if (nr2)
		{
			printf("%s\n",nr2->WhoAmI());

			// move the item into the holding list
			dataList.Remove(nr);
			tempList.Append(nr);

			// sort the holding list by age
			for (int j = 0; j < tempList.ItemsInList(); ++j)
			{
				nr2 = (NameRecord *) tempList.Last();
				tempList.Remove(nr2);

				nr  = (NameRecord *) tempList.First();
				while (nr)
				{
					if (nr->WhatAmI() > nr2->WhatAmI())
						nr = (NameRecord *) tempList.Next();
					else
					{
						tempList.AddBefore(nr2,nr);
						nr = NULL;
						nr2 = NULL;
					}
				}

				if (nr2)
					tempList.Append(nr2);
			}

			// change the names in the holding list so they aren't identical any more
			int num = 0;
			nr  = (NameRecord *) tempList.First();
			while (nr)
			{
				if (num > 0)
				{
					sprintf(tempText,"%s%d",nr->WhoAmI(), num+1);
					sprintf(nr->do_name, tempText);
				}
				nr  = (NameRecord *) tempList.Next();
				++num;
			}

			// put the items back into the end of the name list
			nr  = (NameRecord *) tempList.First();
			while (nr)
			{
				tempList.Remove(nr);
				dataList.Append(nr);
				nr  = (NameRecord *) tempList.First();
			}

			// reset the name list loop to the top
			resetFlag = TRUE;
		}

		if (resetFlag)
			nr = (NameRecord *) dataList.First();
		else
			nr = (NameRecord *) dataList.Next();
	}

	// for each item in name list
	nr  = (NameRecord *) dataList.First();
	while (nr)
	{
		// change the account information
		BBOSAvatar *curAvatar = (BBOSAvatar *) nr->userFile;
		sprintf(curAvatar->charInfoArray[nr->index].name, nr->WhoAmI());
		nr  = (NameRecord *) dataList.Next();
	}
	*/


	FILE *fp = fopen("SwordData.txt","w");

	fprintf(fp,"Account Name, Character Name, Character Age, Character Level, ");
	fprintf(fp,"Weapon Type, Weapon Name, Weapon Value, ");
	fprintf(fp,"damageDone, toHit, Size, Poison, Blinding, Slowing, Healing, Age\n");

	curAvatar = (BBOSAvatar *) accountList.First();
	while (curAvatar)
	{
//		long totalValue = 0;

		for (int i = 0; i < NUM_OF_CHARS_PER_USER; ++i)
		{
			if (curAvatar->charInfoArray[i].topIndex > -1 &&
				 !(ACCOUNT_TYPE_ADMIN == curAvatar->accountType ||
					ACCOUNT_TYPE_MODERATOR == curAvatar->accountType)
				)
			{
				CleanString(curAvatar->name);

				long value = 0;

				//********** inventory
				InventoryObject *io = (InventoryObject *) 
					       curAvatar->charInfoArray[i].inventory->objects.First();
				while (io)
				{
					if (INVOBJ_BLADE == io->type)
					{

						fprintf(fp,"%s, %s, %ld, %d, ", 
							     curAvatar->name, curAvatar->charInfoArray[i].name,
							     curAvatar->charInfoArray[i].lifeTime, 
								  (int)curAvatar->charInfoArray[i].cLevel);

						InvBlade *ib = (InvBlade *) io->extra;

						if (BLADE_TYPE_KATANA == ib->type)
							fprintf(fp,"KATANA, %s, %f, ", io->WhoAmI(), io->value);
						else if (BLADE_TYPE_CLAWS == ib->type)
							fprintf(fp,"CLAWS, %s, %f, ", io->WhoAmI(), io->value);
						else if (BLADE_TYPE_CHAOS == ib->type)
							fprintf(fp,"CHAOS, %s, %f, ", io->WhoAmI(), io->value);
						else if (BLADE_TYPE_DOUBLE == ib->type)
							fprintf(fp,"BLADESTAFF, %s, %f, ", io->WhoAmI(), io->value);
						else if (BLADE_TYPE_MACE == ib->type)
							fprintf(fp,"MACE, %s, %f, ", io->WhoAmI(), io->value);
						else
							fprintf(fp,"BLADE, %s, %f, ", io->WhoAmI(), io->value);

						fprintf(fp,"%ld, %ld, %f, %d, %d, %d, %d, %d\n", 
							ib->damageDone, ib->toHit, ib->size, ib->poison, 
							ib->blind, ib->slow, ib->heal, ib->numOfHits);
					}

					io = (InventoryObject *) curAvatar->charInfoArray[i].inventory->objects.Next();
				}

				//********** wield
				io = (InventoryObject *) 
					       curAvatar->charInfoArray[i].wield->objects.First();
				while (io)
				{
					if (INVOBJ_BLADE == io->type)
					{

						fprintf(fp,"%s, %s, %ld, %d, ", 
							     curAvatar->name, curAvatar->charInfoArray[i].name,
							     curAvatar->charInfoArray[i].lifeTime, 
								  (int)curAvatar->charInfoArray[i].cLevel);

						InvBlade *ib = (InvBlade *) io->extra;

						if (BLADE_TYPE_KATANA == ib->type)
							fprintf(fp,"KATANA, %s, %f, ", io->WhoAmI(), io->value);
						else if (BLADE_TYPE_CLAWS == ib->type)
							fprintf(fp,"CLAWS, %s, %f, ", io->WhoAmI(), io->value);
						else if (BLADE_TYPE_CHAOS == ib->type)
							fprintf(fp,"CHAOS, %s, %f, ", io->WhoAmI(), io->value);
						else if (BLADE_TYPE_DOUBLE == ib->type)
							fprintf(fp,"BLADESTAFF, %s, %f, ", io->WhoAmI(), io->value);
						else if (BLADE_TYPE_MACE == ib->type)
							fprintf(fp,"MACE, %s, %f, ", io->WhoAmI(), io->value);
						else
							fprintf(fp,"BLADE, %s, %f, ", io->WhoAmI(), io->value);

						fprintf(fp,"%ld, %ld, %f, %d, %d, %d, %d, %d\n", 
							ib->damageDone, ib->toHit, ib->size, ib->poison, 
							ib->blind, ib->slow, ib->heal, ib->numOfHits);
					}

					io = (InventoryObject *) curAvatar->charInfoArray[i].wield->objects.Next();
				}

				//********** workbench
				io = (InventoryObject *) 
					       curAvatar->charInfoArray[i].workbench->objects.First();
				while (io)
				{
					if (INVOBJ_BLADE == io->type)
					{

						fprintf(fp,"%s, %s, %ld, %d, ", 
							     curAvatar->name, curAvatar->charInfoArray[i].name,
							     curAvatar->charInfoArray[i].lifeTime, 
								  (int)curAvatar->charInfoArray[i].cLevel);

						InvBlade *ib = (InvBlade *) io->extra;

						if (BLADE_TYPE_KATANA == ib->type)
							fprintf(fp,"KATANA, %s, %f, ", io->WhoAmI(), io->value);
						else if (BLADE_TYPE_CLAWS == ib->type)
							fprintf(fp,"CLAWS, %s, %f, ", io->WhoAmI(), io->value);
						else if (BLADE_TYPE_CHAOS == ib->type)
							fprintf(fp,"CHAOS, %s, %f, ", io->WhoAmI(), io->value);
						else if (BLADE_TYPE_DOUBLE == ib->type)
							fprintf(fp,"BLADESTAFF, %s, %f, ", io->WhoAmI(), io->value);
						else if (BLADE_TYPE_MACE == ib->type)
							fprintf(fp,"MACE, %s, %f, ", io->WhoAmI(), io->value);
						else
							fprintf(fp,"BLADE, %s, %f, ", io->WhoAmI(), io->value);

						fprintf(fp,"%ld, %ld, %f, %d, %d, %d, %d, %d\n", 
							ib->damageDone, ib->toHit, ib->size, ib->poison, 
							ib->blind, ib->slow, ib->heal, ib->numOfHits);
					}

					io = (InventoryObject *) curAvatar->charInfoArray[i].workbench->objects.Next();
				}

			}
		}

		// find swords in bank, too.

		InventoryObject *io = (InventoryObject *) 
			       curAvatar->bank->objects.First();
		while (io)
		{
			if (INVOBJ_BLADE == io->type &&
				 !(ACCOUNT_TYPE_ADMIN == curAvatar->accountType ||
					ACCOUNT_TYPE_MODERATOR == curAvatar->accountType)
				)
			{

				fprintf(fp,"%s, BANK, 0, 0, ", curAvatar->name);

				InvBlade *ib = (InvBlade *) io->extra;

				if (BLADE_TYPE_KATANA == ib->type)
					fprintf(fp,"KATANA, %s, %f, ", io->WhoAmI(), io->value);
				else if (BLADE_TYPE_CLAWS == ib->type)
					fprintf(fp,"CLAWS, %s, %f, ", io->WhoAmI(), io->value);
				else if (BLADE_TYPE_CHAOS == ib->type)
					fprintf(fp,"CHAOS, %s, %f, ", io->WhoAmI(), io->value);
				else if (BLADE_TYPE_DOUBLE == ib->type)
					fprintf(fp,"BLADESTAFF, %s, %f, ", io->WhoAmI(), io->value);
				else if (BLADE_TYPE_MACE == ib->type)
					fprintf(fp,"MACE, %s, %f, ", io->WhoAmI(), io->value);
				else
					fprintf(fp,"BLADE, %s, %f, ", io->WhoAmI(), io->value);

				fprintf(fp,"%ld, %ld, %f, %d, %d, %d, %d, %d\n", 
					ib->damageDone, ib->toHit, ib->size, ib->poison, 
					ib->blind, ib->slow, ib->heal, ib->numOfHits);
			}

			io = (InventoryObject *) curAvatar->bank->objects.Next();
		}


		curAvatar = (BBOSAvatar *) accountList.Next();
	}

	fclose(fp);



	return 0;
}
