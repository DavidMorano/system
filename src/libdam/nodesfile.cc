/* nodesfile SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* read (process) a standard UNIX® "nodes" file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	nodesfile

	Description:
	This little object supports reading the cluster 'nodes'
	file by mapping it into memory.  No data is copied out of
	the file map!  This is assumed to provide some performance
	advantage over reading it line by line and then allocating
	space for found strings.  Duplicate node entries are ignored.

	File-format:
	|
	| <node0>
	| <node1> <node2> <node2>
	| <node4>
	| 

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* |caddr_t| */
#include	<sys/stat.h>
#include	<sys/mman.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |lenstr(3c)| */
#include	<ctime>			/* |time(2)| */
#include	<new>			/* |nothrow(3c++)| */
#include	<memory>		/* |destroy_at(3c++)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<mallocxx.h>
#include	<estrings.h>
#include	<sfx.h>
#include	<hdb.h>
#include	<linebuffer.h>
#include	<bfile.h>
#include	<strwcpy.h>
#include	<hash.h>		/* |hash_elf(3uc)| */
#include	<localmisc.h>

#include	"nodesfile.h"

import libutil ;
import sif ;

/* local defines */

#define	NF		nodesfile
#define	NF_FI		nodesfile_fi
#define	NF_ENT		nodesfile_ent
#define	NF_CUR		nodesfile_cur
#define	NF_DEFNODES	200

#define	TO_CHECK	4
#define	TO_HOLD		2


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::destroy_at ;			/* subroutine */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct nodesfile_fi {
	cchar		*fname ;
	ino_t		ino ;
	time_t		timod ;
	dev_t		dev ;
	int start(cc *,dev_t,ino_t,time_t) noex ;
	int finish() noex ;
    } ;
    struct nodesfile_ent {
    	cchar		*name ;
	int		fi ;
	int start(cc *,int,int) noex ;
	int finish() noex ;
    } ;
}

int nodesfile_fi::start(cc *sp,dev_t d,ino_t i,time_t t) noex {
    	int		rs = SR_OK ;
	dev = d ;
	ino = i ;
	timod = t ;
	fname = nullptr ;
	if (sp) {
	    if (cchar *cp ; (rs = uc_mallocstrw(sp,-1,&cp)) >= 0) {
	        fname = cp ;
	    }
	}
	return rs ;
}

int nodesfile_fi::finish() noex {
    	int		rs = SR_FAULT ;
	int		rs1 ;
	dev = 0 ;
	ino = 0 ;
	timod = 0 ;
	if (fname) {
	    rs1 = uc_free(fname) ;
	    if (rs >= 0) rs = rs1 ;
	    fname = nullptr ;
	}
	return rs ;
}

int nodesfile_ent::start(cc *sp,int sl,int idx) noex {
    	int		rs = SR_FAULT ;
	if (sp) {
	    fi = idx ;
	    if (cchar *cp ; (rs = uc_mallocstrw(sp,sl,&cp)) >= 0) {
	        name = cp ;
	    }
	}
	return rs ;
}

int nodesfile_ent::finish() noex {
    	int		rs = SR_FAULT ;
	int		rs1 ;
	if (name) {
	    rs1 = uc_free(name) ;
	    if (rs >= 0) rs = rs1 ;
	    name = nullptr ;
	}
	return rs ;
}


/* forward references */

template<typename ... Args>
static inline int nodesfile_ctor(nodesfile *op,Args ... args) noex {
    	NODESFILE	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->flp = new(nothrow) vechand) != nullptr) {
	        if ((op->elp = new(nothrow) hdb) != nullptr) {
		    rs = SR_OK ;
	        } /* end if (new-hdb) */
		if (rs < 0) {
		    delete op->flp ;
		    op->flp = nullptr ;
		}
	    } /* end if (new-vechand) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (nodesfile_ctor) */

static int nodesfile_dtor(nodesfile *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->elp) {
		delete op->elp ;
		op->elp = nullptr ;
	    }
	    if (op->flp) {
		delete op->flp ;
		op->flp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (nodesfile_dtor) */

template<typename ... Args>
static inline int nodesfile_magic(nodesfile *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == NODESFILE_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (nodesfile_magic) */

static int	nodesfile_fnparse(NF *,cchar *) noex ;
static int	nodesfile_fnparseln(NF *,int,cchar *,int) noex ;
static int	nodesfile_fnparseload(NF *,int,cchar *,int) noex ;

static int	nodesfile_fnloadbegin(NF *,cc *,bfile *) noex ;
static int	nodesfile_fnloadend(NF *,int) noex ;
static int	nodesfile_fnalready(NF *,USTAT *) noex ;

#ifdef	COMMENT
static int	nodesfile_filechanged(NF *,time_t) noex ;
#endif

static int	nodesfile_fins(NF *) noex ;
static int	nodesfile_finents(NF *) noex ;
static int	nodesfile_finfis(NF *) noex ;

extern "C" {
    static uint	enthash(cvoid *,int) noex ;
    static int	entmat(cvoid *,cvoid *,int) noex ;
}


/* local variables */


/* exported variables */


/* exported subroutines */

int nodesfile_open(NF *op,cchar *fname,int maxsz) noex {
	int		rs ;
	if ((rs = nodesfile_ctor(op)) >= 0) {
	    vechand	*flp = op->flp ;
	    hdb		*nlp = op->elp ;
	    rs = SR_INVALID ;
	    if (maxsz >= 0) {
		cint	vn = 1 ;
		cint	vo = 0 ;
		op->maxsz = maxsz ;
		if ((rs = flp->start(vn,vo)) >= 0) {
		    cint	hn = 1 ;
		    cint	at = 0 ;
		    if ((rs = nlp->start(hn,at,enthash,entmat)) >= 0) {
			op->magic = NODESFILE_MAGIC ;
			if (fname) {
		            rs = nodesfile_fnparse(op,fname) ;
			}
			if (rs < 0) {
			    nlp->finish() ;
			    op->magic = 0 ;
			}
	            } /* end if (hdb_start) */
		    if (rs < 0) {
			flp->finish() ;
		    }
		} /* end if (vechand_start) */
	    } /* end if (valid) */
	    if (rs < 0) {
		nodesfile_dtor(op) ;
	    }
	} /* end if (nodesfile_ctor) */
	return rs ;
}
/* end subroutine (nodesfile_open) */

int nodesfile_close(NF *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_OK ;
	    {
		rs1 = nodesfile_fins(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    if (op->elp) {
		rs1 = hdb_finish(op->elp) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    if (op->flp) {
		rs1 = vechand_finish(op->flp) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = nodesfile_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (nodesfile_close) */

static int nodesfile_fins(NF *op) noex {
    	int		rs = SR_OK ;
	int		rs1 ;
	{
	    rs1 = nodesfile_finents(op) ;
	    if (rs >= 0) rs = rs1 ;
	}
	{
	    rs1 = nodesfile_finfis(op) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (nodesfile_fins) */

static int nodesfile_finents(NF *op) noex {
    	hdb		*elp = op->elp ;
	cint		rsn = SR_NOTFOUND ;
    	int		rs ;
	int		rs1 ;
	int		rs2 ;
	if (hdb_cur cur ; (rs = hdb_curbegin(elp,&cur)) >= 0) {
	    hdb_dat	key{} ;
	    hdb_dat	val{} ;
	    while ((rs2 = hdb_curenum(elp,&cur,&key,&val)) >= 0) {
		void	*vp = cast_const<voidp>(key.buf) ;
		if (vp) {
		    NF_ENT *ep = resumelife<NF_ENT>(vp) ;
		    {
		        rs1 = ep->finish() ;
		        if (rs >= 0) rs = rs1 ;
		    }
		    destroy_at(ep) ;
		    {
			rs1 = uc_free(vp) ;
		        if (rs >= 0) rs = rs1 ;
		    }
		} /* end if (non-null) */
	    } /* end while */
	    if ((rs >= 0) && (rs2 != rsn)) rs = rs2 ;
	    rs1 = hdb_curend(elp,&cur) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (hdb-cur) */
	return rs ;
}
/* end subroutine (nodesfile_finents) */

static int nodesfile_finfis(NF *op) noex {
    	vechand		*flp = op->flp ;
	cint		rsn = SR_NOTFOUND ;
    	int		rs ;
	int		rs1 ;
	int		rs2 ;
	void *vp ;
	for (int i = 0 ; (rs2 = flp->get(i,&vp)) >= 0 ; i += 1) {
	    if (vp) {
	        NF_FI	*fep = resumelife<NF_FI>(vp) ;
		{
		    rs1 = fep->finish() ;
		    if (rs >= 0) rs = rs1 ;
		}
		destroy_at(fep) ;
		{
		    rs1 = uc_free(vp) ;
		    if (rs >= 0) rs = rs1 ;
		}
	    } /* end if (non-null) */
	} /* end for */
	if ((rs >= 0) && (rs2 != rsn)) rs = rs2 ;
	return rs ;
}
/* end subroutine (nodesfile_finfis) */

int nodesfile_add(NF *op,cchar *fn) noex {
    	int		rs ;
	if ((rs = nodesfile_magic(op,fn)) >= 0) {
	    rs = SR_INVALID ;
	    if (fn[0]) {
		rs = nodesfile_fnparse(op,fn) ;
	    }
	} /* end if (magic) */
    	return rs ;
}
/* end subroutine (nodesfile_add) */

int nodesfile_check(NF *op,time_t dt) noex {
	int		rs ;
	int		f_changed = false ;
	if ((rs = nodesfile_magic(op)) >= 0) {
	    if (dt == 0) dt = getustime ;
	    if ((dt - op->ti_check) >= TO_CHECK) {
	        op->ti_check = dt ;
	    } /* end if (timeout) */
	} /* end if (magic) */
	return (rs >= 0) ? f_changed : rs ;
}
/* end subroutine (nodesfile_check) */

int nodesfile_have(NF *op,cchar *sp,int sl) noex {
	int		rs ;
	int		rs1 ;
	int		fal = false ;
	if ((rs = nodesfile_magic(op,sp)) >= 0) {
	    if (sl < 0) sl = lenstr(sp) ;
	    if (NF_ENT e ; (rs = e.start(sp,sl,0)) >= 0) {
	        hdb		*elp = op->elp ;
	        hdb_dat		key ;
	        key.buf = &e ;
	        key.len = sl ;
	        if ((rs = elp->have(key)) > 0) {
		    fal = true ;
	        }
	        rs1 = e.finish() ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ent-) */
	} /* end if (magic) */
	return (rs >= 0) ? fal : rs ;
}
/* end subroutine (nodesfile_have) */

int nodesfile_curbegin(NF *op,NF_CUR *curp) noex {
    	cnullptr	np{} ;
	int		rs ;
	if ((rs = nodesfile_magic(op,curp)) >= 0) {
	    cint	sz = szof(hdb_cur) ;
	    if (void *vp ; (rs = uc_malloc(sz,&vp)) >= 0) {
		rs = SR_BUGCHECK ;
		if ((curp->hcp = new(vp) hdb_cur) != np) {
		    {
	                rs = hdb_curbegin(op->elp,curp->hcp) ;
		    }
		    if (rs < 0) {
		        destroy_at(curp->hcp) ;
		    }
		} /* end if (new-hdb_cur) */
		if (rs < 0) {
		    uc_free(vp) ;
		    curp->hcp = nullptr ;
		}
	    } /* end if (m-a) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (nodesfile_curbegin) */

int nodesfile_curend(NF *op,NF_CUR *curp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = nodesfile_magic(op,curp)) >= 0) {
	    if (curp->hcp) {
		void	*vp = curp->hcp ;
		{
	            rs1 = hdb_curend(op->elp,curp->hcp) ;
		    if (rs >= 0) rs = rs1 ;
	        }
		destroy_at(curp->hcp) ;
		{
		    rs1 = uc_free(vp) ;
		    if (rs >= 0) rs = rs1 ;
		    curp->hcp = nullptr ;
	        }
	    } /* end if (non-null cursor) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (nodesfile_curend) */

int nodesfile_curenum(NF *op,NF_CUR *curp,char *rbuf,int rlen) noex {
	int		rs ;
	int		rl = 0 ;
	if ((rs = nodesfile_magic(op,curp,rbuf)) >= 0) {
	    hdb		*elp = op->elp ;
	    hdb_datum	key{} ;
	    hdb_datum	val{} ;
	    if ((rs = elp->curenum(curp->hcp,&key,&val)) >= 0) {
		NF_ENT	*ep = (NF_ENT *) key.buf ;
		if (key.buf) {
		    rs = sncpy(rbuf,rlen,ep->name) ;
		    rl = rs ;
		}
	    }
	} /* end if (magic) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (nodesfile_curenum) */


/* private subroutines */

static int nodesfile_fnparse(NF *op,cc *fn) noex {
    	int		rs ;
	int		rs1 ;
	if (linebuffer lb ; (rs = lb.start) >= 0) {
	    if (bfile bf ; (rs = bf.open(fn,"r")) >= 0) {
	        if ((rs = nodesfile_fnloadbegin(op,fn,&bf)) >= 0) {
		    if (rs < INT_MAX) {
		        cint	fi = rs ;
		        while ((rs = bf.readln(lb.lbuf,lb.llen)) >= 0) {
			    auto 	sfc = sfcontent ;
			    cchar	*lp ;
			    if (int ll ; (ll = sfc(lb.lbuf,rs,&lp)) > 0) {
			        rs = nodesfile_fnparseln(op,fi,lp,ll) ;
			    }
			    if (rs < 0) break ;
		        } /* end while */
		        if (rs < 0) {
			    nodesfile_fnloadend(op,fi) ;
		        }
		    } /* end if (go ahead) */
	        } /* end if (fnloadbegin) */
		rs1 = bf.close ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (bfile) */
	    rs1 = lb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (linebuffer) */
	return rs ;
}
/* end subroutine (nodesfile_fnparse) */

static int nodesfile_fnparseln(NF *op,int fi,cc *lp,int ll) noex {
    	int		rs = SR_OK ;
	sif		lo(lp,ll) ;
	cchar		*cp ;
	for (int cl ; (cl = lo.next(&cp)) > 0 ; ) {
	    rs = nodesfile_fnparseload(op,fi,cp,cl) ;
	    if (rs < 0) break ;
	} /* end for */
    	return rs ;
}
/* end subroutine (nodesfile_fnparseln) */

static int nodesfile_fnloadbegin(NF *op,cchar *fn,bfile *fp) noex {
    	cnullptr	np{} ;
    	int		rs ;
	int		idx = INT_MAX ;
	if (USTAT sb ; (rs = fp->stat(&sb)) >= 0) {
	    if ((rs = nodesfile_fnalready(op,&sb)) == 0) {
		cint	sz = szof(NF_FI) ;
		if (void *vp ; (rs = uc_malloc(sz,&vp)) >= 0) {
		    rs = SR_BUGCHECK ;
		    if (NF_FI *fep ; (fep = new(vp) NF_FI) != np) {
		        const dev_t	d = sb.st_dev ;
		        const ino_t	i = sb.st_ino ;
		        const time_t	t = sb.st_mtime ;
		        if ((rs = fep->start(fn,d,i,t)) >= 0) {
			    vechand	*flp = op->flp ;
			    if ((rs = flp->add(fep)) >= 0) {
			        idx = rs ;
			    }
			    if (rs < 0) {
			        flp->finish() ;
			    }
		        } /* end if (file_start) */
			if (rs < 0) {
			    destroy_at(fep) ;
			}
		    } /* end if (new-file) */
		    if (rs < 0) {
			uc_free(vp) ;
		    }
		} /* end if (memory-allocation) */
	    } /* end if (nodesfile_fnalready) */
	} /* end if (bfile_stat) */
	return (rs >= 0) ? idx : rs ;
}
/* end subroutine (nodesfile_fnloadbegin) */

static int nodesfile_fnloadend(NF *op,int fi) noex {
    	vechand		*flp = op->flp ;
    	int		rs ;
	if (void *vp ; (rs = flp->get(fi,&vp)) >= 0) {
	    if (vp) {
	        NF_FI	*fep = resumelife<NF_FI>(vp) ;
		if ((rs = flp->del(fi)) >= 0) {
	            if ((rs = fep->finish()) >= 0) {
			destroy_at(fep) ;
		        rs = uc_free(vp) ;
	            }
		}
	    } /* end if (non-null) */
	} /* end if (get) */
	return rs ;
}
/* end subroutine (nodesfile_fnloadend) */

static int nodesfile_fnalready(NF *op,USTAT *sbp) noex {
    	vechand		*flp = op->flp ;
    	int		rs ;
	int		fal = false ;
	void		*vp ;
	for (int i = 0 ; (rs = flp->get(i,&vp)) >= 0 ; i += 1) {
	    if (vp) {
	        NF_FI	*fep = resumelife<NF_FI>(vp) ;
	        fal = true ;
		fal = fal && (sbp->st_dev == fep->dev) ;
		fal = fal && (sbp->st_ino == fep->ino) ;
	    }
	    if (fal) break ;
	} /* end for */
	return (rs >= 0) ? fal : rs ;
}
/* end subroutine (nodesfile_fnalready) */

static int nodesfile_fnparseload(NF *op,int fi,cchar *sp,int sl) noex {
    	cnullptr	np{} ;
    	cint		esz = szof(NF_ENT) ;
	int		rs ;
	if (char *vp ; (rs = uc_malloc(esz,&vp)) >= 0) {
	    hdb		*elp = op->elp ;
	    rs = SR_BUGCHECK ;
	    if (NF_ENT *ep ; (ep = new(vp) NF_ENT) != np) {
	        if ((rs = ep->start(sp,sl,fi)) >= 0) {
		    hdb_dat	key ;
		    key.buf = ep ;
		    key.len = 0 ;
	            if ((rs = elp->have(key)) == 0) {
		        hdb_dat	val{} ;
		        rs = elp->store(key,val) ;
		    }
		    if (rs < 0) {
		        ep->finish() ;
		    }
	        } /* end if (entry_start) */
		if (rs < 0) {
		    destroy_at(ep) ;
		}
	    } /* end if (new-entry) */
	    if (rs < 0) {
		uc_free(vp) ;
	    }
	} /* end if (memory-allocation) */
	return rs ;
}
/* end subroutine (nodesfile_fnparseload) */

#ifdef	COMMENT
static int nodesfile_filechanged(NF *op,time_t dt) noex {
	(void) op ;
	(void) dt ;
	return 0 ;
}
/* end subroutine (nodesfile_filechanged) */
#endif /* COMMENT */

#ifdef	COMMENT
static int nodesfile_filechanged(NF *op,time_t daytime) noex {
	int		rs ;
	int		f = false ;
	if (USTAT sb ; (rs = uc_stat(op->fi.fname,&sb)) >= 0) {
	    cuint	fsz = uint(sb.st_size) ;
	    f = (op->fi.timod > sb.st_mtime) ;
	    f = f || (op->fi.ino != sb.st_ino) ;
	    f = f || (op->fi.dev != sb.st_dev) ;
	    if (f) {
	        f = f && ((daytime - op->fi.timod) >= TO_HOLD) ;
	    }
	    if (f) {
	        op->filesize = fsz ;
	        op->fi.timod = sb.st_mtime ;
	        op->fi.ino = sb.st_ino ;
	        op->fi.dev = sb.st_dev ;
	        if (fsz > op->mapsize) {
	            if ((rs = nodesfile_filemapend(op)) >= 0) {
	                rs = nodesfile_filemapbegin(op) ;
		    }
	        }
	    } /* end if (file changed) */
	} /* end if (uc_stat) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (nodesfile_filechanged) */
#endif /* COMMENT */

static uint enthash(cvoid *cvp,int cvl) noex {
    NF_ENT	*ep = (NF_ENT *) cvp ;
    uint	hv ;
    {
	cchar	*nbuf = ep->name ;
        cint	nlen = cvl ;
        hv = hash_elf(nbuf,nlen) ;
    }
    return hv ;
}
/* end subroutine (enthash) */

static int entmat(cvoid *v1p,cvoid *v2p,int) noex {
    	NF_ENT	*e1p = (NF_ENT *) v1p ;
    	NF_ENT	*e2p = (NF_ENT *) v2p ;
	int	rc = 0 ;
	if (e1p || e2p) {
	    rc = +1 ;
	    if (e1p) {
		rc = -1 ;
		if (e2p) {
		    cchar	*s1 = e1p->name ;
		    cchar	*s2 = e2p->name ;
		    if ((rc = (*s1 - *s2)) == 0) {
	    	        rc = (strcmp(s1,s2) == 0) ;
		    }
		}
	    }
	}
	return rc ;
}
/* end subroutine (entmat) */


