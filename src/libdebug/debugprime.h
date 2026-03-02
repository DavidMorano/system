/* debugprime HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* debug utilities */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	DEBUGPRIME_INCLUDE
#define	DEBUGPRIME_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<netdb.h>		/* |HOSTENT| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int d_openfiles() noex ;
extern int d_ispath(cchar *) noex ;
extern void d_whoopen(int *) noex ;
extern char *d_reventstr(int,char *,int) noex ;
extern int heaudit(HOSTENT *,cchar *,int) noex ;
extern char *stroflags(char *,int) noex ;

EXTERNC_end


#endif /* DEBUGPRIME_INCLUDE */


