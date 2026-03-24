/* mktmpusers SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* make the USERS directory in TMP directory */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-01, David A­D­ Morano
	This subroutine was originally written for Rightcore Network
	Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mktmpusers

	Description:
	This subroutine creates the USERS directory in the TMP directory.
	That is:
		TMPDIR/users

	Synopsis:
	int mktmpusers(char *rbuf) noex

	Arguments:
	rbuf		buffer to receive resulting created directory name

	Returns:
	>=0		length of resulting directory name
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<strlibval.hh>
#include	<mkpathx.h>
#include	<removes.h>
#include	<isnot.h>		/* |isNotPresent(3uc)| */
#include	<localmisc.h>

#include	"mktmp.h"

#pragma		GCC dependency		"mod/uconstants.ccm"

import uconstants ;			/* |sysword(3u)| */

/* local defines */

#ifndef	TMPUSERDMODE
#define	TMPUSERDMODE	(S_IAMB | S_ISVTX)
#endif


/* imported namespaces */


/* local typenames */


/* external subroutines */

extern "C" {
    extern int uc_stat(cchar *,ustat *) noex ;
    extern int uc_minmod(cchar *,mode_t) noex ;
}


/* external variables */


/* local structures */


/* forward references */

static int		tmpusers(char *rbuf) noex ;


/* local variables */

static strlibval	strtmpdir(strlibval_tmpdir) ;


/* exported variables */


/* exported subroutines */

int mktmpusers(char *rbuf) noex {
	int		rs = SR_FAULT ;
	int		rl = 0 ;
	if (rbuf) ylikely {
            static cchar    *tmpdir = strtmpdir ;
            rs = SR_NOMEM ;
            if (tmpdir != nullptr) ylikely {
                cchar       *users = sysword.w_users ;
                if ((rs = mkpath(rbuf,tmpdir,users)) >= 0) ylikely {
                    rl = rs ;
                    if (ustat sb ; (rs = uc_stat(rbuf,&sb)) >= 0) {
                        if (! S_ISDIR(sb.st_mode)) {
                            if ((rs = removes(rbuf)) >= 0) {
                                rs = tmpusers(rbuf) ;
                            } /* end if (rmdirs) */
                        } /* end if */
                    } else if (isNotPresent(rs)) {
                        rs = tmpusers(rbuf) ;
                    } /* end if */
                } /* end if (mkpath) */
            } /* end if (strtmpdir) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (mktmpusers) */


/* local subroutines */

static int tmpusers(char *rbuf) noex {
	cmode		dm = TMPUSERDMODE ;
	int		rs ;
	if ((rs = u_mkdir(rbuf,dm)) >= 0) ylikely {
	    rs = uc_minmod(rbuf,dm) ;
	} /* end if (ok) */
	return rs ;
}
/* end subroutine (mktmpuser) */


