/* kvsfile SUPPORT */
/* lang=C++20 */

/* perform access table kf_file related functions */
/* version %I% last-modified %G% */

#define	CF_PWDINIT	0		/* initialize PWD on open */
#define	CF_CHDIR	1		/* allow for changed directories */
#define	CF_DEVINO	1		/* check device-inode */

/* revision history:

	= 1998-06-01, David A­D­ Morano
	This object was originally written.

	- 2004-05-25, David A­D­ Morano
	This subroutine was adopted for use as a general key-value
	kf_file reader.

*/

/* Copyright © 1998,2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This object processes an access table for use by daemon
	multiplexing server programs that want to control access
	to their sub-servers.

	Implementation note:
	We let stale keys stay around. They are not lost, just not
	freed when no longer needed. There is no memory leak as
	they are all freed when the object is deconstructed. Stale
	keys do sort of serve as a ready key cache for those cases
	when they may be need later on with future entries!

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstdlib>
#include	<cstring>		/* <- for |strlen(3c)| */
#include	<ctime>
#include	<algorithm>		/* <- for |min(3c++)| */
#include	<netdb.h>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<getpwd.h>
#include	<bfile.h>
#include	<vecobj.h>
#include	<hdb.h>
#include	<field.h>
#include	<hash.h>
#include	<sncpyx.h>
#include	<snwcpy.h>
#include	<mkpathx.h>
#include	<strwcpy.h>
#include	<sfx.h>
#include	<char.h>
#include	<localmisc.h>

#include	"kvsfile.h"


/* local defines */

#define	KVSFILE_MINCHECKTIME	5	/* kf_file check interval (seconds) */
#define	KVSFILE_CHECKTIME	60	/* kf_file check interval (seconds) */
#define	KVSFILE_CHANGETIME	3	/* wait change interval (seconds) */
#define	KVSFILE_DEFNETGROUP	"DEFAULT"

#define	KVSFILE_FILE		struct kvsfile_file
#define	KVSFILE_KEY		struct kvsfile_key
#define	KVSFILE_ENT		struct kvsfile_ent

#ifdef	KVSFILE_KEYLEN
#define	KEYBUFLEN		KVSFILE_KEYLEN
#else
#define	KEYBUFLEN		MAXHOSTNAMELEN
#endif


/* local namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */


/* local typedefs */

typedef kvsfile		kf ;
typedef kvsfile_cur	kf_cur ;


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
	int		size ;
} ;

struct kvsfile_key {
	cchar		*kname ;
	int		count ;
} ;

struct kvsfile_ent {
	KVSFILE_KEY	*kep ;
	cchar		*vname ;
	int		vlen ;
	int		fi ;		/* kf_file index */
	int		ki ;		/* key index */
} ;

typedef kvsfile_file	kf_file ;
typedef kvsfile_key	kf_key ;
typedef kvsfile_ent	kf_ent ;

typedef kvsfile_file *	filep ;
typedef kvsfile_key *	keyp ;
typedef kvsfile_ent *	entp ;


/* forward references */

template<typename ... Args>
static int kvsfile_ctor(kvsfile *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = SR_OK ;
	    op->magic = 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (kvsfile_ctor) */

static int kvsfile_dtor(kvsfile *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (kvsfile_dtor) */

template<typename ... Args>
static int kvsfile_magic(kvsfile *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == KVSFILE_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (kvsfile_magic) */

static int	kvsfile_filefins(kvsfile *) noex ;
static int	kvsfile_keyfins(kvsfile *) noex ;
static int	kvsfile_fh(kvsfile *,dev_t,ino_t) noex ;
static int	kvsfile_fparse(kvsfile *,int) noex ;
static int	kvsfile_fparser(kvsfile *,int,bfile *) noex ;
static int	kvsfile_fparsel(kvsfile *,int,cc *,int) noex ;
static int	kvsfile_getkeyp(kvsfile *,cchar *,kf_key **) noex ;
static int	kvsfile_filedump(kvsfile *,int) noex ;
static int	kvsfile_addentry(kvsfile *,kf_ent *) noex ;
static int	kvsfile_already(kvsfile *,kf_ent *) noex ;
static int	kvsfile_checkfiles(kvsfile *,time_t) noex ;

#ifdef	COMMENT
static int	kvsfile_filedel(kvsfile *,int) noex ;
#endif

static int	file_start(kf_file *,cchar *) noex ;
static int	file_finish(kf_file *) noex ;

static int	key_start(kf_key *,cchar *) noex ;
static int	key_increment(kf_key *) noex ;
static int	key_decrement(kf_key *) noex ;
static int	key_finish(kf_key *) noex ;

static int entry_start(kf_ent *,int,int,kf_key *,cchar *,int) noex ;
static int entry_finish(kf_ent *) noex ;

static int	cmpfname(kf_file **,kf_file **) noex ;
static int	cmpkey(kf_key **,KVSFILE_KEY **) noex ;
static int	cmpkeyval(kf_ent *,kf_ent *,int) noex ;

static uint	hashkeyval(kf_ent *,int) noex ;


/* local variables */

/* all white space, pound ('#'), colon (':'), and comma (',') */
static constexpr cchar	fterms[] = {
	0x00, 0x1F, 0x00, 0x00,
	0x09, 0x10, 0x00, 0x04,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ;


/* exported variables */


/* exported subroutines */

int kvsfile_open(kvsfile *op,int ndef,cchar *atfname) noex {
	int		rs ;
	if ((rs = kvsfile_ctor(op)) >= 0) {
	    cint	fsz = sizeof(kf_file) ;
	    cint	vo = (VECOBJ_OSTATIONARY | VECOBJ_OREUSE) ;
	    if (ndef < KVSFILE_DEFENTS) ndef = KVSFILE_DEFENTS ;
	    if ((rs = vecobj_start(&op->files,fsz,ndef,vo)) >= 0) {
	        cint	n = (ndef / 10) ;
	        cint	ksz = sizeof(kf_key) ;
	        if ((rs = vecobj_start(&op->keys,ksz,n,vo)) >= 0) {
		    hdbcmp_f	cf = hdbcmp_f(cmpkeyval) ;
		    hdbhash_f	hf = hdbhash_f(hashkeyval) ;
	            if ((rs = hdb_start(&op->keyvals,ndef,0,hf,cf)) >= 0) {
		        const nullptr_t	np{} ;
		        hdb		*elp = &op->entries ;
	                if ((rs = hdb_start(elp,ndef,0,np,np)) >= 0) {
	                    op->magic = KVSFILE_MAGIC ;
	                    op->ti_check = time(nullptr) ;
	                    if (atfname && (atfname[0] != '\0')) {
	                        rs = kvsfile_fileadd(op,atfname) ;
	                        if (rs < 0) {
	                            op->magic = 0 ;
	                        }
	                    } /* end if (adding first file) */
	                    if (rs < 0) {
	                        hdb_finish(&op->entries) ;
			    }
	                } /* end if (entries) */
	                if (rs < 0) {
	                    hdb_finish(&op->keyvals) ;
		        }
	            } /* end if (keyvals) */
	            if (rs < 0) {
	                vecobj_finish(&op->keys) ;
		    }
	        } /* end if (keys) */
	        if (rs < 0) {
	            vecobj_finish(&op->files) ;
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
	    {
	        rs1 = hdb_finish(&op->entries) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = hdb_finish(&op->keyvals) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = vecobj_finish(&op->keys) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = vecobj_finish(&op->files) ;
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
	int		fi = 0 ;
	if ((rs = kvsfile_magic(op,atfname)) >= 0) {
	    rs = SR_INVALID ;
	    if (atfname[0]) {
	        cchar		*fp = atfname ;
	        char		abuf[MAXPATHLEN + 1] ;
	        if (atfname[0] != '/') {
	            cint	plen = MAXPATHLEN ;
	            char	pbuf[MAXPATHLEN+1] ;
	            if ((rs = getpwd(pbuf,plen)) >= 0) {
	                rs = mkpath2(abuf,pbuf,atfname) ;
	                fp = abuf ;
	            }
	        } /* end if (added PWD) */
	        if (rs >= 0) {
	            kf_file	fe ;
	            if ((rs = file_start(&fe,fp)) >= 0) {
	                vecobj		*flp = &op->files ;
		        vecobj_vcf	vcf = vecobj_vcf(cmpfname) ;
	                cint		nrs = SR_NOTFOUND ;
	                if ((rs = vecobj_search(flp,&fe,vcf,nullptr)) == nrs) {
	                    if ((rs = vecobj_add(&op->files,&fe)) >= 0) {
	                        fi = rs ;
	                        rs = kvsfile_fparse(op,fi) ;
	                        if (rs < 0) {
	                            vecobj_del(&op->files,fi) ;
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
	        } /* end if (ok) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? fi : rs ;
}
/* end subroutine (kvsfile_fileadd) */

int kvsfile_curbegin(kvsfile *op,kf_cur *curp) noex {
	int		rs ;
	if ((rs = kvsfile_magic(op,curp)) >= 0) {
	    curp->i = -1 ;
	    rs = hdb_curbegin(&op->entries,&curp->ec) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (kvsfile_curbegin) */

int kvsfile_curend(kvsfile *op,kf_cur *curp) noex {
	int		rs ;
	if ((rs = kvsfile_magic(op,curp)) >= 0) {
	    curp->i = -1 ;
	    rs = hdb_curend(&op->entries,&curp->ec) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (kvsfile_curend) */

int kvsfile_curenumkey(kvsfile *op,kf_cur *curp,char *kbuf,int klen) noex {
	int		rs ;
	int		kl = 0 ;
	if ((rs = kvsfile_magic(op,curp,kbuf)) >= 0) {
	    int		oi = (curp->i >= 0) ? (curp->i + 1) : 0 ;
	    void	*vp{} ;
	    forever {
	        rs = vecobj_get(&op->keys,oi,&vp) ;
	        if (rs < 0) break ;
	        if (vp) break ;
	        oi += 1 ;
	    } /* end forever */
	    if ((rs >= 0) && vp) {
	        kf_key	*kep = static_cast<keyp>(vp) ;
	        rs = sncpy1(kbuf,klen,kep->kname) ;
	        kl = rs ;
	        curp->i = oi ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? kl : rs ;
}
/* end subroutine (kvsfile_curenumkey) */

int kvsfile_curenum(kvsfile *op,kf_cur *curp,
		char *kbuf,int klen,char *vbuf,int vlen) noex {
	int		rs ;
	int		kl = 0 ;
	if ((rs = kvsfile_magic(op,curp,kbuf)) >= 0) {
	    hdb_datum	key ;
	    hdb_datum	val ;
	    hdb_cur	cur = curp->ec ;
	    int		vl ;
	    if ((rs = hdb_enum(&op->entries,&cur,&key,&val)) >= 0) {
	        cchar	*kp = (cchar *) key.buf ;
	        cchar	*vp ;
	        kl = key.len ;
	        rs = snwcpy(kbuf,klen,kp,kl) ;
	        if ((rs >= 0) && (vbuf != nullptr)) {
	            kf_ent	*ep = (kf_ent *) val.buf ;
	            vp = ep->vname ;
	            vl = ep->vlen ;
	            rs = snwcpy(vbuf,vlen,vp,vl) ;
	        } /* end if (wanted the value also) */
	        if (rs >= 0) {
	            curp->ec = cur ;
	        }
	    } /* end if (had an entry) */
	} /* end if (magic) */
	return (rs >= 0) ? kl : rs ;
}
/* end subroutine (kvsfile_curenum) */

int kvsfile_fetch(kf *op,cc *kbuf,kf_cur *curp,char *vbuf,int vlen) noex {
	int		rs ;
	int		vl = 0 ;
	if ((rs = kvsfile_magic(op,curp,kbuf)) >= 0) {
	    hdb_datum	key ;
	    hdb_datum	val{} ;
	    hdb_cur	cur = curp->ec ;
	    int		kl ;
	    cchar	*kp = (cchar *) kbuf ;
	    cchar	*vp ;
	    if (kbuf[0] == '\0') kp = "default" ;
	    kl = strlen(kp) ;
	    key.buf = kp ;
	    key.len = kl ;
	    if ((rs = hdb_fetch(&op->entries,key,&cur,&val)) >= 0) {
	        kf_ent	*ep = (kf_ent *) val.buf ;
	        vp = ep->vname ;
	        vl = ep->vlen ;
	        if (vbuf) {
	            rs = snwcpy(vbuf,vlen,vp,vl) ;
	        }
	        if (rs >= 0) {
	            curp->ec = cur ;
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
	    if ((dt - op->ti_check) > KVSFILE_CHECKTIME) {
	        f = true ;
	        rs = kvsfile_checkfiles(op,dt) ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (kvsfile_check) */


/* private subroutines */

static int kvsfile_keyfins(kvsfile *op) noex {
	vecobj		*klp = &op->keys ;
	int		rs = SR_OK ;
	int		rs1 ;
	void		*vp{} ;
	for (int i = 0 ; vecobj_get(klp,i,&vp) >= 0 ; i += 1) {
	    if (vp) {
		kf_key	*kep = static_cast<keyp>(vp) ;
	        rs1 = key_finish(kep) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end for */
	return rs ;
}
/* end subroutine (kvsfile_keyfins) */

static int kvsfile_filefins(kvsfile *op) noex {
	vecobj		*flp = &op->files ;
	int		rs = SR_OK ;
	int		rs1 ;
	void		*vp{} ;
	for (int i = 0 ; vecobj_get(flp,i,&vp) >= 0 ; i += 1) {
	    if (vp) {
		kf_file	*fep = static_cast<filep>(vp) ;
	        rs1 = file_finish(fep) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end for */
	return rs ;
}
/* end subroutine (kvsfile_filefins) */

static int kvsfile_checkfiles(kvsfile *op,time_t dt) noex {
	vecobj		*flp = &op->files ;
	USTAT		sb ;
	int		rs = SR_OK ;
	int		c_changed = 0 ;
	void		*vp{} ;
	for (int i = 0 ; vecobj_get(flp,i,&vp) >= 0 ; i += 1) {
	    if (vp) {
	        kf_file	*fep = static_cast<filep>(vp) ;
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
	vecobj		*flp = &op->files ;
	int		rs ;
	void		*vp{} ;
	for (int i = 0 ; (rs = vecobj_get(flp,i,&vp)) >= 0 ; i += 1) {
	    if (vp) {
		kf_file	*fep = static_cast<filep>(vp) ;
	        if ((fep->dev == dev) && (fep->ino == ino)) break ;
	    }
	} /* end for */
	return rs ;
}
/* end subroutine (kvsfile_fh) */

static int kvsfile_fparse(kvsfile *op,int fi) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	void		*vp{} ;
	if ((rs = vecobj_get(&op->files,fi,&vp)) >= 0) {
	    if (vp) {
		kf_file		*fep = static_cast<filep>(vp) ;
	        bfile		kfile, *lfp = &kfile ;
	        if ((rs = bopen(lfp,fep->fname,"r",0664)) >= 0) {
	            USTAT	sb ;
	            if ((rs = bstat(lfp,&sb)) >= 0) {
	                if (! S_ISDIR(sb.st_mode)) {
	                    if (sb.st_mtime > fep->mtime) {
	                        cint		nrs = SR_NOTFOUND ;
	                        const dev_t	dev = sb.st_dev ;
	                        const ino_t	ino = sb.st_ino ;
	                        if ((rs = kvsfile_fh(op,dev,ino)) == nrs) {
	                            fep->dev = dev ;
	                            fep->ino = ino ;
	                            fep->mtime = sb.st_mtime ;
	                            fep->size = sb.st_size ;
	                            rs = kvsfile_fparser(op,fi,lfp) ;
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
	            rs1 = bclose(lfp) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (file) */
	    } else {
	        rs = SR_NOTFOUND ;
	    }
	} /* end if (vec-get) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (kvsfile_fparse) */

static int kvsfile_fparser(kvsfile *op,int fi,bfile *lfp) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	char		*lbuf{} ;
	if ((rs = malloc_ml(&lbuf)) >= 0) {
	    cint	llen = rs ;
	    while ((rs = breadln(lfp,lbuf,llen)) > 0) {
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
	field		fsb ;
	int		rs ;
	int		rs1 ;
	int		c_added = 0 ;
	if ((rs = field_start(&fsb,lp,ll)) >= 0) {
	    kf_key	*kep = nullptr ;
	    kf_ent	ve ;
	    int		ki ;
	    int		fl ;
	    int		c_field = 0 ;
	    int		c = 0 ;
	    cchar	*fp{} ;
	    char	keybuf[KEYBUFLEN + 1] = {} ;
	    while ((fl = field_get(&fsb,fterms,&fp)) >= 0) {
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
	                    bool	f = true ;
	                    if ((rs = entry_start(&ve,fi,ki,kep,fp,fl)) >= 0) {
	                        cint	nrs = SR_NOTFOUND ;
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

static int kvsfile_getkeyp(kvsfile *op,cchar *kbuf,kf_key **kpp) noex {
	kf_key		ke ;
	kf_key		*kep = nullptr ;
	int		rs ;
	int		ki = 0 ;
	if ((rs = key_start(&ke,kbuf)) >= 0) {
	    vecobj	*klp = &op->keys ;
	    vecobj_vcf	vcf = vecobj_vcf(cmpkey) ;
	    cint	nrs = SR_NOTFOUND ;
	    bool	f = true ;
	    void	*vp{} ;
	    if ((rs = vecobj_search(klp,&ke,vcf,&vp)) == nrs) {
	        kep = nullptr ;
	        if ((rs = vecobj_add(klp,&ke)) >= 0) {
	            ki = rs ;
	            f = false ;
	            if ((rs = vecobj_get(klp,ki,&vp)) >= 0) {
			kep = static_cast<keyp>(vp) ;
		    }
	        } /* end if */
	    } else if (rs >= 0) {
		kep = static_cast<keyp>(vp) ;
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
/* end subroutine (kvsfile_getlkeyp) */

static int kvsfile_already(kvsfile *op,kf_ent *nep) noex {
	hdb_datum	key ;
	hdb_datum	val{} ;
	int		rs = SR_OK ;
	key.buf = nep ;
	key.len = sizeof(kf_ent) ;
#ifdef	COMMENT
	{
	    hdb_cur	cur ;
	    hdb_curbegin(&op->entries,&cur) ;
	    while (hdb_fetch(&op->entries,key,&cur,&val) >= 0) {
	        kf_ent	*ep = static_cast<entp>(val.buf) ;
	        f = (strcmp(nep->vname,ep->vname) == 0) ;
	        if (f) break ;
	    } /* end while */
	    hdb_curend(&op->entries,&cur) ;
	    rs = (f) ? SR_OK : SR_NOTFOUND ;
	}
#else /* COMMENT */
	rs = hdb_fetch(&op->keyvals,key,nullptr,&val) ;
#endif /* COMMENT */
	return rs ;
}
/* end subroutine (kvsfile_already) */

static int kvsfile_addentry(kvsfile *op,kf_ent *nep) noex {
	kf_key		*kep ;
	kf_ent		*ep{} ;
	cint		esz = sizeof(kf_ent) ;
	int		rs ;
	if ((rs = uc_malloc(esz,&ep)) >= 0) {
	    hdb_datum	key ;
	    hdb_datum	val{} ;
	    *ep = *nep ;
	    kep = ep->kep ;
	    key.buf = ep ;
	    key.len = sizeof(kf_ent) ;
	    val.buf = ep ;
	    val.len = sizeof(kf_ent) ;
	    if ((rs = hdb_store(&op->keyvals,key,val)) >= 0) {
	        key.buf = kep->kname ;
	        key.len = strlen(kep->kname) ;
	        val.buf = ep ;
	        val.len = sizeof(kf_ent) ;
	        rs = hdb_store(&op->entries,key,val) ;
	        if (rs < 0) {
	            hdb_cur	cur ;
	            hdb_curbegin(&op->keyvals,&cur) ;
	            {
	                key.buf = ep ;
	                key.len = sizeof(kf_ent) ;
	                if (hdb_fetch(&op->keyvals,key,&cur,&val) >= 0) {
	                    hdb_delcur(&op->keyvals,&cur,0) ;
			}
	            }
	            hdb_curend(&op->keyvals,&cur) ;
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
	kf_ent		*ep ;
	hdb_cur		cur ;
	hdb_datum	key ;
	hdb_datum	val{} ;
	int		rs ;
	int		rs1 ;
	if ((rs = hdb_curbegin(&op->keyvals,&cur)) >= 0) {
	    while (hdb_enum(&op->keyvals,&cur,&key,&val) >= 0) {
	        ep = (kf_ent *) val.buf ;
	        if ((ep->fi == fi) || (fi < 0)) {
	            hdb_delcur(&op->keyvals,&cur,0) ;
	        } /* end if (found matching entry) */
	    } /* end while (looping through entries) */
	    rs1 = hdb_curend(&op->keyvals,&cur) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (cursor) */
/* delete all entries w/ this kf_file */
	if (rs >= 0) {
	    if ((rs = hdb_curbegin(&op->entries,&cur)) >= 0) {
	        while (hdb_enum(&op->entries,&cur,&key,&val) >= 0) {
	            ep = (kf_ent *) val.buf ;
	            if ((ep->fi == fi) || (fi < 0)) {
			{
	                    rs1 = hdb_delcur(&op->entries,&cur,0) ;
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
	        rs1 = hdb_curend(&op->entries,&cur) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (cursor) */
	} /* end if (ok) */
	return rs ;
}
/* end subroutine (kvsfile_filedump) */

#ifdef	COMMENT
/* delete a kf_file from the list of files (that is all) */
static int kvsfile_filedel(kvsfile *op,int fi) noex {
	int		rs ;
	void		*vp{} ;
	if ((rs = vecobj_get(&op->files,fi,&vp)) >= 0) {
	    if (vp) {
		kf_file	*fep = static_cast<filep>(vp) ;
	        file_finish(fep) ;
	    }
	    rs = vecobj_del(&op->files,fi) ;
	}
	return rs ;
}
/* end subroutine (kvsfile_filedel) */
#endif /* COMMENT */

static int file_start(kf_file *fep,cchar *fname) noex {
	int		rs = SR_FAULT ;
	if (fname) {
	    memclear(fep) ;
	    if (cchar *cp{} ; (rs = uc_mallocstrw(fname,-1,&cp)) >= 0) {
	        fep->fname = cp ;
	    } /* end if (m-a) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (file_start) */

static int file_finish(kf_file *fep) noex {
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

static int key_start(kf_key *kep,cchar *kname) noex {
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

static int key_finish(kf_key *kep) noex {
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

static int key_increment(kf_key *kep) noex {
	int		rs = SR_FAULT ;
	if (kep) {
	    rs = ++kep->count ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (key_increment) */

static int key_decrement(kf_key *kep) noex {
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

static int entry_start(kf_ent *ep,int fi,int ki,
		kf_key *kep,cchar *vp,int vl) noex {
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

static int entry_finish(kf_ent *ep) noex {
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

static int cmpfname(kf_file **e1pp,KVSFILE_FILE **e2pp) noex {
	int		rc = 0 ;
	if ((*e1pp != nullptr) || (*e2pp != nullptr)) {
	    if (*e1pp != nullptr) {
	        if (*e2pp != nullptr) {
	            rc = strcmp((*e1pp)->fname,(*e2pp)->fname) ;
	        } else
	            rc = -1 ;
	    } else
	       rc = 1 ;
	}
	return rc ;
}
/* end subroutine (cmpfname) */

static int cmpkey(kf_key **e1pp,kf_key **e2pp) noex {
	int		rc = 0 ;
	if ((*e1pp != nullptr) || (*e2pp != nullptr)) {
	    if (*e1pp != nullptr) {
	        if (*e2pp != nullptr) {
	            rc = strcmp((*e1pp)->kname,(*e2pp)->kname) ;
	        } else
	            rc = -1 ;
	    } else
	        rc = 1 ;
	}
	return rc ;
}
/* end subroutine (cmpkey) */

static int cmpkeyval(kf_ent *e1p,kf_ent *e2p,int) noex {
	int		rc ;
	if ((rc = strcmp(e1p->kep->kname,e2p->kep->kname)) == 0) {
	    rc = strcmp(e1p->vname,e2p->vname) ;
	}
	return rc ;
}
/* end subroutine (cmpkeyval) */

static uint hashkeyval(kf_ent *ep,int) noex {
	kf_key		*kep = ep->kep ;
	uint		hv = 0 ;
	hv += hash_elf(kep->kname,-1) ;
	hv += hash_elf(ep->vname,-1) ;
	return hv ;
}
/* end subroutine (hashkeyval) */


