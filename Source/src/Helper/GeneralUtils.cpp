/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#include <mmsystem.h>
#include <d3dx8.h>
#include <math.h>
#include <Iphlpapi.h> 

#include "GeneralUtils.h"
#include "crc.h"

COLORREF normalColor = RGB(180, 180, 230);
COLORREF selectColor = RGB(100, 255, 100);

int keysPressed;



//************************************************************************
int NextWord(char * textBuff, int *linePoint)
{
	int retVal = *linePoint;

	while (	( textBuff[(*linePoint)] > 256 || textBuff[(*linePoint)] < 0 ) || ( !isspace(textBuff[(*linePoint)]) && textBuff[(*linePoint)] != 0) )
		(*linePoint)++;
	while (	( textBuff[(*linePoint)] > 256 || textBuff[(*linePoint)] < 0 ) || ( isspace(textBuff[(*linePoint)])  && textBuff[(*linePoint)] != 0) )
		(*linePoint)++;

	return(retVal);
}

//************************************************************************
int SkipSpaces(char * textBuff, int *linePoint)
{
	int retVal = *linePoint;

	while (	isspace(textBuff[(*linePoint)])  && textBuff[(*linePoint)] != 0)
		(*linePoint)++;

	return(retVal);
}

//*************************************************************************
void LoadLineToString(FILE *dest, char *string, int *eorFlag)
{
	if (eorFlag)
	   *eorFlag = (int)fgets(string, 2047, dest);
	else
	   fgets(string, 2047, dest);

   int i = strlen(string) - 1;
   char c = string[i];
   while ('\n' == string[i])
   {
      string[i] = 0;
      i = strlen(string) - 1; 
   }

}



//*************************************************************************
float Distance(float x1, float y1, float x2, float y2)
{

   return (float) sqrt((x1-x2)*(x1-x2) + (y1-y2) * (y1-y2) );

}


//*************************************************************************
float TurnTowardsIntPoint(float x, float y, int targetX, int targetY, 
                             float heading, float maxDelta)
{
	
	float ttpDir, ttpChange;
//   float x,y;

    ttpDir = (float) atan2(targetX - x, y - targetY);
//  ttpDir = atan2(x - targetX, y - targetY) + D3DX_PI;

    // normalize our vector.
    while (ttpDir < 0 && heading >= 0)
        ttpDir += D3DX_PI * 2;
    while (ttpDir >= 0 && heading < 0)
        ttpDir -= D3DX_PI * 2;

    while (heading - ttpDir > D3DX_PI * 2)
        ttpDir += D3DX_PI * 2;
    while (ttpDir - heading > D3DX_PI * 2)
        ttpDir -= D3DX_PI * 2;


    ttpChange = ttpDir - heading; //heading - ttpDir;

    if (ttpChange > D3DX_PI)
    {
        ttpDir -= D3DX_PI * 2;    
        ttpChange = ttpDir - heading; //heading - ttpDir;
    }
    if (ttpChange < -1 * D3DX_PI)
    {
        ttpDir += D3DX_PI * 2;    
        ttpChange = ttpDir - heading; //heading - ttpDir;
    }


    if (maxDelta > 0.0)
    {
        if (ttpChange > maxDelta)
            ttpChange = maxDelta;
        else
        if (-1 * ttpChange > maxDelta)
            ttpChange = -1 * maxDelta;
        return (ttpChange);
    }
    else
        return (ttpDir);    

}


//*************************************************************************
float TurnTowardsPoint(float x, float y, float targetX, float targetY, 
                             float heading, float maxDelta)
{
	
	float ttpDir, ttpChange;
//   float x,y;

    ttpDir = (float) atan2(targetX - x, y - targetY);
//  ttpDir = atan2(x - targetX, y - targetY) + D3DX_PI;

    // normalize our vector.
    while (ttpDir < 0 && heading >= 0)
        ttpDir += D3DX_PI * 2;
    while (ttpDir >= 0 && heading < 0)
        ttpDir -= D3DX_PI * 2;

    while (heading - ttpDir > D3DX_PI * 2)
        ttpDir += D3DX_PI * 2;
    while (ttpDir - heading > D3DX_PI * 2)
        ttpDir -= D3DX_PI * 2;


    ttpChange = ttpDir - heading; //heading - ttpDir;

    if (ttpChange > D3DX_PI)
    {
        ttpDir -= D3DX_PI * 2;    
        ttpChange = ttpDir - heading; //heading - ttpDir;
    }
    if (ttpChange < -1 * D3DX_PI)
    {
        ttpDir += D3DX_PI * 2;    
        ttpChange = ttpDir - heading; //heading - ttpDir;
    }


    if (maxDelta > 0.0)
    {
        if (ttpChange > maxDelta)
            ttpChange = maxDelta;
        else
        if (-1 * ttpChange > maxDelta)
            ttpChange = -1 * maxDelta;
        return (ttpChange);
    }
    else
        return (ttpDir);    

}


//*******************************************************************************
double NormalizeAngle(double angle)
{
   while (angle < 0)
   	  angle += D3DX_PI * 2;
   while (angle >= D3DX_PI * 2)
   	  angle -= D3DX_PI * 2;

   return angle;
};

//*******************************************************************************
double NormalizeAngle2(double angle)
{
   while (angle > D3DX_PI)
   	  angle -= D3DX_PI * 2;
   while (angle <= -1 * D3DX_PI)
   	  angle += D3DX_PI * 2;

   return angle;
};


//*************************************************************************
void HSVtoRGB(float h, float s, float v, float &r, float &g, float &b) 
{ 
    int i; 
    float f, p, q, t,hTemp; 
  
    if( s == 0.0 || h == -1.0) // s==0? Totally unsaturated = grey so R,G and B all equal value 
    { 
      r = g = b = v; 
      return; 
    } 
    hTemp = h/60.0f; 
    i = (int)floor( hTemp );                 // which sector 
    f = hTemp - i;                      // how far through sector 
    p = v * ( 1 - s ); 
    q = v * ( 1 - s * f ); 
    t = v * ( 1 - s * ( 1 - f ) ); 
  
    switch( i )  
    { 
    case 0:{r = v;g = t;b = p;break;} 
    case 1:{r = q;g = v;b = p;break;} 
    case 2:{r = p;g = v;b = t;break;} 
    case 3:{r = p;g = q;b = v;break;}  
    case 4:{r = t;g = p;b = v;break;} 
    case 5:{r = v;g = p;b = q;break;} 
    } 
} 
  
//*************************************************************************
void RGBtoHSV(float r, float g, float b, float &h, float &s, float &v) 
 { 
    float mn=r,mx=r; 
    int maxVal=0; 
  
    if (g > mx){ mx=g;maxVal=1;} 
    if (b > mx){ mx=b;maxVal=2;}  
    if (g < mn) mn=g; 
    if (b < mn) mn=b;  
    float  delta = mx - mn; 
  
    v = mx;  
    if( mx != 0 ) 
      s = delta / mx;  
    else  
    { 
      s = 0; 
      h = 0; 
      return; 
    } 
    if (s==0.0f) 
    { 
      h=-1; 
      return; 
    } 
    else 
    {  
      switch (maxVal) 
      { 
      case 0:{h = ( g - b ) / delta;break;}         // yel < h < mag 
      case 1:{h = 2 + ( b - r ) / delta;break;}     // cyan < h < yel 
      case 2:{h = 4 + ( r - g ) / delta;break;}     // mag < h < cyan 
      } 
    } 
  
    h *= 60; 
    if( h < 0 ) h += 360; 
}

//*************************************************************************
void CopyStringSafely(char *src, int srcBufferSize, char *dst, int dstBufferSize)
{
	int stringSize = 0;
	while (src[stringSize] != 0 && stringSize < srcBufferSize-1) 
		++stringSize;

	int amountToCopy = dstBufferSize-1;
	if (amountToCopy > srcBufferSize-1)
		amountToCopy = srcBufferSize-1;
	if (amountToCopy > stringSize)
		amountToCopy = stringSize;

	memcpy(dst,src,amountToCopy);
	dst[amountToCopy] = 0;
}

//*************************************************************************
void AppendStringSafely(char *src, int srcBufferSize, char *dst, int dstBufferSize)
{
	int dstStringSize = 0;
	while (dst[dstStringSize] != 0 && dstStringSize < dstBufferSize-1) 
		++dstStringSize;

	CopyStringSafely(src, srcBufferSize, &dst[dstStringSize], dstBufferSize-dstStringSize);
}

//*************************************************************************
void CorrectString(char *string)
{
	int stringSize = 0;
	while (string[stringSize] != 0)
	{
		if ('%' == string[stringSize])
			string[stringSize] = '-';
		if ('\\' == string[stringSize])
			string[stringSize] = '-';
		if ('/' == string[stringSize])
			string[stringSize] = '-';
		++stringSize;
	}
}

//*************************************************************************
void RemoveStringTrailingSpaces(char *string)
{
	int stringSize = strlen(string);
	while (stringSize > 0)
	{
		if (' ' == string[stringSize-1])
			string[stringSize-1] = 0;
		else
			return;
		--stringSize;
	}
}

//*************************************************************************
void GuaranteeTermination(char *string, int size)
{
	string[size-1] = 0;
}

//*******************************************************************************
int IsSame(char *a, char *b)
{
	if (!strnicmp( a , b , strlen(b)))
		return TRUE;
	return FALSE;
}

//*******************************************************************************
int IsCompletelySame(char *a, char *b)
{
	if (!strnicmp( a , b , strlen(b)))
		if (!strnicmp( b , a , strlen(a)))
			return TRUE;
	return FALSE;
}

//*******************************************************************************
int IsCompletelyVisiblySame(char *a, char *b)
{
	char tempA[1024], tempB[1024];

	sprintf(tempA,a);
	int i = 0;
	while (tempA[i] != 0)
	{
		if ('l' == tempA[i])
			tempA[i] = '1';
		if ('i' == tempA[i])
			tempA[i] = '1';
		if ('j' == tempA[i])
			tempA[i] = '1';
		if (' ' == tempA[i])
			tempA[i] = '-';
		if ('!' == tempA[i])
			tempA[i] = '1';
		if ('$' == tempA[i])
			tempA[i] = 'S';
		if ('_' == tempA[i])
			tempA[i] = '-';

		++i;
	}

	sprintf(tempB,b);
	i = 0;
	while (tempB[i] != 0)
	{
		if ('l' == tempB[i])
			tempB[i] = '1';
		if ('i' == tempB[i])
			tempB[i] = '1';
		if ('j' == tempB[i])
			tempB[i] = '1';
		if (' ' == tempB[i])
			tempB[i] = '-';
		if ('!' == tempB[i])
			tempB[i] = '1';
		if ('$' == tempB[i])
			tempB[i] = 'S';
		if ('_' == tempB[i])
			tempB[i] = '-';

		++i;
	}

	if (!strnicmp( tempA , tempB , strlen(tempB)))
		if (!strnicmp( tempB , tempA , strlen(tempA)))
			return TRUE;
	
	return IsCompletelySame(a, b);
}

const int BAD_WORD_LIST_LEN = 32;

char badWordList[BAD_WORD_LIST_LEN][32] =
{
        {"fuck"},
        {"piss"},
        {"shit"},
        {"cock"},
        {"cunt"},
        {"pussy"},
        {"nigger"},
        {"twat"},
        {"vagina"},
        {"marijuana"},
        {"cocaine"},
        {"bitch"},
        {"bastard"},
        {"whore"},
        {"slut"},
        {"pimp"},
        {"chink"},
        {"asshole"},
        {"dumbass"},
        {"dumass"},
        {"jizz"},
        {"damn"},
        {"beatch"},
        {"beotch"},
        {" ass "},
        {"spick"},
        {"masturbate"},
        {"lesbian"},
        {"faggot"},
        {"fag"},
        {"phallus"},
        {"penis"}
};

//*************************************************************************
void CleanString(char *string, long args)
{
	char tempText[1024];

	// replace % with -
	int stringSize = 0;
	while (string[stringSize] != 0)
	{
		if ('%' == string[stringSize])
			string[stringSize] = '-';
		if ((args & CLEANSTRING_COMMAS) && ',' == string[stringSize])
			string[stringSize] = '-';
		if (string[stringSize] < ' ' || string[stringSize] > 'z')
			string[stringSize] = '0';

		++stringSize;
	}

	int start = 0;
	while (start < (int)strlen(string) && ' ' == string[start])
		++start;
	if (start >= (int)strlen(string))
	{
		sprintf(tempText,"A");
	}
	else
		sprintf(tempText, &string[start]);  // get rid of whitespace

	// fix bad words
	stringSize = 0;
	while (tempText[stringSize] != 0)
	{
		for (int i = 0; i < BAD_WORD_LIST_LEN; ++i)
		{
			if (IsSame(&tempText[stringSize],badWordList[i]))
			{
				for (int j = stringSize; j < stringSize + (int)strlen(badWordList[i]); ++j)
					tempText[j] = '-';
			}
		}
		++stringSize;
	}

	sprintf(string,tempText);
}

//*************************************************************************
void NormalizeRect(RECT &r)
{
	RECT box = r;

	if (r.left > r.right)
	{
		r.right = box.left;
		r.left  = box.right;
	}
	if (r.top > r.bottom)
	{
		r.bottom = box.top;
		r.top    = box.bottom;
	}
}


//*******************************************************************************
void CheckForLineCollision(CollisionLine movement, CollisionLine line, 
									double &x, double &y, double &ua, double &ub)
{
					// (y4 - y3)*(x2 - x1) - (x4 - x3)*(y2 - y1)
	double denom = (line.destY - line.sourceY) * (movement.destX - movement.sourceX) -
	               (line.destX - line.sourceX) * (movement.destY - movement.sourceY) ;

	                // (x4 - x3)*(y1 - y3) - (y4 - y3)*(x1 - x3)
	ua =    (line.destX - line.sourceX) * (movement.sourceY - line.sourceY) -
	        (line.destY - line.sourceY) * (movement.sourceX - line.sourceX) ;

	ua = ua / denom;

	                // (x2 - x1)*(y1 - y3) - (y2 - y1)*(x1 - x3)
	ub =    (movement.destX - movement.sourceX) * (movement.sourceY - line.sourceY) -
	        (movement.destY - movement.sourceY) * (movement.sourceX - line.sourceX) ;

	ub = ub / denom;

	if (ua >= 0.0 && ua <= 1.0)
	{
         x = movement.sourceX + ua*(movement.destX - movement.sourceX);
         y = movement.sourceY + ua*(movement.destY - movement.sourceY);
	}
	else
	{
		x = y = 0;
	}
}


const int TY_WORD_LIST_LEN = 5;

char thanksWordList[TY_WORD_LIST_LEN][32] =
{
	{"Thank You"},
	{"Thanks"},
	{"ty"},
	{"thanx"},
	{"thnx"}
};

//*************************************************************************
int StringContainsThanks(char *string)
{
//	char tempText[1024];

	// find words
	int stringSize = 0;
	while (string[stringSize] != 0)
	{
		for (int i = 0; i < TY_WORD_LIST_LEN; ++i)
		{
			if (IsSame(&string[stringSize],thanksWordList[i]))
				return TRUE;
		}
		++stringSize;
	}

	return FALSE;
}


const int WELCOME_WORD_LIST_LEN = 7;

char welcomeWordList[WELCOME_WORD_LIST_LEN][32] =
{
	{"You're Welcome"},
	{"Youre Welcome"},
	{"Your Welcome"},
	{"yw"},
	{"No problem"},
	{"No prob"},
	{"np"}
};

//*************************************************************************
int StringContainsWelcome(char *string)
{
//	char tempText[1024];

	// find words
	int stringSize = 0;
	while (string[stringSize] != 0)
	{
		for (int i = 0; i < WELCOME_WORD_LIST_LEN; ++i)
		{
			if (IsSame(&string[stringSize],welcomeWordList[i]))
				return TRUE;
		}
		++stringSize;
	}

	return FALSE;
}


const int PLEASE_WORD_LIST_LEN = 3;

char pleaseWordList[PLEASE_WORD_LIST_LEN][32] =
{
	{"please"},
	{"pls"},
	{"plz"}
};

//*************************************************************************
int StringContainsPlease(char *string)
{
//	char tempText[1024];

	// find words
	int stringSize = 0;
	while (string[stringSize] != 0)
	{
		for (int i = 0; i < PLEASE_WORD_LIST_LEN; ++i)
		{
			if (IsSame(&string[stringSize],pleaseWordList[i]))
				return TRUE;
		}
		++stringSize;
	}

	return FALSE;
}


//*************************************************************************
void RemoveCommasAndReturnsFromString(char *string)
{

	// replace with -
	int stringSize = 0;
	while (string[stringSize] != 0)
	{
		if (',' == string[stringSize])
			string[stringSize] = '-';
		if ('\n' == string[stringSize])
			string[stringSize] = '-';

		++stringSize;
	}

}

//*************************************************************************
int Bracket(int source, int low, int high)
{
	if (source < low)
		source = low;
	if (source > high)
		source = high;

	return source;
}

//*************************************************************************
float Bracket(float source, float low, float high)
{
	if (source < low)
		source = low;
	if (source > high)
		source = high;

	return source;
}

//***************************************************************
int GetUniqueComputerId()
{
	int iRet = 0;

	PIP_ADAPTER_INFO pInfo;
	char* pbBuffer;
	char* pbStr;
	ULONG ulSize = 0;

	GetAdaptersInfo(NULL, &ulSize);
	pbBuffer = new char[ulSize];
	pbStr = new char[256];
	GetAdaptersInfo((PIP_ADAPTER_INFO)pbBuffer, &ulSize);
	pInfo = (PIP_ADAPTER_INFO)pbBuffer;

	while (pInfo != NULL)
	{
		if (pInfo->Type == MIB_IF_TYPE_ETHERNET && pInfo->AddressLength == 6)
			sprintf(pbStr, 
					"%02X %02X %02X %02X %02X %02X", 
					pInfo->Address[0],
					pInfo->Address[1],
					pInfo->Address[2],
					pInfo->Address[3],
					pInfo->Address[4],
					pInfo->Address[5] );
		pInfo = pInfo->Next;
	} 

	iRet = GetCRCForString(pbStr);
	delete [] pbBuffer;
	delete [] pbStr;

	return iRet;
}

/* end of file */
