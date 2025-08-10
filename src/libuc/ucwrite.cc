/* ucwrite SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* UNIX® write system call subroutine */
/* version %I% last-modified %G% */


/* revision history:

	= 1986-03-26, David A­D­ Morano
	This code was originally written to get around some
	stupid UNIX® sematics of their stupid system calls!

	= 1993-02-21, David A­D­ Morano
	The interrupt code below was changed so that stupid UNIX®
	would not ____ up when the file descriptor got a HANUP on
	the other end.  This problem surfaced in connection with
	networking stuff.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This is an enhanced |write(2)|-like subroutine that waits
	until a write will not hang before making the actual write.


	Name:
	uc_writen

	Description:
	This subroutine writes a fixed number of bytes even for
	those devices that can return without writing all of the
	bytes that we wanted to.

	Synopsis:
	int uc_writen(int fd,cvoid *ubuf,int ulen) noex

	Arguments:
	fd		file descriptor
	ubuf		buffer holding data to write
	ulen		length of data to write

	Returns:
	>=0		number of bytes written
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/uio.h>
#include	<unistd.h>
#include	<poll.h>
#include	<cerrno>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<localmisc.h>

#pragma		GCC dependency	"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| + |getlenstr(3u)| */

/* local defines */

#ifndef	POLL_INTMULT
#define	POLL_INTMULT	1000
#endif

#define	TO_NOSR		(10 * 60)
#define	TO_NOSPC	(10 * 60)
#define	TO_NOLCK	10
#define	TO_AGAIN	2


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_writeto(int fd,cvoid *ubuf,int ulen,int to) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (ubuf) {
	    rs = SR_BADF ;
	    if (fd >= 0) {
	        POLLFD		fds[1] ;
	        time_t		ti_now ;
	        time_t		ti_start ;
	        int		nfds = 0 ;
	        bool		fexit = false ;
	        if (to >= 0) {
	            ti_now = time(nullptr) ;
	            ti_start = ti_now ;
	        }
	        fds[nfds].fd = fd ;
	        fds[nfds].events = POLLOUT ;
	        fds[nfds].revents = 0 ;
	        nfds += 1 ;
	        while (rs >= 0) {
	            if ((rs = u_poll(fds,nfds,500)) >= 0) {
	                if (rs > 0) {
	                    cint 	re = fds[0].revents ;
	                    if (re & POLLHUP) {
	                        rs = SR_HANGUP ;
	                    } else if (re & POLLERR) {
	                        rs = SR_POLLERR ;
	                    } else if (re & POLLNVAL) {
	                        rs = SR_NOTOPEN ;
	                    } else if (re & POLLOUT) {
			        fexit = true ;
	                        rs = u_write(fd,ubuf,ulen) ;
	                        len = rs ;
	                    }
	                } /* end if (got something) */
	            } else if (rs == SR_INTR) {
		        rs = SR_OK ;
	            }
	            if (rs >= 0) {
		        if (fexit) break ;
		        if (to >= 0) {
		            ti_now = time(nullptr) ;
		            if ((ti_now - ti_start) >= to) {
		                rs = SR_TIMEDOUT ;
		                break ;
		            }
		        }
	            } /* end if (ok) */
	        } /* end while */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (uc_writeto) */

int uc_write(int fd,cvoid *wbuf,int wlen) noex {
	return uc_writeto(fd,wbuf,wlen,-1) ;
}
/* end subroutine (uc_write) */

int uc_writen(int fd,cvoid *abuf,int µalen) noex {
	int		rs = SR_FAULT ;
	int		tlen = 0 ; /* return-value */
	if (abuf) {
            rs = SR_BADF ;
	    if (fd >= 0) {
	        cchar	*abp = charp(abuf) ;
		if (int alen ; (alen = getlenstr(abp,µalen)) > 0) {
	            for (int alenr = alen ; (rs >= 0) && (alenr > 0) ; ) {
	                if ((rs = u_write(fd,abp,alenr)) >= 0) {
		            abp += rs ;
		            alenr -= rs ;
			    tlen += rs ;
		        } /* end if (u_write) */
	            } /* end for */
	        } else {
	            rs = u_write(fd,abp,0) ;
		    tlen = rs ;
	        } /* end if */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? tlen : rs ;
}
/* end subroutine (uc_writen) */


