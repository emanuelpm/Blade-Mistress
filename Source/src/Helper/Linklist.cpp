/*******************************************************************
 *
 *	  DESCRIPTION:	Classes for handling linked lists and varieties descended
 *
 *	  AUTHOR:		Patrick E. Hughes
 *
 *	  HISTORY:		Incept: 2-27-95
 *
 *******************************************************************/

#include <assert.h>
#include <string.h>
#include "linklist.h"

/*		                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           
 ** DoublyLinkedList::DoublyLinkedList()
 *
 *	FILENAME: d:\source\vampire\linklist.cpp
 *
 *	PARAMETERS:
 *
 *	DESCRIPTION: class initializer
 *
 *	RETURNS: _v (internal variable) == 0 on failure
 *
 */
//==============================================================
DoublyLinkedList::DoublyLinkedList()
{
	_head = (NODEPTR)new node_struc;
	if( !_head )
	{
		_v = 0;
		_current = NULL;
	}
	else
	{
		_v = 1;
		_current = _head;
	}
}

/*
 ** DoublyLinkedList::~DoublyLinkedList()
 *
 *	FILENAME: d:\source\vampire\linklist.cpp
 *
 *	PARAMETERS:
 *
 *	DESCRIPTION: class destructor
 *
 *	RETURNS:
 *
 */
//--------------------------------------------------------------
DoublyLinkedList::~DoublyLinkedList()
{
	if( _v )
	{
		NODEPTR nptr, nptr2;

		nptr = _head->next;

		while( nptr->data )
		{
			nptr2 = nptr->next;
			delete nptr->data;
			delete nptr;
			nptr = nptr2;
		}

		if( _head )
			delete _head;
	}
}

/*
 ** DataObject *DoublyLinkedList::First( void )
 *
 *	FILENAME: d:\source\vampire\linklist.cpp
 *
 *	PARAMETERS:
 *
 *	DESCRIPTION: set this list to the first member
 *
 *	RETURNS: ptr to the first member's data
 *		   : NULL if no members in list
 */
//--------------------------------------------------------------
DataObject *DoublyLinkedList::First( void )
{
	if( _v )
	{
		_current = _head->next;
		return( _current->data );
	}
	else
	{
		return( NULL );
	}	
}

/*
 ** DataObject *DoublyLinkedList::Last( void )
 *
 *	FILENAME: d:\source\vampire\linklist.cpp
 *
 *	PARAMETERS:
 *
 *	DESCRIPTION: set this list to the last member
 *
 *	RETURNS: ptr to the last member's data
 *		   : NULL if no members in list
 */
//--------------------------------------------------------------
DataObject *DoublyLinkedList::Last( void )
{
	if( _v )
	{
		_current = _head->prev;
		return( _current->data );
	}
	else
	{
		return( NULL );
	}	
}

/*
 ** DataObject *DoublyLinkedList::Next( void )
 *
 *	FILENAME: d:\source\vampire\linklist.cpp
 *
 *	PARAMETERS:
 *
 *	DESCRIPTION: advance this list to the next member
 *
 *	RETURNS: ptr to the next member's data
 *		   : NULL if no members in list, or past end of list
 */
//--------------------------------------------------------------
DataObject *DoublyLinkedList::Next( void )
{
	if( _v )
	{
		_current = _current->next;
		return( _current->data );
	}
	else
	{
		return( NULL );
	}	
}

/*
 ** DataObject *DoublyLinkedList::Prev( void )
 *
 *	FILENAME: d:\source\vampire\linklist.cpp
 *
 *	PARAMETERS:
 *
 *	DESCRIPTION: recede this list to the previous member
 *
 *	RETURNS: ptr to the previous member's data
 *		   : NULL if no members in list, or past beginning of list
 */
//--------------------------------------------------------------
DataObject *DoublyLinkedList::Prev( void )
{
	if( _v )
	{
		_current = _current->prev;
		return( _current->data );
	}
	else
	{
		return( NULL );
	}	
}

/*
 ** DataObject *DoublyLinkedList::Curr( void )
 *
 *	FILENAME: d:\source\vampire\linklist.cpp
 *
 *	PARAMETERS:
 *
 *	DESCRIPTION:
 *
 *	RETURNS: ptr to the current member's data
 *		   : NULL if no members in list
 */
//--------------------------------------------------------------
DataObject *DoublyLinkedList::Curr( void )
{
	if( _v )
	{
		return( _current->data );
	}
	else
	{
		return( NULL );
	}	
}

/*
 ** DataObject *DoublyLinkedList::Append( DataObject *incoming )
 *
 *	FILENAME: d:\source\vampire\linklist.cpp
 *
 *	PARAMETERS: list member ptr
 *
 *	DESCRIPTION: add a new member at the end of the list
 *
 *	RETURNS: ptr to the new member's data
 *		   : NULL if invalid list, or can't add
 */
//--------------------------------------------------------------
DataObject *DoublyLinkedList::Append( DataObject *incoming )
{
	assert( incoming );

	if( _v && incoming )
	{
		NODEPTR prev_ndp;
		NODEPTR store = _current;

		Last();
		prev_ndp = _current;

		NODEPTR ndp = (NODEPTR)new node_struc;
		if( ndp )
		{
			_current = ndp;
			ndp->data = incoming;

			ndp->next = prev_ndp->next;
			ndp->prev = prev_ndp;
			prev_ndp->next = ndp;
			(ndp->next)->prev = ndp;

			_current = store;

			return( incoming );
		}

		_current = store;

		return( NULL );
	}
	else
	{
		return( NULL );
	}	
}

/*
 ** DataObject *DoublyLinkedList::Prepend( DataObject *incoming )
 *
 *	FILENAME: d:\source\vampire\linklist.cpp
 *
 *	PARAMETERS: list member ptr
 *
 *	DESCRIPTION: add a new member to the beginning of the list
 *
 *	RETURNS: ptr to the new member's data
 *		   : NULL if invalid list, or can't add
 */
//--------------------------------------------------------------
DataObject *DoublyLinkedList::Prepend( DataObject *incoming )
{
	assert( incoming );

	if( _v && incoming )
	{
		NODEPTR prev_ndp;
		NODEPTR store = _current;

		prev_ndp = _head;

		NODEPTR ndp = (NODEPTR)new node_struc;
		if( ndp )
		{
			_current = ndp;
			ndp->data = incoming;

			ndp->next = prev_ndp->next;
			ndp->prev = prev_ndp;
			prev_ndp->next = ndp;
			(ndp->next)->prev = ndp;

			_current = store;

			return( incoming );
		}

		_current = store;

		return( NULL );
	}
	else
	{
		return( NULL );
	}	
}

/*
 ** DataObject *DoublyLinkedList::AddAfter( DataObject *incoming, DataObject *target = NULL )
 *
 *	FILENAME: d:\source\vampire\linklist.cpp
 *
 *	PARAMETERS: list member ptr to add, target member ptr to add after
 *			  : if no target is specified, then use the current member
 *
 *	DESCRIPTION: add a new member just past the target member's ptr
 *
 *	RETURNS: ptr to the new member's data
 *		   : NULL if invalid list, or can't add
 */
//--------------------------------------------------------------
DataObject *DoublyLinkedList::AddAfter( DataObject *incoming, DataObject *target )
{
	assert( incoming );

	if( _v && incoming )
	{
		NODEPTR store = _current;

		if( target )
		{
			Find( target );
		}

		NODEPTR prev_ndp;

		prev_ndp = _current;

		NODEPTR ndp = (NODEPTR)new node_struc;
		if( ndp )
		{
			_current = ndp;
			ndp->data = incoming;

			ndp->next = prev_ndp->next;
			ndp->prev = prev_ndp;
			prev_ndp->next = ndp;
			(ndp->next)->prev = ndp;

			_current = store;

			return( incoming );
		}

		_current = store;

		return( 0 );
	}
	else
	{
		return( NULL );
	}	
}


/*
 ** DataObject *DoublyLinkedList::AddBefore( DataObject *incoming, DataObject *target = NULL )
 *
 *	FILENAME: d:\source\vampire\linklist.cpp
 *
 *	PARAMETERS: list member ptr to add, target member ptr to add after
 *			  : if no target is specified, then use the current member
 *
 *	DESCRIPTION: add a new member just in front of the target member's ptr
 *
 *	RETURNS: ptr to the new member's data
 *		   : NULL if invalid list, or can't add
 */
//--------------------------------------------------------------
DataObject *DoublyLinkedList::AddBefore( DataObject *incoming, DataObject *target )
{
	assert( incoming );

	if( _v && incoming )
	{
		NODEPTR store = _current;

		if( target )
		{
			Find( target );
		}

		NODEPTR prev_ndp;

		prev_ndp = _current->prev;

		NODEPTR ndp = (NODEPTR)new node_struc;
		if( ndp )
		{
			_current = ndp;
			ndp->data = incoming;

			ndp->next = prev_ndp->next;
			ndp->prev = prev_ndp;
			prev_ndp->next = ndp;
			(ndp->next)->prev = ndp;

			_current = store;

			return( incoming );
		}

		_current = store;

		return( 0 );
	}
	else
	{
		return( NULL );
	}	
}

/*
 ** DataObject *DoublyLinkedList::Find( DataObject *target )
 *
 *	FILENAME: d:\source\vampire\linklist.cpp
 *
 *	PARAMETERS: list member ptr to compare against
 *
 *	DESCRIPTION: get me this member
 *
 *	RETURNS: ptr to the new member's data
 *		   : NULL if invalid list, or can't add
 */
//--------------------------------------------------------------
DataObject *DoublyLinkedList::Find( DataObject *target )
{
	assert( target );

	if( _v && target )
	{
		NODEPTR nptr;

		nptr = _head->next;
		while( nptr->data )
		{
			if( nptr->data == target )
			{
				_current = nptr;
				return( _current->data );
			}
			nptr = nptr->next;
		}
		_current = _head;
		return( 0 );
	}
	else
	{
		return( NULL );
	}	
}

/*
 ** DataObject *DoublyLinkedList::Find( int by_id )
 *
 *	FILENAME: d:\source\vampire\linklist.cpp
 *
 *	PARAMETERS: do_id to compare against
 *
 *	DESCRIPTION: get me this member
 *
 *	RETURNS: ptr to the new member's data
 *		   : NULL if invalid list, or can't add
 */
//--------------------------------------------------------------
DataObject *DoublyLinkedList::Find( unsigned long by_id )
{
	if( _v )
	{
		NODEPTR nptr;

		nptr = _head->next;
		while( nptr->data )
		{
			if( nptr->data->do_id == by_id )
			{
				_current = nptr;
				return( _current->data );
			}
			nptr = nptr->next;
		}
		_current = _head;
		return( 0 );
	}
	else
	{
		return( NULL );
	}	
}

/*
 ** DataObject *DoublyLinkedList::Find( int by_id )
 *
 *	FILENAME: d:\source\vampire\linklist.cpp
 *
 *	PARAMETERS: do_id to compare against
 *
 *	DESCRIPTION: get me this member
 *
 *	RETURNS: ptr to the new member's data
 *		   : NULL if invalid list, or can't add
 */
//--------------------------------------------------------------
DataObject *DoublyLinkedList::Find( char *by_name )
{
	if( _v )
	{
		NODEPTR nptr;

		nptr = _head->next;
		while( nptr->data )
		{
			if( !strcmp( nptr->data->do_name, by_name ) )
			{
				_current = nptr;
				return( _current->data );
			}
			nptr = nptr->next;
		}
		_current = _head;
		return( 0 );
	}
	else
	{
		return( NULL );
	}	
}

/*
 ** DataObject *DoublyLinkedList::Remove( DataObject *target = NULL )
 *
 *	FILENAME: d:\source\vampire\linklist.cpp
 *
 *	PARAMETERS: list member ptr to remove from the list
 *			  : if no target is specified, then the current member is removed
 *
 *	DESCRIPTION: delete this member from the list
 *
 *	RETURNS: ptr to the target member's data
 *		   : NULL if invalid list, or can't add
 */
//--------------------------------------------------------------
DataObject *DoublyLinkedList::Remove( DataObject *target )
{
	if( _v )
	{
		if( target )
		{
			Find( target );
		}
		if( !_current->data )
			return( 0 );

		NODEPTR tmp = _current->prev;

		(_current->prev)->next = _current->next;
		(_current->next)->prev = _current->prev;

		delete _current;

		_current = tmp;
		return( _current->data );
	}
	else
	{
		return( NULL );
	}	
}

/*
 ** int DoublyLinkedList::ItemsInList( void )
 *
 *	FILENAME: d:\source\vampire\linklist.cpp
 *
 *	PARAMETERS:
 *
 *	DESCRIPTION: how many items in this list
 *
 *	RETURNS: integer count of how many items in list
 *
 */
//--------------------------------------------------------------
int DoublyLinkedList::ItemsInList( void )
{
	if( _v )
	{
		NODEPTR nptr;
		int count = 0;
		NODEPTR store = _current;

		nptr = _head->next;
		while( nptr->data )
		{
			count++;
			nptr = nptr->next;
		}

		_current = store;

		return( count );
	}
	else
	{
		return( 0 );
	}	
}

/*
 ** int DoublyLinkedList::IsListEmpty( void )
 *
 *	FILENAME: d:\source\vampire\linklist.cpp
 *
 *	PARAMETERS:
 *
 *	DESCRIPTION: quick check, are there any members in this list?
 *
 *	RETURNS: !0 if there are NO MEMBERS
 *		   : 0 if there ARE MEMBERS
 */
//--------------------------------------------------------------
int DoublyLinkedList::IsListEmpty( void )
{
	if( _v )
	{
		if( (_head->next)->data )
			return( 0 );
		else
			return( 1 );	
	}
	return( 1 );	
}	

/*
 ** Queue::Queue()
 *
 *	FILENAME: d:\source\vampire\linklist.cpp
 *
 *	PARAMETERS:
 *
 *	DESCRIPTION: class initializer
 *
 *	RETURNS:
 *
 */
//==============================================================
Queue::Queue()
	: DoublyLinkedList()
{
}

/*
 ** Queue::~Queue()
 *
 *	FILENAME: d:\source\vampire\linklist.cpp
 *
 *	PARAMETERS:
 *
 *	DESCRIPTION: class destructor
 *
 *	RETURNS:
 *
 */
//--------------------------------------------------------------
Queue::~Queue()
{
}

/*
 ** DataObject *Queue::Push( DataObject *incoming )
 *
 *	FILENAME: d:\source\vampire\linklist.cpp
 *
 *	PARAMETERS: list member ptr
 *
 *	DESCRIPTION: add this new member in the queue's sequence
 *
 *	RETURNS: as in Append()
 *
 */
//--------------------------------------------------------------
DataObject *Queue::Push( DataObject *target )
{
	assert( target );

	return( Append( target ) );
}

/*
 ** DataObject *Queue::Pop( void )
 *
 *	FILENAME: d:\source\vampire\linklist.cpp
 *
 *	PARAMETERS: list member ptr
 *
 *	DESCRIPTION: get the next member in the queue's sequence
 *
 *	RETURNS: list member ptr
 *		   : NULL on empty list
 */
//--------------------------------------------------------------
DataObject *Queue::Pop( void )
{
	DataObject *o = First();
	if( o )
	{
		Remove();
	}

	return( o );
}

/*
 ** Stack::~Stack()
 *
 *	FILENAME: d:\source\vampire\linklist.cpp
 *
 *	PARAMETERS:
 *
 *	DESCRIPTION: class initializer
 *
 *	RETURNS:
 *
 */
//==============================================================
Stack::Stack() : DoublyLinkedList()
{
}

/*
 ** Stack::~Stack()
 *
 *	FILENAME: d:\source\vampire\linklist.cpp
 *
 *	PARAMETERS:
 *
 *	DESCRIPTION: class destructor
 *
 *	RETURNS:
 *
 */
//--------------------------------------------------------------
Stack::~Stack()
{
}

/*
 ** DataObject *Stack::Push( DataObject *incoming )
 *
 *	FILENAME: d:\source\vampire\linklist.cpp
 *
 *	PARAMETERS: list member ptr
 *
 *	DESCRIPTION: add this new member in the stack's sequence
 *
 *	RETURNS:
 *
 */
//--------------------------------------------------------------
DataObject *Stack::Push( DataObject *incoming )
{
	assert( incoming );

	return( Append( incoming ) );
}

/*
 ** DataObject *Stack::Pop( void )
 *
 *	FILENAME: d:\source\vampire\linklist.cpp
 *
 *	PARAMETERS: list member ptr
 *
 *	DESCRIPTION: get the next member in the stack's sequence
 *
 *	RETURNS: list member ptr
 *		   : NULL on empty list
 */
//--------------------------------------------------------------
DataObject *Stack::Pop( void )
{
	DataObject *o = Last();
	if( o )
	{
		Remove();
	}

	return( o );
}


/*
 ** CircularList::CircularList()
 *
 *	FILENAME: d:\source\vampire\linklist.cpp
 *
 *	PARAMETERS:
 *
 *	DESCRIPTION: class initializer
 *
 *	RETURNS:
 *
 */
//==============================================================
CircularList::CircularList()
	: DoublyLinkedList()
{
}

/*
 ** CircularList::CircularList()
 *
 *	FILENAME: d:\source\vampire\linklist.cpp
 *
 *	PARAMETERS:
 *
 *	DESCRIPTION: class destructor
 *
 *	RETURNS:
 *
 */
//--------------------------------------------------------------
CircularList::~CircularList()
{
}

/*
 ** DataObject *CircularList::Next( void )
 *
 *	FILENAME: d:\source\vampire\linklist.cpp
 *
 *	PARAMETERS:
 *
 *	DESCRIPTION: advance this list to the next member
 *			   : with wrap-around at the end of the list
 *	RETURNS: ptr to the next member's data
 *		   : NULL if no members in list
 */
//--------------------------------------------------------------
DataObject *CircularList::Next( void )
{
	if( _v )
	{
		if( _current->next == _head )
			return( First() );
		else
			return( DoublyLinkedList::Next() );
	}	
	else
	{
		return( NULL );
	}	
}

/*
 ** DataObject *CircularList::Prev( void )
 *
 *	FILENAME: d:\source\vampire\linklist.cpp
 *
 *	PARAMETERS:
 *
 *	DESCRIPTION: recede this list to the previous member
 *			   : with wrap-around at the beginning of the list
 *	RETURNS: ptr to the next member's data
 *		   : NULL if no members in list
 */
//--------------------------------------------------------------
DataObject *CircularList::Prev( void )
{
	if( _v )
	{
		if( _current->prev == _head )
			return( Last() );
		else
			return( DoublyLinkedList::Prev() );
	}	
	else
	{
		return( NULL );
	}	
}
