/* msfilee SUPPORT */
/* lang=C20 */

/* machine status entry */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-06-01, David A­D­ Morano
	This is a whole rdebugwrite of the marshalling for dealing
	with MS entries. The previous stuff (now eradicated!) was
	too error-prone and inflexible to deal with partial updates.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	These subroutines provide for marshalling (and unmarshalling)
	of an MS file entry.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<inttypes.h>
#include	<usystem.h>
#include	<serialbuf.h>
#include	<stdorder.h>
#include	<localmisc.h>

#include	"msfilee.h"


/* local defines */


/* exported subroutines */

int msfilee_all(MSFILEE_ALL *ep,int f_read,char *buf,int buflen) noex {
	serialbuf	msgbuf ;
	int		rs ;
	int		rs1 ;
	int		i ;

#if	CF_DEBUGS
	char	timebuf[TIMEBUFLEN + 1] ;
#endif

#if	CF_DEBUGS
	debugprintf("msfilee_all: buf=%p buflen=%d\n",buf,buflen) ;
#endif

	if (ep == NULL)
	    return SR_FAULT ;

	if (buflen < 0)
	    buflen = INT_MAX ;

	if ((rs = serialbuf_start(&msgbuf,buf,buflen)) >= 0) {

	if (f_read) {

	    serialbuf_rui(&msgbuf,&ep->btime) ;

	    serialbuf_rui(&msgbuf,&ep->atime) ;

	    serialbuf_rui(&msgbuf,&ep->utime) ;

	    serialbuf_rui(&msgbuf,&ep->dtime) ;

	    serialbuf_rui(&msgbuf,&ep->stime) ;

	    for (i = 0 ; i < 3 ; i += 1) {
	        serialbuf_rui(&msgbuf,(ep->la + i)) ;
	    }

	    serialbuf_rui(&msgbuf,&ep->nproc) ;

	    serialbuf_rui(&msgbuf,&ep->nuser) ;

	    serialbuf_rui(&msgbuf,&ep->pmtotal) ;

	    serialbuf_rui(&msgbuf,&ep->pmavail) ;

	    serialbuf_rui(&msgbuf,&ep->speed) ;

	    serialbuf_rui(&msgbuf,&ep->ncpu) ;

	    serialbuf_rui(&msgbuf,&ep->nodehash) ;

	    serialbuf_rui(&msgbuf,&ep->pid) ;

	    serialbuf_rus(&msgbuf,&ep->flags) ;

	    serialbuf_rstrn(&msgbuf,ep->nodename,MSFILEE_LNODENAME) ;

	    ep->nodename[MSFILEE_LNODENAME] = '\0' ;

	} else {

	    serialbuf_wui(&msgbuf,ep->btime) ;

	    if (ep->atime != 0) {
	        serialbuf_wui(&msgbuf,ep->atime) ;
	    } else {
	        serialbuf_adv(&msgbuf,sizeof(uint)) ;
	    }

	    serialbuf_wui(&msgbuf,ep->utime) ;

	    serialbuf_wui(&msgbuf,ep->dtime) ;

	    serialbuf_wui(&msgbuf,ep->stime) ;

	    for (i = 0 ; i < 3 ; i += 1) {
	        serialbuf_wui(&msgbuf,ep->la[i]) ;
	    }

	    serialbuf_wui(&msgbuf,ep->nproc) ;

	    serialbuf_wui(&msgbuf,ep->nuser) ;

	    serialbuf_wui(&msgbuf,ep->pmtotal) ;

	    serialbuf_wui(&msgbuf,ep->pmavail) ;

	    serialbuf_wui(&msgbuf,ep->speed) ;

	    serialbuf_wui(&msgbuf,ep->ncpu) ;

	    serialbuf_wui(&msgbuf,ep->nodehash) ;

	    serialbuf_wui(&msgbuf,ep->pid) ;

	    serialbuf_wus(&msgbuf,ep->flags) ;

	    serialbuf_wstrn(&msgbuf,ep->nodename,MSFILEE_LNODENAME) ;

	} /* end if */

	    rs1 = serialbuf_finish(&msgbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */

	return rs ;
}
/* end subroutine (msfilee_all) */

int msfilee_la(MSFILEE_LA *ep,int f_read,char *buf,int) noex {
	int		rs ;
	int		i ;
	char		*bp = (char *) (buf + MSFILEE_OLA) ;

	if (ep == NULL)
	    return SR_FAULT ;

	if (f_read) {
	    for (i = 0 ; i < 3 ; i += 1) {
		stdorder_rui(bp,&ep->la[i]) ;
		bp += sizeof(uint) ;
	    }
	} else {
	    for (i = 0 ; i < 3 ; i += 1) {
		stdorder_wui(bp,ep->la[i]) ;
		bp += sizeof(uint) ;
	    }
	} /* end if */

	rs = (MSFILEE_OLA + MSFILEE_LLA) ;
	return rs ;
}
/* end subroutine (msfilee_la) */

int msfilee_atime(MSFILEE_ATIME *ep,int f_read,char *buf,int) noex {
	int		rs ;
	char		*bp = (char *) (buf + MSFILEE_OATIME) ;

	if (ep == NULL) return SR_FAULT ;

	if (f_read) {
	    stdorder_rui(bp,&ep->atime) ;
	} else {
	    stdorder_wui(bp,ep->atime) ;
	} /* end if */

	rs = MSFILEE_OATIME + MSFILEE_LATIME ;
	return rs ;
}
/* end subroutine (msfilee_atime) */

int msfilee_utime(MSFILEE_UTIME *ep,int f_read,char *buf,int) noex {
	int		rs ;
	char		*bp = (char *) (buf + MSFILEE_OUTIME) ;

	if (ep == NULL)
	    return SR_FAULT ;

	if (f_read) {
	    stdorder_rui(bp,&ep->utime) ;
	} else {
	    stdorder_wui(bp,ep->utime) ;
	} /* end if */

	rs = (MSFILEE_OUTIME + MSFILEE_LUTIME) ;
	return rs ;
}
/* end subroutine (msfilee_utime) */

int msfilee_dtime(MSFILEE_DTIME *ep,int f_read,char *buf,int) noex {
	int		rs ;
	char		*bp = (char *) (buf + MSFILEE_ODTIME) ;

	if (ep == NULL)
	    return SR_FAULT ;

	if (f_read) {
	    stdorder_rui(bp,&ep->dtime) ;
	} else {
	    stdorder_wui(bp,ep->dtime) ;
	} /* end if */

	rs = MSFILEE_ODTIME + MSFILEE_LDTIME ;
	return rs ;
}
/* end subroutine (msfilee_dtime) */

int msfilee_stime(MSFILEE_STIME *ep,int f_read,char *buf,int) noex {
	int		rs ;
	char		*bp = (char *) (buf + MSFILEE_OSTIME) ;

	if (ep == NULL)
	    return SR_FAULT ;

	if (f_read) {
	    stdorder_rui(bp,&ep->stime) ;
	} else {
	    stdorder_wui(bp,ep->stime) ;
	} /* end if */

	rs = (MSFILEE_OSTIME + MSFILEE_LSTIME) ;
	return rs ;
}
/* end subroutine (msfilee_stime) */


