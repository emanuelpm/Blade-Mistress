//***************************************************************************************
// Mesh object designed for PUMA and spaceship parts.
//***************************************************************************************

#include "puma.h"
#include "PlasmaTexture.h"
#include "../helper/GeneralUtils.h"
#include "noise.h"

//***************************************************************************************
PlasmaTexture::PlasmaTexture(int doid, char *doname)	 : DataObject(doid,doname)
{
	pTexture = NULL;
}

//***************************************************************************************
PlasmaTexture::PlasmaTexture(void) : DataObject(0,"PLASMA_TEXTURE")
{
	pTexture = NULL;
}

//***************************************************************************************
PlasmaTexture::~PlasmaTexture()
{

   SAFE_RELEASE(pTexture);

}

//***************************************************************************************
void PlasmaTexture::Generate(LPDIRECT3DDEVICE8 pd3dDevice, int w, int h, unsigned long randSeed)
{

	if (0 == randSeed)
		randSeed = rand();

//	CFractal(int nDimensions, unsigned int nSeed, float fH, float fLacunarity)
	int dimensions = gTweakSystem.GetTweak("FRACTALDIMENSIONS", 2, 
				                            1, 4,
													 "number of dimensions in fractal seed table");
	float fH = gTweakSystem.GetTweak("FRACTALROUGHNESS", 0.5f, 
				                            0.01f, 0.99f,
													 "'roughness' of fractal");
	float lacunarity = gTweakSystem.GetTweak("FRACTALLAC", 4.0f, 
				                            1.0f, 4.0f,
													 "fractal 'lacunarity'; magic number");


	CFractal fractal(dimensions, randSeed, fH, lacunarity);

	float f[2];
	int nDest = 0;

   SAFE_RELEASE(pTexture);

	unsigned char *hBuffer = (unsigned char *) new char[(w+20)*h];

	for (int y = 0; y < h; ++y)
	{
		for (int x = 0; x < w+20; ++x)
		{
			f[0] = 2.0f * (float)x / (float)(w+20);
			f[1] = 2.0f * (float)y / (float)h;
			float fValue = fractal.fBm(f, 10);
			hBuffer[y*(w+20)+x] = (unsigned char)Max(0.0f, Min(255.0f, ((fValue+1.0f) * 128.0f)));
		}
	}

	for (int y = 0; y < h; ++y)
	{
		for (int x = 0; x < 20; ++x)
		{
			float percent = x / 20.0f;
			float value = hBuffer[y*(w+20)+x] * percent + hBuffer[y*(w+20)+w + x] * (1.0f - percent);
			hBuffer[y*(w+20)+x] = (unsigned char)value;
		}
	}


	pd3dDevice->CreateTexture(w, h, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pTexture);

	D3DLOCKED_RECT lockInfo;

	pTexture->LockRect(0, &lockInfo, NULL, 0);

	for (int y = 0; y < h; ++y)
	{
		char *charPtr = (char *)lockInfo.pBits + lockInfo.Pitch * y;
		D3DCOLOR *pixelPtr = (D3DCOLOR *) charPtr;
		for (int x = 0; x < w; ++x)
		{
			unsigned char c = hBuffer[y*(w+20)+x];
			pixelPtr[x] = D3DCOLOR_RGBA(c, c, c, 255);
		}
	}

	pTexture->UnlockRect(0);
	delete[] hBuffer;

	sizeW = w;
	sizeH = h;


}

//***************************************************************************************
void PlasmaTexture::Colorize(void)
{
	struct CColor
	{
		float r, g, b, a;
	};
	CColor fOcean[7] =
	{
		{0.0f, 0.0f, 0.29f},
		{0.0f, 0.02f, 0.35f},
		{0.0f, 0.05f, 0.39f},
		{0.0f, 0.07f, 0.44f},
		{0.01f, 0.09f, 0.49f},
		{0.01f, 0.14f, 0.55f},
		{0.02f, 0.25f, 0.65f}
	};
	CColor fLand[10] =
	{
		{0.5f, 0.39f, 0.2f},
		{0.2f, 0.3f, 0.0f},
		{0.085f, 0.2f, 0.04f},
		{0.065f, 0.22f, 0.04f},
		{0.5f, 0.42f, 0.28f},
		{0.6f, 0.5f, 0.23f},
		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f}
	};

	float tempF = gTweakSystem.GetTweak("LANDCOLORVARIANCE", 0.7f, 
				                            0.0f, 1.0f,
													 "variation (up and down) of the R,G, and B values of planet surface colors");

	float offsetR = rnd(-tempF,tempF);
	float offsetG = rnd(-tempF,tempF);
	float offsetB = rnd(-tempF,tempF);

	D3DLOCKED_RECT lockInfo;

	pTexture->LockRect(0, &lockInfo, NULL, 0);

	for (int y = 0; y < sizeH; ++y)
	{
		char *charPtr = (char *)lockInfo.pBits + lockInfo.Pitch * y;
		D3DCOLOR *pixelPtr = (D3DCOLOR *) charPtr;

		for (int x = 0; x < sizeW; ++x)
		{
			int colorVal = (int)((pixelPtr[x] & 0xff) * 17.0f / 256.0f);

			if (colorVal > 6)  // if land
			{
//				float tempVal = Clamp(0, 1, 2);

				pixelPtr[x] = D3DCOLOR_RGBA( (unsigned char)(255 * Clamp(0,1,fLand[colorVal-7].r + offsetR)), 
					                          (unsigned char)(255 * Clamp(0,1,fLand[colorVal-7].g + offsetG)), 
													  (unsigned char)(255 * Clamp(0,1,fLand[colorVal-7].b + offsetB)), 
													  255);
			}
			else
			{
				pixelPtr[x] = D3DCOLOR_RGBA( (unsigned char)(255 * Clamp(0,1,fOcean[colorVal].r + offsetR)), 
					                          (unsigned char)(255 * Clamp(0,1,fOcean[colorVal].g + offsetG)), 
													  (unsigned char)(255 * Clamp(0,1,fOcean[colorVal].b + offsetB)), 
													  255);
			}

		}
	}
	pTexture->UnlockRect(0);
}


//***************************************************************************************
void PlasmaTexture::MakeTranslucent(void)
{

	D3DLOCKED_RECT lockInfo;

	float tempF = gTweakSystem.GetTweak("CLOUDCOVER", 1.0f, 
				                            0.0f, 1.0f,
													 "multiplier of sky intensity and alpha");

	pTexture->LockRect(0, &lockInfo, NULL, 0);

	for (int y = 0; y < sizeH; ++y)
	{
		char *charPtr = (char *)lockInfo.pBits + lockInfo.Pitch * y;
		D3DCOLOR *pixelPtr = (D3DCOLOR *) charPtr;

		for (int x = 0; x < sizeW; ++x)
		{
			int colorVal = pixelPtr[x] & 0xff;
			float colorF = (float) colorVal * tempF;
			colorVal = (int) colorF;

			pixelPtr[x] = D3DCOLOR_RGBA(colorVal, colorVal, colorVal, colorVal);

		}
	}
	pTexture->UnlockRect(0);
}




/* end of file */
