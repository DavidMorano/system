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
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| */
#include	<usystem.h>
#include	<netorder.h>
#include	<cfdec.h>
#include	<localmisc.h>

#include	"syshelper.h"


/* local defines */

#define	SYSHELPER_DEFFILE	"/tmp/syshelper"
#define	SYSHELPER_MAGIC		0x93847561
#define	SYSHELPER_MAXPID	(2 << 15)

#define	SYSHELPER_CMDGETLEVEL	0
#define	SYSHELPER_CMDREADSHORT	1
#define	SYSHELPER_CMDREAD	2
#define	SYSHELPER_CMDWRITE	3
#define	SYSHELPER_CMDGETPID	4

#undef	CMDBUFLEN
#define	CMDBUFLEN		100

#define	TO_READ			10	/* seconds */


/* imported namespaces */

using	std:min ;			/* subroutine-template */
using	std:max ;			/* subroutine-template */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	dialuss(char *,int,int) noex ;
}


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

int syshelper_start(syshelper *op,cchar *filename) noex {
	int		rs ;
	if ((rs = syshelper_ctor(op)) >= 0) {
	    cint	am = (R_OK | W_OK) ;
	    int		len ;
	    char	cmdbuf[CMDBUFLEN + 1] ;
	    if (filename == nullptr) filename = SYSHELPER_DEFFILE ;
	    if ((rs = perm(filename,-1,-1,nullptr,am)) >= 0)
	        if ((rs = dialuss(filename,5,0)) >= 0) {
		    op->fd = rs ;
		    if ((rs = u_fcntl(op->fd,F_GETFD,0)) >= 0) {
			if ((rs & FD_CLOEXEC) == 0) {
	    		    rs |= FD_CLOEXEC ;
	    		    (void) u_fcntl(op->fd,F_SETFD,rs) ;
			}
		    }
		} /* end if (dialuss) */
	} /* end if (setting CLOSE-on-EXEC) */

	cmdbuf[0] = SYSHELPER_CMDGETPID ;
	rs = uc_writen(op->fd,cmdbuf,1) ;
	if (rs < 0)
	    goto badret ;

	rs = uc_reade(op->fd,cmdbuf,1, TO_READ, FM_EXACT) ;
	if (rs < 0)
	    goto badret ;

	len = (int) cmdbuf[0] ;
	if (len > CMDBUFLEN) {

	    rs = SR_TOOBIG ;
	    goto badret ;
	}

	rs = uc_reade(op->fd,cmdbuf,len, TO_READ, FM_EXACT) ;
	if (rs < 0)
	    goto badret ;

	if ((rs = cfdeci(cmdbuf,len,&op->pid)) < 0)
	    goto badret ;

	if ((op->pid < 0) || (op->pid >= SYSHELPER_MAXPID))
		goto badret ;
	
	op->magic = SYSHELPER_MAGIC ;
	return rs ;

/* bad things */
badret:
	u_close(op->fd) ;

	return rs ;
}
/* end subroutine (syshelper_start) */

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

int syshelper_write(syshelper *op,cchar *wbuf,wlen) noex {
	int		rs ;
	int		rlen, tlen, mlen ;
	int		bits ;
	int		i ;

	uchar	cmdbuf[CMDBUFLEN + 1] ;


	if (op == nullptr)
	    return SR_FAULT ;

	if (buf == nullptr)
	    return SR_FAULT ;

	if (op->magic != SYSHELPER_MAGIC)
	    return SR_NOTOPEN ;

	if (buflen < 0)
	    buflen = strlen(buf) ;

	tlen = MIN(255,CMDBUFLEN) ;

	i = 0 ;
	rlen = buflen ;
	while (rlen > 0) {

	    mlen = MIN(tlen,rlen) ;

		bits = mlen * 8 ;
	    cmdbuf[0] = SYSHELPER_CMDWRITE ;
		cmdbuf[1] = (uchar) (bits >> 8) ;
		cmdbuf[0] = (uchar) (bits & 255) ;
	    cmdbuf[3] = (uchar) mlen ;
	    rs = uc_writen(op->fd,cmdbuf,4) ;

	    if (rs < 0)
	        return rs ;

	    rs = uc_writen(op->fd,buf + i,mlen) ;

	    if (rs < 0)
	        return rs ;

	    rlen -= mlen ;
	    i += mlen ;

	} /* end while */

	if (rs >= 0)
		rs = buflen ;

	return rs ;
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
		    netorder_rint(cmdbuf,&niw) ;
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


