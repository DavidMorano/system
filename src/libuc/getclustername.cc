/* getclustername SUPPORT */
/* lang=C++20 */

/* get a cluster name given a nodename */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-11-22, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getclustername

	Description:
	Get a cluster name given a nodename.

	Synopsis:
	int getclustername(cc *pr,char *rbuf,int rlen,cc *nn) noex

	Arguments:
	pr		program root
	rbuf		buffer to receive the requested cluster name
	rlen		length of supplied buffer
	nn		nodename used to find associated cluster


	>=0		string length of cluster name
	SR_OK		if OK
	SR_NOTFOUND	if could not get something needed for correct operation
	SR_ISDIR	database file was a directory (admin error)
	<0		some other error

	Design note:
	If there is no entry in the NODE DB file for the given
	nodename, then we proceed on to lookup the nodename in the
	CLUSTER DB. We use the CLUSTERDB object to interface with
	the CLUSTER DB itself. Generally, results (key-value pairs)
	are returned in a random order. If the idea was to return
	the => first <= cluster with the given node as a member,
	this will not always give predictable results. This is just
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

	Q. Were there any alternatives?
	A. Yes; the predicessor to this present implementation was an 
	   implementation that was quite simple, but it had a lot
	   of static memory storage.  It was the desire to eliminate
	   the static memory storage that led to this present
	   implementation.

	Q. Are there ways to clean this up further?
	A. Probably, but it looks I have already done more to this simple
	   function than may have been ever warranted to begin with!

	Q. Did this subroutine have to be Asyc-Signal-Safe?
	A. Not really.

	Q. Then why did you do it?
	A. The system-call |uname(2)| is Async-Signal-Safe.  Since this
	   subroutine (|getclustername(3dam)|) sort of looks like
	   |uname(2)| (of a sort), I thought it was a good idea.

	Q. Was it really a good idea?
	A. I guess not.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<uclustername.h>
#include	<sncpyx.h>
#include	<mkpathx.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"nodedb.h"
#include	"clusterdb.h"


/* local defines */

#ifndef	NODEFNAME
#define	NODEFNAME	"etc/node"
#endif

#ifndef	CLUSTERFNAME
#define	CLUSTERFNAME	"etc/cluster"
#endif

#define	SI		struct subinfo

#define	TO_TTL		(2*3600)	/* two hours */


/* local namespaces */

using std::nullptr_t ;			/* type */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct subinfo {
	cchar		*pr ;
	cchar		*nn ;
	char		*rbuf ;
	int		rlen ;
} ;


/* forward references */

static int	subinfo_start(SI *,cchar *,char *,int,cchar *) noex ;
static int	subinfo_finish(SI *) noex ;
static int	subinfo_cacheget(SI *) noex ;
static int	subinfo_cacheset(SI *) noex ;
static int	subinfo_ndb(SI *) noex ;
static int	subinfo_cdb(SI *) noex ;


/* local variables */


/* exported subroutines */

int getclustername(cchar *pr,char *rbuf,int rlen,cchar *nn) noex {
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
}
/* end subroutine (getclustername) */


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
	cint	ttl = TO_TTL ;
	return uclustername_set(sip->rbuf,sip->rlen,sip->nn,ttl) ;
}
/* end subroutine (subinfo_cacheset) */

static int subinfo_ndb(SI *sip) noex {
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	cint		rlen = sip->rlen ;
	cchar		*pr = sip->pr ;
	cchar		*nn = sip->nn ;
	char		*rbuf = sip->rbuf ;
	char		*tbuf{} ;
	if ((rs = malloc_mp(&tbuf)) >= 0) {
	    rbuf[0] = '\0' ;
	    if ((rs = mkpath2(tbuf,pr,NODEFNAME)) >= 0) {
	        NODEDB		st ;
	        NODEDB_ENT	ste ;
	        NODEDB_CUR	cur ;
	        if ((rs = nodedb_open(&st,tbuf)) >= 0) {
	            if ((rs = nodedb_curbegin(&st,&cur)) >= 0) {
	                cint	rsn = SR_NOTFOUND ;
	                cint	elen = NODEDB_ENTLEN ;
	                char	ebuf[NODEDB_ENTLEN+1] ;
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
	    } /* end if (mkpath) */
	    rs1 = uc_free(tbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (subinfo_ndb) */

static int subinfo_cdb(SI *sip) noex {
	cint		rlen = sip->rlen ;
	int		rs ;
	int		rs1 ;
	int		len = 0 ;
	cchar		*pr = sip->pr ;
	cchar		*nn = sip->nn ;
	char		*rbuf = sip->rbuf ;
	char		*tbuf{} ;
	if ((rs = malloc_mp(&tbuf)) >= 0) {
	    rbuf[0] = '\0' ;
	    if ((rs = mkpath2(tbuf,pr,CLUSTERFNAME)) >= 0) {
	        CLUSTERDB	cdb ;
		nullptr_t	np{} ;
	        if ((rs = clusterdb_open(&cdb,tbuf)) >= 0) {
	            auto	cf = clusterdb_fetchrev ;
	            cint	rsn = SR_NOTFOUND ;
	            char	*cbuf ;
    		    if ((rs = malloc_nn(&cbuf)) >= 0) {
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


