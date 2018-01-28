#ifndef MESSAGESYSTEM_H
#define MESSAGESYSTEM_H

#include "dataobje.h"
#include "linklist.h"


extern int messageSurfaceListLength;
extern ThomBM *messageSurfaceList;  // alloc an array of ThomBMs for this.



//***************************************************************
class Message : public DataObject
{
public:

   Message( char *text, int faceIndex, int r, int g, int b, int doid = 0, char *doname = NULL); 
   virtual ~Message();

   UINT flags;
   int r,g,b;
   int faceIndex;
   int timeLeft;
   char text[2048];

};


//***************************************************************
class MessageManager
{
public:

   MessageManager(void); 
   virtual ~MessageManager();

	virtual void Draw(void);
	void AddMessage(Message *m);

   DoublyLinkedList messageList;
};



#endif
