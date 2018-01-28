
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include ".\Helper\GeneralUtils.h"
#include ".\Helper\TweakSystem.h"
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
	// for each point,
	FlockPoint *fp = (FlockPoint *) points->First();
	while (fp)
	{
		float realSpeed = fp->maxSpeed * timeDelta;

		if (fp->owned && realSpeed > 0.0f)
		{
			// start out with a vector towards the target point
			float tX = fp->x;
			float tY = fp->y;
			float myTargetX = targetX + fp->targetX;
			float myTargetY = targetY + fp->targetY;
			float oldAngle = fp->angle;

			fp->angle += TurnTowardsPoint(tX, tY, myTargetX, myTargetY, fp->angle, fp->maxTurn * timeDelta); 
			float dist = Distance(tX, tY, myTargetX, myTargetY);

			int isClose = FALSE;
			float tweakF = gTweakSystem.GetTweak("FLOCKINGRANGECOEFF", 3, 
				                            1, 6,
													 "coefficient of flocking closeness; bigger means more space between flock objects");

			if (!fp->isClose && dist < realSpeed * tweakF)// * 2)
			{
//				fp->angle = TurnTowardsPoint(tX, tY, (int)myTargetX, (int)myTargetY, 0,0); 
				isClose = TRUE;
			}
			else if (fp->isClose && dist < realSpeed * tweakF * 4.7f)// * 2)
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
					tX = tY = 0;
				}
				else
				{
					tX = (float)sin(fp->angle) * dist;
					tY = (float)cos(fp->angle) * dist;
				}

				fp->isClose = isClose;

				// go through all other points,
				if (!isClose && fp->isCaring)
				{
					FlockPoint *otherPoint = (FlockPoint *) points->First();
					while (otherPoint)
					{	
						if (otherPoint != fp && otherPoint->owned &&
							 fabs(fp->x - otherPoint->x) < 0.01f   &&
							 fabs(fp->y - otherPoint->y) < 0.01f)
						{
							fp->x += 0.05f;
						}
						if (otherPoint != fp && otherPoint->owned &&
							 fabs(fp->x - otherPoint->x) < (fp->size + otherPoint->size)*3  &&
							 fabs(fp->y - otherPoint->y) < (fp->size + otherPoint->size)*3)
						{
							// add to the base vector by the closeness of the other point	
							float angle = TurnTowardsPoint(fp->x, fp->y, otherPoint->x, otherPoint->y, 0.0f, 0.0f); 
							dist = Distance(fp->x, fp->y, otherPoint->x, otherPoint->y);
							float coeff = 1/dist * otherPoint->size;
							tweakF = gTweakSystem.GetTweak("FLOCKINGRANGECUTOFF", 0.4f, 
				                            0.3f, 0.5f,
													 "least the flocking 'push' force can be before it's ignored.");
							if (coeff < tweakF)
								coeff = 0.000;
							else
								coeff -= tweakF;

							tX -= (float)sin(angle) * coeff;
							tY -= (float)cos(angle) * coeff;

						}
						otherPoint = (FlockPoint *) points->Next();
					}
				}
				// and try to move towards the final vector.

				fp->x += tX;
				fp->y -= tY;

				fp->lastDistMovedSQR = tX * tX + tY * tY;

				if (isClose)
					fp->isCaring = TRUE;
			}
		}
		points->Find(fp);
		fp = (FlockPoint *) points->Next();
	}

}



/* end of file */



