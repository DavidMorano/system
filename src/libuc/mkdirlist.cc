/* mkdirlist SUPPORT */
/* lang=C++20 */

/* create a list of the newsgroup directories */
/* version %I% last-modified %G% */


/* revision history:

	= 1994-04-13, David A­D­ Morano
	This is new so that we can track directory visits for
	duplicates.

	= 2014-11-25, David A­D­ Morano
	This object was enhanced to include much of the functionality
	from the old 'get_bds' function.

*/

/* Copyright © 2014 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Create a list of pathnames for each newsgroup in the spool
	area.  This routine is called with the path to the spool
	area directory.  The routine returns two open file pointers
	(Basic I/O).  These file pointers must be eventually closed
	by the calling routine or somebody!

	One returned file pointer is to a file of the path names.
	The other file pointer is a file of an array of 'stat(2)'
	structures corresponding to the directory specified by the
	path in the other file.

	Synopsis:
	int mkdirlist(char *basedir,bfile *sfp,bfile *nfp,int (*uf)()) noex

	Arguments:
	basedir		directory at top of tree
	nfp		Name File Pointer
	sfp		Stat File Pointer

	Returns:
	>=0		count
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<climits>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<usupport.h>		/* |memclear(3u)| */
#include	<mallocxx.h>
#include	<bfile.h>
#include	<filebuf.h>
#include	<fsdirtree.h>
#include	<mkpathx.h>
#include	<pathadd.h>
#include	<strwcmp.h>
#include	<mktmp.h>		/* |mktmpfile(3uc)| */
#include	<isnot.h>
#include	<localmisc.h>

#include	"mkdirlist.h"


/* local defines */

#define	MKDIRLIST_DIRCACHE	".dircache"
#define	MKDIRLIST_DCMAGIC	"DIRCACHE"

#define	DS_SIZE		sizeof(MKDIRLIST)

#ifndef	TMPDNAME
#define	TMPDNAME	"/tmp"
#endif

#define	ENT		mkdirlist_ent


/* local namespaces */

using std::nothrow ;			/* constant */


/* local typedefs */

typedef mkdirlist_ent	ent ;
typedef mkdirlist_ent *	entp ;


/* external subroutines */

extern "C" {
    extern int	openpcsdircache(cchar *,cchar *,int,mode_t,int) noex ;
    extern int	bbcmp(cchar *,cchar *) noex ;
}


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static int mkdirlist_ctor(mkdirlist *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = SR_NOMEM ;
	    memclear(op) ;		/* dangerous */
	    if ((op->dlp = new(nothrow) vechand) != nullptr) {
		rs = SR_OK ;
	    } /* end if (new-vechand) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mkdirlist_ctor) */

static int mkdirlist_dtor(mkdirlist *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->dlp) {
		delete op->dlp ;
		op->dlp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (clusterdb_dtor) */

template<typename ... Args>
static int mkdirlist_magic(mkdirlist *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == MKDIRLIST_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (mkdirlist_magic) */

static int mkdirlist_procdircache(mkdirlist *,cchar *,int) noex ;
static int mkdirlist_procnewsdir(mkdirlist *,cchar *) noex ;
static int mkdirlist_newent(mkdirlist *,USTAT *,cchar *,int) noex ;
static int mkdirlist_entfins(mkdirlist *) noex ;

static int entry_start(ENT *,USTAT *,cchar *,int) noex ;
static int entry_finish(ENT *) noex ;
static int entry_showdef(ENT *) noex ;
static int entry_show(ENT *,cchar *,int) noex ;
static int entry_matung(ENT *,cchar *,time_t,int,int) noex ;

static int ordercmp(ENT **,ENT **) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int mkdirlist_start(mkdirlist *op,cchar *pr,cchar *ndname) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = mkdirlist_ctor(op,pr,ndname)) >= 0) {
	    rs = SR_INVALID ;
	    if (pr[0] && ndname[0]) {
	        if ((rs = vechand_start(op->dlp,20,0)) >= 0) {
	            cint	of = O_RDONLY ;
	            cmode	om = 0666 ;
	            if ((rs = openpcsdircache(pr,ndname,of,om,-1)) >= 0) {
	                cint	fd = rs ;
	                if ((rs = mkdirlist_procdircache(op,ndname,fd)) >= 0) {
	                    c = rs ;
		        }
	                uc_close(fd) ;
	            } else if (isNotPresent(rs)) {
	                rs = mkdirlist_procnewsdir(op,ndname) ;
	                c = rs ;
	            }
	            if (rs >= 0) {
			op->magic = MKDIRLIST_MAGIC ;
		    }
	            if (rs < 0) {
	                mkdirlist_entfins(op) ;
	                vechand_finish(op->dlp) ;
	            }
	        } /* end if (vechand) */
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
	    {
	        rs1 = mkdirlist_entfins(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = vechand_finish(op->dlp) ;
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
	    rs = vechand_get(op->dlp,i,epp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mkdirlist_get) */

int mkdirlist_link(mkdirlist *op) noex {
	int		rs ;
	if ((rs = mkdirlist_magic(op)) >= 0) {
	    vechand		*dlp = op->dlp ;
	    mkdirlist_ent	*ep ;
	    for (int i = 0 ; vechand_get(dlp,i,&ep) >= 0 ; i += 1) {
	        if (ep == nullptr) continue ;

	    if (! ep->f.link) {
		auto	vg = vechand_get ;
		mkdirlist_ent	*pep = ep ;
	        for (int j = (i+1) ; vg(dlp,j,&ep) >= 0 ; j += 1) {
	            if (ep == nullptr) continue ;

	            if ((! ep->f.link) && (bbcmp(pep->name,ep->name) == 0)) {

	                pep->link = ep ;
	                ep->f.link = true ;

	                pep = ep ;
	            } /* end if (board match) */

	        } /* end for (inner) */

	    } /* end if (entry not linked) */

	    } /* end for (linking like entries) */

	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mkdirlist_link) */

int mkdirlist_showdef(mkdirlist *op) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = mkdirlist_magic(op)) >= 0) {
	    mkdirlist_ent	*ep ;
	    for (int i = 0 ; vechand_get(op->dlp,i,&ep) >= 0 ; i += 1) {
	        if (ep != nullptr) {
	            rs = entry_showdef(ep) ;
	            c += rs ;
	        }
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
	        mkdirlist_ent	*ep ;
	        for (int i = 0 ; vechand_get(op->dlp,i,&ep) >= 0 ; i += 1) {
	            if (ep == nullptr) continue ;
	            rs = entry_show(ep,ng,order) ;
	            c += rs ;
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
	        mkdirlist_ent	*ep ;
	        for (int i = 0 ; vechand_get(dlp,i,&ep) >= 0 ; i += 1) {
	            if (ep == nullptr) continue ;
	            rs = entry_matung(ep,ung,utime,f_sub,order) ;
	            c += rs ;
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
	    vechand_vcmp	vcf = vechand_vcmp(ordercmp) ;
	    rs = vechand_sort(op->dlp,vcf) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mkdirlist_sort) */

int mkdirlist_audit(MKDIRLIST *op) noex {
	int		rs ;
	if ((rs = mkdirlist_magic(op)) >= 0) {
	    vechand	*dlp = op->dlp ;
	    rs = vechand_audit(dlp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mkdirlist_audit) */


/* private subroutines */

static int mkdirlist_procdircache(mkdirlist *op,cchar *ndname,int fd) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	cchar		*dcm = MKDIRLIST_DCMAGIC ;
	char		dbuf[MAXPATHLEN+1] ;

	if ((rs = mkpath1(dbuf,ndname)) >= 0) {
	    filebuf	b ;
	    cint	dlen = rs ;

	    if ((rs = filebuf_start(&b,fd,0L,0,0)) >= 0) {
	        USTAT	sb ;
	        cint	nlen = MAXPATHLEN ;
	        int	line = 0 ;
	        int	f_bol = true ;
	        int	f_eol ;
	        char	nbuf[MAXPATHLEN+1] ;
	        while ((rs = filebuf_readln(&b,nbuf,nlen,-1)) > 0) {
	            int		len = rs ;
	            f_eol = (len && (nbuf[len-1] == '\n')) ;
	            if (f_eol) nbuf[--len] = '\0' ;
	            if (f_bol) {
	                if (line++ == 0) {
	                    if (strwcmp(dcm,nbuf,len) != 0) {
	                        rs = SR_BADFMT ;
	                    }
	                } else {
	                    if ((rs = pathadd(dbuf,dlen,nbuf)) >= 0) {
	                        if (u_stat(dbuf,&sb) >= 0) {
	                            rs = mkdirlist_newent(op,&sb,nbuf,len) ;
	                            c += rs ;
	                        }
	                    }
	                }
	            } /* end if (BOL) */
	            f_bol = f_eol ;
	            if (rs < 0) break ;
	        } /* end while */
	        filebuf_finish(&b) ;
	    } /* end if */

	} /* end if (mkpath) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mkdirlist_procdircache) */

static int mkdirlist_procnewsdir(MKDIRLIST *op,cchar *ndname) noex {
	cint		dot = (FSDIRTREE_MFOLLOW | FSDIRTREE_MDIR) ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	char		*nbuf{} ;
	if ((rs = malloc_mp(&nbuf)) >= 0) {
	    fsdirtree	dir ;
	    cint	nlen = rs ;
	    if ((rs = fsdirtree_open(&dir,ndname,dot)) >= 0) {
	        USTAT	sb ;
	        while ((rs = fsdirtree_read(&dir,&sb,nbuf,nlen)) > 0) {
	            cint	ngl = rs ;
	            if (nbuf[0] != '.') {
	                rs = mkdirlist_newent(op,&sb,nbuf,ngl) ;
	                c += rs ;
	            }
	            if (rs < 0) break ;
	        } /* end while */
	        rs1 = fsdirtree_close(&dir) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (fsdirtree) */
	    rs1 = uc_free(nbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mkdirlist_procnewsdir) */

static int mkdirlist_newent(mkdirlist *op,USTAT *sbp,char *nbuf,int nlen) noex {
	cint		esize = sizeof(mkdirlist_ent) ;
	int		rs ;
	int		c = 0 ;
	void		*vp{} ;
	if ((rs = uc_malloc(esize,&vp)) >= 0) {
	    mkdirlist_ent	*ep = entp(vp) ;
	    if ((rs = entry_start(ep,sbp,nbuf,nlen)) > 0) { /* rs>0 */
	        c = rs ;
	        rs = vechand_add(op->dlp,ep) ;
	        if (rs < 0) {
	            entry_finish(ep) ;
		}
	    } /* end if (entry) */
	    if ((rs < 0) || (c == 0)) {
	        uc_free(vp) ;
	    }
	} /* end if (memory-allocation) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (mkdirlist_newent) */

static int mkdirlist_entfins(mkdirlist *op) noex {
	vechand		*dlp = op->dlp ;
	int		rs = SR_OK ;
	int		rs1 ;
	void		*vp{} ;
	for (int i = 0 ; vechand_get(dlp,i,&vp) >= 0 ; i += 1) {
	    if (vp) {
		mkdirlist_ent	*ep = entp(vp) ;
		{
	    	    rs1 = entry_finish(ep) ;
	    	    if (rs >= 0) rs = rs1 ;
		}
		{
	    	    rs1 = uc_free(ep) ;
	    	    if (rs >= 0) rs = rs1 ;
		}
	    }
	} /* end for */
	return rs ;
}
/* end subroutine (mkdirlist_entfins) */

static int entry_start(ENT *ep,USTAT *sbp,cchar *dbuf,int dlen) noex {
	int		rs ;
	int		c = 0 ;
	cchar		*cp ;
	memclear(ep) ;			/* dangerous */
	if ((rs = uc_mallocstrw(dbuf,dlen,&cp)) >= 0) {
	    cint	nlen = rs ;
	    ep->name = cp ;
	    c += 1 ;
	    ep->nlen = nlen ;
	    ep->mode = sbp->st_mode ;
	    ep->mtime = sbp->st_mtime ;
	    ep->ino = sbp->st_ino ;
	    ep->dev = sbp->st_dev ;
	} /* end if (memory-allocation) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (entry_start) */

static int entry_finish(ENT *ep) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (ep->name) {
	    rs1 = uc_free(ep->name) ;
	    if (rs >= 0) rs = rs1 ;
	    ep->name = nullptr ;
	}
	return rs ;
}
/* end subroutine (entry_finish) */

static int entry_matung(ENT *ep,cc *ung,time_t utime,int f_sub,int order) noex {
	int		rs = SR_OK ;
	if (! ep->f.link) {
	    if (bbcmp(ung,ep->name) == 0) {
	        rs = 1 ;
	        ep->f.seen = true ;
	        ep->f.subscribe = f_sub ;
	        ep->utime = utime ;
		ep->order = order ;
	    } /* end if (name match) */
	} /* end if (not a linked entry) */
	return rs ;
}
/* end subroutine (entry_matung) */

static int entry_showdef(ENT *ep) noex {
	int		rs = SR_OK ;
	if (!ep->f.link) {
	    ep->f.show = ep->f.subscribe ;
	    if (ep->f.show) rs = 1 ;
	}
	return rs ;
}
/* end subroutine (entry_showdef) */

static int entry_show(ENT *ep,cchar *ng,int order) noex {
	int		rs = SR_OK ;
	if (! ep->f.link) {
	    if (bbcmp(ng,ep->name) == 0) {
		ep->order = order ;
	        ep->f.show = true ;
	        rs = 1 ;
	    } /* end if (name match) */
	} /* end if (not a linked entry) */
	return rs ;
}
/* end subroutine (entry_show) */

static int ordercmp(ENT **e1pp,ENT **e2pp) noex {
	mkdirlist_ent	*e1p = *e1pp ;
	mkdirlist_ent	*e2p = *e2pp ;
	int		rc = 0 ;
	if (e1p || e2p) {
	    rc = 1 ;
	    if (e1p) {
		rc = -1 ;
	        if (e2p) {
		    rc = (e1p->order - e2p->order) ;
		}
	    }
	}
	return rc ;
}
/* end subroutine (ordercmp) */


