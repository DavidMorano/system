/* quickselecti SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++98 */

/* this is a Qucik-Selection function */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* compile-time debugging */

/* revision history:

	= 1998-09-10, David A­D­ Morano
	Originally written elsewhere as part of some subroutine
	code modeule.

	= 2018-09-15, David A­D­ Morano
	Took from another file and made it a stand-alone (exported)
	subroutine.

*/

/* Copyright © 1998,2018 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	qckselecti

	Description:
	We perform a "quick-select" function on an array of integers.

	Synopsis:
	int quickselecti(int *a,int first,int last,int k)

	Arguments:
	a		array of integers to sort
	first		index of first element in range to sort
	last		index of over-last element of range to sort
	k		index of pivot element

	Returns:
	-		inconsequential at this time

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"quickselecti.h"


/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* compile-time debugging */
#endif


/* name-spaces */


/* type-defs */


/* external subroutines */

extern "C" {
    typedef int	(*partpred_t)(int,int) noex ;
} /* end extern (C) */

extern "C" {
    extern void	arrswapi(int *,int,int) noex ;
    extern int	partitionai(int *,int,partpred_t,int) noex ;
    extern int	nthai(int *,int,int,int) noex ;
} /* end extern (C) */


/* external variables */


/* local structures */


/* forward references */

local int	getpivot(cint *,int) noex ;
local int	partpred1(int,int) noex ;
local int	partpred2(int,int) noex ;

#if	CF_DEBUG
local int debugprinta(cint *a,int al) noex ;
#endif


/* local variables */

cbool		f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

int quickselecti(int *a,int first,int last,int k) noex {
	int		kv = a[k] ;
	if ((k >= first) && (k < last)) {
	    if ((last - first) == 2) {
	        if (a[first] > a[last - 1]) {
		    arrswapi(a,first,(last - 1)) ;
		}
	        kv = a[k] ;
	    } else if ((last-first) > 2) {
	        cint	pv = getpivot(a+first,(last-first)) ;
	        int	m1, m2 ;
	        m1 = partitionai(a+first,(last-first),partpred1,pv) + first ;
	        m2 = partitionai(a+m1,(last-m1),partpred2,pv) + m1 ;
	        if (k < m1) {
		    kv = quickselecti(a,first,m1,k) ;
	        } else if (k >= m2) {
		    kv = quickselecti(a,m2,last,k) ;
	        } else {
	    	    kv = a[k] ;
		}
	    }
	} /* end if (in range) */
	return kv ;
} /* end subroutine (quickselecti) */


/* local subroutines */

local int getpivot(cint *a,int al) noex {
	int	pvi = (al / 2) ;
	if (pvi == 0) {
	    if (al > 1) pvi = 1 ;
	}
	return a[pvi] ;
} /* end subroutine */

local int partpred1(int e,int pv) noex {
	return (e < pv) ;
} /* end subroutine */

local int partpred2(int e,int pv) noex {
	return (e <= pv) ;
} /* end subroutine */

#if	CF_DEBUG
local int debugprinta(cint *a,int al) noex {
	int		i ;
	for (i = 0 ; i < al ; i += 1) {
	    debugprintf(" %2u\\",a[i]) ;
	}
	debugprintf("\n") ;
	return 0 ;
} /* end subroutine (debugprinta) */
#endif /* CF_DEBUG */


