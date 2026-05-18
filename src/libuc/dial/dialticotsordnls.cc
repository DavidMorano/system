/* dialticotsordnls SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* dial out to a server listening on TI-COTS-ORD-NLS */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	dialticotsordnls

	Description:
	This subroutine will dial out to the TICOTSORD transport which
	has the NLS listener on it.

	Synopsis:
	int dialticotsordnls(cc *abuf,int alen,cc *svc,int to,int dot) noex

	Arguments:
	abuf		XTI address
	alen		address of XTI address
	svc		service specification
	to		to ('>=0' mean use one, '-1' means do not)
	dot

	Returns:
	>=0		file descriptor
	<0		error in dialing (system-return)

	Origin:
	-Dial
	-Transport
	-Interface
	-Connection
	-Oriented
	-Transport
	-Service
	-Orderly
	-Release

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<csignal>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<uxti.h>
#include	<uclibmem.h>
#include	<ucopen.h>
#include	<ucdesc.h>
#include	<ucsigset.h>
#include	<bufsizevar.hh>
#include	<sigign.h>
#include	<sbuf.h>
#include	<rmx.h>
#include	<cfdec.h>
#include	<char.h>		/* |CHAR_ISWHITE(3uc)| */
#include	<localmisc.h>

#include	"nlsdialassist.h"
#include	"dialticotsord.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	NLSBUFLEN_MULT	30


/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct dialer {
	cc	*svcp ;			/* caller supplied */
	cc	*abuf ;			/* caller supplied */
	char	*nlsbuf ;		/* allocated */
	int	alen ;			/* caller supplied */
	int	to ;			/* caller supplied */
	int	opts ;			/* caller supplied */
	int	nlslen ;		/* calculated */
	int	svcl ;			/* calculated */
	dialer(cc *ab,int al,cc *s,int t,int o) noex : abuf(ab), alen(al) {
	    svcp = s ;
	    to = t ;
	    opts = o ;
	    svcl = rmwht(s) ;
	} ; /* emd ctor */
	operator int () noex ;
	int dialout() noex ;
    } ; /* end struct (dialer) */
} /* end namespace */

/* forward references */


/* local vaiables */

static bufsizevar	maxpathlen(bufsize_mp) ;

constexpr int	igns[] = {
    	SIGPIPE,
	SIGHUP,
	0
} ; /* end array (igns) */


/* exported variables */


/* exported subroutines */

int dialticotsordnls(cc *abuf,int alen,cc *svc,int to,int opts) noex {
    	int		rs = SR_FAULT ;
	int		fd = -1 ;
	if (svc) {
	    rs = SR_INVALID ;
	    if(svc[0]) {
	        if ((rs = maxpathlen) >= 0) {
	            if (dialer dobj(abuf,alen,svc,to,opts) ; (rs = dobj) >= 0) {
		        fd = rs ;
		    }
	        } /* end if (maxpathlen) */
	    } /* end if (valid) */
	} /* end if (non-null) */
    	return (rs >= 0) ? fd : rs ;
} /* end subroutine (dialticotsordnls) */


/* local subroutines */

dialer::operator int () noex {
    	int		rs = SR_INVALID ;
	int		rs1 ;
	int		fd = -1 ;
	if (svcl > 0) {
	    if ((rs = maxpathlen) >= 0) {
	        nlslen = (rs * NLSBUFLEN_MULT) ;
	        if ((rs = lm_mall((nlslen + 1),&nlsbuf)) >= 0) {
		    {
		        rs = dialout() ;
		        fd = rs ;
		    }
		    rs1 = lm_free(nlsbuf) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } /* end if (maxpathlen) */
	} /* end if (valid) */
    	return (rs >= 0) ? fd : rs ;
} /* end method (dialer::operator) */

int dialer::dialout() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		fd = -1 ;
	if ((abuf == nullptr) || (abuf[0] == '\0')) {
	    abuf = "local" ;
	    alen = lenstr(abuf) ;
	} /* end if (default UNIX® address!) */
	if ((rs = mknlsreq(nlsbuf,nlslen,svcp,svcl)) >= 0) {
	    cint	blen = rs ;
	    if (sigign sig ; (rs = sig.start(igns)) >= 0) {
	        if ((rs = dialticotsord(abuf,alen,to,opts)) >= 0) {
	            fd = rs ;
	            if ((rs = uc_writen(fd,nlsbuf,blen)) >= 0) {
			if (char *tbuf ; (rs = lm_mn(&tbuf)) >= 0) {
			    cint tlen = rs ;
			    {
	                        rs = readnlsresp(fd,tbuf,tlen,to) ;
			    }
			    rs1 = lm_free(tbuf) ;
			    if (rs >= 0) rs = rs1 ;
			} /* end if (m-a-f) */
	            } /* end if (reading response) */
	        } /* end if (opened) */
	        rs1 = sig.finish ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (sigign) */
	    if ((rs < 0) && (fd >= 0)) {
		u_close(fd) ;
	    } /* end if (error) */
	} /* end if (mknlsreq) */
	return (rs >= 0) ? fd : rs ;
} /* end method (dialer::dialout) */


