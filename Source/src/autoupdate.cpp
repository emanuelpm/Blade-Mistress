//***************************************************************************************
// Mesh object designed for PUMA and spaceship parts.
//***************************************************************************************

#include "puma.h"
#include "AvatarTexture.h"
#include "noise.h"

LPDIRECT3DTEXTURE8 AvatarTexture::skins[NUM_OF_SKINS];
LPDIRECT3DTEXTURE8 AvatarTexture::hair;
int AvatarTexture::bitmapRefCount = 0;

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

	if (bitmapRefCount < 1)
	{
		HRESULT hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\avatar1.png",
							0,0,1,0,	D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0xffffffff, NULL, NULL, 
							&skins[0]);
		hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\avatar2.png",
							0,0,1,0,	D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0xffffffff, NULL, NULL, 
							&skins[1]);
		hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\avatar3.png",
							0,0,1,0,	D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0xffffffff, NULL, NULL, 
							&skins[2]);
		hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\avatar4.png",
							0,0,1,0,	D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0xffffffff, NULL, NULL, 
							&skins[3]);
		hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\avatar5.png",
							0,0,1,0,	D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0xffffffff, NULL, NULL, 
							&skins[4]);
		hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\avatar6.png",
							0,0,1,0,	D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0xffffffff, NULL, NULL, 
							&skins[5]);
		hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\avatar7.png",
							0,0,1,0,	D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0xffffffff, NULL, NULL, 
							&skins[6]);
		hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\avatar8.png",
							0,0,1,0,	D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0xffffffff, NULL, NULL, 
							&skins[7]);
		hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\avatarHair.png",
							0,0,1,0,	D3DFMT_A8, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0xffffffff, NULL, NULL, 
							&hair);
	}
	++bitmapRefCount;

}

//***************************************************************************************
AvatarTexture::~AvatarTexture()
{

   SAFE_RELEASE(currentBitmap);

	--bitmapRefCount;
	if (bitmapRefCount < 1)
	{
		SAFE_RELEASE(skins[0]);
		SAFE_RELEASE(skins[1]);
		SAFE_RELEASE(skins[2]);
		SAFE_RELEASE(skins[3]);
		SAFE_RELEASE(skins[4]);
		SAFE_RELEASE(hair);
	}

}

//***************************************************************************************
void AvatarTexture::Generate(LPDIRECT3DDEVICE8 pd3dDevice, 
									  int skinIndex, int hairR, int hairG, int hairB)
{

   SAFE_RELEASE(currentBitmap);

	D3DSURFACE_DESC desc;
	skins[0]->GetLevelDesc(0, &desc);

   hair->GetLevelDesc(0, &desc);

	HRESULT hr = pd3dDevice->CreateTexture(desc.Width, desc.Height, 1,0,	
		                        D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &currentBitmap);

	D3DLOCKED_RECT srcLock, destLock, hairLock;
	currentBitmap->LockRect(0, &destLock, NULL, 0);
	skins[skinIndex]->LockRect(0, &srcLock, NULL, 0);
	hair->LockRect(0, &hairLock, NULL, 0);

	for (int y = 0; y < desc.Height; ++y)
	{
		D3DCOLOR *srcPixel  = (D3DCOLOR *)((char *)srcLock.pBits  + srcLock.Pitch  * y);
		D3DCOLOR *destPixel = (D3DCOLOR *)((char *)destLock.pBits + destLock.Pitch * y);
		D3DCOLOR *hairPixel = (D3DCOLOR *)((char *)hairLock.pBits + hairLock.Pitch * y);

		for (int x = 0; x < desc.Width; ++x)
		{
			unsigned char c = (hairPixel[x]) & 0xff;
			if (c > 0)
			{
				destPixel[x] = D3DCOLOR_RGBA(hairR * c / 256, hairG * c / 256, hairB * c / 256, 255);
			}
			else
			{
				destPixel[x] = srcPixel[x];
			}
		}
	}

	hair->UnlockRect(0);
	skins[skinIndex]->UnlockRect(0);
	currentBitmap->UnlockRect(0);

}




/* end of file */
