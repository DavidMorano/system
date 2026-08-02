/* envmgr HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* Environment Manager */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-01-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	ENVMGR_INCLUDE
#define	ENVMGR_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU |ulogerror(3u)| */
#include	<vecstr.h>		/* LIBUC */
#include	<vechand.h>		/* LIBUC */


#define	ENVMGR		struct envmgr_head
#define	ENVMGR_MAGIC	0x58261222


struct envmgr_head {
	vechand		*listp ;
	vecstr		*strp ;
	mainv		envv ;
} ; /* end struct (envmgr_head) */

typedef ENVMGR		envmgr ;

EXTERNC_begin

extern int	envmgr_start	(envmgr *) noex ;
extern int	envmgr_set	(envmgr *,cchar *,cchar *,int) noex ;
extern int	envmgr_getvec	(envmgr *,cchar ***) noex ;
extern int	envmgr_finish	(envmgr *) noex ;

EXTERNC_end


#endif /* ENVMGR_INCLUDE */


