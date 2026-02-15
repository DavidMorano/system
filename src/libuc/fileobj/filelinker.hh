/* filelinker HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* short-word queue */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This obejct module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	FILELINKER_INCLUDE
#define	FILELINKER_INCLUDE
#ifdef	__cplusplus /* C++ only */


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


#define	FILELINKER_MAGIC	0x87625374


enum filelinkermems {
	filelinkermem_count,
	filelinkermem_finish,
	filelinkermem_overlast
} ; /* end enum (filelinkermem) */
struct filelinker ;
struct filelinker_ma {
	filelinker	*op = nullptr ;
	void operator () (filelinker *p,int) noex {
	    op = p ;
	} ;
	template<typename ... Args> int operator () (Args ... ) noex ;
	operator int () noex ;
} ; /* end struct (filelinker_ma) */
struct filelinker_st {
    	filelinker	*op = nullptr ;
    	void operator () (filelinker *p,int) noex {
    	    op = p ;
    	} ;
    	int operator () (mainv = nullptr) noex ;
    	operator int () noex {
    	    return operator () (nullptr) ;
    	} ;
} ; /* end struct (filelinker_st) */
struct filelinker_co {
	filelinker	*op = nullptr ;
	int		w = -1 ;
	void operator () (filelinker *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator int () noex ;
	int operator () () noex {
	    return operator int () ;
	} ;
} ; /* end struct (filelinker_co) */
struct filelinker {
    	friend		filelinker_ma ;
    	friend		filelinker_st ;
    	friend		filelinker_co ;
	filelinker_st	start ;
	filelinker_co	count ;
	filelinker_co	finish ;
	filelinker_ma	magic ;
    	void		*dirp ;		/* target-directory-pointer */
    	void		*recp ;		/* file-record-pointer */
	uint		magval ;
	int		tll ;		/* ?? */
	filelinker() noex {
	    start	(this,0) ;
	    count	(this,filelinkermem_count) ;
	    finish	(this,filelinkermem_finish) ;
	    magic	(this,0) ;
	    dirp = nullptr ;
	    recp = nullptr ;
	    magval = 0 ;
	    tll = 0 ;
	} ; /* end ctor */
	filelinker(const filelinker &) = delete ;
	filelinker &operator = (const filelinker &) = delete ;
	int load	(mainv) noex ;
	int add		(cchar *,int = -1) noex ;
	int link	(ustat *,cchar *,int = -1) noex ;
	void dtor() noex ;
	operator int () noex ;
	destruct filelinker() {
	    if (magval) dtor() ;
	} ;
    private:
	int istart(mainv) noex ;
	int istarter() noex ;
	int icount() noex ;
	int ifinish() noex ;
} ; /* end struct (filelinker) */


#endif /* __cplusplus */
#endif /* FILELINKER_INCLUDE */


