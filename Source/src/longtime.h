#ifndef LONGTIME_H
#define LONGTIME_H

#include <windows.h>

class LongTime
{
public:
	LongTime();

	void SetToNow(void);
	void AddMinutes(long minutes);
	long MinutesDifference(LongTime *futureTime);

	SYSTEMTIME value;
};


#endif
