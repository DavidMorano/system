/* nonpathfn HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* assert a NUL-terminated string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	NONPATHFN_INCLUDE
#define	NONPATHFN_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	NONPATHFN		struct nonpathfn_head
#define	NONPATHFN_SHORTLEN	128		/* "string length optimization" */


struct nonpathfn_head {
	cchar		*as ;	/* allocated string */
	char		buf[NONPATHFN_SHORTLEN+1] ;
} ;

#ifdef	__cplusplus
enum nonpathfnmems {
	nonpathfnmem_finish,
	nonpathfnmem_overlast
} ;
struct nonpathfn ;
struct nonpathfn_co {
	nonpathfn		*op = nullptr ;
	int		w = -1 ;
	void operator () (nonpathfn *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (nonpathfn_co) */
struct nonpathfn : nonpathfn_head {
	nonpathfn_co	finish ;
	nonpathfn() noex {
	    finish(this,nonpathfnmem_finish) ;
	} ;
	nonpathfn(const nonpathfn &) = delete ;
	nonpathfn &operator = (const nonpathfn &) = delete ;
	int start(cchar *sp,int sl,cchar **) noex ;
	void dtor() noex ;
	destruct nonpathfn() {
	    if (as) dtor() ;
	} ;
} ; /* end struct (nonpathfn) */
#else /* __cplusplus */
typedef NONPATHFN		nonpathfn ;
#endif /* __cplusplus */

EXTERNC_begin

extern int	nonpathfn_start(nonpathfn *,cchar *,int,cchar **) noex ;
extern int	nonpathfn_finish(nonpathfn *) noex ;

EXTERNC_end


#endif /* NONPATHFN_INCLUDE */


