/* envhelp INCLUDE */
/* lang=C20 */

/* help w/ handling environment */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-10, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	ENVHELP_INCLUDE
#define	ENVHELP_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<usystem.h>
#include	<vechand.h>
#include	<strpack.h>


#define	ENVHELP		struct envhelp_head


struct envhelp_head {
	vechand		env ;
	strpack		stores ;
} ;

typedef ENVHELP		envhelp ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int envhelp_start(ENVHELP *,cchar **,cchar **) noex ;
extern int envhelp_present(ENVHELP *,cchar *,int,cchar **) noex ;
extern int envhelp_envset(ENVHELP *,cchar *,cchar *,int) noex ;
extern int envhelp_setexecs(ENVHELP *,cchar *,cchar *) noex ;
extern int envhelp_sort(ENVHELP *) noex ;
extern int envhelp_getvec(ENVHELP *,cchar ***) noex ;
extern int envhelp_finish(ENVHELP *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* ENVHELP_INCLUDE */


