
#include "longtime.h"


//******************************************************************
LongTime::LongTime()
{
	SetToNow();
}

//******************************************************************
void LongTime::SetToNow(void)
{
	GetSystemTime( &value );
}

//******************************************************************
long LongTime::MinutesDifference(LongTime *future)
{
	FILETIME thisTime, futureTime;
	LARGE_INTEGER thisNum, futureNum;

	SystemTimeToFileTime(&value, &thisTime);
	SystemTimeToFileTime(&future->value, &futureTime);

	memcpy(&thisNum   ,&thisTime  , sizeof(thisNum));
	memcpy(&futureNum ,&futureTime, sizeof(thisNum));

	long result = (futureNum.QuadPart - thisNum.QuadPart) / (LONGLONG)60 / (LONGLONG)10000000;
 
	return result;
}


//******************************************************************
void LongTime::AddMinutes(long minutes)
{
	FILETIME thisTime;
	LARGE_INTEGER thisNum;

	SystemTimeToFileTime(&value, &thisTime);

	memcpy(&thisNum   ,&thisTime  , sizeof(thisNum));

	thisNum.QuadPart += (LONGLONG)minutes * (LONGLONG)60 * (LONGLONG)10000000;

	memcpy(&thisTime  ,&thisNum  , sizeof(thisTime));

	FileTimeToSystemTime(&thisTime, &value);
}

/* end of file */



