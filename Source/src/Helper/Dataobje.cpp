/*******************************************************************
 *
 *	  DESCRIPTION:	Class which is used as base data for all containers
 *
 *	  AUTHOR:		Patrick E. Hughes
 *
 *	  HISTORY:		Incept: 2-27-95
 *
 *******************************************************************/

#include <string.h>
#include "dataobje.h"

/***************************************************************
 ** DataObject::DataObject( unsigned long doid = 0, char *doname = NULL )
 *
 *	FILENAME: d:\source\vampire\dataobje.cpp
 *
 *	PARAMETERS: object id, and object name
 *
 *	DESCRIPTION: class initializer
 *
 *	RETURNS:
 *
 **************************************************************/
DataObject::DataObject( unsigned long doid, char *doname )
{
	do_id = doid;
	memset( do_name, 0, DO_NAME_LENGTH );

	if( doname )
	{
		int i = strlen( doname ) +1;

		if( i < DO_NAME_LENGTH )
		{
			strcpy( do_name, doname );
		}
		else
		{
			strncpy( do_name, doname, DO_NAME_LENGTH -1 );
		}	
	}
}

/***************************************************************
 ** DataObject::~DataObject()
 *
 *	FILENAME: d:\source\vampire\dataobje.cpp
 *
 *	PARAMETERS:
 *
 *	DESCRIPTION: class destructor
 *			   : deletes the name char * if non-NULL
 *	RETURNS:
 *
 **************************************************************/
DataObject::~DataObject()
{
}

/***************************************************************
 ** unsigned long DataObject::WhatAmI( void )
 *
 *	FILENAME: d:\source\vampire\dataobje.cpp
 *
 *	PARAMETERS:
 *
 *	DESCRIPTION: reports this object's id number
 *
 *	RETURNS:
 *
 **************************************************************/
unsigned long DataObject::WhatAmI( void )
{
	return( do_id );
}

/***************************************************************
 ** char * DataObject::WhoAmI( void )
 *
 *	FILENAME: d:\source\vampire\dataobje.cpp
 *
 *	PARAMETERS:
 *
 *	DESCRIPTION: reports this object's name
 *
 *	RETURNS:
 *
 **************************************************************/
char *DataObject::WhoAmI( void )
{
	return( do_name );
}

/***************************************************************
 ** unsigned long DataObject::operator ==, !=, <, >
 *
 *	FILENAME: d:\source\vampire\dataobje.cpp
 *
 *	PARAMETERS:
 *
 *	DESCRIPTION: commonplace operator replacments
 *
 *	RETURNS:
 *
 **************************************************************/
int DataObject::operator ==( DataObject &mdref )
{
	if( do_id == mdref.do_id )
	{
		if( do_name[ 0 ] && mdref.do_name[ 0 ] )
		{
			if( !strcmp( do_name, mdref.do_name ) )
			{
				return( 1 );
			}
		}	
	}
	return( 0 );  
}

int DataObject::operator !=( DataObject &mdref )
{
	return( !( *this == mdref ) );
}

int DataObject::operator <( DataObject &mdref )
{
	return( 0 );
}

int DataObject::operator >( DataObject &mdref )
{
	return( 0 );
}
