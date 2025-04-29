/* denpercents HEADER */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* various probability-density interfaces */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	ENPERCENTS_INCLUDE
#define	ENPERCENTS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int denpercentsi(uint *,int,double *) noex ;
extern int denpercentsl(ulong *,int,double *) noex ;

static inline int denpercentsi(ulong *a,int n,double *pp) noex {
	return denpercentsl(a,n,pp) ;
}

EXTERNC_end


#endif /* ENPERCENTS_INCLUDE */


