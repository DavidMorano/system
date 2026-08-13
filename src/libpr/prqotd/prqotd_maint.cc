/* prqotd_maint SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* open a channel (file-descriptor) to the quote-of-the-day (QOTD) */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* compile-time debugging */

/* revision history:

	= 1998-02-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	prqotd_maint

	Description:
	This subroutine sort of forms the back-end maintenance end
	of the QOTD mechansim.

	Synopsis:
	int prqotd_maint(cchar *pr,int mjd,int of,int to) noex

	Arguments:
	pr		program-root
	mjd		modified-julian-day
	of		open-flags
	to		time-out

	Returns:
	>=0		FD of QOTD
	<0		error (system-return)

	Notes:
	- open flags:
		O_NOTTY
		O_EXCL		
		O_SYNC
		O_NDELAY
		O_NONBLOCK

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<ucfileop.h>
#include	<ucgetx.h>
#include	<getx.h>
#include	<getmjd.h>
#include	<ascii.h>
#include	<localmisc.h>		/* |DIGBUFLEN| */

#include	"prqotd.h"
#include	"prqotd_util.hh"
#include	"prqotd_subinfo.hh"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/uconstants.ccm"

import libutil ;			/* |lenstr(3u)| */
import uconstants ;			/* |varname(3u)| */

/* local defines */

#define	SUB		subinfo
#define	SUB_FL		subinfo_flags

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* compile-time debugging */
#endif


/* imported namespaces */

using prqotd::subinfo ;			/* type */
using prqotd::subinfo_start ;		/* subroutine */
using prqotd::subinfo_defaults ;	/* subroutine */
using prqotd::subinfo_logbegin ;	/* subroutine */
using prqotd::subinfo_spoolcheck ;	/* subroutine */
using prqotd::subinfo_qdirname ;	/* subroutine */
using prqotd::subinfo_gather ;		/* subroutine */
using prqotd::subinfo_logend ;		/* subroutine */
using prqotd::subinfo_finish ;		/* subroutine */
using prqotd::getdefmjd ;		/* subroutine */
using prqotd::mkqfname ;		/* subroutine */
using prqotd::init ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int prqotd_maints(cchar *,int,int,int,time_t) noex ;


/* local variables */

cbool		f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

int prqotd_maint(cchar *pr,int mjd,int of,int to) noex {
	int		rs = SR_FAULT ;
	int		fd = -1 ; /* return-value */
	if (pr) {
	    time_t	dt = 0 ;
	    rs = SR_OK ;
	    if (mjd <= 0) {
	        if (dt == 0) dt = getustime ;
	        rs = getdefmjd(dt) ;
	        mjd = rs ;
	    } /* end if */
	    if (rs >= 0) {
		if (static cint	rsi = init ; (rs = rsi) >= 0) {
	            rs = prqotd_maints(pr,mjd,of,to,dt) ;
		    fd = rs ;
		} /* end if (init) */
	    } /* end if (ok) */
	} /* end if (non-null) */
	return (rs >= 0) ? fd : rs ;
} /* end subroutine (prqotd_maint) */


/* local subroutines */

local int prqotd_maints(cchar *pr,int mjd,int of,int to,time_t dt) noex {
	int		rs ;
	int		rs1 ;
	int		fd = -1 ; /* return-value */
	SUB	 	si, *sip = &si ;
	if ((rs = subinfo_start(sip,dt,pr,of,to,mjd)) >= 0) {
            if ((rs = subinfo_defaults(sip)) >= 0) {
                if ((rs = subinfo_logbegin(sip)) >= 0) {
                    if ((rs = subinfo_spoolcheck(sip)) >= 0) {
                        if ((rs = subinfo_qdirname(sip,mjd)) >= 0) {
                            cchar   *qd = sip->qdname ;
			    if (char *qfname ; (rs = lm_mp(&qfname)) >= 0) {
                                if ((rs = mkqfname(qfname,qd,mjd)) >= 0) {
                                    cmode       om = 0664 ;
                                    of &= (~ OM_SPECIAL) ;
                                    rs = u_open(qfname,of,om) ;
                                    fd = rs ;
                                    if (rs == SR_NOENT) {
                                        rs = subinfo_gather(sip,qfname,om) ;
                                        fd = rs ;
                                        if (rs < 0) {
                                            uc_unlink(qfname) ;
                                        } /* end if (error) */
                                    } /* end if (NOENT) */
                                } /* end if (mkqfname) */
			        rs1 = lm_free(qfname) ;
			        if (rs >= 0) rs = rs1 ;
                            } /* end if (m-a-f) */
			} /* end if (subinfo_qfirname) */
                    } /* end if (spoolcheck) */
                    rs1 = subinfo_logend(sip) ;
                    if (rs >= 0) rs = rs1 ;
                } /* end if (logging) */
            } /* end if (defaults) */
            rs1 = subinfo_finish(sip) ;
            if (rs >= 0) rs = rs1 ;
            if ((rs < 0) && (fd >= 0)) {
                u_close(fd) ;
            } /* end if (error) */
        } /* end if (subinfo) */
	return (rs >= 0) ? fd : rs ;
} /* end subroutine (prqotd_maints) */


