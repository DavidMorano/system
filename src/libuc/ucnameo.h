/* ucnameo HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* UNIX® information (a cache for |uname(2)| and sisters) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UCNAMEO_INCLUDE
#define	UCNAMEO_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#define	UCNAMEO		struct ucnameo_head


struct ucnameo_head {
	cchar		*sysname ;
	cchar		*nodename ;
	cchar		*release ;
	cchar		*version ;
	cchar		*machine ;
	char		*a ;		/* the memory-allocation */
} ; /* end struct */

typedef UCNAMEO		ucnameo ;

EXTERNC_begin

extern int ucnameo_start	(ucnameo *) noex ;
extern int ucnameo_finish	(ucnameo *) noex ;

EXTERNC_end


#endif /* UCNAMEO_INCLUDE */


