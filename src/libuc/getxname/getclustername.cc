/* getclustername SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* get a cluster name given a nodename */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

	= 2004-11-22, David A-D- Morano
	I make the |prgetclustername()| subroutine availåble elsewhere
	(everywhere) within the namespace |libuc|.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getclustername
	prgetclustername

	Description:
	Get a cluster name given a nodename.

	Synopsis:
	int getclustername(char *rbuf,int rlen,cc *nn) noex
	int prgetclustername(cc *pr,char *rbuf,int rlen,cc *nn) noex

	Arguments:
	pr		program root
	rbuf		buffer to receive the requested cluster name
	rlen		length of supplied buffer
	nn		nodename used to find associated cluster

	Returns:
	>=0		string length of cluster name
	SR_NOTFOUND	if could not get something needed for correct operation
	SR_ISDIR	database file was a directory (admin error)
	<0		some other error (system-return)

	Design note:
	If there is no entry in the NODE DB file for the given
	nodename, then we proceed on to lookup the nodename in the
	CLUSTER DB.  We use the CLUSTERDB object to interface with
	the CLUSTER DB itself.  Generally, results (key-value pairs)
	are returned in a random order.  If the idea was to return
	the => first <= cluster with the given node as a member,
	this will not always give predictable results.  This is just
	something to keep in mind, and another reason to have an
	entry for the given node in the NODE DB if deterministic
	results need to be returned for a cluster name lookup by
	nodename.

	Q. Is this (mess) multi-thread safe?
	A. Duh!

	Q. Does this need to be so complicated?
	A. Yes.

	Q. Was the amount of complication supported by the need?
	A. Looking at it now ... maybe not.

	Q. Are there ways to clean this up further?
	A. Probably, but it looks like I have already done more to this 
	   simple function than may have been ever warranted to begin with!

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<mallocxx.h>
#include	<uclustername.h>
#include	<nodedb.h>
#include	<clusterdb.h>
#include	<sncpyx.h>
#include	<mkpathx.h>
#include	<ids.h>
#include	<xperm.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"getclustername.h"


/* local defines */

#ifndef	NODEFNAME
#define	NODEFNAME	"etc/node"
#endif

#ifndef	CLUSTERFNAME
#define	CLUSTERFNAME	"etc/cluster"
#endif

#define	SI		struct subinfo

#define	TO_TTL		(2*3600)	/* two hours */


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using libuc::prgetclustername ;		/* subroutine */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */

namespace libuc {
    int prgetclustername(cchar *,char *,int,cchar *) noex ;
}


/* external variables */


/* local structures */

struct subinfo {
	cchar		*pr ;
	cchar		*nn ;
	char		*rbuf ;
	int		rlen ;
} ;

namespace {
    struct searcher {
    	char		*rbuf ;
	cchar		*nn ;
	char		*a{} ;
	int		rlen ;
	searcher(char *rb,int rl,cchar *n) noex : rbuf(rb), rlen(rl) {
	    nn = n ;
	    rbuf[0] = '\0' ;
	} ;
	int start(cchar **) noex ;
	int finish() noex ;
    } ; /* end struct (searcher) */
}


/* forward references */

static int	subinfo_start(SI *,cchar *,char *,int,cchar *) noex ;
static int	subinfo_finish(SI *) noex ;
static int	subinfo_cacheget(SI *) noex ;
static int	subinfo_cacheset(SI *) noex ;
static int	subinfo_ndb(SI *) noex ;
static int	subinfo_ndber(SI *,cchar *) noex ;
static int	subinfo_cdb(SI *) noex ;


/* local variables */

constexpr cpcchar	prs[] = {
    "/",
    "/usr/extra",
    "/usr/local"
} ;

constexpr cchar		nodefname[] = NODEFNAME ;
constexpr cchar		clusterfname[] = CLUSTERFNAME ;


/* exported variables */


/* exported subroutines */

int getclustername(char *rbuf,int rlen,cchar *nname) noex {
    	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (rbuf) {
	    searcher	so(rbuf,rlen,nname) ;
	    rs = SR_OK ;
	    rbuf[0] = '\0' ;
	    if (cchar *nn{} ; (rs = so.start(&nn)) >= 0) {
	        if (ids id ; (rs = id.load) >= 0) {
	            if (char *pbuf{} ; (rs = malloc_mp(&pbuf)) >= 0) {
		        cint	am = R_OK ;
		        rs = SR_OK ;
		        for (const auto pr : prs) {
		            if ((rs = mkpath(pbuf,nodefname)) >= 0) {
			        if (USTAT sb ; (rs = uc_stat(pbuf,&sb)) >= 0) {
			            if ((rs = sperm(&id,&sb,am)) >= 0) {
    				        rs = prgetclustername(pr,rbuf,rlen,nn) ;
    				        len = rs ;
				    } /* end if (sperm) */
			        } /* end if (stat) */
		            } /* end if (mkpath) */
		            if (rs != 0) break ;
		        } /* end for */
		        rs = rsfree(rs,pbuf) ;
	            } /* end if (m-a-f) */
	            rs1 = id.release ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (ids) */
	        rs1 = so.finish() ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (searcher) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (getclustername) */

namespace libuc {
    int prgetclustername(cchar *pr,char *rbuf,int rlen,cchar *nn) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (pr && rbuf && nn) {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if (pr[0] && nn[0]) {
	        SI	si ;
	        if ((rs = subinfo_start(&si,pr,rbuf,rlen,nn)) >= 0) {
	            if ((rs = subinfo_cacheget(&si)) == 0) {
	                if ((rs = subinfo_ndb(&si)) == 0) {
	                    rs = subinfo_cdb(&si) ;
	                }
	                if (rs > 0) {
	                    len = rs ;
	                    rs = subinfo_cacheset(&si) ;
	                }
	            } else {
	                len = rs ;
	            }
	            rs1 = subinfo_finish(&si) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (subinfo) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
    } /* end subroutine (prgetclustername) */
}


/* local subroutines */

static int subinfo_start(SI *sip,cc *pr,char *rbuf,int rlen,cc *nn) noex {
	int		rs = SR_FAULT ;
	if (sip) {
	    sip->pr = pr ;
	    sip->rbuf = rbuf ;
	    sip->rlen = rlen ;
	    sip->nn = nn ;
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (subinfo_start) */

static int subinfo_finish(SI *sip) noex {
	int		rs = SR_FAULT ;
	if (sip) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (subinfo_finish) */

static int subinfo_cacheget(SI *sip) noex {
	return uclustername_get(sip->rbuf,sip->rlen,sip->nn) ;
}
/* end subroutine (getsubinfo_cacheget) */

static int subinfo_cacheset(SI *sip) noex {
	cint		ttl = TO_TTL ;
	return uclustername_set(sip->rbuf,sip->rlen,sip->nn,ttl) ;
}
/* end subroutine (subinfo_cacheset) */

static int subinfo_ndb(SI *sip) noex {
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	cchar		*pr = sip->pr ;
	if (char *tbuf{} ; (rs = malloc_mp(&tbuf)) >= 0) {
	    if ((rs = mkpath2(tbuf,pr,nodefname)) >= 0) {
		rs = subinfo_ndber(sip,tbuf) ;
		len = rs ;
	    } /* end if (mkpath) */
	    rs1 = uc_free(tbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (subinfo_ndb) */

static int subinfo_ndber(SI *sip,cchar *nfn) noex {
	cint		elen = NODEDB_ENTLEN ;
    	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	cint		rlen = sip->rlen ;
	cchar		*nn = sip->nn ;
	char		*rbuf = sip->rbuf ;
	if (char *ebuf{} ; (rs = uc_malloc(elen,&ebuf)) >= 0) {
            if (nodedb st ; (rs = nodedb_open(&st,nfn)) >= 0) {
                if (nodedb_cur cur ; (rs = nodedb_curbegin(&st,&cur)) >= 0) {
                    nodedb_ent  ste ;
                    cint    	rsn = SR_NOTFOUND ;
                    while (rs >= 0) {
                        rs1 = nodedb_fetch(&st,nn,&cur,&ste,ebuf,elen) ;
                        if (rs1 == rsn) break ;
                        rs = rs1 ;
                        if (rs >= 0) {
                            if (ste.clu && (ste.clu[0] != '\0')) {
                                rs = sncpy1(rbuf,rlen,ste.clu) ;
                                len = rs ;
                            }
                        } /* end if (ok) */
                        if ((rs >= 0) && (len > 0)) break ;
                        if (rs < 0) break ;
                    } /* end while (fetching node entries) */
                    rs1 = nodedb_curend(&st,&cur) ;
                    if (rs >= 0) rs = rs1 ;
                } /* end if (nodedb-cursor) */
                rs1 = nodedb_close(&st) ;
                if (rs >= 0) rs = rs1 ;
            } else if (isNotPresent(rs)) {
                rs = SR_OK ;
            }
	    rs = rsfree(rs,ebuf) ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? len : rs ;
}
/* end if (subinfor_ndber) */

static int subinfo_cdb(SI *sip) noex {
	cint		rlen = sip->rlen ;
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	cchar		*pr = sip->pr ;
	cchar		*nn = sip->nn ;
	char		*rbuf = sip->rbuf ;
	if (char *tbuf{} ; (rs = malloc_mp(&tbuf)) >= 0) {
	    rbuf[0] = '\0' ;
	    if ((rs = mkpath2(tbuf,pr,clusterfname)) >= 0) {
		cnullptr	np{} ;
	        if (clusterdb cdb ; (rs = clusterdb_open(&cdb,tbuf)) >= 0) {
	            auto	cf = clusterdb_curfetchrev ;
	            cint	rsn = SR_NOTFOUND ;
	            if (char *cbuf ; (rs = malloc_nn(&cbuf)) >= 0) {
			cint	clen = rs ;
	                if ((rs = cf(&cdb,nn,np,cbuf,clen)) >= 0) {
	                    rs = sncpy1(rbuf,rlen,cbuf) ;
	                    len = rs ;
	                } else if (rs == rsn) {
	                    rs = SR_OK ;
		        }
	    	        rs1 = uc_free(cbuf) ;
	    	        if (rs >= 0) rs = rs1 ;
		    } /* end if (m-a-f) */
	            rs1 = clusterdb_close(&cdb) ;
		    if (rs >= 0) rs = rs1 ;
	        } else if (isNotPresent(rs)) {
	            rs = SR_OK ;
	        }
	    } /* end if (mkpath) */
	    rs1 = uc_free(tbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (subinfo_cdb) */

int searcher::start(cchar **npp) noex {
    	int		rs = SR_OK ;
	if (nn == nullptr) {
	    if ((rs = malloc_nn(&a)) >= 0) {
		*npp = a ;
	    } /* end if (memory-allocation) */
	} else {
	    *npp = nn ;
	}
	return rs ;
}
/* end method (search::start) */

int searcher::finish() noex {
    	int		rs = SR_OK ;
	int		rs1 ;
	if (a) {
	    rs1 = uc_free(a) ;
	    if (rs >= 0) rs = rs1 ;
	    a = nullptr ;
	}
	return rs ;
}
/* end method (search::finish) */


