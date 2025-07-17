/* sysnamedb SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* object to access both NODEDB and CLUSTERDB objects */
/* version %I% last-modified %G% */


/* revision history:

	= 1988-06-01, David A­D­ Morano
	This code was originally written.

	= 1993-02-01, David A­D­ Morano
	This subroutine was adopted for use in the RUNADVICE program.

	= 1996-07-01, David A­D­ Morano
	This subroutine was adopted for use in the REXECD daemon program.

*/

/* Copyright © 1988,1993,1996 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	sysnamedb

	Description:
	This hack just keeps some interactions with a NODEDB object
	and a CLUSTERDB object together.  These two objects are often
	accessed together when attempting to determine a current
	default cluster name.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<getbufsize.h>
#include	<mallocxx.h>
#include	<netdb.h>
#include	<clusterdb.h>
#include	<ids.h>
#include	<mkpathx.h>
#include	<permx.h>
#include	<isnot.h>
#include	<iserror.h>
#include	<localmisc.h>

#include	"sysnamedb.h"

import libutil ;

/* local defines */

#ifndef	NODEFNAME
#define	NODEFNAME	"etc/node"
#endif

#ifndef	CLUSTERFNAME
#define	CLUSTERFNAME	"etc/cluster"
#endif

#define	EBUFMULT	3		/* entry-buffer multiplier */


/* local namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct vars {
	int		nodenamelen ;
	int		elen ;
	operator int () noex ;
    } ; /* end struct (vars) */
}


/* forward references */

template<typename ... Args>
static int sysnamedb_ctor(sysnamedb *op,Args ... args) noex {
    	SYSNAMEDB	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    memclear(hop) ;
	    if ((op->nlp = new(nothrow) nodedb) != np) {
	        if ((op->clp = new(nothrow) clusterdb) != np) {
		    rs = SR_OK ;
		}
		if (rs < 0) {
		    delete op->nlp ;
		    op->nlp = nullptr ;
		}
	    } /* end if (new-nodedb) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sysnamedb_ctor) */

static int sysnamedb_dtor(sysnamedb *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->clp) {
		delete op->clp ;
		op->clp = nullptr ;
	    }
	    if (op->nlp) {
		delete op->nlp ;
		op->nlp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sysnamedb_dtor) */

template<typename ... Args>
static inline int sysnamedb_magic(sysnamedb *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == SYSNAMEDB_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (sysnamedb_magic) */

static int sysnamedb_nodebegin(sysnamedb *,ids *,cchar *) noex ;
static int sysnamedb_nodeend(sysnamedb *) noex ;
static int sysnamedb_clusterbegin(sysnamedb *,ids *,cchar *) noex ;
static int sysnamedb_clusterend(sysnamedb *) noex ;
static int sysnamedb_trynodes(sysnamedb *,vecstr *,cc *) noex ;
static int sysnamedb_tryclusters(sysnamedb *,vecstr *,cc *) noex ;


/* local variables */

static vars		var ;


/* exported variables */


/* exported subroutines */

int sysnamedb_open(sysnamedb *op,cchar *pr) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = sysnamedb_ctor(op,pr)) >= 0) {
	    rs = SR_INVALID ;
	    if (pr[0]) {
		static cint	rsv = var ;
		if ((rs = rsv) >= 0) {
	            if (ids id ; (rs = id.load) >= 0) {
	                if ((rs = sysnamedb_nodebegin(op,&id,pr)) >= 0) {
	                    if ((rs = sysnamedb_clusterbegin(op,&id,pr)) >= 0) {
			        op->magic = SYSNAMEDB_MAGIC ;
			    }
	                    if (rs < 0) {
	                        sysnamedb_nodeend(op) ;
		            }
	                } /* end if (node-db) */
	                rs1 = id.release ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (ids) */
		} /* end if (vars) */
	    } /* end if (valid) */
	    if (rs < 0) {
		sysnamedb_dtor(op) ;
	    }
	} /* end if (sysnamedb_ctor) */
	return rs ;
}
/* end subroutine (sysnamedb_open) */

int sysnamedb_close(sysnamedb *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = sysnamedb_magic(op)) >= 0) {
	    {
	        rs1 = sysnamedb_nodeend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = sysnamedb_clusterend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = sysnamedb_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (sysnamedb_close) */

int sysnamedb_getclusters(sysnamedb *op,vecstr *slp,cchar *nn) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = sysnamedb_magic(op,slp,nn)) >= 0) {
	    rs = SR_INVALID ;
	    if (nn[0]) {
		if ((rs = sysnamedb_trynodes(op,slp,nn)) >= 0) {
		    c += rs ;
		    if ((rs = sysnamedb_tryclusters(op,slp,nn)) >= 0) {
			c += rs ;
		    }
		}
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (sysnamedb_getclusters) */

int sysnamedb_getnodes(sysnamedb *op,vecstr *clp,vecstr *nlp) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = sysnamedb_magic(op,clp,nlp)) >= 0) {
	    if (char *cbuf{} ; (rs = malloc_nn(&cbuf)) >= 0) {
		clusterdb	*cop = op->clp ;
		cint		clen = rs ;
		int		cl ;
		cchar		*cp{} ;
	        for (int i = 0 ; clp->get(i,&cp) >= 0 ; i += 1) {
	            if (cp) {
		        clusterdb_cur	cur ;
	                if ((rs = clusterdb_curbegin(cop,&cur)) >= 0) {
	                    cint	rsn = SR_NOTFOUND ;
	                    while (rs >= 0) {
	                        cl = clusterdb_curfetch(cop,cp,&cur,cbuf,clen) ;
	                        if (cl == SR_NOTFOUND) break ;
	                        rs = cl ;
	                        if (rs >= 0) {
	                            if ((rs = nlp->findn(cbuf,cl)) == rsn) {
	                                c += 1 ;
	                                rs = nlp->add(cbuf,cl) ;
	                            }
	                        } /* end if (ok) */
	                    } /* end while (looping through cluster entries) */
	                    rs1 = clusterdb_curend(cop,&cur) ;
	                    if (rs >= 0) rs = rs1 ;
	                } /* end if (clusterdb-cursor) */
	            }
	            if (rs < 0) break ;
	        } /* end for */
		rs = rsfree(rs,cbuf) ;
	    } /* end if (m-a-f) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (sysnamedb_getnodes) */


/* private subroutines */

static int sysnamedb_nodebegin(sysnamedb *op,ids *idp,cchar *pr) noex {
	int		rs ;
	if (char *tbuf{} ; (rs = malloc_mp(&tbuf)) >= 0) {
	    if ((rs = mkpath(tbuf,pr,NODEFNAME)) >= 0) {
	        if (USTAT sb ; (rs = uc_stat(tbuf,&sb)) >= 0) {
	            if ((rs = permid(idp,&sb,R_OK)) >= 0) {
	                nodedb	*ndp = op->nlp ;
	                if ((rs = nodedb_open(ndp,tbuf)) >= 0) {
	                    op->fl.node = true ;
	                }
	            } else if (isNotAccess(rs)) {
	                rs = SR_OK ;
	            }
	        } else if (isNotPresent(rs)) {
	            rs = SR_OK ;
	        }
	    }
	    rs = rsfree(rs,tbuf) ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end subroutine (sysnamedb_nodebegin) */

static int sysnamedb_nodeend(sysnamedb *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->fl.node) {
	    nodedb	*ndp = op->nlp ;
	    op->fl.node = false ;
	    rs1 = nodedb_close(ndp) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (sysnamedb_nodeend) */

static int sysnamedb_clusterbegin(sysnamedb *op,ids *idp,cchar *pr) noex {
	int		rs ;
	if (char *tbuf{} ; (rs = malloc_mp(&tbuf)) >= 0) {
	    if ((rs = mkpath(tbuf,pr,CLUSTERFNAME)) >= 0) {
	        if (USTAT sb ; (rs = uc_stat(tbuf,&sb)) >= 0) {
	            if ((rs = permid(idp,&sb,R_OK)) >= 0) {
	                clusterdb	*ndp = op->clp ;
	                if ((rs = clusterdb_open(ndp,tbuf)) >= 0) {
	                    op->fl.clu = true ;
	                }
	            } else if (isNotAccess(rs)) {
	                rs = SR_OK ;
	            }
	        } else if (isNotPresent(rs)) {
	            rs = SR_OK ;
	        }
	    } /* end if (mkpath) */
	    rs = rsfree(rs,tbuf) ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end subroutine (sysnamedb_clusterbegin) */

static int sysnamedb_clusterend(sysnamedb *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->fl.clu) {
	    clusterdb	*ndp = op->clp ;
	    op->fl.clu = false ;
	    rs1 = clusterdb_close(ndp) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (sysnamedb_clusterend) */

static int sysnamedb_trynodes(sysnamedb *op,vecstr *slp,cc *nn) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		c = 0 ;
	if (op->fl.node) {
	    cint	elen = var.elen ;
	    if (char *ebuf{} ; (rs = uc_malloc((elen + 1),&ebuf)) >= 0) {
	        nodedb		*nop = op->nlp ;
	        nodedb_cur	cur ;
	        if ((rs = nodedb_curbegin(nop,&cur)) >= 0) {
	    	    nodedb_ent	ste ;
	            while (rs >= 0) {
	                rs1 = nodedb_fetch(nop,nn,&cur,&ste,ebuf,elen) ;
	                if (rs1 == rsn) break ;
	                rs = rs1 ;
	                if (rs >= 0) {
	                    if (ste.clu && (ste.clu[0] != '\0')) {
	                        if ((rs = slp->find(ste.clu)) == rsn) {
	                            c += 1 ;
	                            rs = slp->add(ste.clu) ;
	                        }
	                    } /* end if (got one) */
	                } /* end if (ok) */
	            } /* end while (looping through node-db entries) */
	            rs1 = nodedb_curend(nop,&cur) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (cursor) */
		rs = rsfree(rs,ebuf) ;
	    } /* end if (m-a-f) */
	} /* end if (DB lookup) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (sysnamedb_trynodes) */

static int sysnamedb_tryclusters(sysnamedb *op,vecstr *slp,cc *nn) noex {
	cint		rsn = SR_NOTFOUND ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		c = 0 ;
	if (op->fl.clu) {
	    if (char *cbuf{} ; (rs = malloc_nn(&cbuf)) >= 0) {
	        clusterdb	*cop = op->clp ;
	    	cint		clen = rs ;
	        clusterdb_cur	cur ; 
		if ((rs = clusterdb_curbegin(cop,&cur)) >= 0) {
	            while (rs >= 0) {
	                rs1 = clusterdb_curfetchrev(cop,nn,&cur,cbuf,clen) ;
	                if (rs1 == rsn) break ;
	                rs = rs1 ;
	                if (rs >= 0) {
	                    if ((rs = slp->find(cbuf)) == rsn) {
	                        c += 1 ;
	                        rs = slp->add(cbuf) ;
	                    }
	                } /* end if (ok) */
	            } /* end while (reverse fetch) */
	            rs1 = clusterdb_curend(cop,&cur) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (cluster-cursor) */
		rs = rsfree(rs,cbuf) ;
	    } /* end if (m-a-f) */
	} /* end if (have cluster-DB) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (sysnamedb_tryclusters) */

vars::operator int () noex {
    	int		rs ;
	if ((rs = getbufsize(getbufsize_nn)) >= 0) {
	    nodenamelen = rs ;
	    elen = (rs * EBUFMULT) ;
	}
    	return rs ;
}
/* end method (vars::operator) */


