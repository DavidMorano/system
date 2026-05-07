/* getpfopts SUPPORT (Get-Postfacts-Options) */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* get the POSTFACTS options */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */

/* revision history:

	= 1995-05-01, David A­D­ Morano
	This code module was completely rewritten to replace any
	original garbage that was here before.

*/

/* Copyright © 1995 David A­D­ Morano.  All rights reserved. */
/* Use is subject to license terms. */

/******************************************************************************

  	Name:
	getpfopts

  	Description:
	This subroutine parses out options from the main PCS
	configuration file.

	Synopsis:
	int getpfopts(proginfo *gp,vecstr *setsp) noex

	Arguments:
	gp		pointer to PROGINO
	setsp		pointer to VECSTR

	Returns:
	>=0		OK
	<0		error (system-return)

******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<ctime>
#include	<csignal>
#include	<cstddef>
#include	<cstdlib>
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<bfile.h>
#include	<userinfo.h>
#include	<baops.h>
#include	<field.h>
#include	<vecstr.h>
#include	<pcsconf.h>
#include	<mallocstuff.h>
#include	<headkeymat.h>
#include	<localmisc.h>

#include	"config.h"
#include	"defs.h"
#include	"getpfopts.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr cpcchar	progopts[] = {
	[getpfopt_mailername]	= "mailername",
	[getpfopt_progrbbpost]	= "progrbbpost",
	[getpfopt_progmsgs]	= "progmsgs",
	[getpfopt_newsgroup]	= "newsgroup",
	[getpfopt_spooldir]	= "spooldir",
	[getpfopt_overlast]	= nullptr
} ; /* end array */

constexpr char		sname[] = SEARCHNAME ;


/* exported variables */


/* exported subroutines */

int getpfopts(proginfo *gp,vecstr *setsp) noex {
    	cnullptr	np{} ;
    	int		rs = SR_FAULT ;
	if (gp & setsp) {
            char    *cp ;
            /* system-wide options? */
	    rs = SR_OK ;
            for (int i = 0 ; vecstr_get(setsp,i,&cp) >= 0 ; i += 1) {
                char        *cp2 ;
                if (cp == nullptr) continue ;
                if (! headkeymat(sname,cp,-1)) continue ;
                /* we have one of ours, separate the keyname from the value */
                cp += (lenstr(sname) + 1) ;
                if ((cp2 = strchr(cp,'=')) == nullptr) continue ;
		cint cl = intconv(cp2 - cp) ;
                if (int oi ; (oi = matstr3(progopts,cp,cl)) >= 0) {
                    cp2 += 1 ;
                    switch (oi) {
                    case getpfopt_mailername:
                        if (*cp2 && (gp->mailername == nullptr)) {
                            gp->mailername = mallocstr(cp2) ;
                        }
                        break ;
                    case getpfopt_progrbbpost:
                        if (*cp2 && (gp->prog_rbbpost == nullptr)) {
                            gp->prog_rbbpost = mallocstr(cp2) ;
                        }
                        break ;
                    case getpfopt_progmsgs:
                        if (*cp2 && (gp->prog_msgs == nullptr)) {
                            gp->prog_msgs = mallocstr(cp2) ;
                        }
                        break ;
                    case getpfopt_newsgroup:
                        if (*cp2 && (gp->newsgroup == nullptr)) {
                            gp->newsgroup = mallocstr(cp2) ;
                        }
                        break ;
                    case getpfopt_spooldir:
                        if (*cp2 && (gp->spooldname == nullptr)) {
                            gp->spooldname = mallocstr(cp2) ;
                        }
                        break ;
		    default:
			rs = SR_BUGCHECK ;
			break ;
                    } /* end switch */
                } /* end if (got a match) */
            } /* end for */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (getpfopts) */


