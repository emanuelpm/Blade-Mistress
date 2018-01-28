
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "./helper/GeneralUtils.h"
#include "BBO-Smonster.h"
#include "MapList.h"
#include "version.h"

//******************************************************************
MapList::MapList(int w, int h)
{
	rowW = w;
	rowH = h;
	rows = new DoublyLinkedList[rowW];

	curSortRow = 0;
	state.curMob = NULL;
}


//******************************************************************
MapList::~MapList()
{
	delete[] rows;
}

//******************************************************************
void MapList::Add(BBOSMob *mob)
{
	assert(mob);
	assert(mob->cellX >= 0);
	assert(mob->cellY >= 0);
	assert(mob->cellX < rowW);
	assert(mob->cellY < rowH);
	rows[mob->cellX].Append(mob);
}


//******************************************************************
void MapList::Remove(BBOSMob *mob)
{
	// check to see if it's in the proper list
	if (rows[mob->cellX].Find(mob))
	{
		rows[mob->cellX].Remove(mob);
		return;
	}

	// try to find it in any list
	for (int i = 0; i < rowW; ++i)
	{
		if (rows[i].Find(mob))
		{
			rows[i].Remove(mob);
			return;
		}
	}


}


//******************************************************************
void MapList::Move(BBOSMob *mob)
{
	// check to see if it's in the proper list already
	if (rows[mob->cellX].Find(mob))
		return;

	// try to find it in any list
	for (int i = 0; i < rowW; ++i)
	{
		if (rows[i].Find(mob))
		{
			rows[i].Remove(mob);
			rows[mob->cellX].Append(mob);
			return;
		}
	}

	// should never get here
	assert(0);
}


//******************************************************************
void MapList::Tick(void)
{
	// validate and maintain integrity of one row

	++curSortRow;
	if (curSortRow >= rowW)
		curSortRow = 0;

	BBOSMob *mob = (BBOSMob *) rows[curSortRow].First();
	while (mob)
	{
		if (mob->cellX != curSortRow)
		{
			rows[curSortRow].Remove();
			rows[mob->cellX].Append(mob);
		}

		mob = (BBOSMob *) rows[curSortRow].Next();
	}

}


//******************************************************************
BBOSMob *MapList::GetFirst(int x, int y, int radius)
{

	// init state
	state.searchX = x;
	state.searchY = y;
	state.searchRadius = radius;
	state.curMob = NULL;

	// find valid first
	state.curRowIndex = state.searchX - state.searchRadius;
	if (state.curRowIndex < 0)
		state.curRowIndex = 0;

	if (state.curRowIndex >= rowW)
		return NULL;

	state.curMob = (BBOSMob *) rows[state.curRowIndex].First();
	while (!state.curMob)
	{
		++state.curRowIndex;
		if (state.curRowIndex > state.searchX + state.searchRadius ||
			 state.curRowIndex >= rowW)
			return NULL;

		state.curMob = (BBOSMob *) rows[state.curRowIndex].First();
	}

	if (state.curMob->cellY <= state.searchY + state.searchRadius &&
		      state.curMob->cellY >= state.searchY - state.searchRadius)
		return state.curMob;
	else
		return GetNext();
}


//******************************************************************
BBOSMob *MapList::GetNext (void)
{
	while (state.curMob)
	{
		state.curMob = (BBOSMob *) rows[state.curRowIndex].Next();
		while (!state.curMob)
		{
			++state.curRowIndex;
			if (state.curRowIndex > state.searchX + state.searchRadius ||
				 state.curRowIndex >= rowW)
				return NULL;

			state.curMob = (BBOSMob *) rows[state.curRowIndex].First();
		}

		if (state.curMob->cellY <= state.searchY + state.searchRadius &&
			      state.curMob->cellY >= state.searchY - state.searchRadius)
			return state.curMob;
	}
	return NULL;
}


//******************************************************************
void MapList::SetToLast(void)
{
	state.curMob = NULL;
}

//******************************************************************
BBOSMob *MapList::IsInList(BBOSMob *target, int tryAll)
{

	if (!target)
		return NULL;

	BBOSMob * retVal = NULL;

	if (!tryAll)
	{
		if (target->cellX < 0 || target->cellX >= rowW)
			return NULL;
	}
	// check to see if it's in the proper list
	if (!tryAll && rows[target->cellX].Find(target))
	{
		retVal = target;
	}
	else
	{
		// try to find it in any list
		for (int i = 0; i < rowW; ++i)
		{
			if (rows[i].Find(target))
			{
				retVal = target;
			}
		}
	}

	if (state.curMob)
		rows[state.curRowIndex].Find(state.curMob);

	return retVal;
}

//******************************************************************
MapListState MapList::GetState(void)
{
	return state;
}


//******************************************************************
void MapList::SetState(MapListState heldState)
{
	state = heldState;
	state.curMob = (BBOSMob *) rows[state.curRowIndex].Find(state.curMob);

	assert(state.curMob);
}



/* end of file */



