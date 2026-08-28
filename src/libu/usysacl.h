/* usysacl HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* version %I% last-modified %G% */
/* virtual-system definitions */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************
 
	Header:
	usysacl

	Description:
	This extra header is required for correct compilation
	on Apple-Darwin operating systems.  On those systems,
	the |acl| header can only be include *after* including
	the |sys/types| header.  Why?  Because the very stupid
	Apple-Darwin developers (of which that are a great number)
	are stupid to realize that their header |acl| did not
	already include its own proper dependencies to compile
	properly.  Yes, my friends.  That is how very stupid the
	Apple-Darwin develoeper were and still are!

*******************************************************************************/

#ifndef	USYSACL_INCLUDE
#define	USYSACL_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/acl.h>		/* POSIX® */


#endif /* USYSACL_INCLUDE */


