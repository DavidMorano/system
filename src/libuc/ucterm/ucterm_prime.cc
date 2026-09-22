/* ucterm_prime SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* "UNIX Terminal" helper routines */
/* version %I% last-modified %G% */

#define	CF_FIRSTREAD	0	/* perform an initial |read()|? */
#define	CF_SUBUNIX	1	/* allow UNIX® to handle Control-Z */
#define	CF_SIGNAL	0	/* allow signals */

/* revision history:

	= 1998-02-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	ucterm

	Description:
	These routines provide a stand-alone terminal environment.

	History:
	This code is very old.  Although the code below is "officially"
	dated from 1998, in reality the code dates from 1984.  This
	code was taken from the driver code for the
	Multi-Function-Peripherial (MFP) used in the various computers
	architected and designed dating from 1984.  This code is
	essentially a UNIX® adapted version of the C-language version
	of MFP driver code written for the eXecutive-Debugging-Tool
	(XDT) ROM Debug-Monitor.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<termios.h>		/* POSIX */
#include	<unistd.h>		/* POSIX */
#include	<poll.h>		/* POSIX */
#include	<ctime>			/* CSTD */
#include	<climits>		/* CSTD |CHAR_UMAX| + |CHAR_BIT| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstdarg>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ascii.h>		/* LIBU */
#include	<baops.h>		/* LIBU */
#include	<uctc.h>		/* LIBUC terminal-control */
#include	<ucsysconf.h>		/* LIBUC */
#include	<ucsigx.h>		/* LIBUC */
#include	<ucdesc.h>		/* LIBUC */
#include	<charq.h>		/* LIBUC */
#include	<vecobj.h>		/* LIBUC */
#include	<buffer.h>		/* LIBUC */
#include	<sbuf.h>		/* LIBUC */
#include	<sigign.h>		/* LIBUC */
#include	<strn.h>		/* LIBUC */
#include	<ischarx.h>		/* LIBUC */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"ucterm.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	TTY_MINCHARS	0	/* minimum characters to get */
#define	TTY_MINTIME	5	/* minimum time (x100 milliseconds) */

#define	TTY_READCHARS	20

#define	WBUFLEN		40	/* maximum output write buffer */

#define	TO_HANGUP	3	/* the error monitoring interval */
#define	MAXLOOPS	300	/* maximum loops in monitor interval */

#define	TA_SIZE		256
#define	EC_SIZE		256

/* output priorities */

#define	OPV_WRITE	0
#define	OPV_READ	1
#define	OPV_ECHO	2

#define	OPM_WRITE	(1 << OPV_WRITE)
#define	OPM_READ	(1 << OPV_READ)
#define	OPM_ECHO	(1 << OPV_ECHO)
#define	OPM_ANY		(OPM_WRITE | OPM_READ | OPM_ECHO)

#undef	CH_FILTER
#define	CH_FILTER	0x100

#define	FILTER(c)	((c) | CH_FILTER)

#ifndef	CF_FIRSTREAD
#define	CF_FIRSTREAD	0	/* perform an initial |read()|? */
#endif
#ifndef	CF_SUBUNIX
#define	CF_SUBUNIX	1	/* allow UNIX® to handle Control-Z */
#endif
#ifndef	CF_SIGNAL
#define	CF_SIGNAL	0	/* allow signals */
#endif


/* externals subroutines */

extern "C" {
    extern int uc_fuid(int) noex ;
} /* end extern (C) */

extern "C" {
    extern int	tcsetmesg(int,int) noex ;
    extern int	tcsetbiff(int,int) noex ;
    extern int	tcgetlines(int) noex ;
    extern int	tcsetlines(int,int) noex ;
} /* end extern (C) */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
local inline int ucterm_magic(ucterm *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == UCTERM_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (ucterm_magic) */

local int	ucterm_loadterms(ucterm *,const uchar *) noex ;
local int	ucterm_attrbegin(ucterm *) noex ;
local int	ucterm_attrend(ucterm *) noex ;
local int	ucterm_qbegin(ucterm *) noex ;
local int	ucterm_qend(ucterm *) noex ;
local int	ucterm_controlmode(ucterm *) noex ;
local int	ucterm_writeproc(ucterm *,cchar *,int) noex ;

local int	tty_wait(ucterm *,int) noex ;
local int	tty_echo(ucterm *,cchar *,int) noex ;
local int	tty_risr(ucterm *,cchar *,int) noex ;
local int	tty_wps(ucterm *,cchar *,int) noex ;
local int	tty_loadchar(ucterm *,cchar *,int) noex ;

local int	sinotprint(cchar *,int) noex ;


/* local variables */

constexpr int		fieldterm_tabsize = ((UCHAR_MAX + 1) / CHAR_BIT) ;

constexpr uid_t		uidend(-1) ;

constexpr uchar		ucterms[] = {
	0xEF, 0xFC, 0xC0, 0xFE,
	0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 
} ; /* end array (ucterms) */

constexpr int		sigouts[] = {
	SIGTTOU,
	0
} ; /* end array (sigouts) */

constexpr bool		f_firstread	= CF_FIRSTREAD ;
constexpr bool		f_subunix	= CF_SUBUNIX ;


/* exported variables */


/* exported subroutines */

int ucterm_start(ucterm *op,int fd) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_INVALID ;
	    if (fd >= 0) ylikely {
	        op->ti_start = getustime ;
	        op->fd = fd ;
	        op->uid = -1 ;
	        if ((rs = uc_tcgetpgrp(fd)) == SR_NOTTY) {
	            op->fl.noctty = true ;
	            rs = SR_OK ;
	        }
	        if (rs >= 0) ylikely {
		    if ((rs = ucterm_loadterms(op,ucterms)) >= 0) ylikely {
	                if ((rs = ucterm_attrbegin(op)) >= 0) ylikely {
		            if ((rs = ucterm_qbegin(op)) >= 0) ylikely {
		                op->magval = UCTERM_MAGIC ;
		            }
	 	            if (rs < 0) {
		                ucterm_attrend(op) ;
		            }
	                }
		    } /* end if (ucterm_loadterms) */
	        } /* end if (ok) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (ucterm_start) */

int ucterm_finish(ucterm *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = ucterm_magic(op)) >= 0) ylikely {
	    {
		rs1 = ucterm_qend(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = ucterm_attrend(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (ucterm_finish) */

int ucterm_suspend(ucterm *op) noex {
	return ucterm_restore(op) ;
} /* end subroutine (ucterm_suspend) */

int ucterm_resume(ucterm *op) noex {
	return ucterm_ensure(op) ;
} /* end subroutine (ucterm_resume) */

int ucterm_restore(ucterm *op) noex {
	int		rs ;
	if ((rs = ucterm_magic(op)) >= 0) ylikely {
	    TERMIOS	*attrp = &op->ts_old ;
	    if ((rs = uc_tcattrset(op->fd,TCSADRAIN,attrp)) >= 0) ylikely {
	        rs = op->fd ;
	    }
	} /* end if (magic) */
	return rs ;
} /* end subroutine (ucterm_restore) */

int ucterm_ensure(ucterm *op) noex {
	int		rs ;
	if ((rs = ucterm_magic(op)) >= 0) ylikely {
	    TERMIOS	*attrp = &op->ts_new ;
	    cint	cmd = TCSADRAIN ;
	    rs = uc_tcattrset(op->fd,cmd,attrp) ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (ucterm_ensure) */

int ucterm_control(ucterm *op,int cmd,...) noex {
	va_list		ap ;
	int		rs ;
	if ((rs = ucterm_magic(op)) >= 0) ylikely {
	    cint	fc = (cmd & FM_CMDMASK) ;
	    int		iw ;
	    int		*iwp ;
	    va_begin(ap,cmd) ;
	    switch (fc) {
	    case uctermcmd_noop:
		break ;
	    case uctermcmd_getuid:
		if (op->uid == uidend) {
	            if ((rs = uc_fuid(op->fd)) >= 0) {
			op->uid = rs ;
		    }
		} else {
		    rs = op->uid ;
		}
	        break ;
	    case uctermcmd_getsid:
	        rs = uc_tcgetsid(op->fd) ;
	        break ;
	    case uctermcmd_getmode:
	        iwp = (int *) va_arg(ap,int *) ;
	        if (iwp != nullptr) *iwp = op->mode ;
	        break ;
	    case uctermcmd_setmode:
	        iw = (int) va_arg(ap,int) ;
	        op->mode = iw ;
		rs = ucterm_controlmode(op) ;
	        break ;
	    case uctermcmd_setrterms:
		{
	            const uchar	*rp = (uchar *) va_arg(ap,uchar *) ;
		    memcpy(op->rterms,rp,32) ;
		}
		break ;
	    case uctermcmd_reestablish:
		{
		    TERMIOS	*attrp = &op->ts_new ;
	            cint	acmd = TCSADRAIN ;
	            rs = uc_tcattrset(op->fd,acmd,attrp) ;
		}
	        break ;
	    case uctermcmd_getmesg:
		rs = ucterm_getmesg(op) ;
		break ;
	    case uctermcmd_setmesg:
		{
		    cint	f = (int) va_arg(ap,int) ;
	            rs = tcsetmesg(op->fd,f) ;
		}
		break ;
	    case uctermcmd_getbiff:
		rs = ucterm_getbiff(op) ;
		break ;
	    case uctermcmd_setbiff:
		{
		    cint	f = (int) va_arg(ap,int) ;
	            rs = tcsetbiff(op->fd,f) ;
		}
		break ;
	    case uctermcmd_getpgrp:
	        rs = uc_tcgetpgrp(op->fd) ;
	        break ;
	    case uctermcmd_setpgrp:
		{
		    sigign si ;
		    if ((rs = sigign_start(&si,sigouts)) >= 0) ylikely {
		        const pid_t	pgrp = (pid_t) va_arg(ap,pid_t) ;
	                rs = uc_tcsetpgrp(op->fd,pgrp) ;
		        sigign_finish(&si) ;
		    } /* end if (sigign) */
		}
	        break ;
	    case uctermcmd_getpop:
		rs = ucterm_getpop(op) ;
		break ;
	    case uctermcmd_setpop:
		{
		    cint v = (int) va_arg(ap,int) ;
	            rs = ucterm_setpop(op,v) ;
		}
		break ;
	    default:
	        rs = SR_INVALID ;
	        break ;
	    } /* end switch */
	    va_end(ap) ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (ucterm_control) */

int ucterm_status(ucterm *op,int cmd,...) noex {
	va_list		ap ;
	int		rs ;
	if ((rs = ucterm_magic(op)) >= 0) ylikely {
	    va_begin(ap,cmd) ;
	    switch (cmd) {
	    case uctermcmd_noop:
	        break ;
	    case uctermcmd_setmesg:
	        {
	            cint	v = (int) va_arg(ap,int) ;
	            rs = tcsetmesg(op->fd,v) ;
	        }
	        break ;
	    case uctermcmd_getlines:
	        rs = tcgetlines(op->fd) ;
	        break ;
	    case uctermcmd_setlines:
	        {
	            cint	v = (int) va_arg(ap,int) ;
	            rs = tcsetlines(op->fd,v) ;
	        }
	        break ;
	    case uctermcmd_getsid:
	        rs = uc_tcgetsid(op->fd) ;
	        break ;
	    case uctermcmd_getpgrp:
	        rs = uc_tcgetpgrp(op->fd) ;
	        break ;
	    case uctermcmd_setpgrp:
	        {
	            const pid_t	pgrp = (pid_t) va_arg(ap,pid_t) ;
	            rs = uc_tcsetpgrp(op->fd,pgrp) ;
	        }
	        break ;
	    default:
	        rs = SR_INVALID ;
	        break ;
	    } /* end switch */
	    va_end(ap) ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (ucterm_status) */

int ucterm_reade(ucterm *op,char *rbuf,int rlen,int timeout,int fc,
		ucterm_pr *lpp,ucterm_ld *llp) noex {
	int		rs ;
	int		count = 0 ;
	if ((rs = ucterm_magic(op,rbuf)) >= 0) ylikely {
	int		ch = -1 ;
	cuchar		*terms = op->rterms ;
	char		qbuf[2] ;
	fc |= op->mode ;
	if (fc & fm_rawin) fc |= fm_nofilter ;
	if (fc & fm_noecho) fc |= fm_notecho ;

	op->fl.cntl_c = false ;
	op->fl.cntl_o = false ;		/* cancel ^O effect */
	op->fl.read = true ;		/* read is in progress */

/* top of further access */
/* top */
	if ((rs >= 0) && (lpp != nullptr) && (lpp->plen > 0)) {
	    rs = tty_wps(op,lpp->pbuf,lpp->plen) ;
	}

	if ((rs >= 0) && (llp != nullptr) && (llp->llen > 0)) {
	    rs = tty_loadchar(op,llp->lbuf,llp->llen) ;
	}

	count = 0 ;

/* check TA buffer first */
/* next */
	while ((rs >= 0) && (count < rlen)) {
	    bool	f_eot = false ;

	    while ((rs = charq_rem(&op->taq,qbuf)) == SR_EMPTY) {
	 	{
	            rs = tty_wait(op,timeout) ;
		}
	        if ((timeout >= 0) && (op->timeout <= 0)) break ;
	        if (rs < 0) break ;
	    } /* end while */

	    if (rs < 0) break ;

	    if ((timeout >= 0) && (op->timeout <= 0)) break ;

	    if (op->fl.cntl_c) break ;

	    ch = mkchar(qbuf[0]) ;
	    rbuf[count] = charconv(ch) ;

/* check for terminator */

	    if (BATST(terms,ch))
	        break ;

/* check for a filter character */

	    if (! (fc & fm_nofilter)) {
	        switch (ch) {
	        case CH_EOT:
	            if (count == 0)
	                f_eot = true ;
	            ch = -1 ;
	            break ;
	        case CH_NAK:
	            if (! (fc & fm_noecho)) {
	                rs = tty_echo(op," ^U\r\n",5) ;
		    }
	            ch = -1 ;
	            break ;
	        case CH_DEL:
	        case CH_BS:
	            if (count > 0) {
	                count -= 1 ;
	                if (! (fc & fm_noecho)) {
	                    rs = tty_echo(op,"\b \b",3) ;
			}
	            }
	            ch = -1 ;
	            break ;
	        case CH_DC2:
	            if (! (fc & fm_noecho)) {
	                rs = tty_echo(op," ^R\r\n",5) ;
	                if ((rs >= 0) && (lpp != nullptr) && 
	                    (lpp->pbuf != nullptr) && (lpp->plen > 0)) {
	                    rs = tty_wps(op,lpp->pbuf,lpp->plen) ;
			}
	                if ((rs >= 0) && (count > 0)) {
	                    rs = tty_echo(op,rbuf,count) ;
			}
	            }
	            ch = -1 ;
	            break ;
	        case CH_CAN:
	            if (! (fc & fm_noecho)) {
	                rs = tty_echo(op," ^X\r\n",5) ;
		    }
	            ch = -1 ;
	            break ;
	        } /* end switch */
	    } /* end if (filtering) */

	    if ((rs >= 0) && (ch >= 0)) {
	        if ((! (fc & fm_noecho)) && isprintlatin(ch)) {
		    char	ebuf[2] ;
		    ebuf[0] = charconv(ch) ;
	            rs = tty_echo(op,ebuf,1) ;
	        }
	        count += 1 ;
	    } /* end if (normal character) */

	    if (f_eot) break ;
	} /* end while */

/* exit processing */

	if ((rs >= 0) && (ch >= 0) && BATST(terms,ch)) {
	    switch (ch) {
	    case CH_CR:
	    case CH_LF:
	        if (! ((fc & fm_notecho) || (fc & fm_noecho))) {
	            rs = tty_echo(op,"\r\n",2) ;
		}
	        if (! (fc & fm_nofilter)) {
	            rbuf[count] = CH_LF ;
		}
	        break ;
	    case CH_ESC:
	    case CH_CSI:
	        if (! ((fc & fm_notecho) || (fc & fm_noecho))) {
	            rs = tty_echo(op,"$",1) ;
		}
	        break ;
	    } /* end switch */
	    count += 1 ;
	} /* end if (terminator processing) */

	op->fl.read = false ;
	} /* end if (magic) */
	return (rs >= 0) ? count : rs ;
} /* end subroutine (ucterm_reade) */

int ucterm_read(ucterm *op,char *rbuf,int rlen) noex {
	return ucterm_reade(op,rbuf,rlen,-1,0,nullptr,nullptr) ;
} /* end subroutine (ucterm_read) */

int ucterm_write(ucterm *op,cchar *wbuf,int wlen) noex {
	int		rs ;
	int		tlen = 0 ;
	if ((rs = ucterm_magic(op)) >= 0) {
	    if (! op->fl.cntl_o) ylikely {
	        if (wlen < 0) wlen = lenstr(wbuf) ;
	        if (op->mode & fm_rawout) {
	            rs = u_write(op->fd,wbuf,wlen) ;
		    tlen = rs ;
	        } else {
	            rs = ucterm_writeproc(op,wbuf,wlen) ;
	            tlen = rs ;
	        }
	    } /* end if (not-in-cancel) */
	} /* end if (magic) */
	return (rs >= 0) ? tlen : rs ;
} /* end subroutine (ucterm_write) */

int ucterm_cancel(ucterm *op,int fc,int cparam) noex {
	int		rs ;
	(void) fc ;
	if ((rs = ucterm_magic(op)) >= 0) ylikely {
	    rs = cparam ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (ucterm_cancel) */

int ucterm_poll(ucterm *op) noex {
	int		rs ;
	if ((rs = ucterm_magic(op)) >= 0) ylikely {
	    if ((rs = tty_wait(op,0)) >= 0) ylikely {
	        if (op->fl.cntl_c) rs = SR_CANCELED ;
	    }
	} /* end if (magic) */
	return rs ;
} /* end subroutine (ucterm_poll) */

int ucterm_getmesg(ucterm *op) noex {
	int		rs ;
	if ((rs = ucterm_magic(op)) >= 0) ylikely {
	    if (ustat sb ; (rs = u_fstat(op->fd,&sb)) >= 0) {
	        rs = bool(sb.st_mode & S_IWGRP) ;
	    }
	} /* end if (magic) */
	return rs ;
} /* end subroutine (ucterm_getmesg) */

int ucterm_getbiff(ucterm *op) noex {
	int		rs ;
	if ((rs = ucterm_magic(op)) >= 0) ylikely {
	    if (ustat sb ; (rs = u_fstat(op->fd,&sb)) >= 0) {
	        rs = bool(sb.st_mode & S_IXUSR) ;
	    }
	} /* end if (magic) */
	return rs ;
} /* end subroutine (ucterm_getbiff) */

int ucterm_getpop(ucterm *op) noex {
	int		rs ;
	if ((rs = ucterm_magic(op)) >= 0) ylikely {
	    if (ustat sb ; (rs = u_fstat(op->fd,&sb)) >= 0) ylikely {
	        int	v = 0 ;
	        if (sb.st_mode & S_IXUSR) v |= 0x01 ;
	        if (sb.st_mode & S_IWGRP) v |= 0x02 ;
	        rs = v ;
	    } /* end if (u_fstat) */
	} /* end if (magic) */
	return rs ;
} /* end subroutine (ucterm_getpop) */

int ucterm_setpop(ucterm *op,int v) noex {
	int		rs ;
	int		rv = 0 ;
	if ((rs = ucterm_magic(op)) >= 0) ylikely {
	    if (ustat sb ; (rs = u_fstat(op->fd,&sb)) >= 0) ylikely {
	        mode_t	fm = sb.st_mode ;
	        if (sb.st_mode & S_IXUSR) rv |= 0x01 ;
	        if (sb.st_mode & S_IWGRP) rv |= 0x02 ;
	        if (v != rv) {
		    fm &= S_IXUSR ;
		    if (v & 0x01) fm |= S_IXUSR ;
		    fm &= S_IWGRP ;
		    if (v & 0x02) fm |= S_IWGRP ;
		    rs = u_fchmod(op->fd,fm) ;
	        } /* end if (needed update) */
	    } /* end if (stat) */
	} /* end if (magic) */
	return (rs >= 0) ? rv : rs ;
} /* end subroutine (ucterm_setpop) */


/* private subroutines */

local int ucterm_loadterms(ucterm *op,const uchar *ut) noex {
	int		rs = SR_FAULT ;
	if (ut) ylikely {
	    rs = SR_OK ;
	    memcpy(op->rterms,ut,fieldterm_tabsize) ;
	}
	return rs ;
} /* end subroutine */

local int ucterm_attrbegin(ucterm *op) noex {
	cint		fd = op->fd ;
	int		rs ;
	if ((rs = uc_tcattrget(fd,&op->ts_old)) >= 0) {
	    TERMIOS	*tp = &op->ts_new ;
	    op->ts_new = op->ts_old ;

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
    
	    tp->c_cc[VMIN] = TTY_MINCHARS ;
	    tp->c_cc[VTIME] = TTY_MINTIME ;
    
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
	    rs = uc_tcattrset(fd,TCSADRAIN,tp) ;
	    if (rs < 0) {
		uc_tcattrset(fd,TCSADRAIN,&op->ts_old) ;
	    } /* end if (error) */
	} /* end if */
	return rs ;
} /* end subroutine (ucterm_attrbegin) */

local int ucterm_attrend(ucterm *op) noex {
	cint		fd = op->fd ;
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    rs1 = uc_tcattrset(fd,TCSADRAIN,&op->ts_old) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
} /* end subroutine (ucterm_attrend) */

local int ucterm_qbegin(ucterm *op) noex {
	int		rs ;
	if ((rs = charq_start(&op->taq,TA_SIZE)) >= 0) ylikely {
	    rs = charq_start(&op->ecq,EC_SIZE) ;
	    if (rs < 0) {
		charq_finish(&op->taq) ;
	    }
	}
	return rs ;
} /* end subroutine (ucterm_qbegin) */

local int ucterm_qend(ucterm *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    rs1 = charq_finish(&op->taq) ;
	    if (rs >= 0) rs = rs1 ;
	}
	{
	    rs1 = charq_finish(&op->ecq) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
} /* end subroutine (ucterm_qend) */

local int ucterm_controlmode(ucterm *op) noex {
	cint		m = op->mode ;
	int		rs = SR_OK ;
	op->fl.nosig = ((m & fm_nosig) != 0) ;
	return rs ;
} /* end subroutine (ucterm_controlmode) */

local int ucterm_writeproc(ucterm *op,cchar *buf,int buflen) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		tlen = 0 ;

	tlen = buflen ;
	if (cchar *tp ; (tp = strnchr(buf,buflen,'\n')) != nullptr) {
	    cint bsz = (buflen + 10) ;
	    if (buffer pb ; (rs = buffer_start(&pb,bsz)) >= 0) {
		int	tl ;
	        int	bl = buflen ;
	        cchar	*bp = buf ;

	        tl = intconv(tp - bp) ;
	        buffer_buf(&pb,bp,tl) ;
	        buffer_chr(&pb,'\r') ;
	        buffer_chr(&pb,'\n') ;

	        bl -= intconv((tp + 1) - bp) ;
	        bp = (tp + 1) ;

	        while ((tp = strnchr(bp,bl,'\n')) != nullptr) {
		    tl = intconv(tp - bp) ;
	            buffer_buf(&pb,bp,tl) ;
	            buffer_chr(&pb,'\r') ;
	            buffer_chr(&pb,'\n') ;
	            bl -= intconv((tp + 1) - bp) ;
	            bp = (tp + 1) ;
	        } /* end while */

	        if (bl > 0) {
	            buffer_buf(&pb,bp,bl) ;
	        }

	        if (cchar *sp ; (rs = buffer_get(&pb,&sp)) > 0) {
	            rs = u_write(op->fd,sp,rs) ;
		}

	        rs1 = buffer_finish(&pb) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (initialize buffer) */

	} else {
	    rs = u_write(op->fd,buf,buflen) ;
	}

	return (rs >= 0) ? tlen : rs ;
} /* end subroutine (ucterm_writeproc) */

local int tty_wps(ucterm *op,cchar *ubuf,int ulen) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (ulen < 0) ulen = lenstr(ubuf) ;
	if (ulen > 0) ylikely {
	    if (int ci ; (ci = sinotprint(ubuf,ulen)) >= 0) ylikely {
	        if (buffer pb ; (rs = buffer_start(&pb,ulen)) >= 0) {
	            int		bl = ulen ;
	            cchar	*bp = ubuf ;
	            if (ci > 0) {
	                buffer_buf(&pb,bp,ci) ;
		    }
	            bp += (ci + 1) ;
	            bl -= (ci + 1) ;
	            while ((bl > 0) && ((ci = sinotprint(bp,bl)) >= 0)) {
	                if (ci > 0) buffer_buf(&pb,bp,ci) ;
	                bp += (ci + 1) ;
	                bl -= (ci + 1) ;
	            } /* end while */
	            if (bl > 0) {
	                buffer_buf(&pb,bp,bl) ;
		    }
	            if (cchar *sp ; (rs = buffer_get(&pb,&sp)) > 0) {
	                rs = u_write(op->fd,sp,rs) ;
		    }
	            rs1 = buffer_finish(&pb) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (initialize buffer) */
	    } else {
	        rs = u_write(op->fd,ubuf,ulen) ;
	    }
	} /* end if (non-zero) */
	return (rs >= 0) ? ulen : rs ;
} /* end subroutine (tty_wps) */

local int tty_loadchar(ucterm *op,cchar *pbuf,int pbuflen) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	for (int i = 0 ; (rs >= 0) && (i < pbuflen) ; i += 1) {
	    cint	ch = mkchar(pbuf[i]) ;
	    if (isprintlatin(ch)) {
	        c += 1 ;
	        rs = charq_ins(&op->taq,ch) ;
	    }
	} /* end for */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (tty_loadchar) */

local int tty_wait(ucterm *op,int timeout) noex {
	POLLFD		fds[2] = {} ;
	time_t		daytime = getustime ;
	time_t		lasttime ;
	int		rs ;
	int		len = 0 ;
	int		nfds ;
	int		polltime ;
	int		f_starting, f_looping ;
	char		cbuf[TTY_READCHARS+1] ;

/* initialize the POLL structure */

	nfds = 0 ;
	fds[nfds].fd = op->fd ;
	fds[nfds].events = POLLIN ;
	nfds += 1 ;
	fds[nfds].fd = -1 ;
	fds[nfds].events = POLLIN ;

	if (timeout < 0)
	    timeout = INT_MAX ;

	polltime = MIN(3000,(timeout * 1000)) ;

	op->timeout = timeout ;
	lasttime = daytime ;

	/* try it out the first time */
	if_constexpr (f_firstread) {
	    rs = u_read(op->fd,cbuf,TTY_READCHARS) ;
	    len = rs ;
	    f_starting = true ;
	    goto enter ;
	} /* end if_constexpr (f_firstread) */

/* loop it */
loop:
	f_starting = false ;

/* zero out the buffer so that we can tell if Solaris screws us later */

	for (int i = 0 ; i < TTY_READCHARS ; i += 1) {
	    cbuf[i] = '\0' ;
	}

/* wait for read attention */

	rs = u_poll(fds,1,polltime) ;

	if (rs == SR_INTR) {
	    goto loop ;
	}

/* do the system-call read */

	if ((rs >= 0) && (fds[0].revents & POLLIN)) {
	    rs = u_read(op->fd,cbuf,TTY_READCHARS) ;
	    len = rs ;
	}  /* end if */

enter:
	if (rs < 0)
	    goto ret0 ;

	op->loopcount += 1 ;
	if (len < 0)
	    goto ret0 ;

	if (len == 0) {
	    daytime = time(nullptr) ;
	    if (op->timeout >= 0) {
	        op->timeout -= intconv(daytime - lasttime) ;
	        if (op->timeout <= 0)
	            return SR_OK ;

	    }

	    if ((! f_starting) && 
	        ((daytime - op->ti_start) >= TO_HANGUP)) {

	        f_looping = (op->loopcount > MAXLOOPS) ;
	        op->ti_start = daytime ;
	        op->loopcount = 0 ;
	        if (f_looping) {

	            rs = SR_NOANODE ;
	            goto ret0 ;
	        }

	    } /* end if (looping check) */

	} else {
	    int		i ; /* used-afterwards */
	    /* skip over leading zero characters fabricated by Solaris SVR4 */
	    for (i = 0 ; (i < len) && (cbuf[i] == '\0') ; i += 1) {
	    } /* end for */
	    /* call the Receive-Interrupt-Service-Routine */
	    if ((len - i) > 0) {
	        rs = tty_risr(op,(cbuf + i),(len - i)) ;
	   }
	} /* end if */
	/* should we go around again? */
	lasttime = daytime ;
	if ((rs >= 0) &&
	    (len <= 0) && ((op->timeout < 0) || (op->timeout > 0)))
	    goto loop ;

/* we-re out of here */
ret0:
	return (rs >= 0) ? len : rs ;
} /* end subroutine (tty_wait) */

local int tty_risr(ucterm *op,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		f_dle = false ;
	for (int i = 0 ; i < sl ; i += 1) {
	    cint	ch = mkchar(sp[i]) ;
	    if (op->fl.nosig) {
	        rs = charq_ins(&op->taq,ch) ;
	        op->fl.rw = true ;
	    } else {
	        switch (ch) {
	        case CH_XOFF:
	            op->fl.suspend = true ;
	            break ;
	        case CH_XON:
	            op->fl.suspend = false ;
	            break ;
	        case CH_SO:
	            if (op->fl.cntl_o) {
	                op->fl.cntl_o = false ;
	            } else {
	                op->fl.cntl_o = true ;
		    	if (! op->fl.nosigecho) {
	                    rs = tty_echo(op," ^O\r\n",5) ;
			}
	            }
	            break ;
	        case CH_ETX:
		    if (! op->fl.nosigecho) {
	                rs = tty_echo(op," ^C\r\n",5) ;
		    }
	            op->fl.cntl_c = true ;
	            op->fl.rw = true ;
	            break ;
	        case CH_EM:
		    if (! op->fl.nosigecho) {
	                rs = tty_echo(op," ^Y\r\n",5) ;
		    }
	            op->fl.cntl_c = true ;
	            op->fl.rw = true ;
	            break ;
	        case CH_DLE:
	            f_dle = true ;
	            break ;
/* Control-Z (substitute) */
	        case CH_SUB:
		    if (op->fl.noctty) {
		        op->fl.cntl_z = true ;
		    } else {
			if_constexpr (f_subunix) {
		            rs = uc_raise(SIGTSTP) ;
			} else {
		            op->fl.cntl_z = true ;
			} /* end if_constexpr (f_subunix) */
		    } /* end if */
	            break ;
	        default:
	            if ((! op->fl.read) && (ch == CH_CAN))  {
	                char	dch ;
	                while (charq_rem(&op->taq,&dch) >= 0) ;
	            } else {
	                rs = charq_ins(&op->taq,ch) ;
	                op->fl.rw = true ;
	            }
	            break ;
	        } /* end switch */
	    } /* end if (signal-generation or not) */
	    if (rs < 0) break ;
	} /* end for */
	return (rs >= 0) ? f_dle : rs ;
} /* end subroutine (tty_risr) */

local int tty_echo(ucterm *op,cchar *ebuf,int elen) noex {
	int		rs = SR_OK ;
	if (elen < 0) elen = lenstr(ebuf) ;
	if (elen > 0) {
	    rs = u_write(op->fd,ebuf,elen) ;
	}
	return rs ;
} /* end subroutine (tty_echo) */

local int sinotprint(cchar *sp,int sl) noex {
	int		i = 0 ; /* used-afterwards */
	bool		f = false ;
	for (i = 0 ; (i < sl) && sp[0] ; i += 1) {
	    cint	ch = mkchar(sp[0]) ;
	    f = isprintlatin(ch) ;
	    f = f || (ch == CH_SI) || (ch == CH_SO) ;
	    if (! f) break ;
	} /* end for */
	return (f) ? -1 : i ;
} /* end subroutine (sinotprint) */


