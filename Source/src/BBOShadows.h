
#ifndef BBOSHADOWS_H
#define BBOSHADOWS_H

#include ".\puma\puma.h"

struct SHADOWVERTEX { D3DXVECTOR3 p;   D3DXVECTOR3 n;   DWORD color;    FLOAT tu, tv; };

#define D3DFVF_SHADOWVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1)

inline SHADOWVERTEX InitShadowVertex( const D3DXVECTOR3& p, const D3DXVECTOR3& n,
                             D3DCOLOR color, FLOAT tu, FLOAT tv )
{
   SHADOWVERTEX v;   v.p = p;   v.n = n;   v.tu = tu;   v.tv = tv; v.color = color;
   return v;
}

const int MAX_NUM_LAND_SHADOWS = 100;
const int MAX_LAND_SHADOW_VERTS = MAX_NUM_LAND_SHADOWS * 3 * 2;


#endif
