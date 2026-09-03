/* utimeout SUPPORT */
/* charset=ISO8859-1 */
/* lang=C20 (MUST be C-lang) */

/* manage UNIX® kernal-call (and std-c library) timeouts */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was born out of frustration with cleaning up bad
	legacy code (of which there is quite a bit -- like almost
	all of it).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Data:
	utimeout

	Description:
	This module (in part) manages UNIX® system kernel-call timeouts.

	Synopsis:
	int u_timeout(int w,int val) noex

	Arguments:
	w		which timeout to set
	val		timeout value to be set

	Returns:
	>=0		ok
	<0		error (system-return)

	Note:
	1. This module MUST be coded in the C-language!  We need
	C-language features that are NOT in (brain-damaged) C++.
	Also, this module (translaton unit) MUST be loaded (by the
	run-time dynamic linker) into memory before the 'utimeouts'
	(note the 's' on the end) module so that proper run-time
	initialization of the timeout values can occur.  As noted
	elsewhere, neither the C language nor the C++ language offer
	the necessary capabilies to create the type of initialization
	environment I really want.  So the current arrangement using
	both C and C++ is a total hack!  More information (notes)
	on this whole "timeout" facility is located in the source
	language file 'utimeoutdefs.h'.
	2. Use C++ you say? Well, C++ does NOT have the type of
	initialization feature that is used below in this C-language
	source file (the file you are reading right now).  So C++
	can suck cock eggs for all the good it does.  As an aside,
	earlier C++ implementation DID allow for the type of
	initialization used below, but it was a secretly added
	feature that was not standard.  The C++ international ISO
	standards committee eventually cracked down on this fringe
	radical non-standard feature of C++ and the compiler people
	eventually followed suite and are now disallowing this.

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysrets.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<utimeoutdefs.h>

module modsys ;

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct timemgr {
	int		n[uto_overlast] ;
	consteval void mktab() noex {
	    n[uto_again]	= UTO_AGAIN ;
	    n[uto_intr]		= UTO_INTR ;
	    n[uto_nomem]	= UTO_NOMEM ;
	    n[uto_nosr]		= UTO_NOSR ;
	    n[uto_nobufs]	= UTO_NOBUFS ;
	    n[uto_busy]		= UTO_BUSY ;
	    n[uto_get]		= UTO_GET ;
	    n[uto_deadlock]	= UTO_DEADLOCK ;
	    n[uto_io]		= UTO_IO ;
	    n[uto_dquot]	= UTO_DQUOT ;
	    n[uto_nospc]	= UTO_NOSPC ;
	    n[uto_dial]		= UTO_DIAL ;
	    n[uto_open]		= UTO_OPEN ;
	    n[uto_read]		= UTO_READ ;
	    n[uto_mfile]	= UTO_MFILE ;
	    n[uto_nfile]	= UTO_NFILE ;
	    n[uto_poll]		= UTO_POLL ;
	    n[uto_nolck]	= UTO_NOLCK ;
	    n[uto_capture]	= UTO_NOLCK ;
	    n[uto_txtbusy]	= UTO_TXTBUSY ;
	    n[uto_closewait]	= UTO_CLOSEWAIT ;
	    n[uto_inprogress]	= UTO_INPROGRESS ;
	} ; /* end method (mktab) */
	consteval timemgr() noex {
	    mktab() ;
	} ; /* end ctor */
    } ; /* end struct (timemgr) */
    struct initer {
	initer() noex ;
    } ; /* end struct (initer) */
} /* end namespace */


/* forward references */


/* local variables */

constinit timemgr	timeout_data ;
static initer		init_data ;


/* exported variables */

intp			utimeout ;


/* exported subroutines */

int ms_timeout(int w,int val) noex {
	int		rs = SR_INVALID ;
	if ((w >= 0) && (w < uto_overlast) && (val != 0)) {
	    rs = timeout_data.n[w] ;
	    if (val > 0) {
		timeout_data.n[w] = val ;
	    }
	} /* end if (valid) */
	return rs ;
} /* end subroutine (ms_timeout) */


/* local subroutines */

initer::initer() noex {
	utimeout = timeout_data.n ;
} /* end ctor (initer::initer) */


