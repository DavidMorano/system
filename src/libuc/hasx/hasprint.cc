/* hasprint SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* does a counted c-string has some characteristic? */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-10-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

	= 2026-03-04, David A­D­ Morano
	I changed the object-struct below ('printinfo´) from
	initalizing at load-time to initializing at first use.  I
	atempted to have it initialized at compile time (using C++11
	'constexpr') but the subroutine |iscmdstart(3uc)| is not a
	constant-expression evaluated subroutine.  Sigh.  Yes, I
	could have circumvented the use of that subroutine.  So
	instead of settling for load-time initialization, I changed
	it to first-use initialization.  After thoughts: yes, after
	all of these years (2026 - 1998), this code has seen some
	change.  Funny?  I do not know.

*/

/* Copyright © 1998,2026 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	hasprint{x}

	Namess:
	hasprintcmd
	hasprintbad

	Description:
	These subroutines check if a specified c-string has any of
	some characteristic we are looking for.

	Synopsis:
	bool hasprintcmd(cchar *sp,int sl) noex
	bool hasprintbad(cchar *sp,int sl) noex

	Arguments:
	sp		test c-string pointer
	sl		test c-string length

	Returns:
	true		true (empty)
	false		false (not empty)

	Notes:
	1. I allow multiple initializations due to multi-thread
	races (to initialize), because it is harmless.  But the
	use of an atomic flag (AFLAG) limits the amount of possible
	multiple initializations to a small window (the time it
	takes to initialize).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |UCHAR_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<bitset>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<aflag.hh>
#include	<ascii.h>
#include	<mkchar.h>
#include	<ischarx.h>		/* |isprintbad(3uc)| */
#include	<localmisc.h>

#include	"hasprint.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |getlenstr(3u)| + |lenstr(3u)| */

/* local defines */


/* imported namespaces */

using std::bitset ;			/* type */


/* local typedefs */

extern "C" {
    typedef bool (*isx_f)(int) noex ;
}


/* external subroutines */


/* external variables */


/* local structures */

constexpr int   nch = (UCHAR_MAX + 1) ;

namespace {
    struct printinfo ;
    struct printinfo_co {
	printinfo	*op = nullptr ;
	void operator () (printinfo *p) noex {
	    op = p ;
	} ; /* end method */
	operator bool () noex ;
    } ; /* end struct (printinfo_co) */
    struct printinfo {
	friend		printinfo_co ;
	bitset<nch>	iscmd ;
	printinfo_co	init ;
	printinfo() noex {
	    init	(this) ;
	} ;
    private:
	aflag		finit ;
	bool create() noex ;
	void mkiscmd() noex ;
    } ; /* end struct (printinfo) */
} /* end namespace */

bool printinfo::create() noex {
    	if (! finit) {
    	    mkiscmd() ;
	    finit = true ;
	} /* end if (initialization needed) */
	return finit ;
} /* end method (printinfo::create) */

void printinfo::mkiscmd() noex {
    	for (int ch = 0 ; ch < nch ; ch += 1) {
	    if (iscmdstart(ch)) {
		iscmd.set(ch,true) ;
	    } else {
		bool	f = false ;
	        f = f || (ch == CH_ESC) ;
	        f = f || (ch == CH_TAB) ;
	        f = f || (ch == CH_CR) ;
	        f = f || (ch == CH_NL) ;
	        f = f || (ch == CH_BS) ;
	        f = f || (ch == CH_BEL) ;
	        f = f || (ch == CH_VT) || (ch == CH_FF) ;
	        f = f || (ch == CH_SO) || (ch == CH_SI) ;
	        f = f || (ch == CH_SS2) || (ch == CH_SS3) ;
		if (f) {
		    iscmd.set(ch,true) ;
		}
	    } /* end if */
	} /* end for */
} /* end method (printinfo::mkiscmd) */


/* forward references */

local bool isprintcmd(int) noex ;

local bool hasx(isx_f,cchar *,int) noex ;


/* local variables */

static printinfo	print_data ;


/* exported variables */


/* exported subroutines */

bool hasprintcmd(cchar *sp,int sl) noex {
    	bool f = false ;
    	if (print_data.init) {
	    f = hasx(isprintcmd,sp,sl) ;
	}
	return f ;
}
/* end subroutine (hasprintcmd) */

bool hasprintbad(cchar *sp,int sl) noex {
	return hasx(isprintbad,sp,sl) ;
}
/* end subroutine (hasprintbad) */


/* local subroutines */

local bool isprintcmd(int ch) noex {
    	return print_data.iscmd[ch & UCHAR_MAX] ;
} /* end subroutine (isprintcmd) */

local bool hasx(isx_f isx,cchar *sp,int sl) noex {
	bool		f = false ;
	if (sp) ylikely {
	    while (sl && *sp) {
	        cint	ch = mkchar(*sp) ;
	        f = isx(ch) ;
	        if (f) break ;
	        sp += 1 ;
	        sl -= 1 ;
	    } /* end while */
	} /* end if (non-null) */
	return f ;
} /* end subroutine (hasx) */

printinfo_co::operator bool () noex {
    	bool f = false ;
    	if (op) {
 	    f = op->create() ;
	} /* end if (non-null) */
 	return f ;
} /* end method (printinfo_co::operator) */


