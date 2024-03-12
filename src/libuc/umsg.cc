/* umsg SUPPORT (UNIX® System-V IPC Messaging) */
/* lang=C++23 */

/* UNIX® System V IPC Messaging (MSG) subroutine interface */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-26, David A­D­ Morano
        This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This module manages the UNIX® System V Inter-Process-Communication
	(IPC) Messaging (MSG) subroutine interface.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/msg.h>
#include	<unistd.h>
#include	<cerrno>
#include	<usystem.h>
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
    typedef int (umsg::*mem_f)(int,void *,int,int) noex ;
    struct umsg {
	mem_f		m ;
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


/* exported subroutines */

int u_msgget(key_t key,int msgflag) noex {
	const nullptr_t	np{} ;
	umsg		umo(key) ;
	umo.m = &umsg::get ;
	return umo(0,np,0,msgflag) ;
}

int u_msgctl(int msqid,int cmd,MSQIDDS *buf) noex {
	const nullptr_t	np{} ;
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
	int		to_nosr = utimeout[uto_nosr] ;
	int		to_nospc = utimeout[uto_nospc] ;
	int		to_nomem = utimeout[uto_nomem] ;
	int		to_nolck = utimeout[uto_nolck] ;
	int		to_again = utimeout[uto_again] ;
	int		rs ;
	bool		f_exit = false ;
	repeat {
	    errno = 0 ;
	    if ((rs = (this->*m)(msqid,msgp,msgl,msgflag)) < 0) {
	        switch (rs) {
	        case SR_NOSR:
	            if (to_nosr-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = true ;
		    }
	            break ;
	        case SR_NOSPC:
	            if (to_nospc-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = true ;
		    }
	            break ;
	        case SR_NOMEM:
	            if (to_nomem-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = true ;
		    }
	            break ;
	        case SR_NOLCK:
	            if (to_nolck-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = true ;
		    }
	            break ;
	        case SR_AGAIN:
	            if (to_again-- > 0) {
			msleep(1000) ;
		    } else {
			f_exit = true ;
		    }
	            break ;
	        case SR_INTR:
		    break ;
	        } /* end switch */
	    } /* end if (some kind of error) */
	} until ((rs >= 0) || f_exit) ;
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
	        if ((rs = msgrcv(msqid,msgp,msgl,msgtype,msgflag)) < 0) {
	            rs = (- errno) ;
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
	        if ((rs = msgsnd(msqid,msgp,msgl,msgflag)) < 0) {
	            rs = (- errno) ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end method (umsg::snd) */


