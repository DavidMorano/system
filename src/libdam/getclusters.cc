/* getclusters SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* get all clusters for the specified nodename */
/* version %I% last-modified %G% */


/* revision history:

	= 1995-07-01, David A­D­ Morano
	This code was originally written.

	= 2004-11-22, David A­D­ Morano
	This subroutine was hi-jacked and pressed into service to
	get the name of the primary cluster that the local node is
	in.

*/

/* Copyright © 1995,2004 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

  	Name:
	getclusters

	Description:
	Get all cluster names given a nodename.

	Synopsis:
	int getclusters(cc *pr,vecstr *slp,cc *nodename) noex

	Arguments:
	pr		program root
	slp		pointer to VECSTR object for results
	nodename	nodename used to find associated cluster

	Returns:
	>=0		string length of cluster name
	SR_OK		if OK
	SR_NOTFOUND	if could not get something needed for correct operation
	SR_ISDIR	database file was a directory (admin error)
	<0		some other error

******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<vecstr.h>
#include	<mkpathx.h>
#include	<isnot.h>
#include	<localmisc.h>
#include	<nodedb.h>
#include	<clusterdb.h>

#include	"getclusters.h"


/* local defines */

#define	EBUFMULT	3		/* entry-buffer multiplier */

#define	NODEFNAME	"etc/node"
#define	CLUSTERFNAME	"etc/cluster"


/* imported namespaces */


/* local typedefs */

typedef clusterdb	cdb ;
typedef clusterdb_cur	cdb_cur ;


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct vars {
	int		maxpathlen ;
	int		nodenamelen ;
	int		elen ;
	operator int () noex ;
    } ; /* end struct (vars) */
    struct geter {
	cchar		*pr ;
	cchar		*nn ;
	vecstr		*slp ;
	char		*a ;
	char		*tbuf ;
	char		*ebuf ;
	int		tlen ;
	int		elen ;
	geter(cc *p,vecstr *s,cc *n) noex : pr(p), nn(n), slp(s) { } ;
	operator int () noex ;
	int ndb() noex ;
	int cdb() noex ;
    } ; /* end struct (geter) */
}


/* forward references */


/* local variables */

static vars		var ;


/* exported variables */


/* exported subroutines */

int getclusters(cchar *pr,vecstr *slp,cchar *nn) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (pr && slp && nn) {
	    rs = SR_INVALID ;
	    if (pr[0] && nn[0]) {
		static cint	rsv = var ;
		if ((rs = rsv) >= 0) {
		    geter go(pr,slp,nn) ;
		    rs = go ;
		    c = rs ;
		} /* end if (vars) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (getclusters) */


/* local subroutines */

geter::operator int () noex {
    	cint		sz = (var.maxpathlen + 2 + var.elen) ;
    	int		rs ;
	int		c = 0 ;
	if (char *a{} ; (rs = uc_malloc(sz,&a)) >= 0) {
	    tlen = var.maxpathlen ;
	    tbuf = a ;
	    elen = var.elen ;
	    ebuf = (a + (var.maxpathlen + 1)) ;
	    if ((rs = ndb()) >= 0) {
		c += rs ;
		if ((rs = cdb()) >= 0) {
		    c += rs ;
		}
	    }
	    rs = rsfree(rs,a) ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? c : rs ;
}
/* end method (geter::operator) */

int geter::ndb() noex {
    	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = mkpath(tbuf,pr,NODEFNAME)) >= 0) {
	    if (nodedb st ; (rs = nodedb_open(&st,tbuf)) >= 0) {
	        if (nodedb_cur cur ; (rs = nodedb_curbegin(&st,&cur)) >= 0) {
	    	    nodedb_ent	ste ;
		    cint	nrs = SR_NOTFOUND ;
	            while (rs >= 0) {
	                rs1 = nodedb_fetch(&st,nn,&cur,&ste,ebuf,elen) ;
	                if (rs1 == nrs) break ;
			rs = rs1 ;
			if (rs >= 0) {
	            	    if (ste.clu && (ste.clu[0] != '\0')) {
	                	if ((rs = slp->find(ste.clu)) == nrs) {
	                    	    c += 1 ;
	                    	    rs = slp->add(ste.clu) ;
	                	}
	            	    } /* end if (got one) */
			} /* end if (ok) */
	            } /* end while (looping through node entries) */
	            rs1 = nodedb_curend(&st,&cur) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (nodedb-cursor) */
	        rs1 = nodedb_close(&st) ;
		if (rs >= 0) rs = rs1 ;
	    } else if (isNotPresent(rs)) {
	        rs = SR_OK ;
	    }
	} /* end if (mkpath) */
	return (rs >= 0) ? c : rs ;
}
/* end method (geter::ndb) */

int geter::cdb() noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = mkpath(tbuf,pr,CLUSTERFNAME)) >= 0) {
	    if (clusterdb clu ; (rs = clusterdb_open(&clu,tbuf)) >= 0) {
	        if (cdb_cur cur{} ; (rs = clusterdb_curbegin(&clu,&cur)) >= 0) {
		    cint	nrs = SR_NOTFOUND ;
	            while (rs >= 0) {
	                rs1 = clusterdb_curfetchrev(&clu,nn,&cur,ebuf,elen) ;
	                if (rs1 == nrs) break ;
			rs = rs1 ;
			if (rs >= 0) {
	            	    if ((rs = slp->find(ebuf)) == nrs) {
	                	c += 1 ;
	                	rs = slp->add(ebuf) ;
			    }
	            	} /* end if (ok) */
	            } /* end while (fetchrev) */
	            rs1 = clusterdb_curend(&clu,&cur) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (clusterdb-cursor) */
	        rs1 = clusterdb_close(&clu) ;
		if (rs >= 0) rs = rs1 ;
	    } else if (isNotPresent(rs)) {
		rs = SR_OK ;
	    }
	} /* end if (mkpath) */
	return (rs >= 0) ? c : rs ;
}
/* end method (geter::cdb) */

vars::operator int () noex {
    	int		rs ;
	if ((rs = getbufsize(getbufsize_mp)) >= 0) {
	    maxpathlen = rs ;
	    if ((rs = getbufsize(getbufsize_nn)) >= 0) {
		nodenamelen = rs ;
		elen = (rs * EBUFMULT) ;
	    }
	}
    	return rs ;
}
/* end method (vars::operator) */


