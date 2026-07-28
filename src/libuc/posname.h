/* posname SUPPORT (POSIX® Entity name) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* create a POSIX® entify name */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	posname
	
	Description:
	This object module (POSNAME) creates a POSIX® entity name
	when needed.  It does this by possibly prepending a slash
	('/') character before the given name.

*******************************************************************************/

#ifndef	POSNAME_INCLUDE
#define	POSNAME_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


#define	POSNAME			struct posname_head
#define	POSNAME_SHORTLEN	16	/* "string length optimization" */


struct posname_head {
	cchar		*as ;	/* allocated string */
	char		buf[POSNAME_SHORTLEN+1] ;
} ; /* end struct (posname_head) */

#ifdef	__cplusplus
enum posnamemems {
	posnamemem_finish,
	posnamemem_overlast
} ; /* end enum */
struct posname ;
struct posname_co {
	posname		*op = nullptr ;
	int		w = -1 ;
	void operator () (posname *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (posname_co) */
struct posname : posname_head {
	posname_co	finish ;
	posname() noex {
	    finish(this,posnamemem_finish) ;
	} ;
	posname(const posname &) = delete ;
	posname &operator = (const posname &) = delete ;
	int start(cchar *sp,int sl,cchar **) noex ;
	void dtor() noex ;
	destruct posname() {
	    if (as) dtor() ;
	} ;
} ; /* end struct (posname) */
#else /* __cplusplus */
typedef POSNAME		posname ;
#endif /* __cplusplus */

EXTERNC_begin

extern int	posname_start(posname *,cchar *,int,cchar **) noex ;
extern int	posname_finish(posname *) noex ;

EXTERNC_end


#endif /* POSNAME_INCLUDE */


