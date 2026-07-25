/* ucinfo HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* UNIX® information (a cache for 'uname(2)' and sisters) */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#ifndef	UCINFO_INCLUDE
#define	UCINFO_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#define	UCINFO_NAMES	struct ucinfo_infoname
#define	UCINFO_AUXS	struct ucinfo_infoaux


struct ucinfo_infoname {
	cchar		*sysname ;
	cchar		*nodename ;
	cchar		*release ;
	cchar		*version ;
	cchar		*machine ;
} ; /* end struct (ucinfo_infoname) */

struct ucinfo_infoaux {
	cchar		*architecture ;
	cchar		*platform ;
	cchar		*hwprovider ;
	cchar		*hwserial ;
	cchar		*nisdomain ;
} ; /* end struct (ucinfo_infoaux) */

typedef UCINFO_NAMES	ucinfo_names ;
typedef UCINFO_AUXS	ucinfo_auxs ;

EXTERNC_begin

extern int ucinfo_init() noex ;
extern int ucinfo_name(ucinfo_names *) noex ;
extern int ucinfo_aux(ucinfo_auxs *) noex ;
extern int ucinfo_fini() noex ;

EXTERNC_end


#endif /* UCINFO_INCLUDE */


