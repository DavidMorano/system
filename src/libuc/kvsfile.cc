/* kvsfile SUPPORT */
/* lang=C++20 */

/* perform access table file related functions */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* non-switchable debug print-outs */
#define	CF_DEBUGHDB	0		/* eval HDB performance */
#define	CF_DEBUGFILE	1		/* debug file operations */
#define	CF_DEBUGLIST	1		/* debug list? */
#define	CF_SAFE		1		/* run safer */
#define	CF_PWDINIT	0		/* initialize PWD on open */
#define	CF_CHDIR	1		/* allow for changed directories */
#define	CF_DEVINO	1		/* check device-inode */

/* revision history:

	= 1998-06-01, David A­D­ Morano
	This object was originally written.

	- 2004-05-25, David A­D­ Morano
	This subroutine was adopted for use as a general key-value
	file reader.

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
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstdlib>
#include	<cstring>
#include	<ctime>
#include	<netdb.h>
#include	<usystem.h>
#include	<usupport.h>
#include	<getpwd.h>
#include	<bfile.h>
#include	<field.h>
#include	<vecobj.h>
#include	<hdb.h>
#include	<char.h>
#include	<hash.h>
#include	<sncpyx.h>
#include	<snwcpy.h>
#include	<mkpathx.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"kvsfile.h"


/* local defines */

#define	KVSFILE_MINCHECKTIME	5	/* file check interval (seconds) */
#define	KVSFILE_CHECKTIME	60	/* file check interval (seconds) */
#define	KVSFILE_CHANGETIME	3	/* wait change interval (seconds) */
#define	KVSFILE_DEFNETGROUP	"DEFAULT"

#define	KVSFILE_FILE		struct kvsfile_file
#define	KVSFILE_KEY		struct kvsfile_key
#define	KVSFILE_ENT		struct kvsfile_ent

#ifndef	LINEBUFLEN
#ifdef	LINE_MAX
#define	LINEBUFLEN		MAX(LINE_MAX,2048)
#else
#define	LINEBUFLEN		2048
#endif
#endif

#ifdef	KVSFILE_KEYLEN
#define	KEYBUFLEN		KVSFILE_KEYLEN
#else
#define	KEYBUFLEN		MAXHOSTNAMELEN
#endif

#if	CF_DEBUGS && CF_DEBUGHDB
#ifndef	HDBHASHTABCOUNTS
#define	HDBHASHTABCOUNTS	"hdbhashtabcounts"
#endif
#endif


/* local namespaces */

using std::nullptr_t ;			/* type */


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
	uino_t		ino ;
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
	int		fi ;		/* file index */
	int		ki ;		/* key index */
} ;

typedef kvsfile_file		file ;
typedef kvsfile_key		key ;
typedef kvsfile_ent		ent ;

typedef kvsfile_file *		filep ;
typedef kvsfile_key *		keyp ;
typedef kvsfile_ent *		entp ;


/* forward references */

static int	kvsfile_filefins(kvsfile *) noex ;
static int	kvsfile_keyfins(kvsfile *) noex ;
static int	kvsfile_fh(kvsfile *,dev_t,uino_t) noex ;
static int	kvsfile_fileparse(kvsfile *,int) noex ;
static int	kvsfile_fileparser(kvsfile *,int,bfile *) noex ;
static int	kvsfile_getkeyp(kvsfile *,cchar *,KVSFILE_KEY **) noex ;
static int	kvsfile_filedump(kvsfile *,int) noex ;
static int	kvsfile_addentry(kvsfile *,KVSFILE_ENT *) noex ;
static int	kvsfile_already(kvsfile *,KVSFILE_ENT *) noex ;
static int	kvsfile_checkfiles(kvsfile *,time_t) noex ;

#ifdef	COMMENT
static int	kvsfile_filedel(kvsfile *,int) noex ;
#endif

static int	file_start(file *,cchar *) noex ;
static int	file_finish(file *) noex ;

static int	key_start(KVSFILE_KEY *,cchar *) noex ;
static int	key_increment(KVSFILE_KEY *) noex ;
static int	key_decrement(KVSFILE_KEY *) noex ;
static int	key_finish(KVSFILE_KEY *) noex ;

static int entry_start(KVSFILE_ENT *,int,int,KVSFILE_KEY *,cchar *,int) noex ;
static int entry_finish(KVSFILE_ENT *) noex ;

static int	cmpfname(KVSFILE_FILE **,KVSFILE_FILE **) noex ;
static int	cmpkey(KVSFILE_KEY **,KVSFILE_KEY **) noex ;
static int	cmpkeyval(KVSFILE_ENT *,KVSFILE_ENT *,int) noex ;

static uint	hashkeyval(KVSFILE_ENT *,int) noex ;


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


/* exported subroutines */

int kvsfile_open(kvsfile *op,int ndef,cchar *atfname) noex {
	int		rs ;
	int		size ;
	int		opts ;

	if (op == nullptr) return SR_FAULT ;

	if (ndef < KVSFILE_DEFENTS) ndef = KVSFILE_DEFENTS ;

	memset(op,0,sizeof(KVSFILE)) ;

/* this vector structure must remain fixed so that indices do not change */

	size = sizeof(KVSFILE_FILE) ;
	opts = (VECOBJ_OSTATIONARY | VECOBJ_OREUSE) ;
	if ((rs = vecobj_start(&op->files,size,ndef,opts)) >= 0) {
	    cint	n = (ndef / 10) ;
	    cint	esz = sizeof(KVSFILE_KEY) ;
	    opts = (VECOBJ_OSTATIONARY | VECOBJ_OREUSE) ;
	    if ((rs = vecobj_start(&op->keys,esz,n,opts)) >= 0) {
		hdbcmp_f	cf = hdbcmp_f(cmpkeyval) ;
		hdbhash_f	hf = hdbhash_f(hashkeyval) ;
	        if ((rs = hdb_start(&op->keyvals,ndef,0,hf,cf)) >= 0) {
		    nullptr_t	np{} ;
	            if ((rs = hdb_start(&op->entries,ndef,0,np,np)) >= 0) {
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

	return rs ;
}
/* end subroutine (kvsfile_open) */

int kvsfile_close(kvsfile *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != KVSFILE_MAGIC) return SR_NOTOPEN ;

	rs1 = kvsfile_filedump(op,-1) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = kvsfile_keyfins(op) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = kvsfile_filefins(op) ;
	if (rs >= 0) rs = rs1 ;

/* free up the rest of the main object data */

	rs1 = hdb_finish(&op->entries) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = hdb_finish(&op->keyvals) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = vecobj_finish(&op->keys) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = vecobj_finish(&op->files) ;
	if (rs >= 0) rs = rs1 ;

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (kvsfile_close) */

int kvsfile_fileadd(kvsfile *op,cchar *atfname) noex {
	int		rs = SR_OK ;
	int		fi = 0 ;
	cchar		*np ;
	char		abuf[MAXPATHLEN + 1] ;

	if (op == nullptr) return SR_FAULT ;
	if (atfname == nullptr) return SR_FAULT ;

	if (op->magic != KVSFILE_MAGIC) return SR_NOTOPEN ;

	if (atfname[0] == '\0') return SR_INVALID ;

	np = atfname ;
	if (atfname[0] != '/') {
	    cint	plen = MAXPATHLEN ;
	    char	pbuf[MAXPATHLEN+1] ;

	    if ((rs = getpwd(pbuf,plen)) >= 0) {
	        rs = mkpath2(abuf,pbuf,atfname) ;
	        np = abuf ;
	    }

	} /* end if (added PWD) */

	if (rs >= 0) {
	    KVSFILE_FILE	fe ;
	    if ((rs = file_start(&fe,np)) >= 0) {
	        vecobj		*flp = &op->files ;
		vecobj_vcf	vcf = vecobj_vcf(cmpfname) ;
	        cint		nrs = SR_NOTFOUND ;
	        if ((rs = vecobj_search(flp,&fe,vcf,nullptr)) == nrs) {
	            if ((rs = vecobj_add(&op->files,&fe)) >= 0) {
	                fi = rs ;
	                rs = kvsfile_fileparse(op,fi) ;
	                if (rs < 0)
	                    vecobj_del(&op->files,fi) ;
	            } /* end if (vecobj_add) */
	            if (rs < 0)
	                file_finish(&fe) ;
	        } else {
	            fi = rs ;
	            file_finish(&fe) ;
	        }
	    } /* end if (file-start) */
	} /* end if (ok) */

	return (rs >= 0) ? fi : rs ;
}
/* end subroutine (kvsfile_fileadd) */

int kvsfile_curbegin(kvsfile *op,KVSFILE_CUR *curp) noex {
	int		rs ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magic != KVSFILE_MAGIC) return SR_NOTOPEN ;

	curp->i = -1 ;
	rs = hdb_curbegin(&op->entries,&curp->ec) ;

	return rs ;
}
/* end subroutine (kvsfile_curbegin) */

int kvsfile_curend(kvsfile *op,KVSFILE_CUR *curp) noex {
	int		rs ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magic != KVSFILE_MAGIC) return SR_NOTOPEN ;

	curp->i = -1 ;
	rs = hdb_curend(&op->entries,&curp->ec) ;

	return rs ;
}
/* end subroutine (kvsfile_curend) */

int kvsfile_curenumkey(kvsfile *op,KVSFILE_CUR *curp,char *kbuf,int klen) noex {
	int		rs = SR_OK ;
	int		oi ;
	int		kl = 0 ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;
	if (kbuf == nullptr) return SR_FAULT ;

	if (op->magic != KVSFILE_MAGIC) return SR_NOTOPEN ;

	oi = (curp->i >= 0) ? (curp->i + 1) : 0 ;

	void	*vp{} ;
	forever {
	    rs = vecobj_get(&op->keys,oi,&vp) ;
	    if (rs < 0) break ;
	    if (vp) break ;
	    oi += 1 ;
	} /* end forever */
	if ((rs >= 0) && vp) {
	    KVSFILE_KEY	*kep = static_cast<keyp>(vp) ;
	    rs = sncpy1(kbuf,klen,kep->kname) ;
	    kl = rs ;
	    curp->i = oi ;
	}

	return (rs >= 0) ? kl : rs ;
}
/* end subroutine (kvsfile_curenumkey) */

int kvsfile_curenum(kvsfile *op,KVSFILE_CUR *curp,
		char *kbuf,int klen,char *vbuf,int vlen) noex {
	hdb_datum	key ;
	hdb_datum	val ;
	hdb_cur		cur ;
	int		rs ;
	int		vl ;
	int		kl = 0 ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;
	if (kbuf == nullptr) return SR_FAULT ;

	if (op->magic != KVSFILE_MAGIC) return SR_NOTOPEN ;

	cur = curp->ec ;
	if ((rs = hdb_enum(&op->entries,&cur,&key,&val)) >= 0) {
	    KVSFILE_ENT	*ep ;
	    cchar	*kp = (cchar *) key.buf ;
	    cchar	*vp ;
	    kl = key.len ;
	    rs = snwcpy(kbuf,klen,kp,kl) ;
	    if ((rs >= 0) && (vbuf != nullptr)) {
	        ep = (KVSFILE_ENT *) val.buf ;
	        vp = ep->vname ;
	        vl = ep->vlen ;
	        rs = snwcpy(vbuf,vlen,vp,vl) ;
	    } /* end if (wanted the value also) */
	    if (rs >= 0) {
	        curp->ec = cur ;
	    }
	} /* end if (had an entry) */

	return (rs >= 0) ? kl : rs ;
}
/* end subroutine (kvsfile_curenum) */

int kvsfile_fetch(kf *op,cc *kbuf,kf_cur *curp,char *vbuf,int vlen) noex {
	KVSFILE_ENT	*ep ;
	hdb_datum	key ;
	hdb_datum	val{} ;
	hdb_cur		cur ;
	int		rs ;
	int		kl ;
	int		vl = 0 ;
	cchar		*kp, *vp ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;
	if (kbuf == nullptr) return SR_FAULT ;

	if (op->magic != KVSFILE_MAGIC) return SR_NOTOPEN ;

	kp = (cchar *) kbuf ;
	if (kbuf[0] == '\0') kp = "default" ;

	kl = strlen(kp) ;

#if	CF_DEBUGS
	debugprintf("kvsfile_fetch: key=%s\n",kp) ;
#endif

#if	CF_DEBUGS && CF_DEBUGHDB
	{
	    int	rs1 ;
	    rs1 = hdb_audit(&op->entries) ;
	    debugprintf("kvsfile_fetch: hdb_audit() rs=%d\n",rs1) ;
	    hdb_debugdump(&op->entries) ;
	}
#endif

	key.buf = kp ;
	key.len = kl ;
	cur = curp->ec ;
	if ((rs = hdb_fetch(&op->entries,key,&cur,&val)) >= 0) {
	    ep = (KVSFILE_ENT *) val.buf ;
	    vp = ep->vname ;
	    vl = ep->vlen ;
	    if (vbuf) {
	        rs = snwcpy(vbuf,vlen,vp,vl) ;
	    }
	    if (rs >= 0) {
	        curp->ec = cur ;
	    }
	} /* end if (had an entry) */

#if	CF_DEBUGS
	debugprintf("kvsfile_fetch: ret rs=%d vl=%u\n",rs,vl) ;
#endif

	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (kvsfile_fetch) */

int kvsfile_check(kvsfile *op,time_t daytime) noex {
	int		rs = SR_OK ;
	int		f = false ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != KVSFILE_MAGIC) return SR_NOTOPEN ;

	if (daytime == 0) daytime = time(nullptr) ;

/* should we even check? */

	if ((daytime - op->ti_check) > KVSFILE_CHECKTIME) {
	    f = true ;
	    rs = kvsfile_checkfiles(op,daytime) ;
	}

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
		KVSFILE_KEY	*kep = static_cast<keyp>(vp) ;
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
		KVSFILE_FILE	*fep = static_cast<filep>(vp) ;
	        rs1 = file_finish(fep) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end for */
	return rs ;
}
/* end subroutine (kvsfile_filefins) */

static int kvsfile_checkfiles(kvsfile *op,time_t daytime) noex {
	vecobj		*flp = &op->files ;
	USTAT		sb ;
	int		rs = SR_OK ;
	int		c_changed = 0 ;
	void		*vp{} ;
	for (int i = 0 ; vecobj_get(flp,i,&vp) >= 0 ; i += 1) {
	    if (vp) {
	    KVSFILE_FILE	*fep = static_cast<filep>(vp) ;
	    if ((u_stat(fep->fname,&sb) >= 0) && (sb.st_mtime > fep->mtime)) {

	        kvsfile_filedump(op,i) ;

	        rs = kvsfile_fileparse(op,i) ;
	        if (rs >= 0) {
	            c_changed += 1 ;
		}
	    } /* end if */
	    } /* end if */
	    if (rs < 0) break ;
	} /* end for */
	op->ti_check = daytime ;
	return (rs >= 0) ? c_changed : rs ;
}
/* end subroutine (kvsfile_checkfiles) */

static int kvsfile_fh(kvsfile *op,dev_t dev,uino_t ino) noex {
	vecobj		*flp = &op->files ;
	int		rs ;
	void		*vp{} ;
	for (int i = 0 ; (rs = vecobj_get(flp,i,&vp)) >= 0 ; i += 1) {
	    if (vp) {
		KVSFILE_FILE	*fep = static_cast<filep>(vp) ;
	        if ((fep->dev == dev) && (fep->ino == ino)) break ;
	    }
	} /* end for */
	return rs ;
}
/* end subroutine (kvsfile_fh) */

static int kvsfile_fileparse(kvsfile *op,int fi) noex {
	int		rs ;
	int		c = 0 ;
	void		*vp{} ;
	if ((rs = vecobj_get(&op->files,fi,&vp)) >= 0) {
	    if (vp) {
		KVSFILE_FILE	*fep = static_cast<filep>(vp) ;
	        bfile		kvsfile, *lfp = &kvsfile ;
	        if ((rs = bopen(lfp,fep->fname,"r",0664)) >= 0) {
	            USTAT	sb ;
	            if ((rs = bstat(lfp,&sb)) >= 0) {
	                if (! S_ISDIR(sb.st_mode)) {
	                    if (sb.st_mtime > fep->mtime) {
	                        cint		nrs = SR_NOTFOUND ;
	                        const dev_t	dev = sb.st_dev ;
	                        const uino_t	ino = sb.st_ino ;
	                        if ((rs = kvsfile_fh(op,dev,ino)) == nrs) {
	                            fep->dev = dev ;
	                            fep->ino = ino ;
	                            fep->mtime = sb.st_mtime ;
	                            fep->size = sb.st_size ;
	                            rs = kvsfile_fileparser(op,fi,lfp) ;
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
	            bclose(lfp) ;
	        } /* end if (file) */
	    } else {
	        rs = SR_NOTFOUND ;
	    }
	} /* end if (vec-get) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (kvsfile_fileparse) */

static int kvsfile_fileparser(kvsfile *op,int fi,bfile *lfp) noex {
	KVSFILE_KEY	*kep = nullptr ;
	KVSFILE_ENT	ve ;
	FIELD		fsb ;
	cint	llen = LINEBUFLEN ;
	int		rs = SR_OK ;
	int		c_field ;
	int		ki, len ;
	int		fl, cl ;
	int		c_added = 0 ;
	int		c = 0 ;
	int		f_eol ;
	cchar	*fp ;
	cchar	*cp ;
	char		lbuf[LINEBUFLEN + 1] ;
	char		keybuf[KEYBUFLEN + 1] ;

#if	CF_DEBUGS
	debugprintf("kvsfile_fileparser: ent fi=%u\n",fi) ;
#endif

	keybuf[0] = '\0' ;

/* loop, reading all records and figuring things out */

	while ((rs = breadln(lfp,lbuf,llen)) > 0) {
	    len = rs ;

	    f_eol = (lbuf[len - 1] == '\n') ;
	    if (f_eol) len -= 1 ;
	    lbuf[len] = '\0' ;

#if	CF_DEBUGS && CF_DEBUGFILE
	    debugprintf("kvsfile_fileparse: line>%t<\n",
	        lbuf,MIN(len,40)) ;
#endif

	    cp = lbuf ;
	    cl = len ;
	    while (CHAR_ISWHITE(*cp)) {
	        cp += 1 ;
	        cl -= 1 ;
	    }

	    if ((*cp == '\0') || (*cp == '#'))
	        continue ;

	    c_field = 0 ;
	    if ((rs = field_start(&fsb,cp,cl)) >= 0) {

	        while ((fl = field_get(&fsb,fterms,&fp)) >= 0) {

	            if ((c_field++ == 0) && (fsb.term == ':')) {

	                c = 0 ;
	                strwcpy(keybuf,fp,MIN(fl,KEYBUFLEN)) ;

#if	CF_DEBUGS && CF_DEBUGFILE
	                debugprintf("kvsfile_fileparse: key=%s\n",keybuf) ;
#endif

	            } else if ((fl > 0) && (keybuf[0] != '\0')) {

#if	CF_DEBUGS && CF_DEBUGFILE
	                debugprintf("kvsfile_fileparse: value w/ key\n") ;
#endif

/* enter key into string table (if first time) */

#if	CF_DEBUGS && CF_DEBUGFILE
	                debugprintf("kvsfile_fileparse: c=%u\n",c) ;
#endif

	                if (c++ == 0) {
	                    rs = kvsfile_getkeyp(op,keybuf,&kep) ;
	                    ki = rs ;
	                } /* end if (entering key) */

#if	CF_DEBUGS && CF_DEBUGFILE
	                debugprintf("kvsfile_fileparse: mid kep(%p)\n", 
	                    kep) ;
	                if (kep != nullptr)
	                    debugprintf("kvsfile_fileparse: mid key=%s\n", 
	                        kep->kname) ;
#endif

	                if ((rs >= 0) && (kep != nullptr)) {
	                    int	f = true ;

#if	CF_DEBUGS && CF_DEBUGFILE
	                    debugprintf("kvsfile_fileparse: key=%s val=%t\n",
	                        kep->kname,fp,fl) ;
#endif

	                    if ((rs = entry_start(&ve,fi,ki,kep,fp,fl)) >= 0) {
	                        cint	nrs = SR_NOTFOUND ;

	                        if ((rs = kvsfile_already(op,&ve)) == nrs) {

#if	CF_DEBUGS && CF_DEBUGFILE
	                            debugprintf("kvsfile_fileparse: "
	                                "new value "
	                                "for this key\n") ;
#endif

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

	        field_finish(&fsb) ;
	    } /* end if (field) */

	    if (rs < 0) break ;
	} /* end while (reading lines) */

#if	CF_DEBUGS
	debugprintf("kvsfile_fileparser: ret rs=%d c=%u\n",rs,c_added) ;
#endif

	return (rs >= 0) ? c_added : rs ;
}
/* end subroutine (kvsfile_fileparser) */

static int kvsfile_getkeyp(kvsfile *op,cchar *kbuf,KVSFILE_KEY **kpp) noex {
	KVSFILE_KEY	ke ;
	KVSFILE_KEY	*kep = nullptr ;
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
	    } else {
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

static int kvsfile_already(kvsfile *op,KVSFILE_ENT *nep) noex {
	hdb_datum	key ;
	hdb_datum	val{} ;
	int		rs = SR_OK ;
	key.buf = nep ;
	key.len = sizeof(KVSFILE_ENT) ;
#ifdef	COMMENT
	{
	    hdb_cur	cur ;
	    hdb_curbegin(&op->entries,&cur) ;
	    while (hdb_fetch(&op->entries,key,&cur,&val) >= 0) {
	        KVSFILE_ENT	*ep = static_cast<entp>(val.buf) ;
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

static int kvsfile_addentry(kvsfile *op,KVSFILE_ENT *nep) noex {
	KVSFILE_KEY	*kep ;
	KVSFILE_ENT	*ep ;
	cint		esz = sizeof(KVSFILE_ENT) ;
	int		rs ;
	if ((rs = uc_malloc(esz,&ep)) >= 0) {
	    hdb_datum	key ;
	    hdb_datum	val{} ;
	    *ep = *nep ;
	    kep = ep->kep ;
	    key.buf = ep ;
	    key.len = sizeof(KVSFILE_ENT) ;
	    val.buf = ep ;
	    val.len = sizeof(KVSFILE_ENT) ;
	    if ((rs = hdb_store(&op->keyvals,key,val)) >= 0) {
	        key.buf = kep->kname ;
	        key.len = strlen(kep->kname) ;
	        val.buf = ep ;
	        val.len = sizeof(KVSFILE_ENT) ;
	        rs = hdb_store(&op->entries,key,val) ;
	        if (rs < 0) {
	            hdb_cur	cur ;
	            hdb_curbegin(&op->keyvals,&cur) ;
	            {
	                key.buf = ep ;
	                key.len = sizeof(KVSFILE_ENT) ;
	                if (hdb_fetch(&op->keyvals,key,&cur,&val) >= 0) {
	                    hdb_delcur(&op->keyvals,&cur,0) ;
			}
	            }
	            hdb_curend(&op->keyvals,&cur) ;
	        } /* end if (bad) */
	    } /* end if (keyvals-store) */
	    if (rs < 0)
	        uc_free(ep) ;
	} /* end if (memory-allocation) */

#if	CF_DEBUGS
	debugprintf("kvsfile_addentry: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (kvsfile_addentry) */

static int kvsfile_filedump(kvsfile *op,int fi) noex {
	KVSFILE_ENT	*ep ;
	hdb_cur		cur ;
	hdb_datum	key ;
	hdb_datum	val{} ;
	int		rs ;
	int		rs1 ;

	if ((rs = hdb_curbegin(&op->keyvals,&cur)) >= 0) {

	    while (hdb_enum(&op->keyvals,&cur,&key,&val) >= 0) {
	        ep = (KVSFILE_ENT *) val.buf ;

	        if ((ep->fi == fi) || (fi < 0)) {
	            hdb_delcur(&op->keyvals,&cur,0) ;
	        } /* end if (found matching entry) */

	    } /* end while (looping through entries) */

	    rs1 = hdb_curend(&op->keyvals,&cur) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (cursor) */

/* delete all entries w/ this file */

	if (rs >= 0) {
	    if ((rs = hdb_curbegin(&op->entries,&cur)) >= 0) {

	        while (hdb_enum(&op->entries,&cur,&key,&val) >= 0) {
	            ep = (KVSFILE_ENT *) val.buf ;

	            if ((ep->fi == fi) || (fi < 0)) {

#if	CF_DEBUGS
	                debugprintf("kvsfile_filedump: got one\n") ;
#endif

	                rs1 = hdb_delcur(&op->entries,&cur,0) ;
	                if (rs >= 0) rs = rs1 ;

	                rs1 = entry_finish(ep) ;
	                if (rs >= 0) rs = rs1 ;

	                rs1 = uc_free(ep) ;
	                if (rs >= 0) rs = rs1 ;

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
/* delete a file from the list of files (that is all) */
static int kvsfile_filedel(kvsfile *op,int fi) noex {
	int		rs ;
	void		*vp{} ;
	if ((rs = vecobj_get(&op->files,fi,&vp)) >= 0) {
	    if (vp) {
		KVSFILE_FILE	*fep = static_cast<filep>(vp) ;
	        file_finish(fep) ;
	    }
	    rs = vecobj_del(&op->files,fi) ;
	}
	return rs ;
}
/* end subroutine (kvsfile_filedel) */
#endif /* COMMENT */

static int file_start(KVSFILE_FILE *fep,cchar *fname) noex {
	int		rs = SR_FAULT ;
	if (fname == nullptr) return SR_FAULT ;
	memset(fep,0,sizeof(KVSFILE_FILE)) ;
	cchar		*cp ;
	rs = uc_mallocstrw(fname,-1,&cp) ;
	if (rs >= 0) fep->fname = cp ;

	return rs ;
}
/* end subroutine (file_start) */

static int file_finish(KVSFILE_FILE *fep) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (fep == nullptr) return SR_FAULT ;

	if (fep->fname != nullptr) {
	    rs1 = uc_free(fep->fname) ;
	    if (rs >= 0) rs = rs1 ;
	    fep->fname = nullptr ;
	}

	return rs ;
}
/* end subroutine (file_finish) */

static int key_start(KVSFILE_KEY *kep,cchar *kname) noex {
	int		rs = SR_OK ;

	memset(kep,0,sizeof(KVSFILE_KEY)) ;

	if (kname != nullptr) {
	    cint	klen = KEYBUFLEN ;
	    cchar	*cp ;
	    rs = uc_mallocstrw(kname,klen,&cp) ;
	    if (rs >= 0) kep->kname = cp ;
	}

	return rs ;
}
/* end subroutine (key_start) */

static int key_finish(KVSFILE_KEY *kep) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (kep->kname != nullptr) {
	    rs1 = uc_free(kep->kname) ;
	    if (rs >= 0) rs = rs1 ;
	    kep->kname = nullptr ;
	}

	kep->count = 0 ;
	return rs ;
}
/* end subroutine (key_finish) */

static int key_increment(KVSFILE_KEY *kep) noex {
	int		rs = SR_FAULT ;
	if (kep) {
	    rs = ++kep->count ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (key_increment) */

static int key_decrement(KVSFILE_KEY *kep) noex {
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

static int entry_start(KVSFILE_ENT *ep,int fi,int ki,
	KVSFILE_KEY *kep,cchar *vp,int vl) noex {
	int		rs ;
	cchar		*cp ;

	memset(ep,0,sizeof(KVSFILE_ENT)) ;
	ep->fi = fi ;
	ep->ki = ki ;
	ep->kep = kep ;
	ep->vlen = vl ;
	if ((rs = uc_mallocstrw(vp,vl,&cp)) >= 0) {
	    ep->vname = cp ;
	    key_increment(kep) ;
	}

	return rs ;
}
/* end subroutine (entry_start) */

static int entry_finish(KVSFILE_ENT *ep) noex {
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

static int cmpfname(KVSFILE_FILE **e1pp,KVSFILE_FILE **e2pp) noex {
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

static int cmpkey(KVSFILE_KEY **e1pp,KVSFILE_KEY **e2pp) noex {
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

static int cmpkeyval(KVSFILE_ENT *e1p,KVSFILE_ENT *e2p,int) noex {
	int		rc ;
	if ((rc = strcmp(e1p->kep->kname,e2p->kep->kname)) == 0) {
	    rc = strcmp(e1p->vname,e2p->vname) ;
	}
	return rc ;
}
/* end subroutine (cmpkeyval) */

static uint hashkeyval(KVSFILE_ENT *ep,int) noex {
	KVSFILE_KEY	*kep = ep->kep ;
	uint		hv = 0 ;
	hv += hash_elf(kep->kname,-1) ;
	hv += hash_elf(ep->vname,-1) ;
	return hv ;
}
/* end subroutine (hashkeyval) */


