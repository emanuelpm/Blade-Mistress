
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "stubBBOServer.h"

BBOServer *bboServer = NULL; // better be only one instance of this server at a time!

int lastAvatarCount = 0;


//******************************************************************
int  BBOServer::FindAvatarInGuild(char *name, SharedSpace **sp)
{

	return 0;
}

//******************************************************************
void BBOServer::UpdateInventory(BBOSAvatar *avatar)
{

}

//******************************************************************
BBOSAvatar * BBOServer::FindAvatar(BBOSMob *mobPtr, SharedSpace **sp)
{

	return NULL;
}


/* end of file */
