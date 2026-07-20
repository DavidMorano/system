/* getngroups HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* get the maximum number of supplemetary groups allowed per process */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETNGROUPS_INCLUDE
#define	GETNGROUPS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


enum grouptypes {
    	grouptype_sys,
    	grouptype_cache,
    	grouptype_overlast
} ; /* end enum */

EXTERNC_begin
    extern int getngroups()		noex ;
    extern int getngroupsx(grouptypes)	noex ;
EXTERNC_end

#ifdef	__cplusplus
inline int getngroups(grouptypes w) noex {
    	return getngroupsx(w) ;
} /* end subroutine */
#endif /* __cplusplus */


#endif /* GETNGROUPS_INCLUDE */


