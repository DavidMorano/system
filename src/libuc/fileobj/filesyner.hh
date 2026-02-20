/* filesyncer HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* short-word queue */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This obejct module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	FILESYNCER_INCLUDE
#define	FILESYNCER_INCLUDE
#ifdef	__cplusplus /* C++ only */


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


#define	FILESYNCER_MAGIC	0x87625374


enum filesyncermems {
	filesyncermem_count,
	filesyncermem_finish,
	filesyncermem_overlast
} ; /* end enum (filesyncermem) */
struct filesyncer ;
struct filesyncer_ma {
	filesyncer	*op = nullptr ;
	void operator () (filesyncer *p,int) noex {
	    op = p ;
	} ;
	template<typename ... Args> int operator () (Args ... ) noex ;
	operator int () noex ;
} ; /* end struct (filesyncer_ma) */
struct filesyncer_st {
    	filesyncer	*op = nullptr ;
    	void operator () (filesyncer *p,int) noex {
    	    op = p ;
    	} ;
    	int operator () (mainv = nullptr) noex ;
    	operator int () noex {
    	    return operator () (nullptr) ;
    	} ;
} ; /* end struct (filesyncer_st) */
struct filesyncer_co {
	filesyncer	*op = nullptr ;
	int		w = -1 ;
	void operator () (filesyncer *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex {
	    return operator int () ;
	} ;
} ; /* end struct (filesyncer_co) */
struct filesyncer {
    	friend		filesyncer_ma ;
    	friend		filesyncer_st ;
    	friend		filesyncer_co ;
	filesyncer_st	start ;
	filesyncer_co	count ;
	filesyncer_co	finish ;
	filesyncer_ma	magic ;
    	void		*dirp ;		/* target-directory-pointer */
    	void		*recp ;		/* file-record-pointer */
	char		*pbuf ;
	char		*dbuf ;
	uint		magval ;
	int		plen ;
	int		dlen ;
	int		tll ;		/* ?? */
	filesyncer() noex {
	    start	(this,0) ;
	    count	(this,filesyncermem_count) ;
	    finish	(this,filesyncermem_finish) ;
	    magic	(this,0) ;
	    dirp = nullptr ;
	    recp = nullptr ;
	    magval = 0 ;
	    tll = 0 ;
	    a = nullptr ;
	} ; /* end ctor */
	filesyncer(const filesyncer &) = delete ;
	filesyncer &operator = (const filesyncer &) = delete ;
	int load	(mainv) noex ;
	int add		(cchar *,int = -1) noex ;
	int link	(custat *,cchar *,int = -1) noex ;
	void dtor() noex ;
	operator int () noex ;
	destruct filesyncer() {
	    if (magval) dtor() ;
	} ;
	int ione(custat *,cc *,custat *,cc *,int) noex ;
    private:
	char *a ;
	int istart(mainv) noex ;
	int ifinish() noex ;
	int istarter() noex ;
	int icount() noex ;
	int idir(custat *,custat *,cchar *,int) noex ;
	int mkpdirs(cchar *,mode_t) noex ;
} ; /* end struct (filesyncer) */


#endif /* __cplusplus */
#endif /* FILESYNCER_INCLUDE */


