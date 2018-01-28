#ifndef AUTOLOG_H
#define AUTOLOG_H

class AutoLog
{
public:
	void Start(char *appName = "BMO");
	void Append(void);
	void Finish(void);
	void Log(char *);
	void Log(int);
	void Log(float);

	int logEnabled;
};

extern AutoLog aLog;

#endif
