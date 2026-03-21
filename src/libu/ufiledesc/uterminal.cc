/* uterminal SUPPORT (UNIX®-At-Xntthing) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* translation layer interface for UNIX® equivalents */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	u_terminal

	Description:
	Thes perform some access-permisson deterinations.

	Synopsis:
	int u_terminal(int nfds,fdset *ifds,fdset *ofds,fdset *efds) noex

	Arguments:
	nfds		maximum number of file-descriptors set
	ifds		file-descriptors input
	ofds		file-descriptors output
	efds		file-descriptors error

	Returns:
	>=0		number of descriptors with activity
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<localmisc.h>

#include	"ufiledescbase.hh"
#include	"uterminal.h"


/* local defines */


/* imported namespaces */

using libu::ufiledescbase ;		/* type */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct umgr ;
    typedef int (umgr::*umgr_m)(int) noex ;
    struct umgr : ufiledescbase {
	umgr_m		m ;
	TERMIOS		*tip ;
	int		cmd ;
	umgr(umgr_m µm) noex {
	    m = µm ;
	} ; /* end ctor */
	umgr(TERMIOS *p,int c = 0) noex : tip(p), cmd(c) { } ;
	umgr(const TERMIOS *p,int c = 0) noex : cmd(c) { 
	    	tip = cast_const<TERMIOS *>(p) ;
	} ;
	int callstd(int fd) noex override final {
	    int		rs = SR_BUGCHECK ;
	    if (m) {
		rs = (this->*m)(fd) ;
	    }
	    return rs ;
	} ; /* end method */
	sysret_t iterminal(int) noex ;
	sysret_t attrget(int) noex ;
	sysret_t attrset(int) noex ;
    } ; /* end struct (umgr::) */
} /* end namespace */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

namespace libu {
    int uterminal(int fd) noex {
	umgr fo(&umgr::iterminal) ;
	return fo(fd) ;
    } /* end subroutine (uterminal) */
    int utermattrget(int fd,TERMIOS *tip) noex {
	umgr fo(tip) ;
	fo.m = &umgr::attrget ;
	return fo(fd) ;
    } /* end subroutine (utermattrget) */
    int utermattrset(int fd,int cmd,const TERMIOS *tip) noex {
	umgr fo(tip,cmd) ;
	fo.m = &umgr::attrset ;
	return fo(fd) ;
    } /* end subroutine (utermattrset) */
} /* end namespace (libu) */

int u_terminal(int fd) noex {
    	using libu::uterminal ;
	return uterminal(fd) ;
} /* end subroutine (u_terminal) */

int u_termattrget(int fd,TERMIOS *tip) noex {
    	using libu::utermattrget ;
	int		rs = SR_FAULT ;
	if (tip) {
	    rs = utermattrget(fd,tip) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (u_termattrget) */

int u_termattrset(int fd,int cmd,const TERMIOS *tip) noex {
    	using libu::utermattrset ;
	int		rs = SR_FAULT ;
	if (tip) {
	    rs = utermattrset(fd,cmd,tip) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (u_termattrset) */


/* local subroutines */

sysret_t umgr::iterminal(int fd) noex {
    	int		rs ;
	if ((rs = isatty(fd)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
} /* end method (umgr::iterminal) */

sysret_t umgr::attrget(int fd) noex {
	int		rs ;
	if ((rs = tcgetattr(fd,tip)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
} /* end method (umgr::attrget) */

sysret_t umgr::attrset(int fd) noex {
    	const TERMIOS	*cp = cast_const<const TERMIOS *>(tip) ;
	int		rs ;
	if ((rs = tcsetattr(fd,cmd,cp)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
} /* end method (umgr::attrset) */

