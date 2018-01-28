/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

#ifndef GENERAL_UTILS_H
#define GENERAL_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include "linklist.h"

#ifndef PI
const double PI =  3.1415927;
#endif

const int ONE_SECOND = 30;


extern float Distance(float x1, float y1, float x2, float y2);

extern float TurnTowardsIntPoint(float x, float y, int targetX, int targetY, 
                             float heading, float maxDelta);
extern float TurnTowardsPoint(float x, float y, float targetX, float targetY, 
                             float heading, float maxDelta);

extern void HSVtoRGB(float h, float s, float v, float &r, float &g, float &b);
extern void RGBtoHSV(float r, float g, float b, float &h, float &s, float &v);

extern int Bracket(int source, int low, int high);
extern float Bracket(float source, float low, float high);

int NextWord(char * textBuff, int *linePoint);
int SkipSpaces(char * textBuff, int *linePoint);
void LoadLineToString(FILE *dest, char *string, int *eorFlag = NULL);

extern double NormalizeAngle(double angle);
extern double NormalizeAngle2(double angle);

extern void NormalizeRect(RECT &r);

extern COLORREF normalColor, selectColor;

// defined in main.cpp
//extern MouseManager *mouseMan;

const int KEY_ACCEL = 0x01;
const int KEY_BRAKE = 0x02;
const int KEY_LEFT  = 0x04;
const int KEY_RIGHT = 0x08;
const int KEY_FIRE1 = 0x10;
const int KEY_FIRE2 = 0x20;
const int KEY_FIRE3 = 0x40;
const int KEY_FIRE4 = 0x80;

extern int keysPressed;

// Helper function to stuff a FLOAT into a DWORD argument
inline DWORD FtoDW( FLOAT f ) { return *((DWORD*)&f); }

// Simple function for generating random numbers
inline FLOAT rnd( FLOAT low, FLOAT high )
{
    return low + ( high - low ) * ( (FLOAT)rand() ) / RAND_MAX;
}

const long CLEANSTRING_COMMAS = 0x0001;

void CopyStringSafely(char *src, int srcBufferSize, char *dst, int dstBufferSize);
void AppendStringSafely(char *src, int srcBufferSize, char *dst, int dstBufferSize);
void CorrectString(char *string);
void GuaranteeTermination(char *string, int size);
int  IsSame(char *a, char *b);
int  IsCompletelySame(char *a, char *b);
int  IsCompletelyVisiblySame(char *a, char *b);
void CleanString(char *string, long args = CLEANSTRING_COMMAS);
void RemoveStringTrailingSpaces(char *string);
void RemoveCommasAndReturnsFromString(char *string);

int StringContainsThanks(char *string);
int StringContainsWelcome(char *string);
int StringContainsPlease(char *string);

int GetUniqueComputerId();

//*******************************************************************************
struct CollisionLine
{
	double sourceX;
	double sourceY;
	double destX;  
	double destY;  
};

extern void CheckForLineCollision(CollisionLine movement, CollisionLine line, 
									double &x, double &y, double &ua, double &ub);
#endif
