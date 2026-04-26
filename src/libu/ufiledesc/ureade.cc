/* ureade SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* extended read */

#define	CF_DEBUG	0		/* debugging */
#define	CF_REVENTS	0
#define	CF_NONBLOCK	0		/* use nonblocking mode */

/* revision history:

	= 1998-03-26, David A­D­ Morano
	This was first written to give a little bit to UNIX® what
	we have in our own circuit pack OSes!

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	u_reade

	Description:
	Get some amount of data and time it also so that we can
	abort (return early) if it times out.

	Synopsis:
	int u_reade(int fd,void *rbuf,int rlen,int to,int opts) noex

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

	What do we want to return on a timeout?  This is the big
	unanswered question of the ages?  Do we want to treat the
	input FD like a STREAM or a SOCKET (returning SR_AGAIN) or
	do we want to treat it like a FIFO or TERMINAL (returning
	SR_OK == 0)?  We will let this be determined by the caller
	by setting (or not setting) 'FM_AGAIN' in the options!

	If the caller sets 'FM_AGAIN' in the options, we return
	SR_AGAIN if there is no data (it timed out).  If the caller
	sets 'FM_TIMED', then we return SR_TIMEDOUT if it times
	out.  Finally, if the caller does not set that, we will
	return the amount of data received at the time of the timeout
	(which can inlucde the value ZERO).

	An explicit read of 0 bytes (EOF) always return 0 (EOF).
	If FM_EXACT was specified and the requested number of bytes
	has not yet arrived an EOF will be ignored and an attempt
	will be made to read more data in.  Also if FM_EXACT is
	specified and the required number of bytes has not arrived
	(but some have), we continue reading until the required
	number of bytes arrives or a time-out occurs.

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

	Watch out for receiving hang-ups!  This can happen when the
	file-descriptor used for these reads is also used for some
	writes elsewhere.  How should we handle a hang-up?  That is
	a good question.  Since input is not supposed to be affected
	by a hang-up, we just continue on a hang-up unless there
	was data read.  If we get a hang-up and no data was read,
	then it is an EOF condition and we will return as if we
	received an EOF (like we assume that we must have).

	= The observation:

	Is it poossible to receive an EOF condition on the input
	*without* receiving a POLLIN?  Amazingly, the answer is
	YES!  If a hang-up is present on the input, then a hang-up
	condition will be received (POLLHUP) *rather* than a usual
	EOF condition.  Amazing as it is, this is possible.  In my
	opinion, this should not be possible (an EOF should always
	create a POLLIN) but believe it or not that is not what
	happens in real life.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<termios.h>
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>
#include	<cstdlib>
#include	<string>
#include	<string_view>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<funcodes.h>
#include	<filetypes.h>
#include	<timeval.hh>
#include	<strnul.hh>
#include	<ascii.h>
#include	<localmisc.h>
#include	<dprint.hh>		/* debugging */

#include	"ureade.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#ifndef	POLL_INTMULT
#define	POLL_INTMULT	1000		/* poll-time multiplier */
#endif

#define	TERMIOS_MINCHARS	0	/* minimum characters to get */
#define	TERMIOS_MINTIME		5	/* minimum time (x100 milliseconds) */

#define	MAXEOF		3

#define	POLLEVENTS	(POLLIN | POLLPRI)

#define	SI		subinfo

#ifndef	CF_DEBUG
#define	CF_DEBUG	0
#endif


/* imported namespaces */

using std::string_view ;		/* type */
using libu::upoll ;			/* subroutine */
using libu::uselect ;			/* subroutine */
using libu::uterminal ;			/* subroutine */
using libu::utermattrget ;		/* subroutine */
using libu::utermattrset ;		/* subroutine */
using libu::uread ;			/* subroutine */
using libu::sichr ;			/* subroutine */


/* local typedefs */

typedef string_view	strview ;


/* external subroutines */

extern "C" {
    extern int u_nonblock(int,int) noex ;
}


/* external variables */


/* local structures */

namespace {
   struct subinfo_ft {
	uint		fifo:1 ;
	uint		chr:1 ;
	uint		dir:1 ;
	uint		block:1 ;
	uint		reg:1 ;
	uint		socket:1 ;
	uint		other:1 ;
   } ; /* end struct (subinfi_dt) */
   struct subinfo_fl {
	uint		again:1 ;
	uint		timed:1 ;
	uint		exact:1 ;
	uint		timeint:1 ;
	uint		terminal:1 ;	/* operating on a terminal */
	uint		nonblock:1 ;
	uint		isnonblock:1 ;	/* was non-blocking */
	uint		done:1 ;
    } ; /* end struct (subinfo_flags) */
    struct subinfo {
	char		*ubuf ;		/* caller argument */
	char		*bp ;
	TERMIOS		attrs ;
	subinfo_fl	fl ;
	subinfo_ft	ft ;
	int		fd ;		/* caller argument */
	int		ulen ;		/* caller argument */
	int		uto ;		/* caller argument */
	int		tlen ;
	int		toc ;		/* down-counter */
	int		opts ;		/* caller argument */
	int		neof ;
	int		maxeof ;
	subinfo() noex {
	    fl = {} ;
	    ft = {} ;
	    ubuf = nullptr ;
	    bp = nullptr ;
	    ulen = 0 ;
	    tlen = 0 ;
	} ;
	int start	(int,char *,int,int,int) noex ;
	int finish	() noex ;
	int choose	() noex ;
	int setmode	(mode_t) noex ;
	int readwait	() noex ;
	int readreg	() noex ;
	int readslow	() noex ;
	int readchr	() noex ;
	int readterm	() noex ;
	int readafter	() noex ;
	int attrbegin	() noex ;
	int attrend	() noex ;
	int mkret	() noex ;
    } ; /* end struct (subinfo) */
} /* end nameapace */


/* forward references */

#if	CF_DEBUG && CF_REVENTS
local char	*d_reventstr() noex ;
#endif


/* local variables */

cint		nfds = 1 ;
cbool		f_debug		= CF_DEBUG ;
cbool		f_nonblock	= CF_NONBLOCK ;


/* exported variables */


/* exported subroutines */

namespace libu {
    int ureade(int fd,void *vbuf,int ulen,int to,int opts) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		tlen = 0 ; /* return-value */
	char		*ubuf = charp(vbuf) ;
	if (SI si ; (rs = si.start(fd,ubuf,ulen,to,opts)) >= 0) {
	    {
		rs = si.choose() ;
		tlen = rs ;
	    }
	    rs1 = si.finish() ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (subinfo) */
	return (rs >= 0) ? tlen : rs ;
    } /* end subroutine (ureade) */
} /* end namespace (libu) */

int u_reade(int fd,void *vbuf,int ulen,int to,int opts) noex {
    	using		libu::ureade ;
	int		rs = SR_FAULT ;
	int		tlen = 0 ; /* return-value */
	DPRINTF("ent fd=%d to=%d\n",fd,to) ;
	if (vbuf) ylikely {
	    rs = SR_BADFD ;
	    if (fd >= 0) ylikely {
		rs = ureade(fd,vbuf,ulen,to,opts) ;
		tlen = rs ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	DPRINTF("ret rs=%d tlen=%d\n",rs,tlen) ;
	return (rs >= 0) ? tlen : rs ;
} /* end subroutine (u_reade) */


/* local subroutines */

int subinfo::start(int ªfd,char *ªubuf,int ªulen,int ªto,int ªro) noex {
	int		rs ;
	if (ªto < 0) ªto = INT_MAX ;
	{
	    fd		= ªfd ;
	    ubuf	= ªubuf ;
	    bp		= ªubuf ;
	    ulen	= ªulen ;
	    uto		= ªto ;
	    toc		= ªto ;
	    opts	= ªro ;
	}
	{
	    fl.again	= !!(ªro & FM_AGAIN) ;
	    fl.timed	= !!(ªro & FM_TIMED) ;
	    fl.timeint	= !!(ªro & FM_TIMEINT) ;
	    fl.exact	= !!(ªro & FM_EXACT) ;
	}
	if (ustat sb ; (rs = u_fstat(fd,&sb)) >= 0) ylikely {
	    if ((rs = setmode(sb.st_mode)) >= 0) {
		if (ft.other) {
		    /* yes! some files do *not* support non-blocking mode */
		    if_constexpr (f_nonblock) {
	                if (! ft.reg) {
	                    fl.nonblock = true ;
	                    if ((rs = u_nonblock(fd,true)) >= 0) {
	                        fl.isnonblock = true ;
			    } else if (rs == SR_NOSYS) {
				rs = SR_OK ;
	                        fl.isnonblock = false ;
	                    }
			} /* end if (isreg) */
	    	    } /* end if_constexpr (f_nonblock) */
	       } /* end if (other file type) */
	   } /* end if (subinfo_setmode) */
	} /* end if (stat) */
	return rs ;
} /* end subroutine (subinfo::start) */

int subinfo::finish() noex {
	int		rs = SR_OK ;
	if_constexpr (f_nonblock) {
	    if (fl.nonblock) {
	        rs = u_nonblock(fd,false) ;
	    }
	} /* end if_constexpr (f_debug) */
	return rs ;
} /* end subroutine (subinfo::finish) */

int subinfo::choose() noex {
    	int		rs = SR_OK ;
	bool f = false ;
	DPRINTF("ent fd=%d\n",fd) ;
	f = f || ft.dir || ft.block ;
	f = f || ft.reg || fl.nonblock ;
	DPRINTF("strategy f=%u\n",f) ;
	if (f) {
	    rs = readreg() ;
	} else if (ft.chr && ((rs = uterminal(fd)) > 0)) {
	    rs = readterm() ;
	} else if (rs >= 0) {
	    if (ft.chr) {
	        rs = readchr() ;
	    } else {
	        rs = readslow() ;
	    }
	} /* end if */
	if (rs >= 0) {
	    rs = mkret() ;
	}
	DPRINTF("ret rs=%d tlen=%d\n",rs,tlen) ;
	return (rs >= 0) ? tlen : rs ;
} /* end method (subinfo::choose) */

int subinfo::mkret() noex {
    	int		rs = SR_OK ;
	if (tlen == 0) {
	    if (fl.timed) {
		rs = SR_TIMEDOUT ;
	    }
	}
	return rs ;
} /* end method (subinfo::mkret) */

int subinfo::setmode(mode_t fm) noex {
    	if_constexpr (f_debug) {
	    filetypes dt = filetype(fm) ;
	    {
		cc *ftn = filetypes_names[dt] ;
	        DPRINTF("ftn=%s\n",ftn) ;
	    }
	} /* end if_constexpr (f_debug) */
	if (S_ISFIFO(fm)) {
	    ft.fifo = true ;
	} else if (S_ISCHR(fm)) {
	    ft.chr = true ;
	} else if (S_ISDIR(fm)) {
	    ft.dir = true ;
	} else if (S_ISBLK(fm)) {
	    ft.block = true ;
	} else if (S_ISREG(fm)) {
	    ft.reg = true ;
	} else if (S_ISSOCK(fm)) {
	    ft.socket = true ;
	} else {
	    ft.other = true ;
	}
	return SR_OK ;
} /* end subroutine (subinfo::setmode) */

int subinfo::readreg() noex {
	int		rs ;
	int		wlen = 0 ; /* return-value */
	maxeof = 0 ;
	if ((rs = uread(fd,ubuf,ulen)) >= 0) ylikely {
	    wlen = rs ;
	    if (wlen > 0) {
	        tlen += wlen ;
	        neof = 0 ;
	    } else {
	        neof += 1 ;
	    }
	} /* end if (uread) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (subinfo::readreg) */

int subinfo::readterm() noex {
    	int		rs = SR_OK ;
	int		rs1 ;
	DPRINTF("ent fd=%d\n",fd) ;
	fl.terminal = true ;
	if ((rs = attrbegin()) >= 0) {
	    while ((rs >= 0) && ((ulen - tlen) > 0)) {
		if ((rs = readwait()) > 0) {
	            rs = readafter() ;
	        } else if (rs == SR_INTR) {
	            DPRINTF("select intr\n") ;
	            rs = SR_OK ;
	        } else if (rs == 0) { /* u_poll() returned w/ nothing */
	            DPRINTF("select zero\n") ;
		    if (--toc == 0) break ;
		} /* end if (readwait) */
		if (fl.done) break ;
	    } /* end while */
	    rs1 = attrend() ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (terminal-attributes) */
	DPRINTF("ret rs=%d tlen=%d\n",rs,tlen) ;
	return rs ;
} /* end subroutine (subinfo::readterm) */

int subinfo::readchr() noex {
    	int		rs = SR_OK ;
	DPRINTF("ent fd=%d\n",fd) ;
	    while ((rs >= 0) && ((ulen - tlen) > 0)) {
		if ((rs = readwait()) > 0) {
	            rs = readafter() ;
	        } else if (rs == SR_INTR) {
	            DPRINTF("select intr\n") ;
	            rs = SR_OK ;
	        } else if (rs == 0) { /* u_poll() returned w/ nothing */
	            DPRINTF("select zero\n") ;
		    if (--toc == 0) break ;
		} /* end if (readwait) */
		if (fl.done) break ;
	    } /* end while */
	DPRINTF("ret rs=%d tlen=%d\n",rs,tlen) ;
	return rs ;
} /* end subroutine (subinfo::readchr) */

int subinfo::readslow() noex {
	int		rs = SR_OK ;
	int		events = POLLEVENTS ;
	DPRINTF("ent fd=%d\n",fd) ;
	maxeof = MAXEOF ;
	events |= POLLRDNORM ;
	events |= POLLRDBAND ;
	/* initialization for |u_poll(3u)| */
	POLLFD		fds[nfds] = {} ; {
	    fds[0].fd = fd ;
	    fds[0].events = short(events) ;
	    fds[1].fd = -1 ;
	}
	/* go */
	while ((rs >= 0) && ((ulen - tlen) > 0)) {
	    if ((rs = upoll(fds,nfds,POLL_INTMULT)) > 0) {
	        cint	re = fds[0].revents ;
	        DPRINTF("poll got rs=%d re=%08X\n",rs,re) ;
	        if ((re & POLLIN) || (re & POLLPRI)) {
	            rs = readafter() ;
	        } else if (re & POLLNVAL) {
	            rs = SR_NOTOPEN ;
	        } else if (re & POLLERR) {
	            rs = SR_POLLERR ;
	        } else if (re & POLLHUP) {
	            if (tlen == 0) break ;
	            msleep(1) ;
	        }
	    } else if (rs == SR_INTR) {
	        DPRINTF("poll intr\n") ;
	        rs = SR_OK ;
	    } else if (rs == 0) { /* u_poll() returned w/ nothing */
	        DPRINTF("poll zero\n") ;
		if (--toc == 0) break ;
	    } /* end if (otherwise it must be an error) */
	    DPRINTF("poll-out rs=%d\n",rs) ;
	    if (fl.done) break ;
	} /* end while (looping on poll) */
	DPRINTF("ret rs=%d tlen=%d\n",rs,tlen) ;
	return rs ;
} /* end subroutine (subinfo::readslow) */

int subinfo::readwait() noex {
    	timeval_t	tv(1) ;
    	cnullptr	np{} ;
    	cint		n = (fd + 1) ;
	int		rs = SR_OK ;
	fdset		ifds{} ;
	DPRINTF("ent fd=%d uto=%d\n",fd,uto) ;
	FD_SET(fd,&ifds) ;
	{
	    rs = uselect(n,&ifds,np,np,&tv) ;
	}
	DPRINTF("ret rs=%d tlen=%d\n",rs,tlen) ;
	return rs ;
} /* end subroutine (subinfo::readwait) */

int subinfo::readafter() noex {
	int		rs ;
	int		len = 0 ; /* return-value */
	bool		fdone = false ;
	DPRINTF("ent\n") ;
	neof = 0 ;		/* reset */
	while ((! fdone) && (rs >= 0)) {
	    cint bl = (ulen - tlen) ;
	    if ((rs = uread(fd,bp,bl)) > 0) {
	       len = rs ;
	       {
		   strnul ps(bp,len) ;
	           DPRINTF("uead() rs=%d data=>%s<\n",rs,ccp(ps)) ;
	       }
	       if (int si = sichr(bp,len,CH_CR) ; si >= 0) {
		   bp[si] = CH_NL ;
		   fl.done = true ;
		   fdone = true ;
	       }
	       tlen += len ;
	       bp += len ;
	       fdone = true ;
	    } else if (rs == 0) {
	        neof += 1 ;
	        if ((! ft.socket) || (neof >= maxeof)) {
	            fdone = true ;
	        }
	    } else if (rs == SR_AGAIN) {
	        if (! fl.isnonblock) rs = SR_OK ;
	    }
	} /* end while */
	DPRINTF("ret rs=%d fdone=%u\n",rs,fdone) ;
	return (rs >= 0) ? len : rs ;
} /* end subroutine (subinfo::readafter) */

int subinfo::attrbegin() noex {
	int		rs ;
	if ((rs = utermattrget(fd,&attrs)) >= 0) {
	    TERMIOS	attrnew = attrs ;
	    TERMIOS	*tp = &attrnew ;
	    tp->c_iflag &= 
	        (~ (INLCR | ICRNL | IXANY | ISTRIP | INPCK | PARMRK)) ;
	    tp->c_iflag |= IXON ;
	    tp->c_cflag &= (~ (CSIZE)) ;
	    tp->c_cflag |= CS8 ;
	    tp->c_lflag &= (~ (ICANON | ECHO | ECHOE | ECHOK | ECHONL)) ;
#if	CF_SIGNAL
	    tp->c_lflag &= (~ (ISIG)) ;
#endif
	    tp->c_oflag &= (~ (OCRNL | ONOCR | ONLRET)) ;
	    tp->c_cc[VMIN]	= TERMIOS_MINCHARS ;
	    tp->c_cc[VTIME]	= TERMIOS_MINTIME ;
	    tp->c_cc[VINTR]	= CH_ETX ;	/* Control-C */
	    tp->c_cc[VQUIT]	= CH_EM ;	/* Control-Y */
	    tp->c_cc[VERASE]	= CH_DEL ;	/* Delete */
	    tp->c_cc[VKILL]	= CH_NAK ;	/* Control-U */
	    tp->c_cc[VSTART]	= CH_DC1 ;	/* Control-Q */
	    tp->c_cc[VSTOP]	= CH_DC3 ;	/* Control-S */
	    tp->c_cc[VSUSP]	= CH_SUB ;	/* Control-Z */
	    tp->c_cc[VREPRINT]	= CH_DC2 ;	/* Control-R */
	    tp->c_cc[VDISCARD]	= CH_SO ;	/* Control-O */
	    /* set the new attributes */
	    rs = utermattrset(fd,TCSADRAIN,tp) ;
	    if (rs < 0) {
	    	utermattrset(fd,TCSADRAIN,&attrs) ;
	    } /* end if (error) */
	} /* end if */
	return rs ;
} /* end method (subinfo::attrbegin) */

int subinfo::attrend() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    rs1 = utermattrset(fd,TCSADRAIN,&attrs) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
} /* end method (subinfo::attrend) */


