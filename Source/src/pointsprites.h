#ifndef POINTSPRITES_H
#define POINTSPRITES_H


//-----------------------------------------------------------------------------
// Custom vertex types
//-----------------------------------------------------------------------------
struct PARTICLEVERTEX
{
    D3DXVECTOR3 v;
    D3DCOLOR    color;
    FLOAT       tu;
    FLOAT       tv;
};

#define D3DFVF_PARTICLEVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)


struct PARTICLEPOINTVERTEX
{
    D3DXVECTOR3 v;
    D3DCOLOR    color;
};

#define D3DFVF_PARTICLEPOINTVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)


//-----------------------------------------------------------------------------
// Global data for the particles
//-----------------------------------------------------------------------------
struct PARTICLE
{
    BOOL        m_bSpark;     // Sparks are less energetic particles that
                              // are generated where/when the main particles
                              // hit the ground

    D3DXVECTOR3 m_vPos;       // Current position
    D3DXVECTOR3 m_vVel;       // Current velocity

    D3DXVECTOR3 m_vPos0;      // Initial position
    D3DXVECTOR3 m_vVel0;      // Initial velocity
    FLOAT       m_fTime0;     // Time of creation

    D3DXCOLOR   m_clrDiffuse; // Initial diffuse color
    D3DXCOLOR   m_clrFade;    // Faded diffuse color
    FLOAT       m_fFade;      // Fade progression

    PARTICLE*   m_pNext;      // Next particle in list
};


enum PARTICLE_COLORS { COLOR_WHITE, COLOR_RED, COLOR_GREEN, COLOR_BLUE, NUM_COLORS };



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CParticleSystem
{
protected:
    FLOAT     m_fRadius;

    DWORD     m_dwBase;
	DWORD     m_dwFlush;
    DWORD     m_dwDiscard;

    DWORD     m_dwParticles;
    DWORD     m_dwParticlesLim;
    PARTICLE* m_pParticles;
    PARTICLE* m_pParticlesFree;

    // Geometry
    LPDIRECT3DVERTEXBUFFER8 m_pVB;

public:
    CParticleSystem( DWORD dwFlush, DWORD dwDiscard, FLOAT fRadius );
   ~CParticleSystem();

    HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICE8 pd3dDevice );
    HRESULT InvalidateDeviceObjects();

    HRESULT Update( FLOAT fSecsPerFrame, DWORD dwNumParticlesToEmit,
                    const D3DXCOLOR &dwEmitColor, const D3DXCOLOR &dwFadeColor,
                    FLOAT fEmitVel, D3DXVECTOR3 vPosition );

    HRESULT Render( LPDIRECT3DDEVICE8 pd3dDevice );
};

extern D3DXCOLOR g_clrColor[NUM_COLORS];
extern DWORD g_clrColorFade[NUM_COLORS];


#endif
