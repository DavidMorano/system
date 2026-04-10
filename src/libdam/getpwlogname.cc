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
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<uclibmem.h>
#include	<ucentpw.h>
#include	<getax.h>
#include	<getpwx.h>		/* |getpwx_name(3uc)| */
#include	<getutmpent.h>		/* |getutmpname(3uc)| */
#include	<bufsizevar.hh>		/* <- currently unused */
#include	<localmisc.h>

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
	if (pwp && pwbuf) {
	    rs = SR_OVERFLOW ;
	    if (pwlen >= PWBUF_MIN) {
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
	static cchar  *vnp = getenv(varname.logname) ;
	return check(vnp) ;
} /* end method (tryer::try_logname) */

int tryer::try_username() noex {
	static cchar  *vnp = getenv(varname.username) ;
	return check(vnp) ;
} /* end method (tryer::try_username) */

int tryer::try_user() noex {
	static cchar  *vnp = getenv(varname.user) ;
	return check(vnp) ;
} /* end method (tryer::try_user) */

int tryer::try_utmp() noex {
    	int		rs ;
	int		rs1 ;
	int		pwl = 0 ; /* return-value */
	if (char *nbuf ; (rs = lm_un(&nbuf)) >= 0) {
	    cint nlen = rs ;
	    if ((rs = getutmpname(nbuf,nlen,sid)) >= 0) {
		rs = check(nbuf) ;
		pwl = rs ;
	    } /* end if (getpwx_uid) */
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
	if (namep && namep[0]) {
	    if ((rs = getpwx_name(pwp,pwbuf,pwlen,namep)) >= 0) {
		pwl = rs ;
	        if (pwp->pw_uid != uid) {
	            pwl = 0 ;
		}
	    } /* end if (getpwx_name) */
	} /* end if (variable) */
	return (rs >= 0) ? pwl : rs ;
} /* end method (tryer::check) */


