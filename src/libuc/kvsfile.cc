/* kvsfile SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* perform key-value-file related functions */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-01, David A­D­ Morano
	This object was originally written.

	- 2004-05-25, David A­D­ Morano
	This subroutine was adopted for use as a general key-value
	key-value-file reader.

*/

/* Copyright © 1998,2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	kvsfile

	Description:
	This object processes an access table for use by daemon
	multiplexing server programs that want to control access
	to their sub-servers.

	Implementation note:
	We let stale keys stay around.  They are not lost, just not
	freed when no longer needed.  There is no memory leak as
	they are all freed when the object is deconstructed.  Stale
	keys do sort of serve as a ready key cache for those cases
	when they may be needed later on with future entries!

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>			/* |time_t| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strcmp(3c)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<netdb.h>
#include	<usystem.h>
#include	<getpwd.h>
#include	<mallocxx.h>
#include	<vecobj.h>
#include	<hdb.h>
#include	<field.h>
#include	<fieldterminit.hh>
#include	<absfn.h>		/* absolute-file-name */
#include	<hash.h>
#include	<sncpyx.h>
#include	<snwcpy.h>
#include	<mkpathx.h>
#include	<strwcpy.h>
#include	<sfx.h>
#include	<intsat.h>
#include	<char.h>
#include	<localmisc.h>

#include	"kvsfile.h"

#pragma		GCC dependency	"mod/libutil.ccm"
#pragma		GCC dependency	"mod/ucstream.ccm"

import libutil ;			/* |lenstr(3u)| */
import ucstream ;

/* local defines */

#define	KF_MINCHECKTIME	5	/* check interval (seconds) */
#define	KF_CHECKTIME	60	/* check interval (seconds) */
#define	KF_CHANGETIME	3	/* wait change interval (seconds) */
#define	KF_DEFNETGROUP	"DEFAULT"
#define	KF_MAGIC	KVSFILE_MAGIC
#define	KF_KEYLEN	KVSFILE_KEYLEN
#define	KF_DEFENTS	KVSFILE_DEFENTS
#define	KF_DEFILES	KVSFILE_DEFFILES

#define	KF		kvsfile
#define	KF_FILE		kvsfile_file
#define	KF_KEY		kvsfile_key
#define	KF_ENT		kvsfile_ent
#define	KF_CUR		kvsfile_cur

#define	KEYBUFLEN	KF_KEYLEN


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */

extern "C" {
    int		kvsfile_fileadd(kvsfile *,cchar *) noex ;
}


/* external variables */


/* local structures */

struct kvsfile_file {
	cchar		*fname ;
	time_t		mtime ;
	ino_t		ino ;
	dev_t		dev ;
	int		fsize ;
} ;

struct kvsfile_key {
	cchar		*kname ;
	int		count ;
} ;

struct kvsfile_ent {
	KF_KEY	*kep ;
	cchar		*vname ;
	int		vlen ;
	int		fi ;		/* KF_FILE index */
	int		ki ;		/* key index */
} ;

typedef kvsfile_file *	filep ;
typedef kvsfile_key *	keyp ;
typedef kvsfile_ent *	entp ;


/* forward references */

template<typename ... Args>
static int kvsfile_ctor(kvsfile *op,Args ... args) noex {
	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = SR_NOMEM ;
	    op->magic = 0 ;
	    if ((op->flp = new(nothrow) vecobj) != np) {
	        if ((op->klp = new(nothrow) vecobj) != np) {
	            if ((op->kvlp = new(nothrow) hdb) != np) {
	                if ((op->elp = new(nothrow) hdb) != np) {
			    rs = SR_OK ;
			} /* end if (new-hdb) */
		        if (rs < 0) {
		            delete op->kvlp ;
		            op->kvlp = nullptr ;
		        }
	            } /* end if (new-hdb) */
		    if (rs < 0) {
		        delete op->klp ;
		        op->klp = nullptr ;
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
/* end subroutine (kvsfile_ctor) */

static int kvsfile_dtor(kvsfile *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->elp) {
		delete op->elp ;
		op->elp = nullptr ;
	    }
	    if (op->kvlp) {
		delete op->kvlp ;
		op->kvlp = nullptr ;
	    }
	    if (op->klp) {
		delete op->klp ;
		op->klp = nullptr ;
	    }
	    if (op->flp) {
		delete op->flp ;
		op->flp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (kvsfile_dtor) */

template<typename ... Args>
static int kvsfile_magic(kvsfile *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == KF_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (kvsfile_magic) */

static int	kvsfile_filefins(kvsfile *) noex ;
static int	kvsfile_keyfins(kvsfile *) noex ;
static int	kvsfile_fh(kvsfile *,dev_t,ino_t) noex ;
static int	kvsfile_fparse(kvsfile *,int) noex ;
static int	kvsfile_fparser(kvsfile *,int,ucstream *) noex ;
static int	kvsfile_fparsel(kvsfile *,int,cc *,int) noex ;
static int	kvsfile_getkeyp(kvsfile *,cchar *,KF_KEY **) noex ;
static int	kvsfile_filedump(kvsfile *,int) noex ;
static int	kvsfile_addentry(kvsfile *,KF_ENT *) noex ;
static int	kvsfile_already(kvsfile *,KF_ENT *) noex ;
static int	kvsfile_checkfiles(kvsfile *,time_t) noex ;

#ifdef	COMMENT
static int	kvsfile_filedel(kvsfile *,int) noex ;
#endif

static int	file_start(KF_FILE *,cchar *) noex ;
static int	file_finish(KF_FILE *) noex ;

static int	key_start(KF_KEY *,cchar *) noex ;
static int	key_increment(KF_KEY *) noex ;
static int	key_decrement(KF_KEY *) noex ;
static int	key_finish(KF_KEY *) noex ;

static int entry_start(KF_ENT *,int,int,KF_KEY *,cchar *,int) noex ;
static int entry_finish(KF_ENT *) noex ;

extern "C" {
    static int	vcmpfname(cvoid **,cvoid **) noex ;
    static int	vcmpkey(cvoid **,cvoid **) noex ;
    static int	cmpkeyval(cvoid *,cvoid *,int) noex ;
}

extern "C" {
    static uint	hashkeyval(cvoid *,int) noex ;
}


/* local variables */

/* all white space, pound ('#'), colon (':'), and comma (',') */
constexpr fieldterminit		ta("\b\t\r\v\f #,:") ; /* term-array */


/* exported variables */


/* exported subroutines */

int kvsfile_open(kvsfile *op,int ndef,cchar *atfname) noex {
	cnullptr	np{} ;
	int		rs ;
	if ((rs = kvsfile_ctor(op)) >= 0) {
	    cint	fsz = szof(KF_FILE) ;
	    cint	vo = (VECOBJ_OSTATIONARY | VECOBJ_OREUSE) ;
	    if (ndef < KF_DEFENTS) ndef = KVSFILE_DEFENTS ;
	    if ((rs = vecobj_start(op->flp,fsz,ndef,vo)) >= 0) {
	        cint	vn = (ndef / 10) ;
	        cint	ksz = szof(KF_KEY) ;
	        if ((rs = vecobj_start(op->klp,ksz,vn,vo)) >= 0) {
		    hdbcmp_f	cf = hdbcmp_f(cmpkeyval) ;
		    hdbhash_f	hf = hdbhash_f(hashkeyval) ;
	            if ((rs = hdb_start(op->kvlp,ndef,0,hf,cf)) >= 0) {
		        hdb		*elp = op->elp ;
	                if ((rs = hdb_start(elp,ndef,0,np,np)) >= 0) {
	                    op->magic = KF_MAGIC ;
	                    op->ti_check = getustime ;
	                    if (atfname && (atfname[0] != '\0')) {
	                        rs = kvsfile_fileadd(op,atfname) ;
	                        if (rs < 0) {
	                            op->magic = 0 ;
	                        }
	                    } /* end if (adding first file) */
	                    if (rs < 0) {
	                        hdb_finish(op->elp) ;
			    }
	                } /* end if (entries) */
	                if (rs < 0) {
	                    hdb_finish(op->kvlp) ;
		        }
	            } /* end if (keyvals) */
	            if (rs < 0) {
	                vecobj_finish(op->klp) ;
		    }
	        } /* end if (keys) */
	        if (rs < 0) {
	            vecobj_finish(op->flp) ;
	        }
	    } /* end if (files) */
	    if (rs < 0) {
		kvsfile_dtor(op) ;
	    }
	} /* end if (kvsfile_ctor) */
	return rs ;
}
/* end subroutine (kvsfile_open) */

int kvsfile_close(kvsfile *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = kvsfile_magic(op)) >= 0) {
	    {
	        rs1 = kvsfile_filedump(op,-1) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = kvsfile_keyfins(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = kvsfile_filefins(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->elp) {
	        rs1 = hdb_finish(op->elp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->kvlp) {
	        rs1 = hdb_finish(op->kvlp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->klp) {
	        rs1 = vecobj_finish(op->klp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->flp) {
	        rs1 = vecobj_finish(op->flp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = kvsfile_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (kvsfile_close) */

int kvsfile_fileadd(kvsfile *op,cchar *atfname) noex {
	int		rs ;
	int		rs1 ;
	int		fi = 0 ;
	if ((rs = kvsfile_magic(op,atfname)) >= 0) {
	    rs = SR_INVALID ;
	    if (atfname[0]) {
		cchar	*fp ;
		if (absfn af ; (rs = af.start(atfname,0,&fp)) >= 0) {
	            if (KF_FILE	fe ; (rs = file_start(&fe,fp)) >= 0) {
			cnullptr	np{} ;
	                vecobj		*flp = op->flp ;
		        vecobj_vcf	vcf = vecobj_vcf(vcmpfname) ;
	                cint		nrs = SR_NOTFOUND ;
	                if ((rs = flp->search(&fe,vcf,np)) == nrs) {
	                    if ((rs = flp->add(&fe)) >= 0) {
	                        fi = rs ;
	                        rs = kvsfile_fparse(op,fi) ;
	                        if (rs < 0) {
	                            flp->del(fi) ;
				}
	                    } /* end if (vecobj_add) */
	                    if (rs < 0) {
	                        file_finish(&fe) ;
			    }
	                } else {
	                    fi = rs ;
	                    file_finish(&fe) ;
	                }
	            } /* end if (file-start) */
		    rs1 = af.finish ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (absfn) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? fi : rs ;
}
/* end subroutine (kvsfile_fileadd) */

int kvsfile_curbegin(kvsfile *op,KF_CUR *curp) noex {
	int		rs ;
	if ((rs = kvsfile_magic(op,curp)) >= 0) {
	    rs = SR_NOMEM ;
	    if ((curp->ecp = new(nothrow) hdb_cur) != nullptr) {
	        curp->i = -1 ;
	        rs = hdb_curbegin(op->elp,curp->ecp) ;
	    } /* end if (new-hdb_cur) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (kvsfile_curbegin) */

int kvsfile_curend(kvsfile *op,KF_CUR *curp) noex {
	int		rs ;
	if ((rs = kvsfile_magic(op,curp)) >= 0) {
	    rs = SR_BUGCHECK ;
	    if (curp->ecp) {
		{
	            curp->i = -1 ;
	            rs = hdb_curend(op->elp,curp->ecp) ;
		}
		{
		    delete curp->ecp ;
		    curp->ecp = nullptr ;
		}
	    } /* end if (non-null) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (kvsfile_curend) */

int kvsfile_curenumkey(kvsfile *op,KF_CUR *curp,char *kbuf,int klen) noex {
	int		rs ;
	int		kl = 0 ;
	if ((rs = kvsfile_magic(op,curp,kbuf)) >= 0) {
	    int		oi = (curp->i >= 0) ? (curp->i + 1) : 0 ;
	    void	*vp{} ;
	    forever {
	        rs = vecobj_get(op->klp,oi,&vp) ;
	        if (rs < 0) break ;
	        if (vp) break ;
	        oi += 1 ;
	    } /* end forever */
	    if ((rs >= 0) && vp) {
	        KF_KEY	*kep = cast_static<keyp>(vp) ;
	        rs = sncpy1(kbuf,klen,kep->kname) ;
	        kl = rs ;
	        curp->i = oi ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? kl : rs ;
}
/* end subroutine (kvsfile_curenumkey) */

int kvsfile_curenum(kvsfile *op,KF_CUR *curp,
		char *kbuf,int klen,char *vbuf,int vlen) noex {
	int		rs ;
	int		kl = 0 ;
	if ((rs = kvsfile_magic(op,curp,kbuf)) >= 0) {
	    hdb_datum	key ;
	    hdb_datum	val ;
	    hdb_cur	*ecp = curp->ecp ;
	    if ((rs = hdb_curenum(op->elp,ecp,&key,&val)) >= 0) {
	        cchar	*kp = charp(key.buf) ;
	        cchar	*vp ;
		int	vl ;
	        kl = key.len ;
	        rs = snwcpy(kbuf,klen,kp,kl) ;
	        if ((rs >= 0) && (vbuf != nullptr)) {
	            KF_ENT	*ep = entp(val.buf) ;
	            vp = ep->vname ;
	            vl = ep->vlen ;
	            rs = snwcpy(vbuf,vlen,vp,vl) ;
	        } /* end if (wanted the value also) */
	    } /* end if (had an entry) */
	} /* end if (magic) */
	return (rs >= 0) ? kl : rs ;
}
/* end subroutine (kvsfile_curenum) */

int kvsfile_fetch(KF *op,cc *kbuf,KF_CUR *curp,char *vbuf,int vlen) noex {
	int		rs ;
	int		vl = 0 ;
	if ((rs = kvsfile_magic(op,curp,kbuf)) >= 0) {
	    hdb_datum	key ;
	    hdb_datum	val{} ;
	    hdb_cur	*ecp = curp->ecp ;
	    int		kl ;
	    cchar	*kp = kbuf ;
	    cchar	*vp ;
	    if (kbuf[0] == '\0') kp = "default" ;
	    kl = lenstr(kp) ;
	    key.buf = kp ;
	    key.len = kl ;
	    if ((rs = hdb_fetch(op->elp,key,ecp,&val)) >= 0) {
	        KF_ENT	*ep = entp(val.buf) ;
	        vp = ep->vname ;
	        vl = ep->vlen ;
	        if (vbuf) {
	            rs = snwcpy(vbuf,vlen,vp,vl) ;
	        }
	    } /* end if (had an entry) */
	} /* end if (magic) */
	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (kvsfile_fetch) */

int kvsfile_check(kvsfile *op,time_t dt) noex {
	int		rs ;
	int		f = false ;
	if ((rs = kvsfile_magic(op)) >= 0) {
	    if (dt == 0) dt = time(nullptr) ;
	    if ((dt - op->ti_check) > KF_CHECKTIME) {
	        f = true ;
	        rs = kvsfile_checkfiles(op,dt) ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (kvsfile_check) */


/* private subroutines */

static int kvsfile_keyfins(kvsfile *op) noex {
	vecobj		*klp = op->klp ;
	int		rs = SR_OK ;
	int		rs1 ;
	void		*vp{} ;
	for (int i = 0 ; klp->get(i,&vp) >= 0 ; i += 1) {
	    if (vp) {
		KF_KEY	*kep = cast_static<keyp>(vp) ;
	        rs1 = key_finish(kep) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end for */
	return rs ;
}
/* end subroutine (kvsfile_keyfins) */

static int kvsfile_filefins(kvsfile *op) noex {
	vecobj		*flp = op->flp ;
	int		rs = SR_OK ;
	int		rs1 ;
	void		*vp{} ;
	for (int i = 0 ; flp->get(i,&vp) >= 0 ; i += 1) {
	    if (vp) {
		KF_FILE	*fep = cast_static<filep>(vp) ;
	        rs1 = file_finish(fep) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end for */
	return rs ;
}
/* end subroutine (kvsfile_filefins) */

static int kvsfile_checkfiles(kvsfile *op,time_t dt) noex {
	vecobj		*flp = op->flp ;
	ustat		sb ;
	int		rs = SR_OK ;
	int		c_changed = 0 ;
	void		*vp{} ;
	for (int i = 0 ; flp->get(i,&vp) >= 0 ; i += 1) {
	    if (vp) {
	        KF_FILE	*fep = cast_static<filep>(vp) ;
		bool	f = true ;
	        f = f && (u_stat(fep->fname,&sb) >= 0) ;
		f = f && (sb.st_mtime > fep->mtime) ;
		if (f) {
	            kvsfile_filedump(op,i) ;
	            if ((rs = kvsfile_fparse(op,i)) >= 0) {
	                c_changed += 1 ;
		    }
	        } /* end if */
	    } /* end if */
	    if (rs < 0) break ;
	} /* end for */
	op->ti_check = dt ;
	return (rs >= 0) ? c_changed : rs ;
}
/* end subroutine (kvsfile_checkfiles) */

static int kvsfile_fh(kvsfile *op,dev_t dev,ino_t ino) noex {
	vecobj		*flp = op->flp ;
	int		rs ;
	void		*vp{} ;
	for (int i = 0 ; (rs = flp->get(i,&vp)) >= 0 ; i += 1) {
	    if (vp) {
		KF_FILE	*fep = cast_static<filep>(vp) ;
	        if ((fep->dev == dev) && (fep->ino == ino)) break ;
	    }
	} /* end for */
	return rs ;
}
/* end subroutine (kvsfile_fh) */

static int kvsfile_fparse(kvsfile *op,int fi) noex {
	vecobj		*flp = op->flp ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (void *vp ; (rs = flp->get(fi,&vp)) >= 0) {
	    if (vp) {
		KF_FILE		*fep = cast_static<filep>(vp) ;
	        if (ucstream kf ; (rs = kf.open(fep->fname,"r")) >= 0) {
	            if (ustat sb ; (rs = kf.stat(&sb)) >= 0) {
	                if (! S_ISDIR(sb.st_mode)) {
	                    if (sb.st_mtime > fep->mtime) {
	                        cint		nrs = SR_NOTFOUND ;
	                        const dev_t	dev = sb.st_dev ;
	                        const ino_t	ino = sb.st_ino ;
	                        if ((rs = kvsfile_fh(op,dev,ino)) == nrs) {
	                            fep->dev = dev ;
	                            fep->ino = ino ;
	                            fep->mtime = sb.st_mtime ;
	                            fep->fsize = intsat(sb.st_size) ;
	                            rs = kvsfile_fparser(op,fi,&kf) ;
	                            c = rs ;
	                            if (rs < 0) {
	                                kvsfile_filedump(op,fi) ;
				    }
	                        } /* end if (adding file) */
	                    } /* end if (not previously added) */
	                } else {
	                    rs = SR_ISDIR ;
			}
	            } /* end if (bstat) */
	            rs1 = kf.close ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (file) */
	    } else {
	        rs = SR_NOTFOUND ;
	    }
	} /* end if (vec-get) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (kvsfile_fparse) */

static int kvsfile_fparser(kvsfile *op,int fi,ucstream *kfp) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (char *lbuf ; (rs = malloc_ml(&lbuf)) >= 0) {
	    cint	llen = rs ;
	    while ((rs = kfp->readln(lbuf,llen)) > 0) {
		cchar	*lp{} ;
		if (int ll ; (ll = sfcontent(lbuf,rs,&lp)) > 0) {
		    rs = kvsfile_fparsel(op,fi,lp,ll) ;
		    c += rs ;
		}
		if (rs < 0) break ;
	    } /* end while (reading lines) */
	    rs1 = uc_free(lbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (kvsfile_fparser) */

static int kvsfile_fparsel(kvsfile *op,int fi,cc *lp,int ll) noex {
	int		rs ;
	int		rs1 ;
	int		c_added = 0 ;
	if (field fsb ; (rs = field_start(&fsb,lp,ll)) >= 0) {
	    KF_KEY	*kep = nullptr ;
	    KF_ENT	ve ;
	    int		ki ;
	    int		fl ;
	    int		c_field = 0 ;
	    int		c = 0 ;
	    cchar	*fp{} ;
	    char	keybuf[KEYBUFLEN + 1] = {} ;
	    while ((fl = field_get(&fsb,ta.terms,&fp)) >= 0) {
                if ((c_field++ == 0) && (fsb.term == ':')) {
                    c = 0 ;
                    strwcpy(keybuf,fp,min(fl,KEYBUFLEN)) ;
                } else if ((fl > 0) && (keybuf[0] != '\0')) {
                    /* enter key into string table (if first time) */
                    if (c++ == 0) {
                        rs = kvsfile_getkeyp(op,keybuf,&kep) ;
                        ki = rs ;
                    } /* end if (entering key) */
                    if ((rs >= 0) && (kep != nullptr)) {
                        bool        f = true ;
                        if ((rs = entry_start(&ve,fi,ki,kep,fp,fl)) >= 0) {
                            cint    nrs = SR_NOTFOUND ;
                            if ((rs = kvsfile_already(op,&ve)) == nrs) {
                                if ((rs = kvsfile_addentry(op,&ve)) >= 0) {
                                    f = false ;
                                    c_added += 1 ;
                                }
                            } /* end if (new entry) */
                            if (f) entry_finish(&ve) ;
                        } /* end if (entry initialized) */
                    } /* end if */
                } /* end if (handling record) */
                if (fsb.term == '#') break ;
                if (rs < 0) break ;
	    } /* end while (fields) */
	    rs1 = field_finish(&fsb) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (field) */
	return (rs >= 0) ? c_added : rs ;
}
/* end subroutine (kvsfile_fparsel) */

static int kvsfile_getkeyp(kvsfile *op,cchar *kbuf,KF_KEY **kpp) noex {
	KF_KEY		*kep = nullptr ;
	int		rs ;
	int		ki = 0 ;
	if (KF_KEY ke ; (rs = key_start(&ke,kbuf)) >= 0) {
	    vecobj	*klp = op->klp ;
	    vecobj_vcf	vcf = vecobj_vcf(vcmpkey) ;
	    cint	nrs = SR_NOTFOUND ;
	    bool	f = true ;
	    if (void *vp ; (rs = klp->search(&ke,vcf,&vp)) == nrs) {
	        kep = nullptr ;
	        if ((rs = klp->add(&ke)) >= 0) {
	            ki = rs ;
	            f = false ;
	            if ((rs = klp->get(ki,&vp)) >= 0) {
			kep = cast_static<keyp>(vp) ;
		    }
	        } /* end if */
	    } else if (rs >= 0) {
		kep = cast_static<keyp>(vp) ;
	        ki = rs ;
	    }
	    if (f) {
	        key_finish(&ke) ;
	    }
	} /* end if (key-start) */
	if (kpp) {
	    *kpp = (rs >= 0) ? kep : nullptr ;
	}
	return (rs >= 0) ? ki : rs ;
}
/* end subroutine (kvsfile_getkeyp) */

static int kvsfile_already(kvsfile *op,KF_ENT *nep) noex {
	hdb_datum	key ;
	hdb_datum	val{} ;
	int		rs = SR_OK ;
	key.buf = nep ;
	key.len = szof(KF_ENT) ;
#ifdef	COMMENT
	{
	    hdb_cur	cur ;
	    hdb_curbegin(op->elp,&cur) ;
	    while (hdb_fetch(op->elp,key,&cur,&val) >= 0) {
	        KF_ENT	*ep = cast_static<entp>(val.buf) ;
	        f = (strcmp(nep->vname,ep->vname) == 0) ;
	        if (f) break ;
	    } /* end while */
	    hdb_curend(op->elp,&cur) ;
	    rs = (f) ? SR_OK : SR_NOTFOUND ;
	}
#else /* COMMENT */
	rs = hdb_fetch(op->kvlp,key,nullptr,&val) ;
#endif /* COMMENT */
	return rs ;
}
/* end subroutine (kvsfile_already) */

static int kvsfile_addentry(kvsfile *op,KF_ENT *nep) noex {
	cint		esz = szof(KF_ENT) ;
	int		rs ;
	if (void *vp ; (rs = uc_malloc(esz,&vp)) >= 0) {
	    KF_ENT	*ep = (KF_ENT *) vp ;
	    KF_KEY	*kep ;
	    hdb_datum	key ;
	    hdb_datum	val{} ;
	    *ep = *nep ;
	    kep = ep->kep ;
	    key.buf = ep ;
	    key.len = szof(KF_ENT) ;
	    val.buf = ep ;
	    val.len = szof(KF_ENT) ;
	    if ((rs = hdb_store(op->kvlp,key,val)) >= 0) {
	        key.buf = kep->kname ;
	        key.len = lenstr(kep->kname) ;
	        val.buf = ep ;
	        val.len = szof(KF_ENT) ;
	        rs = hdb_store(op->elp,key,val) ;
	        if (rs < 0) {
	            hdb_cur	cur ;
	            hdb_curbegin(op->kvlp,&cur) ;
	            {
	                key.buf = ep ;
	                key.len = szof(KF_ENT) ;
	                if (hdb_fetch(op->kvlp,key,&cur,&val) >= 0) {
	                    hdb_curdel(op->kvlp,&cur,0) ;
			}
	            }
	            hdb_curend(op->kvlp,&cur) ;
	        } /* end if (bad) */
	    } /* end if (keyvals-store) */
	    if (rs < 0) {
	        uc_free(ep) ;
	    }
	} /* end if (memory-allocation) */
	return rs ;
}
/* end subroutine (kvsfile_addentry) */

static int kvsfile_filedump(kvsfile *op,int fi) noex {
	KF_ENT		*ep ;
	hdb_datum	key ;
	hdb_datum	val{} ;
	int		rs ;
	int		rs1 ;
	if (hdb_cur cur ; (rs = hdb_curbegin(op->kvlp,&cur)) >= 0) {
	    while (hdb_curenum(op->kvlp,&cur,&key,&val) >= 0) {
	        ep = entp(val.buf) ;
	        if ((ep->fi == fi) || (fi < 0)) {
	            hdb_curdel(op->kvlp,&cur,0) ;
	        } /* end if (found matching entry) */
	    } /* end while (looping through entries) */
	    rs1 = hdb_curend(op->kvlp,&cur) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (cursor) */
	/* delete all entries w/ this KF_FILE */
	if (rs >= 0) {
	    if (hdb_cur cur ; (rs = hdb_curbegin(op->elp,&cur)) >= 0) {
	        while (hdb_curenum(op->elp,&cur,&key,&val) >= 0) {
	            ep = entp(val.buf) ;
	            if ((ep->fi == fi) || (fi < 0)) {
			{
	                    rs1 = hdb_curdel(op->elp,&cur,0) ;
	                    if (rs >= 0) rs = rs1 ;
			}
			{
	                    rs1 = entry_finish(ep) ;
	                    if (rs >= 0) rs = rs1 ;
			}
			{
	                    rs1 = uc_free(ep) ;
	                    if (rs >= 0) rs = rs1 ;
			}
	            } /* end if (found matching entry) */
	        } /* end while (looping through entries) */
	        rs1 = hdb_curend(op->elp,&cur) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (cursor) */
	} /* end if (ok) */
	return rs ;
}
/* end subroutine (kvsfile_filedump) */

#ifdef	COMMENT
/* delete a KF_FILE from the list of files (that is all) */
static int kvsfile_filedel(kvsfile *op,int fi) noex {
    	vecobj		*flp = op->flp ;
	int		rs ;
	if (void *vp ; (rs = flp->get(fi,&vp)) >= 0) {
	    if (vp) {
		KF_FILE	*fep = cast_static<filep>(vp) ;
	        file_finish(fep) ;
	    }
	    rs = flp->del(fi) ;
	}
	return rs ;
}
/* end subroutine (kvsfile_filedel) */
#endif /* COMMENT */

static int file_start(KF_FILE *fep,cchar *fname) noex {
	int		rs = SR_FAULT ;
	if (fname) {
	    memclear(fep) ;
	    if (cchar *cp ; (rs = uc_mallocstrw(fname,-1,&cp)) >= 0) {
	        fep->fname = cp ;
	    } /* end if (m-a) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (file_start) */

static int file_finish(KF_FILE *fep) noex {
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

static int key_start(KF_KEY *kep,cchar *kname) noex {
	int		rs = SR_FAULT ;
	if (kep) {
	    rs = memclear(kep) ;
	    if (kname) {
	        cint	klen = KEYBUFLEN ;
	        if (cchar *cp{} ; (rs = uc_mallocstrw(kname,klen,&cp)) >= 0) {
	            kep->kname = cp ;
	        }
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (key_start) */

static int key_finish(KF_KEY *kep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (kep) {
	    rs = SR_OK ;
	    if (kep->kname) {
	        rs1 = uc_free(kep->kname) ;
	        if (rs >= 0) rs = rs1 ;
	        kep->kname = nullptr ;
	    }
	    kep->count = 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (key_finish) */

static int key_increment(KF_KEY *kep) noex {
	int		rs = SR_FAULT ;
	if (kep) {
	    rs = ++kep->count ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (key_increment) */

static int key_decrement(KF_KEY *kep) noex {
	int		rs = SR_FAULT ;
	if (kep) {
	    if (kep->count > 0) {
	        kep->count -= 1 ;
	    }
	    rs = kep->count ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (key_decrement) */

static int entry_start(KF_ENT *ep,int fi,int ki,
		KF_KEY *kep,cchar *vp,int vl) noex {
	int		rs = SR_FAULT ;
	if (ep && kep && vp) {
	    memclear(ep) ;
	    ep->fi = fi ;
	    ep->ki = ki ;
	    ep->kep = kep ;
	    ep->vlen = vl ;
	    if (cchar *cp{} ; (rs = uc_mallocstrw(vp,vl,&cp)) >= 0) {
	        ep->vname = cp ;
	        key_increment(kep) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (entry_start) */

static int entry_finish(KF_ENT *ep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rc = 0 ;
	if (ep) {
	    rs = SR_OK ;
	    if (ep->vname) {
		cint	nk = key_decrement(ep->kep) ;
	        {
	            rs1 = uc_free(ep->vname) ;
	            if (rs >= 0) rs = rs1 ;
	            ep->vname = nullptr ;
	        }
	        rc = (nk == 0) ? ep->ki : INT_MAX ;
	    } /* end if (occupied) */
	} /* end if (non-null) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (entry_finish) */

static int vcmpfname(cvoid **v1pp,cvoid **v2pp) noex {
	KF_FILE		*e1p = (KF_FILE *) *v1pp ;
	KF_FILE		*e2p = (KF_FILE *) *v2pp ;
	int		rc = 0 ;
	if (e1p || e2p) {
	    if (e1p) {
	        if (e2p) {
	            rc = strcmp(e1p->fname,e2p->fname) ;
	        } else {
	            rc = -1 ;
		}
	    } else {
	       rc = +1 ;
	    }
	}
	return rc ;
}
/* end subroutine (vcmpfname) */

static int vcmpkey(cvoid **v1pp,cvoid **v2pp) noex {
	KF_KEY		*e1p = (KF_KEY *) *v1pp ;
	KF_KEY		*e2p = (KF_KEY *) *v2pp ;
	int		rc = 0 ;
	if (e1p || e2p) {
	    if (e1p) {
	        if (e2p) {
	            rc = strcmp(e1p->kname,e2p->kname) ;
	        } else {
	            rc = -1 ;
		}
	    } else {
	        rc = +1 ;
	    }
	}
	return rc ;
}
/* end subroutine (vcmpkey) */

static int cmpkeyval(cvoid *v1p,cvoid *v2p,int) noex {
	KF_ENT		*e1p = (KF_ENT *) v1p ;
	KF_ENT		*e2p = (KF_ENT *) v2p ;
	int		rc ;
	if ((rc = strcmp(e1p->kep->kname,e2p->kep->kname)) == 0) {
	    rc = strcmp(e1p->vname,e2p->vname) ;
	}
	return rc ;
}
/* end subroutine (cmpkeyval) */

static uint hashkeyval(cvoid *vp,int) noex {
	KF_ENT		*ep = entp(vp) ;
	uint		hv = 0 ;
	{
	    KF_KEY	*kep = ep->kep ;
	    hv += hash_elf(kep->kname,-1) ;
	    hv += hash_elf(ep->vname,-1) ;
	}
	return hv ;
}
/* end subroutine (hashkeyval) */


