/* umsg SUPPORT (UNIX® System-V IPC Messaging) */
/* encoding=ISO8859-1 */
/* lang=C++23 */

/* UNIX® System V IPC Messaging (MSG) subroutine interface */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-26, David A­D­ Morano
        This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Facility:
	umsg

	Description:
	This module manages the UNIX® System V Inter-Process-Communication
	(IPC) Messaging (MSG) facility interface.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/msg.h>
#include	<unistd.h>
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<errtimer.hh>
#include	<localmisc.h>

#include	"umsg.h"


/* local defines */


/* imported namespaces */

using std::nullptr_t ;			/* type */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct umsg ;
    typedef int (umsg::*mem_m)(int,void *,int,int) noex ;
    struct umsg {
	mem_m		m ;
	MSQIDDS		*buf ;
	key_t		key ;
	sysvmsgtype	msgtype ;
	int		cmd ;
	umsg() noex { } ;
	umsg(key_t k) noex : key(k) { } ;
	umsg(int c,MSQIDDS *b) noex : buf(b), cmd(c) { } ;
	umsg(sysvmsgtype t) noex : msgtype(t) { } ;
	int get(int,void *,int,int) noex ;
	int ctl(int,void *,int,int) noex ;
	int rcv(int,void *,int,int) noex ;
	int snd(int,void *,int,int) noex ;
	int operator () (int,void *,int,int) noex ;
    } ; /* end struct (umsg) */
}


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int u_msgget(key_t key,int msgflag) noex {
	cnullptr	np{} ;
	umsg		umo(key) ;
	umo.m = &umsg::get ;
	return umo(0,np,0,msgflag) ;
}

int u_msgctl(int msqid,int cmd,MSQIDDS *buf) noex {
	cnullptr	np{} ;
	umsg		umo(cmd,buf) ;
	umo.m = &umsg::ctl ;
	return umo(msqid,np,0,0) ;
}

int u_msgrcv(int msqid,void *msgp,int msgl,sysvmsgtype mt,int msgflag) noex {
	umsg		umo(mt) ;
	umo.m = &umsg::rcv ;
	return umo(msqid,msgp,msgl,msgflag) ;
}
/* end method (u_msgrcv) */

int u_msgsnd(int msqid,void *msgp,int msgl,int msgflag) noex {
	umsg		umo ;
	umo.m = &umsg::snd ;
	return umo(msqid,msgp,msgl,msgflag) ;
}
/* end method (u_msgsnd) */


/* local subroutines */

int umsg::operator () (int msqid,void *msgp,int msgl,int msgflag) noex {
	errtimer	to_again	= utimeout[uto_again] ;
	errtimer	to_busy		= utimeout[uto_busy] ;
	errtimer	to_nomem	= utimeout[uto_nomem] ;
	errtimer	to_nosr		= utimeout[uto_nosr] ;
	errtimer	to_nobufs	= utimeout[uto_nobufs] ;
	errtimer	to_mfile	= utimeout[uto_mfile] ;
	errtimer	to_nfile	= utimeout[uto_nfile] ;
	errtimer	to_nolck	= utimeout[uto_nolck] ;
	errtimer	to_nospc	= utimeout[uto_nospc] ;
	errtimer	to_dquot	= utimeout[uto_dquot] ;
	errtimer	to_io		= utimeout[uto_io] ;
	reterr		r ;
	int		rs ;
	repeat {
	    errno = 0 ;
	    if ((rs = (this->*m)(msqid,msgp,msgl,msgflag)) < 0) {
                r(rs) ;                 /* <- default causes exit */
                switch (rs) {
                case SR_AGAIN:
                    r = to_again(rs) ;
                    break ;
                case SR_BUSY:
                    r = to_busy(rs) ;
                    break ;
                case SR_NOMEM:
                    r = to_nomem(rs) ;
                    break ;
                case SR_NOSR:
                    r = to_nosr(rs) ;
                    break ;
                case SR_NOBUFS:
                    r = to_nobufs(rs) ;
                    break ;
                case SR_MFILE:
                    r = to_mfile(rs) ;
                    break ;
                case SR_NFILE:
                    r = to_nfile(rs) ;
                    break ;
                case SR_NOLCK:
                    r = to_nolck(rs) ;
                    break ;
                case SR_NOSPC:
                    r = to_nospc(rs) ;
                    break ;
                case SR_DQUOT:
                    r = to_dquot(rs) ;
                    break ;
                case SR_IO:
                    r = to_io(rs) ;
                    break ;
                case SR_INPROGRESS:
                case SR_INTR:
                    r(false) ;
                    break ;
                } /* end switch */
                rs = r ;
	    } /* end if (error) */
	} until ((rs >= 0) || r.fexit) ;
	return rs ;
}
/* end subroutine (umsg::operator) */

int umsg::get(int,void *,int,int msgflag) noex {
	int		rs ;
	if ((rs = msgget(key,msgflag)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (umsg::get) */

int umsg::ctl(int msqid,void *,int,int) noex {
	int		rs ;
	if ((rs = msgctl(msqid,cmd,buf)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (umsg::ctl) */

int umsg::rcv(int msqid,void *msgp,int msgl,int msgflag) noex {
	int		rs = SR_FAULT ;
	if (msgp) {
	    rs = SR_INVALID ;
	    if (msgl > 0) {
		csize	msize = size_t(msgl) ;
		ssize_t	rsize ;
	        if ((rsize = msgrcv(msqid,msgp,msize,msgtype,msgflag)) < 0) {
	            rs = (- errno) ;
	        } else {
		    rs = intsat(rsize) ;
		}
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end method (umsg::rcv) */

int umsg::snd(int msqid,void *msgp,int msgl,int msgflag) noex {
	int		rs = SR_FAULT ;
	if (msgp) {
	    rs = SR_INVALID ;
	    if (msgl > 0) {
		csize	msize = size_t(msgl) ;
	        if ((rs = msgsnd(msqid,msgp,msize,msgflag)) < 0) {
	            rs = (- errno) ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end method (umsg::snd) */


