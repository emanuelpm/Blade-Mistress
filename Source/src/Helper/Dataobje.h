/*******************************************************************
 *
 *	  DESCRIPTION: Class for holding containerable data,
 *				 : ie anything that gets put on a list
 *				 : or needs an id/name tag association
 *
 *	  AUTHOR:		Patrick E. Hughes
 *
 *	  HISTORY:		Incept: 2-27-95
 *
 *******************************************************************/

#ifndef DATAOBJECT_H
#define DATAOBJECT_H

#include <stdlib.h>
//#include "datatype.h"
#include <assert.h>


#define DO_NAME_LENGTH 256

class DataObject
{
	public:

		DataObject( unsigned long doid = 0, char *doname = NULL );
		virtual ~DataObject();

		unsigned long WhatAmI( void );
		char *WhoAmI( void );

		virtual int operator ==( DataObject &doref );
		virtual int operator !=( DataObject &doref );
		virtual int operator <( DataObject &doref );
		virtual int operator >( DataObject &doref );

		unsigned long do_id;
		char do_name[ DO_NAME_LENGTH ];
};

#endif // DATAOBJECT_H
