/* ucread SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface components for UNIX® library-3c */
/* read a line from a file descriptor but time it */
/* version %I% last-modified %G% */

#define	CF_STREAM	1		/* optimization for STREAM */

/* revision history:

	= 1998-03-26, David A­D­ Morano
	This was first written to give a little bit to UNIX® what
	we have in our own circuit pack OSes! Note that this
	subroutine depends on another little subroutine (|uc_reade(3uc)|)
	that is used to provide an underlying extended |read(2)|
	like capability.

	= 2003-11-25, David A­D­ Morano
	I have to laugh at how long some of these subroutines go
	before maintenance (looking at the date above)! I am adding
	the "peeking" type method of grabbing a line. This is
	superior to the old method in performance. In fairness,
	this subroutine was never used very much in performance-critical
	places but at least now I won't have to be embarassed about
	reading one character at a time (like many Internet daemons
	already do -- see the Berkeley remote-type protocols). For
	consistency with other similar subroutines, the option of
	FM_TIMED is assumed (check the name of this subroutine!).

*/

/* Copyright © 1998,2003 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	uc_readlnto
	uc_readln

	Description:
	Get a line code amount of data (data ending in an NL) and
	time it also so that we can abort if it times out.

	Synopsis:
	int uc_readlnto(kint fd,char *lbuf,int llen,int to) noex
	int uc_readln(int fd,char *lbuf,int llen) noex

	Arguments:
	fd		file descriptor
	lbuf		user buffer to receive daa
	llen		maximum amount of data the user wants
	to		time in seconds to wait

	Returns:
	>=0		amount of data returned
	<0		error (system-return)

	Note 2003-11-25, David A­D­ Morano
	I am going to try to use one or more peeking techniques to
	speed this up. If it is a socket, then we are good to go
	and we will use |recv(3n)| with the "PEEK" option. If it
	is a STREAM (who knows what is and what isn't now-a-days)
	we will try to use |ioctl(2)| with "I_PEEK".  Else, we punt
	back to reading a character at a time.  This subroutine
	performs like other subroutines, that can time the operation,
	with the option FM_TIMED set.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/socket.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<six.h>
#include	<strn.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"ucread.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	BUFLEN		2048

#ifndef	POLL_INTMULT
#define	POLL_INTMULT	1000
#endif

#define	POLLEVENTS	(POLLIN | POLLPRI)


/* imported namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	uc_recve(int,void *,int,int,int,int) noex ;
}

extern "C" {
    extern int	isasocket(int) noex ;
}


/* external variables */


/* local structures */


/* forward references */

local int	readln_socket(int,char *,int,int) noex ;
local int	readln_stream(int,char *,int,int) noex ;
local int	readln_seekable(int,char *,int,int,off_t) noex ;
local int	readln_default(int,char *,int,int) noex ;

local int 	isseekable(int,off_t *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_readlnto(int fd,char *lbuf,int llen,int to) noex {
	int		rs = SR_FAULT ;
	if (lbuf) ylikely {
	    rs = SR_BADF ;
	    if (fd >= 0) ylikely {
	        rs = SR_INVALID ;
	        if (llen > 0) ylikely {
	            off_t	fo{} ;
	            if (isasocket(fd)) {
	               rs = readln_socket(fd,lbuf,llen,to) ;
	            } else if (isastream(fd)) {
	                rs = readln_stream(fd,lbuf,llen,to) ;
	            } else if (isseekable(fd,&fo)) {
	                rs = readln_seekable(fd,lbuf,llen,to,fo) ;
	            } else {
	                rs = readln_default(fd,lbuf,llen,to) ;
	            }
	        } /* end if (positive) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_readlnto) */

int uc_readln(int fd,char *lbuf,int llen) noex {
	return uc_readlnto(fd,lbuf,llen,-1) ;
}
/* end subroutine (uc_readln) */


/* private subroutines */

local int readln_socket(int fd,char *lbuf,int llen,int to) noex {
    	cnullptr	np{} ;
	cint		mopts = MSG_PEEK ;
	cint		opts = (FM_TIMED | FM_EXACT) ;
	int		rs = SR_OK ;
	int		lbl ;
	int		tlen = 0 ; /* return-value */
	char		*lbp ;
	while ((rs >= 0) && (tlen < llen)) {
	    lbp = (lbuf + tlen) ;
	    lbl = (llen - tlen) ;
	    if ((rs = uc_recve(fd,lbp,lbl,mopts,to,opts)) > 0) {
		int rlen ;
	        int len = rs ;
	            if (cc *tp ; (tp = strnchr(lbp,len,'\n')) != np) {
	                rlen = intconv((tp + 1) - lbp) ;
		    } else {
	                rlen = len ;
		    }
	    	    if ((rs = u_read(fd,lbp,rlen)) > 0) {
			int ch ;
	    		len = rs ;
	    		tlen += len ;
	    		ch = mkchar(lbuf[tlen-1]) ;
	    		if ((ch == '\n') || (ch == '\0')) break ;
		    } else if (rs == 0) {
			break ;
		    }
	    } else if (rs == 0) {
		break ;
	    } /* end if (uc_recve) */
	} /* end while */
	return (rs >= 0) ? tlen : rs ;
}
/* end subroutine (readln_socket) */

local int readln_stream(int fd,char *lbuf,int llen,int to) noex {
	time_t		ti_now = time(nullptr) ;
	time_t		ti_start ;
	cnullptr	np{} ;
	int		rs = SR_OK ;
	int		events = POLLEVENTS ;
	int		rlen ;
	int		len ;
	int		ch ;
	int		tlen = 0 ;
	int		f_eof = false ;
	int		f_to = false ;
	char		cbuf[BUFLEN + 1] ;

	if (to < 0) to = INT_MAX ;

#ifdef	POLLRDNORM
	events |= POLLRDNORM ;
#endif
#ifdef	POLLRDBAND
	events |= POLLRDBAND ;
#endif
	POLLFD		fds[2] = {} ; {
	    fds[0].fd = fd ;
	    fds[0].events = short(events) ;
	    fds[1].fd = -1 ;
	    fds[1].events = 0 ;
	}
	STRPEEK		pd{} ; {
	    pd = {} ;
	    pd.flags = 0 ;
	    pd.ctlbuf.buf = cbuf ;
	    pd.ctlbuf.maxlen = BUFLEN ;
	}
	ti_start = ti_now ;
	while ((rs >= 0) && (tlen < llen) && (to >= 0)) {
	    pd.databuf.buf = (lbuf + tlen) ;
	    pd.databuf.maxlen = (llen - tlen) ;
	    if ((rs = u_poll(fds,1,POLL_INTMULT)) > 0) {
	        if ((rs = u_ioctl(fd,I_PEEK,&pd)) >= 0) {
		    cchar *sbuf = (lbuf + tlen) ;
	            len = pd.databuf.len ;
		    /* is there a NL present? */
	            if (cc *tp ; (tp = strnchr(sbuf,len,'\n')) != np) {
	                rlen = intconv((tp + 1) - (lbuf + tlen)) ;
		    } else {
			rlen = len ;
		    }
	            if ((rs = u_read(fd,(lbuf + tlen),rlen)) >= 0) {
	                len = rs ;
	                f_eof = (len == 0) ;
			if (len > 0) {
	                    tlen += len ;
	                    ch = mkchar(lbuf[tlen-1]) ;
	                    if ((ch == '\n') || (ch == '\0')) break ;
			} else {
			    break ;
			}
		    } /* end if (u_read) */
		} /* end if (u_ioctl) */
	    } else if ((rs == 0) && (to >= 0)) {
	        ti_now = time(nullptr) ;
	        f_to = ((ti_now - ti_start) >= to) ;
	        if (f_to) break ;
	    } else if (rs == SR_INTR) {
		rs = SR_OK ;
	    }
	} /* end while */
	if ((rs >= 0) && (tlen == 0) && f_to && (! f_eof) && (llen > 0)) {
	    rs = SR_TIMEDOUT ;
	} /* end if */
	return (rs >= 0) ? tlen : rs ;
}
/* end subroutine (readln_stream) */

local int readln_seekable(int fd,char *lbuf,int llen,int to,off_t fo) noex {
	int		rs ;
	(void) to ;
	if ((rs = u_pread(fd,lbuf,llen,fo)) > 0) {
	    int	rlen = rs ;
	    if (int si ; (si = sichr(lbuf,rlen,'\n')) > 0) {
		rlen = (si + 1) ;
	    }
	    rs = u_read(fd,lbuf,rlen) ;
	} /* end if (u_pread) */
	return rs ;
}
/* end subroutine (readln_seekable) */

local int readln_default(int fd,char *lbuf,int llen,int to) noex {
	cint		opts = (FM_TIMED | FM_EXACT) ;
	int		rs = SR_OK ;
	int		tlen = 0 ; /* return-value */
	while ((rs >= 0) && (tlen < llen)) {
	    if ((rs = uc_reade(fd,(lbuf + tlen),1,to,opts)) >= 0) {
	        if (cint len = rs ; len > 0) {
		    int	ch ;
	            tlen += len ;
	            ch = mkchar(lbuf[tlen - 1]) ;
	            if ((ch == '\n') || (ch == '\0')) break ;
	        } else {
		    break ;
	        }
	    } /* end if (uc_reade) */
	} /* end while */
	return (rs >= 0) ? tlen : rs ;
}
/* end subroutine (readln_default) */

local int isseekable(int fd,off_t *fop) noex {
	int		rs ;
	int		f = false ;
	if ((rs = u_seeko(fd,0z,SEEK_CUR,fop)) >= 0) {
	    f = true ;
	} else if (rs == SR_SPIPE) {
	    rs = SR_OK ;
	}
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (isseekable) */


