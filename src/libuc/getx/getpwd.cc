/* getpwd SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get the Present-Working-Directory (PWD) of the process */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

	= 2023-10-27, David A-D- Morano
	1. I added the code to dynamically determine the MAXPATHLEN
	value. 
	2. I removed an unnecessary note about stupid BSD-like
	systems (like Linux and Apple-Darwin, but we will not name
	names here) not having bidirectional UNIX® pipes.

*/

/* Copyright © 1998,2023 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getpwd

	Description:
	This subroutine gets and returns, into a user supplied
	buffer, the present working directory (PWD) maintained by
	UNIX®-based systems.  The current working directory of the
	SHELL (maintained in the environment variable 'PWD') is
	returned if it is the same directory as that maintained by
	UNIX®.

	Synopsis:
	int getpwd(char *pwbuf,int pwlen) noex

	Arguments:
	pwbuf	- buffer to hold resultant path string
	pwlen	- buflen of buffer

	Returns:
	>=0	length of returned string in user buffer
	<0	error (system-return)


	Name:
	getpwds

	Description:
	If you also want to get the internal 'ustat' block from the
	file-system (sort of as a free-bee of using this subroutine),
	use the form of this call as:

	Synopsis:
	int getpwds(ustat *sbp,char *pwbuf,int pwlen) noex

	Arguments:
	sbp		ustat block pointer to receive result
	pwbuf		present-working buffer pointer
	pwlen		prsent-working buffer length

	Returns:
	>=0	length of returned string in user buffer
	<0	error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<usystem.h>
#include	<varnames.hh>		/* |varname(3u)| */
#include	<bufsizevar.hh>
#include	<sncpyx.h>
#include	<isnot.h>		/* isNotPresent(3uc)| */
#include	<localmisc.h>

#include	"getpwd.h"

#pragma		GCC dependency	"mod/uconstants.ccm"

import uconstants ;			/* |varname(3u)| */

/* local defines */


/* imported namespaces */


/* local typenames */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

static bufsizevar	maxpathlen(getbufsize_mp) ;


/* exported variables */


/* exported subroutines */

int getpwd(char *pwbuf,int pwlen) noex {
	return getpwds(nullptr,pwbuf,pwlen) ;
}
/* end subroutine (getpwd) */

int getpwds(ustat *sbp,char *pwbuf,int pwlen) noex {
	int		rs = SR_FAULT ;
	int		pl = 0 ;
	if (pwbuf) {
	    pwbuf[0] = '\0' ;
	    if ((rs = maxpathlen) >= 0) {
		static cchar	*pwd = getenv(varname.pwd) ;
	        if (pwlen < 0) pwlen = rs ;
	        if (pwd != nullptr) {
	            ustat	*ssbp, sb1, sb2 ;
	            if ((rs = u_stat(pwd,&sb1)) >= 0) {
		        ssbp = (sbp) ? sbp : &sb2 ;
	                if ((rs = u_stat(".",ssbp)) >= 0) {
			    bool	f = true ;
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
	        if ((rs >= 0) && (pl == 0)) {
	            if ((rs = uc_getcwd(pwbuf,pwlen)) >= 0) {
	                pl = rs ;
	                if (sbp) {
		            rs = u_stat(pwbuf,sbp) ;
		        }
	            }
	        } /* end if (longer) */
	    } /* end if (maxpathlen) */
	} /* end if (non-null) */
	return (rs >= 0) ? pl : rs ;
}
/* end subroutine (getpwds) */


