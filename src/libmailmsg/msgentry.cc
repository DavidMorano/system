/* msgentry SUPPORT */
/* lang=C++20 */

/* Message-Entry */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This object represents a Message-Entry.
	It is used within the MAILMSGSTAGE object (unless used elsewhere
	also).

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<climits>
#include	<cstdlib>
#include	<cstring>		/* |memcpy(3c)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<mallocxx.h>
#include	<fdliner.h>
#include	<mailmsghdrs.h>
#include	<mailmsgmathdr.h>
#include	<mailmsg.h>
#include	<comparse.h>
#include	<mhcom.h>
#include	<strn.h>
#include	<six.h>
#include	<mkpathx.h>
#include	<hdrextnum.h>
#include	<char.h>
#include	<hasx.h>
#include	<localmisc.h>

#include	"msgentry.h"


/* local defines */


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int msgentry_start(msgentry *mep) noex {
	int		rs = SR_FAULT ;
	if (mep) {
	    memclear(mep) ;
	    mep->clines = -1 ;
	    mep->clen = -1 ;
	    mep->blen = -1 ;
	    mep->boff = -1 ;
	    rs = mailmsg_start(&mep->m) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (msgentry_start) */

int msgentry_finish(msgentry *mep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (mep) {
	    rs = SR_OK ;
	    mep->boff = -1 ;
	    mep->blen = -1 ;
	    {
	        rs1 = mailmsg_finish(&mep->m) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (msgentry_finish) */

int msgentry_loadline(msgentry *mep,cchar *lp,int ll) noex {
	int		rs = SR_OK ;
	if (ll > 0) {
	    rs = mailmsg_loadline(&mep->m,lp,ll) ;
	}
	return rs ;
}
/* end subroutine (msgentry_loadline) */

int msgentry_loadhdrs(msgentry *mep,fdliner *lsp) noex {
	int		rs ;
	int		ll ;
	int		tlen = 0 ;
	int		f_eoh = false ;
	cchar		*lp ;
	while ((rs = fdliner_getln(lsp,&lp)) > 0) {
	    ll = rs ;
	    tlen += ll ;
	    f_eoh = (lp[0] == '\n') ;
	    if (! f_eoh) {
	        rs = mailmsg_loadline(&mep->m,lp,ll) ;
	    }
	    if (rs < 0) break ;
	    fdliner_done(lsp) ;
	    if (f_eoh) break ;
	} /* end while */
	mep->f.eoh = f_eoh ;
	return (rs >= 0) ? tlen : rs ;
}
/* end subroutine (msgentry_loadhdrs) */

int msgentry_getclen(msgentry *mep) noex {
	int		rs = SR_OK ;
	int		clen ;
	cchar		*kn = HN_CLEN ;
	clen = mep->clen ;
	if (! mep->f.clen) {
	    mep->f.clen = true ; /* once-flag */
	    mep->clen = -1 ;
	    clen = msgentry_gethdrnum(mep,kn) ;
	    if (clen >= 0) {
	        mep->hdr.clen = true ;
	        mep->clen = clen ;
	    }
	} /* end if (only once) */

	return (rs >= 0) ? clen : rs ;
}
/* end subroutine (msgentry_getclen) */

int msgentry_setclen(msgentry *mep,int clen) noex {
	int		rs = SR_FAULT ;
	if (mep) {
	    rs = SR_OK ;
	    mep->f.clen = true ;
	    mep->clen = clen ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (msgentry_setclen) */

int msgentry_setflags(msgentry *mep) noex {
	int		rs ;
	if ((rs = msgentry_setct(mep)) >= 0) {
	    if ((rs = msgentry_setce(mep)) >= 0) {
	        mep->f.plaintext = mep->f.ctplain && mep->f.ceplain ;
	    }
	}
	return rs ;
}
/* end subroutine (msgentry_setflags) */

int msgentry_setct(msgentry *mep) noex {
	mhcom		hc ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		hl ;
	cchar		*hp ;
	mep->f.ctplain = true ;
	if ((hl = mailmsg_hdrval(&mep->m,HN_CTYPE,&hp)) > 0) {
	    mep->hdr.ctype = true ;
	    if ((rs = mhcom_start(&hc,hp,hl)) >= 0) {
		int	vl ;
		cchar	*vp ;
	        if ((vl = mhcom_getval(&hc,&vp)) > 0) {
	            if (cchar *tp ; (tp = strnchr(vp,vl,';')) != nullptr) {
	                vl = (tp - vp) ;
		    }
	            rs1 = sisub(vp,vl,"text") ;
	            if ((rs1 >= 0) && (strnchr(vp,vl,'/') != nullptr)) {
	                rs1 = sisub(vp,vl,"plain") ;
		    }
	            mep->f.ctplain = (rs1 >= 0) ;
	        } /* end if */
	        rs1 = mhcom_finish(&hc) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (mhcom) */
	} /* end if (positive) */
	return rs ;
}
/* end subroutine (msgentry_setct) */

int msgentry_setce(msgentry *mep) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		hl ;
	cchar		*hp ;
	mep->f.ceplain = true ;
	if ((hl = mailmsg_hdrval(&mep->m,HN_CENCODING,&hp)) > 0) {
	    comparse	com ;
	    int		vl ;
	    cchar	*tp ;
	    cchar	*vp ;
	    mep->hdr.cencoding = true ;
	    if ((rs = comparse_start(&com,hp,hl)) >= 0) {
	        if ((vl = comparse_getval(&com,&vp)) > 0) {
	            if ((tp = strnchr(vp,vl,';')) != nullptr) {
	                vl = (tp - vp) ;
	            }
	            rs1 = sisub(vp,vl,"7bit") ;
	            if (rs1 < 0) {
	                rs1 = sisub(vp,vl,"8bit") ;
		    }
	            mep->f.ceplain = (rs1 >= 0) ;
	        } /* end if */
	        rs1 = comparse_finish(&com) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (comparse) */
	} /* end if (mailmsg_hdrval) */
	return rs ;
}
/* end subroutine (msgentry_setce) */

int msgentry_getclines(msgentry *mep) noex {
	int		rs = SR_OK ;
	int		clines = 0 ;
	if (! mep->f.clines) {
	    cchar		*kn = HN_CLINES ;
	    mep->f.clines = true ; /* once-flag */
	    mep->clines = -1 ;
	    if ((clines = msgentry_gethdrnum(mep,kn)) >= 0) {
	        mep->hdr.clines = true ;
	        mep->clines = clines ;
	    }
	} else {
	    clines = mep->clines ;
	}
	return (rs >= 0) ? clines : rs ;
}
/* end subroutine (msgentry_getclines) */

int msgentry_setclines(msgentry *mep,int clines) noex {
	mep->f.clines = true ;
	mep->clines = clines ;
	return SR_OK ;
}
/* end subroutine (msgentry_setclines) */

int msgentry_setoff(msgentry *mep,off_t boff) noex {
	mep->boff = boff ;
	return SR_OK ;
}
/* end subroutine (msgentry_setoff) */

int msgentry_setlen(msgentry *mep,int blen) noex {
	mep->blen = blen ;
	return SR_OK ;
}
/* end subroutine (msgentry_setoff) */

int msgentry_gethdrnum(msgentry *mep,cchar *kn) noex {
	mailmsg		*mmp = &mep->m ;
	int		rs = SR_NOTFOUND ;
	int		hl ;
	int		v = -1 ;
	cchar		*hp ;
	for (int i = 0 ; (hl = mailmsg_hdrival(mmp,kn,i,&hp)) >= 0 ; i += 1) {
	    if (hl > 0) {
	        rs = hdrextnum(hp,hl) ;
	        v = rs ;
	    }
	    if (rs >= 0) break ;
	} /* end for */
	return (rs >= 0) ? v : rs ;
}
/* end subroutine (msgentry_gethdrnum) */


