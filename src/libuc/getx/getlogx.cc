/* getlogx SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get the name of the controlling terminal for the current session */
/* version %I% last-modified %G% */

#define	CF_GETUTMPTERM	1		/* use |getutmpterm(3uc)| */

/* revision history:

	= 1999-01-10, David A­D­ Morano
	This code was originally written. It was prompted by
	the failure of other terminal message programs from finding
	the proper controlling terminal.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getlogid
	getlogname
	getlogterm
	getloghost

	Description:
	This subroutine will find and return the name of the
	controlling {name, terminal, host} for the given session
	ID.

	Synopsis:
	int getlogid(char *rbuf,int rlen,pid_t sid) noex
	int getlogname(char *rbuf,int rlen,pid_t sid) noex
	int getlogterm(char *rbuf,int rlen,pid_t sid) noex
	int getloghost(char *rbuf,int rlen,pid_t sid) noex

	Arguments:
	rbuf		result buffer pointer
	rlen		result buffer length
	sid		session ID

	Returns:
	>=0		length of returned c-string
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>		/* POSIX® system types */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ucgetpid.h>		/* LIBUC |ucsid| */
#include	<getutmp.h>		/* LIBUC |getutmpterm(3uc)| */
#include	<tmpx.h>		/* LIBUC */
#include	<storebuf.h>		/* LIBUC */
#include	<mknpath.h>		/* LIBUC */
#include	<sncpyx.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"getlogx.h"

#pragma		GCC dependency		"mod/uconstants.ccm"

import uconstants ;			/* |sysword(3u)| */

/* local defines */

#ifndef	CF_GETUTMPTERM
#define	CF_GETUTMPTERM	1		/* use |getutmpterm(3uc)| */
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int getusid(pid_t sid) noex {
    	int	rs = int(sid) ;
	if (sid < 0) {
	    rs = ucsid ;
	}
	return rs ;
} /* end subroutine (getusid) */


/* local variables */

constexpr int		lline = TMPX_LLINE ;
constexpr int		luser = TMPX_LUSER ;
constexpr int		lhost = TMPX_LHOST ;

constexpr bool		f_getutmpterm = CF_GETUTMPTERM ;


/* exported variables */


/* exported subroutines */

int getlogid(char *rbuf,int rlen,pid_t sid) noex {
	int		rs = SR_FAULT ;
	if (rbuf) ylikely {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if (rlen > 0) ylikely {
	        if (utmpentx ue{} ; (rs = getutmpent(&ue,sid)) >= 0) ylikely {
	            rs = sncpy(rbuf,rlen,ue.id) ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (getlogid) */

int getlogname(char *rbuf,int rlen,pid_t sid) noex {
	int		rs = SR_FAULT ;
	if (rbuf) ylikely {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if (rlen > 0) ylikely {
	        if (utmpentx ue{} ; (rs = getutmpent(&ue,sid)) >= 0) ylikely {
	            rs = sncpy(rbuf,rlen,ue.user) ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (getlogname) */

local int getlogtermer(char *dbuf,int dlen,int usid) noex {
	cnullptr	np{} ;
	cint		of = O_RDONLY ;
	int		rs ;
	int		rs1 ;
	int		len = 0 ; /* return-value */
        if (tmpx ut ; (rs = ut.open(np,of)) >= 0) ylikely {
            if (tmpx_ent ue{} ; (rs = ut.fetchpid(&ue,usid)) >= 0) ylikely {
		cchar	*devdir = sysword.w_devdir ;
		rs = mknpathw(dbuf,dlen,devdir,ue.ut_line,lline) ;
		len = rs ;
            } /* end if (got it) */
            rs1 = ut.close ;
            if (rs >= 0) rs = rs1 ;
        } /* end if (TMPX) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (getlogtermer) */

int getlogterm(char *dbuf,int dlen,pid_t sid) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (dbuf) ylikely {
	    rs = SR_INVALID ;
	    dbuf[0] = '\0' ;
	    if (dlen > 0) ylikely {
		if_constexpr (f_getutmpterm) {
		    rs = getutmpterm(dbuf,dlen,sid) ;
		    len = rs ;
		} else {
		    if ((rs = getusid(sid)) >= 0) ylikely {
		        rs = getlogtermer(dbuf,dlen,rs) ;
		        len = rs ;
		    } /* end if (getusid) */
		} /* end if_constexpr (f_getutmpterm) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (getlogterm) */

int getloghost(char *dbuf,int dlen,pid_t sid) noex {
	int		rs = SR_FAULT ;
	if (dbuf) ylikely {
	    rs = SR_INVALID ;
	    dbuf[0] = '\0' ;
	    if (dlen > 0) ylikely {
	        if (utmpentx ue{} ; (rs = getutmpent(&ue,sid)) >= 0) ylikely {
	            rs = sncpy(dbuf,dlen,ue.host) ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (getloghost) */


