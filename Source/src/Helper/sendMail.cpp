
#include <windows.h>
#include <stdio.h>
//#include <mapix.h>
#include <mapi.h>

LPMAPILOGON lpfnMAPILogon;

LPMAPISENDMAIL lpfnMAPISendMail;

LPMAPILOGOFF lpfnMAPILogoff;

MapiRecipDesc recipient =

{

	0, MAPI_TO,
	// Add a valid name and email if you want emails :)
	"PERSONS NAME", "SMTP:EMAILADDR",

	0, NULL

};

MapiMessage message =

{

	0, "Server Mail",
	// Feel free to personalize this wonderful email
	"Hello, PERSON!\n",

	NULL, NULL, NULL, 0, NULL, 1, &recipient, 0, NULL

};

void SendMailMessage(char *address, char *text)
{
	/*
	recipient.lpszAddress = address;

	message.lpszNoteText = text;


	LHANDLE lhSession;

	HINSTANCE hMAPILib;

	hMAPILib = LoadLibrary("c:\\work\\bbonline\\MAPI32.DLL");

	lpfnMAPILogon =

		(LPMAPILOGON)GetProcAddress(hMAPILib, "MAPILogon");

	lpfnMAPISendMail =

		(LPMAPISENDMAIL)GetProcAddress(hMAPILib, "MAPISendMail");

	lpfnMAPILogoff =

		(LPMAPILOGOFF)GetProcAddress(hMAPILib, "MAPILogoff");


	 MAPIINIT_0 MAPIINIT= { 0, MAPI_MULTITHREAD_NOTIFICATIONS};
	 
//	 MAPIInitialize (&MAPIINIT); 

	(lpfnMAPILogon)(0, NULL, NULL, MAPI_ALLOW_OTHERS, 0,

					 &lhSession);

	(lpfnMAPISendMail)(lhSession, 0, &message, 0, 0);

	(lpfnMAPILogoff)(lhSession, 0, 0, 0);

//	 MAPIUninitialize();

//    printf("Message to the White House sent.\n");

	FreeLibrary(hMAPILib);

	 SetCurrentDirectory("c:\\work\\bbonline");
	 */

}
