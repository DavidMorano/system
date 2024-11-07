/* getnodeinfo SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* get a cluster name given a nodename */
/* version %I% last-modified %G% */


/* revision history:

	= 1995-07-01, David A­D­ Morano
	This code was originally written.

	= 1996-05-22, David A­D­ Morano
	This subroutine was enhanced to get the local node-name if
	one if not supplied, using |getnodename(3dam)|.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getnodeinfo

	Description:
	Get a (optional) cluster-name and a (optional) system-name
	given a nodename.

	Synopsis:
	int getnodeinfo(cc *pr,char *cbuf,char *sbuf,vecstr *kp,cc *nn) noex

	Arguments:
	pr		program root
	cbuf		optional cluster-name result buffer pointer
	sbuf		optional system-name result buffer pointer
	kp		pointer to VECSTR to hold resulting key-value pairs
	nn		nodename used to find associated cluster

	Returns:
	>=0		string length of cluster name
	SR_OK		if OK
	SR_NOTFOUND	if could not get something needed for correct operation
	SR_ISDIR	database file was a directory (admin error)
	<0		some other error (system-return)

	Design note:
	If there is no entry in the NODE DB file for the given
	nodename, then we proceed on to lookup the nodename in the
	CLUSTER DB.  Since we are using a NODEDB object to read the
	CLUSTER DB file, results (key-value pairs) are returned in
	a random order.  If the idea was to return the -> first <-
	cluster with the given node as a member, this will not
	always give predictable results.  This is just something
	to keep in mind, and another reason to have an entry for
	the given node in the NODE DB if deterministic results need
	to be returned for a cluster name lookup by nodename.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<unistd.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
#include	<usystem.h>
#include	<mallocxx.h>
#include	<getnodename.h>
#include	<vecstr.h>
#include	<nodedb.h>
#include	<sncpyx.h>
#include	<mkpathx.h>
#include	<localmisc.h>

#include	"getnodeinfo.h"


/* local defines */

#define	NODEFNAME	"etc/node"


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef nodedb_ent	ent ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	getx(cc *,char *,char *,vecstr *,cc *) noex ;
static int	vecload(vecstr *,ent *) noex ;


/* local variables */

constexpr cchar		nodefname[] = NODEFNAME ;


/* exported variables */


/* exported subroutines */

int getnodeinfo(cc *pr,char *cbuf,char *sbuf,vecstr *klp,cc *nn) noex {
	int		rs = SR_FAULT ;
	int		len = -1 ;
	if (pr == nullptr) pr = "/" ;
	if (nn) {
	    if (pr[0] && nn[0]) {
		rs = getx(pr,cbuf,sbuf,klp,nn) ;
		len = rs ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (getnodeinfo) */


/* local subroutines */

static int getx(cc *pr,char *cbuf,char *sbuf,vecstr *klp,cc *nn) noex {
    	int		rs ;
	int		rs1 ;
	int		len = -1 ;
	if (char *nbuf{} ; (rs = malloc_nn(&nbuf)) >= 0) {
	    cint	nlen = rs ;		/* "node" length */
	    cint	clen = rs ;		/* "cluster" length */
	    if ((rs = getnodename(nbuf,nlen)) >= 0) {
		if (char *tbuf{} ; (rs = malloc_mp(&tbuf)) >= 0) {
	            if ((rs = mkpath(tbuf,pr,nodefname)) >= 0) {
			cnullptr	np{} ;
	                if (nodedb st ; (rs = nodedb_open(&st,tbuf)) >= 0) {
	                    nodedb_ent	ste{} ;
			    auto	nf = nodedb_fetch ;
	                    cint	elen = NODEDB_ENTLEN ;
	                    char	ebuf[NODEDB_ENTLEN + 1] ;
	                    if ((rs = nf(&st,nn,np,&ste,ebuf,elen)) >= 0) {
	                        if (cbuf) {
	                            rs = sncpy1(cbuf,clen,ste.clu) ;
			            len = rs ;
	                        } else {
	                            len = strlen(ste.clu) ;
			        }
	                        if ((rs >= 0) && sbuf) {
	                            rs = sncpy(sbuf,nlen,ste.sys) ;
			        }
	                        if ((rs >= 0) && klp) {
				    rs = vecload(klp,&ste) ;
	                        } /* end if (keys) */
	                    } /* end if (fetched result found) */
	                    rs1 = nodedb_close(&st) ;
		            if (rs >= 0) rs = rs1 ;
	                } /* end if (DB opened) */
	            } /* end if (mkpath) */
		    rs1 = uc_free(tbuf) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
	    } /* end if (getnodename) */
	    rs1 = uc_free(nbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (getx) */

static int vecload(vecstr *klp,ent *ep) noex {
    	int		rs = SR_OK ;
	for (int i = 0 ; ep->keys[i][0] ; i += 1) {
	    cchar	*kp = ep->keys[i][0] ;
	    cchar	*vp = ep->keys[i][1] ;
	    rs = klp->envadd(kp,vp) ;
	    if (rs < 0) break ;
	} /* end for */
	return rs ;
}
/* end subroutine (vecload) */


