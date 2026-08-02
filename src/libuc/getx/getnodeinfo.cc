/* getnodeinfo SUPPORT */
/* charset=ISO8859-1 */
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

/* Copyright © 1995,1996 David A­D­ Morano.  All rights reserved. */

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
#include	<sys/param.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<bufsizeget.h>		/* LIBUC */
#include	<getnodename.h>		/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<nodedb.h>		/* LIBUC */
#include	<sncpyx.h>		/* LIBUC */
#include	<mkpathx.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"getnodeinfo.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;

/* local defines */

#define	NODEFNAME	"etc/node"
#define	ENTLENMULT	4		/* entry-length multiply factor */


/* imported namespaces */


/* local typedefs */

typedef nodedb_ent	ent ;


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct vars {
	int		nodenamelen ;
	int		maxpathlen ;
	int		entlen ;
	int		sz ;
	operator int () noex ;
    } ; /* end struct (vars) */
} /* end namespace */


/* forward references */

local int	getx(cc *,char *,char *,vecstr *,cc *) noex ;
local int	vecload(vecstr *,ent *) noex ;


/* local variables */

static vars		var ;

constexpr char		nodefname[] = NODEFNAME ;


/* exported variables */


/* exported subroutines */

int getnodeinfo(cc *pr,char *cbuf,char *sbuf,vecstr *klp,cc *nn) noex {
	int		rs = SR_FAULT ;
	int		len = -1 ; /* return-value */
	if (pr == nullptr) pr = "/" ;
	if (nn) ylikely {
	    rs = SR_INVALID ;
	    if (pr[0] && nn[0]) ylikely {
		if (static cint rsv = var ; (rs = rsv) >= 0) ylikely {
		    rs = getx(pr,cbuf,sbuf,klp,nn) ;
		    len = rs ;
		} /* end if (vars) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (getnodeinfo) */


/* local subroutines */

/****
  Be careful with the memory allocation here (below).  We want to
  consolidate several allocations into one (a curse afflicting many
  novice developes).  But one has to be careful to get the allocation
  size correct.  Also, the offsets for each buffer requires attention.
****/

local int getx(cc *pr,char *cbuf,char *sbuf,vecstr *klp,cc *nn) noex {
	cnullptr        np{} ;
	cint		clen = var.nodenamelen ;	/* "cluster" length */
	cint		slen = var.nodenamelen ;	/* "system" length */
    	int		rs ;
	int		rs1 ;
	int		len = -1 ; /* return-value */
	int		ai = 0 ;
	if (char *a ; (rs = lm_mall(var.sz,&a)) >= 0) ylikely {
	    cint	nlen = var.nodenamelen ;	/* "node" length */
	    char	*nbuf = (a + (ai++ * (clen + 1))) ;
	    if ((rs = getnodename(nbuf,nlen)) >= 0) ylikely {
		cint	tlen = var.maxpathlen ;
	        char	*tbuf = (a + (ai++ * (clen + 1))) ;
                if ((rs = mkpath(tbuf,pr,nodefname)) >= 0) ylikely {
                    if (nodedb st ; (rs = nodedb_open(&st,tbuf)) >= 0) {
                        nodedb_ent  ste{} ;
                        cauto	    nf = nodedb_fetch ;
                        cint        elen = var.entlen ;
                        char        *ebuf = (a + (nlen + 1) + (tlen + 1)) ;
                        if ((rs = nf(&st,nn,np,&ste,ebuf,elen)) >= 0) {
                            if (cbuf) {
                                rs = sncpy1(cbuf,clen,ste.clu) ;
                                len = rs ;
                            } else {
                                len = lenstr(ste.clu) ;
                            }
                            if ((rs >= 0) && sbuf) {
                                rs = sncpy(sbuf,slen,ste.sys) ;
                            }
                            if ((rs >= 0) && klp) {
                                rs = vecload(klp,&ste) ;
                            } /* end if (keys) */
                        } /* end if (fetched result found) */
                        rs1 = nodedb_close(&st) ;
                        if (rs >= 0) rs = rs1 ;
                    } /* end if (DB opened) */
                } /* end if (mkpath) */
	    } /* end if (getnodename) */
	    rs1 = lm_free(a) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (getx) */

local int vecload(vecstr *klp,ent *ep) noex {
    	int		rs = SR_OK ;
	for (int i = 0 ; ep->keys[i][0] ; i += 1) {
	    cchar	*kp = ep->keys[i][0] ;
	    cchar	*vp = ep->keys[i][1] ;
	    rs = klp->envadd(kp,vp) ;
	    if (rs < 0) break ;
	} /* end for */
	return rs ;
} /* end subroutine (vecload) */

vars::operator int () noex {
    	int		rs ;
	if ((rs = bufsizeget(bufsize_nn)) >= 0) ylikely {
	    nodenamelen = rs ;
	    if ((rs = bufsizeget(bufsize_mp)) >= 0) ylikely {
		maxpathlen = rs ;
		entlen = (ENTLENMULT * nodenamelen) ;
		sz += (nodenamelen + 1) ;
		sz += (maxpathlen + 1) ;
		sz += (entlen + 1) ;
	    }
	}
	return rs ;
} /* end method (vars::operator) */


