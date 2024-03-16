/* ucread SUPPORT */
/* lang=C++20 */

/* read a line from a file descriptor but time it */
/* version %I% last-modified %G% */

#define	CF_STREAM	1		/* optimization for STREAM */

/* revision history:

	= 1998-03-26, David A�D� Morano
	This was first written to give a little bit to UNIX� what
	we have in our own circuit pack OSes! Note that this
	subroutine depends on another little subroutine (|uc_reade(3uc)|)
	that is used to provide an underlying extended |read(2)|
	like capability.

	= 2003-11-25, David A�D� Morano
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

/* Copyright � 1998,2003 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	uc-readlnto

	Description:
	Get a line code amount of data (data ending in an NL) and
	time it also so that we can abort if it times out.

	Synopsis:
	int uc_readlnto(kint fd,char *lbuf,int llen,int to) noex

	Arguments:
	fd		file descriptor
	lbuf		user buffer to receive daa
	llen		maximum amount of data the user wants
	to		time in seconds to wait

	Returns:
	>=0		amount of data returned
	<0		error (system-return)

	Note 2003-11-25, David A�D� Morano
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

#if	(! defined(SYSHAS_STREAMS)) || (SYSHAS_STREAMS == 0)
#undef	CF_STREAM
#define	CF_STREAM	0
#endif

#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/socket.h>
#include	<climits>
#include	<unistd.h>
#include	<fcntl.h>
#include	<time.h>
#include	<string.h>

#if	CF_STREAM
#endif

#include	<usystem.h>
#include	<six.h>
#include	<strn.h>
#include	<mkchar.h>
#include	<localmisc.h>


/* local defines */

#define	BUFLEN		2048

#ifndef	POLLINTMULT
#define	POLLINTMULT	1000
#endif

#define	POLLEVENTS	(POLLIN | POLLPRI)


/* external subroutines */

extern "C" {
    extern int	isasocket(int) noex ;
}


/* local structures */


/* forward references */

static int	readline_socket(int,char *,int,int) noex ;
static int	readline_stream(int,char *,int,int) noex ;
static int	readline_seekable(int,char *,int,int,off_t) noex ;
static int	readline_default(int,char *,int,int) noex ;

static int 	isseekable(int,off_t *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_readlnto(int fd,char *lbuf,int llen,int to) noex {
	int		rs = SR_FAULT ;
	if (lbuf) {
	    rs = SR_BADF ;
	    if (fd >= 0) {
	        rs = SR_INVALID ;
	        if (llen > 0) {
	            off_t	fo{} ;
	            if (isasocket(fd)) {
	               rs = readline_socket(fd,lbuf,llen,to) ;
	            } else if (isastream(fd)) {
	                rs = readline_stream(fd,lbuf,llen,to) ;
	            } else if (isseekable(fd,&fo)) {
	                rs = readline_seekable(fd,lbuf,llen,to,fo) ;
	            } else {
	                rs = readline_default(fd,lbuf,llen,to) ;
	            }
	        } /* end if (positive) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_readlnto) */

int uc_readln(int fd,char *lbuf,int llen) noex {
	return uc_readlinetimed(fd,lbuf,llen,-1) ;
}
/* end subroutine (uc_readln) */


/* private subroutines */

static int readline_socket(int fd,char *lbuf,int llen,int to) noex {
	int		rs = SR_OK ;
	int		mopts = MSG_PEEK ;
	int		opts = (FM_TIMED | FM_EXACT) ;
	int		rlen ;
	int		len ;
	int		ch ;
	int		lbl ;
	int		tlen = 0 ;
	cchar	*tp ;
	char		*lbp ;

	while ((rs >= 0) && (tlen < llen)) {
	    lbp = (lbuf + tlen) ;
	    lbl = (llen - tlen) ;
	    if ((rs = uc_recve(fd,lbp,lbl,mopts,to,opts)) >= 0) {
	        len = rs ;
		if (len > 0) {
	            tp = strnchr(lbp,len,'\n') ; /* NL present? */
	            rlen = (tp != NULL) ? ((tp+1)-lbp) : len ;
	    	    if ((rs = u_read(fd,lbp,rlen)) > 0) {
	    		len = rs ;
	    		tlen += len ;
	    		ch = mkchar(lbuf[tlen-1]) ;
	    		if ((ch == '\n') || (ch == '\0')) break ;
		    } else if (rs == 0) {
			break ;
		    }
		} else if (len == 0) {
		    break ;
		}
	    } /* end if (uc_recve) */
	} /* end while */

	return (rs >= 0) ? tlen : rs ;
}
/* end subroutine (readline_socket) */


static int readline_stream(int fd,char *lbuf,int llen,int to) noex {
	POLLFD		fds[2] ;
	STRPEEK		pd ;
	time_t		ti_now = time(NULL) ;
	time_t		ti_start ;
	int		rs = SR_OK ;
	int		events = POLLEVENTS ;
	int		rlen ;
	int		len ;
	int		ch ;
	int		tlen = 0 ;
	int		f_eof = false ;
	int		f_to = false ;
	cchar		*tp ;
	char		cbuf[BUFLEN + 1] ;

	if (to < 0) to = INT_MAX ;

#ifdef	POLLRDNORM
	events |= POLLRDNORM ;
#endif
#ifdef	POLLRDBAND
	events |= POLLRDBAND ;
#endif

	memset(fds,0,sizeof(fds)) ;
	fds[0].fd = fd ;
	fds[0].events = events ;
	fds[1].fd = -1 ;
	fds[1].events = 0 ;

	memset(&pd,0,sizeof(STRPEEK)) ;
	pd.flags = 0 ;
	pd.ctlbuf.buf = cbuf ;
	pd.ctlbuf.maxlen = BUFLEN ;
	ti_start = ti_now ;
	while ((rs >= 0) && (tlen < llen) && (to >= 0)) {
	    pd.databuf.buf = (lbuf + tlen) ;
	    pd.databuf.maxlen = (llen - tlen) ;
	    if ((rs = u_poll(fds,1,POLLINTMULT)) > 0) {
	        if ((rs = u_ioctl(fd,I_PEEK,&pd)) >= 0) {
	            len = pd.databuf.len ;
/* is there a NL present? */
	            tp = strnchr((lbuf + tlen),len,'\n') ;
	            rlen = (tp != NULL) ? ((tp + 1) - (lbuf + tlen)) : len ;
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
	        ti_now = time(NULL) ;
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
/* end subroutine (readline_stream) */

static int readline_seekable(int fd,char *lbuf,int llen,int to,off_t fo) noex {
	int		rs ;
	if ((rs = u_pread(fd,lbuf,llen,fo)) > 0) {
	    int	rlen = rs ;
	    int	si ;
	    if ((si = sichr(lbuf,rlen,'\n')) > 0) {
		rlen = (si+1) ;
	    }
	    rs = u_read(fd,lbuf,rlen) ;
	} /* end if (u_pread) */
	return rs ;
}
/* end subroutine (readline_seekable) */

static int readline_default(int fd,char *lbuf,int llen,int to) noex {
	int		rs = SR_OK ;
	int		opts = (FM_TIMED | FM_EXACT) ;
	int		len ;
	int		ch ;
	int		tlen = 0 ;
	while ((rs >= 0) && (tlen < llen)) {
	    if ((rs = uc_reade(fd,(lbuf + tlen),1,to,opts)) >= 0) {
	        len = rs ;
	        if (len > 0) {
	            tlen += len ;
	            ch = mkchar(lbuf[tlen-1]) ;
	            if ((ch == '\n') || (ch == '\0')) break ;
	        } else {
		    break ;
	        }
	    } /* end if (uc_reade) */
	} /* end while */
	return (rs >= 0) ? tlen : rs ;
}
/* end subroutine (readline_default) */

static int isseekable(int fd,off_t *fop) noex {
	int		rs ;
	int		f = false ;
	if ((rs = u_seeko(fd,0L,SEEK_CUR,fop)) >= 0) {
	    f = true ;
	} else if (rs == SR_SPIPE) {
	    rs = SR_OK ;
	}
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (isseekable) */


