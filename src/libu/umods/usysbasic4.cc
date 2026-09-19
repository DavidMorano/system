/* usysbasic4 MODULE (implementation) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get the Present-Working-Directory (PWD) of the process */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	ugetpwd

	Description:
	This subroutine gets and returns, into a user supplied
	buffer, the present working directory (PWD) maintained by
	UNIX®-based systems.  The current working directory of the
	SHELL (maintained in the environment variable 'PWD') is
	returned if it is the same directory as that maintained by
	UNIX®.

	Synopsis:
	int ugetpwd(char *pwbuf,int pwlen) noex

	Arguments:
	pwbuf		- result buffer pointer
	pwlen		- result buffer length

	Returns:
	>=0		length of returned string in user buffer
	<0		error (system-return)


*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

module usysbasic ;

/* local defines */

#ifndef	VARPWD
#define	VARPWD		"PWD"
#endif


/* imported namespaces */


/* local typenames */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct getter ;
    typedef int (getter::*getter_m)() noex ;
    struct getter {
	ustat	*sbp ;
	char	*pwbuf ;
	int	pwlen ;
	getter(ustat *s,char *b,int l) noex : sbp(s), pwbuf(b), pwlen(l) { } ;
	int tryenv() noex ;
	int trycwd() noex ;
	operator int () noex ;
    } ; /* end struct (getter) */
} /* end namespace */


/* forward references */


/* local variables */

constexpr getter_m	mgets[] = {
    	&getter::tryenv,
    	&getter::trycwd
} ; /* end array (gets) */


/* exported variables */


/* exported subroutines */

int ugetpwd(char *pwbuf,int pwlen) noex {
	return ugetpwds(pwbuf,pwlen) ;
} /* end subroutine (getpwd) */

int ugetpwds(char *pwbuf,int pwlen,ustat *sbp) noex {
	int		rs = SR_FAULT ;
	if (pwbuf) {
	    rs = SR_INVALID ;
	    pwbuf[0] = '\0' ;
	    if (pwlen > 0) {
	        getter go(sbp,pwbuf,pwlen) ;
	        rs = go ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (getpwds) */


/* local subroutines */

getter::operator int () noex {
    	int	rs = SR_OK ;
	for (cauto &e : mgets) {
	    rs = (this->*e)() ;
	    if (rs) break ;
	} /* end for */
	return rs ;
} /* end method (getter::operator) */

int getter::tryenv() noex {
	static cchar	*pwd = getenv(VARPWD) ;
    	int		rs = SR_OK ;
	int		pl = 0 ; /* return-value */
        if (pwd) ylikely {
            ustat       *ssbp, sb1, sb2 ;
            if ((rs = ustatfile(pwd,&sb1)) >= 0) {
                ssbp = (sbp) ? sbp : &sb2 ;
                if ((rs = ustatfile(".",ssbp)) >= 0) {
                    bool        f = true ;
                    f = f && (sb1.st_dev == ssbp->st_dev) ;
                    f = f && (sb1.st_ino == ssbp->st_ino) ;
                    if (f) {
                        rs = sncpy(pwbuf,pwlen,pwd) ;
                        pl = rs ;
                    }
                } else if (isNotPresent(rs)) {
                    rs = SR_OK ;
                } /* end if (stat) */
            } else if (isNotPresent(rs)) {
                rs = SR_OK ;
            } /* end if (stat) */
        } /* end if (quickie) */
	return (rs >= 0) ? pl : rs ;
} /* end method (getter::tryenv) */

int getter::trycwd() noex {
    	int		rs ;
	int		pl = 0 ; /* return-value */
	if ((rs = ugetcwd(pwbuf,pwlen)) >= 0) ylikely {
	    pl = rs ;
	    if (sbp) {
		rs = ustatfile(pwbuf,sbp) ;
	    }
	} /* end if (getcwd) */
	return (rs >= 0) ? pl : rs ;
} /* end method (getter::trycwd) */


