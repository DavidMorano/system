/* dialcprogmsg SUPPORT */
/* lang=C++20 */

/* create and parse the internal messages */
/* version %I% last-modified %G% */


/* revision history:

	= 2003-07-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2003 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This module contains the code to make and parse the internal
	messages that are used in this whole server facility.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/socket.h>
#include	<sys/uio.h>
#include	<arpa/inet.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<usystem.h>
#include	<sockaddress.h>
#include	<serialbuf.h>
#include	<localmisc.h>

#include	"dialcprogmsg.h"


/* local defines */

#define	DMSG_END	DIALCPROGMSG_END
#define	DMSG_LIGHT	DIALCPROGMSG_LIGHT


/* external subroutines */


/* local structures */


/* local variables */


/* exported subroutines */

int dialcprogmsg_end(char *mbuf,int mlen,bool f,DMSG_END *sp) noex {
	serialbuf	msgbuf ;
	int		rs ;
	int		rs1 ;
	if ((rs = serialbuf_start(&msgbuf,mbuf,mlen)) >= 0) {
	    if (f) { /* read */
	        serialbuf_ruc(&msgbuf,&sp->type) ;
	        serialbuf_rus(&msgbuf,&sp->len) ;
	        serialbuf_rus(&msgbuf,&sp->flags) ;
	        serialbuf_ri(&msgbuf,&sp->opts) ;
	    } else { /* write */
	        sp->type = dialcprogmsgtype_end ;
	        serialbuf_wuc(&msgbuf,sp->type) ;
	        serialbuf_wus(&msgbuf,4) ;
	        serialbuf_wus(&msgbuf,sp->flags) ;
	        serialbuf_wi(&msgbuf,sp->opts) ;
	    } /* end if */
	    rs1 = serialbuf_finish(&msgbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */
	return rs ;
}
/* end subroutine (dialcprogmsg_end) */

int dialcprogmsg_light(char *mbuf,int mlen,bool f,DMSG_LIGHT *sp) noex {
	serialbuf	msgbuf ;
	int		rs ;
	int		rs1 ;
	if ((rs = serialbuf_start(&msgbuf,mbuf,mlen)) >= 0) {
	    ushort	usw ;
	    if (f) { /* read */
	        serialbuf_ruc(&msgbuf,&sp->type) ;
	        serialbuf_rus(&msgbuf,&sp->len) ;
	        serialbuf_rus(&msgbuf,&sp->salen1) ;
	        serialbuf_rus(&msgbuf,&sp->salen2) ;
	        serialbuf_robj(&msgbuf,&sp->saout,int(sp->salen1)) ;
	        serialbuf_robj(&msgbuf,&sp->saerr,int(sp->salen2)) ;
	    } else { /* write */
	        sp->type = dialcprogmsgtype_light ;
	        serialbuf_wuc(&msgbuf,sp->type) ;
	        usw = sp->salen1 + sp->salen2 + (2 * szof(ushort)) ;
	        serialbuf_wus(&msgbuf,usw) ;
	        serialbuf_wus(&msgbuf,sp->salen1) ;
	        serialbuf_wus(&msgbuf,sp->salen2) ;
	        serialbuf_wobj(&msgbuf,&sp->saout,int(sp->salen1)) ;
	        serialbuf_wobj(&msgbuf,&sp->saerr,int(sp->salen2)) ;
	    } /* end if */
	    rs1 = serialbuf_finish(&msgbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */
	return rs ;
}
/* end subroutine (dialcprogmsg_light) */


