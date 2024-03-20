/* mailbox_getfrom SUPPORT */
/* lang=C++20 */

/* get a FROM address from a mail message */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine gets a FROM address from a mail message.

	Synopsis:
	int mailbox_getfrom(MAILBOX *mbp,char *rbuf,int rlen,cc *fn,int mi) noex

	Arguments:
	mbp		pointer to MAILBOX object
	rbuf		buffer to hold result
	rlen		length of supplied result buffer
	fn		mailbox file-name
	mi		message-id (ID of message within mail-box)

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<climits>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<estrings.h>
#include	<bfile.h>
#include	<mailmsg.h>
#include	<mailmsghdrs.h>
#include	<mkx.h>
#include	<isoneof.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"mailbox.h"


/* local defines */


/* external subroutines */

extern "C" {
    extern int	mailmsg_loadfile(mailmsg *,bfile *) noex ;
}


/* external variables */


/* local structures */


/* forward references */

static int	mailmsg_fromer(mailmsg *,char *,int) noex ;
static bool	isNoMsg(int) noex ;


/* local variables */

static const int	rsnomsg[] = {
	SR_NOMSG,
	SR_NOENT,
	0
} ;


/* exported variables */


/* exported subroutines */

int mailbox_getfrom(MAILBOX *mbp,char *rbuf,int rlen,cchar *fn,int mi) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		len = 0 ;

	if (mbp == NULL) return SR_FAULT ;
	if (fn == NULL) return SR_FAULT ;
	if (rbuf == NULL) return SR_FAULT ;

	if (fn[0] == '\0') return SR_INVALID ;

	if (mi < 0) {
	    mailbox_info	mbinfo{} ;
	    rs = mailbox_getinfo(mbp,&mbinfo) ;
	    if (mbinfo.nmsgs > 0) mi = (mbinfo.nmsgs - 1) ;
	} /* end if (default) */

	if ((rs >= 0) && (mi >= 0)) {
	    mailbox_mi		*mip{} ;
	    if ((rs = mailbox_msgret(mbp,mi,&mip)) >= 0) {
		bfile		mf ;
	        if ((rs = bopen(&mf,fn,"r",0666)) >= 0) {
		    const off_t	moff = mip->moff ;
	            if ((rs = bseek(&mf,moff,SEEK_SET)) >= 0) {
			MAILMSG		m ;
	                if ((rs = mailmsg_start(&m)) >= 0) {
			    if ((rs = mailmsg_loadfile(&m,&mf)) >= 0) {
				rs = mailmsg_fromer(&m,rbuf,rlen) ;
			        len = rs ;
			    }
	                    rs1 = mailmsg_finish(&m) ;
	                    if (rs >= 0) rs = rs1 ;
	                } /* end if (mailmsg) */
	            } /* end if (seek) */
	            rs1 = bclose(&mf) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (mail-file) */
	    } /* end if (msg-info) */
	} /* end if (positive MI) */

	return (rs >= 0) ? len : rs ;
}
/* end subroutine (mailbox_getfrom) */


/* local subroutines */

static int mailmsg_fromer(mailmsg *mmp,char *rbuf,int rlen) noex {
	int		rs ;
	int		vl = 0 ;
	int		len = 0 ;
	cchar		*vp ;
	cchar		*hn = HN_FROM ;
	if ((rs = mailmsg_hdrival(mmp,hn,0,&vp)) > 0) {
	    vl = rs ;
	} else if ((rs == 0) || isNoMsg(rs)) {
	    hn = HN_RETURNPATH ;
	    if ((rs = mailmsg_hdrival(mmp,hn,0,&vp)) > 0) {
	        vl = rs ;
	    } else if ((rs == 0) || isNoMsg(rs)) {
	        hn = HN_REPLYTO ;
	        if ((rs = mailmsg_hdrival(mmp,hn,0,&vp)) > 0) {
	            vl = rs ;
	        } else if ((rs == 0) || isNoMsg(rs)) {
	            hn = HN_SENDER ;
	            if ((rs = mailmsg_hdrival(mmp,hn,0,&vp)) > 0) {
	                vl = rs ;
	            } else if ((rs == 0) || isNoMsg(rs)) {
	                rs = mailmsg_envaddress(mmp,0,&vp) ;
	                vl = rs ;
		    }
		}
	    }
	}
	if ((rs >= 0) && (vl > 0)) {
	    rs = mkbestfrom(rbuf,rlen,vp,vl) ;
	    len = rs ;
	}
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (mailmsg_fromer) */

static bool isNoMsg(int rs) noex {
	return isOneOf(rsnomsg,rs) ;
}
/* end subroutine (isNoMsg) */


