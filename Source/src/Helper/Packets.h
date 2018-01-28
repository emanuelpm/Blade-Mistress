//
// Packet Definitions
//

#define PACKET_PLAYERID			0
#define PACKET_PLAYERCOMMAND	1
#define PACKET_TEXTRESPONSE		2

struct stPlayerCommandPacket
{
	stPacketHeader	stHeader;
	char			szPlayerID[32];
	char			szCommand[32];
	char			szTarget[32];
};

struct stTextResponsePacket
{
	stPacketHeader	stHeader;
	char			szResponse[2048];
};

struct stDataResponsePacket
{
	stPacketHeader	stHeader;
	char			szResponse[2048];
};

struct stPlayerIDResponsePacket
{
	stPacketHeader	stHeader;
	ULONG			iID;
};