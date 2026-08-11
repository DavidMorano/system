/* cyimk SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* Calendar-Year-Index DB-make (make a CYI database) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A≠D≠ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A≠D≠ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	cyimk

	Description:
	This module creates a CYI database file.

	Synopsis:
	int cyimk_open(op,year,dname,cname,of,om)
	CYIMK		*op ;
	int		year ;
	const char	dname[] ;
	const char	cname[] ;
	int		of ;
	mode_t		om ;

	Arguments:
	op		object pointer
	dname		directory path
	cname		name of calendar
	of		open-flags
	om		open (create) file permissions 
	year		year

	Returns:
	>=0		OK
	<0		error code (system-return)

	Notes:

	= possible returns to an open attempt
	- OK (creating)
	- already exists
	- doesn't exist but is in progress
	- exists and is in progress

	= open-flags

			if DB exits	if NDB exists	returns
	___________________________________________________________________

	-		no		no		SR_OK (created)
	-		no		yes		SR_INPROGRESS
	-		yes		no		SR_OK
	-		yes		yes		SR_INPROGRESS

	O_CREAT|O_EXCL	no		no		SR_OK (created)
	O_CREAT|O_EXCL	no		yes		SR_INPROGRESS
	O_CREAT|O_EXCL	yes		no		SR_EXIST
	O_CREAT|O_EXCL	yes		yes		SR_INPROGRESS

	O_CREAT		x		x		SR_OK (created)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIXÆ */
#include	<sys/param.h>		/* POSIXÆ */
#include	<sys/stat.h>		/* POSIXÆ */
#include	<tzfile.h>		/* POSIXÆ |TM_YEAR_BASE|[ */
#include	<unistd.h>		/* POSIXÆ */
#include	<fcntl.h>		/* POSIXÆ */
#include	<ctime>			/* CSTD */
#include	<climits>		/* CSTD |INT_MAX| */
#include	<cstddef>		/* CSTD |nullptr_t| */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<algorithm>		/* C++STD |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<endian.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<estrings.h>		/* LIBUC */
#include	<ids.h>			/* LIBUC */
#include	<storebuf.h>		/* LIBUC */
#include	<filer.h>		/* LIBUC */
#include	<tmtime.hh>		/* LIBUC */
#include	<opentmp.h>		/* LIBUC */
#include	<char.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"cyimk.h"
#include	"cyihdr.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	CYIMK_DEFENTS	1024
#define	CYIMK_NSKIP	5

#define	HDRBUFLEN	(szof(CYIHDR) + 128)
#define	BUFLEN		(szof(CYIHDR) + 128)

#define	FSUF_IDX	"cyi"

#define	TO_OLDFILE	(5 * 60)


/* imported nameﬂpaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct bventry {
	uint		voff ;
	uint		vlen ;
	uint		li ;		/* index-number of first line-entry */
	uint		hash ;
	uint		citation ;	/* (nlines, m, d) */
} ; /* end struct */

struct blentry {
	uint		loff ;
	uint		llen ;
} ; /* end struct */


/* forward references */

local int	cyimk_idbegin(CYIMK *,cchar *,int) ;
local int	cyimk_idend(CYIMK *) ;
local int	cyimk_idxdir(CYIMK *,IDS *,cchar *) ;
local int	cyimk_minown(CYIMK *,cchar *,mode_t) ;

local int	cyimk_filesbegin(CYIMK *) ;
local int	cyimk_filesbeginc(CYIMK *) ;
local int	cyimk_filesbeginwait(CYIMK *) ;
local int	cyimk_filesbegincreate(CYIMK *,cchar *,int,mode_t) ;
local int	cyimk_filesend(CYIMK *) ;

local int	cyimk_listbegin(CYIMK *,int) ;
local int	cyimk_listend(CYIMK *) ;

local int	cyimk_mkidx(CYIMK *) ;
local int	cyimk_mkidxmain(CYIMK *,CYIHDR *) ;
local int	cyimk_mkidxhdr(CYIMK *,CYIHDR *,filer *) ;
local int	cyimk_mkidxstrs(CYIMK *,CYIHDR *,filer *,int) ;
local int	cyimk_mkidxents(CYIMK *,CYIHDR *,filer *,int) ;
local int	cyimk_mkidxlines(CYIMK *,CYIHDR *,filer *,int) ;
local int	cyimk_nidxopen(CYIMK *) ;
local int	cyimk_nidxclose(CYIMK *) ;
local int	cyimk_renamefiles(CYIMK *) ;

local int	mkydname(char *,cchar *,int) ;
local int	mkcitation(uint *,CYIMK_ENT *) ;
local int	mknewfname(char *,int,cchar *,cchar *) ;
local int	unlinkstale(cchar *,int) ;

local int	vvecmp(cvoid *,cvoid *) ;


/* local variables */


/* exported variables */

const cyimk_obj		cyimk_modinfo = {
	"cyimk",
	szof(cyimk)
} ; /* end initialization */


/* exported subroutines */

int cyimk_open(CYIMK *op,int year,cc *dname,cc *cname,int of,mode_t om) noex {
	cint		n = CYIMK_DEFENTS ;
	int		rs = SR_OK ;
	int		c = 0 ;

	if (op == nullptr) return SR_FAULT ;
	if (dname == nullptr) return SR_FAULT ;
	if (cname == nullptr) return SR_FAULT ;

	if (dname[0] == '\0') return SR_INVALID ;
	if (cname[0] == '\0') return SR_INVALID ;

	if (year <= 0) {
	    TMTIME	tm ;
	    time_t	dt = time(nullptr) ;
	    rs = tmtime_timelocal(&tm,dt) ;
	    year = (tm.year + TM_YEAR_BASE) ;
	} /* end if */

	memclear(op) ;
	op->om = (om|0600) ;
	op->nfd = -1 ;
	op->year = year ;

	op->fl.ofcreat = MKBOOL(of & O_CREAT) ;
	op->fl.ofexcl = MKBOOL(of & O_EXCL) ;
	op->fl.none = (! op->fl.ofcreat) && (! op->fl.ofexcl) ;

	if (rs >= 0) {
	    if ((rs = cyimk_idbegin(op,dname,year)) >= 0) {
	        cchar	*cp ;
	        if ((rs = uc_mallocstrw(cname,-1,&cp)) >= 0) {
	            op->cname = cp ;
	            if ((rs = cyimk_filesbegin(op)) >= 0) {
	                c = rs ;
	                if ((rs = cyimk_listbegin(op,n)) >= 0) {
	                    op->magval = CYIMK_MAGIC ;
	                }
	                if (rs < 0)
	                    cyimk_filesend(op) ;
	            }
	            if (rs < 0) {
	                if (op->cname != nullptr) {
	                    uc_free(op->cname) ;
	                    op->cname = nullptr ;
	                }
	            }
	        } /* end if (memory-allocation) */
	        if (rs < 0)
	            cyimk_idend(op) ;
	    } /* end if (cyim_idbegin) */
	} /* end if (ok) */

	return (rs >= 0) ? c : rs ;
} /* end subroutine (cyimk_open) */

int cyimk_close(CYIMK *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		n = 0 ; /* return-value */
	bool		f_go = false ;
	if (op == nullptr) return SR_FAULT ;
	if (op->magval != CYIMK_MAGIC) return SR_NOTOPEN ;
	f_go = (! op->fl.abort) ;
	n = op->nentries ;
	if (n > 0) {
	    if (op->fl.notsorted) {
	        vecobj_sort(&op->verses,vvecmp) ;
	    }
	    {
	        rs1 = cyimk_mkidx(op) ;
	        if (rs >= 0) rs = rs1 ;
	        f_go = f_go && (rs1 >= 0) ;
	    }
	}
	{
	    rs1 = cyimk_listend(op) ;
	    if (rs >= 0) rs = rs1 ;
	    f_go = f_go && (rs1 >= 0) ;
	}
	if ((rs >= 0) && f_go) {
	    rs1 = cyimk_renamefiles(op) ;
	    if (rs >= 0) rs = rs1 ;
	}
	{
	rs1 = cyimk_filesend(op) ;
	if (rs >= 0) rs = rs1 ;
	}
	if (op->cname) {
	    rs1 = uc_free(op->cname) ;
	    if (rs >= 0) rs = rs1 ;
	    op->cname = nullptr ;
	} /* end if (momory-release) */
	{
	    rs1 = cyimk_idend(op) ;
	    if (rs >= 0) rs = rs1 ;
	}
	op->magval = 0 ;
	return (rs >= 0) ? n : rs ;
} /* end subroutine (cyimk_close) */

int cyimk_add(CYIMK *op,CYIMK_ENT *bvp) noex {
	uint		li = UINT_MAX ;
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;
	if (bvp == nullptr) return SR_FAULT ;

	if (op->magval != CYIMK_MAGIC) return SR_NOTOPEN ;

	if ((bvp->lines != nullptr) && (bvp->nlines > 0)) {
	    blentry	ble ;
	    cint	imax = UCHAR_MAX ;
	    for (int i = 0 ; (i < bvp->nlines) && (i < imax) ; i += 1) {
	        ble.loff = bvp->lines[i].loff ;
	        ble.llen = bvp->lines[i].llen ;
	        rs = vecobj_add(&op->lines,&ble) ;
	        if (i == 0) li = rs ;
	        if (rs < 0) break ;
	    } /* end for */
	} /* end if */

	if (rs >= 0) {
	    bventry	bve ;
	    uint	citcmpval ;
	    bve.voff = bvp->voff ;
	    bve.vlen = bvp->vlen ;
	    bve.li = li ;
	    bve.hash = bvp->hash ;
	    mkcitation(&bve.citation,bvp) ;
	    citcmpval = (bve.citation & 0x0000FFFF) ;
	    if (citcmpval < op->pcitation) op->fl.notsorted = true ;
	    op->pcitation = citcmpval ;
	    rs = vecobj_add(&op->verses,&bve) ;
	    op->nentries += 1 ;
	} /* end if (ok) */
	return rs ;
} /* end subroutine (cyimk_add) */

int cyimk_abort(CYIMK *op,int f) noex {
	op->fl.abort = f ;
	return SR_OK ;
} /* end subroutine (cyimk_abort) */


/* private subroutines */

local int cyimk_idbegin(CYIMK *op,cchar *dname,int year) noex {
	int		rs ;
	if (ustat sb ; (rs = uc_stat(dname,&sb)) >= 0) {
	    cint	am = (W_OK|X_OK) ;
	    op->gid = sb.st_gid ;
	    op->uid = sb.st_uid ;
	    if (ids id ; (rs = ids_load(&id)) >= 0) {
	        if ((rs = permid(&id,&sb,am)) >= 0) {
	            char	ydname[MAXPATHLEN+1] ;
	            if ((rs = mkydname(ydname,dname,year)) >= 0) {
		        if ((rs = cyimk_idxdir(op,&id,ydname)) >= 0) {
	                    cchar	*cp ;
	                    if ((rs = uc_mallocstrw(ydname,-1,&cp)) >= 0) {
	                        op->idname = cp ;
	                    }
		        } /* end if (cyimk_idxdir) */
	            } /* end if (mkydname) */
		} /* end if (permid) */
		ids_release(&id) ;
	    } /* end if (ids) */
	} /* end if */
	return rs ;
} /* end subroutine (cyimk_idbegin) */

local int cyimk_idxdir(CYIMK *op,IDS *idp,cchar *ydname) noex {
	cmode		dm = 0777 ;
	cint		nrs = SR_NOENT ;
	int		rs = SR_OK ;
	if (ustat sb ; (rs = uc_stat(ydname,&sb)) >= 0) {
	    cint	am = (W_OK|X_OK) ;
	    uid_t	uid_yd = sb.st_uid ;
	    gid_t	gid_yd = sb.st_gid ;
	    if ((rs = permid(idp,&sb,am)) >= 0) {
		uid_t	uid = geteuid() ;
		if (uid == uid_yd) {
		    if ((uid_yd != op->uid) || (gid_yd != op->gid)) {
			rs = cyimk_minown(op,ydname,dm) ;
		    }
		}
	    } /* end if (permid) */
	} else if (rs == nrs) {
	    if ((rs = mkdirs(ydname,dm)) >= 0) {
		rs = cyimk_minown(op,ydname,dm) ;
	    } /* end if (mkdirs) */
	} /* end if */
	return rs ;
} /* end subroutine (cyimk_idxdir) */

local int cyimk_minown(CYIMK *op,cchar *dname,mode_t dm) noex {
	int		rs ;
	if ((rs = uc_minmod(dname,dm)) >= 0) {
	    gid_t	gid = op->gid ;
	    uid_t	uid = op->uid ;
	    rs = uc_chown(dname,uid,gid) ;
	}
	return rs ;
} /* end subroutine (cyimk_minown) */

local int cyimk_idend(CYIMK *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->idname) {
	    rs1 = uc_free(op->idname) ;
	    if (rs >= 0) rs = rs1 ;
	    op->idname = nullptr ;
	}
	return rs ;
} /* end subroutine (cyimk_idend) */

local int cyimk_filesbegin(CYIMK *op) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	if (op->fl.ofcreat) {
	    rs = cyimk_filesbeginc(op) ;
	} else {
	    rs = cyimk_filesbeginwait(op) ;
	    c = rs ;
	}
	return (rs >= 0) ? c : rs ;
} /* end subroutine (cyimk_filesbegin) */

local int cyimk_filesbeginc(CYIMK *op) noex {
	cint		type = (op->fl.ofcreat && (! op->fl.ofexcl)) ;
	int		rs ;
	char		dbn[MAXPATHLEN+1] ;
	if ((rs = mkpath2(dbn,op->idname,op->cname)) >= 0) {
	    cchar	*suf = FSUF_IDX	 ;
	    char	tbuf[MAXPATHLEN+1] ;
	    if ((rs = mknewfname(tbuf,type,dbn,suf)) >= 0) {
	        cmode	om = op->om ;
	        cchar	*tfn = tbuf ;
	        char	rbuf[MAXPATHLEN+1] ;
	        if (type) {
	            if ((rs = mktmpfile(rbuf,om,tbuf)) >= 0) {
	                op->fl.created = true ;
	                tfn = rbuf ;
	            }
	        }
	        if (rs >= 0) {
	            mode_t	om = op->om ;
	            int	of = O_CREAT ;
	            if (op->fl.ofexcl) of |= O_EXCL ;
	            rs = cyimk_filesbegincreate(op,tfn,of,om) ;
	            if ((rs < 0) && type) {
	                uc_unlink(rbuf) ;
	            }
	        } /* end if (ok) */
	    } /* end if (mknewfname) */
	} /* end if (mkpath) */
	return rs ;
} /* end subroutine (cyimk_filesbeginc) */

local int cyimk_filesbeginwait(CYIMK *op) noex {
	int		rs ;
	int		c = 0 ;
	char		dbn[MAXPATHLEN+1] ;
	if ((rs = mkpath2(dbn,op->idname,op->cname)) >= 0) {
	    cchar	*suf = FSUF_IDX	 ;
	    char	tbuf[MAXPATHLEN+1] ;
	    if ((rs = mknewfname(tbuf,false,dbn,suf)) >= 0) {
	        cmode	om = op->om ;
	        cint	to_stale = CYIMK_INTSTALE ;
	        cint	nrs = SR_EXISTS ;
	        cint	of = (O_CREAT|O_WRONLY|O_EXCL) ;
	        int	to = CYIMK_INTOPEN ;
	        while ((rs = cyimk_filesbegincreate(op,tbuf,of,om)) == nrs) {
	            c = 1 ;
	            sleep(1) ;
	            unlinkstale(tbuf,to_stale) ;
	            if (to-- == 0) break ;
	        } /* end while (db exists) */
	        if (rs == nrs) {
	            op->fl.ofcreat = false ;
	            c = 0 ;
	            rs = cyimk_filesbeginc(op) ;
	        }
	    } /* end if (mknewfname) */
	} /* end if (mkpath) */
	return (rs >= 0) ? c : rs ;
} /* end subroutine (cyimk_filesbeginwait) */

local int cyimk_filesbegincreate(CYIMK *op,cchar *tfn,int of,mode_t om) noex {
	int		rs ;
	if ((rs = uc_open(tfn,of,om)) >= 0) {
	    cint	fd = rs ;
	    cchar	*cp ;
	    op->fl.created = true ;
	    if ((rs = uc_mallocstrw(tfn,-1,&cp)) >= 0) {
	        op->nidxfname = (char *) cp ;
	    }
	    u_close(fd) ;
	} /* end if (create) */
	return rs ;
} /* end subroutine (cyimk_filesbegincreate) */

local int cyimk_filesend(CYIMK *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->nfd >= 0) {
	    rs1 = u_close(op->nfd) ;
	    if (rs >= 0) rs = rs1 ;
	    op->nfd = -1 ;
	}
	if (op->nidxfname) {
	    if (op->fl.created && (op->nidxfname[0] != '\0')) {
	        rs1 = u_unlink(op->nidxfname) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    rs1 = uc_free(op->nidxfname) ;
	    if (rs >= 0) rs = rs1 ;
	    op->nidxfname = nullptr ;
	}
	return rs ;
} /* end subroutine (cyimk_filesend) */

local int cyimk_listbegin(CYIMK *op,int n) noex {
	cint		sz = szof(bventry) ;
	int		rs ;
	int		vo = 0 ;
	vo |= vecobjm.stationary ;
	vo |= vecobjm.ordered ;
	vo |= vecobjm.compact ;
	if ((rs = vecobj_start(&op->verses,sz,n,opts)) >= 0) {
	    rs = vecobj_start(&op->lines,sz,(n * 2),opts) ;
	    if (rs < 0) {
	        vecobj_finish(&op->verses) ;
	    } /* end if (error) */
	}
	return rs ;
} /* end subroutine (cyimk_listbegin) */

local int cyimk_listend(CYIMK *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	{
	    rs1 = vecobj_finish(&op->lines) ;
	    if (rs >= 0) rs = rs1 ;
	}
	{
	    rs1 = vecobj_finish(&op->verses) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
} /* end subroutine (cyimk_listend) */

local int cyimk_mkidx(CYIMK *op) noex {
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;

	if ((rs = cyimk_nidxopen(op)) >= 0) {
	    CYIHDR	hdr{} ;
	    hdr.vetu[0] = CYIHDR_VERSION ;
	    hdr.vetu[1] = ENDIAN ;
	    hdr.vetu[2] = 0 ;
	    hdr.vetu[3] = 0 ;
	    hdr.wtime = (uint) time(nullptr) ;
	    hdr.nentries = op->nentries ;
	    hdr.nskip = CYIMK_NSKIP ;
	    hdr.year = op->year ;

	    if ((rs = cyimk_mkidxmain(op,&hdr)) >= 0) {
	        cint	hlen = HDRBUFLEN ;
	        char		hbuf[HDRBUFLEN+1] ;
	        hdr.fsize = rs ;
	        wlen = rs ;

	        if ((rs = cyihdr(&hdr,0,hbuf,hlen)) >= 0) {
	            cint	bl = rs ;
	            if ((rs = u_writep(op->nfd,hbuf,bl,0L)) >= 0) {
	                cmode	om = op->om ;
	                rs = uc_fminmod(op->nfd,om) ;
	            }
	        }

	    } /* end if (cyimk_mkidxmain) */

	    rs1 = cyimk_nidxclose(op) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (cyimk_nidx) */

	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (cyimk_mkidx) */

local int cyimk_mkidxmain(CYIMK *op,CYIHDR *hdrp) noex {
	filer		hf, *hfp = &hf ;
	cint	nfd = op->nfd ;
	cint	ps = getpagesize() ;
	int		bsz ;
	int		rs ;
	int		rs1 ;
	int		off = 0 ;
	bsz = (ps * 4) ;
	if ((rs = filer_start(hfp,nfd,0,bsz,0)) >= 0) {
	    if ((rs = cyimk_mkidxhdr(op,hdrp,hfp)) >= 0) {
	        off += rs ;
	        if (rs >= 0) {
	            rs = cyimk_mkidxstrs(op,hdrp,hfp,off) ;
	            off += rs ;
	        }
	        if (rs >= 0) {
	            rs = cyimk_mkidxents(op,hdrp,hfp,off) ;
	            off += rs ;
	        }
	        if (rs >= 0) {
	            rs = cyimk_mkidxlines(op,hdrp,hfp,off) ;
	            off += rs ;
	        }
	    } /* end if (cyimk_mkidxhdr) */
	    rs1 = filer_finish(hfp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (filer) */
	return (rs >= 0) ? off : rs ;
} /* end subroutine (cyimk_mkidxmain) */

/* ARGSUSED */
local int cyimk_mkidxhdr(CYIMK *op,CYIHDR *hdrp,filer *hfp) noex {
	cint		hlen = HDRBUFLEN ;
	int		rs ;
	int		wlen = 0 ;
	char		hbuf[HDRBUFLEN+1] ;
	if (op == nullptr) return SR_FAULT ; /* LINT */
	if ((rs = cyihdr(hdrp,0,hbuf,hlen)) >= 0) {
	    rs = filer_writefill(hfp,hbuf,rs) ;
	    wlen += rs ;
	}
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (cyimk_mkidxhdr) */

local int cyimk_mkidxstrs(CYIMK *op,CYIHDR *hdrp,filer *hfp,int off) noex {
	int		rs ;
	int		wlen = 0 ;
	char		tbuf[MAXPATHLEN + 1] ;

	hdrp->diroff = off ;
	if ((rs = pathclean(tbuf,op->idname,-1)) >= 0) {
	    int	tl = rs ;
	    if ((rs = filer_writefill(hfp,tbuf,(tl+1))) >= 0) {
	        tl = lenstr(op->cname) ;
	        off += rs ;
	        wlen += rs ;
	        hdrp->caloff = off ;
	        rs = filer_writefill(hfp,op->cname,(tl+1)) ;
	        off += rs ;
	        wlen += rs ;
	    } /* end if (filer_writefill) */
	} /* end if (pathclean) */

	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (cyimk_mkidxstrs) */

local int cyimk_mkidxents(CYIMK *op,CYIHDR *hdrp,filer *hfp,int off) noex {
	struct bventry	*bvep ;
	vecobj		*elp = &op->verses ;
	uint		a[5] ;
	cint	size = (5 * szof(uint)) ;
	int		rs = SR_OK ;
	int		i ;
	int		n = 0 ;
	int		wlen = 0 ;

	hdrp->vioff = off ;
	for (i = 0 ; vecobj_get(elp,i,&bvep) >= 0 ; i += 1) {
	    if (bvep != nullptr) {
	        a[0] = bvep->voff ;
	        a[1] = bvep->vlen ;
	        a[2] = bvep->li ;
	        a[3] = bvep->citation ;
	        a[4] = bvep->hash ;
	        n += 1 ;
	        rs = filer_write(hfp,a,size) ;
	        off += rs ;
	        wlen += rs ;
	    }
	    if (rs < 0) break ;
	} /* end for */
	hdrp->vilen = n ;

	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (cyimk_mkidxents) */

local int cyimk_mkidxlines(CYIMK *op,CYIHDR *hdrp,filer *hfp,int off) noex {
	struct blentry	*blep ;
	vecobj		*llp = &op->lines ;
	uint		a[2] ;
	cint	size = (2 * szof(uint)) ;
	int		rs = SR_OK ;
	int		n = 0 ;
	int		i ;
	int		wlen = 0 ;

	hdrp->vloff = off ;
	for (i = 0 ; vecobj_get(llp,i,&blep) >= 0 ; i += 1) {
	    if (blep) {
	        a[0] = blep->loff ;
	        a[1] = blep->llen ;
	        n += 1 ;
	        rs = filer_write(hfp,a,size) ;
	        off += rs ;
	        wlen += rs ;
	    }
	    if (rs < 0) break ;
	} /* end for */
	hdrp->vllen = n ;

	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (cyimk_mkidxlines) */

local int cyimk_nidxopen(CYIMK *op) noex {
	cmode		om = op->om ;
	int		rs ;
	int		fd = -1 ;
	int		of = (O_CREAT|O_WRONLY) ;
	if (op->nidxfname == nullptr) {
	    cint	type = (op->fl.ofcreat && (! op->fl.ofexcl)) ;
	    cchar	*dbn = op->cname ;
	    cchar	*suf = FSUF_IDX ;
	    char	tbuf[MAXPATHLEN+1] ;
	    if ((rs = mknewfname(tbuf,type,dbn,suf)) >= 0) {
	        cchar	*tfn = tbuf ;
	        char	rbuf[MAXPATHLEN+1] ;
	        if (type) {
	            rs = opentmpfile(tbuf,of,om,rbuf) ;
	            op->nfd = rs ;
	            fd = rs ;
	            tfn = rbuf ;
	        } else {
	            if (op->fl.ofexcl) of |= O_EXCL ;
	            rs = uc_open(tbuf,of,om) ;
	            op->nfd = rs ;
	            fd = rs ;
	        }
	        if (rs >= 0) {
	            cchar	*cp ;
	            if ((rs = uc_mallocstrw(tfn,-1,&cp)) >= 0) {
	                op->nidxfname = (char *) cp ;
	            }
	        } /* end if (ok) */
	    } /* end if (mknewfname) */
	} else {
	    if (op->fl.ofexcl) of |= O_EXCL ;
	    rs = uc_open(op->nidxfname,of,om) ;
	    op->nfd = rs ;
	    fd = rs ;
	}
	return (rs >= 0) ? fd : rs ;
} /* end subroutine (cyimk_nidxopen) */

local int cyimk_nidxclose(CYIMK *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->nfd >= 0) {
	    rs1 = u_close(op->nfd) ;
	    if (rs >= 0) rs = rs1 ;
	    op->nfd = -1 ;
	}
	return rs ;
} /* end subroutine (cyimk_nidxclose) */

local int cyimk_renamefiles(CYIMK *op) noex {
	int		rs ;
	cchar		*suf = FSUF_IDX ;
	cchar		*end = ENDIANSTR ;
	char		dbn[MAXPATHLEN+1] ;
	if ((rs = mkpath2(dbn,op->idname,op->cname)) >= 0) {
	    char	idxfname[MAXPATHLEN + 1] ;
	    if ((rs = mkfnamesuf2(idxfname,dbn,suf,end)) >= 0) {
	        if ((rs = u_rename(op->nidxfname,idxfname)) >= 0) {
	            op->nidxfname[0] = '\0' ;
	        } else {
	            u_unlink(op->nidxfname) ;
	            op->nidxfname[0] = '\0' ;
	        }
	    } /* end if (mkfnamesuf) */
	} /* end if (mkpath) */
	return rs ;
} /* end subroutine (cyimk_renamefiles) */

local int mkcitation(uint *cip,CYIMK_ENT *bvp) noex {
	uint		ci = 0 ;
	uint		nlines ;

	nlines = (bvp->lines != nullptr) ? bvp->nlines : 0 ;

	ci |= ((nlines & UCHAR_MAX) << 24) ;
	ci |= ((bvp->m & UCHAR_MAX) << 8) ;
	ci |= ((bvp->d & UCHAR_MAX) << 0) ;

	*cip = ci ;
	return SR_OK ;
} /* end subroutine (mkcitation) */

local int mkydname(char *rbuf,cchar *dname,int year) noex {
	cint	rlen = MAXPATHLEN ;
	int		rs = SR_OK ;
	int		i = 0 ;
	if (rs >= 0) {
	    rs = storebuf_strw(rbuf,rlen,i,dname,-1) ;
	    i += rs ;
	}
	if (rs >= 0) {
	    rs = storebuf_chr(rbuf,rlen,i,'/') ;
	    i += rs ;
	}
	if (rs >= 0) {
	    rs = storebuf_chr(rbuf,rlen,i,'y') ;
	    i += rs ;
	}
	if (rs >= 0) {
	    rs = storebuf_deci(rbuf,rlen,i,year) ;
	    i += rs ;
	}
	return (rs >= 0) ? i : rs ;
} /* end subroutine (mkydname) */

local int mknewfname(char *tbuf,int type,cchar *dbn,cchar *suf) noex {
	cchar		*end = ENDIANSTR ;
	cchar		*fin = (type) ? "xXXXX" : "n" ;
	return mkfnamesuf3(tbuf,dbn,suf,end,fin) ;
} /* end subroutine (mknewfname) */

local int unlinkstale(cchar *fn,int to) {
	const time_t	dt = time(nullptr) ;
	int		rs ;
	if (ustat sb ; (rs = uc_stat(fn,&sb)) >= 0) {
	    if ((dt-sb.st_mtime) >= to) {
	        uc_unlink(fn) ;
	        rs = 1 ;
	    } else {
	        rs = 0 ;
	    }
	} else if (isNotPresent(rs)) {
	    rs = SR_OK ;
	}
	return rs ;
} /* end subroutine (unlinkstale) */

local int vvecmp(cvoid *v1p,cvoid *v2p) noex {
	struct bventry	**e1pp = (struct bventry **) v1p ;
	struct bventry	**e2pp = (struct bventry **) v2p ;
	int		rc = 0 ;
	if (*e1pp) {
	    if (*e2pp) {
	        uint	vc1 = (*e1pp)->citation & 0x0000FFFF ;
	        uint	vc2 = (*e2pp)->citation & 0x0000FFFF ;
	        rc = (vc1 - vc2) ;
	    } else {
	        rc = -1 ;
	    }
	} else {
	    rc = 1 ;
	}
	return rc ;
} /* end subroutine (vvecmp) */


