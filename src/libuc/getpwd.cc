/* getpwd SUPPORT */
/* lang=C++20 */

/* get the Present-Working-Directory (PWD) of the process */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This subroutine was originally written.

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
	<0	error


	Name:
	getpwds

	Description:
	If you also want to get the internal 'STAT' block from the
	file-system (sort of as a free-bee of using this subroutine),
	use the form of this call as:

	Synopsis:
	int getpwds(USTAT *sbp,char *pwbuf,int pwlen) noex

	Arguments:
	sbp		STAT block pointer to receive result
	pwbuf		present-working buffer pointer
	pwlen		prsent-working buffer length

	Returns:
	>=0	length of returned string in user buffer
	<0	error

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<varnames.hh>
#include	<bufsizevar.hh>
#include	<sncpyx.h>
#include	<localmisc.h>

#include	"getpwd.h"


/* local defines */


/* external subroutines */


/* external variables */


/* forward references */


/* local variables */

static bufsizevar	maxpathlen(getbufsize_mp) ;


/* exported subroutines */

int getpwd(char *pwbuf,int pwlen) noex {
	return getpwds(nullptr,pwbuf,pwlen) ;
}
/* end subroutine (getpwd) */

int getpwds(USTAT *sbp,char *pwbuf,int pwlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		pl = 0 ;
	if (pwbuf) {
	    cchar	*vn = varname.pwd ;
	    pwbuf[0] = '\0' ;
	    if ((rs = maxpathlen) >= 0) {
		cchar	*pwd ;
	        if (pwlen < 0) pwlen = rs ;
		rs = SR_NOENT ;
	        if ((pwd = getenv(vn)) != nullptr) {
	            USTAT	*ssbp, sb1, sb2 ;
	            if ((rs1 = u_stat(pwd,&sb1)) >= 0) {
		        ssbp = (sbp) ? sbp : &sb2 ;
	                if ((rs = u_stat(".",ssbp)) >= 0) {
			    bool	f = true ;
			    rs = SR_NOENT ;
	                    f = f && (sb1.st_dev == ssbp->st_dev) ;
			    f = f && (sb1.st_ino == ssbp->st_ino) ;
			    if (f) {
		                rs = sncpy1(pwbuf,pwlen,pwd) ;
			        pl = rs ;
		            }
	                } /* end if (stat) */
	            } /* end if (stat) */
	        } /* end if (quickie) */
	        if (rs == SR_NOENT) {
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


