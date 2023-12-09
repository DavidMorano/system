/* uinfo INCLUDE */
/* lang=C20 */

/* UNIX® information (a cache for 'uname(2)' and sisters) */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-02-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#ifndef	UINFO_INCLUDE
#define	UINFO_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<utypedefs.h>
#include	<clanguage.h>


#define	UINFO_NAME	struct uinfo_infoname
#define	UINFO_AUX	struct uinfo_infoaux


struct uinfo_infoname {
	cchar		*sysname ;
	cchar		*nodename ;
	cchar		*release ;
	cchar		*version ;
	cchar		*machine ;
} ;

struct uinfo_infoaux {
	cchar		*architecture ;
	cchar		*platform ;
	cchar		*hwprovider ;
	cchar		*hwserial ;
	cchar		*nisdomain ;
} ;

typedef UINFO_NAME	uinfo_name ;
typedef UINFO_AUX	uinfo_aux ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int uinfo_init() noex ;
extern int uinfo_name(UINFO_NAME *) noex ;
extern int uinfo_aux(UINFO_AUX *) noex ;
extern int uinfo_fini() noex ;

#ifdef	__cplusplus
}
#endif


#endif /* UINFO_INCLUDE */


