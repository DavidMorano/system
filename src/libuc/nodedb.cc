/* nodedb SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* magement for the NODE-DB file */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-05-25, David A­D­ Morano
	This subroutine was adopted for use as the node-database
	reader.

	= 2013-04-20, David A­D­ Morano
	I was totally on drugs when I originally wrote this.  It was
	incredibly complicated.  It allowed for service entries to
	span multiple lines (like a service-entry file).  I must
	have been popping acid at the time.  This
	service-entry-spanning-muliple lines business was never
	used, so I got rid of it.  It all has to be and look so
	simple so that no errors are really possible.  This is often
	easier said than done, but when some complicated behavior
	is not needed (at all), then we get rid of it in favor of
	much simpler implementations.

*/

/* Copyright © 2004,2013 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	nodedb

	Description:
	This object manages access to one or more NODE-DB files.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>		/* |u_stat(3u)| */
#include	<climits>		/* |UCHAR_MAX| + |CHAR_BIT| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strcmp(3c)| + |lenstr(3c)| */
#include	<ctime>
#include	<netdb.h>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<mallocxx.h>
#include	<bfile.h>
#include	<field.h>
#include	<fieldterms.h>
#include	<vecobj.h>
#include	<hdb.h>
#include	<mkchar.h>
#include	<strwcpy.h>
#include	<mkpathx.h>
#include	<sfx.h>
#include	<mkpathrooted.h>
#include	<char.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"nodedb.h"

import libutil ;

/* local defines */

#define	ND_INTCHECK	60	/* file check interval (seconds) */
#define	ND		nodedb
#define	ND_C		nodedb_cur
#define	ND_E		nodedb_ent
#define	ND_F		nodedb_file
#define	ND_K		nodedb_keyname
#define	ND_IE		nodedb_ie

#define	SE		svcentry
#define	SE_KEY		svcentry_key

#define	NODEDB_KA	szof(char *(*)[2])
#define	NODEDB_BO(v)	\
	((NODEDB_KA - ((v) % NODEDB_KA)) % NODEDB_KA)

#define	ARGSMULT	4		/* args-buffer mulitplier */
#define	LINEBUFMULT	4		/* line-buffer multiplier */

#define	KEYALIGNMENT	szof(char *(*)[2])


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef cchar		*(*keytabp)[2] ;


/* external subroutines */


/* external variables */


/* local structures */

struct nodedb_file {
	cchar		*fname ;
	time_t		timod ;
	size_t		fsize ;
	ino_t		ino ;
	dev_t		dev ;
} ;

struct nodedb_keyname {
	cchar		*kname ;
	int		count ;
} ;

struct nodedb_ie {
	cchar		*(*keys)[2] ;
	cchar		*svc ;
	cchar		*clu ;
	cchar		*sys ;
	cchar		*a ;			/* allocation */
	int		nkeys ;			/* number of keys */
	int		tsize ;			/* total size */
	int		fi ;			/* file index */
} ;

struct lineinfo_field {
	cchar		*fp ;
	int		fl ;
} ;

struct lineinfo {
	lineinfo_field	f[3] ;
} ;

struct svcentry {
	vecobj		keys ;
	cchar		*svc ;
	cchar		*clu ;
	cchar		*sys ;
	cchar		*a ;
} ;

struct svcentry_key {
	cchar		*kname ;
	cchar		*args ;
	int		kl ;
	int		al ;
} ;

namespace {
    struct vars {
	int		maxhostlen ;
	int		lineslen ;
	int		entbuflen ;
	operator int () noex ;
    } ; /* end struct (vars) */
}


/* forward references */

static int	mkinit() noex ;
static int	mkterms() noex ;

template<typename ... Args>
static inline int nodedb_ctor(nodedb *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    op->checktime = 0 ;
	    op->magic = 0 ;
	    op->cursors = 0 ;
	    if ((op->filep = new(nothrow) vecobj) != np) {
	        if ((op->entsp = new(nothrow) hdb) != np) {
		    rs = SR_OK ;
		} /* end if (new-vecobj) */
		if (rs < 0) {
		    delete op->filep ;
		    op->filep = nullptr ;
		}
	    } /* end if (new-vecobj) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (nodedb_ctor) */

static int nodedb_dtor(nodedb *op) noex {
	int		rs = SR_OK ;
	if (op->entsp) {
	    delete op->entsp ;
	    op->entsp = nullptr ;
	}
	if (op->filep) {
	    delete op->filep ;
	    op->filep = nullptr ;
	}
	return rs ;
}
/* end subroutine (nodedb_dtor) */

template<typename ... Args>
static inline int nodedb_magic(nodedb *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magic == NODEDB_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (nodedb_magic) */

static int	ND_Fileadder(nodedb *op,cchar *fname) noex ;
static int	ND_Filefins(nodedb *) noex ;
static int	nodedb_entfins(nodedb *) noex ;
static int	ND_Fileparse(nodedb *,int) noex ;
static int	ND_Fileparser(nodedb *,ND_F *,int) noex ;
static int	ND_Fileparseln(nodedb *,int,cchar *,int) noex ;
static int	ND_Filedump(nodedb *,int) noex ;
static int	ND_Filedel(nodedb *,int) noex ;
static int	nodedb_addentry(nodedb *,int,SE *) noex ;
static int	nodedb_checkfiles(nodedb *,time_t) noex ;

static int	file_start(ND_F *,cchar *) noex ;
static int	file_finish(ND_F *) noex ;

static int	ientry_start(ND_IE *,int,SE *) noex ;
static int	ientry_finish(ND_IE *) noex ;

static int	svcentry_start(SE *,lineinfo *) noex ;
static int	svcentry_addkey(SE *,cchar *,int,cchar *,int) noex ;
static int	svcentry_finish(SE *) noex ;

static int	entry_load(NODEDB_ENT *,char *,int,ND_IE *) noex ;

extern "C" {
    static int	vcmpfn(cvoid **,cvoid **) noex ;
}

static int	freeit(cchar **) noex ;


/* local variables */

/* all white-space, pound ('#'), colon (':'), equal ('='), and comma (',') */
static char		fterms[fieldterms_termsize] ;

/* argument field terminators (pound '#' and comma ',') */
static char 		saterms[fieldterms_termsize] ;

static vars		var ;


/* exported variables */


/* exported subroutines */

int nodedb_open(nodedb *op,cchar *fname) noex {
	int		rs ;
	if ((rs = nodedb_ctor(op,fname)) >= 0) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
		static cint	rsi = mkinit() ;
		if ((rs = rsi) >= 0) {
		    vecobj	*flp = op->filep ;
	            cint	vsz = szof(ND_F) ;
		    cint	vn = NODEDB_NFILES ;
	            cint	vo = VECOBJ_OREUSE ;
	            if ((rs = flp->start(vsz,vn,vo)) >= 0) {
	                cnullptr	np{} ;
		        hdb		*elp = op->entsp ;
			cint		hn = NODEDB_DEFENTS ;
	                if ((rs = hdb_start(elp,hn,0,np,np)) >= 0) {
		    	    op->entbuflen = var.entbuflen ;
	                    op->checktime = getustime ;
	                    op->magic = NODEDB_MAGIC ;
	                    if (fname && fname[0]) {
	                        rs = nodedb_fileadd(op,fname) ;
	                        if (rs < 0) {
	                            op->magic = 0 ;
	                        }
	                    } /* end if (had an optional file) */
	                    if (rs < 0) {
	                        hdb_finish(op->entsp) ;
			    }
	                } /* end if (entries) */
	                if (rs < 0) {
	                    vecobj_finish(op->filep) ;
			}
	            } /* end if (files) */
		} /* end if (mkinit) */
	    } /* end if (valid) */
	    if (rs < 0) {
		nodedb_dtor(op) ;
	    }
	} /* end if (nodedb_ctor) */
	return rs ;
}
/* end subroutine (nodedb_open) */

int nodedb_close(nodedb *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = nodedb_magic(op)) >= 0) ylikely {
	    {
	        rs1 = nodedb_entfins(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = ND_Filefins(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->entsp) {
	        rs1 = hdb_finish(op->entsp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->filep) {
	        rs1 = vecobj_finish(op->filep) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = nodedb_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (nodedb_close) */

int ND_Fileadd(nodedb *op,cchar *fname) noex {
	int		rs ;
	int		rs1 ;
	int		rc = 0 ;
	if ((rs = nodedb_magic(op,fname)) >= 0) ylikely {
	    rs = SR_INVALID ;
	    if (fname[0]) ylikely {
		if (char *tbuf{} ; (rs = malloc_mp(&tbuf)) >= 0) ylikely {
		    if ((rs = mkpathrooted(tbuf,fname)) >= 0) ylikely {
		        rs = ND_Fileadder(op,tbuf) ;
		        rc = rs ;
	            } /* end if (mkpathrooted) */
		    rs1 = uc_free(tbuf) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
	    } else {
		rs = ND_Fileadder(op,fname) ;
		rc = rs ;
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (ND_Fileadd) */

int nodedb_curbegin(nodedb *op,nodedb_cur *curp) noex {
	int		rs ;
	if ((rs = nodedb_magic(op,curp)) >= 0) ylikely {
	    rs = SR_NOMEM ;
	    curp->i = -1 ;
	    if ((curp->ecp = new(nothrow) hdb_cur) != nullptr) {
	        if ((rs = hdb_curbegin(op->entsp,curp->ecp)) >= 0) {
	            op->cursors += 1 ;
	        }
		if (rs < 0) {
		    delete curp->ecp ;
		    curp->ecp = nullptr ;
		}
	    } /* end if (new-hdb_cur) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (nodedb_curbegin) */

int nodedb_curend(nodedb *op,nodedb_cur *curp) noex {
	int		rs ;
	if ((rs = nodedb_magic(op,curp)) >= 0) ylikely {
	    curp->i = -1 ;
	    if ((rs = hdb_curend(op->entsp,curp->ecp)) >= 0) ylikely {
		delete curp->ecp ;
		curp->ecp = nullptr ;
	        op->cursors -= 1 ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (nodedb_curend) */

int nodedb_enum(ND *op,ND_C *curp,ND_E *ep,char *ebuf,int elen) noex {
	int		rs ;
	int		svclen = 0 ;
	if ((rs = nodedb_magic(op,curp)) >= 0) ylikely {
	    hdb_dat	key{} ;
	    hdb_dat	val{} ;
	    hdb_cur	*ecp = curp->ecp ;
	    if ((rs = hdb_curenum(op->entsp,ecp,&key,&val)) >= 0) ylikely {
	        ND_IE	*iep = (ND_IE *) val.buf ;
	        if (ep && ebuf) {
	            rs = entry_load(ep,ebuf,elen,iep) ;
	            svclen = rs ;
	        } else {
	            svclen = lenstr(iep->svc) ;
	        }
	    } /* end if (had an entry) */
	} /* end if (magic) */
	return (rs >= 0) ? svclen : rs ;
}
/* end subroutine (nodedb_enum) */

int ND_Fetch(ND *op,cc *svcbuf,ND_C *curp,
		ND_E *ep,char *ebuf,int elen) noex {
	int		rs ;
	int		svclen = 0 ;
	if ((rs = nodedb_magic(op,curp)) >= 0) ylikely {
	    hdb_dat	key ;
	    hdb_dat	val{} ;
	    hdb_cur	*ecp = curp->ecp ;
	    key.buf = svcbuf ;
	    key.len = lenstr(svcbuf) ;
	    if ((rs = hdb_fetch(op->entsp,key,ecp,&val)) >= 0) {
	        ND_IE	*iep = (ND_IE *) val.buf ;
	        if (ep && ebuf) {
	            rs = entry_load(ep,ebuf,elen,iep) ;
	            svclen = rs ;
	        } else {
	            svclen = lenstr(iep->svc) ;
	        }
	    } /* end if (hdb_fetch) */
	} /* end if (magic) */
	return (rs >= 0) ? svclen : rs ;
}
/* end subroutine (ND_Fetch) */

int nodedb_check(nodedb *op) noex {
	int		rs ;
	if ((rs = nodedb_magic(op)) >= 0) ylikely {
	    if (op->cursors == 0) {
	        custime		dt = getustime ;
	        if ((dt - op->checktime) > ND_INTCHECK) {
	            rs = nodedb_checkfiles(op,dt) ;
	        }
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (nodedb_check) */


/* private subroutines */

static int ND_Fileadder(NODEDB *op,cchar *fname) noex {
	int		rs ;
	if (ND_F fe{} ; (rs = file_start(&fe,fname)) >= 0) ylikely {
	    vecobj	*flp = op->filep ;
	    vecobj_vcf	vcf = vecobj_vcf(vcmpfn) ;
	    cint	rsn = SR_NOTFOUND ;
	    if ((rs = flp->search(&fe,vcf,nullptr)) == rsn) {
	        if ((rs = flp->add(&fe)) >= 0) {
	            cint	fi = rs ;
	            rs = ND_Fileparse(op,fi) ;
	            if (rs < 0) {
	                ND_Filedel(op,fi) ;
	            }
	        } /* end if (vecobj_add) */
	    } /* end if (vecobj_search) */
	    if (rs < 0) {
	        file_finish(&fe) ;
	    }
	} /* end if (file-start) */
	return rs ;
}
/* end subroutine (ND_Fileadder) */

static int ND_Filefins(nodedb *op) noex {
	vecobj		*flp = op->filep ;
	int		rs = SR_OK ;
	int		rs1 ;
	void		*vp{} ;
	for (int i = 0 ; flp->get(i,&vp) >= 0 ; i += 1) {
	    ND_F	*fep = (ND_F *) vp ;
	    if (vp) {
	        rs1 = file_finish(fep) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end for */
	return rs ;
}
/* end subroutine (ND_Filefins) */

static int nodedb_checkfiles(nodedb *op,time_t daytime) noex {
	vecobj		*flp = op->filep ;
	int		rs = SR_OK ;
	int		c_changed = 0 ;
	void		*vp{} ;
	for (int i = 0 ; flp->get(i,&vp) >= 0 ; i += 1) {
	    if (vp) {
		ND_F	*fep = (ND_F *) vp ;
		if (USTAT sb ; (rs = u_stat(fep->fname,&sb)) >= 0) {
	            if (sb.st_mtime > fep->timod) {
	                c_changed += 1 ;
	                ND_Filedump(op,i) ;
	                rs = ND_Fileparse(op,i) ;
		    }
		} else if (isNotPresent(rs)) {
		    rs = SR_OK ;
	        } /* end if (u_stat) */
	    }
	    if (rs < 0) break ;
	} /* end for (looping through files) */
	op->checktime = daytime ;
	return (rs >= 0) ? c_changed : rs ;
}
/* end subroutine (nodedb_checkfiles) */

static int ND_Fileparse(nodedb *op,int fi) noex {
	vecobj		*flp = op->filep ;
	int		rs ;
	int		c = 0 ;
	if (void *vp{} ; (rs = flp->get(fi,&vp)) >= 0) {
	    rs = SR_NOTFOUND ;
	    if (vp) {
		ND_F	*fep = (ND_F *) vp ;
	        rs = ND_Fileparser(op,fep,fi) ;
		c = rs ;
	        if (rs < 0) {
	            ND_Filedump(op,fi) ;
	        }
	    } /* end if (non-null) */
	} /* end if (vector_get) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (ND_Fileparse) */

static int ND_Fileparser(nodedb *op,ND_F *fep,int fi) noex {
    	cint		lsz = var.lineslen ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (char *lbuf{} ; (rs = uc_malloc(lsz,&lbuf)) >= 0) {
	    bfile	loadfile, *lfp = &loadfile ;
	    cint	llen = rs ;
	    if ((rs = bopen(lfp,fep->fname,"r",0664)) >= 0) {
	        bool	f_bol = true ;
	        bool	f_eol ;
	        if (USTAT sb ; (rs = bcontrol(lfp,BC_STAT,&sb)) >= 0) {
	            if (S_ISREG(sb.st_mode)) {
	                if (fep->timod < sb.st_mtime) {
			    cnullptr	np{} ;
	                    fep->dev = sb.st_dev ;
	                    fep->ino = sb.st_ino ;
	                    fep->timod = sb.st_mtime ;
	                    fep->fsize = size_t(sb.st_size) ;
	                    while ((rs = breadlns(lfp,lbuf,llen,-1,np)) > 0) {
	                        int	len = rs ;
	                        int	cl ;
	                        cchar	*cp ;
	                        f_eol = (lbuf[len - 1] == '\n') ;
	                        if (f_eol) len -= 1 ;
	                        lbuf[len] = '\0' ;
	                        if ((cl = sfskipwhite(lbuf,len,&cp)) > 0) {
	                            if (f_bol && (*cp != '#')) {
	                                rs = ND_Fileparseln(op,fi,cp,cl) ;
	                                if (rs > 0) c += 1 ;
	                            }
	                        }
	                        f_bol = f_eol ;
	                        if (rs < 0) break ;
	                    } /* end while (reading extended lines) */
	                } /* end if (needed) */
	            } else {
	                rs = SR_ISDIR ;
	            }
	        } /* end if (bcontrol) */
	        rs1 = bclose(lfp) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (bfile) */
	    rs1 = uc_free(lbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (ND_Fileparser) */

static int ND_Fileparseln(nodedb *op,int fi,cchar *lp,int ll) noex {
    	cint		alen = (var.maxhostlen * ARGSMULT) ;
	int		rs ;
	int		rs1 ;
	int		f_ent = false ;
	if (char *abuf{} ; (rs = uc_malloc((alen + 1),&abuf)) >= 0) {
	    SE		se ; /* used multiple blocks below */
	    lineinfo	li{} ;
	    int		c_field = 0 ;
	    if (field fsb ; (rs = fsb.start(lp,ll)) >= 0) {
	        cchar	*fp{} ;
	        for (int fl ; (fl = fsb.get(fterms,&fp)) >= 0 ; ) {
	            if ((c_field == 0) && (fl == 0)) break ;
	            if ((c_field < 3) && (fsb.term == ':')) {
	                li.f[c_field].fp = fp ;
	                li.f[c_field].fl = fl ;
	            }
	            if ((c_field >= 3) && (fl > 0)) {
	                cchar	*kp = fp ;
	                int	kl = fl ;
	                int	al ;
	                if (! f_ent) {
	                    rs = svcentry_start(&se,&li) ;
	                    f_ent = (rs >= 0) ;
	                    if (rs < 0) break ;
	                }
	                abuf[0] = '\0' ;
	                al = 0 ;
	                if (fsb.term == '=') {
	                    al = fsb.srvarg(saterms,abuf,alen) ;
	                }
	                if ((rs >= 0) && f_ent) {
	                    rs = svcentry_addkey(&se,kp,kl,abuf,al) ;
	                }
	            } /* end if (handling key-value pair) */
	            c_field += 1 ;
	            if (fsb.term == '#') break ;
	            if (rs < 0) break ;
	        } /* end for (fields) */
	        rs1 = fsb.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (field) */
	    if (rs >= 0) {
	        if ((! f_ent) && (c_field > 0) && (li.f[0].fl > 0)) {
	            rs = svcentry_start(&se,&li) ;
	            f_ent = (rs >= 0) ;
	        }
	    }
	    if ((rs >= 0) && f_ent) {
	        rs = nodedb_addentry(op,fi,&se) ;
	        svcentry_finish(&se) ;
	    } /* end if (adding previous entry) */
	    rs = rsfree(rs,abuf) ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? f_ent : rs ;
}
/* end subroutine (ND_Fileparseln) */

static int nodedb_addentry(nodedb *op,int fi,SE *sep) noex {
	int		rs = SR_FAULT ;
	if (sep) {
	    cint	sz = szof(ND_IE) ;
	    if (void *vp ; (rs = uc_malloc(sz,&vp)) >= 0) {
	        ND_IE	*iep = (ND_IE *) vp ;
	        if ((rs = ientry_start(iep,fi,sep)) >= 0) {
	            hdb_dat	key ;
	            hdb_dat	val ;
	            key.buf = iep->svc ;
	            key.len = lenstr(iep->svc) ;
	            val.buf = iep ;
	            val.len = szof(ND_IE) ;
	            rs = hdb_store(op->entsp,key,val) ;
	            if (rs < 0) {
	                ientry_finish(iep) ;
		    }
	        } /* end if (ientry-start) */
	        if (rs < 0) {
	            uc_free(iep) ;
		}
	    } /* end if (memory-allocation) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (nodedb_addentry) */

static int ND_Filedump(nodedb *op,int fi) noex {
	hdb		*elp = op->entsp ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (hdb_cur cur ; (rs = hdb_curbegin(elp,&cur)) >= 0) {
	    hdb_dat	key ;
	    hdb_dat	val ;
	    while (hdb_curenum(elp,&cur,&key,&val) >= 0) {
	        ND_IE	*iep = (ND_IE *) val.buf ;
	        if ((iep->fi == fi) || (fi < 0)) {
	            c += 1 ;
		    {
	                rs1 = hdb_curdel(elp,&cur,0) ;
	                if (rs >= 0) rs = rs1 ;
		    }
		    {
	                rs1 = ientry_finish(iep) ;
	                if (rs >= 0) rs = rs1 ;
		    }
		    {
	                rs1 = uc_free(iep) ;
	                if (rs >= 0) rs = rs1 ;
		    }
	        } /* end if (found matching entry) */
	    } /* end while (looping through entries) */
	    rs1 = hdb_curend(elp,&cur) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (cursor) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (ND_Filedump) */

static int ND_Filedel(nodedb *op,int fi) noex {
	vecobj		*flp = op->filep ;
	int		rs ;
	int		rs1 ;
	if (void *vp{} ; (rs = flp->get(fi,&vp)) >= 0) {
	    if (vp) {
		{
	            ND_F	*fep = (ND_F *) vp ;
	            rs1 = file_finish(fep) ;
	            if (rs >= 0) rs = rs1 ;
		}
		{
	            rs1 = flp->del(fi) ;
	            if (rs >= 0) rs = rs1 ;
		}
	    } /* end if (non-null) */
	} /* end if (vecob_get) */
	return rs ;
}
/* end subroutine (ND_Filedel) */

static int nodedb_entfins(nodedb *op) noex {
	return ND_Filedump(op,-1) ;
}
/* end subroutine (nodedb_entfins) */

static int file_start(ND_F *fep,cchar *fname) noex {
	int		rs = SR_FAULT ;
	if (fep && fname) {
	    rs = SR_INVALID ;
	    memclear(fep) ;
	    if (fname[0]) {
	        if (cchar *cp{} ; (rs = uc_mallocstrw(fname,-1,&cp)) >= 0) {
	            fep->fname = cp ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (file_start) */

static int file_finish(ND_F *fep) noex {
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

static int ientry_start(ND_IE *iep,int fi,SE *sep) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (iep && sep) {
	    vecobj	*klp = &sep->keys ;
	    rs = SR_INVALID ;
	    memclear(iep) ;
	    /* basic checks on input data */
	    if (sep->svc && sep->svc[0]) {
	        iep->fi = fi ;
		/* OK to continue */
	        if ((rs = klp->count) >= 0) {
	            SE_KEY	*kep ;
	            int		sz = 0 ;
	            void	*vp{} ;
	            iep->nkeys = rs ;
	            c = rs ; /* <- return value */
		    /* find the size to allocate (everything) */
	            sz += ((c + 1) * 2 * szof(char *)) ;
	            for (int i = 0 ; klp->get(i,&vp) >= 0 ; i += 1) {
			kep = (SE_KEY *) vp ;
	                if (vp) {
	                    if (kep->kname) sz += kep->kl ;
	                    sz += 1 ;
	                    if (kep->args) sz += kep->al ;
	                    sz += 1 ;
	                } /* end if (non-null) */
	            } /* end for */
		    for (int i = 0 ; i < 3 ; i += 1) {
			cchar	*cp{} ;
	                switch (i) {
	                case 0:
	                    cp = sep->svc ;
	                    break ;
	                case 1:
	                    cp = sep->clu ;
	                    break ;
	                case 2:
	                    cp = sep->sys ;
	                    break ;
	                } /* end switch */
	                sz += (lenstr(cp) + 1) ;
	            } /* end for */
		    /* allocate */
	            iep->tsize = sz ;
	            if ((rs = uc_malloc(sz,&vp)) >= 0) {
	                int	j = 0 ;
	                cchar	*(*keys)[2] = keytabp(vp) ;
	                char	*bp = charp(vp) ;
	                iep->a = charp(vp) ;
	                iep->keys = keys ;
	                bp += ((c + 1) * 2 * szof(char *)) ;
			/* copy over the key-table w/ strings */
	                for (int i = 0 ; klp->get(i,&vp) >= 0 ; i += 1) {
	                    if (vp) {
			        kep = (SE_KEY *) vp ;
	                        if (kep->kname) {
	                            keys[j][0] = bp ;
	                            bp = strwcpy(bp,kep->kname,kep->kl) ;
	                            keys[j][1] = bp++ ;
	                            if (kep->args) {
					cchar	*ap = kep->args ;
	                                keys[j][1] = bp ;
	                                bp = (strwcpy(bp,ap,kep->al) + 1) ;
	                            }
	                            j += 1 ;
				}
	                    } /* end if (non-null) */
	                } /* end for */
	                keys[j][0] = nullptr ;
	                keys[j][1] = nullptr ;
			/* copy over the other stuff */
	                for (int i = 0 ; i < 3 ; i += 1) {
	                    cchar	*cp{} ;
	                    switch (i) {
	                    case 0:
	                        iep->svc = bp ;
	                        cp = sep->svc ;
	                        break ;
	                    case 1:
	                        iep->clu = bp ;
	                        cp = sep->clu ;
	                        break ;
	                    case 2:
	                        iep->sys = bp ;
	                        cp = sep->sys ;
	                        break ;
	                    } /* end switch */
	                    bp = (strwcpy(bp,cp,-1) + 1) ;
	                } /* end for */
	            } else {
	                iep->svc = nullptr ;
	            } /* end if (m-a) */
	        } /* end if (vecobj_count) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (ientry_start) */

static int ientry_finish(ND_IE *iep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (iep) {
	    rs = SR_OK ;
	    if (iep->a) {
	        rs1 = uc_free(iep->a) ;
	        if (rs >= 0) rs = rs1 ;
	        iep->a = nullptr ;
	    }
	    iep->svc = nullptr ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ientry_finish) */

static int svcentry_start(SE *sep,lineinfo *lip) noex {
	int		rs = SR_FAULT ;
	if (sep && lip) {
	    int		sz = 0 ;
	    memclear(sep) ;
	    for (int i = 0 ; i < 3 ; i += 1) {
	        int	cl = lip->f[i].fl ;
	        cchar	*cp = lip->f[i].fp ;
	        if (cl < 0) cl = lenstr(cp) ;
	        sz += (cl + 1) ;
	    } /* end for */
	    if (char *bp ; (rs = uc_malloc(sz,&bp)) >= 0) {
	        cint	ksize = szof(SE_KEY) ;
	        sep->a = bp ;
	        for (int i = 0 ; i < 3 ; i += 1) {
	            switch (i) {
	            case 0:
	                sep->svc = bp ;
	                break ;
	            case 1:
	                sep->clu = bp ;
	                break ;
	            case 2:
	                sep->sys = bp ;
	                break ;
	            } /* end switch */
	            bp = (strwcpy(bp,lip->f[i].fp,lip->f[i].fl) + 1) ;
	        } /* end for */
		/* prepare for arguments */
		cint	vn = 5 ;
		cint	vo = VECOBJ_OORDERED ;
	        rs = vecobj_start(&sep->keys,ksize,vn,vo) ;
	        if (rs < 0) {
	            uc_free(sep->a) ;
	            sep->a = nullptr ;
	        } /* end if (error handling) */
	    } else {
	        sep->svc = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (svcentry_start) */

/* add a key to this entry */
static int svcentry_addkey(SE *sep,cc *kp,int kl,cc *ap,int al) noex {
	int		rs = SR_FAULT ;
	int		sz = 0 ;
	if (sep && kp) {
	    rs = SR_INVALID ;
 	    if ((kl != 0) && kp[0]) {
	        if (kl < 0) kl = lenstr(kp) ;
	        sz += (kl + 1) ;
	        if (ap != nullptr) {
	            if (al < 0) al = lenstr(ap) ;
	            sz += (al + 1) ;
	        } else {
	            sz += 1 ;
	        }
		if (char *bp{} ; (rs = uc_malloc(sz,&bp)) >= 0) {
	            SE_KEY	key{} ;
		    /* copy over (load) the key-name */
	            key.kname = bp ;
	            bp = (strwcpy(bp,kp,kl) + 1) ;
	            key.kl = kl ;
		    /* copy over (load) the arguments (if any) */
	            key.args = bp ;
	            key.al = 0 ;
	            if (ap) {
	                bp = (strwcpy(bp,ap,al) + 1) ;
	                key.al = al ;
	            } else {
	                *bp++ = '\0' ;
	            }
		    /* add the key object to the key-list */
	            rs = vecobj_add(&sep->keys,&key) ;
	            if (rs < 0) {
	                if (key.kname) {
	                    uc_free(key.kname) ;
	                    key.kname = nullptr ;
	                    key.args = nullptr ;
	                }
	            } /* end if (error handling) */
	        } /* end if (m-a) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (svcentry_addkey) */

static int svcentry_finish(SE *sep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (sep) {
	    rs = SR_OK ;
	    if (sep->svc) {
	        vecobj	*klp = &sep->keys ;
	        void	*vp{} ;
	        for (int i = 0 ; klp->get(i,&vp) >= 0 ; i += 1) {
	    	    SE_KEY	*kep = (SE_KEY *) vp ;
	            if (vp) {
	                if (kep->kname) {
	                    rs1 = uc_free(kep->kname) ;
	                    if (rs >= 0) rs = rs1 ;
	                }
	            }
	        } /* end for */
	        {
	            rs1 = vecobj_finish(&sep->keys) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	        {
	            rs1 = freeit(&sep->a) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	        sep->svc = nullptr ;
	    } /* end if (non-null) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (svcentry_finish) */

static int entry_load(ND_E *ep,char *ebuf,int elen,ND_IE *iep) noex {
	cint		bo = NODEDB_BO((ulong) ebuf) ;
	int		rs = SR_OK ;
	int		svclen = 0 ;
	if (iep->tsize <= (elen - bo)) {
	    cint	kal = (iep->nkeys + 1) * 2 * szof(char *) ;
	    cchar	*(*keys)[2] = (cchar *(*)[2]) (ebuf + bo) ;
	    char	*bp ;
	    bp = charp(ebuf + bo + kal) ;
	    /* copy in the nodename */
	    ep->svc = bp ;
	    bp = (strwcpy(bp,iep->svc,-1) + 1) ;
	    svclen = intconv(bp - ep->svc - 1) ;
	    /* copy in the clustername */
	    ep->clu = bp ;
	    if (iep->clu) {
	        bp = (strwcpy(bp,iep->clu,-1) + 1) ;
	    } else {
	        *bp++ = '\0' ;
	    }
	    /* copy in the systemname */
	    ep->sys = bp ;
	    if (iep->sys) {
	        bp = (strwcpy(bp,iep->sys,-1) + 1) ;
	    } else {
	        *bp++ = '\0' ;
	    }
	    /* copy in the key=values */
	    {
		int	i{} ; /* used-afterwards */
	        for (i = 0 ; i < iep->nkeys ; i += 1) {
	            keys[i][0] = bp ;
	            bp = (strwcpy(bp,iep->keys[i][0],-1) + 1) ;
	            if (iep->keys[i][1] != nullptr) {
	                keys[i][1] = bp ;
	                bp = (strwcpy(bp,iep->keys[i][1],-1) + 1) ;
	            } else {
	                keys[i][1] = nullptr ;
	            }
	        } /* end for */
	        keys[i][0] = nullptr ;
	        keys[i][1] = nullptr ;
	    } /* end block */
	    ep->keys = keys ;
	    ep->fi = iep->fi ;
	    ep->nkeys = iep->nkeys ;
	    ep->esize = iep->tsize ;
	} else {
	    rs = SR_OVERFLOW ;
	}
	return (rs >= 0) ? svclen : rs ;
}
/* end subroutine (entry_load) */

static int freeit(cchar **pp) noex {
	int		rs = SR_OK ;
	if (*pp) {
	    rs = uc_free(*pp) ;
	    *pp = nullptr ;
	}
	return rs ;
}
/* end subroutine (freeit) */

static int mkterms() noex {
	int		rs ;
	if ((rs = fieldterms(fterms,false,'\b','\t','\v','\f',' ')) >= 0) {
	    if ((rs = fieldterms(fterms,true,'#',',',':','=')) >= 0) {
		rs = fieldterms(saterms,false,'#') ;
	    }
	}
	return rs ;
}
/* end subroutine (mkterms) */

vars::operator int () noex {
    	int		rs ;
	if ((rs = getbufsize(getbufsize_hn)) >= 0) {
	    maxhostlen = rs ;
	    if ((rs = getbufsize(getbufsize_ml)) >= 0) {
		lineslen = ((rs + 1) * LINEBUFMULT) ;
	        if ((rs = getbufsize(getbufsize_nn)) >= 0) {
		    entbuflen = (rs * NODEDB_ENTLENMULT) ;
		}
	    }
	}
	return rs ;
}
/* end method (vars::operator) */

static int mkinit() noex {
    	int		rs ;
	if ((rs = var) >= 0) {
	    rs = mkterms() ;
	}
	return rs ;
}
/* end subroutine (mkinit) */

static int cmpfne(ND_F *e1p,ND_F *e2p) noex {
	int		rc = 0 ;
	cchar		*s1 = e1p->fname ;
	cchar		*s2 = e2p->fname ;
	{
	    cint	ch1 = mkchar(*s1) ;
	    cint	ch2 = mkchar(*s2) ;
	    if ((rc = (ch1 - ch2)) == 0) {
		rc = strcmp(s1,s2) ;
	    }
	}
	return rc ;
}
/* end subroutine (cmpfne) */

static int vcmpfn(cvoid **v1pp,cvoid **v2pp) noex {
	ND_F		*e1p = (ND_F *) *v1pp ;
	ND_F		*e2p = (ND_F *) *v2pp ;
	int		rc = 0 ;
	if (e1p || e2p) {
	    rc = +1 ;
	    if (e1p) {
		rc = -1 ;
	        if (e2p) {
		    rc = cmpfne(e1p,e2p) ;
	        }
	    }
	}
	return rc ;
}
/* end subroutine (vcmpfn) */


