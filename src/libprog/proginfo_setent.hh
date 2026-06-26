/* proginfo_setent HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* log handling for Time-Zones stastics */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	PROGINFOSETENT_INCLUDE
#define	PROGINFOSETENT_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<sysbase.h>		/* LIBU */

#include	<proginfo.hh>


EXTERNC_begin

extern int	proginfo_setentry(proginfo *,cchar **,cchar *,int) noex ;

EXTERNC_end


#endif /* PROGINFOSETENT_INCLUDE */


