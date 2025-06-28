/* prognamecache SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* name-cache management */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-09-01, David A­D­ Morano
	This subroutine manages a (simple) name cahce.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	prognamecache

	Description:
	We maintain a process cache for real-names.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<userinfo.h>
#include	<namecache.h>
#include	<localmisc.h>

#include	"proginfo.h"


/* local defines */


/* external subroutines */

extern "C" {
    extern cchar	*getourenv(cchar **,cchar *) ;
}


/* external variables */


/* local structures */


/* forward references */

static int prognamecache_namecache(proginfo *) ;


/* local variables */


/* exported variables */


/* exported subroutines */

int prognamecache_begin(proginfo *pip,USERINFO *uip) noex {
	int		rs = SR_OK ;

	if (pip == nullptr) return SR_FAULT ;

	if (pip->uip == nullptr) pip->uip = uip ;

	return rs ;
}
/* end subroutine (prognamecache_begin) */

int prognamecache_end(proginfo *pip) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (pip == nullptr) return SR_FAULT ;

	if (pip->namecache != nullptr) {
	    NAMECACHE	*ncp = (NAMECACHE *) pip->namecache ;
	    rs1 = namecache_finish(ncp) ;
	    if (rs >= 0) rs = rs1 ;
	    rs1 = uc_free(pip->namecache) ;
	    if (rs >= 0) rs = rs1 ;
	    pip->namecache = nullptr ;
	}

	return rs ;
}
/* end subroutine (prognamecache_end) */

int prognamecache_lookup(proginfo *pip,cchar *un,cchar **rpp) noex {
	NAMECACHE	*ncp ;
	int		rs = SR_OK ;

	if (pip == nullptr) return SR_FAULT ;
	if (un == nullptr) return SR_FAULT ;
	if (un[0] == '\0') return SR_INVALID ;

	if (pip->namecache == nullptr) {
	    rs = prognamecache_namecache(pip) ;
	}

	if (rs >= 0) {
	    ncp = (NAMECACHE *) pip->namecache ;
	    rs = namecache_lookup(ncp,un,rpp) ;
	}

	return rs ;
}
/* end subroutine (prognamecache_lookup) */


/* local subroutines */

static int prognamecache_namecache(proginfo *pip) noex {
	int		rs = SR_OK ;
	if (pip->namecache == nullptr) {
	    cint	msize = szof(NAMECACHE) ;
	    if (void *p ; (rs = uc_malloc(msize,&p)) >= 0) {
	        NAMECACHE	*ncp = (NAMECACHE *) p ;
	        cint		to = (5*60) ;
	        pip->namecache = p ;
	        if ((rs = namecache_start(ncp,VARUSERNAME,0,to)) >= 0) {
	            cchar	*up = getourenv(pip->envv,VARUSERNAME) ;
	            if (up != nullptr) {
		        cchar	*cp = getourenv(pip->envv,VARFULLNAME) ;
		        if ((cp == nullptr) || (cp[0] == '\0'))
		            cp = getourenv(pip->envv,VARNAME) ;
		        if ((cp != nullptr) && (cp[0] != '\0')) {
	        	    rs = namecache_add(ncp,up,cp,-1) ;
		        }
		    }
		    if (rs < 0) {
		        namecache_finish(ncp) ;
		    }
	        } /* end if (namecache) */
		if (rs < 0) {
		    uc_free(p) ;
		    pip->namecache = nullptr ;
		}
	    } /* end if (memory-allocation) */
	} /* end if (needed-construction) */

	return rs ;
}
/* end subroutine (prognamecache_namecache) */


