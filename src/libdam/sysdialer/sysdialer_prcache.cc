/* sysdialer_prcache SUPPORT */
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
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<dlfcn.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strcmp(3c)| */
#include	<new>			/* |nothrow(3c++)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<usupport.h>		/* |getustime(3u)| */
#include	<uclibmem.h>
#include	<getbufsize.h>
#include	<getnodedomain.h>
#include	<vecobj.h>
#include	<vecstr.h>
#include	<fsdir.h>
#include	<ids.h>
#include	<dirseen.h>
#include	<strn.h>		/* |strnrchr(3uc)| */
#include	<strx.h>
#include	<sncpyx.h>
#include	<strwcpy.h>
#include	<mkpathx.h>
#include	<mkfname.h>
#include	<mkpr.h>
#include	<pathclean.h>
#include	<isoneof.h>
#include	<ischarx.h>
#include	<isnot.h>		/* |isNotPresent(3uc)| */
#include	<localmisc.h>

#include	"sysdialer.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	SD		sysdialer
#define	SD_FL		sysdialer_fl
#define	SD_ENT		sysdialer_ent
#define	SD_CALLS	sysdialer_calls
#define	SD_INFO		sysdialer_info
#define	SD_ARGS		sysdialer_args
#define	SD_PRC		sysdialer_prc
#define	SD_MOD		sysdialer_mod
#define	SD_MAGIC	SYSDIALER_MAGIC
#define	SD_DNAME	"sysdialers"


/* imported namespaces */

using sysdialer_util::var ;		/* variable */
using sysdialer_util::prnames ;		/* variable */


/* local typedefs */

typedef SD_ENT		ent ;
typedef SD_MOD		mod ;
typedef SD_PRC		prcache ;
typedef SD_ENT *	entp ;


/* external subroutines */


/* external variables */
 

/* local structures */


/* forward references */

local int celems(cpcchar *carr) noex {
    	int	c = 0 ;
	while (! carr[c]) {
	    c += 1 ;
	}
	return c ;
} /* end subroutine (celems) */


/* local variables */

cint		nprs = celems(prnames) ;


/* exported variables */


/* exported subroutines */


/* private subroutines */

int prcache_start(prcache *pcp) noex {
	cint		osz = (nprs + 1) * szof(char *) ;
	int		rs ;
	pcp->domainname = nullptr ;
	if (void *vp ; (rs = lm_mall(osz,&vp)) >= 0) {
	    memclear(vp,osz) ;
	    pcp->prs = ccharpp(vp) ;
	} /* end if (memory-allocation) */
	return rs ;
} /* end subroutine (prcache_start) */

int prcache_finish(prcache *pcp) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (pcp->domainname != nullptr) {
	    void *vp = voidp(pcp->domainname) ;
	    rs1 = lm_free(vp) ;
	    if (rs >= 0) rs = rs1 ;
	    pcp->domainname = nullptr ;
	} /* end if (non-null) */
	if (pcp->prs != nullptr) {
	    for (int i = 0 ; i < nprs; i += 1) {
		if (pcp->prs[i] != nullptr) {
		    void *vp = voidp(pcp->prs[i]) ;
		    rs1 = lm_free(vp) ;
	    	    if (rs >= 0) rs = rs1 ;
		}
	    } /* end for */
	    void *vp = voidp(pcp->prs) ;
	    rs1 = lm_free(vp) ;
	    if (rs >= 0) rs = rs1 ;
	    pcp->prs = nullptr ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (prcache_finish) */

local int prcache_domain(prcache *pcp,char *dnbuf,int) noex {
    	cnullptr	np{} ;
    	int		rs = SR_OK ;
	if (pcp->domainname == np) {
	    if ((rs = getnodedomain(np,dnbuf)) >= 0) {
		if (cchar *cp ; (rs = lm_strw(dnbuf,-1,&cp)) >= 0) {
		    pcp->domainname = cp ;
		}
	    } /* end if (getnodedomain) */
	} /* end if */
	return rs ;
} /* end subroutine (prcache_domain) */

int prcache_lookup(prcache *pcp,int i,cchar **rpp) noex {
    	cnullptr	np{} ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		len = 0 ;
	*rpp = nullptr ;
	if (i < nprs) {
	    cint	maxpath = var.maxpathlen ;
	    cint	asz = ((var.maxpathlen + 1) * 2) ;
	    int		ai = 0 ;
	    if (char *a ; (rs = lm_mall(asz,&a)) >= 0) {
	        char *prbuf = (a + ((maxpath + 1) * ai++)) ;
	        char *dnbuf = (a + ((maxpath + 1) * ai++)) ;
		if ((rs = prcache_domain(pcp,dnbuf,maxpath)) >= 0) {
	            if (pcp->prs[i] == np) {
			cchar *dn = pcp->domainname ;
	                if ((rs = mkpr(prbuf,maxpath,prnames[i],dn)) >= 0) {
	    		    if (cchar *cp ; (rs = lm_strw(prbuf,rs,&cp)) >= 0) {
				pcp->prs[i] = cp ;
			    }
			} else if (rs == SR_NOTFOUND) {
			    rs = SR_OK ;
			}
		    } /* end if (non-null entry) */
	            if ((rs >= 0) && (pcp->prs[i][0] != '\0')) {
	                *rpp = pcp->prs[i] ;
	                if ((*rpp)[0] != '\0') {
		            len = clenstr(*rpp) ;
	                }
	            } /* end if */
		} /* end if (prcache_domain) */
		rs1 = lm_free(a) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (valid) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (prcache_lookup) */


