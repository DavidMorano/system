/* tcx SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* various terminal functions */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-20, David A­D­ Morano
	This is new code.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	tcsetown

	Description:
	This subroutine is similar to |logindevperm(3c)| (or whatever
	equivalent your system has). It sets the ownership of a
	terminal.

	Synopsis:
	int tcsetown(int fd,cchar *tdev,uid_t uid,gid_t gid,mode_t tp) noex

	Arguments:
	fd		file-descriptor of terminal
	tdev		device path to terminal device
	uid		UID to set
	gid		GID to set
	tp		permissions to set

	Returns:
	>=0		success
	<0		error (system-return)


	Name:
	tcsetlines

	Synopsis:
	int tcsetlines(int fd,int lines) noex

	Arguments:
	fd		file-descriptor of termainal
	lines		number of lines to try to set

	Returns:
	>=0		success (previous number of lines)
	<0		error (system-return)


	Name:
	tcgetlines

	Synopsis:
	int tcgetlines(int fd) noex

	Arguments:
	fd		file-descriptor

	Returns:
	>=0		OK
	<0		error (system-return)


	Synopsis:
	int tcsetws(int fd,WINSIZE *wsp) noex

	Arguments:
	fd		file-descriptor of termainal
	wsp		pointer to window size

	Returns:
	>=0		success (previous number of lines)
	<0		error (system-return)


	Synopsis:
	int tcsetws(int fd,WINSIZE *wsp) noex

	Arguments:
	fd		file-descriptor of termainal
	wsp		pointer to window size

	Returns:
	>=0		success (previous number of lines)
	<0		error (system-return)


	Name:
	tcsetmesg

	Description:
        This is an attempt at abstracting how to set the state of the terminal
        with regard to the reception of messages (stray writes to the terminal
        from the nether world).

	Synopsis:
	int tcsetmesg(int fd,int f_new) noex

	Arguments:
	fd		file-descriptor of terminal
	f_new		new TRUE or FALSE setting of message reeption

	Returns:
	>=0		previous TRUE or FALSE setting
	<0		error (system-return)


	Name:
	tcsetbiff

	Description:
        This is an attempt at abstracting how to set the state of the terminal
        with regard to biffing.

	Synopsis:
	int tcsetbiff(int fd,int f_new) noex

	Arguments:
	fd		file-descriptor of terminal
	f_new		new TRUE or FALSE setting biffing

	Returns:
	>=0		previous TRUE or FALSE setting
	<0		error (system-return)


	Name:
	tcpeek

	Description:
	We peek ahead in the terminal input buffer.

	Synosis:
	int tcpeek(int fd,char *dbuf,int dlen) noex

	Arguments:
	fd		file-descriptor of terminal
	dbuf		data buffer
	dlen		data buffer size provided

	Return:
	>=0		length of characters peeked at
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>
#include	<termios.h>
#include	<usystem.h>
#include	<localmisc.h>

#include	"tcx.h"


/* local defines */

#ifndef	CMSGBUFLEN
#define	CMSGBUFLEN	1024
#endif

#if	(defined(SYSHAS_STREAMS) && (SYSHAS_STREAMS > 0))
#define	F_STREAMS	1
#else
#define	F_STREAMS	0
#endif


/* imported namespaces */


/* local typedefs */


/* local variables */

constexpr bool		f_streams = F_STREAMS ;


/* exported variables */


/* exported subroutines */

int tcsetown(int fd,cchar *termdev,uid_t uid,gid_t gid,mode_t perms) noex {
	int		rs = SR_NOTOPEN ;
	if (fd >= 0) {
	    rs = SR_NOTTY ;
	    perms &= S_IAMB ;
	    if (isatty(fd)) {
	        if ((rs = u_fchmod(fd,perms)) >= 0) {
	            if ((rs = u_fchown(fd,uid,gid)) >= 0) {
	                if (termdev && termdev[0]) {
	                    if ((rs = u_chmod(termdev,perms)) >= 0) {
	                        rs = u_chown(termdev,uid,gid) ;
			    }
		        }
	            }
	        }
	    } /* end if (isatty) */
	} /* end if (valid) */
	return rs ;
}
/* end subroutine (tcsetown) */

int tcsetlines(int fd,int lines) noex {
	int		rs = SR_NOTOPEN ;
	int		plines = 0 ;
	if (fd >= 0) {
	    WINSIZE	ws{} ;
	    if (lines < 0) {
	        lines = 0 ;
	    } else if (lines > SHORT_MAX) {
	        lines = SHORT_MAX ;
	    }
	    if ((rs = u_ioctl(fd,TIOCGWINSZ,&ws)) >= 0) {
	        plines = ws.ws_row ;
	        ws.ws_row = short(lines) ;
	        rs = u_ioctl(fd,TIOCSWINSZ,&ws) ;
	    }
	} /* end if (valid) */
	return (rs >= 0) ? plines : rs ;
}
/* end subroutine (tcsetlines) */

int tcgetlines(int fd) noex {
	int		rs = SR_NOTOPEN ;
	if (fd >= 0) {
	    WINSIZE	ws{} ;
	    if ((rs = u_ioctl(fd,TIOCGWINSZ,&ws)) >= 0) {
	        rs = ws.ws_row ;
	    }
	} 
	return rs ;
}
/* end subroutine (tcgetlines) */

int tcsetws(int fd,WINSIZE *wsp) noex {
	int		rs = SR_FAULT ;
	if (wsp) {
	    rs = SR_NOTOPEN ;
	    if (fd >= 0) {
	        rs = u_ioctl(fd,TIOCSWINSZ,wsp) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (tcsetws) */

int tcgetws(int fd,WINSIZE *wsp) noex {
	int		rs = SR_FAULT ;
	if (wsp) {
	    rs = SR_NOTOPEN ;
	    if (fd >= 0) {
	        memclear(wsp) ;
	        rs = u_ioctl(fd,TIOCGWINSZ,wsp) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (tcgetws) */

int tcsetmesg(int fd,int f_new) noex {
	int		rs = SR_NOTOPEN ;
	int		f_old = false ;
	if (fd >= 0) {
	    USTAT	sb ;
	    if ((rs = u_fstat(fd,&sb)) >= 0) {
	        mode_t	m_old = sb.st_mode ;
	        f_old = (m_old & S_IWGRP) ;
	        if (! LEQUIV(f_old,f_new)) {
	            mode_t	m_new ;
	            if (f_new) {
	                m_new = (m_old | S_IWGRP) ;
	            } else {
	                m_new = (m_old & (~ S_IWGRP)) ;
		    }
	            rs = u_fchmod(fd,m_new) ;
	        } /* end if (old and new were different) */
	    } /* end if (stat) */
	} /* end if (valid) */
	return (rs >= 0) ? f_old : rs ;
}
/* end subroutine (tcsetmesg) */

int tcsetbiff(int fd,int f_new) noex {
	int		rs = SR_NOTOPEN ;
	int		f_old = false ;
	if (fd >= 0) {
	    USTAT		sb ;
	    if ((rs = u_fstat(fd,&sb)) >= 0) {
	        mode_t	m_old = sb.st_mode ;
	        f_old = (m_old & S_IWGRP) ;
	        if (! LEQUIV(f_old,f_new)) {
	            mode_t	m_new ;
	            if (f_new) {
	                m_new = (m_old | S_IXUSR) ;
	            } else {
	                m_new = (m_old & (~ S_IXUSR)) ;
		    }
	            rs = u_fchmod(fd,m_new) ;
	        } /* end if (old and new were different) */
	    } /* end if (stat) */
	} /* end if (valid) */
	return (rs >= 0) ? f_old : rs ;
}
/* end subroutine (tcsetbiff) */

int tcpeek(int fd,char *dbuf,int dlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (dbuf) {
	    rs = SR_NOTOPEN ;
	    if (fd >= 0) {
	        if_constexpr (f_streams) {
	            cint	clen = CMSGBUFLEN ;
	            char	*cbuf{} ;
	            if ((rs = lm_mall((clen+1),&cbuf)) >= 0) {
			{
	                    STRPEEK	pd{} ;
	                    pd.flags = 0 ;
	                    pd.ctlbuf.buf = cbuf ;
	                    pd.ctlbuf.maxlen = clen ;
	                    pd.databuf.buf = dbuf ;
	                    pd.databuf.maxlen = dlen ;
	                    rs = u_ioctl(fd,I_PEEK,&pd) ;
	                    len = pd.databuf.len ;
			}
		        rs1 = lm_free(cbuf) ;
			if (rs >= 0) rs = rs1 ;
	            } /* end if (m-a-f) */
	        } else {
	            dbuf[0] = '\0' ;
	            rs = SR_NOSYS ;
	        } /* end if_constexpr (f_streams) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs > 0) ? len : rs ;
}
/* end subroutine (tcpeek) */


