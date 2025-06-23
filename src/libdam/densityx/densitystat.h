/* densitystat HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* various probability-density interfaces */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	DENSITYSTAT_INCLUDE
#define	DENSITYSTAT_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int densitystati(uint *,int,double *,double *) noex ;
extern int densitystatl(ulong *,int,double *,double *) noex ;

static inline int densitystat(ulong *a,int n,double *mp,double *vp) noex {
	return densitystatl(a,n,mp,vp) ;
}

EXTERNC_end


#endif /* DENSITYSTAT_INCLUDE */


