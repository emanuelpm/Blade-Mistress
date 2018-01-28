
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include "BBOClient.h"
#include "BBOPet.h"
#include "monsterData.h"
#include ".\network\NetWorldMessages.h"
#include "particle2.h"

extern BBOClient *bboClient;

char dragonArtFileName[DRAGON_QUALITY_NUM][DRAGON_TYPE_NUM][32] =
{
	{
		{"dat\\petPlainRed.png"},
		{"dat\\petPlainGreen.png"},
		{"dat\\petPlainBlue.png"},
		{"dat\\petPlainWhite.png"},
		{"dat\\petPlainBlack.png"},
		{"dat\\petPlainGold.png"},
		{"dat\\petPlainWonder.png"}
	},
	{
		{"dat\\petStripedRed.png"},
		{"dat\\petStripedGreen.png"},
		{"dat\\petStripedBlue.png"},
		{"dat\\petStripedWhite.png"},
		{"dat\\petStripedBlack.png"},
		{"dat\\petStripedGold.png"},
		{"dat\\petStripedWonder.png"}
	},
	{
		{"dat\\petSpotsRed.png"},
		{"dat\\petSpotsGreen.png"},
		{"dat\\petSpotsBlue.png"},
		{"dat\\petSpotsWhite.png"},
		{"dat\\petSpotsBlack.png"},
		{"dat\\petSpotsGold.png"},
		{"dat\\petSpotsWonder.png"}
	},
	{
		{"dat\\petHoodRed.png"},
		{"dat\\petHoodGreen.png"},
		{"dat\\petHoodBlue.png"},
		{"dat\\petHoodWhite.png"},
		{"dat\\petHoodBlack.png"},
		{"dat\\petHoodGold.png"},
		{"dat\\petHoodWonder.png"}
	}
};

//***************************************************************
void BBOPet::Init(MessPet *messPet)
{
	char tempText[1024];

	if (messPet)
	{
		petScaleVal = 0.0020f * (messPet->size+1);

		sprintf(petName,messPet->name);
		if (petDragonAnims[0])
		{
			delete petDragonAnims[0];
			petDragonAnims[0] = NULL;
		}
		if (petDragonAnims[1])
		{
			delete petDragonAnims[1];
			petDragonAnims[1] = NULL;
		}

		if (messPet->type != 255)
		{
			sprintf(tempText,dragonArtFileName[messPet->quality][messPet->type]);
			petAttackType = messPet->type;
		
			petDragonAnims[0] = new PumaAnim(1,"MONSTER_ANIM");
//			petDragonAnims[0]->LoadFromASC(puma->m_pd3dDevice, "dat\\dragon-hover.ase");
//			petDragonAnims[0]->SaveCompressed(puma->m_pd3dDevice, "dat\\dragon-hover.anc");
			petDragonAnims[0]->LoadCompressed(puma->m_pd3dDevice, "dat\\dragon-hover.anc");
			petDragonAnims[0]->LoadTexture(puma->m_pd3dDevice, tempText);
			petDragonAnims[0]->Scale(puma->m_pd3dDevice, 
		                               petScaleVal, petScaleVal, petScaleVal);

			petDragonAnims[1] = new PumaAnim(1,"MONSTER_ANIM");
//			petDragonAnims[1]->LoadFromASC(puma->m_pd3dDevice, "dat\\dragon-hover-attack.ase");
//			petDragonAnims[1]->SaveCompressed(puma->m_pd3dDevice, "dat\\dragon-hover-attack.anc");
			petDragonAnims[1]->LoadCompressed(puma->m_pd3dDevice, "dat\\dragon-hover-attack.anc");
			petDragonAnims[1]->LoadTexture(puma->m_pd3dDevice, tempText);
			petDragonAnims[1]->Scale(puma->m_pd3dDevice, 
		                               petScaleVal, petScaleVal, petScaleVal);

			petEmotion = messPet->state;
		}
	}
	else
	{
		petDragonAnims[0] = NULL;
		petDragonAnims[1] = NULL;
		petName[0] = 0;
		petAttackType = 0;

		curState = nextState = 0; // flapping in place
		stateCounter = rand() % 24;

		pos[0].x = pos[0].y = pos[0].z = 0;
		pos[1].x = pos[1].y = pos[1].z = 0;
		offset = pos[0];
		angle[0] = angle[1] = 0;
		petAngle = petAzimuth = petRoll = 0;
	}

}

//***************************************************************
void BBOPet::Draw(SpacePoint *spacePoint, int petIndex)
{
	puma->m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );


	int indexSign = 1;
	if (1 == petIndex)
	{
		indexSign = -1;
	}

	pos[0]   = spacePoint->location;
	angle[0]	= spacePoint->angle;

	if (0 == pos[1].x && 0 == pos[1].y && 0 == pos[1].z)
	{
		pos[1]   = pos[0];
		angle[1] = angle[0];
	}
	else
	{
		pos[1].x += (pos[0].x - pos[1].x)/15;
		pos[1].y += (pos[0].y - pos[1].y)/15;
		pos[1].z += (pos[0].z - pos[1].z)/15;

		angle[1] += (angle[0] - angle[1])/15;
//		angle[1] = angle[0];
	}

	PositionPetAnim(petIndex);

	if (1 == curState) // if attacking
	{
		petDragonAnims[1]->Draw(puma->m_pd3dDevice, 
				     stateCounter/2);
		if (0 == stateCounter)
		{
			// set up sourse and target points
			DrawPetAttackEffect(petIndex, 0);
			nextState = 0;
			offset.x = offset.y = offset.z = 0;
		}

		DrawPetAttackEffect(petIndex, 1);

		stateCounter++;
		if (stateCounter > 23)
		{
			curState = nextState;
			stateCounter = 0;
		}
	}
	else if (2 == curState) // if happy move 1
	{
		if (0 == stateCounter)
		{
			offset.x = offset.y = offset.z = 0;
			nextState = 0;
		}

		petDragonAnims[0]->Draw(puma->m_pd3dDevice, (stateCounter%12));

		if (2 == stateCounter)
			bboClient->psMan.Play(posDragon, PET_SOUND_CHIRP, petScaleVal);

		stateCounter++;
		if (stateCounter > 12 * 2 - 1)
		{
			curState = nextState;
			stateCounter = 0;
			offset.x = offset.y = offset.z = 0;
		}
	}
	else if (3 == curState) // fly up, glide down
	{
		if (0 == stateCounter)
		{
			offset.x = offset.y = offset.z = 0;
			nextState = 0;
		}

		if (stateCounter < 12 * 2 + 9)
		{
			petDragonAnims[0]->Draw(puma->m_pd3dDevice, (stateCounter%12));
			offset.y += 0.03f;
		}
		else if (stateCounter < 12 * 6 - 3)
		{
			petDragonAnims[0]->Draw(puma->m_pd3dDevice, 9);
			offset.y -= 0.03f;
		}
		else
		{
			petDragonAnims[0]->Draw(puma->m_pd3dDevice, (stateCounter%12));
			offset.y -= 0.03f;
		}

		if (2 == stateCounter)
			bboClient->psMan.Play(posDragon, PET_SOUND_CHIRP, petScaleVal);
		if (12 * 6 - 3 == stateCounter)
			bboClient->psMan.Play(posDragon, PET_SOUND_GRUMBLE, petScaleVal);


		stateCounter++;
		if (stateCounter > 12 * 6 - 1)
		{
			curState = nextState;
			stateCounter = 0;
			offset.x = offset.y = offset.z = 0;
		}
	}
	else if (4 == curState) // loop
	{
		if (0 == stateCounter)
		{
			offset.x = offset.y = offset.z = 0;
			nextState = 0;
		}

		petDragonAnims[0]->Draw(puma->m_pd3dDevice, (stateCounter%12));
		petAzimuth += D3DX_PI/12;

		if (12 * 1 - 9 == stateCounter)
			bboClient->psMan.Play(posDragon, PET_SOUND_CALL, petScaleVal);

		stateCounter++;
		if (stateCounter > 12 * 2 - 1)
		{
			curState = nextState;
			stateCounter = 0;
			offset.x = offset.y = offset.z = 0;
		}
	}
	else if (5 == curState) // roll
	{
		if (0 == stateCounter)
		{
			offset.x = offset.y = offset.z = 0;
			nextState = 0;
		}

		petDragonAnims[0]->Draw(puma->m_pd3dDevice, (stateCounter%12));
		petRoll += D3DX_PI/12 * indexSign;

		if (12 * 1 - 9 == stateCounter)
			bboClient->psMan.Play(posDragon, PET_SOUND_CHIRP, petScaleVal);

		stateCounter++;
		if (stateCounter > 12 * 2 - 1)
		{
			curState = nextState;
			stateCounter = 0;
			offset.x = offset.y = offset.z = 0;
		}
	}
	else if (6 == curState) // bank around
	{
		if (0 == stateCounter)
		{
			offset.x = offset.y = offset.z = 0;
			nextState = 0;
		}

		if (stateCounter < 12 * 1 + 9)
		{
			petDragonAnims[0]->Draw(puma->m_pd3dDevice, (stateCounter%12));
			offset.y += 0.03f;
		}
		else if (stateCounter < 12 * 1 + 18)  // bank to move out
		{
			petDragonAnims[0]->Draw(puma->m_pd3dDevice, (stateCounter%12));
			petRoll -= 0.03f * indexSign;
			offset.x -= sin(petRoll) * 0.1f;
			offset.y -= 0.01f;
		}
		else if (stateCounter < 12 * 6 - 18)	// glide out
		{
			petDragonAnims[0]->Draw(puma->m_pd3dDevice, 9);
			offset.x -= sin(petRoll) * 0.1f;
			offset.y -= 0.01f;
		}
		else if (stateCounter < 12 * 6 - 0)	// bank to glide in
		{
			petDragonAnims[0]->Draw(puma->m_pd3dDevice, 9);
			petRoll += 0.03f * indexSign;
			offset.x -= sin(petRoll) * 0.1f;
		}
		else if (stateCounter < 12 * 9 - 9)	// glide in
		{
			petDragonAnims[0]->Draw(puma->m_pd3dDevice, 9);
			offset.x -= sin(petRoll) * 0.1f;
			offset.y -= 0.01f;
		}
		else if (stateCounter < 12 * 9)	// finish up
		{
			petDragonAnims[0]->Draw(puma->m_pd3dDevice, (stateCounter%12));
			petRoll -= 0.03f * indexSign;
			offset.x -= sin(petRoll) * 0.1f;
		}

		if (12 * 3 - 9 == stateCounter)
			bboClient->psMan.Play(posDragon, PET_SOUND_CHIRP, petScaleVal);
		if (12 * 4 - 0 == stateCounter)
			bboClient->psMan.Play(posDragon, PET_SOUND_CALL, petScaleVal);
		if (12 * 7 - 9 == stateCounter)
			bboClient->psMan.Play(posDragon, PET_SOUND_CHIRP, petScaleVal);

		stateCounter++;
		if (stateCounter > 12 * 9 - 1)
		{
			curState = nextState;
			stateCounter = 0;
			offset.x = offset.y = offset.z = 0;
		}
	}
	else if (7 == curState) // burp
	{
		petDragonAnims[1]->Draw(puma->m_pd3dDevice, 
				     stateCounter/2);
		if (0 == stateCounter)
		{
			// set up sourse and target points
			DrawPetAttackEffect(petIndex, 0);
			nextState = 0;
			offset.x = offset.y = offset.z = 0;
		}

		DrawPetAttackEffect(petIndex, 3);

		stateCounter++;
		if (stateCounter > 23)
		{
			curState = nextState;
			stateCounter = 0;
		}
	}
	else if (8 == curState) // fly sickly
	{
		if (0 == stateCounter)
		{
			offset.x = offset.y = offset.z = 0;
			nextState = 0;
		}

		if (stateCounter < 9)
		{
			petDragonAnims[0]->Draw(puma->m_pd3dDevice, (stateCounter%12));
			offset.y -= 0.01f;
		}
		else if (stateCounter < 12 * 5 - 9)
		{
			petDragonAnims[0]->Draw(puma->m_pd3dDevice, 9);
			offset.y -= 0.01f;
		}
		else
		{
			petDragonAnims[0]->Draw(puma->m_pd3dDevice, (stateCounter%12)/2);
			offset.y += 0.01f;
		}

		if (12 * 3 - 9 == stateCounter)
			bboClient->psMan.Play(posDragon, PET_SOUND_GRUMBLE, petScaleVal);

		stateCounter++;
		if (stateCounter > 12 * 8 - 1)
		{
			curState = nextState;
			stateCounter = 0;
			offset.x = offset.y = offset.z = 0;
		}
	}
	else
	{
		petAngle = petAzimuth = petRoll = 0;
		petDragonAnims[0]->Draw(puma->m_pd3dDevice, stateCounter/2);
		stateCounter++;
		if (stateCounter > 23)
		{
			curState = nextState;

			switch(petEmotion)
			{
			case DRAGON_HEALTH_GREAT:
				nextState = rand() % (7 + 100);
				if (nextState > 100)
					nextState -= 99;    // skip 1, which is attack
				else
					nextState = 0;
				break;

			case DRAGON_HEALTH_SICK:
				nextState = rand() % 20;
				if (1 == nextState)
					nextState = 8;    // sickly move
				else
					nextState = 0;
				break;

			default:
			case DRAGON_HEALTH_NORMAL:
				nextState = 0;
				break;

			}
			stateCounter = 0;
			offset.x = offset.y = offset.z = 0;
		}

	}

}


//***************************************************************
void BBOPet::Clear(void)
{
	for (int i = 0; i < 2; i++)
	{
		if (petDragonAnims[i])
			delete petDragonAnims[i];
	}

}

extern Particle2Type   *swordParticle;
extern Particle2Type   *petHitParticle;

//***************************************************************
void BBOPet::DrawPetAttackEffect(int which, int job)
{
	D3DXCOLOR redParticleColor    = D3DCOLOR_ARGB(55, 255, 0, 0);
	D3DXCOLOR greenParticleColor  = D3DCOLOR_ARGB(55, 0, 255, 0);
	D3DXCOLOR blueParticleColor   = D3DCOLOR_ARGB(55, 0, 0, 255);
	D3DXCOLOR whiteParticleColor  = D3DCOLOR_ARGB(55, 255, 255, 255);
	D3DXCOLOR blackParticleColor  = D3DCOLOR_ARGB(235,125, 65, 125);
	D3DXCOLOR goldParticleColor   = D3DCOLOR_ARGB(55, 255, 210, 0);

	if (0 == job)
	{
		D3DXVECTOR3 pOut, pV;
		pV.x = 0; 
		pV.y = -0.2f;
		pV.z = -0.2f;

		D3DXVec3TransformCoord(&pOut, &pV, &matDragon);
		petAttackStart = pOut;
	}

	if (1 == job)
	{
		D3DXCOLOR *color = NULL;
		switch(petAttackType)
		{
		case DRAGON_TYPE_RED  :
		default:
			color = &redParticleColor;
			break;

		case DRAGON_TYPE_GREEN:
			color = &greenParticleColor;
			break;

		case DRAGON_TYPE_BLUE :
			color = &blueParticleColor;
			break;

		case DRAGON_TYPE_WHITE:
			color = &whiteParticleColor;
			break;

		case DRAGON_TYPE_BLACK:
			color = &blackParticleColor;
			break;

		case DRAGON_TYPE_GOLD :
			color = &goldParticleColor;
			break;
		}

		if (11 == stateCounter)
			bboClient->psMan.Play(petAttackStart, PET_SOUND_FIRE, petScaleVal);

		if (stateCounter > 10 && stateCounter < 22)
		{
			int step = stateCounter - 11;
			D3DXVECTOR3 pos;
			pos.x = ((petAttackEnd.x * step) + 
				      (petAttackStart.x   * (11 - step))) / 11;
			pos.y = ((petAttackEnd.y * step) + 
				      (petAttackStart.y   * (11 - step))) / 11;
			pos.z = ((petAttackEnd.z * step) + 
				      (petAttackStart.z   * (11 - step))) / 11;
			
			swordParticle->SetEmissionPoint(pos);
			swordParticle->Emit(3, *color); 
		}
		else if (22 == stateCounter)
		{
			D3DXVECTOR3 pos = petAttackEnd;
//			pos.y += 1;
			petHitParticle->SetEmissionPoint(pos);
			petHitParticle->Emit(40, *color); 
			bboClient->psMan.Play(pos, PET_SOUND_IMPACT, petScaleVal);
		}

	}

	if (3 == job) // burp
	{
		D3DXCOLOR *color = NULL;
		switch(petAttackType)
		{
		case DRAGON_TYPE_RED  :
		default:
			color = &redParticleColor;
			break;

		case DRAGON_TYPE_GREEN:
			color = &greenParticleColor;
			break;

		case DRAGON_TYPE_BLUE :
			color = &blueParticleColor;
			break;

		case DRAGON_TYPE_WHITE:
			color = &whiteParticleColor;
			break;

		case DRAGON_TYPE_BLACK:
			color = &blackParticleColor;
			break;

		case DRAGON_TYPE_GOLD :
			color = &goldParticleColor;
			break;
		}

		if (10 == stateCounter)
		{
			swordParticle->SetEmissionPoint(petAttackStart);
			swordParticle->Emit(9, *color); 
			bboClient->psMan.Play(petAttackStart, PET_SOUND_BURP, petScaleVal);
		}

	}

/*
	if (2 == job)
	{
		DWORD color = D3DCOLOR_ARGB(255, 255, 255, 255);

		D3DXVECTOR3 pOut, pV;
		pV.x = 0; 
		pV.y = 0;
		pV.z = -0.2f;

		D3DXVec3TransformCoord(&pOut, &pV, &matWorldValue);

		InfoRecord3D *itd = new InfoRecord3D(
							curCamAngle * -1 + D3DX_PI, pOut.x, pOut.y, pOut.z,
							petName, -1, color);
		infoList.Append(itd);
	}
	*/
}

//***************************************************************
void BBOPet::PositionPetAnim(int petIndex)
{
	/*
	bboClient->SetupObjectOrientation(angle[1] * -1, 
		pos[1].x + sin(-angle[1] + offsetAngle) * 100 * petScaleVal,
		pos[1].y + 1.0f,
		pos[1].z + cos(-angle[1] + offsetAngle) * 100 * petScaleVal);
	*/
	
   D3DXMATRIX MatWorld, MatLocal, MatTemp; 
   D3DXMATRIX MatRot, MatLYaw, MatLPitch, MatLRoll;
 
	float offsetAngle = D3DX_PI*0.4f;
	int indexSign = 1;
	if (1 == petIndex)
	{
		offsetAngle = -D3DX_PI*0.4f;
		indexSign = -1;
	}

   // Using the left-to-right order of matrix concatenation,
   // apply the translation to the object's world position
   // before applying the rotations.
   D3DXMatrixTranslation(&MatWorld, pos[1].x, pos[1].y, pos[1].z);
   D3DXMatrixIdentity(&MatRot);

	D3DXMatrixTranslation( &MatLocal, 
		sin(offsetAngle) * 100 * petScaleVal + offset.x,
		offset.y + 1.0f, 
		cos(offsetAngle) * 100 * petScaleVal + offset.z);

   D3DXMatrixRotationY(&MatLYaw, petAngle);           // Yaw

   D3DXMatrixRotationX(&MatLPitch, petAzimuth);       // Pitch

   D3DXMatrixRotationZ(&MatLRoll, petRoll); 

   D3DXMatrixMultiply(&MatLPitch, &MatLRoll, &MatLPitch);

   D3DXMatrixMultiply(&MatLYaw, &MatLPitch, &MatLYaw);

   D3DXMatrixMultiply(&MatLocal, &MatLYaw, &MatLocal);

   D3DXMatrixRotationY(&MatTemp, angle[1] * -1);           // Yaw
   D3DXMatrixMultiply(&MatRot, &MatRot, &MatTemp);
   // Apply the rotation matrices to complete the world matrix.
   D3DXMatrixMultiply(&MatRot, &MatLocal, &MatRot);

   D3DXMatrixMultiply(&MatWorld, &MatRot, &MatWorld);

	matDragon = MatWorld;

	posDragon.x = posDragon.y = posDragon.z = 0;
	D3DXVec3TransformCoord(&posDragon, &posDragon, &matDragon);

   puma->m_pd3dDevice->SetTransform( D3DTS_WORLD, &MatWorld );
}

/* end of file */



