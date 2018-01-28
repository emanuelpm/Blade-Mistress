/*
	The Universe Development Kit
	Copyright (C) 2000  Sean O'Neil
	soneil@home.com

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "puma.h"
#include "Noise.h"

void CNoise::Init(int nDimensions, unsigned int nSeed)
{
	m_nDimensions = MIN(nDimensions, MAX_DIMENSIONS);
	CRandom r(nSeed);

	int i, j, k;
	for(i=0; i<256; i++)
	{
		m_nMap[i] = i;
		for(j=0; j<m_nDimensions; j++)
			m_nBuffer[i][j] = (float)r.RandomD(-0.5, 0.5);
		Normalize(m_nBuffer[i], m_nDimensions);
	}

	while(--i)
	{
		j = r.RandomI(0, 255);
		SWAP(m_nMap[i], m_nMap[j], k);
	}
	_fpreset();	// Bug in CRandom! Causes messed up floating point operations!
}

float CNoise::Noise(float *f)
{
	int n[MAX_DIMENSIONS];			// Indexes to pass to lattice function
	float r[MAX_DIMENSIONS];		// Remainders to pass to lattice function
	float w[MAX_DIMENSIONS];		// Cubic values to pass to interpolation function

	for(int i=0; i<m_nDimensions; i++)
	{
		n[i] = Floor(f[i]);
		r[i] = f[i] - n[i];
		w[i] = Cubic(r[i]);
	}

	float fValue;
	switch(m_nDimensions)
	{
		case 1:
			fValue = Lerp(Lattice(n[0], r[0]),
						  Lattice(n[0]+1, r[0]-1),
						  w[0]);
			break;
		case 2:
			fValue = Lerp(Lerp(Lattice(n[0], r[0], n[1], r[1]),
							   Lattice(n[0]+1, r[0]-1, n[1], r[1]),
							   w[0]),
						  Lerp(Lattice(n[0], r[0], n[1]+1, r[1]-1),
							   Lattice(n[0]+1, r[0]-1, n[1]+1, r[1]-1),
							   w[0]),
						  w[1]);
			break;
		case 3:
			fValue = Lerp(Lerp(Lerp(Lattice(n[0], r[0], n[1], r[1], n[2], r[2]),
									Lattice(n[0]+1, r[0]-1, n[1], r[1], n[2], r[2]),
									w[0]),
							   Lerp(Lattice(n[0], r[0], n[1]+1, r[1]-1, n[2], r[2]),
									Lattice(n[0]+1, r[0]-1, n[1]+1, r[1]-1, n[2], r[2]),
									w[0]),
							   w[1]),
						  Lerp(Lerp(Lattice(n[0], r[0], n[1], r[1], n[2]+1, r[2]-1),
									Lattice(n[0]+1, r[0]-1, n[1], r[1], n[2]+1, r[2]-1),
									w[0]),
							   Lerp(Lattice(n[0], r[0], n[1]+1, r[1]-1, n[2]+1, r[2]-1),
									Lattice(n[0]+1, r[0]-1, n[1]+1, r[1]-1, n[2]+1, r[2]-1),
									w[0]),
							   w[1]),
						  w[2]);
			break;
		case 4:
			fValue = Lerp(Lerp(Lerp(Lerp(Lattice(n[0], r[0], n[1], r[1], n[2], r[2], n[3], r[3]),
										 Lattice(n[0]+1, r[0]-1, n[1], r[1], n[2], r[2], n[3], r[3]),
										 w[0]),
									Lerp(Lattice(n[0], r[0], n[1]+1, r[1]-1, n[2], r[2], n[3], r[3]),
										 Lattice(n[0]+1, r[0]-1, n[1]+1, r[1]-1, n[2], r[2], n[3], r[3]),
										 w[0]),
									w[1]),
									Lerp(Lerp(Lattice(n[0], r[0], n[1], r[1], n[2]+1, r[2]-1, n[3], r[3]),
										 Lattice(n[0]+1, r[0]-1, n[1], r[1], n[2]+1, r[2]-1, n[3], r[3]),
										 w[0]),
									Lerp(Lattice(n[0], r[0], n[1]+1, r[1]-1, n[2]+1, r[2]-1),
										 Lattice(n[0]+1, r[0]-1, n[1]+1, r[1]-1, n[2]+1, r[2]-1, n[3], r[3]),
										 w[0]),
									w[1]),
							   w[2]),
						  Lerp(Lerp(Lerp(Lattice(n[0], r[0], n[1], r[1], n[2], r[2], n[3]+1, r[3]-1),
										 Lattice(n[0]+1, r[0]-1, n[1], r[1], n[2], r[2], n[3]+1, r[3]-1),
										 w[0]),
									Lerp(Lattice(n[0], r[0], n[1]+1, r[1]-1, n[2], r[2], n[3]+1, r[3]-1),
										 Lattice(n[0]+1, r[0]-1, n[1]+1, r[1]-1, n[2], r[2], n[3]+1, r[3]-1),
										 w[0]),
									w[1]),
									Lerp(Lerp(Lattice(n[0], r[0], n[1], r[1], n[2]+1, r[2]-1, n[3]+1, r[3]-1),
										 Lattice(n[0]+1, r[0]-1, n[1], r[1], n[2]+1, r[2]-1, n[3]+1, r[3]-1),
										 w[0]),
									Lerp(Lattice(n[0], r[0], n[1]+1, r[1]-1, n[2]+1, r[2]-1),
										 Lattice(n[0]+1, r[0]-1, n[1]+1, r[1]-1, n[2]+1, r[2]-1, n[3]+1, r[3]-1),
										 w[0]),
									w[1]),
							   w[2]),
						  w[3]);
			break;
	}
	return CLAMP(-0.99999f, 0.99999f, fValue);
}

float CFractal::fBm(float *f, float fOctaves)
{
	// Initialize locals
	float fValue = 0;
	float fTemp[MAX_DIMENSIONS];
	int i = 0;
	for(i=0; i<m_nDimensions; i++)
		fTemp[i] = f[i];

	// Inner loop of spectral construction, where the fractal is built
	for(i=0; i<fOctaves; i++)
	{
		fValue += Noise(fTemp) * m_fExponent[i];
		for(int j=0; j<m_nDimensions; j++)
			fTemp[j] *= m_fLacunarity;
	}

	// Take care of remainder in fOctaves
	fOctaves -= (int)fOctaves;
	if(fOctaves > DELTA)
		fValue += fOctaves * Noise(fTemp) * m_fExponent[i];
	return CLAMP(-0.99999f, 0.99999f, fValue);
}

float CFractal::fBmTest(float *f, float fOctaves)
{
	// Initialize locals
	float fValue = 0;
	float fTemp[MAX_DIMENSIONS];
	for(int i=0; i<m_nDimensions; i++)
		fTemp[i] = f[i] * 2;

	//fOctaves *= Abs(Noise(fTemp)) + 1.0f;
	//fOctaves = Clamp(2, 16, fOctaves);

	// Inner loop of spectral construction, where the fractal is built
	int i = 0;
	for(i=0; i<fOctaves; i++)
	{
		fValue += Noise(fTemp) * m_fExponent[i];
		for(int j=0; j<m_nDimensions; j++)
			fTemp[j] *= m_fLacunarity;
	}

	// Take care of remainder in fOctaves
	fOctaves -= (int)fOctaves;
	if(fOctaves > DELTA)
		fValue += fOctaves * Noise(fTemp) * m_fExponent[i];

	if(fValue <= 0.0f)
		return (float)-pow(-fValue, 0.7f);
	return (float)pow(fValue, 1 + Noise(fTemp) * fValue);
}

float CFractal::Turbulence(float *f, float fOctaves)
{
	// Initialize locals
	float fValue = 0;
	float fTemp[MAX_DIMENSIONS];
	for(int i=0; i<m_nDimensions; i++)
		fTemp[i] = f[i];

	// Inner loop of spectral construction, where the fractal is built
	int i = 0;
	for(i=0; i<fOctaves; i++)
	{
		fValue += Abs(Noise(fTemp)) * m_fExponent[i];
		for(int j=0; j<m_nDimensions; j++)
			fTemp[j] *= m_fLacunarity;
	}

	// Take care of remainder in fOctaves
	fOctaves -= (int)fOctaves;
	if(fOctaves > DELTA)
		fValue += fOctaves * Abs(Noise(fTemp) * m_fExponent[i]);
	return CLAMP(-0.99999f, 0.99999f, fValue);
}

float CFractal::Multifractal(float *f, float fOctaves, float fOffset)
{
	// Initialize locals
	float fValue = 1;
	float fTemp[MAX_DIMENSIONS];
	for(int i=0; i<m_nDimensions; i++)
		fTemp[i] = f[i];

	// Inner loop of spectral construction, where the fractal is built
	int i = 0;
	for(i=0; i<fOctaves; i++)
	{
		fValue *= Noise(fTemp) * m_fExponent[i] + fOffset;
		for(int j=0; j<m_nDimensions; j++)
			fTemp[j] *= m_fLacunarity;
	}

	// Take care of remainder in fOctaves (shouldn't that be a multiply?)
	fOctaves -= (int)fOctaves;
	if(fOctaves > DELTA)
		fValue *= fOctaves * (Noise(fTemp) * m_fExponent[i] + fOffset);
	return CLAMP(-0.99999f, 0.99999f, fValue);
}

float CFractal::Heterofractal(float *f, float fOctaves, float fOffset)
{
	// Initialize locals
	float fValue = Noise(f) + fOffset;
	float fTemp[MAX_DIMENSIONS];
	for(int i=0; i<m_nDimensions; i++)
		fTemp[i] = f[i] * m_fLacunarity;

	// Inner loop of spectral construction, where the fractal is built
	int i = 0;
	for(i=1; i<fOctaves; i++)
	{
		fValue += (Noise(fTemp) + fOffset) * m_fExponent[i] * fValue;
		for(int j=0; j<m_nDimensions; j++)
			fTemp[j] *= m_fLacunarity;
	}

	// Take care of remainder in fOctaves
	fOctaves -= (int)fOctaves;
	if(fOctaves > DELTA)
		fValue += fOctaves * (Noise(fTemp) + fOffset) * m_fExponent[i] * fValue;
	return CLAMP(-0.99999f, 0.99999f, fValue);
}

float CFractal::HybridMultifractal(float *f, float fOctaves, float fOffset, float fGain)
{
	// Initialize locals
	float fValue = (Noise(f) + fOffset) * m_fExponent[0];
	float fWeight = fValue;
	float fTemp[MAX_DIMENSIONS];
	for(int i=0; i<m_nDimensions; i++)
		fTemp[i] = f[i] * m_fLacunarity;

	// Inner loop of spectral construction, where the fractal is built
	int i = 0;
	for(i=1; i<fOctaves; i++)
	{
		if(fWeight > 1)
			fWeight = 1;
		float fSignal = (Noise(fTemp) + fOffset) * m_fExponent[i];
		fValue += fWeight * fSignal;
		fWeight *= fGain * fSignal;
		for(int j=0; j<m_nDimensions; j++)
			fTemp[j] *= m_fLacunarity;
	}

	// Take care of remainder in fOctaves
	fOctaves -= (int)fOctaves;
	if(fOctaves > DELTA)
	{
		if(fWeight > 1)
			fWeight = 1;
		float fSignal = (Noise(fTemp) + fOffset) * m_fExponent[i];
		fValue += fOctaves * fWeight * fSignal;
	}
	return CLAMP(-0.99999f, 0.99999f, fValue);
}

float CFractal::RidgedMultifractal(float *f, float fOctaves, float fOffset, float fGain)
{
	// Initialize locals
	float fSignal = fOffset - Abs(Noise(f));
	fSignal *= fSignal;
	float fValue = fSignal;
	float fTemp[MAX_DIMENSIONS];
	for(int i=0; i<m_nDimensions; i++)
		fTemp[i] = f[i];

	// Inner loop of spectral construction, where the fractal is built
	int i = 0;
	for(i=1; i<fOctaves; i++)
	{
		for(int j=0; j<m_nDimensions; j++)
			fTemp[j] *= m_fLacunarity;
		float fWeight = Clamp(0, 1, fSignal * fGain);
		fSignal = fOffset - Abs(Noise(fTemp));
		fSignal *= fSignal;
		fSignal *= fWeight;
		fValue += fSignal * m_fExponent[i];
	}
	return CLAMP(-0.99999f, 0.99999f, fValue);
}

