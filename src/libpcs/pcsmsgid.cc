/* pcsmsgid SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* create a mail message ID (for PCS) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-01, David A­D­ Morano
	This subroutine is originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	pcsmsgid

  	Description:
	This subroutine is used to create a mail message ID for PCS
	programs.

	Synopsis:
	int pcsmsgid(cchar *pcsroot,char *rbuf,int rlen) noex

	Arguments:
	pcsroot		PCS program root path
	rbuf		caller supplied buffer to place result in
	rlen		length of caller supplied buffer

	Returns:
	>=0		length of returned ID
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* |getustime(3u)| */
#include	<usupport.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<bufsizeget.h>		/* LIBUC */
#include	<getnodedomain.h>	/* LIBUC */
#include	<sbuf.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<pcsgetserial.h>	/* LIBPCS */

#include	"pcsmsgid.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct variables {
	int	nodenamelen ;
	int	hostnamelen ;
	int	usernamelen ;
	operator int () noex ;
    } ; /* end struct (variables) */
} /* end namespace */


/* forward references */

local int mkstr(char *,int,cchar *,cchar *,int) noex ;


/* local variables */

static variables	var ;


/* exported variables */


/* exported subroutines */

int pcsmsgid(cchar *pr,char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rv = 0 ; /* return-value */
	if (pr && rbuf) ylikely {
	    rs = SR_INVALID ;
	    if (pr[0]) ylikely {
		if (static cint	rsv = var ; (rs = rsv) >= 0) ylikely {
		   cint nlen = var.nodenamelen ;
		   cint hlen = var.hostnamelen ;
	           if ((rs = pcsgetserial(pr)) >= 0) ylikely {
		       cint	sz = ((nlen + 1) + (hlen + 1)) ;
	               cint	sn = rs ;
		       if (char *a ; (rs = lm_mall(sz,&a)) >= 0) ylikely {
	                   char	*nn = (a + 0) ;
	                   char	*dn = (a + (nlen + 1)) ;
	                   if ((rs = getnodedomain(nn,dn)) >= 0) ylikely {
	                       rs = mkstr(rbuf,rlen,dn,nn,sn) ;
			       rv = rs;
	                   } /* end if (getnodedomain) */
		           rs1 = lm_free(a) ;
	        	   if (rs >= 0) rs = rs1 ;
		       } /* end if (m-a-f) */
	           } /* end if (pcsgetserial) */
		} /* end if (variables) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rv : rs ;
} /* end subroutine (pcsmsgid) */


/* local subroutines */

local int mkstr(char *rp,int rl,cchar *dn,cchar *nn,int sn) noex {
    	custime		dt = getustime ;
	int		rs ;
	if (sbuf sb ; (rs = sb.start(rp,rl)) >= 0) ylikely {
	    cuint	tv = conv<uint>(dt) ;
	    cint	pid = getpid() ;
	    cint	nl = lenstr(nn) ;
	    int		len ;
	    {
	        clong	hid = gethostid() ;
	        if (nl > var.usernamelen) {
	            cint iid = int(hid) ;
	            sb.hex(iid) ;
	            sb.chr('-') ;
	        } else {
	            sb.strw(nn,nl) ;
	        }
	        sb.dec(pid) ;
	        sb.chr('.') ;
	        sb.hex(tv) ;
	        sb.chr('.') ;
	        sb.dec(sn) ;
	        sb.chr('@') ;
	        sb.strw(dn) ;
	    } /* end block */
	    len = sb.finish ;
	    if (rs >= 0) rs = len ;
	} /* end if (sbuf) */
	return rs ;
} /* end subroutine (pcsmsgid_join) */

variables::operator int () noex {
    	int		rs ;
	if ((rs = bufsizeget(bufsize_nn)) >= 0) ylikely {
	    nodenamelen = rs ;
	    if ((rs = bufsizeget(bufsize_hostname)) >= 0) ylikely {
	        hostnamelen = rs ;
	        if ((rs = bufsizeget(bufsize_un)) >= 0) ylikely {
		    usernamelen = rs ;
		}
	    }
	} /* end if */
	return rs ;
} /* end method (variables::operator) */


