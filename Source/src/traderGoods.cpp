
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include "traderGoods.h"
#include ".\helper\GeneralUtils.h"
#include "FileTool.h"
#include "BBO-Snpc.h"

//******************************************************************
//******************************************************************
TraderGoods::TraderGoods()
{
	float tempF1, tempF2;

	FileReader *fr = new FileReader("serverdata\\items.txt");

	while (fr->ReadToken())
	{
		InventoryObject *iObject;
		if (!strcmp(fr->valString,"SIMPLE"))
		{
			fr->ReadToken();
			tempF1 = fr->valDouble;
			fr->ReadToken();
			tempF2 = fr->valDouble;
			fr->ReadLine();
			iObject = new InventoryObject(INVOBJ_SIMPLE,0,fr->valString);
			iObject->mass    = tempF1;
			iObject->value   = tempF2;
			iObject->amount  = 1;
			objects.Append(iObject);
		}
		else if (!strcmp(fr->valString,"BLADE"))
		{
			fr->ReadToken();
			tempF1 = fr->valDouble;
			fr->ReadToken();
			tempF2 = fr->valDouble;
			fr->ReadLine();
			iObject = new InventoryObject(INVOBJ_BLADE,0,fr->valString);
			iObject->mass    = tempF1;
			iObject->value   = tempF2;
			iObject->amount  = 1;
			objects.Append(iObject);

			InvBlade *ib     = (InvBlade *)iObject->extra;
			fr->ReadToken();
			ib->damageDone   = fr->valLong;
			fr->ReadToken();
			ib->toHit        = fr->valLong;
			fr->ReadToken();
			ib->size         = fr->valDouble;

			fr->ReadToken();
			ib->r = fr->valLong;
			fr->ReadToken();
			ib->g = fr->valLong;
			fr->ReadToken();
			ib->b = fr->valLong;
		}
		else if (!strcmp(fr->valString,"INGOT"))
		{
			fr->ReadToken();
			tempF1 = fr->valDouble;
			fr->ReadToken();
			tempF2 = fr->valDouble;
			fr->ReadLine();
			iObject = new InventoryObject(INVOBJ_INGOT,0,fr->valString);
			iObject->mass    = tempF1;
			iObject->value   = tempF2;
			iObject->amount  = 1;
			objects.Append(iObject);

			InvIngot *ib     = (InvIngot *)iObject->extra;
			fr->ReadToken();
			ib->damageVal    = fr->valDouble;
			fr->ReadToken();
			ib->challenge    = fr->valDouble;

			fr->ReadToken();
			ib->r = fr->valLong;
			fr->ReadToken();
			ib->g = fr->valLong;
			fr->ReadToken();
			ib->b = fr->valLong;
		}
	}
	delete fr;

}

//******************************************************************
TraderGoods::~TraderGoods()
{
}

//******************************************************************
void TraderGoods::Replenish(BBOSNpc *npc)
{

	// throw away old stuff
	InventoryObject *iObject = (InventoryObject *) npc->inventory->objects.First();
	while (iObject)
	{
		if (INVOBJ_BLADE == iObject->type)
		{
			InvBlade *ib = (InvBlade *) iObject->extra;
			if (iObject->value >= 5000 && 
				 (ib->blind || ib->slow || ib->heal || ib->poison || ib->toHit > 5)
				)
				iObject = (InventoryObject *) npc->inventory->objects.Next();
			else
			{
				npc->inventory->objects.Remove(iObject);
				delete iObject;
				iObject = (InventoryObject *) npc->inventory->objects.First();
			}
		}
		else
		{
			npc->inventory->objects.Remove(iObject);
			delete iObject;
			iObject = (InventoryObject *) npc->inventory->objects.First();
		}
	}

	// add new stuff
	iObject = (InventoryObject *) objects.First();
	while (iObject)
	{
		InventoryObject *i2 = new InventoryObject(iObject->type,0,iObject->WhoAmI());
		iObject->CopyTo(i2);
		i2->amount = 100;
		npc->inventory->AddItemSorted(i2);
		iObject = (InventoryObject *) objects.Next();
	}


}



/* end of file */



