/* prefixfn HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* assert a NUL-terminated string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	PREFIXFN_INCLUDE
#define	PREFIXFN_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	PREFIXFN		struct prefixfn_head
#define	PREFIXFN_SHORTLEN	128	/* "string length optimization" */


struct prefixfn_head {
	cchar		*as ;	/* allocated string */
	char		buf[PREFIXFN_SHORTLEN+1] ;
} ;

#ifdef	__cplusplus
enum prefixfnmems {
	prefixfnmem_finish,
	prefixfnmem_overlast
} ;
struct prefixfn ;
struct prefixfn_co {
	prefixfn	*op = nullptr ;
	int		w = -1 ;
	void operator () (prefixfn *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (prefixfn_co) */
struct prefixfn : prefixfn_head {
	prefixfn_co	finish ;
	prefixfn() noex {
	    finish(this,prefixfnmem_finish) ;
	} ;
	prefixfn(const prefixfn &) = delete ;
	prefixfn &operator = (const prefixfn &) = delete ;
	int start(cchar *sp,int sl,cchar **) noex ;
	void dtor() noex ;
	destruct prefixfn() {
	    if (as) dtor() ;
	} ;
} ; /* end struct (prefixfn) */
#else /* __cplusplus */
typedef PREFIXFN		prefixfn ;
#endif /* __cplusplus */

EXTERNC_begin

extern int	prefixfn_start(prefixfn *,cchar *,int,cchar **) noex ;
extern int	prefixfn_finish(prefixfn *) noex ;

EXTERNC_end


#endif /* PREFIXFN_INCLUDE */


