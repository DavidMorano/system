/* mkdirlist SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* create a list of the newsgroup directories */
/* version %I% last-modified %G% */


/* revision history:

	= 1994-04-13, David A­D­ Morano
	This is new so that we can track directory visits for
	duplicates.

	= 2014-11-25, David A­D­ Morano
	This object was enhanced to include much of the functionality
	from the old PCS |get_bds(3pcs)| function.

*/

/* Copyright © 1994,2014 David A­D­ Morano.  All rights reserved. */
/* Use is subject to license terms. */

/*******************************************************************************

	Name:
	mkdirlist

	Description:
	this object creates the (so-called) "dir-list" file
	within the given directory area.  The "directory area"
	is supposedly a "spool" area for BB-news articles.
	A list of the BB-new "newsgroups" are put into that
	created file.  That file is subsequently access by
	other software as a sort of cache for the BB-new
	newsgroup names.

	Synopsis:
	int mkdirlist_start(mkdirlist *op,cchar *pr,cchar *ndn) noex

	Arguments:
	op		object pointer
	pr		program-root
	ndn		name-directory name

	Returns:
	>=0		count
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<getbufsize.h>
#include	<bfile.h>
#include	<filer.h>
#include	<fsdirtree.h>
#include	<mktmp.h>		/* |mktmpfile(3uc)| */
#include	<mkpathx.h>
#include	<pathadd.h>
#include	<strwcmp.h>
#include	<isnot.h>		/* |isNotPresent(3uc)| */
#include	<localmisc.h>

#include	"mkdirlist.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	MKDIRLIST_DIRCACHE	".dircache"
#define	MKDIRLIST_DCMAGIC	"DIRCACHE"

#define	DS_SIZE		szof(MKDIRLIST)

#ifndef	TMPDNAME
#define	TMPDNAME	"/tmp"
#endif

#define	ENT		mkdirlist_ent


/* imported namespaces */

using std::nothrow ;			/* constant */


/* local typedefs */

typedef mkdirlist_ent	ent ;
typedef mkdirlist_ent *	entp ;


/* external subroutines */

extern "C" {
    extern int	pcsopendircache(cchar *,cchar *,int,mode_t,int) noex ;
    extern int	bbcmp(cchar *,cchar *) noex ;
}

extern "C" {
    extern int uc_close(int) noex ;
}


/* external variables */


/* local structures */

namespace {
    struct vars {
	int		maxpathlen ;
	operator int () noex ;
    } ; /* end struct (vars) */
} /* end namespace */


/* forward references */

template<typename ... Args>
local int mkdirlist_ctor(mkdirlist *op,Args ... args) noex {
	MKDIRLIST	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = SR_NOMEM ;
	    memclear(hop) ;
	    if ((op->dlp = new(nothrow) vechand) != nullptr) ylikely {
		rs = SR_OK ;
	    } /* end if (new-vechand) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mkdirlist_ctor) */

local int mkdirlist_dtor(mkdirlist *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->dlp) ylikely {
		delete op->dlp ;
		op->dlp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (clusterdb_dtor) */

template<typename ... Args>
local int mkdirlist_magic(mkdirlist *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magic == MKDIRLIST_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (mkdirlist_magic) */

local int mkdirlist_pdc(mkdirlist *,cchar *,int) noex ; /* proc-dir-cache */
local int mkdirlist_pdn(mkdirlist *,cchar *) noex ;	/* proc-dir-news */
local int mkdirlist_newent(mkdirlist *,ustat *,cchar *,int) noex ;
local int mkdirlist_finents(mkdirlist *) noex ;

local int entry_start(ENT *,ustat *,cchar *,int) noex ;
local int entry_finish(ENT *) noex ;
local int entry_showdef(ENT *) noex ;
local int entry_show(ENT *,cchar *,int) noex ;
local int entry_matung(ENT *,cchar *,time_t,int,int) noex ;

extern "C" {
    local int vcmporder(cvoid **,cvoid **) noex ;
}


/* local variables */

static vars		var ;


/* exported variables */


/* exported subroutines */

int mkdirlist_start(mkdirlist *op,cchar *pr,cchar *ndn) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = mkdirlist_ctor(op,pr,ndn)) >= 0) {
	    vechand *dlp = op->dlp ;
	    rs = SR_INVALID ;
	    if (pr[0] && ndn[0]) {
		static cint	rsv = var ;
		if ((rs = rsv) >= 0) {
		    cint	vn = 20 ;
		    cint	vo = 0 ;
	            if ((rs = dlp->start(vn,vo)) >= 0) {
	                cint	of = O_RDONLY ;
	                cmode	om = 0666 ;
	                if ((rs = pcsopendircache(pr,ndn,of,om,-1)) >= 0) {
	                    cint	fd = rs ;
	                    if ((rs = mkdirlist_pdc(op,ndn,fd)) >= 0) {
	                        c = rs ;
		            }
	                    rs1 = uc_close(fd) ;
			    if (rs >= 0) rs = rs1 ;
	                } else if (isNotPresent(rs)) {
	                    rs = mkdirlist_pdn(op,ndn) ;
	                    c = rs ;
	                }
	                if (rs >= 0) {
			    op->magic = MKDIRLIST_MAGIC ;
		        }
	                if (rs < 0) {
	                    mkdirlist_finents(op) ;
	                    dlp->finish() ;
	                } /* end if (error handling) */
	            } /* end if (vechand) */
		} /* end if (vars) */
	    } /* end if (valid) */
	    if (rs < 0) {
		mkdirlist_dtor(op) ;
	    }
	} /* end if (mkdirlist_ctor) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mkdirlist_start) */

int mkdirlist_finish(mkdirlist *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = mkdirlist_magic(op)) >= 0) {
	    vechand *dlp = op->dlp ;
	    {
	        rs1 = mkdirlist_finents(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->dlp) {
	        rs1 = dlp->finish ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = mkdirlist_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mkdirlist_finish) */

int mkdirlist_get(mkdirlist *op,int i,ENT **epp) noex {
	int		rs ;
	if ((rs = mkdirlist_magic(op,epp)) >= 0) {
	    vechand	*dlp = op->dlp ;
	    if (void *vp ; (rs = dlp->get(i,&vp)) >= 0) {
		*epp = entp(vp) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mkdirlist_get) */

int mkdirlist_link(mkdirlist *op) noex {
	int		rs ;
	if ((rs = mkdirlist_magic(op)) >= 0) {
	    vechand	*dlp = op->dlp ;
	    void	*vp ;
	    for (int i = 0 ; dlp->get(i,&vp) >= 0 ; i += 1) {
	    	mkdirlist_ent	*ep = entp(vp) ;
	        if (vp) {
	            if (! ep->fl.link) {
	    	        mkdirlist_ent	*pep = ep ;
	                for (int j = (i+1) ; dlp->get(j,&vp) >= 0 ; j += 1) {
	    	    	    mkdirlist_ent	*oep = entp(vp) ;
	                    if (vp) {
				cchar		*n1 = pep->name ;
				cchar		*n2 = oep->name ;
	                        if ((! ep->fl.link) && (bbcmp(n1,n2) == 0)) {
	                            pep->link = oep ;
	                            oep->fl.link = true ;
	                            pep = oep ;
	                        } /* end if (board match) */
        		    } /* end if (non-null entry) */
	                } /* end for (inner) */
	            } /* end if (entry not linked) */
		} /* end if (non-null entry) */
	    } /* end for (linking like entries) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mkdirlist_link) */

int mkdirlist_showdef(mkdirlist *op) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = mkdirlist_magic(op)) >= 0) {
	    vechand	*dlp = op->dlp ;
	    void	*vp ;
	    for (int i = 0 ; dlp->get(i,&vp) >= 0 ; i += 1) {
	        mkdirlist_ent	*ep = entp(vp) ;
	        if (vp) {
	            rs = entry_showdef(ep) ;
	            c += rs ;
	        } /* end if (non-null entry) */
	        if (rs < 0) break ;
	    } /* end for */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mkdirlist_showdef) */

int mkdirlist_show(mkdirlist *op,cchar *ng,int order) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = mkdirlist_magic(op,ng)) >= 0) {
	    rs = SR_INVALID ;
	    if (ng[0]) {
		vechand		*dlp = op->dlp ;
		void		*vp{} ;
		rs = SR_OK ;
	        for (int i = 0 ; dlp->get(i,&vp) >= 0 ; i += 1) {
	            mkdirlist_ent	*ep = entp(vp) ;
	            if (vp) {
	                rs = entry_show(ep,ng,order) ;
	                c += rs ;
		    } /* end if (non-null entry) */
	            if (rs < 0) break ;
	        } /* end for */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mkdirlist_show) */

int mkdirlist_ung(mkdirlist *op,cc *ung,time_t utime,int f_sub,int order) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = mkdirlist_magic(op,ung)) >= 0) {
	    rs = SR_INVALID ;
	    if (ung[0]) {
	        vechand		*dlp = op->dlp ;
		void		*vp{} ;
		rs = SR_OK ;
	        for (int i = 0 ; dlp->get(i,&vp) >= 0 ; i += 1) {
	            mkdirlist_ent	*ep = entp(vp) ;
	            if (vp) {
	                rs = entry_matung(ep,ung,utime,f_sub,order) ;
	                c += rs ;
		    } /* end if (non-null entry) */
	            if (rs < 0) break ;
	        } /* end for (looping through entrires) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mkdirlist_ung) */

int mkdirlist_sort(mkdirlist *op) noex {
	int		rs ;
	if ((rs = mkdirlist_magic(op)) >= 0) {
	    vechand_vcmp	vcf = vechand_vcmp(vcmporder) ;
	    rs = vechand_sort(op->dlp,vcf) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mkdirlist_sort) */

int mkdirlist_audit(mkdirlist *op) noex {
	int		rs ;
	if ((rs = mkdirlist_magic(op)) >= 0) {
	    vechand	*dlp = op->dlp ;
	    rs = vechand_audit(dlp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mkdirlist_audit) */


/* private subroutines */

local int mkdirlist_pdc(mkdirlist *op,cchar *ndn,int fd) noex {
	cint		plen = var.maxpathlen ;
	cint		sz = ((var.maxpathlen + 1) * 2) ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	int		ai = 0 ;
	cchar		*dcm = MKDIRLIST_DCMAGIC ;
	if (char *ap ; (rs = lm_mall(sz,&ap)) >= 0) {
	    char	*dbuf = (ap + (plen * ai++)) ;
	    if ((rs = mkpath(dbuf,ndn)) >= 0) {
	        cint	dlen = rs ;
	        if (filer b ; (rs = b.start(fd,0z,0,0)) >= 0) {
	            cint	nlen = plen ;
	            int		line = 0 ;
	            bool	f_bol = true ;
	            bool	f_eol ;
	            char	*nbuf = (ap + (plen * ai++)) ;
	            while ((rs = b.readln(nbuf,nlen,-1)) > 0) {
	                int	len = rs ;
	                f_eol = (len && (nbuf[len-1] == '\n')) ;
	                if (f_eol) nbuf[--len] = '\0' ;
	                if (f_bol) {
	                    if (line++ == 0) {
	                        if (strwcmp(dcm,nbuf,len) != 0) {
	                            rs = SR_BADFMT ;
	                        }
	                    } else {
	                        if ((rs = pathadd(dbuf,dlen,nbuf)) >= 0) {
	            		    ustat	sb ;
	                            if ((rs = u_stat(dbuf,&sb) >= 0)) {
	                                rs = mkdirlist_newent(op,&sb,nbuf,len) ;
	                                c += rs ;
	                            } else if (isNotPresent(rs)) {
					rs = SR_OK ;
				    }
	                        }
	                    } /* end if */
	                } /* end if (BOL) */
	                f_bol = f_eol ;
	                if (rs < 0) break ;
	            } /* end while */
	            rs1 = b.finish ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if */
	    } /* end if (mkpath) */
	    rs1 = lm_free(ap) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mkdirlist_pdc) */

local int mkdirlist_pdn(MKDIRLIST *op,cchar *ndn) noex {
	cint		fdm = (fsdirtreem.follow | fsdirtreem.dir) ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (char *nbuf ; (rs = lm_mp(&nbuf)) >= 0) {
	    cint	nlen = rs ;
	    if (fsdirtree d ; (rs = d.open(ndn,fdm)) >= 0) {
	        for (ustat sb ; (rs = d.read(&sb,nbuf,nlen)) > 0 ; ) {
	            cint	ngl = rs ;
	            if (nbuf[0] != '.') {
	                rs = mkdirlist_newent(op,&sb,nbuf,ngl) ;
	                c += rs ;
	            }
	            if (rs < 0) break ;
	        } /* end while */
	        rs1 = d.close ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (fsdirtree) */
	    rs1 = lm_free(nbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mkdirlist_pdn) */

local int mkdirlist_newent(mkdirlist *op,ustat *sbp,cc *nbuf,int nlen) noex {
	cint		esz = szof(mkdirlist_ent) ;
	int		rs ;
	int		c = 0 ;
	if (void *vp ; (rs = lm_mall(esz,&vp)) >= 0) {
	    mkdirlist_ent	*ep = entp(vp) ;
	    if ((rs = entry_start(ep,sbp,nbuf,nlen)) > 0) { /* rs>0 */
	        c = rs ;
	        rs = vechand_add(op->dlp,ep) ;
	        if (rs < 0) {
	            entry_finish(ep) ;
		}
	    } /* end if (entry) */
	    if ((rs < 0) || (c == 0)) {
	        lm_free(vp) ;
	    }
	} /* end if (memory-allocation) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mkdirlist_newent) */

local int mkdirlist_finents(mkdirlist *op) noex {
	vechand		*dlp = op->dlp ;
	cint		rsn = SR_NOTFOUND ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		rs2 ;
	void		*vp{} ;
	for (int i = 0 ; (rs2 = dlp->get(i,&vp)) >= 0 ; i += 1) {
	    mkdirlist_ent	*ep = entp(vp) ;
	    if (vp) {
		{
	    	    rs1 = entry_finish(ep) ;
	    	    if (rs >= 0) rs = rs1 ;
		}
		{
	    	    rs1 = lm_free(ep) ;
	    	    if (rs >= 0) rs = rs1 ;
		}
	    } /* end if (non-null entry) */
	} /* end for */
	if ((rs >= 0) && (rs2 != rsn)) rs = rs2 ;
	return rs ;
}
/* end subroutine (mkdirlist_finents) */

local int entry_start(ENT *ep,ustat *sbp,cchar *dbuf,int dlen) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (ep) ylikely {
	    memclear(ep) ; /* dangerous */
	    if (cchar *cp{} ; (rs = lm_strw(dbuf,dlen,&cp)) >= 0) {
	        cint	nlen = rs ;
	        ep->name = cp ;
	        c += 1 ;
	        ep->nlen = nlen ;
	        ep->mode = sbp->st_mode ;
	        ep->mtime = sbp->st_mtime ;
	        ep->ino = sbp->st_ino ;
	        ep->dev = sbp->st_dev ;
	    } /* end if (memory-allocation) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (entry_start) */

local int entry_finish(ENT *ep) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (ep->name) {
	    void *vp = voidp(ep->name) ;
	    rs1 = lm_free(vp) ;
	    if (rs >= 0) rs = rs1 ;
	    ep->name = nullptr ;
	}
	return rs ;
}
/* end subroutine (entry_finish) */

local int entry_matung(ENT *ep,cc *ung,time_t ut,int f_sub,int order) noex {
	int		rs = SR_OK ;
	if (! ep->fl.link) {
	    if (bbcmp(ung,ep->name) == 0) {
	        rs = 1 ;
	        ep->fl.seen = true ;
	        ep->fl.subscribe = !!f_sub ;
	        ep->utime = ut ;
		ep->order = order ;
	    } /* end if (name match) */
	} /* end if (not a linked entry) */
	return rs ;
}
/* end subroutine (entry_matung) */

local int entry_showdef(ENT *ep) noex {
	int		rs = SR_OK ;
	if (!ep->fl.link) {
	    ep->fl.show = ep->fl.subscribe ;
	    if (ep->fl.show) rs = 1 ;
	}
	return rs ;
}
/* end subroutine (entry_showdef) */

local int entry_show(ENT *ep,cchar *ng,int order) noex {
	int		rs = SR_OK ;
	if (! ep->fl.link) {
	    if (bbcmp(ng,ep->name) == 0) {
		ep->order = order ;
	        ep->fl.show = true ;
	        rs = 1 ;
	    } /* end if (name match) */
	} /* end if (not a linked entry) */
	return rs ;
}
/* end subroutine (entry_show) */

local int vcmporder(cvoid **v1pp,cvoid **v2pp) noex {
	mkdirlist_ent	*e1p = entp(*v1pp) ;
	mkdirlist_ent	*e2p = entp(*v2pp) ;
	int		rc = 0 ;
	if (e1p || e2p) {
	    rc = +1 ;
	    if (e1p) {
		rc = -1 ;
	        if (e2p) {
		    rc = (e1p->order - e2p->order) ;
		}
	    }
	}
	return rc ;
}
/* end subroutine (vcmporder) */

vars::operator int () noex {
	int		rs ;
	if ((rs = getbufsize(bufsize_mp)) >= 0) {
	    maxpathlen = rs ;
	}
	return rs ;
}
/* end method (vars::operator) */


