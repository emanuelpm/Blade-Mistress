
#include "LauncherConfig.h"
#include <fstream>

using namespace std;


LauncherConfig::LauncherConfig() 
: lastServer( 0 ), lastResolution( 0 ), lastMode( 0 )
{
	strcpy( userName, "User Name" );
}


void LauncherConfig::LoadConfig() 
{
	ifstream in;
	in.open( "launcher_config.dat" );

	in >> lastServer >> lastMode >> lastResolution;
	in.getline( userName, 80 );
	in.getline( userName, 80 );

	in.close();
}

void LauncherConfig::SaveConfig() 
{
	ofstream out;
	out.open( "launcher_config.dat" );

	out << lastServer << endl
		<< lastMode << endl
		<< lastResolution << endl
		<< userName;

	out.close();
}


// Get functions to retrieve the data
int	LauncherConfig::getLastServer() { return lastServer; }
int LauncherConfig::getLastResolution() { return lastResolution; }
int LauncherConfig::getLastMode() { return lastMode; }
char* LauncherConfig::getUserName() { return userName; }


// Set functions to store config data
void LauncherConfig::setLastServer( int last ) { lastServer = last; }
void LauncherConfig::setLastResolution( int res ) { lastResolution = res; }
void LauncherConfig::setLastMode( int mode ) { lastMode = mode; }
void LauncherConfig::setUserName( char* str ) { strcpy(userName, str); }
