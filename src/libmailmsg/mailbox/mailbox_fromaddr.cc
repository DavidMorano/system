/* mailbox_fromaddr SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* get a FROM address from a mail message */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mailbox_fromaddr

	Description:
	This subroutine gets a FROM address from a mail message.

	Synopsis:
	int mailbox_fromaddr(mailbox *mbp,DATER *dp,MAILMSGFROM *fip,cchar *mfn)

	Arguments:
	mbp		pointer to mailbox object
	dp		pointer to DATER object
	fip		pointer to MAILMSGFROM object
	mfn		mailbox file-name

	Returns:
	>0		got
	==0		did not get
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<estrings.h>
#include	<dater.h>
#include	<bfile.h>
#include	<mailmsg.h>
#include	<mailmsghdrs.h>
#include	<mkx.h>
#include	<isoneof.h>
#include	<isnot.h>
#include	<iserror.h>
#include	<localmisc.h>

#include	"mailbox.h"
#include	"mailmsgfrom.h"


/* local defines */

#define	MMF	MAILMSGFROM


/* imported namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	mailmsg_loadfile(mailmsg *,bfile *) noex ;
}


/* external variables */


/* local structures */


/* forward references */

static int mailbox_proc(mailbox *,DATER *,MAILMSGFROM *,bfile *,int) noex ;

static int mailmsg_msgfrom(mailmsg *,MAILMSGFROM *) noex ;
static int mailmsg_msgtime(mailmsg *,dater *,time_t *) noex ;
static int mailmsg_hdrtime(mailmsg *,dater *,time_t *) noex ;
static int mailmsg_envtime(mailmsg *,dater *,time_t *) noex ;

static int	isNoMsg(int) noex ;


/* local variables */

static constexpr int	rsnomsg[] = {
	SR_NOMSG,
	SR_NOENT,
	0
} ;


/* exported variables */


/* exported subroutines */

int mailbox_fromaddr(mailbox *mbp,DATER *dp,MMF *fip,cchar *mfn) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = mailbox_magic(mbp,dp,fip,mfn)) >= 0) {
	    rs = SR_INVALID ;
	    if (mfn[0]) {
	        mailbox_info	mi{} ;
	        if ((rs = mailbox_getinfo(mbp,&mi)) >= 0) {
	            if (cint n = mi.nmsgs ; (n > 0)) {
		        bfile	mf ;
	                if ((rs = bopen(&mf,mfn,"r",0666)) >= 0) {
		            for (int i = 0 ; (rs >= 0) && (i < n) ; i += 1) {
			        rs = mailbox_proc(mbp,dp,fip,&mf,i) ;
			        c += rs ;
		            }
		            rs1 = bclose(&mf) ;
		            if (rs >= 0) rs = rs1 ;
		        } /* end if (mail-file) */
	            } /* end if (positive) */
	        } /* end if (mailbox_info) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mailbox_fromaddr) */


/* local subroutines */

static int mailbox_proc(mailbox *mbp,DATER *dp,MMF *fip,
		 bfile *mfp,int mi) noex {
	mailbox_mi	*mip{} ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = mailbox_msgret(mbp,mi,&mip)) >= 0) {
	    const off_t		moff = mip->moff ;
	    if ((rs = bseek(mfp,moff,SEEK_SET)) >= 0) {
		mailmsg		m ;
		if ((rs = mailmsg_start(&m)) >= 0) {
		    if ((rs = mailmsg_loadfile(&m,mfp)) >= 0) {
			time_t	t ;
			if ((rs = mailmsg_msgtime(&m,dp,&t)) > 0) {
			    c += 1 ;
			    if ((rs = mailmsgfrom_test(fip,t)) > 0) {
				rs = mailmsg_msgfrom(&m,fip) ;
			    }
			}
		    } /* end if (mailmsg_loadfile) */
		    rs1 = mailmsg_finish(&m) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (mailmsg) */
	    } /* end if (seek) */
	} /* end if (mailbox_msgret) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mailbox_proc) */

static int mailmsg_msgfrom(mailmsg *mmp,MAILMSGFROM *fip) noex {
	int		rs ;
	int		vl ;
	int		len = 0 ;
	cchar		*hn = HN_FROM ;
	cchar		*vp ;
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
	    cint	rlen = vl ;
	    char	*rbuf ;
	    if ((rs = uc_malloc((rlen+1),&rbuf)) >= 0) {
		if ((rs = mkaddrfrom(rbuf,rlen,vp,vl)) >= 0) {
	            rs = mailmsgfrom_loadfrom(fip,rbuf,rs) ;
		    len = rs ;
		}
		uc_free(rbuf) ;
	    } /* end if (memory-allocation) */
	} /* end if (ok) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (mailmsg_msgfrom) */

static int mailmsg_msgtime(mailmsg *mmp,DATER *dp,time_t *tp) noex {
	int		rs ;
	if ((rs = mailmsg_hdrtime(mmp,dp,tp)) == 0) {
	    rs = mailmsg_envtime(mmp,dp,tp) ;
	}
	return rs ;
}
/* end subroutine (mailmsg_msgtime) */

static int mailmsg_hdrtime(mailmsg *mmp,DATER *dp,time_t *tp) noex {
	int		rs ;
	int		f = false ;
	cchar		*hn = HN_FROM ;
	cchar		*vp ;
	if ((rs = mailmsg_hdrval(mmp,hn,&vp)) > 0) {
	    if ((rs = dater_setmsg(dp,vp,rs)) >= 0) {
		f = true ;
		rs = dater_gettime(dp,tp) ;
	    } else if (isInvalid(rs)) {
		rs = SR_OK ;
	    }
	} else if (isNoMsg(rs)) {
	    rs = SR_OK ;
	}
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (mailmsg_hdrtime) */

static int mailmsg_envtime(mailmsg *mmp,DATER *dp,time_t *tp) noex {
	int		rs ;
	int		f = false ;
	cchar		*vp{} ;
	if ((rs = mailmsg_envdate(mmp,0,&vp)) >= 0) {
	    if ((rs = dater_setstd(dp,vp,rs)) >= 0) {
		f = true ;
		rs = dater_gettime(dp,tp) ;
	    } else if (isInvalid(rs)) {
		rs = SR_OK ;
	    }
	} else if (isNoMsg(rs)) {
	    rs = SR_OK ;
	}
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (mailmsg_envtime) */

static int isNoMsg(int rs) noex {
	return isOneOf(rsnomsg,rs) ;
}
/* end subroutine (isNoMsg) */


