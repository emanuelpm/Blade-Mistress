/*******************************************************************
 *
 *	  DESCRIPTION:	Classes for handling linked lists and varieties descended.
 *
 *	  AUTHOR:		Patrick E. Hughes
 *
 *	  HISTORY:		Incept: 2-27-95
 *
 *******************************************************************/

#ifndef LINKLIST_H
#define LINKLIST_H

#include <stdlib.h>
#include <assert.h>
#include "dataobje.h"

/*
IF THEY'D FIX PRIVATE STRUCTURES WITHIN A CLASS ACROSS ALL COMPILERS
I WOULDN'T HAVE TO KEEP THE NODE/NODEPTR STRUCTURE
VISIBLE OUTSIDE LIKE THIS.
*/

typedef struct node_struc
{
	struct	node_struc	*prev;
	struct	node_struc	*next;
	DataObject *data;

	node_struc() { prev = next = this; data = 0; }
} NODE, *NODEPTR;


class DoublyLinkedList
{
	public:

		DoublyLinkedList();
		virtual ~DoublyLinkedList();

		DataObject *First( void );
		DataObject *Last( void );
		DataObject *Next( void );
		DataObject *Prev( void );
		DataObject *Curr( void );

		DataObject *AddAfter( DataObject *incoming, DataObject *target = NULL );
		DataObject *AddBefore( DataObject *incoming, DataObject *target = NULL );

		DataObject *Append( DataObject *incoming );
		DataObject *Prepend( DataObject *incoming );

		DataObject *Find( DataObject *target );
		DataObject *Find( unsigned long by_id );
		DataObject *Find( char *by_name );
		DataObject *Remove( DataObject *target = NULL );

		int ItemsInList( void );
		int IsListEmpty( void );

	protected:

		NODEPTR _head;
		NODEPTR _current;
		int _v;
};

class Queue : public DoublyLinkedList
{
	public:

		Queue();
		virtual ~Queue();

		DataObject *Push( DataObject *incoming );
		DataObject *Pop( void );
};

class Stack : public DoublyLinkedList
{
	public:

		Stack();
		virtual ~Stack();

		DataObject *Push( DataObject *incoming );
		DataObject *Pop( void );
};

class CircularList : public DoublyLinkedList
{
	public:

		CircularList();
		virtual ~CircularList();

		DataObject *Next( void );
		DataObject *Prev( void );
};

		
#endif // LINKLIST_H
