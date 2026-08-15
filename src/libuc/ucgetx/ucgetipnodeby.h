/* ucgetipnode HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* auxillary operating system support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	uc_gethostbegin
	uc_gethostent
	uc_gethostend
	uc_gethostbyname
	uc_gethostbyaddr

	Description:
	These subroutines retrive a HOSTENT object from the system (name
	service).

*******************************************************************************/

#ifndef	UCGETIPNODEBY_INCLUDE
#define	UCGETIPNODEBY_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<netdb.h>		/* POSIX® */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int uc_getipnodebyname	(HOSTENT **,cchar *,int,int) noex ;
extern int uc_getipnodebyaddr	(HOSTENT **,cvoid *,int,int) noex ;
extern int uc_hostentfree	(HOSTENT *) noex ;

EXTERNC_end


#endif /* UCGETIPNODEBY_INCLUDE */


