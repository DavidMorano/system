/* ucaddrinfo HEADER */
/* charset=ISO8859-1 */
/* lang=C20 (conformance reviewed) */

/* interface component for UNIX® library-3c */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#ifndef	UCADDRINFO_INCLUDE
#define	UCADDRINFO_INCLUDE

#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


EXTERNC_begin

extern int uc_addrinfoget(cchar *,cchar *,const ADDRINFO *,ADDRINFO **) noex ;
extern int uc_addrinfofree(ADDRINFO *) noex ;

EXTERNC_end


#endif /* UCADDRINFO_INCLUDE */


