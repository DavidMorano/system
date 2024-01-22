/* sysdbfname HEADER */
/* lang=C20 */

/* standard byte-order serializations */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-03-24, David A­D­ Morano
	This code is newly written but modeled after some code that
	was originally written for the Ethernet-Controller (ETHCON)
	development work.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

#ifndef	SYSDBFNAME_INCLUDE
#define	SYSDBFNAME_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>

#include	<sysdbfnames.h>


EXTERNC_begin

extern int sysdbfnameget(sysdbfiles,cchar *,cchar **) noex ;

EXTERNC_end


#endif /* SYSDBFNAME_INCLUDE */


