
#ifndef __LAUNCHERCONFIG_H
#define __LAUNCHERCONFIG_H

// Class to load and save the config for the Launcher
class LauncherConfig {
private:
	int			lastServer;
	int			lastResolution;
	int			lastMode;
	char		userName[80];

public:
	LauncherConfig();	// constructor

	void LoadConfig();
	void SaveConfig();

	int getLastServer();
	int getLastResolution();
	int getLastMode();
	char* getUserName();

	void setLastServer( int );
	void setLastResolution( int );
	void setLastMode( int );
	void setUserName( char* );
};

#endif