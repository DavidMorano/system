/* nprintf HEADER (libdebug) */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* debug utilities */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	NPRINTF_INCLUDE
#define	NPRINTF_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<netdb.h>		/* |HOSTENT| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int nprint		(cchar *,cchar *,int) noex ;
extern int nprintf		(cchar *,cchar *,...) noex ;
extern int nprinthexblock	(cchar *,cchar *,int,cvoid *,int) noex ;

EXTERNC_end


#endif /* NPRINTF_INCLUDE */


