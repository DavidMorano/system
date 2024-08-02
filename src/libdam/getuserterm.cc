/* getuserterm SUPPORT */
/* lang=C++20 */

/* get the name of the most recently accessed controlling terminal */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-01-10, David A­D­ Morano
	This subroutine was originally written.  

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
	- rbuf		user buffer to receive name of controlling terminal
	- rlen		length of user supplied buffer
	- username	session ID to find controlling terminal for

	Returns:
	>=	length of name of controlling terminal
	<0	error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<tmpx.h>
#include	<snx.h>
#include	<mkx.h>
#include	<localmisc.h>

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
	char		*tbuf ;
	int		rlen ;
	int		tlen ;
	suber(char *tp,int tl,cchar *up) noex : rbuf(tp), rlen(tl) {
	    username = up ;
	} ;
	int start() noex ;
	int finish() noex ;
	int tmpenum() noex ;
    } ; /* end struct (suber) */
}


/* forward references */

static int	load(char *,int,tmpx_ent *) noex ;
static int	newer(char *,const time_t,time_t *) noex ;


/* local variables */

constexpr cchar		devdname[] = DEVDNAME ;


/* exported variables */


/* exported subroutines */

int getuserterm(char *rbuf,int rlen,cchar *username) noex {
	int	rs ;
	int	rs1 ;
	int	len = 0 ;
	if (rbuf && username) {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if (username[0]) {
		suber	so(rbuf,rlen,username) ;
		if ((rs = so.start()) >= 0) {
		    {
			rs = so.tmpenum() ;
			len = rs ;
		    }
		    rs1 = so.finish() ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (so) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (getuserterm) */


/* local subroutines */

int suber::start() noex {
	int		rs ;
	char		*p{} ;
	if ((rs = malloc_mp(&p)) >= 0) {
	    tbuf = p ;
	    tlen = rs ;
	}
	return rs ;
}
/* end method (suber::start) */

int suber::finish() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (tbuf) {
	    rs1 = uc_free(tbuf) ;
	    if (rs >= 0) rs = rs1 ;
	    tbuf = nullptr ;
	    tlen = 0 ;
	}
	return rs ;
}
/* end method (suber::finish) */

int suber::tmpenum() noex {
	tmpx		utmp ;
	time_t		tiacc = 0 ;
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	if ((rs = tmpx_open(&utmp,nullptr,O_RDONLY)) >= 0) {
	    tmpx_cur	cur ;
	    tmpx_ent	ue ;
	    if ((rs = tmpx_curbegin(&utmp,&cur)) >= 0) {
		bool	f ;
	        while (rs >= 0) {
	            rs1 = tmpx_fetchuser(&utmp,&cur,&ue,username) ;
		    if (rs1 == SR_NOTFOUND) break ;
		    rs = rs1 ;
		    if (rs < 0) break ;
	            f = true ;
	            f = f && (ue.ut_type == TMPX_TPROCUSER) ;
	            f = f && (ue.ut_line[0] != '\0') ;
	            if (f) {
	                if ((rs = load(tbuf,tlen,&ue)) >= 0) {
			    time_t	titmp ;
			    len = rs ;
	                    if ((rs = newer(tbuf,tiacc,&titmp)) > 0) {
	                        tiacc = titmp ;
	                        rs = sncpy1(rbuf,rlen,tbuf) ;
	                        len = rs ;
	                    } /* end if (we had a better one) */
	                } /* end if */
		    }
	            if (rs < 0) break ;
	        } /* end while (looping through entries) */
	        rs1 = tmpx_curend(&utmp,&cur) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if */
	    rs1 = tmpx_close(&utmp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (UTMPX open) */
	return (rs >= 0) ? len : rs ;
}
/* end method (suber::tmpenum) */


/* local subroutines */

static int load(char *tbuf,int tlen,tmpx_ent *uep) noex {
	cint		ulen = TMPX_LLINE ;
	return mknpath2w(tbuf,tlen,devdname,uep->ut_line,ulen) ;
}

static int newer(char *termdev,const time_t current,time_t *tp) noex {
	cint		of = O_WRONLY ;
	int		rs ;
	int		rs1 ;
	int		f = false ;
	cmode		om = 0666 ;
	if ((rs = u_open(termdev,of,om)) >= 0) {
	    USTAT	sb ;
	    cint	fd = rs ;
	    if ((rs = u_fstat(fd,&sb)) >= 0) {
		f = true ;
	        f = f && ((sb.st_mode & S_IWGRP) == S_IWGRP) ;
	        f = f && (sb.st_atime > current) ;
		if (f) *tp = sb.st_atime ;
	    } /* end if */
	    rs1 = u_close(fd) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (open) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (newer) */


