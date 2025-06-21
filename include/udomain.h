/* udomain HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* retrieve a domain-name for a given username */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A�D� Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	UDOMAIN_INCLUDE
#define	UDOMAIN_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int udomain(cchar *,char *,int,cchar *) noex ;

EXTERNC_end


#endif /* UDOMAIN_INCLUDE */


