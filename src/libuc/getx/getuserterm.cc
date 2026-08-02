/* getuserterm SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get the name of the most recently accessed controlling terminal */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-01-10, David A­D­ Morano
	This code was originally written.  

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getuserterm

	Description:
	This subroutine will find and return an open FD for the
	controlling terminal for the given username, if that user
	is logged in and even has a controlling terminal.

	Synopsis:
	int getuserterm(char *rbuf,int rlen,cchar *username) noex

	Arguments:
	- rbuf		result buffer pointer
	- rlen		result buffer length
	- username	session ID to find controlling terminal for

	Returns:
	>=	length of name of controlling terminal
	<0	error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<ctime>			/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<tmpx.h>		/* LIBUC */
#include	<snx.h>			/* LIBUC */
#include	<mkx.h>			/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"getuserterm.h"


/* local defines */

#define	DEVDNAME	"/dev/"


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct suber {
	cchar		*username ;
	char		*rbuf ;
	char		*tbuf{} ;
	int		rlen ;
	int		tlen ;
	suber(char *tp,int tl,cchar *up) noex : rbuf(tp), rlen(tl) {
	    username = up ;
	} ;
	int start() noex ;
	int finish() noex ;
	int tmpenum() noex ;
    } ; /* end struct (suber) */
} /* end namespace */


/* forward references */

local int	load(char *,int,tmpx_ent *) noex ;
local int	newer(char *,time_t *) noex ;


/* local variables */

constexpr char		devdname[] = DEVDNAME ;


/* exported variables */


/* exported subroutines */

int getuserterm(char *rbuf,int rlen,cchar *un) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ; /* return-value */
	if (rbuf && un) ylikely {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if (un[0]) ylikely {
		if (suber so(rbuf,rlen,un) ; (rs = so.start()) >= 0) ylikely {
		    if ((rs = so.tmpenum()) >= 0) {
			len = rs ;
		    }
		    rs1 = so.finish() ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (so) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (getuserterm) */


/* local subroutines */

int suber::start() noex {
	int		rs ;
	if (char *p ; (rs = lm_mp(&p)) >= 0) ylikely {
	    tbuf = p ;
	    tlen = rs ;
	} /* end if (memory-acquire) */
	return rs ;
} /* end method (suber::start) */

int suber::finish() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (tbuf) ylikely {
	    rs1 = lm_free(tbuf) ;
	    if (rs >= 0) rs = rs1 ;
	    tbuf = nullptr ;
	    tlen = 0 ;
	} /* end if (memory-release) */
	return rs ;
} /* end method (suber::finish) */

int suber::tmpenum() noex {
    	cnullptr	np{} ;
	time_t		tiacc = 0 ;
	cint		to = O_RDONLY ;
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	if (tmpx ut ; (rs = ut.open(np,to)) >= 0) ylikely {
	    if (tmpx_cur cur ; (rs = ut.curbegin(&cur)) >= 0) ylikely {
	        tmpx_ent	ue ;
	        while (rs >= 0) {
		    bool	f = true ;
	            rs1 = ut.fetchuser(&cur,&ue,username) ;
		    if (rs1 == SR_NOTFOUND) break ;
		    rs = rs1 ;
		    if (rs < 0) break ;
	            f = f && (ue.ut_type == TMPX_TPROCUSER) ;
	            f = f && (ue.ut_line[0] != '\0') ;
	            if (f) {
	                if ((rs = load(tbuf,tlen,&ue)) >= 0) {
	                    if ((rs = newer(tbuf,&tiacc)) > 0) {
	                        rs = sncpy(rbuf,rlen,tbuf) ;
	                        len = rs ;
	                    } /* end if (we had a better one) */
	                } /* end if (load) */
		    }
	            if (rs < 0) break ;
	        } /* end while (looping through entries) */
	        rs1 = ut.curend(&cur) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if */
	    rs1 = ut.close ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (UTMPX open) */
	return (rs >= 0) ? len : rs ;
} /* end method (suber::tmpenum) */

local int load(char *tbuf,int tlen,tmpx_ent *uep) noex {
	cint		ulen = TMPX_LLINE ;
	return mknpathw(tbuf,tlen,devdname,uep->ut_line,ulen) ;
} /* end subroutine (load) */

local int newer(char *termdev,time_t *tp) noex {
	cint		of = O_WRONLY ;
	int		rs ;
	int		rs1 ;
	int		f = false ; /* return-value */
	cmode		om = 0666 ;
	if ((rs = u_open(termdev,of,om)) >= 0) ylikely {
	    cint	fd = rs ;
	    if (ustat sb ; (rs = u_fstat(fd,&sb)) >= 0) ylikely {
		f = true ;
	        f = f && ((sb.st_mode & S_IWGRP) == S_IWGRP) ;
	        f = f && (sb.st_atime > *tp) ;
		if (f) *tp = sb.st_atime ;
	    } /* end if */
	    rs1 = u_close(fd) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (open) */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (newer) */


