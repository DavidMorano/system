/* egs SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* entropy gathering and manipulation */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	egs

	Description:
	This module provides a convenient way to perform some
	operations with the Entropy Gathering Daemon (EGD) that may
	be running on the current system.

	Commands to the Entropy Gathering Daemon (EGD):

+ 0x00 (get entropy level)
  0xMM (msb) 0xmm 0xll 0xLL (lsb)
+ 0x01 (read entropy nonblocking) 0xNN (bytes requested)
  0xMM (bytes granted) MM bytes
+ 0x02 (read entropy blocking) 0xNN (bytes desired)
  [block] NN bytes
+ 0x03 (write entropy) 0xMM 0xLL (bits of entropy) 0xNN (bytes of data) NN bytes
+ 0x04 (report PID)
  0xMM (length of PID string, not null-terminated) MM chars

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* system types */
#include	<sys/param.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>		/* |INT_MAX| + |UCHAR_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<new>			/* |nothrow(3c++)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<dial.h>
#include	<permx.h>
#include	<cfdec.h>
#include	<netorder.h>
#include	<egscmd.hh>
#include	<localmisc.h>

#include	"egs.h"


/* local defines */

#ifndef	PID_MAX
#define	PID_MAX			999999
#endif

#define	CMDBUFLEN		16

#define	TO_READ			10	/* seconds */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static int egs_ctor(egs *op,Args ... args) noex {
    	EGS		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = memclear(hop) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (egs_ctor) */

static int egs_dtor(egs *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (egs_dtor) */

template<typename ... Args>
static inline int egs_magic(egs *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == EGS_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (egs_magic) */

static int egs_opencheck(egs *) noex ;


/* exported variables */


/* exported subroutines */

int egs_open(egs *op,cchar *filename) noex {
	int		rs ;
	if (filename == nullptr) filename = EGS_DEFFILE ;
	if ((rs = egs_ctor(op)) >= 0) {
	    cint	am = (R_OK | W_OK) ;
	    op->pid = -1 ;
	    if ((rs = perm(filename,-1,-1,nullptr,am)) >= 0) {
	        if ((rs = dialuss(filename,5,0)) >= 0) {
	            op->fd = rs ;
	            if ((uc_closeonexec(op->fd,true)) >= 0) {
		        if ((rs = egs_opencheck(op)) >= 0) {
		            op->magic = EGS_MAGIC ;
		        }
	            } /* end if (closeonexec) */
	            if (rs < 0) {
		        u_close(op->fd) ;
		        op->fd = -1 ;
		    }
	        } /* end if (dialuss) */
	    } /* end if (perm) */
	    if (rs < 0) {
		egs_dtor(op) ;
	    }
	} /* end if (egs_ctor) */
	return rs ;
}
/* end subroutine (egs_open) */

int egs_close(egs *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = egs_magic(op)) >= 0) {
	    if (op->fd >= 0) {
	        rs1 = u_close(op->fd) ;
	        if (rs >= 0) rs = rs1 ;
	        op->fd = -1 ;
	    }
	    {
		rs1 = egs_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (egs_close) */

int egs_read(egs *op,char *rbuf,int rlen) noex {
	int		rs ;
	int		rl = 0 ;
	if ((rs = egs_magic(op,rbuf)) >= 0) {
	    cint	to = TO_READ ;
	    cint	fm = FM_EXACT ;
	    cint	clen = min(UCHAR_MAX,CMDBUFLEN) ;
	    int		len{} ;
	    uchar	cmdbuf[CMDBUFLEN + 1] ;
	    for (int i = 0 ; rlen > 0 ; i += len) {
	        cint	mlen = min(clen,rlen) ;
		int	ci = 0 ;
	        cmdbuf[ci++] = char(egscmd::read) ;
	        cmdbuf[ci++] = uchar(mlen) ;
	        if ((rs = uc_writen(op->fd,cmdbuf,ci)) >= 0) {
	            rs = uc_reade(op->fd,(rbuf + i),mlen,to,fm) ;
	            len = rs ;
		    rlen -= len ;
		    rl += len ;
	        }
	        if ((rs < 0) || (len == 0)) break ;
	    } /* end for */
	} /* end if (magic) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (egs_read) */

/* add some of our own entropy to the mix (is this a security problem?) */
int egs_write(egs *op,cchar *wbuf,int wlen) noex {
	int		rs ;
	int		wl = 0 ;
	if (wlen < 0) wlen = clenstr(wbuf) ;
	if ((rs = egs_magic(op,wbuf)) >= 0) {
	    cint	clen = min(UCHAR_MAX,CMDBUFLEN) ;
	    int		bits ;
	    int		len{} ;
	    uchar	cmdbuf[CMDBUFLEN + 1] ;
	    for (int i = 0 ; wlen > 0 ; i += 1) {
	        cint	mlen = min(clen,wlen) ;
	        int	ci = 0 ;
		bits = (mlen * CHAR_BIT) ;
	        cmdbuf[ci++] = char(egscmd::write) ;
		cmdbuf[ci++] = uchar(bits >> CHAR_BIT) ;	/* high bits */
		cmdbuf[ci++] = uchar(bits >> 0) ;		/* low  bits */
	        cmdbuf[ci++] = uchar(mlen) ;
	        if ((rs = uc_writen(op->fd,cmdbuf,ci)) >= 0) {
	            rs = uc_writen(op->fd,(wbuf + i),mlen) ;
		    len = rs ;
	            wlen -= len ;
		    wl += len ;
	        }
		if (rs < 0) break ;
	    } /* end for */
	} /* end if (magic) */
	return (rs >= 0) ? wl : rs ;
}
/* end subroutine (egs_write) */

/* return the level of entropy available */
int egs_level(egs *op) noex {
	int		rs ;
	int		level = 0 ;
	if ((rs = egs_magic(op)) >= 0) {
	    char	cmdbuf[CMDBUFLEN + 1] ;
	    int		ci = 0 ;
	    cmdbuf[ci++] = char(egscmd::getlevel) ;
	    if ((rs = uc_writen(op->fd,cmdbuf,ci)) >= 0) {
	        cint	to = TO_READ ;
	        cint	fm = FM_EXACT ;
	        cmdbuf[3] = 0 ; /* <- fourth byte (make up 4-byte integer) */
	        if ((rs = uc_reade(op->fd,cmdbuf,3,to,fm)) >= 0) {
	            uint	uiw ;
	            netorder_rui(cmdbuf,&uiw) ;
	            level = (uiw & INT_MAX) ;
	        } /* end if (uc_reade) */
	    } /* end if (uc_writen) */
	} /* end if (magic) */
	return (rs >= 0) ? level : rs ;
}
/* end subroutine (egs_level) */

int egs_getpid(egs *op,pid_t *pidp) noex {
	int		rs ;
	if ((rs = egs_magic(op)) >= 0) {
	    if (pidp) *pidp = pid_t(op->pid) ;
	    rs = int(op->pid) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (egs_getpid) */


/* private subroutines */

/* get the EGD program PID and check it */
static int egs_opencheck(egs *op) noex {
	int		rs ;
	int		len ;
	char		cmdbuf[CMDBUFLEN + 1] ;
	int		ci = 0 ;
	cmdbuf[ci++] = char(egscmd::getpid) ;
	if ((rs = uc_writen(op->fd,cmdbuf,ci)) >= 0) {
	    cint	to = TO_READ ;
	    cint	fm = FM_EXACT ;
	    if ((rs = uc_reade(op->fd,cmdbuf,1,to,fm)) >= 0) {
		len = int(cmdbuf[0] & UCHAR_MAX) ;
		if (len <= CMDBUFLEN) {
		    memclear(cmdbuf) ;
		    if ((rs = uc_reade(op->fd,cmdbuf,len,to,fm)) >= 0) {
			cmdbuf[rs] = '\0' ;
	    		if (int v ; (rs = cfdeci(cmdbuf,rs,&v)) >= 0) {
	    		    op->pid = (v & INT_MAX) ;
			    if ((op->pid < 0) || (op->pid > PID_MAX)) {
				rs = SR_PROTO ;
			    }
			} /* end if (cfdeci) */
		    } /* end if (uc_reade) */
		} else {
		    rs = SR_PROTO ;
		}
	    } /* end if (uc_reade) */
	} /* end if (uc_writen) */
	return rs ;
}
/* end subroutine (egs_opencheck) */


