
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include ".\helper\GeneralUtils.h"
#include ".\helper\TweakSystem.h"
#include "FlockPoints.h"

//******************************************************************
//******************************************************************
FlockPoint::FlockPoint(int doid, char *doname) : DataObject(doid,doname)
{
	owned = TRUE;
	size = 10.0f;
	isCaring = TRUE;
	targetX = x = 0;
	targetY = y = 0;
	lastDistMovedSQR = 0;
	ownerPtr = NULL;
}

//******************************************************************
FlockPoint::~FlockPoint()
{
}


//******************************************************************
//******************************************************************
FlockSet::FlockSet(void)
{
	points = new DoublyLinkedList();

}

//******************************************************************
FlockSet::~FlockSet()
{
	delete points;
}

//******************************************************************
unsigned long FlockSet::MakePoint(void *owner)
{
	unsigned long res = 0;
	FlockPoint *fp = (FlockPoint *) points->First();
	while (fp)
	{
		if (fp->owned)
		{
			if (fp->WhatAmI() >= res)
				res = fp->WhatAmI() + 1;
		}
		else
		{
			fp->owned = TRUE;
			fp->ownerPtr = owner;
			return fp->WhatAmI();
		}

		fp = (FlockPoint *) points->Next();
	}

	fp = new FlockPoint(res,"FLOCKPOINT");
	fp->ownerPtr = owner;
	points->Append(fp);

	return res;
}

//******************************************************************
FlockPoint *FlockSet::GetPoint(unsigned long index, void *owner)
{
	FlockPoint *fp = (FlockPoint *) points->Find(index);
	assert(owner == fp->ownerPtr);
	return fp;
}

//******************************************************************
void FlockSet::FreePoint(unsigned long index, void *owner)
{
	FlockPoint *fp = (FlockPoint *) points->Find(index);
	assert(owner == fp->ownerPtr);
	if (fp)
		fp->owned = FALSE;
}

//******************************************************************
void FlockSet::Tick(float timeDelta, float targetX, float targetY)
{
//	if (timeDelta > 4)
//		timeDelta = 4;

	// for each point,
	FlockPoint *fp = (FlockPoint *) points->First();
	while (fp)
	{
		float realSpeed = fp->maxSpeed * timeDelta;

		if (fp->owned && realSpeed > 0.0f)
		{
			fp->angle = TurnTowardsPoint(fp->x, fp->y, fp->targetX, fp->targetY, 0.0f, 0.0f); 
			float dist = Distance(fp->x, fp->y, fp->targetX, fp->targetY);
			if (dist > realSpeed)
				dist = realSpeed;

			fp->lastDistMovedSQR = 20;

			fp->x += (float)sin(fp->angle) * dist;
			fp->y -= (float)cos(fp->angle) * dist;
		}

		fp = (FlockPoint *) points->Next();
	}
}

#if 0
//******************************************************************
void FlockSet::Tick(float timeDelta, float targetX, float targetY)
{
	if (timeDelta > 4)
		timeDelta = 4;

	// for each point,
	FlockPoint *fp = (FlockPoint *) points->First();
	while (fp)
	{
		float realSpeed = fp->maxSpeed * timeDelta;

		float dist, tweakF;

		if (fp->owned && realSpeed > 0.0f)
		{
			// start out with a vector towards the target point
			float tX = fp->x;
			float tY = fp->y;
			float myTargetX = targetX + fp->targetX;
			float myTargetY = targetY + fp->targetY;
			float oldAngle = fp->angle;

			// go through all other points,
			float devX, devY;
			CalcPushFromOthers(fp, timeDelta, devX, devY);

			fp->angle += TurnTowardsPoint(tX, tY, (int)myTargetX, (int)myTargetY, fp->angle, fp->maxTurn * timeDelta); 
			dist = Distance(tX, tY, myTargetX, myTargetY);

			int isClose = FALSE;
			
			tweakF = gTweakSystem.GetTweak("FLOCKINGRANGECOEFF", 3, 
				                            1, 6,
													 "coefficient of flocking closeness; bigger means more space between flock objects");
			if (dist < 0.2f) //realSpeed * tweakF)// * 2)
			{
//				fp->angle = TurnTowardsPoint(tX, tY, (int)myTargetX, (int)myTargetY, 0,0); 
				isClose = TRUE;
			}
			
			if (dist > realSpeed)
				dist = realSpeed;

			{

				if (isClose)
				{
//					oldAngle       = NormalizeAngle2(oldAngle);
//					fp->targetAngle = NormalizeAngle2(fp->targetAngle);
//					fp->angle += (fp->targetAngle - oldAngle)/2;
					fp->angle = oldAngle + TurnTowardsPoint(0, 0, 100.0f * sin(fp->targetAngle), 100.0f * -cos(fp->targetAngle), oldAngle, fp->maxTurn/2 * timeDelta); 
//					fp->angle = fp->targetAngle;
					tX = fp->x;
					tY = fp->y;
//					tX = tY = 0;
				}
				else
				{
					tX += (float)sin(fp->angle) * dist;
					tY -= (float)cos(fp->angle) * dist;
				}
			  /*
				fp->isClose = isClose;

				if (isClose || !fp->isCaring)
				{
					tX = fp->x;
					tY = fp->y;
				}
				*/
				// and try to move towards the final vector.

				fp->lastDistMovedSQR = (tX - fp->x) * (tX - fp->x) + (tY - fp->y) * (tY - fp->y);

				fp->x = tX;
				fp->y = tY;

//				if (isClose)
//					fp->isCaring = TRUE;
			}
		}
		points->Find(fp);
		fp = (FlockPoint *) points->Next();
	}

}
#endif

//******************************************************************
void FlockSet::CalcPushFromOthers(FlockPoint *fp, float timeDelta, float &devX, float &devY)
{
	devX = devY = 0;

	FlockPoint *otherPoint = (FlockPoint *) points->First();
	while (otherPoint)
	{	
		if (otherPoint != fp && otherPoint->owned &&
			 abs(fp->x - otherPoint->x) < (fp->size + otherPoint->size)*3  &&
			 abs(fp->y - otherPoint->y) < (fp->size + otherPoint->size)*3)
		{
			// add to the base vector by the closeness of the other point	
			float angle = TurnTowardsPoint(fp->x, fp->y, (int)otherPoint->x, (int)otherPoint->y, 0.0f, 0.0f); 
			float dist = Distance(fp->x, fp->y, otherPoint->x, otherPoint->y);
			if (dist < 0.0001f)
				dist = 0.0001f;
			float coeff = 1/dist * otherPoint->size * timeDelta;
			float tweakF = gTweakSystem.GetTweak("FLOCKINGRANGECUTOFF", 0.4f, 
                            0.3f, 0.5f,
									 "least the flocking 'push' force can be before it's ignored.");
			if (coeff < tweakF)
				coeff = 0.000;
			else
				coeff -= tweakF;

			devX -= (float)sin(angle) * coeff;
			devY += (float)cos(angle) * coeff;

		}
		otherPoint = (FlockPoint *) points->Next();
	}

	points->Find(fp);
}


/* end of file */



