/* mailmsgfile SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* maintain translations for MSGID to filenames */
/* version %I% last-modified %G% */


/* revision history:

	= 2009-01-20, David A­D­ Morano
	This is new code.

*/

/* Copyright © 2009 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Object:
	mailmsgfile

	Description:
	This object implements a translation mapping from message-ids
	(MSGIDs) to unique temporary filenames.  Although not our
	business, these filenames point to files that hold the
	content (body) of mail messages.

	Implementation notes:

	+ Why the child process on exit?
	Because deleting files is just way too slow -- for whatever
	reason.  So we have a child do it do that we can finish up
	*fast* and get out.  This is a user response-time issue. We
	want the user to have the program exit quickly so that they
	are not annoyed (as they were when we previously deleted
	all of the files inline).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdarg>
#include	<cstring>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<getbufsize.h>
#include	<sysval.hh>
#include	<mallocxx.h>
#include	<opentmp.h>
#include	<hdb.h>
#include	<filer.h>
#include	<linefold.h>
#include	<fsdir.h>
#include	<vecstr.h>
#include	<vecpstr.h>
#include	<upt.h>
#include	<ncol.h>		/* |ncolstr(3uc)| */
#include	<tabcols.h>		/* |TABCOLS_DEF| */
#include	<mkx.h>
#include	<mkpathx.h>
#include	<pathadd.h>
#include	<strwcpy.h>
#include	<rmx.h>
#include	<cfdec.h>
#include	<char.h>
#include	<varnames.hh>
#include	<localmisc.h>		/* |COLUMNS| + |NTABCOLS| */

#include	"mailmsgfile.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |getlenstr(3u) */

/* local defines */

#define	MAILMSGFILE_DEFIND	4
#define	MAILMSGFILE_NE		30
#define	MAILMSGFILE_MINCOLS	38
#define	MAILMSGFILE_FILEINT	(2 * 24 * 3600)

#define	MMF			mailmsgfile
#define	MMF_MI			mailmsgfile_mi
#define	MMF_CD			mailmsgfile_checkdat


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using libuc::libmem ;			/* variable */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct mailmsgfile_checkdat {
	char		*pbuf ;
	int		plen ;
	mailmsgfile_checkdat(char *b,int l) noex : pbuf(b), plen(l) { } ;
    } ; /* end struct (mailmsgfile_checkdat) */
    struct vars {
	int		linebuflen ;
	int		outbuflen ;
	int		cols ;
	operator int () noex ;
    } ; /* end struct (vars) */
}


/* forward references */

template<typename ... Args>
static int mailmsgfile_ctor(mailmsgfile *op,Args ... args) noex {
    	MAILMSGFILE	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->flp = new(nothrow) hdb) != np) {
		rs = SR_OK ;
	    } /* end if (new-hdb) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mailmsgfile_ctor) */

static int mailmsgfile_dtor(mailmsgfile *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->flp) {
		delete op->flp ;
		op->flp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mailmsgfile_dtor) */

template<typename ... Args>
static int mailmsgfile_magic(mailmsgfile *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == MAILMSGFILE_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (mailmsgfile_magic) */

static int mailmsgfile_starter(MMF *,cc *) noex ;
static int mailmsgfile_newx(MMF *,cc *,int,off_t,int) noex ;
static int mailmsgfile_mk(MMF *,cchar *,cchar *,int,off_t,int) noex ;
static int mailmsgfile_mkdis(MMF *,MMF_MI *,int,int,off_t) noex ;
static int mailmsgfile_proclines(MMF *,MMF_MI *,filer *,cchar *,int) noex ;

static int mailmsgfile_procout(MMF *,filer *,int,cchar *,int,int) noex ;
static int mailmsgfile_store(MMF *,MMF_MI *) noex ;
static int mailmsgfile_filefins(MMF *) noex ;

static int mailmsgfile_checkbegin(MMF *) noex ;
static int mailmsgfile_checkend(MMF *) noex ;
static int mailmsgfile_checkout(MMF *) noex ;
static int mailmsgfile_checker(MMF *) noex ;
static int mailmsgfile_checkerx(MMF *) noex ;
static int mailmsgfile_checkerxx(MMF *,MMF_CD *) noex ;
static int mailmsgfile_checkerxxx(MMF *,vecpstr *,char *,int) noex ;

static int mi_start(MMF_MI *,cchar *,cchar *,int) noex ;
static int mi_finish(MMF_MI *) noex ;
static int mi_newlines(MMF_MI *,int) noex ;

static int mkcols(int) noex ;


/* local variables */

static sysval		pagesize(sysval_ps) ;

static vars		var ;


/* exported variables */


/* exported subroutines */

int mailmsgfile_start(MMF *op,cc *tmpdname,int cols,int ind) noex {
	int		rs ;
	int		rv = 0 ;
	if ((rs = mailmsgfile_magic(op,tmpdname)) >= 0) {
	    static cint		rsv = var ;
	    if ((rs = rsv) >= 0) {
	        if ((rs = pagesize) >= 0) {
		    op->pagesize = rs ;
		    op->to = MAILMSGFILE_FILEINT ;
	            if (ind < 0) {
		        ind = MAILMSGFILE_DEFIND ;
		    }
	            op->cols = mkcols(cols) ;
	            op->ind = ind ;
		    rs = mailmsgfile_starter(op,tmpdname) ;
		    rv = rs ;
	        } /* end if (sysval) */
	    } /* end if (mkvars) */
	    if (rs < 0) {
		mailmsgfile_dtor(op) ;
	    }
	} /* end if (mailmsgfile_ctor) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (mailmsgfile_start) */

int mailmsgfile_finish(MMF *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = mailmsgfile_magic(op)) >= 0) {
	    {
	        rs1 = mailmsgfile_checkend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = mailmsgfile_filefins(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->fl.files) {
	        op->fl.files = false ;
	        rs1 = hdb_finish(op->flp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->tmpdname) {
		void *vp = voidp(op->tmpdname) ;
	        rs1 = libmem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        op->tmpdname = nullptr ;
	    }
	    {
		rs1 = mailmsgfile_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (mailmsgfile_finish) */

int mailmsgfile_new(MMF *op,int type,cc *msgid,int mfd,
		off_t boff,int blen) noex {
	int		rs ;
	int		vlines = 0 ;
	if ((rs = mailmsgfile_magic(op,msgid)) >= 0) {
	    rs = SR_INVALID ;
	    if ((type >= 0) && msgid[0] && (boff >= 0)) {
		rs = SR_BADF ;
		if (mfd >= 0) {
		    rs = mailmsgfile_newx(op,msgid,mfd,boff,blen) ;
		    vlines = rs ;
	        } /* end if (valid) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? vlines : rs ;
}
/* end subroutine (mailmsgfile_new) */

int mailmsgfile_msginfo(MMF *op,MMF_MI **mipp,cc *msgid) noex {
	int		rs ;
	int		vlines = 0 ;
	if ((rs = mailmsgfile_magic(op,mipp,msgid)) >= 0) {
	    rs = SR_INVALID ;
	    if (msgid[0]) {
	        if ((rs = mailmsgfile_checkout(op)) >= 0) {
		    cnullptr	np{} ;
	            hdb_dat	key ;
	            hdb_dat	val ;
	            key.buf = msgid ;
	            key.len = lenstr(msgid) ;
	            if ((rs = hdb_fetch(op->flp,key,np,&val)) >= 0) {
	                *mipp = (MMF_MI *) val.buf ;
	                vlines = (*mipp)->vlines ;
	            }
	        } /* end if (checkout) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? vlines : rs ;
}
/* end subroutine (mailmsgfile_msginfo) */

int mailmsgfile_get(MMF *op,cc *msgid,cc **rpp) noex {
	int		rs ;
	int		vlines = 0 ;
	if ((rs = mailmsgfile_magic(op,msgid)) >= 0) {
	    rs = SR_INVALID ;
	    if (msgid[0]) {
	        MMF_MI	*mip{} ;
	        if ((rs = mailmsgfile_msginfo(op,&mip,msgid)) >= 0) {
	            vlines = mip->vlines ;
	            if (rpp) {
	                *rpp = charp(mip->mfname) ;
	            }
	        }
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? vlines : rs ;
}
/* end subroutine (mailmsgfile_get) */


/* private subroutines */

static int mailmsgfile_starter(MMF *op,cc *tmpdname) noex {
	cnullptr	np{} ;
	cint		ne = MAILMSGFILE_NE ;
	int		rs ;
	if (cchar *cp ; (rs = libmem.strw(tmpdname,-1,&cp)) >= 0) {
	    op->tmpdname = cp ;
	    if ((rs = hdb_start(op->flp,ne,true,np,np)) >= 0) {
		op->fl.files = true ;
		if ((rs = mailmsgfile_checkbegin(op)) >= 0) {
		    op->magic = MAILMSGFILE_MAGIC ;
		}
	    }
	    if (rs < 0) {
		void *vp = voidp(op->tmpdname) ;
	        libmem.free(vp) ;
		op->tmpdname = nullptr ;
	    }
	} /* end if (memory-allocation) */
	return rs ;
}
/* end subroutine (mailmsgfile_starter) */

static int mailmsgfile_newx(MMF *op,cc *mid,int mfd,off_t bo,int bl) noex {
	int		rs ;
	int		rs1 ;
	int		vlines = 0 ;
	hdb_dat		key ;
	hdb_dat		val ;
	key.buf = mid ;
	key.len = lenstr(mid) ;
	if ((rs = hdb_fetch(op->flp,key,nullptr,&val)) >= 0) {
	    MMF_MI	*mip = (MMF_MI *) val.buf ;
	    vlines = mip->vlines ;
	} else if (rs == SR_NOTFOUND) {
	    cchar	tpat[] = "msgXXXXXXXXXXX" ;
	    if (char *ibuf ; (rs = malloc_mp(&ibuf)) >= 0) {
		if ((rs = mkpath2(ibuf,op->tmpdname,tpat)) >= 0) {
		    rs = mailmsgfile_mk(op,mid,ibuf,mfd,bo,bl) ;
		    vlines = rs ;
		}
		rs1 = malloc_free(ibuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (found or not) */
	return (rs >= 0) ? vlines : rs ;
}
/* end if (mailmsgfile_newx) */

static int mailmsgfile_mk(MMF *op,cc *mid,cc *fn,int mfd,off_t bo,int bl) noex {
	int		rs ;
	int		rs1 ;
	int		vlines = 0 ;
	if (char *mfbuf ; (rs = malloc_mp(&mfbuf)) >= 0) {
	    cint	of = (O_WRONLY | O_CREAT) ;
	    cmode	om = 0666 ;
	    mfbuf[0] = '\0' ;
	    if ((rs = opentmpfile(fn,of,om,mfbuf)) >= 0) {
	        cint	tfd = rs ;
	        cint	w = SEEK_SET ;
	        if ((rs = u_seek(mfd,bo,w)) >= 0) {
		    MMF_MI		mi ;
	            if ((rs = mi_start(&mi,mid,mfbuf,bl)) >= 0) {
	                if ((rs = mailmsgfile_mkdis(op,&mi,tfd,mfd,bo)) >= 0) {
	                    vlines = rs ;
	                    rs = mailmsgfile_store(op,&mi) ; /* <- obj stored */
	                }
	                if (rs < 0) {
	                    mi_finish(&mi) ;
		 	}
	            } /* end if (mi) */
	        } /* end if (seek) */
	        rs1 = uc_close(tfd) ;
		if (rs >= 0) rs = rs1 ;
	        if (rs < 0) {
	            uc_unlink(mfbuf) ;
		    mfbuf[0] = '\0' ;
	        }
	    } /* end if (tmpfile) */
	    rs1 = malloc_free(&mfbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? vlines : rs ;
}
/* end subroutine (mailmsgfile_mk) */

static int mailmsgfile_filefins(MMF *op) noex {
	hdb		*mp = op->flp ;
	hdb_dat		key ;
	hdb_dat		val ;
	hdb_cur		cur ;
	int		rs = SR_OK ;
	int		rs1{} ;
	if ((rs1 = hdb_curbegin(mp,&cur)) >= 0) {
	    MMF_MI	*mip ;
	    while (hdb_curenum(mp,&cur,&key,&val) >= 0) {
	        mip = (MMF_MI *) val.buf ;
	        if (mip) {
		    {
	                rs1 = mi_finish(mip) ;
	                if (rs >= 0) rs = rs1 ;
		    }
	 	    {
	                rs1 = libmem.free(mip) ;
	                if (rs >= 0) rs = rs1 ;
		    }
	        } /* end if */
	    } /* end while */
	    rs1 = hdb_curend(mp,&cur) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (cursor) */
	if (rs >= 0) rs = rs1 ;
	return rs ;
}
/* end subroutine (mailmsgfile_filefins) */

static int mailmsgfile_mkdis(MMF *op,MMF_MI *mip,
		int tfd,int mfd,off_t bo) noex {
	cint		ntab = NTABCOLS ;
	cint		blen = mip->nsize ;
	int		rs ;
	int		rs1 ;
	int		vlines = 0 ;
	if (char *lbuf ; (rs = malloc_ml(&lbuf)) >= 0) {
	    cint	llen = rs ;
	    cint	ibsize = min(blen,(op->pagesize*8)) ;
	    int		inlen ;
	    int		wlen = 0 ;
	    if (filer in ; (rs = filer_start(&in,mfd,bo,ibsize,0)) >= 0) {
	        cint	obsize = rs ;
	        int	rlen = blen ;
	        if (filer out ; (rs = filer_start(&out,tfd,0z,obsize,0)) >= 0) {
		    int		ncols ;
	            int		ll ;
	            while ((rs >= 0) && (rlen > 0)) {
	                rs = filer_readln(&in,lbuf,llen,0) ;
	                inlen = rs ;
	                if (rs <= 0) break ;
	                ll = inlen ;
	                if (lbuf[ll-1] == '\n') {
	                    mip->nlines += 1 ;
	                    ll -= 1 ;
	                }
	                while ((ll > 0) && CHAR_ISWHITE(lbuf[ll-1])) {
	                    ll -= 1 ;
		        }
    /* calculate the number of columns this line would take up */
	                ncols = ncolstr(ntab,0,lbuf,ll) ;
    /* take action based on whether the line fits in the available columns */
	                if (ncols > op->cols) {
	                    rs = mailmsgfile_proclines(op,mip,&out,lbuf,ll) ;
	                    wlen += rs ;
	                } else {
	                    if ((rs = mi_newlines(mip,1)) >= 0) {
			        cbool	f = false ;
	                        rs = mailmsgfile_procout(op,&out,0,lbuf,ll,f) ;
	                        wlen += rs ;
	                    }
	                } /* end if (what kind of printing) */
	                rlen -= inlen ;
	            } /* end while (reading lines) */
	            if (rs >= 0) {
	                mip->vsize = wlen ;
	            }
	            rs1 = filer_finish(&out) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (filer) */
	        rs1 = filer_finish(&in) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (filer) */
	    rs1 = malloc_free(lbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? vlines : rs ;
}
/* end subroutine (mailmsgfile_mkdis) */

static int mailmsgfile_proclines(MMF *op,MMF_MI *mip,filer *fbp,
		cchar *lbuf,int llen) noex {
	linefold	ff ;
	cint		c = op->cols ;
	cint		ind = op->ind ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	if ((rs = linefold_start(&ff,c,ind,lbuf,llen)) >= 0) {
	    int		li = 0 ;
	    bool	f{} ;
	    cchar	*sp ;
	    while ((rs = linefold_getln(&ff,li,&sp)) > 0) {
		int	sl = rs ;
	        while ((rs >= 0) && (sl > 0)) {
	            int		plen = sl ;
	            f = false ;
	            if (plen > op->cols) {
	                f = true ;
	                plen = (op->cols - 1) ;
	            }
	            if ((rs = mi_newlines(mip,1)) >= 0) {
	                rs = mailmsgfile_procout(op,fbp,li,sp,plen,f) ;
	                wlen += rs ;
	            }
	            sp += plen ;
	            sl -= plen ;
	        } /* end while */
	        li += 1 ;
	        if (rs < 0) break ;
	    } /* end while (reading folded lines) */
	    rs1 = linefold_finish(&ff) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (linefold) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (mailmsgfile_proclines) */

static int mailmsgfile_procout(MMF *op,filer *fbp,int li,cc *lp,int ll,
		int f_cont) noex {
	cint		ind = op->ind ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		wlen = 0 ;
	if ((li > 0) && (ind > 0)) {
	    rs = filer_writeblanks(fbp,ind) ;
	    wlen += rs ;
	}
	if (rs >= 0) {
	    cint	olen = var.outbuflen ;
	    if (char *obuf ; (rs = libmem.mall((olen+1),&obuf)) >= 0) {
		bool	f_eol = ((ll > 0) && (lp[ll-1] == '\n')) ;
	        if ((rs = mkdisplayable(obuf,olen,lp,ll)) >= 0) {
	            cint	ol = rmeol(obuf,rs) ;
	            if (f_cont) {
	                if ((rs = filer_write(fbp,obuf,ol)) >= 0) {
	                    wlen += rs ;
	                    f_eol = true ;
	                    obuf[0] = '¬' ;
	                    obuf[1] = '\n' ;
	                    rs = filer_write(fbp,obuf,2) ;
	                    wlen += rs ;
	                } /* end if (filer_write) */
	            } else {
	                f_eol = true ;
	                rs = filer_println(fbp,obuf,olen) ;
	                wlen += rs ;
	            }
	            if ((rs >= 0) && (! f_eol)) {
	                obuf[0] = '\n' ;
	                rs = filer_write(fbp,obuf,1) ;
	                wlen += rs ;
	            }
	        } /* end if (mkdisplayable) */
		rs1 = libmem.free(obuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (ok) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (mailmsgfile_procout) */

static int mailmsgfile_store(MMF *op,MMF_MI *mip) noex {
	int		rs = SR_FAULT ;
	if (op && mip) {
	    cint	sz = szof(MMF_MI) ;
	    if (MMF_MI	*ep ; (rs = libmem.mall(sz,&ep)) >= 0) {
	        hdb_dat	key ;
	        hdb_dat	val ;
	        *ep = *mip ; /* copy */
	        key.buf = ep->mid ;
	        key.len = lenstr(ep->mid) ;
	        val.buf = ep ;
	        val.len = sz ;
	        rs = hdb_store(op->flp,key,val) ;
	        if (rs < 0) {
	            libmem.free(ep) ;
	        }
	    } /* end if (memory-allocation) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mailmsgfile_store) */

static int mailmsgfile_checkbegin(MMF *op) noex {
	pthread_t	tid ;
	int		rs ;
	uptsub_f	sub = uptsub_f(mailmsgfile_checker) ;
	if ((rs = uptcreate(&tid,nullptr,sub,op)) >= 0) {
	    op->tid = tid ;
	    op->fl.checkout = true ;
	}
	return rs ;
}
/* end subroutine (mailmsgfile_checkbegin) */

static int mailmsgfile_checkend(MMF *op) noex {
	int		rs = SR_OK ;
	if (op->fl.checkout) {
	    int	trs = SR_OK ;
	    if ((rs = uptjoin(op->tid,&trs)) >= 0) {
	        op->fl.checkout = false ;
	        rs = trs ;
	    }
	}
	return rs ;
}
/* end subroutine (mailmsgfile_checkend) */

static int mailmsgfile_checkout(MMF *op) noex {
	int		rs = SR_OK ;
	if (op->fl.checkout && op->f_checkdone) {
	    int		trs = SR_OK ;
	    if ((rs = uptjoin(op->tid,&trs)) >= 0) {
	        op->fl.checkout = false ;
	        rs = trs ;
	    }
	}
	return rs ;
}
/* end subroutine (mailmsgfile_checkout) */

static int mailmsgfile_checker(MMF *op) noex {
	int		rs = SR_OK ;
	int		rv = 0 ;
	if (op->tmpdname != nullptr) {
	    if (op->tmpdname[0] != '\0') {
		rs = mailmsgfile_checkerx(op) ;
		rv = rs ;
	    } else {
	        rs = SR_INVALID ;
	    }
	} else {
	    rs = SR_FAULT ;
	}
	op->f_checkdone = true ;
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (mailmsgfile_checker) */

static int mailmsgfile_checkerx(MMF *op) noex {
	int		rs ;
	int		rs1 ;
	int		rv = 0 ;
	if (char *pbuf ; (rs = malloc_mp(&pbuf)) >= 0) {
	    cint	plen = rs ;
	    if ((rs = mkpath1(pbuf,op->tmpdname)) >= 0) {
		MMF_CD	cd(pbuf,plen) ;
		rs = mailmsgfile_checkerxx(op,&cd) ;
		rv = rs ;
	    } /* end if (mkpath) */
	    rs1 = malloc_free(pbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (mailmsgfile_checkerx) */

static int mailmsgfile_checkerxx(MMF *op,MMF_CD *cdp) noex {
	vecpstr		files, *flp = &files ;
	int		rs ;
	int		rs1 ;
	if ((rs = vecpstr_start(flp,10,100,0)) >= 0) {
	    cint	plen = cdp->plen ;
	    char	*pbuf = cdp->pbuf ;
	    if ((rs = mailmsgfile_checkerxxx(op,flp,pbuf,plen)) >= 0) {
		cchar	*fp{} ;
		for (int i = 0 ; vecpstr_get(flp,i,&fp) >= 0 ; i += 1) {
		    if (fp) {
			if (fp[0] != '\0') {
			    u_unlink(fp) ;
			}
		    }
		} /* end for */
	    } /* end if */
	    rs1 = vecpstr_finish(flp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (vecpstr) */
	return rs ;
}
/* end subroutine (mailmsgfile_checkerxx) */

static int mailmsgfile_checkerxxx(MMF *op,vecpstr *flp,
		char *pbuf,int plen) noex {
	const time_t	dt = time(nullptr) ;
	cint		to = op->to ;	/* timeout */
	int		rs ;
	int		rs1 ;
	if (char *ebuf ; (rs = malloc_mn(&ebuf)) >= 0) {
	    fsdir	dir ;
	    fsdir_ent	de ;
	    cint	elen = rs ;
	    if ((rs = fsdir_open(&dir,op->tmpdname)) >= 0) {
	        ustat sb ;
	        while ((rs = fsdir_read(&dir,&de,ebuf,elen)) > 0) {
	            if (de.name[0] != '.') {
	                if ((rs = pathadd(pbuf,plen,de.name)) >= 0) {
	                    cint	pl = rs ;
	                    if (u_stat(pbuf,&sb) >= 0) {
	                        if ((dt - sb.st_mtime) >= to) {
	                            rs = vecpstr_add(flp,pbuf,pl) ;
	                        }
	                    } /* end if (stat) */
	                } /* end if (pathadd) */
		    } /* end if (not a got filename) */
	            if (rs < 0) break ;
	        } /* end while (reading entries) */
	        rs1 = fsdir_close(&dir) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (fsdir) */
	    rs1 = malloc_free(ebuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end subroutine (mailmsgfile_checkerxxx) */

static int mi_start(MMF_MI *mip,cc *msgid,cc *mfname,int blen) noex {
	int		rs = SR_FAULT ;
	if (mip) {
	    cint	milen = lenstr(msgid) ;
	    cint	mflen = lenstr(mfname) ;
	    int		sz = 0 ;
	    memclear(mip) ;
	    sz = (milen + 1 + mflen + 1) ;
	    if (char *bp ; (rs = libmem.mall(sz,&bp)) >= 0) {
	        mip->a = bp ;
	        mip->nsize = blen ;
	        mip->mid = bp ;
	        bp = (strwcpy(bp,msgid,milen) + 1) ;
	        mip->mfname = bp ;
	        bp = (strwcpy(bp,mfname,mflen) + 1) ;
	    } /* end if (memory-allocation) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mi_start) */

static int mi_finish(MMF_MI *mip) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (mip) {
	    rs = SR_OK ;
	    if ((mip->mfname != nullptr) && (mip->mfname[0] != '\0')) {
	        rs1 = u_unlink(mip->mfname) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (mip->a) {
	        rs1 = libmem.free(mip->a) ;
	        if (rs >= 0) rs = rs1 ;
	        mip->a = nullptr ;
	    }
	    mip->mid = nullptr ;
	    mip->mfname = nullptr ;
	    mip->nsize = 0 ;
	    mip->vsize = 0 ;
	    mip->vlines = 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mi_finish) */

static int mi_newlines(MMF_MI *mip,int n) noex {
	mip->vlines += n ;
	return SR_OK ;
}
/* end subroutine (mi_newlines) */

vars::operator int () noex {
	int		rs ;
	if ((rs = getbufsize(getbufsize_ml)) >= 0) {
	    var.linebuflen = rs ;
	    var.outbuflen = (rs * 2) ;
	} /* end if (getbufsize) */
	return rs ;
}
/* end method (vars::operator) */

static int mkcols(int cols) noex {
	static cchar	*vn = varname.columns ;
	if (cols < MAILMSGFILE_MINCOLS) {
	    static cchar	*cval = getenv(vn) ;
	    if (cval) {
		if (cfdec(cval,-1,&cols) < 0) cols = 0 ;
	    } /* end if (non-null) */
	    if (cols < 2) {
		cols = COLUMNS ;
	    }
	} /* end if (new 'cols' needed) */
	return cols ;
}
/* end subroutine (mkcols) */


