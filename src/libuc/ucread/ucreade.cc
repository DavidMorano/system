/* uc_reade SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* extended read */

#define	CF_NONBLOCK	1		/* use nonblocking mode */

/* revision history:

	= 1998-03-26, David A­D­ Morano
	This was first written to give a little bit to UNIX® what
	we have in our own circuit pack OSes!

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	uc_reade
	uc_read

	Description:
	Get some amount of data and time it also so that we can
	abort if it times out.

	Synopsis:
	int uc_reade(int fd,void *rbuf,int rlen,int to,int opts) noex
	int uc_read(int fd,void *vbuf,int ulen) noex

	Arguments:
	fd		file descriptor
	rbuf		user buffer to receive daa
	rlen		maximum amount of data the user wants
	to		time in seconds to wait
	opts		user options for time-out handling

	Returns:
	>=0		amount of data returned
	<0		error code (system-return)

	= The question:

	What do we want to return on a timeout? This is the big
	unanswered question of the ages? Do we want to treat the
	input FD like a STREAM or a SOCKET (returning SR_AGAIN) or
	do we want to treat it like a FIFO or TERMINAL (returning
	SR_OK == 0)? We will let this be determined by the caller
	by setting (or not setting) 'FM_AGAIN' in the options!

	If the caller sets 'FM_AGAIN' in the options, we return
	SR_AGAIN if there is no data (it timed out). If the caller
	sets 'FM_TIMED', then we return SR_TIMEDOUT if it times
	out. Finally, if the caller doesn't set that, we will return
	the amount of data received at the time of the timeout
	(which can inlucde the value ZERO).

	An explicit read of 0 bytes (EOF) always return 0 (EOF).
	If FM_EXACT was specified and the requested number of bytes
	has not yet arrived an EOF will be ignored and an attempt
	will be made to read more data in. Also if FM_EXACT is
	specified and the required number of bytes has not arrived
	(but some have), we continue reading until the required
	number of bytes arrives of if a time-out occurs.

	Read sematics are as follow:

	1. The default semantic (neither "FM_AGAIN" nor "FM_TIMED"):

	+ If a non-negative timeout value is given and the timeout
	occurs when no data has arrived, then we return ZERO.

	+ If a non-negative timeout value is given and the timeout
	occurs when some data has arrived, then we return the amount
	of data received.

	2. The "FM_AGAIN" semantic:

	+ If a non-negative timeout value is given and the timeout
	occurs when no data has arrived, then we return SR_AGAIN.

	+ If a non-negative timeout value is given and the timeout
	occurs when some data has arrived, then we return the amount
	of data received.

	3. The "FM_TIMED" semantic:

	+ If a non-negative timeout value is given and the timeout
	occurs when no data has arrived, then we return SR_TIMEDOUT.

	+ If a non-negative timeout value is given and the timeout
	occurs when some data has arrived, then we return the amount
	of data received.

	= Some notes:

	Watch out for receiving hang-ups! This can happen when the
	file-descriptor used for these reads is also used for some
	writes elsewhere. How should we handle a hang-up? That is
	a good question.  Since input is not supposed to be affected
	by a hang-up, we just continue on a hang-up unless there
	was data read. If we get a hang-up and no data was read,
	then it is an EOF condition and we will return as if we
	received an EOF (like we assume that we must have).

	= The observation:

	Is it poossible to receive an EOF condition on the input
	*without* receiving a POLLIN? Amazingly, the answer is YES!
	If a hang-up is present on the input, then a hang-up condition
	will be received (POLLHUP) *rather* than a usual EOF
	condition. Amazing as it is, this is possible. In my opinion,
	this should not be possible (an EOF should always create a
	POLLIN) but believe it or not that is not what happens in
	real life.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/uio.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<poll.h>
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<localmisc.h>

#include	"ucread.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#ifndef	POLL_INTMULT
#define	POLL_INTMULT	1000		/* poll-time multiplier */
#endif

#ifndef	POLLTIMEINT
#define	POLLTIMEINT	10		/* seconds */
#endif

#define	EBUFLEN		100

#define	MAXEOF		3

#define	POLLEVENTS	(POLLIN | POLLPRI)

#define	SI		subinfo
#define	SI_FL		subinfo_flags


/* imported namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int uc_nonblock(int,int) noex ;
}


/* external variables */


/* local structures */

struct subinfo_flags {
	uint		again:1 ;
	uint		timed:1 ;
	uint		exact:1 ;
	uint		timeint:1 ;
	uint		nonblock:1 ;
	uint		isfifo:1 ;
	uint		ischar:1 ;
	uint		isdir:1 ;
	uint		isblock:1 ;
	uint		isreg:1 ;
	uint		issocket:1 ;
	uint		isother:1 ;
	uint		isnonblock:1 ; /* was non-blocking */
} ; /* end struct (subinfo_flags) */

struct subinfo {
	char		*ubuf ;
	char		*bp ;
	SI_FL		fl ;
	int		fd ;
	int		ulen ;
	int		uto ;
	int		tlen ;
	int		to ;		/* down-counter */
	int		opts ;
	int		neof ;
	int		maxeof ;
} ; /* end struct (subinfo) */


/* forward references */

local int	subinfo_start(SI *,int,char *,int,int,int) noex ;
local int	subinfo_finish(SI *) noex ;
local int	subinfo_setmode(SI *,mode_t) noex ;
local int	subinfo_readreg(SI *) noex ;
local int	subinfo_readslow(SI *) noex ;
local int	subinfo_readpoll(SI *) noex ;

#if	CF_DEBUGS
static char	*d_reventstr() noex ;
#endif


/* local variables */


/* exported variables */


/* exported subroutines */

int uc_reade(int fd,void *vbuf,int ulen,int to,int opts) noex {
	int		rs = SR_FAULT ;
	int		tlen = 0 ; /* return-value */
	if (to < 0) to = INT_MAX ;
	if (vbuf) ylikely {
	    rs = SR_BADF ;
	    if (fd >= 0) ylikely {
	        char	*ubuf = (char *) vbuf ;
	        SI	si, *sip = &si ;
	        if ((rs = subinfo_start(sip,fd,ubuf,ulen,to,opts)) >= 0) {
	            bool f = false ;
	            f = f || sip->fl.isdir || sip->fl.isblock ;
	            f = f || sip->fl.isreg || sip->fl.isnonblock ;
	            if (f) {
	                rs = subinfo_readreg(sip) ;
	            } else {
	                rs = subinfo_readslow(sip) ;
	            } /* end if */
	            tlen = subinfo_finish(sip) ;
	            if (rs >= 0) rs = tlen ;
	        } /* end if (subinfo) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? tlen : rs ;
}
/* end subroutine (uc_reade) */

int uc_read(int fd,void *vbuf,int ulen) noex {
	return uc_reade(fd,vbuf,ulen,-1,0) ;
} /* end subroutine (uc_read) */


/* local subroutines */

local int subinfo_start(SI *sip,int fd,char *ubuf,int ulen,int to,int ro) noex {
	int		rs ;
	int		rs1 ;
	{
	    memclear(sip,szof(SI)) ;
	    sip->fd = fd ;
	    sip->ubuf = ubuf ;
	    sip->bp = ubuf ;
	    sip->ulen = ulen ;
	    sip->uto = to ;
	    sip->to = to ;
	    sip->opts = ro ;
	}
	{
	    sip->fl.again = MKBOOL(ro & FM_AGAIN) ;
	    sip->fl.timed = MKBOOL(ro & FM_TIMED) ;
	    sip->fl.timeint = MKBOOL(ro & FM_TIMEINT) ;
	    sip->fl.exact = MKBOOL(ro & FM_EXACT) ;
	}
	if (ustat sb ; (rs = u_fstat(fd,&sb)) >= 0) ylikely {
	    if ((rs = subinfo_setmode(sip,sb.st_mode)) >= 0) {
		if (sip->fl.isother) {

		/* yes! some files do *not* support non-blocking mode */

#if	CF_NONBLOCK
	    if (! sip->fl.isreg) {
	        sip->fl.nonblock = true ;
	        rs1 = uc_nonblock(fd,true) ;
	        sip->fl.isnonblock = (rs1 > 0) ;
	        if (rs1 == SR_NOSYS) {
	            sip->fl.isnonblock = true ;
	        } else {
	            rs = rs1 ;
	        }

	    }
#endif /* CF_NONBLOCK */

	       } /* end if (other file type) */
	   } /* end if (subinfo_setmode) */
	} /* end if (stat) */

	return rs ;
}
/* end subroutine (subinfo_start) */

local int subinfo_finish(SI *sip) noex {
	int		rs = SR_OK ;
	int		tlen = sip->tlen ;
#if	CF_NONBLOCK
	if (sip->fl.nonblock)
	    rs = uc_nonblock(sip->fd,false) ;
#endif
	if ((rs >= 0) && (tlen == 0) && (sip->to == 0) && (sip->ulen > 0)) {
	    bool f = false ;
	    if (sip->fl.issocket) {
	        f = (sip->neof < sip->maxeof) ;
	    } else {
	        f = (sip->neof == 0) ;
	    }
	    if (f) {
	        if (sip->opts & FM_AGAIN) {
	            rs = SR_AGAIN ;
	        } else if (sip->opts & FM_TIMED) {
	            rs = SR_TIMEDOUT ;
	        }
	    }
	} /* end if (had a timeout) */
	return (rs >= 0) ? tlen : rs ;
}
/* end subroutine (subinfo_finish) */

local int subinfo_setmode(SI *sip,mode_t fm) noex {
	if (S_ISFIFO(fm)) {
	    sip->fl.isfifo = true ;
	} else if (S_ISCHR(fm)) {
	    sip->fl.ischar = true ;
	} else if (S_ISDIR(fm)) {
	    sip->fl.isdir = true ;
	} else if (S_ISBLK(fm)) {
	    sip->fl.isblock = true ;
	} else if (S_ISREG(fm)) {
	    sip->fl.isreg = true ;
	} else if (S_ISSOCK(fm)) {
	    sip->fl.issocket = true ;
	} else {
	    sip->fl.isother = true ;
	}
	return SR_OK ;
}
/* end subroutine (subinfo_setmode) */

local int subinfo_readreg(SI *sip) noex {
	int		rs ;
	int		tlen = 0 ; /* return-value */
	sip->maxeof = 0 ;
	if ((rs = u_read(sip->fd,sip->ubuf,sip->ulen)) >= 0) ylikely {
	    tlen = rs ;
	    if (tlen > 0) {
	        sip->tlen += tlen ;
	        sip->neof = 0 ;
	    } else {
	        sip->neof += 1 ;
	    }
	} /* end if (u_read) */
	return (rs >= 0) ? tlen : rs ;
}
/* end subroutine (subinfo_readreg) */

local int subinfo_readslow(SI *sip) noex {
	int		rs = SR_OK ;
	int		events = POLLEVENTS ;

	sip->maxeof = MAXEOF ;

#if	defined(POLLRDNORM)
	events |= POLLRDNORM ;
#endif
#if	defined(POLLRDBAND)
	events |= POLLRDBAND ;
#endif

/* initialization for 'u_poll(2u)' */

	POLLFD		fds[2]  = {} ; {
	    fds[0].fd = sip->fd ;
	    fds[0].events = short(events) ;
	    fds[1].fd = -1 ;
	}
	/* go */
	while ((rs >= 0) && ((sip->ulen - sip->tlen) > 0)) {
	    int	f_break = false ;
	    if ((rs = u_poll(fds,1,POLL_INTMULT)) > 0) {
	        cint	re = fds[0].revents ;
	        if ((re & POLLIN) || (re & POLLPRI)) {
	            rs = subinfo_readpoll(sip) ;
	            f_break = (rs > 0) ;
	        } else if (re & POLLNVAL) {
	            rs = SR_NOTOPEN ;
	        } else if (re & POLLERR) {
	            rs = SR_POLLERR ;
	        } else if (re & POLLHUP) {
	            if (sip->tlen == 0) break ;
	            msleep(1) ;
	        }
	    } else if (rs == SR_INTR) {
	        rs = SR_OK ;
	    } else if (rs == 0) { /* u_poll() returned w/ nothing */
	        if (sip->to > 0) {
	            sip->to -= 1 ;
	        } else {
	            f_break = true ;
	        }
	    } /* end if (otherwise it must be an error) */
	    if (sip->fl.isnonblock) break ;
	    if (f_break) break ;
	} /* end while (looping on poll) */
	return rs ;
}
/* end subroutine (subinfo_readslow) */

local int subinfo_readpoll(SI *sip) noex {
	int		rs ;
	int		rlen ;
	int		f_break = false ;
	rlen = (sip->ulen - sip->tlen) ;
	if ((rs = u_read(sip->fd,sip->bp,rlen)) >= 0) {
	    cint len = rs ;
	    if (len == 0) {
	        sip->neof += 1 ;
	        if ((! sip->fl.issocket) || (sip->neof >= sip->maxeof)) {
	            f_break = true ;
	        }
	    } else {
	        sip->neof = 0 ;		/* reset */
	    }
	    sip->tlen += len ;
	    sip->bp += len ;
	    if ((! f_break) && (len > 0) && (! sip->fl.exact)) {
	        f_break = true ;
	    }
	    if ((! f_break) && (len > 0) && sip->fl.timeint) {
	        sip->to = sip->uto ;	/* reset */
	    }
	} else if (rs == SR_AGAIN) {
	    if (! sip->fl.isnonblock) rs = SR_OK ;
	}
	return (rs >= 0) ? f_break : rs ;
}
/* end subroutine (subinfo_readpoll) */


