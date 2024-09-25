/* rmermsg SUPPORT */
/* lang=C++20 */

/* create and parse the internal messages */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-07-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

	Name:
	rmermsg

	Description:
	This module contains the code to make and parse the internal
	messages that are used in this whole server facility.

******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>
#include	<serialbuf.h>
#include	<stdorder.h>
#include	<localmisc.h>

#include	"rmermsg.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int rmermsg_fname(sp,f,mbuf,mlen)
struct rmermsg_fname	*sp ;
int			f ;
char			mbuf[] ;
int			mlen ;
{
	serialbuf	msgbuf ;
	int		rs ;
	int		rs1 ;

	if ((rs = serialbuf_start(&msgbuf,mbuf,mlen)) >= 0) {
	    uint	hdr ;
	    int		v ;

	    if (f) { /* read */

	        serialbuf_rui(&msgbuf,&hdr) ;
	        sp->msgtype = (hdr & 0xff) ;
	        sp->msglen = (hdr >> 8) ;

	        serialbuf_rui(&msgbuf,&sp->tag) ;

	        serialbuf_rui(&msgbuf,&sp->delay) ;

	        serialbuf_ri(&msgbuf,&v) ;
	        sp->uid = v ;

	        serialbuf_rstrw(&msgbuf,sp->fname,MAXPATHLEN) ;

	    } else { /* write */

	        sp->msgtype = rmermsgtype_fname ;
	        hdr = sp->msgtype ;
	        serialbuf_wui(&msgbuf,hdr) ;

	        serialbuf_wui(&msgbuf,sp->tag) ;

	        serialbuf_wui(&msgbuf,sp->delay) ;

	        v = sp->uid ;
	        serialbuf_wi(&msgbuf,v) ;

	        serialbuf_wstrw(&msgbuf,sp->fname,-1) ;

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
/* end subroutine (rmermsg_response) */

int rmermsg_unknown(sp,f,mbuf,mlen)
struct rmermsg_unknown	*sp ;
int			f ;
char			mbuf[] ;
int			mlen ;
{
	serialbuf	msgbuf ;
	int		rs ;
	int		rs1 ;

	if ((rs = serialbuf_start(&msgbuf,mbuf,mlen)) >= 0) {
	    uint	hdr ;

	    if (f) { /* read */

	        serialbuf_rui(&msgbuf,&hdr) ;
	        sp->msgtype = (hdr & 0xff) ;
	        sp->msglen = (hdr >> 8) ;

	    } else { /* write */

	        sp->msgtype = rmermsgtype_unknown ;
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
/* end subroutine (rmermsg_unknown) */


