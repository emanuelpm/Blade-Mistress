#include <assert.h>
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

//#include "thomdebug.h"
#include "UIR_ArtTextButton.h"


//**********************************************************************************
//**********************************************************************************
UIRectArtTextButton::UIRectArtTextButton(int doid, RECT boxToCopy, char *doname, 
       UIRect *xparent)
	: UIRectTextButton(doid,boxToCopy, doname, xparent)
{
	for (int i = 0; i < UIRECT_BCOL_MAX; ++i)
		art[i] = NULL;
	artOffsetX = artOffsetY = 0;
}

//**********************************************************************************
UIRectArtTextButton::UIRectArtTextButton(int doid, int left, int top, int right, int bottom, 
                                   char *doname, UIRect *xparent)
	: UIRectTextButton(doid,left, top, right, bottom, doname, xparent)
{
	for (int i = 0; i < UIRECT_BCOL_MAX; ++i)
		art[i] = NULL;
	artOffsetX = artOffsetY = 0;
}

//**********************************************************************************
UIRectArtTextButton::~UIRectArtTextButton()
{
	for (int i = 0; i < UIRECT_BCOL_MAX; ++i)
		SAFE_RELEASE(art[i]);
}

//**********************************************************************************
void UIRectArtTextButton::SetArt(int type, char *fileName, DWORD transparency)
{
	D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, fileName,
							0,0,0,0,	D3DFMT_A8R8G8B8, D3DPOOL_MANAGED,
							D3DX_DEFAULT, D3DX_DEFAULT, transparency, NULL, NULL, &art[type]);
}


//**********************************************************************************
void UIRectArtTextButton::DrawSelf(void)
{

	LPDIRECT3DTEXTURE8 artToUse = art[UIRECT_BCOL_NORMAL   ];
   if (isActive)
      artToUse = art[UIRECT_BCOL_ACTIVE   ];
   if (isDepressed && isActive && isMomentary)
      artToUse = art[UIRECT_BCOL_DEPRESSED   ];
   if (isDepressed && !isMomentary)
      artToUse = art[UIRECT_BCOL_DEPRESSED   ];
   if (isDisabled)
      artToUse = art[UIRECT_BCOL_DISABLED   ];

	if (!artToUse)
		artToUse = art[UIRECT_BCOL_NORMAL   ];

	RECT artBox = absBox;
	artBox.top  += artOffsetY;
	artBox.left += artOffsetX;

   if (artToUse)
   {
		puma->DrawRect(artToUse,artBox, 0xffffffff);
   }

   UIRectTextButton::DrawSelf();

}


/* end of file */



