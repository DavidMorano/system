/* pingstatmsg */

/* create and parse the internal messages */


#define	CF_DEBUGS	0		/* compile-time debugging */


/* revision history:

	= 2001-07-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

        This module contains the subroutines to make and parse the PINGSTAT
        family of messages.


******************************************************************************/


#define	PINGSTATMSG_MASTER	0


#include	<envstandards.h>	/* MUST be first to configure */

#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<fcntl.h>

#include	<usystem.h>
#include	<serialbuf.h>
#include	<stdorder.h>
#include	<localmisc.h>

#include	"pingstatmsg.h"


/* local defines */


/* external subroutines */


/* local structures */


/* local variables */


/* exported subroutines */


int pingstatmsg_update(sp,f,mbuf,mlen)
struct pingstatmsg_update	*sp ;
int			f ;
char			mbuf[] ;
int			mlen ;
{
	SERIALBUF	msgbuf ;
	int		rs ;
	int		rs1 ;

	if ((rs = serialbuf_start(&msgbuf,mbuf,mlen)) >= 0) {
	    uint	hdr ;

	    if (f) { /* read */

	        serialbuf_rui(&msgbuf,&hdr) ;
	        sp->msgtype = (hdr & 0xff) ;
	        sp->msglen = (hdr >> 8) ;

	        serialbuf_rui(&msgbuf,&sp->timestamp) ;

	        serialbuf_rs(&msgbuf,&sp->hostnamelen) ;

	        serialbuf_rstrw(&msgbuf,sp->hostname,MAXHOSTNAMELEN) ;

	    } else { /* write */
		int	len ;

	        if (sp->hostnamelen >= 0) {
	            len = MIN(sp->hostnamelen,MAXHOSTNAMELEN) ;
		} else {
	            len = strnlen(sp->hostname,MAXHOSTNAMELEN) ;
		}

	        sp->msgtype = pingstatmsgtype_update ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&msgbuf,hdr) ;

	        serialbuf_wui(&msgbuf,sp->timestamp) ;

	        serialbuf_ws(&msgbuf,len) ;

	        serialbuf_wstrw(&msgbuf,sp->hostname,len) ;

	        if ((sp->msglen = serialbuf_getlen(&msgbuf)) > 0) {
	            hdr |= (sp->msglen << 8) ;
	            stdorder_wui(mbuf,hdr) ;
	        }

	    } /* end if */

	    rs1 = serialbuf_finish(&msgbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */

	return rs ;
}
/* end subroutine (pingstatmsg_update) */


int pingstatmsg_uptime(sp,f,mbuf,mlen)
struct pingstatmsg_uptime	*sp ;
int			f ;
char			mbuf[] ;
int			mlen ;
{
	SERIALBUF	msgbuf ;
	int		rs ;
	int		rs1 ;

	if ((rs = serialbuf_start(&msgbuf,mbuf,mlen)) >= 0) {
	    uint	hdr ;

	    if (f) { /* read */

	        serialbuf_rui(&msgbuf,&hdr) ;
	        sp->msgtype = (hdr & 0xff) ;
	        sp->msglen = (hdr >> 8) ;

	        serialbuf_rui(&msgbuf,&sp->timestamp) ;

	        serialbuf_rui(&msgbuf,&sp->timechange) ;

	        serialbuf_rui(&msgbuf,&sp->count) ;

	        serialbuf_rs(&msgbuf,&sp->hostnamelen) ;

	        serialbuf_rstrw(&msgbuf,sp->hostname,MAXHOSTNAMELEN) ;

	    } else { /* write */
		int	len ;

	        if (sp->hostnamelen >= 0) {
	            len = MIN(sp->hostnamelen,MAXHOSTNAMELEN) ;
		} else {
	            len = strnlen(sp->hostname,MAXHOSTNAMELEN) ;
		}

	        sp->msgtype = pingstatmsgtype_uptime ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&msgbuf,hdr) ;

	        serialbuf_wui(&msgbuf,sp->timestamp) ;

	        serialbuf_wui(&msgbuf,sp->timechange) ;

	        serialbuf_wui(&msgbuf,sp->count) ;

	        serialbuf_ws(&msgbuf,len) ;

	        serialbuf_wstrw(&msgbuf,sp->hostname,len) ;

	        if ((sp->msglen = serialbuf_getlen(&msgbuf)) > 0) {
	            hdr |= (sp->msglen << 8) ;
	            stdorder_wui(mbuf,hdr) ;
	        }

	    } /* end if */

	    rs1 = serialbuf_finish(&msgbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */

	return rs ;
}
/* end subroutine (pingstatmsg_uptime) */


/* unknown message */
int pingstatmsg_unknown(sp,f,mbuf,mlen)
struct pingstatmsg_unknown	*sp ;
int			f ;
char			mbuf[] ;
int			mlen ;
{
	SERIALBUF	msgbuf ;
	int		rs ;
	int		rs1 ;

	if ((rs = serialbuf_start(&msgbuf,mbuf,mlen)) >= 0) {
	    uint	hdr ;

	    if (f) { /* read */

	        serialbuf_rui(&msgbuf,&hdr) ;
	        sp->msgtype = (hdr & 0xff) ;
	        sp->msglen = (hdr >> 8) ;

	    } else { /* write */

	        sp->msgtype = pingstatmsgtype_unknown ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&msgbuf,hdr) ;

	        if ((sp->msglen = serialbuf_getlen(&msgbuf)) > 0) {
	            hdr |= (sp->msglen << 8) ;
	            stdorder_wui(mbuf,hdr) ;
	        }

	    } /* end if */

	    rs1 = serialbuf_finish(&msgbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (serialbuf) */

	return rs ;
}
/* end subroutine (pingstatmsg_unknown) */


#ifdef	COMMENT
int pingstatmsg_msglen(int type)
{
	int		rs ;

	switch (type) {
	case pingstatmsgtype_update:
	    rs = PINGSTATMSG_SUPDATE ;
	    break ;
	case pingstatmsgtype_uptime:
	    rs = PINGSTATMSG_SUPTIME ;
	    break ;
	default:
	    rs = SR_INVALID ;
	    break ;
	} /* end switch */

	return rs ;
}
/* end subroutine (pingstatmsg_msglen) */
#endif /* COMMENT */


