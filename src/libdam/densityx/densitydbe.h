/* densitydbe HEADER */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* density DB entry */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#ifndef	DENSITYDBE_INCLUDE
#define	DENSITYDBE_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	DENSITYDBE		struct densitydbe_head

struct densitydbe_head {
	uint		count ;		/* count */
	uint		utime ;		/* update time */
} ;

#ifdef	__cplusplus
enum densitydbemems {
    	densitydbemem_start,
	densitydbemem_entsz,
	densitydbemem_finish,
	densitydbemem_overlast
} ;
struct densitydbe ;
struct densitydbe_co {
	densitydbe	*op = nullptr ;
	int		w = -1 ;
	void operator () (densitydbe *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (densitydbe_co) */
struct densitydbe : densitydbe_head {
	densitydbe_co	start ;
	densitydbe_co	entsz ;
	densitydbe_co	finish ;
	densitydbe() noex {
	    start(this,densitydbemem_start) ;
	    entsz(this,densitydbemem_entsz) ;
	    finish(this,densitydbemem_finish) ;
	} ;
	densitydbe(const densitydbe &) = delete ;
	densitydbe &operator = (const densitydbe &) = delete ;
	int rd(char *,int) noex ;
	int wr(cchar *,int) noex ;
	operator int () noex ;
	void dtor() noex ;
	~densitydbe() {
	    dtor() ;
	} ;
} ; /* end struct (densitydbe) */
#else	/* __cplusplus */
typedef	DENSITYDBE	densitydbe ;
#endif /* __cplusplus */


EXTERNC_begin

extern int densitydbe_start(densitydbe *) noex ;
extern int densitydbe_finish(densitydbe *) noex ;
extern int densitydbe_rd(densitydbe *,char *,int) noex ;
extern int densitydbe_wr(densitydbe *,cchar *,int) noex ;
extern int densitydbe_entsz(densitydbe *) noex ;

EXTERNC_end


#endif /* DENSITYDBE_INCLUDE */


