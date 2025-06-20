/* namestr HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* manipulate real names */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	NAMESTR_INCLUDE
#define	NAMESTR_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	NAMESTR			struct namestr_head


struct namestr_head {
	cchar		*strp ;
	int		strl ;
} ;

#ifdef	__cplusplus
enum namestrmems {
	namestrmem_skipwhite,
	namestrmem_finish,
	namestrmem_overlast
} ;
struct namestr ;
struct namestr_co {
	namestr		*op = nullptr ;
	int		w = -1 ;
	void operator () (namestr *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (namestr_co) */
struct namestr : namestr_head {
	namestr_co	skipwhite ;
	namestr_co	finish ;
	namestr() noex {
	    skipwhite	(this,namestrmem_skipwhite) ;
	    finish	(this,namestrmem_finish) ;
	    strp = nullptr ;
	} ;
	namestr(const namestr &) = delete ;
	namestr &operator = (const namestr &) = delete ;
	int start(cchar *,int) noex ;
	int next(cchar **,int *,int *) noex ;
	int brk(cchar *,cchar **) noex ;
	void dtor() noex ;
	destruct namestr() {
	    if (strp) dtor() ;
	} ;
} ; /* end struct (namestr) */
#else	/* __cplusplus */
typedef NAMESTR		namestr ;
#endif /* __cplusplus */

EXTERNC_begin

extern int	namestr_start(namestr *,cchar *,int) noex ;
extern int	namestr_next(namestr *,cchar **,int *,int *) noex ;
extern int	namestr_skipwhite(namestr *) noex ;
extern int	namestr_brk(namestr *,cchar *,cchar **) noex ;
extern int	namestr_finish(namestr *) noex ;

EXTERNC_end


#endif /* NAMESTR_INCLUDE */


