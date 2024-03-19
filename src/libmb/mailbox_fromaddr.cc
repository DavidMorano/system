/* mailbox_fromaddr SUPPORT */
/* lang=C++20 */

/* get a FROM address from a mail message */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mailbox_fromaddr

	Description:
	This subroutine gets a FROM address from a mail message.

	Synopsis:
	int mailbox_fromaddr(MAILBOX *mbp,DATER *dp,MAILMSGFROM *fip,cchar *mfn)

	Arguments:
	mbp		pointer to MAILBOX object
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
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<estrings.h>
#include	<dater.h>
#include	<bfile.h>
#include	<mailbox.h>
#include	<mailmsg.h>
#include	<mailmsghdrs.h>
#include	<mkx.h>
#include	<isoneof.h>
#include	<isnot.h>
#include	<iserror.h>
#include	<localmisc.h>

#include	"mailmsgfrom.h"


/* local defines */


/* external subroutines */

extern "C" {
    extern int	mailmsg_loadfile(MAILMSG *,bfile *) noex ;
}


/* external variables */


/* local structures */


/* forward references */

static int mailbox_proc(MAILBOX *,DATER *,MAILMSGFROM *,bfile *,int) noex ;

static int mailmsg_msgfrom(MAILMSG *,MAILMSGFROM *) noex ;
static int mailmsg_msgtime(MAILMSG *,DATER *,time_t *) noex ;
static int mailmsg_hdrtime(MAILMSG *,DATER *,time_t *) noex ;
static int mailmsg_envtime(MAILMSG *,DATER *,time_t *) noex ;

static int	isNoMsg(int) noex ;


/* local variables */

static constexpr int	rsnomsg[] = {
	SR_NOMSG,
	SR_NOENT,
	0
} ;


/* exported variables */


/* exported subroutines */

int mailbox_fromaddr(MAILBOX *mbp,DATER *dp,MAILMSGFROM *fip,cchar *mfn) noex {
	MAILBOX_INFO	mbinfo ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;

	if (mbp == NULL) return SR_FAULT ;
	if (fip == NULL) return SR_FAULT ;
	if (mfn == NULL) return SR_FAULT ;

	if (mfn[0] == '\0') return SR_INVALID ;
	    
	if ((rs = mailbox_info(mbp,&mbinfo)) >= 0) {
	    cint	n = mbinfo.nmsgs ;
	    if (n > 0) {
		bfile	mf ;
	        if ((rs = bopen(&mf,mfn,"r",0666)) >= 0) {
		    int	i ;
		    for (i = 0 ; (rs >= 0) && (i < n) ; i += 1) {
			rs = mailbox_proc(mbp,dp,fip,&mf,i) ;
			c += rs ;
		    }
		    rs1 = bclose(&mf) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (mail-file) */
	    } /* end if (positive) */
	} /* end if (mailbox_info) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mailbox_fromaddr) */


/* local subroutines */


static int mailbox_proc(MAILBOX *mbp,DATER *dp,MAILMSGFROM *fip,
		bfile *mfp,int mi)
{
	MAILBOX_MSGINFO	msginfo ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = mailbox_msginfo(mbp,mi,&msginfo)) >= 0) {
	    const off_t	moff = msginfo.moff ;
	    if ((rs = bseek(mfp,moff,SEEK_SET)) >= 0) {
		MAILMSG		m ;
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
	} /* end if (mailbox_msginfo) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mailbox_proc) */


/* local subroutines */


static int mailmsg_msgfrom(MAILMSG *mmp,MAILMSGFROM *fip)
{
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
		if ((rs = mkbestfrom(rbuf,rlen,vp,vl)) >= 0) {
	            rs = mailmsgfrom_loadfrom(fip,rbuf,rs) ;
		    len = rs ;
		}
		uc_free(rbuf) ;
	    } /* end if (memory-allocation) */
	} /* end if (ok) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (mailmsg_msgfrom) */


static int mailmsg_msgtime(MAILMSG *mmp,DATER *dp,time_t *tp)
{
	int		rs ;
	if ((rs = mailmsg_hdrtime(mmp,dp,tp)) == 0) {
	    rs = mailmsg_envtime(mmp,dp,tp) ;
	}
	return rs ;
}
/* end subroutine (mailmsg_msgtime) */


static int mailmsg_hdrtime(MAILMSG *mmp,DATER *dp,time_t *tp)
{
	int		rs ;
	int		f = FALSE ;
	cchar		*hn = HN_FROM ;
	cchar		*vp ;
	if ((rs = mailmsg_hdrval(mmp,hn,&vp)) > 0) {
	    if ((rs = dater_setmsg(dp,vp,rs)) >= 0) {
		f = TRUE ;
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


static int mailmsg_envtime(MAILMSG *mmp,DATER *dp,time_t *tp)
{
	int		rs ;
	int		f = FALSE ;
	cchar		*vp ;
	if ((rs = mailmsg_envdate(mmp,0,&vp)) >= 0) {
	    if ((rs = dater_setstd(dp,vp,rs)) >= 0) {
		f = TRUE ;
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


