/* bvchapters HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */


/* revision history:

	= 2008-10-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	BVCHAPTERS_INCLUDE
#define	BVCHAPTERS_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


struct bvchapters ;
enum bvchaptersmems {
	bvchaptersmem_finish,
	bvchaptersmem_overlast
} ; /* end enum (bvchaptersmems) */
struct bvchapters_co {
	bvchapters	*op = nullptr ;
	int		w = -1 ;
	void operator () (bvchapters *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (bvchapters_co) */
struct bvchapters {
    	friend		bvchapters_co ;
    	bvchapters_co	finish ;
	uchar		*ap ;
	int		al ;
	int		chapidx ;
	bvchapters() noex {
	    finish	(this,bvchaptersmem_finish) ;
	    ap = nullptr ;
	    al = 0 ;
	} ; /* end ctor */
	int start	(uchar *,int) noex ;
	int load	(int,int) noex ;
    private:
	int ifinish	() noex ;
} ; /* end struct (chapters) */


#endif /* BVCHAPTERS_INCLUDE */


