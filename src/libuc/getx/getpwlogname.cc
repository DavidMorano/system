/* getpwlogname SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* get the user PASSWD entry based on |LOGNAME| */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	getpwlogname

	Description:
	Get the PASSWD database structure for the logged in user.

	Synopsis:
	int getpwlogname(ucentpw *pwp,char *pwbuf,int pwlen) noex

	Arguments:
	pwp		|ucentpw| entry pointer
	pwbuf		PW-entry buffer pointer
	pwlen		PW-entry buffer length

	Returns:
	>=0		OK
	<0		error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucentpw.h>		/* LIBUC */
#include	<getax.h>		/* LIBUC */
#include	<getpwx.h>		/* LIBUC |getpwx_name(3uc)| */
#include	<getutmpent.h>		/* LIBUC |getutmpname(3uc)| */
#include	<bufsizevar.hh>		/* LIBUC <- currently unused */
#include	<localmisc.h>		/* LIBU */

#pragma		GCC dependency		"mod/uconstants.ccm"

import uconstants ;			/* |varname(3u)| */

/* local defines */

#define	PWBUF_MIN		40


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct tryer {
	ucentpw		*pwp ;
	char		*pwbuf ;
	uid_t		uid ;
	uid_t		sid ;
	int		pwlen ;
	tryer(ucentpw *p,char *b,int l) noex : pwp(p), pwbuf(b), pwlen(l) { 
	    uid = getuid() ;
	    sid = getsid(0) ;
	} ; /* end ctor */
	operator int () noex ;
	int try_logname() noex ;
	int try_utmp() noex ;
	int try_username() noex ;
	int try_user() noex ;
	int try_uid() noex ;
	int check(cchar *) noex ;
    } ; /* end struct (tryer) */
    typedef int (tryer::*tryer_m)() noex ;
} /* end namespace */


/* forward references */


/* local variables */

constexpr tryer_m	tries[] = {
    	&tryer::try_logname,
    	&tryer::try_utmp,
    	&tryer::try_username,
    	&tryer::try_user,
    	&tryer::try_uid
} ; /* end array (tries) */


/* exported variables */


/* exported subroutines */

int getpwlogname(ucentpw *pwp,char *pwbuf,int pwlen) noex {
    	int		rs = SR_FAULT ;
	int		pwl = 0 ; /* return-value */
	if (pwp && pwbuf) ylikely {
	    rs = SR_OVERFLOW ;
	    if (pwlen >= PWBUF_MIN) ylikely {
		if (tryer to(pwp,pwbuf,pwlen) ; (rs = to) >= 0) {
		    pwl = rs ;
		}
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? pwl : rs ;
} /* end subroutine (getpwlogname) */


/* local subroutines */

tryer::operator int () noex {
    	int		rs = SR_OK ;
	for (cauto &m : tries) {
	    rs = (this->*m)() ;
	    if (rs != 0) break ;
	} /* end for */
	return rs ;
} /* end method (tryer::operator) */

int tryer::try_logname() noex {
	static cchar  *vnp = getenver(varname.logname) ;
	return check(vnp) ;
} /* end method (tryer::try_logname) */

int tryer::try_username() noex {
	static cchar  *vnp = getenver(varname.username) ;
	return check(vnp) ;
} /* end method (tryer::try_username) */

int tryer::try_user() noex {
	static cchar  *vnp = getenver(varname.user) ;
	return check(vnp) ;
} /* end method (tryer::try_user) */

int tryer::try_utmp() noex {
    	int		rs ;
	int		rs1 ;
	int		pwl = 0 ; /* return-value */
	if (char *nbuf ; (rs = lm_un(&nbuf)) >= 0) ylikely {
	    cint nlen = rs ;
	    if ((rs = getutmpname(nbuf,nlen,sid)) >= 0) ylikely {
		rs = check(nbuf) ;
		pwl = rs ;
	    } /* end if (getutmpname) */
	    rs1 = lm_free(nbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? pwl : rs ;
} /* end method (tryer::try_ujtmp) */

int tryer::try_uid() noex {
	return getpwx_uid(pwp,pwbuf,pwlen,uid) ;
} /* end method (tryer::try_uid) */

int tryer::check(cchar *namep) noex {
    	int		rs = SR_OK ;
	int		pwl = 0 ; /* return-value */
	if (namep && namep[0]) ylikely {
	    if ((rs = getpwx_name(pwp,pwbuf,pwlen,namep)) >= 0) ylikely {
		pwl = rs ;
	        if (pwp->pw_uid != uid) {
	            pwl = 0 ;
		}
	    } /* end if (getpwx_name) */
	} /* end if (variable) */
	return (rs >= 0) ? pwl : rs ;
} /* end method (tryer::check) */


