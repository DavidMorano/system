/* syshelper SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* syshelper gathering and manipulation */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/**************************************************************************

  	Name:
	syshelper

	Description:
	This module provides a convenient way to perform some
	operations with the Entropy Gathering Daemon (EGD) that
	may be running on the current system.

	Commands to the Entropy Gathering Daemon (EGD):

	0x00 (get syshelper level)
	0xMM (msb) 0xmm 0xll 0xLL (lsb)
	0x01 (read syshelper nonblocking) 0xNN (bytes requested)
	0xMM (bytes granted) MM bytes
	0x02 (read syshelper blocking) 0xNN (bytes desired) [block] NN bytes
	0x03 (write syshelper) 0xMM 0xLL (bits of syshelper) 0xNN 
		(bytes of data) NN bytes
	0x04 (report PID)
	0xMM (length of PID string, not null-terminated) MM chars

**************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>		/* |UCHAR_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
#include	<algorithm>		/* |min(3c++)| */
#include	<usystem.h>
#include	<netorder.h>
#include	<cfdec.h>
#include	<xperm.h>
#include	<dial.h>		/* |dialuss(3uc)| */
#include	<localmisc.h>

#include	"syshelper.h"


/* local defines */

#define	SYSHELPER_DEFFILE	"/tmp/syshelper"
#define	SYSHELPER_MAXPID	(2 << 15)

#define	SYSHELPER_CMDGETLEVEL	0
#define	SYSHELPER_CMDREADSHORT	1
#define	SYSHELPER_CMDREAD	2
#define	SYSHELPER_CMDWRITE	3
#define	SYSHELPER_CMDGETPID	4

#define	CMDBUFLEN		100

#define	TO_READ			10	/* seconds */


/* imported namespaces */

using	std::min ;			/* subroutine-template */
using	std::max ;			/* subroutine-template */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static int syshelper_ctor(syshelper *op,Args ... args) noex {
    	SYSHELPER	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = memclear(hop) ;
	   op->pid = -1 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (syshelper_ctor) */

static int syshelper_dtor(syshelper *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (syshelper_dtor) */

template<typename ... Args>
static inline int syshelper_magic(syshelper *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == SYSHELPER_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (syshelper_magic) */


/* local variables */


/* exported variables */


/* exported subroutines */

static int	syshelper_starter(syshelper *) noex ;

int syshelper_start(syshelper *op,cchar *fname) noex {
	int		rs ;
	if (fname == nullptr) fname = SYSHELPER_DEFFILE ;
	if ((rs = syshelper_ctor(op)) >= 0) {
	    cint	am = (R_OK | W_OK) ;
	    if ((rs = perm(fname,-1,-1,nullptr,am)) >= 0) {
	        if ((rs = dialuss(fname,5,0)) >= 0) {
		    op->fd = rs ;
		    if ((rs = u_fcntl(op->fd,F_GETFD,0)) >= 0) {
			if ((rs & FD_CLOEXEC) == 0) {
	    		    rs |= FD_CLOEXEC ;
	    		    rs = u_fcntl(op->fd,F_SETFD,rs) ;
			}
			if (rs >= 0) {
			    rs = syshelper_starter(op) ;
			}
		    } /* end if (u_fcntl) */
		    if (rs < 0) {
			u_close(op->fd) ;
			op->fd = -1 ;
		    }
		} /* end if (dialuss) */
	    } /* end if (perm) */
	    if (rs < 0) {
		syshelper_dtor(op) ;
	    }
	} /* end if (syshelper_ctor) */
	return rs ;
}
/* end subroutine (syshelper_start) */

static int syshelper_starter(syshelper *op) noex {
    	int		rs ;
	char		cmdbuf[CMDBUFLEN + 1] = { SYSHELPER_CMDGETPID } ;
	if ((rs = uc_writen(op->fd,cmdbuf,1)) >= 0) {
	    cint	to = TO_READ ;
	    cint	fm = FM_EXACT ;
	    if ((rs = uc_reade(op->fd,cmdbuf,1,to,fm)) >= 0) {
		cint	clen = min(CMDBUFLEN,uchar(cmdbuf[0])) ;
	        rs = SR_TOOBIG ;
		if (len <= CMDBUFLEN) {
		    if ((rs = uc_reade(op->fd,cmdbuf,clen,to,fm)) >= 0) {
			if (int v ; (rs = cfdeci(cmdbuf,rs,&v)) >= 0) {
			    cint	mpid = SYSHELPER_MAXPID ;
			    op->pid = v ;
			    if ((v >= 0) && (v < mpid)) {
				op->magic = SYSHELPER_MAGIC ;
			    }
			}
		    }
		} /* end if (length ok) */
	    } /* end if (uc_reade) */
	} /* end if (uc_writen) */
	return rs ;
}
/* end subroutine (syshelper_starter) */

int syshelper_finish(syshelper *op) noex {
    	int		rs ;
	int		rs1 ;
	if ((rs = syshelper_magic(op)) >= 0) {
	   if (op->fd >= 0) {
	       rs1 = u_close(op->fd) ;
	       if (rs >= 0) rs = rs1 ;
	       op->fd = -1 ;
	   }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (syshelper_finish) */

int syshelper_write(syshelper *op,cchar *wbuf,int wlen) noex {
	int		rs ;
	int		tlen = 0 ;
	if ((rs = syshelper_magic(op,wbuf)) >= 0) {
	    char	cmdbuf[CMDBUFLEN + 1] = { SYSHELPER_CMDWRITE } ;
	    cint	clen = min(UCHAR_MAX,CMDBUFLEN) ;
	    int		bits ;
	    int		mlen = 0 ;
	    if (wlen < 0) wlen = strlen(wbuf) ;
	    for (int i = 0 ; (rs >= 0) && (wlen > 0) ; i += mlen) {
		mlen = min(clen,wlen) ;
		bits = mlen * 8 ;
		cmdbuf[1] = char(bits >> 8) ;
		cmdbuf[0] = char(bits & UCHAR_MAX) ;
	        cmdbuf[3] = char(mlen) ;
	        if ((rs = uc_writen(op->fd,cmdbuf,4)) >= 0) {
	            rs = uc_writen(op->fd,(wbuf + i),mlen) ;
	            wlen -= rs ;
		    tlen += rs ;
		}
	    } /* end for */
	} /* end if (magic) */
	return (rs >= 0) ? tlen : rs ;
}
/* end subroutine (syshelper_write) */

int syshelper_read(syshelper *op,char *rbuf,int rlen) noex {
	int		rs ;
	int		tlen = 0 ;
	if ((rs = syshelper_magic(op,rbuf)) >= 0) {
	    cint	to = TO_READ ;
	    cint	fm = FM_EXACT ;
	    cint	clen = min(UCHAR_MAX,CMDBUFLEN) ;
	    uchar	cmdbuf[CMDBUFLEN + 1] = { SYSHELPER_CMDREAD } ;
	    for (int i = 0 ; (rs >= 0) && (rlen > 0) ; ) {
	        cint	mlen = min(clen,rlen) ;
	        cmdbuf[1] = uchar(mlen) ;
	        if ((rs = uc_writen(op->fd,cmdbuf,2)) >= 0) {
	            rs = uc_reade(op->fd,(rbuf + i),mlen,to,fm) ;
	            rlen -= rs ;
	            i += rs ;
		    tlen += rs ;
	        }
	    } /* end for */
	} /* end if (magic) */
	return (rs >= 0) ? tlen : rs ;
}
/* end subroutine (syshelper_read) */

/* return the level of syshelper available */
int syshelper_level(syshelper *op) noex {
	int		rs ;
	if ((rs = syshelper_magic(op)) >= 0) {
	    char	cmdbuf[CMDBUFLEN + 1] = { SYSHELPER_CMDGETLEVEL } ;
	    if ((rs = uc_writen(op->fd,cmdbuf,1)) >= 0) {
		cint	to = TO_READ ;
		cint	fm = FM_EXACT ;
	        if ((rs = uc_reade(op->fd,cmdbuf,4,to,fm)) >= 0) {
	    	    int		niw ;
		    netorder_ri(cmdbuf,&niw) ;
		    rs = niw & INT_MAX ;
	        }
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (syshelper_level) */

int syshelper_getpid(syshelper *op,pid_t *pidp) noex {
	int		rs = SR_OK ;
	if ((rs = syshelper_magic(op)) >= 0) {
	   if (pidp) {
	       *pidp = pid_t(op->pid) ;
	   }
	   rs = op->pid ;
	} /* end if (syshelper_magic) */
	return rs ;
}
/* end subroutine (syshelper_getpid) */


