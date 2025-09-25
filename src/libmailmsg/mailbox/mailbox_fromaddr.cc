/* mailbox_fromaddr SUPPORT */
/* charset=ISO8859-1 */
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
	typedef mailmsgfrom	mmf_t ;
	int mailbox_fromaddr(mailbox *mbp,dater *dp,mmf_t *fip,cchar *mfn) noex

	Arguments:
	mbp		MAILBOX object pointer
	dp		DATER object pointer
	fip		MAILMSGFROM object pointer
	mfn		mailbox file-name

	Returns:
	>0		got
	==0		did not get
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
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

#define	MB	mailbox
#define	MMF	mailmsgfrom


/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	mailmsg_loadfile(mailmsg *,bfile *) noex ;
}


/* external variables */


/* local structures */


/* forward references */

local int mailbox_proc(mailbox *,dater *,MMF *,bfile *,int) noex ;

local int mailmsg_msgfrom(mailmsg *,MMF *) noex ;
local int mailmsg_msgtime(mailmsg *,dater *,time_t *) noex ;
local int mailmsg_hdrtime(mailmsg *,dater *,time_t *) noex ;
local int mailmsg_envtime(mailmsg *,dater *,time_t *) noex ;

local bool isNoMsg(int) noex ;


/* local variables */

constexpr int	rsnomsg[] = {
	SR_NOMSG,
	SR_NOENT,
	0
} ; /* end array (rsnomsg) */


/* exported variables */


/* exported subroutines */

int mailbox_fromaddr(mailbox *mbp,dater *dp,MMF *fip,cchar *mfn) noex {
	int		rs ;
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

local int mailbox_proc(mailbox *mbp,dater *dp,MMF *fip,
		 bfile *mfp,int mi) noex {
	mailbox_mi	*mip{} ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = mailbox_msgret(mbp,mi,&mip)) >= 0) {
	    const off_t		moff = mip->moff ;
	    if ((rs = bseek(mfp,moff,SEEK_SET)) >= 0) {
		if (mailmsg m ; (rs = mailmsg_start(&m)) >= 0) {
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

static cpcchar	hnames[] = {
    	HN_FROM,
	HN_RETURNPATH,
	HN_REPLYTO,
	HN_SENDER
} ; /* end arraya (hnames) */

local int mailmsg_hdrfrom(mailmsg *mmp,cchar **rpp) noex {
    	int		rs ;
	int		rl = 0 ; /* return-value */
	for (cauto &hn : hnames) {
	    if ((rs = mailmsg_hdrival(mmp,hn,0,rpp)) >= 0) {
	        rl = rs ;
	    } else if (isNoMsg(rs)) {
		rs = SR_OK ;
	    }
	    if (rs != 0) break ;
	} /* end for */
	if (rs == 0) {
	    rs = mailmsg_envaddress(mmp,0,rpp) ;
	    rl = rs ;
	} /* end if (empty so far) */
	return (rs >= 0) ? rl : rs ;
} /* end subroutine (mailmsg_hdrfrom) */

local int mailmsg_msgfrom(mailmsg *mmp,MMF *fip) noex {
	int		rs ;
	int		rs1 ;
	int		len = 0 ; /* return-value */
	if (cc *valp ; (rs = mailmsg_hdrfrom(mmp,&valp)) > 0) {
	    cint	vall = rs ;
	    if (char *rbuf ; (rs = libmem.mall((vall + 1),&rbuf)) >= 0) {
		cint rlen = vall ;
		if ((rs = mkaddrfrom(rbuf,rlen,valp,vall)) >= 0) {
	            rs = mailmsgfrom_loadfrom(fip,rbuf,rs) ;
		    len = rs ;
		} /* end if (mkaddrfree) */
		rs1 = libmem.free(rbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (memory-allocation) */
	} /* end if (ok) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (mailmsg_msgfrom) */

local int mailmsg_msgtime(mailmsg *mmp,dater *dp,time_t *tp) noex {
	int		rs ;
	if ((rs = mailmsg_hdrtime(mmp,dp,tp)) == 0) {
	    rs = mailmsg_envtime(mmp,dp,tp) ;
	}
	return rs ;
}
/* end subroutine (mailmsg_msgtime) */

local int mailmsg_hdrtime(mailmsg *mmp,dater *dp,time_t *tp) noex {
	int		rs ;
	int		f = false ; /* return-value */
	cchar		*hn = HN_FROM ;
	if (cchar *vp ; (rs = mailmsg_hdrval(mmp,hn,&vp)) > 0) {
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

local int mailmsg_envtime(mailmsg *mmp,dater *dp,time_t *tp) noex {
	int		rs ;
	int		f = false ; /* return-value */
	if (cchar *vp{} ; (rs = mailmsg_envdate(mmp,0,&vp)) >= 0) {
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

local bool isNoMsg(int rs) noex {
	return isOneOf(rsnomsg,rs) ;
}
/* end subroutine (isNoMsg) */


