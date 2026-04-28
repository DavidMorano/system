/* usys_getnetname SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* UNIX® system emulated support */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A-D- Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2001 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	Important note:

	On Darwin (MacOS) the header 'sys/random.h' -- which is
	supposed to contain the declaration for the subroutine
	|getentropy(2)| --  is broken when using GCC (G++) as the
	compiler.  It figures.  So I declared the |getentropy(2)|
	subroutine myself below.  This cr*p gets very old after a
	while.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cerrno>
#include	<cstddef>
#include	<cstdlib>
#include	<cstring>		/* |srcpy(3c)| */
#include	<new>			/* |nothrow(c++)| */
#include	<string>		/* |string(c++)| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysflag.h>
#include	<localmisc.h>		/* |DECBUFLEN| */

#include	"usys_ugetnisdom.h"
#include	"usys_onc.hh"		/* Open-Network-Computing */
#include	"usys_getnetname.h"

using std::string ;			/* rtype */
using std::to_string ;			/* subroutine */
using usys::getnetnamelen ;		/* subroutine */
using libu::ugetnisdom ;		/* subroutine */
using std::nothrow ;			/* constant */

/* GETRANDOM start */
#if	(!defined(SYSHAS_ONC)) || (SYSHAS_ONC == 0)


local int mknetname(char *,int) noex ;

cint		dlen = DECBUFLEN ;

/* 0=not-found, 1=found */
int getnetname(char *rbuf) noex {
	int		rc = 0 ; /* default is "not-available" */
	if (rbuf) {
	    static cint rlen = getnetnamelen() ;
	    int	rs ;
	    if ((rs = rlen) >= 0) {
		if ((rs = mknetname(rbuf,rlen)) >= 0) {
		    rc = +1 ;
		}
	    } /* end if */
	    errno = (rs >= 0) ? 0 : (neg rs) ;
	} else {
	    errno = EFAULT ;
	} /* end if (non-null) */
	return rc ;
} /* end subroutine (getnetname) */

local int mknetname(char *rbuf,int rlen) noex {
    	const pid_t	pid = getpid() ;
    	int		rs = SR_OK ;
	int		rl = 0 ; /* return-value */
	try {
    	    string	str ;
	    uint uv = uint(pid) ;
	    str += "unix." ;
	    str += to_string(uv) ;
	    str += char('@') ;
	    rs = SR_NOMEM ;
	    if (char *dbuf = new(nothrow) char[dlen + 1] ; dbuf) {
		if ((rs = ugetnisdom(dbuf,dlen)) >= 0) {
	            str += dbuf ;
		}
		delete [] dbuf ;
	    } /* end if (m-a-f) */
	    if (rs >= 0) {
		cchar *sp	= str.c_str() ;
		cint sl		= int(str.size()) ;
		if (sl <= rlen) {
		    strcpy(rbuf,sp) ;
		    rl = sl ;
		} else {
		    rs = SR_OVERFLOW ;
		}
	    } /* end if (ok) */
	} catch (...) {
	    rs = SR_NOMEM ;
	}
	return (rs >= 0) ? rl : rs ;
} /* end subroutine (mknetname) */

#endif /* (!defined(SYSHAS_ONC)) || (SYSHAS_ONC == 0) */
/* GETRANDOM end */


