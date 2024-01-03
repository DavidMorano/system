/* aflag HEADER */
/* lang=C++20 */

/* atomic-flag */
/* version %I% last-modified %G% */


/* revision history:

	= 2020-01-13, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 2020 David A­D­ Morano.  All rights reserved. */

#ifndef	AFLAG_INCLUDE
#define	AFLAG_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be first to configure */
#include	<atomic>
#include	<utypedefs.h>
#include	<clanguage.h>


enum aflagmems {
	aflagmem_set,
	aflagmem_clear,
	aflagmem_test,
	aflagmem_testandset,
	aflagmem_overlast
} ;
struct aflag ;
struct aflag_co {
	aflag		*op = nullptr ;
	int		w = -1 ;
	constexpr void operator () (aflag *p,int m) noex {
	    op = p ;
	    w = m ;
	} ;
	operator bool () noex ;
	bool operator () () noex {
	    return bool(*this) ;
	} ;
} ;
struct aflag {
	std::atomic_flag	af{} ;
	aflag_co	set ;
	aflag_co	clear ;
	aflag_co	test ;
	aflag_co	testandset ;
	constexpr aflag() noex {
	    set(this,aflagmem_set) ;
	    clear(this,aflagmem_clear) ;
	    test(this,aflagmem_test) ;
	    testandset(this,aflagmem_testandset) ;
	} ;
	operator bool () noex {
	    return af.test() ;
	} ;
	bool operator = (const bool f) noex {
	    bool	rf = false ;
	    if (f) {
		rf = af.test_and_set() ;
	    } else {
		rf = af.test() ;
	        af.clear() ;
	    }
	    return rf ;
	} ;
} ; /* end struct (aflag) */


#endif /* __cplusplus */
#endif /* AFLAG_INCLUDE */


