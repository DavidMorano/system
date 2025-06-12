/* tmz HEADER */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* time and timezone parsing */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	TMZ_INCLUDE
#define	TMZ_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* first to configure */
#include	<time.h>		/* |TM| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	TMZ		tmz
#define	TMZ_FLAGS	tmz_flags


struct tmz_flags {
	uint		zoff:1 ;	/* zone offset is present */
	uint		year:1 ;	/* year is present */
} ;

enum tmzmems {
	tmzmem_clear,
	tmzmem_init,
	tmzmem_isset,
	tmzmem_hasyear,
	tmzmem_haszoff,
	tmzmem_haszone,
	tmzmem_getdst,
	tmzmem_getzoff,
	tmzmem_overlast
} ; /* end enum (tmzmems) */

struct tmz ;

struct tmz_co {
	tmz		*op = nullptr ;
	int		w = -1 ;
	void operator () (tmz *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (tmz_co) */

struct tmz {
	char		*zname{} ;	/* dynamically allocated */
	tmz_co		clear ;
	tmz_co		init ;
	tmz_co		isset ;
	tmz_co		hasyear ;
	tmz_co		haszoff ;
	tmz_co		haszone ;
	tmz_co		getdst ;
	tmz_co		getzoff ;
	TM		st ;
	TMZ_FLAGS	fl ;
	short		zoff ;		/* minutes west of GMT */
	tmz() noex {
	    clear(this,tmzmem_clear) ;
	    init(this,tmzmem_init) ;
	    isset(this,tmzmem_isset) ;
	    hasyear(this,tmzmem_hasyear) ;
	    haszoff(this,tmzmem_haszoff) ;
	    haszone(this,tmzmem_haszone) ;
	    getdst(this,tmzmem_getdst) ;
	    getzoff(this,tmzmem_getzoff) ;
	} ;
	int	xstd(cchar *,int) noex ;
	int	xmsg(cchar *,int) noex ;
	int	xtouch(cchar *,int) noex ;
	int	xtoucht(cchar *,int) noex ;
	int	xstrdig(cchar *,int) noex ;
	int	xlogz(cchar *,int) noex ;
	int	xday(cchar *,int) noex ;
	int	setday(int,int,int) noex ;
	int	setyear(int) noex ;
	int	setzone(cchar *,int) noex ;
	int	gettm(TM *) noex ;
	void dtor() noex ;
	destruct tmz() {
	    if (zname) dtor() ;
	} ;
} ; /* end struct (tmz) */

EXTERNC_begin

extern int	tmz_init(tmz *) noex ;
extern int	tmz_xstd(tmz *,cchar *,int) noex ;
extern int	tmz_xmsg(tmz *,cchar *,int) noex ;
extern int	tmz_xtouch(tmz *,cchar *,int) noex ;
extern int	tmz_xtoucht(tmz *,cchar *,int) noex ;
extern int	tmz_xstrdig(tmz *,cchar *,int) noex ;
extern int	tmz_xlogz(tmz *,cchar *,int) noex ;
extern int	tmz_xday(tmz *,cchar *,int) noex ;
extern int	tmz_isset(tmz *) noex ;
extern int	tmz_hasyear(tmz *) noex ;
extern int	tmz_haszoff(tmz *) noex ;
extern int	tmz_haszone(tmz *) noex ;
extern int	tmz_setday(tmz *,int,int,int) noex ;
extern int	tmz_setyear(tmz *,int) noex ;
extern int	tmz_setzone(tmz *,cchar *,int) noex ;
extern int	tmz_gettm(tmz *,TM *) noex ;
extern int	tmz_getdst(tmz *) noex ;
extern int	tmz_getzoff(tmz *) noex ;

EXTERNC_end


#endif /* __cplusplus */
#endif /* TMZ_INCLUDE */


