/* ustr SUPPORT (UNIX® Streams) */
/* lang=C++23 */

/* UNIX® STREAMS management system-call subroutines */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-26, David A­D­ Morano
        This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This module manages UNIX® STREAMS.  These are the STREAMS
	invented by Dennis Ritchie (and ported by others) for UNIX®
	System V Release 4. For completeness, the people who ported
	STREAMS to UNIX® System V Release 4 were: Robert Israel,
	Gil McGrath, Dave Olander, Her-Daw Che, and Maury Bach.

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<unistd.h>
#include	<poll.h>
#include	<cerrno>
#include	<usystem.h>
#include	<usupport.h>
#include	<localmisc.h>
#include	<ustropts.h>			/* <- money shot */

#include	"ustr.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct ustr ;
    typedef int (ustr::*mem_f)(int,STRBUF *,STRBUF *) noex ;
    struct ustr {
	mem_f		m ;
	int		*fp ;
	int		*bp ;
	int		flags ;
	ustr(int *ufp,int *ubp = nullptr) noex {
	    fp = ufp ;
	    bp = ubp ;
	} ; /* end dtor */
	ustr(int fl) noex : flags(fl) { } ;
	int msgget(int,STRBUF *,STRBUF *) noex ;
	int msggetp(int,STRBUF *,STRBUF *) noex ;
	int msgput(int,STRBUF *,STRBUF *) noex ;
	int msgputp(int,STRBUF *,STRBUF *) noex ;
	int operator () (int,STRBUF *,STRBUF *) noex ;
    } ; /* end struct (ustr) */
}


/* forward references */


/* local variables */


/* exported subroutines */

int u_strmsgget(int fd,STRBUF *cmp,STRBUF *dmp,int *fp) noex {
	ustr		uso(fp) ;
	uso.m = &ustr::msgget ;
	return uso(fd,cmp,dmp) ;
}
/* end subroutine (u_strmsgget) */

int u_msgmsggetp(int fd,STRBUF *cmp,STRBUF *dmp,int *bp,int *fp) noex {
	ustr		uso(fp,bp) ;
	uso.m = &ustr::msggetp ;
	return uso(fd,cmp,dmp) ;
}
/* end subroutine (u_strmsggetp) */

int u_strmsgput(int fd,STRBUF *cmp,STRBUF *dmp,int flags) noex {
	ustr		uso(flags) ;
	uso.m = &ustr::msgput ;
	return uso(fd,cmp,dmp) ;
}
/* end subroutine (u_strmsgput) */

int u_strmsgputp(int fd,STRBUF *cmp,STRBUF *dmp,int flags) noex {
	ustr		uso(flags) ;
	uso.m = &ustr::msgputp ;
	return uso(fd,cmp,dmp) ;
}
/* end subroutine (u_strmsgput) */


/* local subroutines */

int ustr::operator () (int fd,STRBUF *cmp,STRBUF *dmp) noex {
	POLLFD		fds[2] = {} ;
	int		to_nosr = utimeout[uto_nosr] ;
	int		to_nospc = utimeout[uto_nospc] ;
	int		to_nomem = utimeout[uto_nomem] ;
	int		to_nolck = utimeout[uto_nolck] ;
	int		to_again = utimeout[uto_again] ;
	int		rs ;
	int		rs1 ;
	int		nfds = 0 ;
	bool		f_exit = false ;
	repeat {
	    errno = 0 ;
	    if ((rs = (this->*m)(fd,cmp,dmp)) < 0) {
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
		    rs = SR_OK ;
	            if (nfds == 0) {
	                fds[nfds].fd = fd ;
	                fds[nfds].events = 0 ;
	                fds[nfds].revents = 0 ;
	                nfds += 1 ;
	            } /* end if */
	            if ((rs1 = u_poll(fds,nfds,0)) < 0) {
	                cint	re = fds[0].revents ;
	                if (re & POLLHUP) {
	                    rs = SR_HANGUP ;
	                } else if (re & POLLERR) {
	                    rs = SR_POLLERR ;
	                } else if (re & POLLNVAL) {
	                    rs = SR_NOTOPEN ;
		        }
			f_exit = (rs < 0) ;
	            } /* end if (we had some poll results) */
		    break ;
	        } /* end switch */
	    } /* end if (some kind of error) */
	} until ((rs >= 0) || f_exit) ;
	return rs ;
}
/* end subroutine (ustr::operator) */

int ustr::msgget(int fd,STRBUF *cmp,STRBUF *dmp) noex {
	int		rs ;
	if ((rs = getmsg(fd,cmp,dmp,fp)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (ustr::msgget) */

int ustr::msggetp(int fd,STRBUF *cmp,STRBUF *dmp) noex {
	int		rs ;
	if ((rs = getpmsg(fd,cmp,dmp,bp,fp)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (ustr::msggetp) */

int ustr::msgput(int fd,STRBUF *cmp,STRBUF *dmp) noex {
	int		rs ;
	if ((rs = putmsg(fd,cmp,dmp,fp)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (ustr::msgput) */

int ustr::msgputp(int fd,STRBUF *cmp,STRBUF *dmp) noex {
	int		rs ;
	if ((rs = putpmsg(fd,cmp,dmp,bp,fp)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end method (ustr::msgputp) */



