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
}


/* forward references */

static int getclusters_ndb(cchar *,vecstr *,cchar *) noex ;
static int getclusters_ndbs(char *,cc *,vecstr *,cc *) noex ;
static int getclusters_cdb(cchar *,vecstr *,cchar *) noex ;


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
	            if ((rs = getclusters_ndb(pr,slp,nn)) >= 0) {
	                c += rs ;
	                if ((rs = getclusters_cdb(pr,slp,nn)) >= 0) {
		            c += rs ;
	                }
	            }
		} /* end if (vars) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (getclusters) */


/* local subroutines */

static int getclusters_ndb(cchar *pr,vecstr *slp,cchar *nn) noex {
    	cint		sz = (var.maxpathlen + 2 + var.elen) ;
	int		rs ;
	int		c = 0 ;
	if (char *tbuf{} ; (rs = uc_malloc(sz,&tbuf)) >= 0) {
	    char	*ebuf = (tbuf + (var.maxpathlen + 1)) ;
	    if ((rs = mkpath(tbuf,pr,NODEFNAME)) >= 0) {
		rs = getclusters_ndbs(ebuf,tbuf,slp,nn) ;
		c = rs ;
	    }
	    rs = rsfree(rs,tbuf) ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (getclusters_ndb) */

static int getclusters_ndbs(char *ebuf,cc *fn,vecstr *slp,cc *nn) noex {
    	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (nodedb st ; (rs = nodedb_open(&st,fn)) >= 0) {
	    if (nodedb_cur cur ; (rs = nodedb_curbegin(&st,&cur)) >= 0) {
	    	    nodedb_ent	ste ;
		    cint	nrs = SR_NOTFOUND ;
	            cint	elen = var.elen ;
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
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (getclusters_ndbs) */

static int getclusters_cdb(cchar *pr,vecstr *slp,cchar *nn) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	char		tbuf[MAXPATHLEN+1] ;

	if ((rs = mkpath2(tbuf,pr,CLUSTERFNAME)) >= 0) {
	    if (clusterdb clu ; (rs = clusterdb_open(&clu,tbuf)) >= 0) {
	        if (cdb_cur cur{} ; (rs = clusterdb_curbegin(&clu,&cur)) >= 0) {
		    cint	nrs = SR_NOTFOUND ;
		    cint	clen = NODENAMELEN ;
	            char	cbuf[NODENAMELEN+ 1] ;
	            while (rs >= 0) {
	                rs1 = clusterdb_curfetchrev(&clu,nn,&cur,cbuf,clen) ;
	                if (rs1 == nrs) break ;
			rs = rs1 ;
			if (rs >= 0) {
	            	    if ((rs = slp->find(cbuf)) == nrs) {
	                	c += 1 ;
	                	rs = slp->add(cbuf) ;
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
/* end subroutine (getclusters_cdb) */

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


