/* svcfile SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* service-table file manager */
/* version %I% last-modified %G% */

#define	CF_DEVINO	0		/* check device-inode */
#define	CF_ALREADY	0		/* disallow duplicate entries */
#define	CF_MOREKEYS	0		/* |ientry_morekeys()| */
#define	CF_FILEDEL	0		/* file-delete */

/* revision history:

	- 2004-05-25, David A­D­ Morano
	This subroutine was adopted for use as a service-table file
	processor.

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

	Name:
	svcfile

	Description:
	This object processes an serice-table for use by daemon
	multiplexing server programs.

******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |lenstr(3c)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<netdb.h>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<bfile.h>
#include	<absfn.h>
#include	<field.h>
#include	<fieldterminit.hh>
#include	<sncpyx.h>
#include	<sfx.h>
#include	<mkpathx.h>
#include	<strwcpy.h>
#include	<char.h>
#include	<timestr.h>
#include	<localmisc.h>

#include	"svcfile.h"

import libutil ;

/* local defines */

#define	SVCFILE_INTCHECK	2	/* file-change check interval */
#define	SVCFILE_INTWAIT		2	/* file-change wait interval */

#define	SVCFILE_FILE		xsvcfile_file
#define	SVCFILE_KEYNAME		svcfile_keyname
#define	SVCFILE_SF_IE		svcfile_ie
#define	SVCFILE_SVCNAME		svcfile_svcname
#define	SF_IE			svcfile_ie

#define	SVCENTRY		svcentry
#define	SVCENTRY_KEY		svcentry_key

#undef	DEFCHUNKSIZE
#define	DEFCHUNKSIZE		512

#define	DEFNFILES		10
#define	DEFNENTRIES		10

#define	SVCFILE_KA		szof(char *(*)[2])
#define	SVCFILE_BO(v)		\
	((SVCFILE_KA - ((v) % SVCFILE_KA)) % SVCFILE_KA)


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct svcfile_svcname {
	cchar		*svcname ;
	int		count ;
} ;

struct xsvcfile_file {
	cchar		*fname ;
	time_t		timod ;
	ino_t		ino ;
	dev_t		dev ;
	int		fsize ;
} ;

struct svcfile_keyname {
	cchar		*kname ;
	int		count ;
} ;

struct svcfile_ie {
	cchar		*(*keyvals)[2] ;
	cchar		*svc ;
	int		nkeys ;			/* number of keys */
	int		sz ;			/* total size */
	int		fi ;			/* file index */
} ;

struct svcentry {
	vecobj		keys ;
	cchar		*svc ;
} ;

struct svcentry_key {
	cchar		*kname ;
	cchar		*args ;
	int		kl, al ;
} ;

struct vars {
	int		maxnamelen ;
	int		maxlinelen ;
	int		maxhostlen ;
} ;

typedef svcentry_key *	keyp ;
typedef cchar		*(*keyvals_t)[2] ;


/* forward references */

template<typename ... Args>
static int svcfile_ctor(svcfile *op,Args ... args) noex {
	SVCFILE		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    memclear(hop) ;
	    if ((op->flp = new(nothrow) vecobj) != np) {
	        if ((op->slp = new(nothrow) vecobj) != np) {
	            if ((op->elp = new(nothrow) hdb) != np) {
	 		rs = SR_OK ;
	            } /* end if (new-hdb) */
		    if (rs < 0) {
		        delete op->slp ;
		        op->slp = nullptr ;
		    }
	        } /* end if (new-vecobj) */
		if (rs < 0) {
		    delete op->flp ;
		    op->flp = nullptr ;
		}
	    } /* end if (new-vecobj) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (svcfile_ctor) */

static int svcfile_dtor(svcfile *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->elp) {
		delete op->elp ;
		op->elp = nullptr ;
	    }
	    if (op->slp) {
		delete op->slp ;
		op->slp = nullptr ;
	    }
	    if (op->flp) {
		delete op->flp ;
		op->flp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (svcfile_dtor) */

template<typename ... Args>
static inline int svcfile_magic(svcfile *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == SVCFILE_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (svcfile_magic) */

static int	svcfile_filefins(svcfile *) noex ;
static int	svcfile_fileparse(svcfile *,int) noex ;
static int	svcfile_fileparser(svcfile *,int,cchar *) noex ;
static int	svcfile_filedump(svcfile *,int) noex ;

#if	CF_FILEDEL
static int	svcfile_filedel(svcfile *,int) noex ;
#endif /* CF_FILEDEL */

static int	svcfile_addentry(svcfile *,int,SVCENTRY *) noex ;
static int	svcfile_checkfiles(svcfile *,time_t) noex ;

static int	svcfile_svcadd(svcfile *,cchar *) noex ;
static int	svcfile_svcdel(svcfile *,cchar *) noex ;
static int	svcfile_svcfins(svcfile *) noex ;

#if	CF_ALREADY
static int	svcfile_already(svcfile *,cchar *) noex ;
#endif

#if	CF_DEVINO
static int	svcfile_filealready(svcfile *,dev_t,ino_t) noex ;
#endif

static int	svcentry_start(SVCENTRY *,cchar *,int = -1) noex ;
static int	svcentry_addkey(SVCENTRY *,cchar *,int,cchar *,int) noex ;
static int	svcentry_nkeys(SVCENTRY *) noex ;
static int	svcentry_size(SVCENTRY *) noex ;
static int	svcentry_finish(SVCENTRY *) noex ;

static int	file_start(SVCFILE_FILE *,cchar *) noex ;
static int	file_finish(SVCFILE_FILE *) noex ;

static int	svcname_start(SVCFILE_SVCNAME *,cchar *) noex ;
static int	svcname_incr(SVCFILE_SVCNAME *) noex ;
static int	svcname_decr(SVCFILE_SVCNAME *) noex ;
static int	svcname_finish(SVCFILE_SVCNAME *) noex ;

static int	ientry_loadstr(SF_IE *,char *,SVCENTRY *) noex ;
static int	ientry_finish(SF_IE *) noex ;
#if	CF_MOREKEYS
static int	ientry_morekeys(SF_IE *,int,int) noex ;
#endif

static int	entry_load(svcfile_ent *,char *,int,SF_IE *) noex ;

static int	mkvars() noex ;

static int	vcmpfname(cvoid **,cvoid **) noex ;
static int	vcmpsvcname(cvoid **,cvoid **) noex ;


/* local variables */

/* all white space, pound ('#'), colon (':'), and comma (',') */
constexpr cchar		fterms[] = {
	0x00, 0x1F, 0x00, 0x00,
	0x09, 0x10, 0x00, 0x24,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ;

/* argument field terminators (pound '#' and comma ',') */
constexpr cchar		saterms[] = {
	0x00, 0x00, 0x00, 0x00,
	0x08, 0x10, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ;

static vars		var ;


/* exported variables */


/* exported subroutines */

int svcfile_open(svcfile *op,cchar *fname) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = svcfile_ctor(op)) >= 0) {
	    static cint		rsv = mkvars() ;
	    if ((rs = rsv) >= 0) {
	        cnullptr	np{} ;
	        int		sz = szof(SVCFILE_FILE) ;
	        int		vn = DEFNFILES ;
	        int		vo = (VECOBJ_OSTATIONARY | VECOBJ_OREUSE) ;
	        if ((rs = vecobj_start(op->flp,sz,vn,vo)) >= 0) {
	            sz = szof(SVCFILE_SVCNAME) ;
	            vo = VECOBJ_OCOMPACT ;
	            if ((rs = vecobj_start(op->slp,sz,10,vo)) >= 0) {
	                vn = DEFNENTRIES ;
	                if ((rs = hdb_start(op->elp,vn,0,np,np)) >= 0) {
	                    op->magic = SVCFILE_MAGIC ;
	                    op->checktime = getustime ;
	                    if (fname) {
	                        rs = svcfile_fileadd(op,fname) ;
	                        c = rs ;
	                    }
	                    if (rs < 0) {
	                        hdb_finish(op->elp) ;
	                        op->magic = 0 ;
	                    }
	                } /* end if (hdb-start) */
	                if (rs < 0) {
	                    svcfile_svcfins(op) ;
	                    vecobj_finish(op->slp) ;
	                }
	            } /* end if svcnames) */
	            if (rs < 0) {
	                vecobj_finish(op->flp) ;
	            }
	        } /* end if (vecobj_start) */
	    } /* end if (mkvars) */
	    if (rs < 0) {
		svcfile_dtor(op) ;
	    }
	} /* end if (svcfile_ctor) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (svcfile_open) */

int svcfile_close(svcfile *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = svcfile_magic(op)) >= 0) {
	    {
	        rs1 = svcfile_filedump(op,-1) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = svcfile_filefins(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = svcfile_svcfins(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    /* primary items */
	    {
	        rs1 = hdb_finish(op->elp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = vecobj_finish(op->slp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = vecobj_finish(op->flp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = svcfile_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (svcfile_close) */

int svcfile_fileadd(svcfile *op,cchar *fname) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = svcfile_magic(op,fname)) >= 0) {
	    absfn	af ;
	    int		fi = 0 ;
	    cchar	*fn{} ;
	    if ((rs = af.start(fname,-1,&fn)) >= 0) {
	        SVCFILE_FILE	fe ;
	        vecobj		*flp = op->flp ;
		cnullptr	np{} ;
	        if ((rs = file_start(&fe,fn)) >= 0) {
	            cint	rsn = SR_NOTFOUND ;
	            bool	f_fin = false ;
		    auto	vc = vcmpfname ;
	            if ((rs = vecobj_search(flp,&fe,vc,np)) == rsn) {
	                if ((rs = vecobj_add(flp,&fe)) >= 0) {
	                    fi = rs ;
	                    rs = svcfile_fileparse(op,fi) ;
	                    if (rs < 0) {
	                        f_fin = true ;
	                        vecobj_del(flp,fi) ;
	                    }
	                } /* end if (vecobj_add) */
	            } else {
	                f_fin = true ;
	            }
	            if ((rs < 0) || f_fin) {
	                file_finish(&fe) ;
	            }
	        } /* end if (file_start) */
		rs1 = af.finish ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (absfn) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (svcfile_fileadd) */

int svcfile_curbegin(svcfile *op,svcfile_cur *curp) noex {
	int		rs ;
	if ((rs = svcfile_magic(op,curp)) >= 0) {
	    if (op->ncursors == 0) {
	        rs = svcfile_check(op,0L) ;
	    }
	    if (rs >= 0) {
		hdb	*elp = op->elp ;
		cint	csz = szof(hdb_cur) ;
		void	*vp{} ;
	        curp->i = -1 ;
		if ((rs = uc_malloc(csz,&vp)) >= 0) {
		    hdb_cur	*ecp = (hdb_cur *) vp ;
	            if ((rs = hdb_curbegin(elp,ecp)) >= 0) {
			curp->ecp = ecp ;
	                op->ncursors += 1 ;
	                op->magic = SVCFILE_MAGIC ;
	            }
		    if (rs < 0) {
			uc_free(vp) ;
		    }
		} /* end if (memory-allocation) */
	    } /* end if (ok) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (svcfile_curbegin) */

int svcfile_curend(svcfile *op,svcfile_cur *curp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = svcfile_magic(op,curp)) >= 0) {
	    rs = SR_FAULT ;
	    if (curp->ecp) {
	        curp->i = -1 ;
	        if ((rs = hdb_curend(op->elp,curp->ecp)) >= 0) {
	            if (op->ncursors > 0) op->ncursors -= 1 ;
		    rs1 = uc_free(curp->ecp) ;
		    if (rs >= 0) rs = rs1 ;
		    curp->ecp = nullptr ;
	        }
	    } /* end if (non-null) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (svcfile_curend) */

int svcfile_curenumsvc(svcfile *op,svcfile_cur *curp,char *ebuf,int elen) noex {
	int		rs ;
	int		kl = 0 ;
	if ((rs = svcfile_magic(op,curp,ebuf)) >= 0) {
	    SVCFILE_SVCNAME	*snp = nullptr ;
	    int		i = (curp->i >= 0) ? (curp->i + 1) : 0 ;
	    void	*vp{} ;
	    while ((rs = vecobj_get(op->slp,i,&vp)) >= 0) {
	        snp = (SVCFILE_SVCNAME *) vp ;
	        if (snp) break ;
	        i += 1 ;
	    } /* end while */
	    if (rs >= 0) {
	        if ((rs = sncpy(ebuf,elen,snp->svcname)) >= 0) {
	            kl = rs ;
	            curp->i = i ;
	        }
	    } /* end if */
	} /* end if (magic) */
	return (rs >= 0) ? kl : rs ;
}
/* end subroutine (svcfile_curenumsvc) */

int svcfile_curenum(svcfile *op,svcfile_cur *curp,svcfile_ent *ep,
		char *ebuf,int elen) noex {
	int		rs ;
	int		svclen = 0 ;
	if ((rs = svcfile_magic(op,curp,ep,ebuf)) >= 0) {
	    rs = SR_FAULT ;
	    if (curp->ecp) {
	        rs = SR_OVERFLOW ;
	        if (elen > 0) {
	            hdb_dat	key ;
	            hdb_dat	val ;
	            hdb_cur	*ecp = curp->ecp ;
	            if ((rs = hdb_curenum(op->elp,ecp,&key,&val)) >= 0) {
	                SF_IE	*iep = (SF_IE *) val.buf ;
	                if ((ep != nullptr) && (ebuf != nullptr)) {
	                    rs = entry_load(ep,ebuf,elen,iep) ;
	                    svclen = rs ;
	                } else {
	                    svclen = lenstr(iep->svc) ;
	                }
	            } /* end if (had an entry) */
	        } /* end if (valid) */
	    } /* end if (non-null) */
	} /* end if (magic) */
	return (rs >= 0) ? svclen : rs ;
}
/* end subroutine (svcfile_enum) */

int svcfile_curfetch(svcfile *op,cc *svcname,svcfile_cur *curp,svcfile_ent *ep,
		char *ebuf,int elen) noex {
	int		rs ;
	int		svclen = 0 ;
	if ((rs = svcfile_magic(op,svcname)) >= 0) {
	    /* check for update */
	    if (op->ncursors == 0) {
	        rs = svcfile_check(op,0L) ;
	    }
	    if (rs >= 0) {
	        hdb_dat		key ;
	        hdb_dat		val ;
	        hdb_cur		cur ;
	        hdb_cur		*ecp{} ;
	        if (curp == nullptr) {
	            if ((rs = hdb_curbegin(op->elp,&cur)) >= 0) {
			ecp = &cur ;
		    }
	        } else {
		    if (curp->ecp) {
	                ecp = curp->ecp ;
		    } else {
			rs = SR_FAULT ;
		    }
	        } /* end if */
	        if (rs >= 0) {
	            key.buf = svcname ;
	            key.len = lenstr(svcname) ;
	            if ((rs = hdb_fetch(op->elp,key,ecp,&val)) >= 0) {
	                SF_IE	*iep = (SF_IE *) val.buf ;
	                if ((ep != nullptr) && (ebuf != nullptr)) {
	                    rs = entry_load(ep,ebuf,elen,iep) ;
	                    svclen = rs ;
	                } else {
	                    svclen = lenstr(iep->svc) ;
	                }
	            } /* end if (had an entry) */
	            if (curp == nullptr) {
	                hdb_curend(op->elp,&cur) ;
		    }
	        } /* end if (ok) */
	    } /* end if (ok) */
	} /* end if (magic) */
	return (rs >= 0) ? svclen : rs ;
}
/* end subroutine (svcfile_fetch) */

int svcfile_check(svcfile *op,time_t dt) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = svcfile_magic(op)) >= 0) {
	    cint	to = SVCFILE_INTCHECK ;
	    if (dt == 0) dt = getustime ;
            /* should we even check? */
	    if ((op->ncursors == 0) && ((dt - op->checktime) >= to)) {
	        op->checktime = dt ;
	        rs = svcfile_checkfiles(op,dt) ;
	        c = rs ;
	    } /* end if */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (svcfile_check) */

int svcfile_match(svcfile *op,cchar *name) noex {
	int		rs ;
	if ((rs = svcfile_magic(op)) >= 0) {
	    rs = svcfile_fetch(op,name,nullptr,nullptr,nullptr,0) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (svcfile_match) */


/* private subroutines */

static int svcfile_filefins(svcfile *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	void		*vp{} ;
	for (int i = 0 ; vecobj_get(op->flp,i,&vp) >= 0 ; i += 1) {
	    SVCFILE_FILE	*fep = (SVCFILE_FILE *) vp ;
	    if (vp) {
		{
	            rs1 = file_finish(fep) ;
	            if (rs >= 0) rs = rs1 ;
		}
	    }
	} /* end for */
	return rs ;
}
/* end subroutine (svcfile_filefins) */

/* check if the access table files have changed */
/* ARGSUSED */
static int svcfile_checkfiles(svcfile *op,time_t dt) noex {
	ustat		sb ;
	cint		wt = SVCFILE_INTWAIT ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		c_changed = 0 ;
	(void) dt ;
	/* check the files */
	void		*vp{} ;
	for (int i = 0 ; vecobj_get(op->flp,i,&vp) >= 0 ; i += 1) {
	    SVCFILE_FILE	*fep = (SVCFILE_FILE *) vp ;
	    if (vp) {
	        rs1 = uc_stat(fep->fname,&sb) ;
	        if ((rs1 >= 0) && ((sb.st_mtime - fep->timod) >= wt)) {
	            c_changed += 1 ;
	            svcfile_filedump(op,i) ;
	            rs = svcfile_fileparse(op,i) ;
	        } /* end if (changed) */
	    }
	    if (rs < 0) break ;
	} /* end for */
	return (rs >= 0) ? c_changed : rs ;
}
/* end subroutine (svcfile_checkfiles) */

static int svcfile_fileparse(svcfile *op,int fi) noex {
	int		rs ;
	int		c = 0 ;
	if (void *vp ; (rs = vecobj_get(op->flp,fi,&vp)) >= 0) {
	    SVCFILE_FILE	*fep = (SVCFILE_FILE *) vp ;
	    if (vp) {
	        cchar	*fname = fep->fname ;
	        if (ustat sb ; (rs = uc_stat(fname,&sb)) >= 0) {
	            if (sb.st_mtime > fep->timod) {
	                fep->dev = sb.st_dev ;
	                fep->ino = sb.st_ino ;
	                fep->timod = sb.st_mtime ;
	                fep->fsize = intsat(sb.st_size) ;
	                rs = svcfile_fileparser(op,fi,fname) ;
	                c = rs ;
	            } /* end if (need new parsing) */
	        } /* end if (stat) */
	    } else {
	        rs = SR_NOTFOUND ;
	    }
	} /* end if (vecstr_get) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (svcfile_fileparse) */

namespace {
    struct fileparser {
	svcfile		*op ;
	cchar		*fname ;
	cchar		*svcp = nullptr ;
	char		*a = nullptr ;
	char		*lbuf ;
	char		*abuf ;
	char		*sbuf ;
	int		fi ;
	int		llen ;
	int		alen ;
	int		slen ;
	int		svcl = 0 ;
	bool		f_ent = false ;
	fileparser(svcfile *p,int idx) noex : op(p), fi(idx) { } ;
        int operator () (cchar *) noex ;
	int allocbegin() noex ;
	int allocend() noex ;
	int parsef(cchar *) noex ;
	int parseln(svcentry *,cchar *,int) noex ;
    } ; /* end struct (fileparser) */
}

static int svcfile_fileparser(svcfile *op,int fi,cchar *fname) noex {
	fileparser	fo(op,fi) ;
	return fo(fname) ;
}

int fileparser::allocbegin() noex {
	int		sz = 0 ;
	int		rs ;
	{
	    llen = var.maxlinelen ;
	    sz += (llen + 1) ;
	}
	{
	    alen = (3 * var.maxhostlen) ;
	    sz += (alen + 1) ;
	}
	{
	    slen = var.maxnamelen ;
	    sz += (alen + 1) ;
	}
	if ((rs = uc_malloc(sz,&a)) >= 0) {
	    lbuf = a ;
	    abuf = (a + (llen + 1)) ;
	    sbuf = (a + (llen + 1) + (alen + 1)) ;
	    sbuf[0] = '\0' ;
	} /* end if (memory-allocation) */
	return rs ;
}

int fileparser::allocend() noex {
	int		rs = SR_BUGCHECK ;
	int		rs1 ;
	if (a) {
	    rs = SR_OK ;
	    {
	        rs1 = uc_free(a) ;
	        if (rs >= 0) rs = rs1 ;
	        a = nullptr ;
	        lbuf = nullptr ;
	        abuf = nullptr ;
	        sbuf = nullptr ;
	        llen = 0 ;
	        alen = 0 ;
	        slen = 0 ;
	    }
	} /* end if (non-null) */
	return rs ;
}

int fileparser::operator () (cchar *fn) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = allocbegin()) >= 0) {
	    {
	        rs = parsef(fn) ;
		c = rs ;
	    }
	    rs1 = allocend() ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (alloc) */
	return (rs >= 0) ? c : rs ;
}

int fileparser::parsef(cchar *fn) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (bfile sf ; (rs = sf.open(fn,"r",0)) >= 0) {
	    svcentry	se{} ;
	    while ((rs = sf.readln(lbuf,llen)) > 0) {
	 	cchar	*cp{} ;
		if (int cl ; (cl = sfcontent(lbuf,rs,&cp)) > 0) {
		    rs = parseln(&se,cp,cl) ;
		    c += rs ;
		} /* end if (sfcontent) */
	        if (rs < 0) break ;
	    } /* end while (reading extended lines) */
	    if (f_ent) {
	        if (rs >= 0) {
	            c += 1 ;
	            rs = svcfile_addentry(op,fi,&se) ;
	        }
	        f_ent = false ;
	        rs1 = svcentry_finish(&se) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (extra entry) */
	    rs1 = sf.close ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (bfile) */
	if (rs < 0) {
	    svcfile_filedump(op,fi) ;
	}
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (fileparser::parsef) */

int fileparser::parseln(svcentry *sep,cchar *cp,int cl) noex {
	field		fsb ;
	int		rs ;
	int		rs1 ;
	int		ce = 0 ;	/* count-entries */
	if ((rs = fsb.start(cp,cl)) >= 0) {
	    int		cf = 0 ;	/* count-fields */
	    int		ck = 0 ;	/* count-keys */
	    cchar	*fp{} ;
	    while ((rs = fsb.get(fterms,&fp)) >= 0) {
		int	fl = rs ;
		int	al = 0 ;
	        abuf[0] = '\0' ;
		if ((cf++ == 0) && (fsb.term == ':')) {
		    svcp = fp ;
		    svcl = fl ;
		} else if ((fl > 0) && (svcl > 0)) {
		    int		kl = fl ;
		    cchar	*kp = fp ;
		    /* create a SVCENTRY if found a first key */
	            if (ck++ == 0) {
	            	if (f_ent) {
			    if (rs >= 0) {
				ce += 1 ;
				rs = svcfile_addentry(op,fi,sep) ;
			    }
			    f_ent = false ;
			    svcentry_finish(sep) ;
			} /* end if */
			if (rs >= 0) {
			    rs = svcentry_start(sep,svcp,svcl) ;
			    f_ent = (rs >= 0) ;
			}
		    } /* end if (created SVCENTRY) */
	            if ((rs >= 0) && (fsb.term != ',')) {
	                rs = fsb.srvarg(saterms,abuf,alen) ;
			al = rs ;
	            }
	            if ((rs >= 0) && f_ent) { /* zero-value is allowed! */
	                rs = svcentry_addkey(sep,kp,kl,abuf,al) ;
	            }
		} /* end if (handling record) */
		if (fsb.term == '#') break ;
		if (rs < 0) break ;
	    } /* end while (fields) */
	    rs1 = fsb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (field) */
	return (rs >= 0) ? ce : rs ;
}
/* end method (fileparser::parseln) */

#if	CF_DEVINO
static int svcfile_filealready(svcfile *op,dev_t dev,ino_t ino) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		f = false ;
	void		*vp{} ;
	for (int i = 0 ; (rs1 = vecobj_get(op->flp,i,&vp)) >= 0 ; i += 1) {
	    SVCFILE_FILE	*fep = (SVCFILE_FILE *) vp ;
	    if (vp) {
	        f = ((fep->dev == dev) && (fep->ino == ino)) ;
	    }
	    if (f) break ;
	} /* end for */
	if ((rs >= 0) && (rs1 != SR_NOTFOUND)) rs = rs1 ;
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (svcfile_filealready) */
#endif /* CF_DEVINO */

/* add an entry to the access entry list */
static int svcfile_addentry(svcfile *op,int fi,SVCENTRY *nep) noex {
	int		sz = szof(SF_IE) ;
	int		rs ;
	int		f_added = false ;

#if	CF_ALREADY
	if ((rs1 = svcfile_already(op,nep->svc)) == SR_NOTFOUND) {
#endif

	    f_added = true ;
	    void	*vp{} ;
	    if ((rs = uc_malloc(sz,&vp)) >= 0) {
		SF_IE	*iep = (SF_IE *) vp ;
	        cint	n = svcentry_nkeys(nep) ;
	        iep->fi = fi ;
	        sz = (n+1) * 2 * szof(char *) ;
	        if ((rs = uc_malloc(sz,&vp)) >= 0) {
	            iep->nkeys = n ;
	            iep->keyvals = keyvals_t(vp) ;
	            sz = svcentry_size(nep) ;
	            if (char *bp{} ; (rs = uc_malloc(sz,&bp)) >= 0) {
	                iep->sz = sz ;
	                if ((rs = ientry_loadstr(iep,bp,nep)) >= 0) {
	                    hdb_dat	key ;
	                    hdb_dat	val ;
	                    cint	sl = rs ;
	                    key.buf = iep->svc ;
	                    key.len = sl ;
	                    val.buf = iep ;
	                    val.len = szof(SF_IE) ;
	                    if ((rs = hdb_store(op->elp,key,val)) >= 0) {
	                        rs = svcfile_svcadd(op,iep->svc) ;
	                        if (rs < 0) {
	                            hdb_delkey(op->elp,key) ;
				}
	                    } /* end if (hdb_store) */
	                } /* end if (ientry_loadstr) */
	                if (rs < 0) {
	                    iep->svc = nullptr ;
	                    uc_free(bp) ;
	                }
	            } /* end if (memory-allocation) */
	            if (rs < 0) {
	                uc_free(iep->keyvals) ;
	                iep->keyvals = nullptr ;
	            }
	        } /* end if (memory-allocation) */
	        if (rs < 0) {
	            uc_free(iep) ;
		}
	    } /* end if (memory-allocation) */

#if	CF_ALREADY
	}
#endif

	return (rs >= 0) ? f_added : rs ;
}
/* end subroutine (svcfile_addentry) */

#if	CF_ALREADY
static int svcfile_already(svcfile *op,cchar *svcname) noex {
	hdb_dat		key ;
	int		rs ;
	{
	    key.buf = svcname ;
	    key.len = lenstr(svcname) ;
	    rs = hdb_fetch(op->elp,key,nullptr,nullptr) ;
	}
	return rs ;
}
/* end subroutine (svcfile_already) */
#endif /* CF_ALREADY */

/* free up all of the entries in this svcfile list associated w/ a file */
static int svcfile_filedump(svcfile *op,int fi) noex {
	hdb_cur		cur ;
	hdb_dat		key ;
	hdb_dat		val ;
	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	int		rs1 ;
	int		rs2 ;
	int		c = 0 ;
	if ((rs = hdb_curbegin(op->elp,&cur)) >= 0) {
	    SF_IE	*ep ;
	    while ((rs2 = hdb_curenum(op->elp,&cur,&key,&val)) >= 0) {
	        ep = (SF_IE *) val.buf ;
	        if ((ep->fi == fi) || (fi < 0)) {
	            c += 1 ;
		    {
	                rs1 = hdb_curdel(op->elp,&cur,0) ;
	                if (rs >= 0) rs = rs1 ;
		    }
		    {
	                rs1 = svcfile_svcdel(op,ep->svc) ;
	                if (rs >= 0) rs = rs1 ;
		    }
		    {
	                rs1 = ientry_finish(ep) ;
	                if (rs >= 0) rs = rs1 ;
		    }
		    {
	                rs1 = uc_free(ep) ;
	                if (rs >= 0) rs = rs1 ;
		    }
	        } /* end if (found matching entry) */
	    } /* end while (looping through entries) */
	    if ((rs >= 0) && (rs1 != rsn)) rs = rs2 ;
	    rs1 = hdb_curend(op->elp,&cur) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (cursor) */
	return rs ;
}
/* end subroutine (svcfile_filedump) */

#if	CF_FILEDEL
static int svcfile_filedel(svcfile *op,int fi) noex {
	int		rs ;
	int		rs1 ;
	void		*vp{} ;
	if ((rs = vecobj_get(op->flp,fi,&vp)) >= 0) {
	    SVCFILE_FILE	*fep = (SVCFILE_FILE *) vp ;
	    if (vp) {
		{
	            rs1 = file_finish(fep) ;
	            if (rs >= 0) rs = rs1 ;
		}
		{
	            rs1 = vecobj_del(op->flp,fi) ;
	            if (rs >= 0) rs = rs1 ;
		}
	    } /* end if */
	} /* end if (vecobj_get) */
	return rs ;
}
/* end subroutine (svcfile_filedel) */
#endif /* CF_FILEDEL */

static int svcfile_svcadd(svcfile *op,cchar *svc) noex {
	SVCFILE_SVCNAME	sn{} ;
	vecobj		*lp = op->slp ;
	int		rs ;
	int		f_added = false ;
	void		*vp{} ;
	sn.svcname = svc ;
	sn.count = 1 ;
	if ((rs = vecobj_search(lp,&sn,vcmpsvcname,&vp)) >= 0) {
	    SVCFILE_SVCNAME	*snp = (SVCFILE_SVCNAME *) vp ;
	    rs = svcname_incr(snp) ;
	} else if (rs == SR_NOTFOUND) {
	    f_added = true ;
	    if ((rs = svcname_start(&sn,svc)) >= 0) {
	        rs = vecobj_add(lp,&sn) ;
	        if (rs < 0) {
	            svcname_finish(&sn) ;
		}
	    } /* end if (memory-allocation) */
	} /* end if */
	return (rs >= 0) ? f_added : rs ;
}
/* end subroutine (svcfile_svcadd) */

static int svcfile_svcdel(svcfile *op,cchar *svc) noex {
	SVCFILE_SVCNAME	sn{} ;
	vecobj		*lp = op->slp ;
	int		rs ;
	int		rs1 ;
	int		si = 0 ;
	void		*vp{} ;
	sn.svcname = svc ;
	sn.count = 0 ;
	if ((rs = vecobj_search(lp,&sn,vcmpsvcname,&vp)) >= 0) {
	    SVCFILE_SVCNAME	*snp = (SVCFILE_SVCNAME *) vp ;
	    si = rs ;
	    {
	        rs1 = svcname_decr(snp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (rs1 == 0) {
		{
	            rs1 = svcname_finish(snp) ;
	            if (rs >= 0) rs = rs1 ;
		}
		{
	            rs1 = vecobj_del(lp,si) ;
	            if (rs >= 0) rs = rs1 ;
		}
	    } /* end if (count == 0) */
	} else if (rs == SR_NOTFOUND) {
	    rs = SR_OK ;
	}
	return (rs >= 0) ? si : rs ;
}
/* end subroutine (svcfile_svcdel) */

static int svcfile_svcfins(svcfile *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		c = 0 ;
	void		*vp{} ;
	for (int i = 0 ; vecobj_get(op->slp,i,&vp) >= 0 ; i += 1) {
	    SVCFILE_SVCNAME	*snp = (SVCFILE_SVCNAME *) vp ;
	    if (vp) {
	        c += 1 ;
	        rs1 = svcname_finish(snp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end for */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (svcfile_svcfins) */

static int file_start(SVCFILE_FILE *fep,cchar *fname) noex {
	int		rs = SR_FAULT ;
	if (fep && fname) {
	    memclear(fep) ;
	    if (cchar *cp ; (rs = uc_mallocstrw(fname,-1,&cp)) >= 0) {
	        fep->fname = cp ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (file_start) */

static int file_finish(SVCFILE_FILE *fep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (fep) {
	    rs = SR_OK ;
	    if (fep->fname) {
	        rs1 = uc_free(fep->fname) ;
	        if (rs >= 0) rs = rs1 ;
	        fep->fname = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (file_finish) */

static int svcname_start(SVCFILE_SVCNAME *snp,cchar *svc) noex {
	int		rs ;
	snp->count = 0 ;
	if (cchar *cp ; (rs = uc_mallocstrw(svc,-1,&cp)) >= 0) {
	    snp->count = 1 ;
	    snp->svcname = cp ;
	}
	return rs ;
}
/* end subroutine (svcname_start) */

static int svcname_finish(SVCFILE_SVCNAME *snp) noex {
	cint		c = snp->count ;
	int		rs = SR_OK ;
	int		rs1 ;
	snp->count = 0 ;
	if (snp->svcname != nullptr) {
	    rs1 = uc_free(snp->svcname) ;
	    if (rs >= 0) rs = rs1 ;
	    snp->svcname = nullptr ;
	}
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (svcname_finish) */

static int svcname_incr(SVCFILE_SVCNAME *snp) noex {
	cint	c = snp->count ;
	snp->count += 1 ;
	return c ;
}
/* end subroutine (svcname_incr) */

static int svcname_decr(SVCFILE_SVCNAME *snp) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (snp->count > 0) snp->count -= 1 ;
	if (snp->count == 0) {
	    if (snp->svcname != nullptr) {
	        rs1 = uc_free(snp->svcname) ;
	        if (rs >= 0) rs = rs1 ;
	        snp->svcname = nullptr ;
	    }
	} /* end if (count == 0) */
	return (rs >= 0) ? snp->count : rs ;
}
/* end subroutine (svcname_decr) */

static int svcentry_start(SVCENTRY *sep,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	if (sep) {
	    rs = memclear(sep) ;
	    if (cchar *cp{} ; (rs = uc_mallocstrw(sp,sl,&cp)) >= 0) {
	        cint	sz = szof(SVCENTRY_KEY) ;
		cint	vn = 5 ;
		cint	vo = VECOBJ_OORDERED ;
	        sep->svc = cp ;
	        rs = vecobj_start(&sep->keys,sz,vn,vo) ;
	        if (rs < 0) {
	            uc_free(cp) ;
	            sep->svc = nullptr ;
	        }
	    } /* end if (memory-allocation) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (svcentry_start) */

static int svcentry_finish(SVCENTRY *sep) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	void		*vp{} ;
	for (int i = 0 ; vecobj_get(&sep->keys,i,&vp) >= 0 ; i += 1) {
	    SVCENTRY_KEY	*kep = (SVCENTRY_KEY *) vp ;
	    if (vp) {
	        if (kep->kname) {
	            rs1 = uc_free(kep->kname) ;
	            if (rs >= 0) rs = rs1 ;
	            kep->kname = nullptr ;
	        }
	    }
	} /* end for */
	{
	    rs1 = vecobj_finish(&sep->keys) ;
	    if (rs >= 0) rs = rs1 ;
	}
	if (sep->svc) {
	    rs1 = uc_free(sep->svc) ;
	    if (rs >= 0) rs = rs1 ;
	    sep->svc = nullptr ;
	}
	return rs ;
}
/* end subroutine (svcentry_finish) */

static int svcentry_addkey(SVCENTRY *sep,cc *kp,int kl,cc *ap,int al) noex {
	int		rs ;
	int		sz = 0 ;
	if (kl < 0) kl = lenstr(kp) ;
	if (al < 0) al = lenstr(ap) ;
	sz += (kl+1) ;
	sz += (al+1) ;
	if (char *bp{} ; (rs = uc_malloc(sz,&bp)) >= 0) {
	    SVCENTRY_KEY	key ;
	    key.kl = kl ;
	    key.kname = bp ;
	    key.al = 0 ;
	    key.args = nullptr ;
	    bp = (strwcpy(bp,kp,kl) + 1) ;
	    if (al > 0) {
	        key.al = al ;
	        key.args = bp ;
	        bp = (strwcpy(bp,ap,al) + 1) ;
	    }
	    rs = vecobj_add(&sep->keys,&key) ;
	    if (rs < 0) {
	        uc_free(key.kname) ;
	    }
	} /* end if (memory-allocation) */
	return rs ;
}
/* end subroutine (svcentry_addkey) */

static int svcentry_nkeys(SVCENTRY *sep) noex {
	return vecobj_count(&sep->keys) ;
}
/* end subroutine (svcentry_nkeys) */

static int svcentry_size(SVCENTRY *sep) noex {
	int		sz = 0 ;
	sz += (lenstr(sep->svc) + 1) ;
	void		*vp{} ;
	for (int i = 0 ; vecobj_get(&sep->keys,i,&vp) >= 0 ; i += 1) {
	    SVCENTRY_KEY	*kep = (SVCENTRY_KEY *) vp ;
	    if (vp) {
	        sz += (kep->kl+1) ;
	        sz += (kep->al+1) ;
	    }
	} /* end for */
	return sz ;
}
/* end subroutine (svcentry_size) */

static int ientry_loadstr(SF_IE *iep,char *bp,SVCENTRY *nep) noex {
	int		rs = SR_FAULT ;
	int		sl = 0 ;
	if (iep && bp && nep) {
	int		j = 0 ; /* used-afterwards */
	sl = lenstr(nep->svc) ;
	iep->svc = bp ;
	bp = (strwcpy(bp,nep->svc,sl)+1) ;
	void	*vp{} ;
	for (int i = 0 ; vecobj_get(&nep->keys,i,&vp) >= 0 ; i += 1) {
	    SVCENTRY_KEY	*kep = (SVCENTRY_KEY *) vp ;
	    if (vp) {
	        iep->keyvals[j][0] = nullptr ;
	        iep->keyvals[j][1] = nullptr ;
	        if (kep->kname != nullptr) {
	            iep->keyvals[j][0] = bp ;
	            bp = (strwcpy(bp,kep->kname,-1)+1) ;
	        }
	        if (kep->args != nullptr) {
	            iep->keyvals[j][1] = bp ;
	            bp = (strwcpy(bp,kep->args,-1)+1) ;
	        }
	        j += 1 ;
	    }
	} /* end for */
	iep->keyvals[j][0] = nullptr ;
	iep->keyvals[j][1] = nullptr ;
	} /* end if (non-null) */
	return (rs >= 0) ? sl : rs ;
}
/* end subroutine (ientry_loadstr) */

static int ientry_finish(SF_IE *iep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (iep) {
	    rs = SR_OK ;
	    if (iep->keyvals) {
	        rs1 = uc_free(iep->keyvals) ;
	        if (rs >= 0) rs = rs1 ;
	        iep->keyvals = nullptr ;
	    }
	    if (iep->svc) {
	        rs1 = uc_free(iep->svc) ;
	        if (rs >= 0) rs = rs1 ;
	        iep->svc = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ientry_finish) */

#if	CF_MOREKEYS
static int ientry_morekeys(SF_IE *iep,int c,int i) noex {
	int		rs = SR_FAULT ;
	int		f_more = true ;
	if (iep) {
	    if (c >= 0) {
	        f_more = (i < c) ;
	    }
	    f_more = f_more && (iep->keyvals[i][0] != nullptr) ;
	} /* end if (non-null) */
	return (rs >= 0) ? f_more : rs ;
}
/* end subroutine (ientry_morekeys) */
#endif /* CF_MOREKEYS */

/* load up the user-interface entry from the internal structure */
static int entry_load(svcfile_ent *ep,char *ebuf,int elen,SF_IE *iep) noex {
	int		rs = SR_FAULT ;
	int		rlen = 0 ;
	if (iep && ebuf && iep) {
	    rs = SR_OVERFLOW ;
	    if (elen > 0) {
		cint	bo = SVCFILE_BO((ulong) ebuf) ;
		char	*bp ;
		if (iep->sz <= (elen - bo)) {
		    cint	kal = (iep->nkeys + 1) * 2 * szof(char *) ;
		    int		i ; /* used-afterwards */
		    cchar	*(*keyvals)[2] = keyvals_t(ebuf + bo) ;
		    bp = charp(ebuf + bo + kal) ;
#ifdef	COMMENT
	    	    bl = (elen - bo - kal) ;
#endif
	    	    ep->svc = bp ;
	    	    bp = strwcpy(bp,iep->svc,-1) + 1 ;
	    	    rlen = intconv(bp - ep->svc - 1) ;
	    	    for (i = 0 ; i < iep->nkeys ; i += 1) {
	        	cchar	*kp = iep->keyvals[i][0] ;
	        	cchar	*vp = iep->keyvals[i][1] ;
	        	keyvals[i][0] = bp ;
	        	bp = strwcpy(bp,kp,-1) + 1 ;
	        	if (vp != nullptr) {
	            	    keyvals[i][1] = bp ;
	            	    bp = strwcpy(bp,vp,-1) + 1 ;
	        	} else {
	            	    keyvals[i][1] = nullptr ;
			}
	    	    } /* end for */
	    	    keyvals[i][0] = nullptr ;
	    	    keyvals[i][1] = nullptr ;
	    	    ep->keyvals = keyvals ;
	    	    ep->fi = iep->fi ;
	    	    ep->nkeys = iep->nkeys ;
	    	    ep->sz = iep->sz ;
	    	    rs = SR_OK ;
		} /* end if (not overflow) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rlen : rs ;
}
/* end subroutine (entry_load) */

static int mkvars() noex {
	int		rs ;
	if ((rs = getbufsize(getbufsize_mn)) >= 0) {
	    var.maxnamelen = rs ;
	    if ((rs = getbufsize(getbufsize_ml)) >= 0) {
		var.maxlinelen = rs ;
	        if ((rs = getbufsize(getbufsize_hn)) >= 0) {
		    var.maxhostlen = rs ;
		}
	    }
	}
	return rs ;
}
/* end subroutine (mkvars) */

static int vcmpfname(cvoid **v1pp,cvoid **v2pp) noex {
	SVCFILE_FILE	*e1p = (SVCFILE_FILE *) *v1pp ;
	SVCFILE_FILE	*e2p = (SVCFILE_FILE *) *v2pp ;
	int		rc = 0 ;
	if (e1p || e2p) {
	    rc = +1 ;
	    if (e1p) {
		rc = -1 ;
	        if (e2p) {
	            rc = strcmp(e1p->fname,e2p->fname) ;
	        }
	    }
	}
	return rc ;
}
/* end subroutine (vcmpfname) */

static int vcmpsvcname(cvoid **v1pp, cvoid **v2pp) noex {
	SVCFILE_SVCNAME	*e1p = (SVCFILE_SVCNAME *) *v1pp ;
	SVCFILE_SVCNAME	*e2p = (SVCFILE_SVCNAME *) *v2pp ;
	int		rc = 0 ;
	if (e1p || e2p) {
	    rc = +1 ;
	    if (e1p) {
		rc = -1 ;
	        if (e2p) {
		    bool	f1 = (e1p->svcname == nullptr) ;
		    bool	f2 = (e2p->svcname == nullptr) ;
		    if (f1 || f2) {
	    	        if (! (f1 && f2)) {
	        	    rc = (f1) ? 1 : -1 ;
	    	        }
		    }
		    if ((rc == 0) && (! f1) && (! f2)) {
	    	        rc = strcmp(e1p->svcname,e2p->svcname) ;
	            }
	        }
	    }
	}
	return rc ;
}
/* end subroutine (vcmpsvcname) */


