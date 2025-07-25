/* msgide HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* message identification (MSG-ID) entry */
/* version %I% last-modified %G% */


/* revision history:

	= 2003-03-04, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2003 David A­D­ Morano.  All rights reserved. */

#ifndef	MAGIDE_INCLUDE
#define	MAGIDE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


enum msgidemems {
    	msgidemem_start,
	msgidemem_entsz,
	msgidemem_finish,
	msgidemem_overlast
} ;

struct msgide ;

template<typename T> struct msgide_co {
	T		*op = nullptr ;
	int		w = -1 ;
	void operator () (T *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex { 
	    return operator int () ;
	} ;
} ; /* end struct (msgide_co) */

struct msgide {
	friend		msgide_co<msgide> ;
	struct msgide_len {
	    int		entsz ;
	    int		recipient ;
	    int		messageid ;
	    int		from ;
	} ;
	char		*a{} ;
	char		*recipient{} ;
	char		*messageid{} ;
	char		*from{} ;
	uint		count{} ;	/* count */
	uint		utime{} ;	/* time-stamp update */
	uint		ctime{} ;	/* time-stamp creation */
	uint		mtime{} ;	/* time-stamp message */
	uint		hash ;
	msgide_len	len ;
	msgide_co<msgide>		start ;
	msgide_co<msgide>		entsz ;
	msgide_co<msgide>		finish ;
	msgide() noex {
	    start(this,msgidemem_start) ;
	    entsz(this,msgidemem_entsz) ;
	    finish(this,msgidemem_finish) ;
	} ;
	msgide(const msgide &) = delete ;
	msgide &operator = (const msgide &) = delete ;
	int rd(char *,int = -1) noex ;
	int rdu(char *,int = -1) noex ;
	int wr(cchar *,int = -1) noex ;
	int wru(cchar *,int = -1) noex ;
	int loadrec(cchar *,int = -1) noex ;
	int loadmid(cchar *,int = -1) noex ;
	destruct msgide() {
	    if (a) finish() ;
	} ;
    private:
	int istart() noex ;
	int ifinish() noex ;
} ; /* end struct (msgide) */


#endif /* MAGIDE_INCLUDE */


