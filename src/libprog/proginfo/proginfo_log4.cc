/* proginfo_log4 SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* create a log-id */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */
#define	CF_SERIAL	1		/* serial or PID */

/* revision history:

	= 2008-09-01, David A­D­ Morano
	This subroutine was adopted from the DWD program.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:

	Description:
	This subroutine creates a log-id for use in logging entries
	to a log-file.

	Synopsis:
	int proglog_id(PI *pip) noex

	Arguments:
	pip		pointer to PI object

	Returns:
	>=0		OK
	<0		error (system-return)

	Notes:
	For programs that are actually built-in SHELL commands, the
	compile-time switch flag CF_SERIAL must always be enabled
	(set to '1') so that the process PID is not used.  This is
	because the process PID will always be the same for calls
	to built-in commands.

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ucmemory.h>		/* LIBUC */
#include	<ucgetpid.h>		/* LIBUC */
#include	<getserial.h>		/* LIBUC */
#include	<mkpathx.h>		/* LIBUC */
#include	<mklogid.h>		/* LIBUC */
#include	<localmisc.h>		/* |LOGIDLEN| */

#include	"proginfo.hh"

#pragma		GCC dependency		"mod/libutil.ccm"

module proginfo.log ;

import libutil ;			/* |lenstr(3u)| */
import uconstants ;			/* |sysword(3u)| */
import proginfo.base ;

/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif
#ifndef	CF_SERIAL
#define	CF_SERIAL	0		/* serial or PID */
#endif


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* externals variables */


/* local structures */

namespace {
    struct ider {
	proginfo	*pip ;
	int		v ;
	ider(proginfo *p) noex : pip(p), v(-1) { } ;
	operator int	() noex ;
	int retpid	() noex ;
	int make	() noex ;
	int serialpr	(char *,int) noex ;
	int serialtmp	(char *,int) noex ;
    } ; /* end struct (ider) */
    typedef int (ider::*ider_m)(char *,int) noex ;
} /* end namespace */


/* forward references */


/* local variables */

constexpr ider_m	tries[] = {
    	&ider::serialpr,
    	&ider::serialtmp
} ; /* end array */

ccharp			serial		= sysword.w_serial ;
cchar			vcname[]	= "var" ;
cbool			f_serial	= CF_SERIAL ;


/* exported variables */


/* exported subroutines */

int proglog_id(PI *pip) noex {
	int		rs = SR_FAULT ;
	if (pip) ylikely {
	    rs = SR_OK ;
	    if (pip->nodename) {
		if (pip->nodename[0]) {
	    	    ider io(pip) ;
	    	    rs = io ;
		} /* end if (non-empty) */
	    } /* end if (non-null) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (proglog_id) */


/* local subroutines */

ider::operator int () noex {
    	int		rs ;
	int		rs1 ;
	int		ll = 0 ; /* return-value */
	if ((rs = retpid()) >= 0) {
	    if (char *tbuf ; (rs = mem.mp(&tbuf)) >= 0) {
		cint tlen = rs ;
		for (cauto &m : tries) {
		    rs = (this->*m)(tbuf,tlen) ;
		    if (rs) break ;
		} /* end for */
		rs1 = mem.free(tbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (maxpathlen) */
	} /* end if (retpid) */
	return (rs >= 0) ? ll : rs ;
} /* end method (ider::operator) */

int ider::retpid() noex {
    	int		rs = SR_OK ;
	if ((rs = pip->pid) < 0) {
	    if ((rs = ucpid) >= 0) {
	        pip->pid = rs ;
	    }
	} /* end if (needed) */
	return rs ;
} /* end method (ider:retpid) */

int ider::make() noex {
	cint		llen = LOGIDLEN ;
    	int		rs ;
	int		ll = 0 ; /* return-value */
	cchar		*nn = pip->nodename ;
	char		lbuf[LOGIDLEN + 1] ;
	if ((rs = mklogid(lbuf,llen,nn,-1,v)) > 0) {
	    ccharpp vpp = &pip->logid ;
	    ll = rs ;
	    rs = pip->setent(vpp,lbuf,ll) ;
	} /* end if (mklogid) */
	return (rs >= 0) ? ll : rs ;
} /* end method (ider::make) */

int ider::serialpr(char *tbuf,int) noex {
    	int		rs = SR_OK ;
	if (ccharp pr = pip->pr) {
	    if ((rs = mkpath(tbuf,pr,vcname,serial)) >= 0) {
	        if ((rs = getserial(tbuf)) >= 0) {
	 	    rs = make() ;
	        } /* end if (getserial) */
	    } /* end if (make) */
	} /* end if (non-null) */
	return rs ;
} /* end method (ider::serialpr) */

int ider::serialtmp(char *tbuf,int) noex {
    	int		rs = SR_OK ;
	if (ccharp tmpdir = pip->tmpdname) {
	    if ((rs = mkpath(tbuf,tmpdir,serial)) >= 0) {
	        if ((rs = getserial(tbuf)) >= 0) {
	 	    rs = make() ;
	        } /* end if (getserial) */
	    } /* end if (make) */
	} /* end if (non-null) */
	return rs ;
} /* end method (ider::serialtmp) */


