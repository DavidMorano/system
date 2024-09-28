/* svcfile SUPPORT */
/* lang=C++20 */

/* service-table file manager */
/* version %I% last-modified %G% */

#define	CF_DEVINO	0		/* check device-inode */
#define	CF_ALREADY	0		/* disallow duplicate entries */
#define	CF_MOREKEYS	0		/* |ientry_morekeys()| */
#define	CF_FILEDEL	0		/* |svcfile_filedel()| */

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
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<netdb.h>
#include	<usystem.h>
#include	<bfile.h>
#include	<field.h>
#include	<sncpyx.h>
#include	<mkpathx.h>
#include	<strwcpy.h>
#include	<char.h>
#include	<getpwd.h>
#include	<timestr.h>
#include	<localmisc.h>

#include	"svcfile.h"


/* local defines */

#define	SVCFILE_INTCHECK	2	/* file-change check interval */
#define	SVCFILE_INTWAIT		2	/* file-change wait interval */

#define	SVCFILE_FILE		struct xsvcfile_file
#define	SVCFILE_KEYNAME		struct svcfile_keyname
#define	SVCFILE_IENT		struct svcfile_ie
#define	SVCFILE_SVCNAME		struct svcfile_svcname

#define	SVCENTRY		struct svcentry
#define	SVCENTRY_KEY		struct svcentry_key

#ifndef	LINEBUFLEN
#ifdef	LINE_MAX
#define	LINEBUFLEN		MAX(LINE_MAX,2048)
#else
#define	LINEBUFLEN		2048
#endif
#endif

#ifndef	SVCNAMELEN
#ifdef	SVCFILE_SVCLEN
#define	SVCNAMELEN		SVCFILE_SVCLEN
#else
#define	SVCNAMELEN		MAXNAMELEN
#endif
#endif

#undef	ABUFLEN
#define	ABUFLEN			(3 * MAXHOSTNAMELEN)

#undef	DEFCHUNKSIZE
#define	DEFCHUNKSIZE		512

#define	DEFNFILES		10
#define	DEFNENTRIES		10

#define	SVCFILE_KA		sizeof(char *(*)[2])
#define	SVCFILE_BO(v)		\
	((SVCFILE_KA - ((v) % SVCFILE_KA)) % SVCFILE_KA)


/* external subroutines */


/* external variables */


/* local structures */

struct svcfile_svcname {
	cchar		*svcname ;
	int		count ;
} ;

struct xsvcfile_file {
	cchar		*fname ;
	time_t		mtime ;
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


/* forward references */

int		svcfile_fileadd(svcfile *,cchar *) noex ;
int		svcfile_check(svcfile *,time_t) noex ;
int		svcfile_fetch(svcfile *,cchar *,svcfile_cur *,
			svcfile_ent *,char *,int) noex ;

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

static int	svcentry_start(SVCENTRY *,cchar *) noex ;
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

static int	ientry_loadstr(SVCFILE_IENT *,char *,SVCENTRY *) noex ;
static int	ientry_finish(SVCFILE_IENT *) noex ;
#if	CF_MOREKEYS
static int	ientry_morekeys(SVCFILE_IENT *,int,int) noex ;
#endif

static int	entry_load(svcfile_ent *,char *,int,SVCFILE_IENT *) noex ;

static int	cmpfname() noex ;
static int	cmpsvcname() noex ;


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

#ifdef	COMMENT
/* key field terminators ('#', ',', ':', '=') */
constexpr cchar		kterms[32] = {
	0x00, 0x00, 0x00, 0x00,
	0x08, 0x10, 0x00, 0x24,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
} ;
#endif /* COMMENT */

/* argument field terminators (pound '#' and comma ',') */
constexpr cchar		saterms[32] = {
	0x00, 0x00, 0x00, 0x00,
	0x08, 0x10, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ;


/* exported variables */


/* exported subroutines */

int svcfile_open(svcfile *op,cchar *fname) noex {
	int		rs = SR_OK ;
	int		size ;
	int		opts ;
	int		n ;
	int		c = 0 ;

	if (op == nullptr) return SR_FAULT ;

	memclear(op) ;

/* this vector structure must remain fixed so that indices do not change */

	size = sizeof(SVCFILE_FILE) ;
	n = DEFNFILES ;
	opts = (VECOBJ_OSTATIONARY | VECOBJ_OREUSE) ;
	if ((rs = vecobj_start(&op->files,size,n,opts)) >= 0) {
	    size = sizeof(SVCFILE_SVCNAME) ;
	    opts = VECOBJ_OCOMPACT ;
	    if ((rs = vecobj_start(&op->svcnames,size,10,opts)) >= 0) {
	        n = DEFNENTRIES ;
	        if ((rs = hdb_start(&op->entries,n,0,nullptr,nullptr)) >= 0) {
	            op->magic = SVCFILE_MAGIC ;
	            op->checktime = time(nullptr) ;
	            if (fname != nullptr) {
	                rs = svcfile_fileadd(op,fname) ;
	                c = rs ;
	            }
	            if (rs < 0) {
	                hdb_finish(&op->entries) ;
	                op->magic = 0 ;
	            }
	        } /* end if (hdb-start) */
	        if (rs < 0) {
	            svcfile_svcfins(op) ;
	            vecobj_finish(&op->svcnames) ;
	        }
	    } /* end if svcnames) */
	    if (rs < 0) {
	        vecobj_finish(&op->files) ;
	    }
	} /* end if (vecobj_start) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (svcfile_open) */

int svcfile_close(svcfile *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != SVCFILE_MAGIC) return SR_NOTOPEN ;

/* secondary items */

	rs1 = svcfile_filedump(op,-1) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = svcfile_filefins(op) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = svcfile_svcfins(op) ;
	if (rs >= 0) rs = rs1 ;

/* primary items */

	rs1 = hdb_finish(&op->entries) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = vecobj_finish(&op->svcnames) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = vecobj_finish(&op->files) ;
	if (rs >= 0) rs = rs1 ;

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (svcfile_close) */

int svcfile_fileadd(svcfile *op,cchar *fname) noex {
	int		rs = SR_OK ;
	int		fi = 0 ;
	cchar	*np ;
	char		tmpfname[MAXPATHLEN + 1] ;

	if (op == nullptr) return SR_FAULT ;
	if (fname == nullptr) return SR_FAULT ;

	if (op->magic != SVCFILE_MAGIC) return SR_NOTOPEN ;

	np = (cchar *) fname ;
	if (fname[0] != '/') {
	    char	pwdbuf[MAXPATHLEN+1] ;
	    np = tmpfname ;
	    if ((rs = getpwd(pwdbuf,MAXPATHLEN)) >= 0) {
	        rs = mkpath2(tmpfname,pwdbuf,fname) ;
	    }
	} /* end if (added PWD) */

	if (rs >= 0) {
	    SVCFILE_FILE	fe ;
	    vecobj		*flp = &op->files ;
	    if ((rs = file_start(&fe,np)) >= 0) {
	        cint	nrs = SR_NOTFOUND ;
	        int		f_fin = false ;
	        if ((rs = vecobj_search(flp,&fe,cmpfname,nullptr)) == nrs) {
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
	} /* end if (ok) */

	return rs ;
}
/* end subroutine (svcfile_fileadd) */

int svcfile_curbegin(svcfile *op,svcfile_cur *curp) noex {
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magic != SVCFILE_MAGIC) return SR_NOTOPEN ;

	if (op->ncursors == 0) {
	    rs = svcfile_check(op,0L) ;
	}

	if (rs >= 0) {
	    curp->i = -1 ;
	    if ((rs = hdb_curbegin(&op->entries,&curp->ec)) >= 0) {
	        op->ncursors += 1 ;
	        op->magic = SVCFILE_MAGIC ;
	    }
	}

	return rs ;
}
/* end subroutine (svcfile_curbegin) */

int svcfile_curend(svcfile *op,svcfile_cur *curp) noex {
	int		rs ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;

	if (op->magic != SVCFILE_MAGIC) return SR_NOTOPEN ;

	curp->i = -1 ;
	if ((rs = hdb_curend(&op->entries,&curp->ec)) >= 0) {
	    if (op->ncursors > 0) op->ncursors -= 1 ;
	}

	return rs ;
}
/* end subroutine (svcfile_curend) */

int svcfile_enumsvc(svcfile *op,svcfile_cur *curp,char *ebuf,int elen) noex {
	SVCFILE_SVCNAME	*snp ;
	int		rs = SR_OK ;
	int		i ;
	int		kl = 0 ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;
	if (ebuf == nullptr) return SR_FAULT ;

	if (op->magic != SVCFILE_MAGIC) return SR_NOTOPEN ;

	i = (curp->i >= 0) ? (curp->i + 1) : 0 ;

	while ((rs = vecobj_get(&op->svcnames,i,&snp)) >= 0) {
	    if (snp != nullptr) break ;
	    i += 1 ;
	} /* end while */

	if (rs >= 0) {
	    if ((rs = sncpy1(ebuf,elen,snp->svcname)) >= 0) {
	        kl = rs ;
	        curp->i = i ;
	    }
	} /* end if */

	return (rs >= 0) ? kl : rs ;
}
/* end subroutine (svcfile_enumsvc) */

int svcfile_enum(EVCFILE *op,svcfile_cur *curp,svcfile_ent *ep,
		char *ebuf,int elen) noex {
	hdb_dat		key ;
	hdb_dat		val ;
	hdb_cur		cur ;
	int		rs = SR_OK ;
	int		svclen = 0 ;

	if (op == nullptr) return SR_FAULT ;
	if (curp == nullptr) return SR_FAULT ;
	if ((ep == nullptr) || (ebuf == nullptr)) return SR_FAULT ;

	if (op->magic != SVCFILE_MAGIC) return SR_NOTOPEN ;

	if (elen <= 0) return SR_OVERFLOW ;

	cur = curp->ec ;
	if ((rs = hdb_enum(&op->entries,&cur,&key,&val)) >= 0) {
	    SVCFILE_IENT	*iep = (struct svcfile_ie *) val.buf ;

	    if ((ep != nullptr) && (ebuf != nullptr)) {
	        rs = entry_load(ep,ebuf,elen,iep) ;
	        svclen = rs ;
	    } else {
	        svclen = strlen(iep->svc) ;
	    }

	    if (rs >= 0) {
	        curp->ec = cur ;
	    }

	} /* end if (had an entry) */

	return (rs >= 0) ? svclen : rs ;
}
/* end subroutine (svcfile_enum) */

int svcfile_fetch(svcfile *op,cc *svcname,svcfile_cur *curp,svcfile_ent *ep,
		char *ebuf,int elen) noex {
	hdb_dat		key ;
	hdb_dat		val ;
	hdb_cur		cur ;
	int		rs = SR_OK ;
	int		svclen = 0 ;

	if (op == nullptr) return SR_FAULT ;
	if (svcname == nullptr) return SR_FAULT ;

	if (op->magic != SVCFILE_MAGIC) return SR_NOTOPEN ;

/* check for update */

	if (op->ncursors == 0) {
	    rs = svcfile_check(op,0L) ;
	}

	if (rs >= 0) {

/* continue */

	    if (curp == nullptr) {
	        rs = hdb_curbegin(&op->entries,&cur) ;
	    } else {
	        cur = curp->ec ;
	    }

	    if (rs >= 0) {
	        key.buf = (void *) svcname ;
	        key.len = strlen(svcname) ;
	        if ((rs = hdb_fetch(&op->entries,key,&cur,&val)) >= 0) {
	            SVCFILE_IENT	*iep = (struct svcfile_ie *) val.buf ;

	            if ((ep != nullptr) && (ebuf != nullptr)) {

	                rs = entry_load(ep,ebuf,elen,iep) ;
	                svclen = rs ;

	            } else {
	                svclen = strlen(iep->svc) ;
	            }

	            if ((rs >= 0) && (curp != nullptr)) {
	                curp->ec = cur ;
	            }

	        } /* end if (had an entry) */
	        if (curp == nullptr) {
	            hdb_curend(&op->entries,&cur) ;
		}
	    } /* end if (ok) */

	} /* end if (ok) */

	return (rs >= 0) ? svclen : rs ;
}
/* end subroutine (svcfile_fetch) */

int svcfile_check(svcfile *op,time_t daytime) noex {
	cint		to = SVCFILE_INTCHECK ;
	int		rs = SR_OK ;
	int		c = 0 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != SVCFILE_MAGIC) return SR_NOTOPEN ;

	if (daytime == 0)
	    daytime = time(nullptr) ;

/* should we even check? */

	if ((op->ncursors == 0) && ((daytime - op->checktime) >= to)) {
	    op->checktime = daytime ;
	    rs = svcfile_checkfiles(op,daytime) ;
	    c = rs ;
	} /* end if */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (svcfile_check) */

int svcfile_match(svcfile *sfp,cchar *name) noex {
	int		rs ;

	rs = svcfile_fetch(sfp,name,nullptr,nullptr,nullptr,0) ;

	return rs ;
}
/* end subroutine (svcfile_match) */


/* private subroutines */

static int svcfile_filefins(svcfile *op) noex {
	SVCFILE_FILE	*fep ;
	int		rs = SR_OK ;
	int		rs1 ;
	for (int i = 0 ; vecobj_get(&op->files,i,&fep) >= 0 ; i += 1) {
	    if (fep != nullptr) {
	        rs1 = file_finish(fep) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end for */

	return rs ;
}
/* end subroutine (svcfile_filefins) */

/* check if the access table files have changed */
/* ARGSUSED */
static int svcfile_checkfiles(svcfile *op,time_t daytime) noex {
	USTAT		sb ;
	SVCFILE_FILE	*fep ;
	cint		wt = SVCFILE_INTWAIT ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		i ;
	int		c_changed = 0 ;

/* check the files */

	for (i = 0 ; vecobj_get(&op->files,i,&fep) >= 0 ; i += 1) {
	    if (fep == nullptr) continue ;

	    rs1 = u_stat(fep->fname,&sb) ;

	    if ((rs1 >= 0) && ((sb.st_mtime - fep->mtime) >= wt)) {

	        c_changed += 1 ;
	        svcfile_filedump(op,i) ;

	        rs = svcfile_fileparse(op,i) ;

	    } /* end if (changed) */

	    if (rs < 0) break ;
	} /* end for */

	return (rs >= 0) ? c_changed : rs ;
}
/* end subroutine (svcfile_checkfiles) */

static int svcfile_fileparse(svcfile *op,int fi) noex {
	SVCFILE_FILE	*fep ;
	int		rs ;
	int		c = 0 ;

	if ((rs = vecobj_get(&op->files,fi,&fep)) >= 0) {
	    if (fep != nullptr) {
	        USTAT	sb ;
	        cchar	*fname = fep->fname ;
	        if ((rs = u_stat(fname,&sb)) >= 0) {
	            if (sb.st_mtime > fep->mtime) {
	                fep->dev = sb.st_dev ;
	                fep->ino = sb.st_ino ;
	                fep->mtime = sb.st_mtime ;
	                fep->sz = sb.st_size ;
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

static int svcfile_fileparser(svcfile *op,int fi,cchar *fname) noex {
	bfile		svcfile, *lfp = &svcfile ;
	int		rs ;
	int		rs1 ;
	int		fl, kl, al ;
	int		c = 0 ;
	cchar	*fp ;
	cchar	*kp ;

	if ((rs = bopen(lfp,fname,"r",0664)) >= 0) {
	    SVCENTRY	se ;
	    FIELD	fsb ;
	    cint	llen = LINEBUFLEN ;
	    cint	alen = ABUFLEN ;
	    int		fn = 0 ;
	    int		len ;
	    int		cl ;
	    int		c_field ;
	    int		f_ent = false ;
	    int		f_bol = true ;
	    int		f_eol ;
	    cchar	*cp ;
	    char	lbuf[LINEBUFLEN + 1] ;
	    char	abuf[ABUFLEN + 1] ;
	    char	svcname[SVCNAMELEN + 1] ;
	    svcname[0] = '\0' ;

	    while ((rs = breadln(lfp,lbuf,llen)) > 0) {
	        len = rs ;

	        f_eol = (lbuf[len - 1] == '\n') ;
	        if (f_eol) len -= 1 ;
	        lbuf[len] = '\0' ;

	        cp = lbuf ;
	        cl = len ;
	        while (CHAR_ISWHITE(*cp)) {
	            cp += 1 ;
	            cl -= 1 ;
	        }

	        if ((*cp == '\0') || (*cp == '#')) continue ;
	        if (! f_bol) continue ;

	        c_field = 0 ;
	        if ((rs = field_start(&fsb,cp,cl)) >= 0) {

	            while ((fl = field_get(&fsb,fterms,&fp)) >= 0) {

	                if ((c_field++ == 0) && (fsb.term == ':')) {

	                    fn = 0 ;
	                    strwcpy(svcname,fp,MIN(fl,SVCNAMELEN)) ;

	                } else if ((fl > 0) && (svcname[0] != '\0')) {

/* create a SVCENTRY if found a first key */

	                    if (fn++ == 0) {

	                        if (f_ent) {

	                            if (rs >= 0) {
	                                c += 1 ;
	                                rs = svcfile_addentry(op,fi,&se) ;
	                            }

	                            f_ent = false ;
	                            svcentry_finish(&se) ;
	                        } /* end if */

	                        if (rs >= 0) {
	                            rs = svcentry_start(&se,svcname) ;
	                            f_ent = (rs >= 0) ;
	                        }

	                    } /* end if (created SVCENTRY) */

	                    kp = fp ;
	                    kl = fl ;
	                    abuf[0] = '\0' ;
	                    al = 0 ;
	                    if (fsb.term != ',') {
	                        al = field_srvarg(&fsb,saterms,abuf,alen) ;
	                    }

	                    if ((rs >= 0) && f_ent) {
	                        rs = svcentry_addkey(&se,kp,kl,abuf,al) ;
	                    }

	                } /* end if (handling record) */

	                if (fsb.term == '#') break ;
	                if (rs < 0) break ;
	            } /* end while (fields) */

	            field_finish(&fsb) ;
	        } /* end if (field) */

	        f_bol = f_eol ;
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

	    rs1 = bclose(lfp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (bfile) */

	if (rs < 0) {
	    svcfile_filedump(op,fi) ;
	}

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (svcfile_fileparser) */

#if	CF_DEVINO
static int svcfile_filealready(svcfile *op,dev_t dev,ino_t ino) noex {
	SVCFILE_FILE	*fep ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		i ;
	int		f = false ;

	for (i = 0 ; (rs1 = vecobj_get(&op->files,i,&fep)) >= 0 ; i += 1) {
	    if (fep == nullptr) continue ;
	    f = ((fep->dev == dev) && (fep->ino == ino)) ;
	    if (f) break ;
	} /* end for */
	if ((rs >= 0) && (rs1 != SR_NOTFOUND)) rs = rs1 ;

	return (rs >= 0) ? f : rs ;
}
/* end subroutine (svcfile_filealready) */
#endif /* CF_DEVINO */

/* add an entry to the access entry list */
static int svcfile_addentry(svcfile *op,int fi,SVCENTRY *nep) noex {
	SVCFILE_IENT	*iep ;
	int		size ;
	int		rs ;
	int		f_added = false ;

#if	CF_ALREADY
	if ((rs1 = svcfile_already(op,nep->svc)) == SR_NOTFOUND) {
#endif

	    f_added = true ;
	    size = sizeof(SVCFILE_IENT) ;
	    if ((rs = uc_malloc(size,&iep)) >= 0) {
	        cint	n = svcentry_nkeys(nep) ;
	        void	*p ;
	        iep->fi = fi ;
	        size = (n+1) * 2 * sizeof(char *) ;
	        if ((rs = uc_malloc(size,&p)) >= 0) {
	            char	*bp ;
	            iep->nkeys = n ;
	            iep->keyvals = p ;
	            size = svcentry_size(nep) ;
	            if ((rs = uc_malloc(size,&bp)) >= 0) {
	                iep->sz = size ;
	                if ((rs = ientry_loadstr(iep,bp,nep)) >= 0) {
	                    hdb_dat	key, val ;
	                    cint	sl = rs ;
	                    key.buf = iep->svc ;
	                    key.len = sl ;
	                    val.buf = iep ;
	                    val.len = sizeof(SVCFILE_IENT) ;
	                    if ((rs = hdb_store(&op->entries,key,val)) >= 0) {
	                        rs = svcfile_svcadd(op,iep->svc) ;
	                        if (rs < 0)
	                            hdb_delkey(&op->entries,key) ;
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
	        if (rs < 0)
	            uc_free(iep) ;
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

	key.buf = (void *) svcname ;
	key.len = strlen(svcname) ;
	rs = hdb_fetch(&op->entries,key,nullptr,nullptr) ;

	return rs ;
}
/* end subroutine (svcfile_already) */
#endif /* CF_ALREADY */

/* free up all of the entries in this svcfile list associated w/ a file */
static int svcfile_filedump(svcfile *op,int fi) noex {
	hdb_cur		cur ;
	hdb_dat		key ;
	hdb_dat		val ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;

	if ((rs = hdb_curbegin(&op->entries,&cur)) >= 0) {
	    SVCFILE_IENT	*ep ;
	    while (hdb_enum(&op->entries,&cur,&key,&val) >= 0) {

	        ep = (SVCFILE_IENT *) val.buf ;

	        if ((ep->fi == fi) || (fi < 0)) {

	            c += 1 ;
	            rs1 = hdb_delcur(&op->entries,&cur,0) ;
	            if (rs >= 0) rs = rs1 ;

	            rs1 = svcfile_svcdel(op,ep->svc) ;
	            if (rs >= 0) rs = rs1 ;

	            rs1 = ientry_finish(ep) ;
	            if (rs >= 0) rs = rs1 ;

	            rs1 = uc_free(ep) ;
	            if (rs >= 0) rs = rs1 ;

	        } /* end if (found matching entry) */

	    } /* end while (looping through entries) */
	    rs1 = hdb_curend(&op->entries,&cur) ;
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
	if ((rs = vecobj_get(&op->files,fi,&vp)) >= 0) {
	    if (vp) {
		SVCFILE_FILE	*fep = (SVCFILE_FILE *) vp ;;
		{
	            rs1 = file_finish(fep) ;
	            if (rs >= 0) rs = rs1 ;
		}
		{
	            rs1 = vecobj_del(&op->files,fi) ;
	            if (rs >= 0) rs = rs1 ;
		}
	    } /* end if */
	} /* end if (vecobj_get) */

	return rs ;
}
/* end subroutine (svcfile_filedel) */
#endif /* CF_FILEDEL */

static int svcfile_svcadd(svcfile *op,cchar *svc) noex {
	SVCFILE_SVCNAME	sn ;
	SVCFILE_SVCNAME	*snp ;
	vecobj		*lp = &op->svcnames ;
	int		rs ;
	int		f_added = false ;

	sn.svcname = svc ;
	sn.count = 1 ;
	if ((rs = vecobj_search(lp,&sn,cmpsvcname,&snp)) >= 0) {
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
	SVCFILE_SVCNAME	sn ;
	SVCFILE_SVCNAME	*snp ;
	vecobj		*lp = &op->svcnames ;
	int		rs ;
	int		rs1 ;
	int		si = 0 ;

	sn.svcname = svc ;
	sn.count = 0 ;
	if ((rs = vecobj_search(lp,&sn,cmpsvcname,&snp)) >= 0) {
	    si = rs ;
	    rs1 = svcname_decr(snp) ;
	    if (rs >= 0) rs = rs1 ;
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
	for (int i = 0 ; vecobj_get(&op->svcnames,i,&vp) >= 0 ; i += 1) {
	    if (vp) {
		SVCFILE_SVCNAME	*snp = (SVCFILE_SVCNAME *) vp ;
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

static int svcentry_start(SVCENTRY *sep,cchar *svc) noex {
	int		rs ;
	cchar		*cp ;

	memclear(sep) ;

	if ((rs = uc_mallocstrw(svc,-1,&cp)) >= 0) {
	    cint	size = sizeof(SVCENTRY_KEY) ;
	    sep->svc = cp ;
	    rs = vecobj_start(&sep->keys,size,5,VECOBJ_OORDERED) ;
	    if (rs < 0) {
	        uc_free(cp) ;
	        sep->svc = nullptr ;
	    }
	} /* end if (memory-allocation) */

	return rs ;
}
/* end subroutine (svcentry_start) */

static int svcentry_finish(SVCENTRY *sep) noex {
	SVCENTRY_KEY	*kep ;
	int		rs = SR_OK ;
	int		rs1 ;
	for (int i = 0 ; vecobj_get(&sep->keys,i,&kep) >= 0 ; i += 1) {
	    if (kep != nullptr) {
	        if (kep->kname != nullptr) {
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
	if (sep->svc != nullptr) {
	    rs1 = uc_free(sep->svc) ;
	    if (rs >= 0) rs = rs1 ;
	    sep->svc = nullptr ;
	}
	return rs ;
}
/* end subroutine (svcentry_finish) */

static int svcentry_addkey(SVCENTRY *sep,cc *kp,int kl,cc *ap,int al) noex {
	int		rs ;
	int		size = 0 ;
	char		*bp ;

	if (kl < 0) kl = strlen(kp) ;
	if (al < 0) al = strlen(ap) ;
	size += (kl+1) ;
	size += (al+1) ;
	if ((rs = uc_malloc(size,&bp)) >= 0) {
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
	    if (rs < 0)
	        uc_free(key.kname) ;
	} /* end if (memory-allocation) */

	return rs ;
}
/* end subroutine (svcentry_addkey) */

static int svcentry_nkeys(SVCENTRY *sep) noex {
	return vecobj_count(&sep->keys) ;
}
/* end subroutine (svcentry_nkeys) */

static int svcentry_size(SVCENTRY *sep) noex {
	SVCENTRY_KEY	*kep ;
	int		size = 0 ;
	size += (strlen(sep->svc) + 1) ;
	for (int i = 0 ; vecobj_get(&sep->keys,i,&kep) >= 0 ; i += 1) {
	    if (kep != nullptr) {
	        size += (kep->kl+1) ;
	        size += (kep->al+1) ;
	    }
	} /* end for */
	return size ;
}
/* end subroutine (svcentry_size) */

static int ientry_loadstr(SVCFILE_IENT *iep,char *bp,SVCENTRY *nep) noex {
	SVCENTRY_KEY	*kep ;
	int		j = 0 ;
	int		sl = strlen(nep->svc) ;
	iep->svc = bp ;
	bp = (strwcpy(bp,nep->svc,sl)+1) ;
	for (int i = 0 ; vecobj_get(&nep->keys,i,&kep) >= 0 ; i += 1) {
	    if (kep != nullptr) {
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
	return sl ;
}
/* end subroutine (ientry_loadstr) */

static int ientry_finish(SVCFILE_IENT *iep) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (iep == nullptr) return SR_FAULT ;

	if (iep->keyvals != nullptr) {
	    rs1 = uc_free(iep->keyvals) ;
	    if (rs >= 0) rs = rs1 ;
	    iep->keyvals = nullptr ;
	}

	if (iep->svc != nullptr) {
	    rs1 = uc_free(iep->svc) ;
	    if (rs >= 0) rs = rs1 ;
	    iep->svc = nullptr ;
	}

	return rs ;
}
/* end subroutine (ientry_finish) */

#if	CF_MOREKEYS
static int ientry_morekeys(SVCFILE_IENT *iep,int c,int i) noex {
	int		f_more = true ;
	if (c >= 0) {
	    f_more = (i < c) ;
	}
	f_more = f_more && (iep->keyvals[i][0] != nullptr) ;
	return f_more ;
}
/* end subroutine (ientry_morekeys) */
#endif /* CF_MOREKEYS */

/* load up the user-interface entry from the internal structure */
static int entry_load(svcfile_ent *ep,char *ebuf,int elen,
		SVCFILE_IENT *iep) noex {
	int		rs = SR_OK ;
	int		bo, i, kal ;
	int		rlen = 0 ;
	cchar	*(*keyvals)[2] ;
	cchar	*kp, *vp ;
	char		*bp ;

	if (iep == nullptr) return SR_FAULT ;

	if ((ebuf != nullptr) && (elen <= 0)) return SR_OVERFLOW ;

	bo = SVCFILE_BO((ulong) ebuf) ;

	if (iep->sz <= (elen - bo)) {

	    keyvals = (cchar *(*)[2]) (ebuf + bo) ;
	    kal = (iep->nkeys + 1) * 2 * sizeof(char *) ;
	    bp = (char *) (ebuf + bo + kal) ;
#ifdef	COMMENT
	    bl = (elen - bo - kal) ;
#endif

	    ep->svc = bp ;
	    bp = strwcpy(bp,iep->svc,-1) + 1 ;

	    rlen = (bp - ep->svc - 1) ;

	    for (i = 0 ; i < iep->nkeys ; i += 1) {
	        kp = iep->keyvals[i][0] ;
	        vp = iep->keyvals[i][1] ;

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

	} else {
	    rs = SR_OVERFLOW ;
	}

	return (rs >= 0) ? rlen : rs ;
}
/* end subroutine (entry_load) */

static int cmpfname(SVCFILE_FILE **e1pp,SVCFILE_FILE **e2pp) noex {
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

static int cmpsvcname(cvoid *v1p, cvoid *v2p) noex {
	SVCFILE_SVCNAME	**e1pp = (SVCFILE_SVCNAME **) v1p ;
	SVCFILE_SVCNAME	**e2pp = (SVCFILE_SVCNAME **) v2p ;
	int		rc = 0 ;

	if ((*e1pp != nullptr) || (*e2pp != nullptr)) {
	    if (*e1pp != nullptr) {
	        if (*e2pp != nullptr) {
		    SVCFILE_SVCNAME	*e1p = (SVCFILE_SVCNAME *) *e1pp ;
		    SVCFILE_SVCNAME	*e2p = (SVCFILE_SVCNAME *) *e2pp ;
		    int		n1, n2 ;

		    n1 = (e1p->svcname == nullptr) ;
		    n2 = (e2p->svcname == nullptr) ;
		    if (n1 || n2) {
	    	        if (! (n1 && n2)) {
	        	    rc = (n1) ? 1 : -1 ;
	    	        }
		    }

		    if ((rc == 0) && (! n1) && (! n2)) {
	    	        rc = strcmp(e1p->svcname,e2p->svcname) ;
	            }

	        } else {
	            rc = -1 ;
		}
	    } else {
	        rc = 1 ;
	    }
	}

	return rc ;
}
/* end subroutine (cmpsvcname) */


