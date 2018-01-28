//***************************************************************************************
// Mesh object designed for PUMA and spaceship parts.
//***************************************************************************************

#include ".\puma\puma.h"
#include "AvatarTexture.h"
#include ".\puma\noise.h"
#include "BBO.h"

LPDIRECT3DTEXTURE8 AvatarTexture::faces[NUM_OF_FACES];
LPDIRECT3DTEXTURE8 AvatarTexture::tops[NUM_OF_TOPS];
LPDIRECT3DTEXTURE8 AvatarTexture::bottoms[NUM_OF_BOTTOMS];
LPDIRECT3DTEXTURE8 AvatarTexture::skin;
LPDIRECT3DTEXTURE8 AvatarTexture::hair;
LPDIRECT3DTEXTURE8 AvatarTexture::helperMark;
LPDIRECT3DTEXTURE8 AvatarTexture::dokkMark;
LPDIRECT3DTEXTURE8 AvatarTexture::anubisMark;
LPDIRECT3DTEXTURE8 AvatarTexture::overlordMark;
int AvatarTexture::bitmapRefCount = 0;

int useSimpleAvatarTextureFlag;

//***************************************************************************************
AvatarTexture::AvatarTexture(int doid, char *doname)	 : DataObject(doid,doname)
{
	Init();
}

//***************************************************************************************
AvatarTexture::AvatarTexture(void) : DataObject(0,"PLASMA_TEXTURE")
{
	Init();
}

//***************************************************************************************
void AvatarTexture::Init(void)
{
	currentBitmap = NULL;

	_D3DFORMAT format = D3DFMT_A8R8G8B8;
//	_D3DFORMAT format = D3DFMT_X8R8G8B8;

	if (bitmapRefCount < 1)
	{
		//************
//		HRESULT hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, fileName,
//							0,0,0,0,	format, D3DPOOL_MANAGED, D3DX_DEFAULT,
//							D3DX_DEFAULT, transparentColor, NULL, NULL, &pTexture);

		// DebugOutput("1 ");
		HRESULT hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\face1.png",
							0,0,1,0,	format, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0xffffffff, NULL, NULL, 
							&faces[0]);
		// DebugOutput("2 ");
		hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\face2.png",
							0,0,1,0,	format, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0xffffffff, NULL, NULL, 
							&faces[1]);
		// DebugOutput("3 ");
		hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\face3.png",
							0,0,1,0,	format, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0xffffffff, NULL, NULL, 
							&faces[2]);
		// DebugOutput("4 ");
		hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\face4.png",
							0,0,1,0,	format, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0xffffffff, NULL, NULL, 
							&faces[3]);
		// DebugOutput("5 ");
		hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\face5.png",
							0,0,1,0,	format, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0xffffffff, NULL, NULL, 
							&faces[4]);
		// DebugOutput("6 ");
		hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\face6.png",
							0,0,1,0,	format, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0xffffffff, NULL, NULL, 
							&faces[5]);



		//************
		// DebugOutput("7 ");
		hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\top1.png",
							0,0,1,0,	format, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0xffffffff, NULL, NULL, 
							&tops[0]);
		// DebugOutput("8 ");
		hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\top2.png",
							0,0,1,0,	format, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0xffffffff, NULL, NULL, 
							&tops[1]);
		// DebugOutput("9 ");
		hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\top3.png",
							0,0,1,0,	format, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0xffffffff, NULL, NULL, 
							&tops[2]);
		// DebugOutput("10 ");
		hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\top4.png",
							0,0,1,0,	format, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0xffffffff, NULL, NULL, 
							&tops[3]);
		// DebugOutput("11 ");
		hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\top5.png",
							0,0,1,0,	format, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0xffffffff, NULL, NULL, 
							&tops[4]);


		//************
		// DebugOutput("12 ");
		hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\bottom1.png",
							0,0,1,0,	format, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0xffffffff, NULL, NULL, 
							&bottoms[0]);
		// DebugOutput("13 ");
		hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\bottom2.png",
							0,0,1,0,	format, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0xffffffff, NULL, NULL, 
							&bottoms[1]);
		// DebugOutput("14 ");
		hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\bottom4.png",
							0,0,1,0,	format, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0xffffffff, NULL, NULL, 
							&bottoms[2]);
		// DebugOutput("15 ");
		hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\bottom5.png",
							0,0,1,0,	format, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0xffffffff, NULL, NULL, 
							&bottoms[3]);


		//************
		// DebugOutput("16 ");
		hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\avatarHair.png",
							0,0,1,0,	format, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0xffffffff, NULL, NULL, 
							&hair);

		hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\avatarBelt.png",
//		hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\avatarWhtGloves.png",
							0,0,1,0,	format, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0xffffffff, NULL, NULL, 
							&helperMark);

		hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\jeweledArmband.png",
							0,0,1,0,	format, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0xffffffff, NULL, NULL, 
							&dokkMark);

		hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\jeweledBelt.png",
							0,0,1,0,	format, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0xffffffff, NULL, NULL, 
							&anubisMark);

		hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\jewelednecklace.png",
							0,0,1,0,	format, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0xffffffff, NULL, NULL, 
							&overlordMark);

//		sprintf(tempText, "Retval of hair load = %d\n", hr);
//		DebugOutput(tempText);

		//************
		// DebugOutput("17 ");
		hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\AvatarNude.png",
							0,0,1,0,	format, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0xffffffff, NULL, NULL, 
							&skin);
	}
	++bitmapRefCount;

}

//***************************************************************************************
AvatarTexture::~AvatarTexture()
{

		// DebugOutput("18 ");
   SAFE_RELEASE(currentBitmap);

	--bitmapRefCount;
	if (bitmapRefCount < 1)
	{
		// DebugOutput("19 ");
		SAFE_RELEASE(faces[0]);
		SAFE_RELEASE(faces[1]);
		SAFE_RELEASE(faces[2]);
		SAFE_RELEASE(faces[3]);

		SAFE_RELEASE(tops[0]);
		SAFE_RELEASE(tops[1]);
		SAFE_RELEASE(tops[2]);
		SAFE_RELEASE(tops[3]);
		SAFE_RELEASE(tops[4]);

		SAFE_RELEASE(bottoms[0]);
		SAFE_RELEASE(bottoms[1]);
		SAFE_RELEASE(bottoms[2]);
		SAFE_RELEASE(bottoms[3]);

		SAFE_RELEASE(hair);
		SAFE_RELEASE(skin);
		SAFE_RELEASE(helperMark);
		SAFE_RELEASE(dokkMark);
		SAFE_RELEASE(anubisMark);
		SAFE_RELEASE(overlordMark);
		// DebugOutput("20 ");
	}

}

//***************************************************************************************
D3DCOLOR AvatarTexture::GetPixel(_D3DFORMAT fmt, char *line, int x)
{
	D3DCOLOR pixel, *pixelPtr;
	char *p;
	unsigned short *s;
	unsigned char r,g,b;

	switch (fmt)
	{
	case D3DFMT_A8R8G8B8:
		pixelPtr = (D3DCOLOR *)line;
		return pixelPtr[x];
		break;

	case D3DFMT_R8G8B8  :
		p = line + 3 * x;
		pixel = D3DCOLOR_ARGB(255, p[2],p[1],p[0]);
		return pixel;
		break;

   case D3DFMT_X8R8G8B8:
		p = line + 4 * x;
		pixel = D3DCOLOR_ARGB(255, p[2],p[1],p[0]);
		return pixel;
		break;

   case D3DFMT_R5G6B5  :
		p = line + 2 * x;
		s = (unsigned short *) p;
		r = (unsigned char) ((*s)>>11) & 0x001f;
		g = (unsigned char) ((*s)>>5)  & 0x002f;
		b = (unsigned char) ((*s))     & 0x001f;
		pixel = D3DCOLOR_ARGB(255, (int)(r / 32.0f * 255.0f), 
			                   (int)(g / 64.0f * 255.0f), (int)(b / 32.0f * 255.0f));
		return pixel;
		break;

   case D3DFMT_X1R5G5B5:
		p = line + 2 * x;
		s = (unsigned short *) p;
		r = (unsigned char) ((*s)>>10) & 0x001f;
		g = (unsigned char) ((*s)>>5)  & 0x001f;
		b = (unsigned char) ((*s))     & 0x001f;
		pixel = D3DCOLOR_ARGB(255, (int)(r / 32.0f * 255.0f), 
			                   (int)(g / 32.0f * 255.0f), (int)(b / 32.0f * 255.0f));
		return pixel;
		break;

   case D3DFMT_A1R5G5B5:
		p = line + 2 * x;
		s = (unsigned short *) p;
		r = (unsigned char) ((*s)>>10) & 0x001f;
		g = (unsigned char) ((*s)>>5)  & 0x001f;
		b = (unsigned char) ((*s))     & 0x001f;
		pixel = D3DCOLOR_ARGB(255, (int)(r / 32.0f * 255.0f), 
			                   (int)(g / 32.0f * 255.0f), (int)(b / 32.0f * 255.0f));
		return pixel;
		break;

   case D3DFMT_A4R4G4B4:
		p = line + 2 * x;
		s = (unsigned short *) p;
		r = (unsigned char) ((*s)>>8)  & 0x000f;
		g = (unsigned char) ((*s)>>4)  & 0x000f;
		b = (unsigned char) ((*s))     & 0x000f;
		pixel = D3DCOLOR_ARGB(255, (int)(r / 16.0f * 255.0f), 
			                   (int)(g / 16.0f * 255.0f), (int)(b / 16.0f * 255.0f));
		return pixel;
		break;
	}

	return 0;
}

//***************************************************************************************
void AvatarTexture::SetPixel(D3DCOLOR color, _D3DFORMAT fmt, char *line, int x)
{
	D3DCOLOR *pixelPtr;
	unsigned char *p, *p2;
	unsigned char r,g,b,a;
	float rf,gf,bf;
	unsigned short *s;

	switch (fmt)
	{
	case D3DFMT_A8R8G8B8:
		pixelPtr = (D3DCOLOR *)line;
		pixelPtr[x] = color;
		break;

	case D3DFMT_R8G8B8  :
		p = (unsigned char *) line + 3 * x;
		p2 = (unsigned char *) &color;
		p[2] = p2[2];
		p[1] = p2[1];
		p[0] = p2[0];
		break;

   case D3DFMT_X8R8G8B8:
		p = (unsigned char *) line + 4 * x;
		p2 = (unsigned char *) &color;
		p[3] = 255;
		p[2] = p2[2];
		p[1] = p2[1];
		p[0] = p2[0];
		break;

   case D3DFMT_R5G6B5  :
		p = (unsigned char *) line + 2 * x;
		s = (unsigned short *) p;
		p2 = (unsigned char *) &color;
		a = 255;
		rf = (float) p2[2] / 255.0f * 0x1f;
		gf = (float) p2[1] / 255.0f * 0x2f;
		bf = (float) p2[0] / 255.0f * 0x1f;
		r = (unsigned char) rf;
		g = (unsigned char) gf;
		b = (unsigned char) bf;
		*s = (r << 11) + (g << 5) + (b);
		break;

   case D3DFMT_X1R5G5B5:
		p = (unsigned char *) line + 2 * x;
		s = (unsigned short *) p;
		p2 = (unsigned char *) &color;
		a = 255;
		rf = (float) p2[2] / 255.0f * 0x1f;
		gf = (float) p2[1] / 255.0f * 0x1f;
		bf = (float) p2[0] / 255.0f * 0x1f;
		r = (unsigned char) rf;
		g = (unsigned char) gf;
		b = (unsigned char) bf;
		*s = (r << 10) + (g << 5) + (b);
		break;

   case D3DFMT_A1R5G5B5:
		p = (unsigned char *) line + 2 * x;
		s = (unsigned short *) p;
		p2 = (unsigned char *) &color;
		a = 255;
		rf = (float) p2[2] / 255.0f * 0x1f;
		gf = (float) p2[1] / 255.0f * 0x1f;
		bf = (float) p2[0] / 255.0f * 0x1f;
		r = (unsigned char) rf;
		g = (unsigned char) gf;
		b = (unsigned char) bf;
		*s = (r << 10) + (g << 5) + (b);
		break;

   case D3DFMT_A4R4G4B4:
		p = (unsigned char *) line + 2 * x;
		s = (unsigned short *) p;
		p2 = (unsigned char *) &color;
		a = 255;
		rf = (float) p2[2] / 255.0f * 0x0f;
		gf = (float) p2[1] / 255.0f * 0x0f;
		bf = (float) p2[0] / 255.0f * 0x0f;
		r = (unsigned char) rf;
		g = (unsigned char) gf;
		b = (unsigned char) bf;
		*s = (r << 8) + (g << 4) + (b);
		break;
	}

}


//***************************************************************************************
void AvatarTexture::Generate(LPDIRECT3DDEVICE8 pd3dDevice, 
		              int faceIndex, int hairR, int hairG, int hairB,
		              int topIndex, int topR, int topG, int topB,
		              int bottomIndex, int bottomR, int bottomG, int bottomB,
						  unsigned short imageFlags)
{

	HRESULT hr;

   SAFE_RELEASE(currentBitmap);

	if (useSimpleAvatarTextureFlag)
	{
		if (topR > 200 && topG > 200 && topB > 200)
			hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\smlAvatarWhite.png",
							0,0,1,0,	D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0xffffffff, NULL, NULL, 
							&currentBitmap);
		else if (topR < 50 && topG < 50 && topB < 50)
			hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\smlAvatarBlack.png",
							0,0,1,0,	D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0xffffffff, NULL, NULL, 
							&currentBitmap);
		else if (topR > topG && topR > topB)
			hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\smlAvatarRed.png",
							0,0,1,0,	D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0xffffffff, NULL, NULL, 
							&currentBitmap);
		else if (topG > topR && topG > topB)
			hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\smlAvatarGreen.png",
							0,0,1,0,	D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0xffffffff, NULL, NULL, 
							&currentBitmap);
		else
			hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\smlAvatarBlue.png",
							0,0,1,0,	D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0xffffffff, NULL, NULL, 
							&currentBitmap);

		return;
	}

	D3DSURFACE_DESC hairDesc, skinDesc, topDesc, bottomDesc, targetDesc, faceDesc;

   hair->GetLevelDesc(0, &hairDesc);

//	sprintf(tempText, "hair info: %dx%d, fmt %d, type %d, pool %d\n", desc.Width, desc.Height, 
//		     desc.Format, desc.Type, desc.Pool);
//	DebugOutput(tempText);

		// DebugOutput("a3 ");
	hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\avatarHair.png",
							0,0,1,0,	D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0xffffffff, NULL, NULL, 
							&currentBitmap);

//	HRESULT hr = pd3dDevice->CreateTexture(hairDesc.Width, hairDesc.Height, 1,0,	
//		                        D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &currentBitmap);

//	sprintf(tempText, "Retval of bitmap create = %d\n", hr);
//	DebugOutput(tempText);

//	sprintf(tempText, "bitmap pointer = %d\n", (int) currentBitmap);
//	DebugOutput(tempText);

	if (!currentBitmap)
	{
		hr = pd3dDevice->CreateTexture(hairDesc.Width, hairDesc.Height, 1,0,	
			                        D3DFMT_R5G6B5 , D3DPOOL_MANAGED, &currentBitmap);

//		sprintf(tempText, "Retval of bitmap create = %d\n", hr);
//		DebugOutput(tempText);

//		sprintf(tempText, "bitmap pointer = %d\n", (int) currentBitmap);
//		DebugOutput(tempText);
	}
		// DebugOutput("a4 ");
	D3DLOCKED_RECT srcLock, destLock, otherLock;
   currentBitmap->GetLevelDesc(0, &targetDesc);
	currentBitmap->LockRect(0, &destLock, NULL, 0);

	//******* add skin and bottom
   bottoms[bottomIndex]->GetLevelDesc(0, &bottomDesc);
	bottoms[bottomIndex]->LockRect(0, &srcLock, NULL, 0);

   skin->GetLevelDesc(0, &skinDesc);
	skin->LockRect(0, &otherLock, NULL, 0);

	for (int y = 0; y < (int) hairDesc.Height; ++y)
	{
		char *srcPixel   = ((char *)srcLock.pBits  + srcLock.Pitch  * y);
		char *destPixel  = ((char *)destLock.pBits + destLock.Pitch * y);
		char *otherPixel = ((char *)otherLock.pBits + otherLock.Pitch * y);

		for (int x = 0; x < (int) hairDesc.Width; ++x)
		{
			D3DCOLOR srcCol   = GetPixel(bottomDesc.Format,srcPixel,x);
			D3DCOLOR destCol  = GetPixel(targetDesc.Format,destPixel,x);
			D3DCOLOR otherCol = GetPixel(skinDesc.Format,otherPixel,x);

			unsigned char c = (unsigned char)((srcCol) & 0xff);
			SetPixel(otherCol, targetDesc.Format, destPixel, x);
			if (c > 0)
			{
				SetPixel(D3DCOLOR_ARGB(255, bottomR * c / 256, bottomG * c / 256, bottomB * c / 256),
					      targetDesc.Format,destPixel,x);
			}
		}
	}

	skin->UnlockRect(0);
	bottoms[bottomIndex]->UnlockRect(0);

	//******* add top
   tops[topIndex]->GetLevelDesc(0, &topDesc);
	tops[topIndex]->LockRect(0, &srcLock, NULL, 0);

	for (int y = 0; y < (int) hairDesc.Height; ++y)
	{
		char *srcPixel   = ((char *)srcLock.pBits  + srcLock.Pitch  * y);
		char *destPixel  = ((char *)destLock.pBits + destLock.Pitch * y);

		for (int x = 0; x < (int) hairDesc.Width; ++x)
		{
			D3DCOLOR srcCol   = GetPixel(topDesc.Format,srcPixel,x);
			D3DCOLOR destCol  = GetPixel(targetDesc.Format,destPixel,x);

			unsigned char c = (unsigned char)((srcCol) & 0xff);
			if (c > 0)
			{
				SetPixel(D3DCOLOR_ARGB(255, topR * c / 256, topG * c / 256, topB * c / 256),
					      targetDesc.Format,destPixel,x);
			}
		}
	}

	tops[topIndex]->UnlockRect(0);

	//******* add face
   faces[faceIndex]->GetLevelDesc(0, &faceDesc);
	faces[faceIndex]->LockRect(0, &srcLock, NULL, 0);

	for (int y = 0; y < (int) hairDesc.Height; ++y)
	{
		char *srcPixel   = ((char *)srcLock.pBits  + srcLock.Pitch  * y);
		char *destPixel  = ((char *)destLock.pBits + destLock.Pitch * y);

		for (int x = 0; x < (int) hairDesc.Width; ++x)
		{
			D3DCOLOR srcCol   = GetPixel(faceDesc.Format,srcPixel,x);
			D3DCOLOR destCol  = GetPixel(targetDesc.Format,destPixel,x);

			unsigned char c = (unsigned char)((srcCol) & 0xff);
			if (c > 0)
			{
				SetPixel(srcCol,targetDesc.Format,destPixel,x);
			}
		}
	}

	faces[faceIndex]->UnlockRect(0);

	//******* add hair
   hair->GetLevelDesc(0, &hairDesc);
	hair->LockRect(0, &srcLock, NULL, 0);

	for (int y = 0; y < (int) hairDesc.Height; ++y)
	{
		char *srcPixel   = ((char *)srcLock.pBits  + srcLock.Pitch  * y);
		char *destPixel  = ((char *)destLock.pBits + destLock.Pitch * y);

		for (int x = 0; x < (int) hairDesc.Width; ++x)
		{
			D3DCOLOR srcCol   = GetPixel(hairDesc.Format,srcPixel,x);
			D3DCOLOR destCol  = GetPixel(targetDesc.Format,destPixel,x);

			unsigned char c = (unsigned char)((srcCol) & 0xff);
			if (c > 0)
			{
				SetPixel(D3DCOLOR_ARGB(255, hairR * c / 256, hairG * c / 256, hairB * c / 256),
					      targetDesc.Format,destPixel,x);
			}
		}
	}

	hair->UnlockRect(0);

	//******* add marks
	if (imageFlags & SPECIAL_LOOK_DOKK_KILLER)
	{
		dokkMark->GetLevelDesc(0, &faceDesc);
		dokkMark->LockRect(0, &srcLock, NULL, 0);

		for (int y = 0; y < (int) hairDesc.Height; ++y)
		{
			char *srcPixel   = ((char *)srcLock.pBits  + srcLock.Pitch  * y);
			char *destPixel  = ((char *)destLock.pBits + destLock.Pitch * y);

			for (int x = 0; x < (int) hairDesc.Width; ++x)
			{
				D3DCOLOR srcCol   = GetPixel(faceDesc.Format,srcPixel,x);
				D3DCOLOR destCol  = GetPixel(targetDesc.Format,destPixel,x);

				unsigned char c = (unsigned char)((srcCol) & 0xff);
				if (c > 0)
				{
					SetPixel(srcCol,targetDesc.Format,destPixel,x);
				}
			}
		}
		dokkMark->UnlockRect(0);
	}
	if (imageFlags & SPECIAL_LOOK_ANUBIS_KILLER)
	{
		anubisMark->GetLevelDesc(0, &faceDesc);
		anubisMark->LockRect(0, &srcLock, NULL, 0);

		for (int y = 0; y < (int) hairDesc.Height; ++y)
		{
			char *srcPixel   = ((char *)srcLock.pBits  + srcLock.Pitch  * y);
			char *destPixel  = ((char *)destLock.pBits + destLock.Pitch * y);

			for (int x = 0; x < (int) hairDesc.Width; ++x)
			{
				D3DCOLOR srcCol   = GetPixel(faceDesc.Format,srcPixel,x);
				D3DCOLOR destCol  = GetPixel(targetDesc.Format,destPixel,x);

				unsigned char c = (unsigned char)((srcCol) & 0xff);
				if (c > 0)
				{
					SetPixel(srcCol,targetDesc.Format,destPixel,x);
				}
			}
		}
		anubisMark->UnlockRect(0);
	}
	if (imageFlags & SPECIAL_LOOK_OVERLORD_KILLER)
	{
		overlordMark->GetLevelDesc(0, &faceDesc);
		overlordMark->LockRect(0, &srcLock, NULL, 0);

		for (int y = 0; y < (int) hairDesc.Height; ++y)
		{
			char *srcPixel   = ((char *)srcLock.pBits  + srcLock.Pitch  * y);
			char *destPixel  = ((char *)destLock.pBits + destLock.Pitch * y);

			for (int x = 0; x < (int) hairDesc.Width; ++x)
			{
				D3DCOLOR srcCol   = GetPixel(faceDesc.Format,srcPixel,x);
				D3DCOLOR destCol  = GetPixel(targetDesc.Format,destPixel,x);

				unsigned char c = (unsigned char)((srcCol) & 0xff);
				if (c > 0)
				{
					SetPixel(srcCol,targetDesc.Format,destPixel,x);
				}
			}
		}
		overlordMark->UnlockRect(0);
	}

	//******* add marks
	if (imageFlags & SPECIAL_LOOK_HELPER)
	{
		helperMark->GetLevelDesc(0, &faceDesc);
		helperMark->LockRect(0, &srcLock, NULL, 0);

		for (int y = 0; y < (int) hairDesc.Height; ++y)
		{
			char *srcPixel   = ((char *)srcLock.pBits  + srcLock.Pitch  * y);
			char *destPixel  = ((char *)destLock.pBits + destLock.Pitch * y);

			for (int x = 0; x < (int) hairDesc.Width; ++x)
			{
				D3DCOLOR srcCol   = GetPixel(faceDesc.Format,srcPixel,x);
				D3DCOLOR destCol  = GetPixel(targetDesc.Format,destPixel,x);

				unsigned char c = (unsigned char)((srcCol) & 0xff);
				if (c > 0)
				{
					SetPixel(srcCol,targetDesc.Format,destPixel,x);
				}
			}
		}
		helperMark->UnlockRect(0);
	}

	currentBitmap->UnlockRect(0);

}




/* end of file */
