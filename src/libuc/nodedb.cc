/* nodedb SUPPORT */
/* lang=C++20 */

/* magement for the NODE-DB file */
/* version %I% last-modified %G% */

#define	CF_SAFECHECK	0		/* extra safety */

/* revision history:

	= 2004-05-25, David A­D­ Morano
	This subroutine was adopted for use as the node-database
	reader.

	= 2013-04-20, David A­D­ Morano
	I was totally on drugs when I originally wrote this. It was
	incredibly complicated. It allowed for service entries to
	span multiple lines (like a service-entry file). I must
	have been popping acid at the time. This
	service-entry-spanning-muliple lines business was never
	used, so I got rid of it. It all has to be and look so
	simple so that no errors are really possible. This is often
	easier said than done, but when some complicated behavior
	is not needed (at all), then we get rid of it in favor of
	much simpler implementations.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This object manages access to one or more NODE-DB files.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>		/* <- for |UCHAR_MAX| + |CHAR_BIT| */
#include	<cstdlib>
#include	<cstring>		/* <- for |strcmp(3c)| + |strlen(3c)| */
#include	<ctime>
#include	<netdb.h>
#include	<usystem.h>
#include	<bufsizevar.hh>
#include	<mallocxx.h>
#include	<bfile.h>
#include	<field.h>
#include	<vecobj.h>
#include	<hdb.h>
#include	<char.h>
#include	<mkchar.h>
#include	<strwcpy.h>
#include	<mkpathx.h>
#include	<sfx.h>
#include	<mkpathrooted.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"nodedb.h"


/* local defines */

#define	NODEDB_MINCHECKTIME	5	/* file check interval (seconds) */
#define	NODEDB_CHECKTIME	60	/* file check interval (seconds) */
#define	NODEDB_CHANGETIME	3	/* wait change interval (seconds) */
#define	NODEDB_DEFNETGROUP	"DEFAULT"

#define	NODEDB_FILE		struct nodedb_file
#define	NODEDB_KEYNAME		struct nodedb_keyname
#define	NODEDB_IE		struct nodedb_ie

#define	ND			nodedb
#define	ND_C			nodedb_cur
#define	ND_E			nodedb_ent
#define	ND_F			nodedb_f
#define	ND_K			nodedb_k
#define	ND_I			nodedb_i

#define	SVCENTRY		struct svcentry
#define	SVCENTRY_KEY		struct svcentry_key

#define	LINEINFO		struct lineinfo
#define	LINEINFO_FIELD		struct lineinfo_field

#define	NODEDB_KA		sizeof(char *(*)[2])
#define	NODEDB_BO(v)		\
	((NODEDB_KA - ((v) % NODEDB_KA)) % NODEDB_KA)

#undef	ARGSBUFLEN
#define	ARGSBUFLEN		(3 * MAXHOSTNAMELEN)

#define	KEYALIGNMENT		sizeof(char *(*)[2])


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */

struct nodedb_file ;
struct nodedb_keyname ;
struct nodedb_ie ;

typedef NODEDB_FILE	nodedb_f ;
typedef NODEDB_KEYNAME	nodedb_k ;
typedef NODEDB_IE	nodedb_i ;

typedef cchar		*(*keytabp)[2] ;


/* external subroutines */

extern "C" {
    extern int	field_srvarg(field *,cchar *,char *,int) noex ;
}


/* external variables */


/* local structures */

struct nodedb_file {
	cchar		*fname ;
	time_t		mtime ;
	dev_t		dev ;
	ino_t		ino ;
	int		size ;
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
	int		size ;			/* total size */
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


/* forward references */

static int	mkterms() noex ;

template<typename ... Args>
static inline int nodedb_ctor(nodedb *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    const nullptr_t	np{} ;
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
	if (op && (args && ...)) {
	    rs = (op->magic == NODEDB_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (nodedb_magic) */

static int	nodedb_fileadder(nodedb *op,cchar *fname) noex ;
static int	nodedb_filefins(nodedb *) noex ;
static int	nodedb_entfins(nodedb *) noex ;
static int	nodedb_fileparse(nodedb *,int) noex ;
static int	nodedb_fileparser(nodedb *,NODEDB_FILE *,int) noex ;
static int	nodedb_fileparseline(nodedb *,int,cchar *,int) noex ;
static int	nodedb_filedump(nodedb *,int) noex ;
static int	nodedb_filedel(nodedb *,int) noex ;
static int	nodedb_addentry(nodedb *,int,SVCENTRY *) noex ;
static int	nodedb_checkfiles(nodedb *,time_t) noex ;

static int	file_start(NODEDB_FILE *,cchar *) noex ;
static int	file_finish(NODEDB_FILE *) noex ;

static int	ientry_start(NODEDB_IE *,int,SVCENTRY *) noex ;
static int	ientry_finish(NODEDB_IE *) noex ;

static int	svcentry_start(SVCENTRY *,LINEINFO *) noex ;
static int	svcentry_addkey(SVCENTRY *,cchar *,int,cchar *,int) noex ;
static int	svcentry_finish(SVCENTRY *) noex ;

static int	entry_load(NODEDB_ENT *,char *,int,NODEDB_IE *) noex ;

static int	vcmpfname(NODEDB_FILE **,NODEDB_FILE **) noex ;

static int	freeit(cchar **) noex ;


/* local variables */

constexpr int		termsize = ((UCHAR_MAX+1)/CHAR_BIT) ;

/* all white-space, pound ('#'), colon (':'), equal ('='), and comma (',') */
static char		fterms[termsize] ;

/* argument field terminators (pound '#' and comma ',') */
static char 		saterms[termsize] ;

static bufsizevar	maxhostlen(getbufsize_hn) ;


/* exported subroutines */

int nodedb_open(nodedb *op,cchar *fname) noex {
	static cint	srs = mkterms() ;
	cint		nf = NODEDB_NFILES ;
	cint		defents = NODEDB_DEFENTS ;
	int		rs ;
	if ((rs = srs) >= 0) {
	    rs = SR_FAULT ;
	    if ((rs = nodedb_ctor(op,fname)) >= 0) {
	        rs = SR_INVALID ;
	        if (fname[0]) {
		    vecobj	*flp = op->filep ;
	            cint	fsize = sizeof(NODEDB_FILE) ;
	            cint	vo = VECOBJ_OREUSE ;
	            if ((rs = vecobj_start(flp,fsize,nf,vo)) >= 0) {
	                const nullptr_t	np{} ;
		        hdb		*elp = op->entsp ;
	                if ((rs = hdb_start(elp,defents,0,np,np)) >= 0) {
	                    op->checktime = time(nullptr) ;
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
	        } /* end if (valid) */
		if (rs < 0) {
		    nodedb_dtor(op) ;
		}
	    } /* end if (non-null) */
	} /* end if (mkterms) */
	return rs ;
}
/* end subroutine (nodedb_open) */

int nodedb_close(nodedb *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = nodedb_magic(op)) >= 0) {
	    {
	        rs1 = nodedb_entfins(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = nodedb_filefins(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = hdb_finish(op->entsp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
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

int nodedb_fileadd(nodedb *op,cchar *fname) noex {
	int		rs ;
	int		rs1 ;
	int		rc = 0 ;
	if ((rs = nodedb_magic(op,fname)) >= 0) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
		char	*tbuf{} ;
		if ((rs = malloc_mp(&tbuf)) >= 0) {
		    if ((rs = mkpathrooted(tbuf,fname)) >= 0) {
		        rs = nodedb_fileadder(op,tbuf) ;
		        rc = rs ;
	            } /* end if (mkpathrooted) */
		    rs1 = uc_free(tbuf) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
	    } else {
		rs = nodedb_fileadder(op,fname) ;
		rc = rs ;
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (nodedb_fileadd) */

int nodedb_curbegin(nodedb *op,nodedb_cur *curp) noex {
	int		rs ;
	if ((rs = nodedb_magic(op,curp)) >= 0) {
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
	if ((rs = nodedb_magic(op,curp)) >= 0) {
	    curp->i = -1 ;
	    if ((rs = hdb_curend(op->entsp,curp->ecp)) >= 0) {
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
	if ((rs = nodedb_magic(op,curp)) >= 0) {
	    hdb_dat	key{} ;
	    hdb_dat	val{} ;
	    hdb_cur	*ecp = curp->ecp ;
	    if ((rs = hdb_enum(op->entsp,ecp,&key,&val)) >= 0) {
	        NODEDB_IE	*iep = (NODEDB_IE *) val.buf ;
	        if (ep && ebuf) {
	            rs = entry_load(ep,ebuf,elen,iep) ;
	            svclen = rs ;
	        } else {
	            svclen = strlen(iep->svc) ;
	        }
	    } /* end if (had an entry) */
	} /* end if (magic) */
	return (rs >= 0) ? svclen : rs ;
}
/* end subroutine (nodedb_enum) */

int nodedb_fetch(ND *op,cc *svcbuf,ND_C *curp,
		ND_E *ep,char *ebuf,int elen) noex {
	int		rs ;
	int		svclen = 0 ;
	if ((rs = nodedb_magic(op,curp)) >= 0) {
	    hdb_dat	key{} ;
	    hdb_dat	val{} ;
	    hdb_cur	*ecp = curp->ecp ;
	    key.buf = svcbuf ;
	    key.len = strlen(svcbuf) ;
	    if ((rs = hdb_fetch(op->entsp,key,ecp,&val)) >= 0) {
	        NODEDB_IE	*iep = (NODEDB_IE *) val.buf ;
	        if (ep && ebuf) {
	            rs = entry_load(ep,ebuf,elen,iep) ;
	            svclen = rs ;
	        } else {
	            svclen = strlen(iep->svc) ;
	        }
	    } /* end if (hdb_fetch) */
	} /* end if (magic) */
	return (rs >= 0) ? svclen : rs ;
}
/* end subroutine (nodedb_fetch) */

int nodedb_check(nodedb *op) noex {
	int		rs ;
	if ((rs = nodedb_magic(op)) >= 0) {
	    if (op->cursors == 0) {
	        time_t	dt = time(nullptr) ;
	        if ((dt - op->checktime) > NODEDB_CHECKTIME) {
	            rs = nodedb_checkfiles(op,dt) ;
	        }
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (nodedb_check) */


/* private subroutines */

static int nodedb_fileadder(NODEDB *op,cchar *fname) noex {
	NODEDB_FILE	fe{} ;
	int		rs ;
	if ((rs = file_start(&fe,fname)) >= 0) {
	    vecobj	*flp = op->filep ;
	    vecobj_vcf	vcf = vecobj_vcf(vcmpfname) ;
	    cint	rsn = SR_NOTFOUND ;
	    if ((rs = vecobj_search(flp,&fe,vcf,nullptr)) == rsn) {
	        if ((rs = vecobj_add(flp,&fe)) >= 0) {
	            cint	fi = rs ;
	            rs = nodedb_fileparse(op,fi) ;
	            if (rs < 0) {
	                nodedb_filedel(op,fi) ;
	            }
	        } /* end if (vecobj_add) */
	    } /* end if (vecobj_search) */
	    if (rs < 0) {
	        file_finish(&fe) ;
	    }
	} /* end if (file-start) */
	return rs ;
}
/* end subroutine (nodedb_fileadder) */

static int nodedb_filefins(nodedb *op) noex {
	vecobj		*flp = op->filep ;
	int		rs = SR_OK ;
	int		rs1 ;
	void		*vp{} ;
	for (int i = 0 ; vecobj_get(flp,i,&vp) >= 0 ; i += 1) {
	    if (vp) {
	        NODEDB_FILE	*fep = (NODEDB_FILE *) vp ;
	        rs1 = file_finish(fep) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end for */
	return rs ;
}
/* end subroutine (nodedb_filefins) */

static int nodedb_checkfiles(nodedb *op,time_t daytime) noex {
	vecobj		*flp = op->filep ;
	int		rs = SR_OK ;
	int		c_changed = 0 ;
	void		*vp{} ;
	for (int i = 0 ; vecobj_get(flp,i,&vp) >= 0 ; i += 1) {
	    if (vp) {
		USTAT		sb ;
		NODEDB_FILE	*fep = (NODEDB_FILE *) vp ;
	        if ((rs = u_stat(fep->fname,&sb)) >= 0) {
	            if (sb.st_mtime > fep->mtime) {
	                c_changed += 1 ;
	                nodedb_filedump(op,i) ;
	                rs = nodedb_fileparse(op,i) ;
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

static int nodedb_fileparse(nodedb *op,int fi) noex {
	vecobj		*flp = op->filep ;
	int		rs ;
	int		c = 0 ;
	void		*vp{} ;
	if ((rs = vecobj_get(flp,fi,&vp)) >= 0) {
	    rs = SR_NOTFOUND ;
	    if (vp) {
		NODEDB_FILE	*fep = (NODEDB_FILE *) vp ;
	        rs = nodedb_fileparser(op,fep,fi) ;
		c = rs ;
	        if (rs < 0) {
	            nodedb_filedump(op,fi) ;
	        }
	    } /* end if (non-null) */
	} /* end if (vector_get) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (nodedb_fileparse) */

static int nodedb_fileparser(nodedb *op,NODEDB_FILE *fep,int fi) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	char		*lbuf{} ;
	if ((rs = malloc_ml(&lbuf)) >= 0) {
	    bfile	loadfile, *lfp = &loadfile ;
	    cint	llen = rs ;
	    if ((rs = bopen(lfp,fep->fname,"r",0664)) >= 0) {
	        USTAT	sb ;
	        int	f_bol = true ;
	        int	f_eol ;
	        if ((rs = bcontrol(lfp,BC_STAT,&sb)) >= 0) {
	            if (S_ISREG(sb.st_mode)) {
	                if (fep->mtime < sb.st_mtime) {
			    const nullptr_t	np{} ;
	                    fep->dev = sb.st_dev ;
	                    fep->ino = sb.st_ino ;
	                    fep->mtime = sb.st_mtime ;
	                    fep->size = sb.st_size ;
	                    while ((rs = breadlns(lfp,lbuf,llen,-1,np)) > 0) {
	                        int	len = rs ;
	                        int	cl ;
	                        cchar	*cp ;
	                        f_eol = (lbuf[len - 1] == '\n') ;
	                        if (f_eol) len -= 1 ;
	                        lbuf[len] = '\0' ;
	                        if ((cl = sfskipwhite(lbuf,len,&cp)) > 0) {
	                            if (f_bol && (*cp != '#')) {
	                                rs = nodedb_fileparseline(op,fi,cp,cl) ;
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
/* end subroutine (nodedb_fileparser) */

static int nodedb_fileparseline(nodedb *op,int fi,cchar *lp,int ll) noex {
	SVCENTRY	se ;
	LINEINFO	li{} ;
	field		fsb ;
	int		rs ;
	int		rs1 ;
	int		c_field = 0 ;
	int		f_ent = false ;
	if ((rs = field_start(&fsb,lp,ll)) >= 0) {
	    cint	argslen = ARGSBUFLEN ;
	    int		fl ;
	    cchar	*fp ;
	    char	argsbuf[ARGSBUFLEN + 1] ;

	    while ((fl = field_get(&fsb,fterms,&fp)) >= 0) {

	        if ((c_field == 0) && (fl == 0)) break ;

	        if ((c_field < 3) && (fsb.term == ':')) {
	            li.f[c_field].fp = fp ;
	            li.f[c_field].fl = fl ;
	        }

	        if ((c_field >= 3) && (fl > 0)) {
	            cchar	*kp = fp ;
	            int		kl = fl ;
	            int		al ;

	            if (! f_ent) {
	                rs = svcentry_start(&se,&li) ;
	                f_ent = (rs >= 0) ;
	                if (rs < 0) break ;
	            }

	            argsbuf[0] = '\0' ;
	            al = 0 ;
	            if (fsb.term == '=') {
	                al = field_srvarg(&fsb,saterms,argsbuf,argslen) ;
	            }

	            if ((rs >= 0) && f_ent) {
	                rs = svcentry_addkey(&se,kp,kl,argsbuf,al) ;
	            }

	        } /* end if (handling key-value pair) */

	        c_field += 1 ;
	        if (fsb.term == '#') break ;
	        if (rs < 0) break ;
	    } /* end while (fields) */

	    rs1 = field_finish(&fsb) ;
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

	return (rs >= 0) ? f_ent : rs ;
}
/* end subroutine (nodedb_fileparseline) */

static int nodedb_addentry(nodedb *op,int fi,SVCENTRY *sep) noex {
	int		rs = SR_FAULT ;
	if (sep) {
	    cint	size = sizeof(NODEDB_IE) ;
	    void	*vp{} ;
	    if ((rs = uc_malloc(size,&vp)) >= 0) {
	        NODEDB_IE	*iep = (NODEDB_IE *) vp ;
	        if ((rs = ientry_start(iep,fi,sep)) >= 0) {
	            hdb_dat	key ;
	            hdb_dat	val ;
	            key.buf = iep->svc ;
	            key.len = strlen(iep->svc) ;
	            val.buf = iep ;
	            val.len = sizeof(NODEDB_IE) ;
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

static int nodedb_filedump(nodedb *op,int fi) noex {
	hdb		*elp = op->entsp ;
	hdb_cur		cur{} ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = hdb_curbegin(elp,&cur)) >= 0) {
	    hdb_dat	key ;
	    hdb_dat	val ;
	    while (hdb_enum(elp,&cur,&key,&val) >= 0) {
	        NODEDB_IE	*iep = (NODEDB_IE *) val.buf ;
	        if ((iep->fi == fi) || (fi < 0)) {
	            c += 1 ;
		    {
	                rs1 = hdb_delcur(elp,&cur,0) ;
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
/* end subroutine (nodedb_filedump) */

static int nodedb_filedel(nodedb *op,int fi) noex {
	vecobj		*flp = op->filep ;
	int		rs ;
	int		rs1 ;
	void		*vp{} ;
	if ((rs = vecobj_get(flp,fi,&vp)) >= 0) {
	    if (vp) {
		{
	            NODEDB_FILE	*fep = (NODEDB_FILE *) vp ;
	            rs1 = file_finish(fep) ;
	            if (rs >= 0) rs = rs1 ;
		}
		{
	            rs1 = vecobj_del(flp,fi) ;
	            if (rs >= 0) rs = rs1 ;
		}
	    } /* end if (non-null) */
	} /* end if (vecob_get) */
	return rs ;
}
/* end subroutine (nodedb_filedel) */

static int nodedb_entfins(nodedb *op) noex {
	return nodedb_filedump(op,-1) ;
}
/* end subroutine (nodedb_entfins) */

static int file_start(NODEDB_FILE *fep,cchar *fname) noex {
	int		rs = SR_FAULT ;
	if (fep && fname) {
	    rs = SR_INVALID ;
	    memclear(fep) ;
	    if (fname[0]) {
	        cchar	*cp{} ;
	        if ((rs = uc_mallocstrw(fname,-1,&cp)) >= 0) {
	            fep->fname = cp ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (file_start) */

static int file_finish(NODEDB_FILE *fep) noex {
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

static int ientry_start(NODEDB_IE *iep,int fi,SVCENTRY *sep) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (iep && sep) {
	    vecobj	*klp = &sep->keys ;
	    rs = SR_INVALID ;
	    memclear(iep) ;
/* basic checks on input data */
	    if (sep->svc && sep->svc[0]) {
	        iep->fi = fi ;
/* ok to continue */
	        if ((rs = vecobj_count(klp)) >= 0) {
	            SVCENTRY_KEY	*kep ;
	            int		size = 0 ;
	            void	*vp{} ;
	            iep->nkeys = rs ;
	            c = rs ;
/* find the size to allocate (everything) */
	            size += ((c + 1) * 2 * sizeof(char *)) ;
	            for (int i = 0 ; vecobj_get(klp,i,&vp) >= 0 ; i += 1) {
	                if (vp) {
			    kep = (SVCENTRY_KEY *) vp ;
	                    if (kep->kname) size += kep->kl ;
	                    size += 1 ;
	                    if (kep->args) size += kep->al ;
	                    size += 1 ;
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
	                size += (strlen(cp) + 1) ;
	            } /* end for */
/* allocate */
	            iep->size = size ;
	            if ((rs = uc_malloc(size,&vp)) >= 0) {
	                int	j = 0 ;
	                cchar	*(*keys)[2] = keytabp(vp) ;
	                char	*bp = charp(vp) ;
	                iep->a = (cchar *) vp ;
	                iep->keys = keys ;
	                bp += ((c + 1) * 2 * sizeof(char *)) ;
/* copy over the key-table and the key and value strings */
	                for (int i = 0 ; vecobj_get(klp,i,&vp) >= 0 ; i += 1) {
	                    if (vp) {
			        kep = (SVCENTRY_KEY *) vp ;
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

static int ientry_finish(NODEDB_IE *iep) noex {
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

static int svcentry_start(SVCENTRY *sep,LINEINFO *lip) noex {
	int		rs = SR_FAULT ;
	if (sep && lip) {
	    int		size = 0 ;
	    char	*bp ;
	    memclear(sep) ;
	    for (int i = 0 ; i < 3 ; i += 1) {
	        int	cl = lip->f[i].fl ;
	        cchar	*cp = lip->f[i].fp ;
	        if (cl < 0) cl = strlen(cp) ;
	        size += (cl + 1) ;
	    } /* end for */
	    if ((rs = uc_malloc(size,&bp)) >= 0) {
	        cint	ksize = sizeof(SVCENTRY_KEY) ;
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
	        rs = vecobj_start(&sep->keys,ksize,5,VECOBJ_OORDERED) ;
	        if (rs < 0) {
	            uc_free(sep->a) ;
	            sep->a = nullptr ;
	        }
	    } else {
	        sep->svc = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (svcentry_start) */

/* add a key to this entry */
static int svcentry_addkey(SVCENTRY *sep,cc *kp,int kl,cc *ap,int al) noex {
	int		rs = SR_FAULT ;
	int		size = 0 ;
	if (sep && kp) {
	    rs = SR_INVALID ;
 	    if ((kl != 0) && kp[0]) {
		char	*bp{} ;
	        if (kl < 0) kl = strlen(kp) ;
	        size += (kl + 1) ;
	        if (ap != nullptr) {
	            if (al < 0) al = strlen(ap) ;
	            size += (al + 1) ;
	        } else {
	            size += 1 ;
	        }
	        if ((rs = uc_malloc(size,&bp)) >= 0) {
	            SVCENTRY_KEY	key{} ;
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
	            }
	        } /* end if (m-a) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (svcentry_addkey) */

static int svcentry_finish(SVCENTRY *sep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (sep) {
	    rs = SR_OK ;
	    if (sep->svc) {
	        vecobj	*klp = &sep->keys ;
	        void	*vp{} ;
	        for (int i = 0 ; vecobj_get(klp,i,&vp) >= 0 ; i += 1) {
	            if (vp) {
	    	        SVCENTRY_KEY	*kep = (SVCENTRY_KEY *) vp ;
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

static int entry_load(ND_E *ep,char *ebuf,int elen,ND_I *iep) noex {
	cint		bo = NODEDB_BO((ulong) ebuf) ;
	int		rs = SR_OK ;
	int		svclen = 0 ;
	if (iep->size <= (elen - bo)) {
	    cint	kal = (iep->nkeys + 1) * 2 * sizeof(char *) ;
	    cchar	*(*keys)[2] = (cchar *(*)[2]) (ebuf + bo) ;
	    char	*bp ;
	    bp = (char *) (ebuf + bo + kal) ;
/* copy in the nodename */
	    ep->svc = bp ;
	    bp = (strwcpy(bp,iep->svc,-1) + 1) ;
	    svclen = (bp - ep->svc - 1) ;
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
		int	i{} ; /* used afterwards */
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
	    ep->size = iep->size ;
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

static int vcmpfname(NODEDB_FILE **e1pp,NODEDB_FILE **e2pp) noex {
	ND_F		*e1p = *e1pp ;
	ND_F		*e2p = *e2pp ;
	int		rc = 0 ;
	if (e1p || e2p) {
	    if (e1p) {
	        if (e2p) {
		    cchar	*s1 = e1p->fname ;
		    cchar	*s2 = e2p->fname ;
		    {
		        cint	ch1 = mkchar(*s1) ;
		        cint	ch2 = mkchar(*s2) ;
			if ((rc = (ch1 - ch2)) == 0) {
	                    rc = strcmp(s1,s2) ;
			}
		    }
	        } else
	            rc = -1 ;
	    } else
	        rc = 1 ;
	}
	return rc ;
}
/* end subroutine (vcmpfname) */


