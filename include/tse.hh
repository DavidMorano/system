/* tse HEADER (Time-Stamp Entry) */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* machine status entry */
/* version %I% last-modified %G% */


/* Copyright © 2003 David A­D­ Morano.  All rights reserved. */

#ifndef	TSE_INCLUDE
#define	TSE_INCLUDE
#ifdef	__cplusplus /* C++ only */


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#define	TSE		struct tse
#define	TSE_LKEYNAME	32


enum tsemems {
    	tsemem_start,
	tsemem_finish,
	tsemem_overlast
} ;

struct tse ;

struct tse_co {
	tse		*op = nullptr ;
	int		w = -1 ;
	void operator () (tse *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (tse_co) */

struct tse {
    	friend		tse_co ;
	uint		count ;		/* count */
	uint		utime ;		/* update time-stamp */
	uint		ctime ;		/* creation time-stamp */
	uint		hash ;
	int		entsz{} ;
	int		keylen = TSE_LKEYNAME ;
	tse_co		start ;
	tse_co		finish ;
	char		keyname[TSE_LKEYNAME+ 1] ;
	tse() noex {
	    start(this,tsemem_start) ;
	    finish(this,tsemem_finish) ;
	} ;
	int rd(char *,int) noex ;
	int wr(cchar *,int) noex ;
	int rdu(char *,int) noex ;
	int wru(cchar *,int) noex ;
	int keyload(cchar *,int) noex ;
	operator int () noex ;
	void dtor() noex ;
	~tse() {
	    dtor() ;
	} ;
    private:
	int all(bool,char *,int) noex ;
	int update(bool,char *,int) noex ;
	int istart() noex ;
	int ifinish() noex ;
} ;

typedef	TSE		tse ;


#endif /* __cplusplus */
#endif /* TSE_INCLUDE */


