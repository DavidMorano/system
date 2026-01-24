/* sysdialer_loadexts SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* system-dialer storage object */
/* version %I% last-modified %G% */


/* revision history:

	- 2003-11-04, David A­D­ Morano
	This subroutine was adopted for use from the DWD program.

*/

/* Copyright © 2003 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	sysdialer

	Description:
	This object manages what system-dialers (SYSDIALER) have
	been loaded so far.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<ctime>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<usupport.h>		/* |getustime(3u)| */
#include	<uclibmem.h>
#include	<vecstr.h>
#include	<fsdir.h>
#include	<strn.h>		/* |strnrchr(3uc)| */
#include	<strx.h>
#include	<matstr.h>		/* |matstr(3uc)| */
#include	<localmisc.h>

#include	"sysdialer.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	NEXTS		5		/* number of extensions (why?) */


/* imported namespaces */

using sysdialer_util::exts ;		/* variable */


/* local typedefs */

typedef sysdialer_fext	fext ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int	getext(fext *,cchar *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

namespace sysdialer_util {
    int vecstr_loadexts(vecstr *elp,cc *dname,cc *name,int namelen) noex {
	int		rs ;
	int		rs1 ;
	int		nl ;
	int		c = 0 ; /* return-value */
	if (char *nbuf ; (rs = lm_mp(&nbuf)) >= 0) ylikely {
	    cint	nlen = rs ;
	    if (fsdir dir ; (rs = dir.open(dname)) >= 0) ylikely {
	        for (fsdir_ent de ; (rs = dir.read(&de,nbuf,nlen)) > 0 ; ) {
		    fext	e ;
		    cint	dnl = rs ;
	            cchar	*dnp = de.name ;
	            if (dnl < namelen) continue ;
		    nl = getext(&e,dnp,dnl) ;
	            if (nl != namelen) continue ;
 		    if (strncmp(dnp,name,namelen) != 0) continue ;
	            if ((e.exl == 0) || (matstr(exts,e.exp,e.exl) >= 0)) {
	                c += 1 ;
	                if ((rs = elp->add(e.exp,e.exl)) >= 0) {
	                    if (c >= NEXTS) break ;
			}
	            } /* end if (got a match) */
		    if (rs < 0) break ;
	        } /* end for (directory entries) */
	        rs1 = dir.close ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (fsdir) */
	    rs1 = lm_free(nbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? c : rs ;
    } /* end subroutine (vecstr_loadexts) */
} /* end namespace (sysdialer_util) */

local int getext(fext *ep,cchar *namep,int namel) noex {
    	cnullptr	np{} ;
	int		mnl = lenstr(namep,namel) ;
	ep->exp = (namep + mnl) ;
	ep->exl = 0 ;
	if (cchar *tp ; (tp = strnrchr(namep,mnl,'.')) != np) {
	    ep->exp = (tp + 1) ;
	    ep->exl = intconv(namep + mnl - (tp + 1)) ;
	    mnl = intconv(tp - namep) ;
	}
	return mnl ;
} /* end subroutine (getext) */


