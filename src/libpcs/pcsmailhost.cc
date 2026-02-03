/* pcsmailhost SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* get the mailhost for the host that we are on */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-01, David A­D­ Morano
	This code module was completely rewritten to replace any
	original garbage that was here before.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	pcsmailhost

	Description:
	This subroutine is used to find the mailhost for a given user.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<sfx.h>
#include	<sncpyx.h>
#include	<snwcpy.h>
#include	<mkpathx.h>
#include	<filereadln.h>
#include	<localmisc.h>

#include	"pcsmailhost.h"

#pragma		GCC dependency		"mod/uconstants.ccm"

import uconstants ;			/* |varname(3u)| */

/* local defines */

#ifndef	MAILHOSTFNAME
#define	MAILHOSTFNAME		"etc/mailhost"
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* forward references */

namespace {
    struct hoster {
	cchar	*pr ;
	cchar	*un ;
	char	*rbuf ;
	int	rlen ;
	hoster(cc *p,char *b,int l,cc *u) noex : pr(p), rbuf(b), rlen(l) {
	    un = u ;
	} ; /* end ctor */
	operator int () noex ;
	int tryvar() noex ;
	int tryfile() noex ;
	int trydef() noex ;
    } ; /* end struct (hoster) */
    typedef int (hoster::*hoster_m)() noex ;
} /* end namespace */


/* local variables */

constexpr hoster_m	mems[] = {
    	&hoster::tryvar,
	&hoster::tryfile,
	&hoster::trydef
} ; /* end array */

cchar			fn[] = MAILHOSTFNAME ;


/* exported variables */


/* exported subroutines */

int pcsmailhost(cchar *pr,char *rbuf,int rlen,cchar *un) noex {
    	int		rs = SR_FAULT ;
	if (pr && rbuf) {
	    rs = SR_INVALID ;
	    if (pr[0]) {
		hoster ho(pr,rbuf,rlen,un) ;
		rs = ho ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (pcsmailhost) */


/* local subroutines */

hoster::operator int () noex {
	int		rs = SR_OK ;
	for (cauto &m : mems) {
	    rs = (this->*m)() ;
	    if (rs != 0) break ;
	} /* end for */
	return rs ;
} /* end method (hoster::operator) */

int hoster::tryvar() noex {
    	static cchar	*hostp = getenv(varname.mailhost) ;
	int		rs = SR_OK ;
	int		len = 0 ; /* return-value */
	if (hostp) {
	    rs = sncpy(rbuf,rlen,hostp) ;
	    len = rs ;
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end method (hoster::tryvar) */

int hoster::tryfile() noex {
    	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	if (char *pbuf ; (rs = lm_mp(&pbuf)) >= 0) {
	    if ((rs = mkpath(pbuf,pr,fn)) >= 0) {
	        if (char *lbuf ; (rs = lm_ml(&lbuf)) >= 0) {
	            if ((rs = filereadln(pbuf,lbuf,rs)) > 0) {
			cchar *cp ;
			if (int cl ; (cl = sfnext(lbuf,rs,&cp)) > 0) {
			    rs = snwcpy(rbuf,rlen,cp,cl) ;
			    len = rs ;
			} /* end if (sfnext) */
		    } /* end if (filereadln) */
	            rs1 = lm_free(lbuf) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } /* end if (mkpath) */
	    rs1 = lm_free(pbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? len : rs ;
} /* end method (hoster::tryfile) */

int hoster::trydef() noex {
    	cchar *mh = sysword.w_mailhost ;
    	return sncpy(rbuf,rlen,mh) ;
} /* end method (hoster::trydef) */


