/* mailmsgfile SUPPORT */
/* lang=C++20 */

/* maintain translations for MSGID to filenames */
/* version %I% last-modified %G% */


/* revision history:

	= 2009-01-20, David A­D­ Morano
	This is new code.

*/

/* Copyright © 2009 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

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
#include	<cstdlib>
#include	<cstdarg>
#include	<cstring>
#include	<ctime>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<hdb.h>
#include	<filebuf.h>
#include	<linefold.h>
#include	<fsdir.h>
#include	<vecstr.h>
#include	<vecpstr.h>
#include	<upt.h>
#include	<ncol.h>		/* |ncolstr(3uc)| */
#include	<tabcols.h>
#include	<mkpathx.h>
#include	<pathadd.h>
#include	<cfdec.h>
#include	<char.h>
#include	<localmisc.h>
#include	<exitcodes.h>

#include	"mailmsgfile.h"


/* local defines */

#define	MMF_MINCOLS	38
#define	MAILMSGFILE_DEFIND	4
#define	MAILMSGFILE_FILEINT	(2 * 24 * 3600)

#define	MMF			mailmsgfile
#define	MMF_MI			mailmsgfile_mi

#ifndef	LINEBUFLEN
#ifdef	LINE_MAX
#define	LINEBUFLEN	MAX(LINE_MAX,2048)
#else
#define	LINEBUFLEN	2048
#endif
#endif

#ifndef	TABCOLS
#define	TABCOLS		8
#endif

#ifndef	VARCOLUMNS
#define	VARCOLUMNS	"COLUMNS"
#endif

#ifndef	COLUMNS
#define	COLUMNS		80
#endif

#define	INBUFLEN	LINEBUFLEN
#define	OUTBUFLEN	(INBUFLEN * 2)


/* imported namespaces */


/* local typedefs */


/* external subroutines */

extern int	opentmpfile(cchar *,int,mode_t,char *) ;
extern int	mkdisplayable(char *,int,cchar *,int) ;
extern int	filebuf_writeblanks(filebuf *,int) ;

extern char	*strwcpy(char *,cchar *,int) ;
extern char	*strwcpyblanks(char *,int) ;
extern char	*strncpylc(char *,cchar *,int) ;
extern char	*strnchr(cchar *,int,int) ;
extern char	*strnrchr(cchar *,int,int) ;
extern char	*strnrpbrk(cchar *,int,cchar *) ;


/* external variables */


/* local structures */

struct dargs {
	hdb		*mp ;
	cchar		*tmpdname ;
} ;


/* forward references */

static int mailmsgfile_mk(MMF *,cchar *,cchar *,int,off_t,int) noex ;
static int mailmsgfile_mkdis(MMF *,MMF_MI *,int,int,off_t) noex ;
static int mailmsgfile_proclines(MMF *,MMF_MI *,filebuf *,cchar *,int) noex ;

static int mailmsgfile_procout(MMF *,filebuf *,int,cchar *,int,int) noex ;
static int mailmsgfile_store(MMF *,MMF_MI *) noex ;
static int mailmsgfile_filefins(MMF *) noex ;

static int mailmsgfile_checkbegin(MMF *) noex ;
static int mailmsgfile_checkend(MMF *) noex ;
static int mailmsgfile_checkout(MMF *) noex ;
static int mailmsgfile_checker(MMF *) noex ;
static int mailmsgfile_checkerx(MMF *) noex ;
static int mailmsgfile_checkerxx(MMF *,vecpstr *,char *,int) noex ;

static int mi_start(MMF_MI *,cchar *,cchar *,int) noex ;
static int mi_finish(MMF_MI *) noex ;
static int mi_newlines(MMF_MI *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int mailmsgfile_start(MMF *op,cc *tmpdname,int cols,int ind) noex {
	int		rs = SR_OK ;
	int		ne = 30 ;
	cchar		*cp ;

	if (op == nullptr) return SR_FAULT ;

	if (tmpdname == nullptr) return SR_INVALID ;

	if (cols < MMF_MINCOLS) {
	    if ((cp = getenv(VARCOLUMNS)) != nullptr) {
	        uint	v ;
	        int	rs1 = cfdecui(cp,-1,&v) ;
	        cols = (rs1 >= 0) ? v : 0 ;
	    }
	    if (cols < 2) {
	        cols = COLUMNS ;
	    }
	} /* end if */

	if (ind < 0) ind = MAILMSGFILE_DEFIND ;

	memclear(op) ;
	op->cols = cols ;
	op->ind = ind ;
	op->pagesize = getpagesize() ;

	if ((rs = uc_mallocstrw(tmpdname,-1,&cp)) >= 0) {
	    cnullptr	np{} ;
	    op->tmpdname = cp ;
	    if ((rs = hdb_start(&op->files,ne,true,np,np)) >= 0) {
	        op->f.files = true ;
	        if ((rs = mailmsgfile_checkbegin(op)) >= 0) {
	            op->magic = MAILMSGFILE_MAGIC ;
	        }
	    }
	    if (rs < 0) {
	        uc_free(op->tmpdname) ;
	        op->tmpdname = nullptr ;
	    }
	} /* end if (memory-allocation) */

	return rs ;
}
/* end subroutine (mailmsgfile_start) */

int mailmsgfile_finish(MMF *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != MAILMSGFILE_MAGIC) return SR_NOTOPEN ;

	rs1 = mailmsgfile_checkend(op) ;
	if (rs >= 0) rs = rs1 ;

	rs1 = mailmsgfile_filefins(op) ;
	if (rs >= 0) rs = rs1 ;

	if (op->f.files) {
	    op->f.files = false ;
	    rs1 = hdb_finish(&op->files) ;
	    if (rs >= 0) rs = rs1 ;
	}

	if (op->tmpdname != nullptr) {
	    rs1 = uc_free(op->tmpdname) ;
	    if (rs >= 0) rs = rs1 ;
	    op->tmpdname = nullptr ;
	}

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (mailmsgfile_finish) */


int mailmsgfile_new(op,type,msgid,mfd,boff,blen)
MAILMSGFILE	*op ;
int		type ;
cchar	msgid[] ;
int		mfd ;
off_t	boff ;
int		blen ;
{
	hdb_dat	key, val ;
	int		rs ;
	int		vlines = 0 ;

	if (op == nullptr) return SR_FAULT ;
	if (msgid == nullptr) return SR_FAULT ;

	if (op->magic != MAILMSGFILE_MAGIC) return SR_NOTOPEN ;

	if (type < 0) return SR_INVALID ;
	if (msgid[0] == nullptr) return SR_INVALID ;
	if (boff < 0) return SR_INVALID ;
	if (blen < 0) return SR_INVALID ;

	if (mfd < 0) return SR_BADF ;

	key.buf = msgid ;
	key.len = strlen(msgid) ;
	if ((rs = hdb_fetch(&op->files,key,nullptr,&val)) >= 0) {
	    MMF_MI	*mip = (MMF_MI *) val.buf ;
	    vlines = mip->vlines ;
	} else if (rs == SR_NOTFOUND) {
	    cchar	*template = "msgXXXXXXXXXXX" ;
	    char	infname[MAXPATHLEN + 1] ;
	    if ((rs = mkpath2(infname,op->tmpdname,template)) >= 0) {
	        rs = mailmsgfile_mk(op,msgid,infname,mfd,boff,blen) ;
	        vlines = rs ;
	    }
	} /* end if (found or not) */

	return (rs >= 0) ? vlines : rs ;
}
/* end subroutine (mailmsgfile_new) */


int mailmsgfile_msginfo(op,mipp,msgid)
MAILMSGFILE	*op ;
MMF_MI	**mipp ;
cchar	msgid[] ;
{
	int		rs ;
	int		vlines = 0 ;

	if (op == nullptr) return SR_FAULT ;
	if (msgid == nullptr) return SR_FAULT ;
	if (mipp == nullptr) return SR_FAULT ;

	if (op->magic != MAILMSGFILE_MAGIC) return SR_NOTOPEN ;

	if (msgid[0] == '\0') return SR_INVALID ;

	if ((rs = mailmsgfile_checkout(op)) >= 0) {
	    hdb_dat	key, val ;
	    key.buf = msgid ;
	    key.len = strlen(msgid) ;
	    if ((rs = hdb_fetch(&op->files,key,nullptr,&val)) >= 0) {
	        *mipp = (MMF_MI *) val.buf ;
	        vlines = (*mipp)->vlines ;
	    }
	} /* end if (checkout) */

	return (rs >= 0) ? vlines : rs ;
}
/* end subroutine (mailmsgfile_msginfo) */


int mailmsgfile_get(op,msgid,rpp)
MAILMSGFILE	*op ;
cchar	msgid[] ;
cchar	**rpp ;
{
	MMF_MI	*mip ;
	int		rs ;
	int		vlines = 0 ;

	if (op == nullptr) return SR_FAULT ;
	if (msgid == nullptr) return SR_FAULT ;

	if (op->magic != MAILMSGFILE_MAGIC) return SR_NOTOPEN ;

	if (msgid[0] == nullptr) return SR_INVALID ;

	if ((rs = mailmsgfile_msginfo(op,&mip,msgid)) >= 0) {
	    vlines = mip->vlines ;
	    if (rpp != nullptr)
	        *rpp = (cchar *) mip->mfname ;
	}

	return (rs >= 0) ? vlines : rs ;
}
/* end subroutine (mailmsgfile_get) */


/* private subroutines */


static int mailmsgfile_mk(op,msgid,infname,mfd,boff,blen)
MAILMSGFILE	*op ;
cchar	msgid[] ;
cchar	infname[] ;
int		mfd ;
off_t	boff ;
int		blen ;
{
	MMF_MI	mi ;
	const mode_t	omode = 0666 ;
	cint	oflags = (O_WRONLY | O_CREAT) ;
	int		rs ;
	int		vlines = 0 ;
	char		mfname[MAXPATHLEN + 1] ;

	mfname[0] = '\0' ;
	if ((rs = opentmpfile(infname,oflags,omode,mfname)) >= 0) {
	    int	tfd = rs ;
	    if ((rs = u_seek(mfd,boff,SEEK_SET)) >= 0) {
	        if ((rs = mi_start(&mi,msgid,mfname,blen)) >= 0) {
	            if ((rs = mailmsgfile_mkdis(op,&mi,tfd,mfd,boff)) >= 0) {
	                vlines = rs ;
	                rs = mailmsgfile_store(op,&mi) ;
	            }
	            if (rs < 0)
	                mi_finish(&mi) ;
	        } /* end if (mi) */
	    } /* end if (seek) */
	    u_close(tfd) ;
	    if (rs < 0)
	        u_unlink(mfname) ;
	} /* end if (tmpfile) */

	return (rs >= 0) ? vlines : rs ;
}
/* end subroutine (mailmsgfile_mk) */


static int mailmsgfile_filefins(op)
MAILMSGFILE	*op ;
{
	HDB		*mp = &op->files ;
	hdb_dat	key, val ;
	HDB_CUR		cur ;
	int		rs = SR_OK ;
	int		rs1 ;

	if ((rs1 = hdb_curbegin(mp,&cur)) >= 0) {
	    MMF_MI	*mip ;
	    while (hdb_enum(mp,&cur,&key,&val) >= 0) {
	        mip = (MMF_MI *) val.buf ;
	        if (mip != nullptr) {
	            rs1 = mi_finish(mip) ;
	            if (rs >= 0) rs = rs1 ;
	            rs1 = uc_free(mip) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if */
	    } /* end while */
	    hdb_curend(mp,&cur) ;
	} /* end if (cursor) */
	if (rs >= 0) rs = rs1 ;

	return rs ;
}
/* end subroutine (mailmsgfile_filefins) */


static int mailmsgfile_mkdis(op,mip,tfd,mfd,boff)
MAILMSGFILE	*op ;
MMF_MI	*mip ;
off_t	boff ;
int		tfd ;
int		mfd ;
{
	filebuf		in ;
	cint	ntab = TABCOLS ;
	cint	blen = mip->nsize ;
	int		rs ;
	int		inlen ;
	int		ibsize ;
	int		vlines = 0 ;
	int		wlen = 0 ;

	ibsize = MIN(blen,(op->pagesize*8)) ;
	if ((rs = filebuf_start(&in,mfd,boff,ibsize,0)) >= 0) {
	    filebuf	out ;
	    cint	obsize = rs ;
	    int		rlen = blen ;

	    if ((rs = filebuf_start(&out,tfd,0L,obsize,0)) >= 0) {
	        cint	llen = LINEBUFLEN ;
		int		ncols ;
	        int		ll ;
	        char		lbuf[INBUFLEN + 1] ;

	        while ((rs >= 0) && (rlen > 0)) {
	            rs = filebuf_readln(&in,lbuf,llen,0) ;
	            inlen = rs ;
	            if (rs <= 0) break ;

	            ll = inlen ;
	            if (lbuf[ll-1] == '\n') {
	                mip->nlines += 1 ;
	                ll -= 1 ;
	            }

	            while ((ll > 0) && CHAR_ISWHITE(lbuf[ll-1]))
	                ll -= 1 ;

/* calculate the number of columns this line would take up */

	            ncols = ncolstr(ntab,0,lbuf,ll) ;

/* take action based on whether the line fits in the available columns */

	            if (ncols > op->cols) {
	                rs = mailmsgfile_proclines(op,mip,&out,lbuf,ll) ;
	                wlen += rs ;
	            } else {
	                if ((rs = mi_newlines(mip,1)) >= 0) {
	                    rs = mailmsgfile_procout(op,&out,0,lbuf,ll,false) ;
	                    wlen += rs ;
	                }
	            } /* end if (what kind of printing) */

	            rlen -= inlen ;

	        } /* end while (reading lines) */

	        if (rs >= 0) {
	            mip->vsize = wlen ;
	        }

	        filebuf_finish(&out) ;
	    } /* end if (filebuf) */

	    filebuf_finish(&in) ;
	} /* end if (filebuf) */

	return (rs >= 0) ? vlines : rs ;
}
/* end subroutine (mailmsgfile_mkdis) */

static int mailmsgfile_proclines(MMF *op,MMF_NI *mip,filebuf *fbp,
		char *lbuf,int llen) noex {
	linefold	ff ;
	cint		c = op->cols ;
	cint		ind = op->ind ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	if ((rs = linefold_start(&ff,c,ind,lbuf,llen)) >= 0) {
	    int		li = 0 ;
	    int		plen ;
	    int		sl ;
	    bool	f = false ;
	    cchar	*sp ;
	    while ((rs = linefold_getline(&ff,li,&sp)) > 0) {
		sl = rs ;
	        while ((rs >= 0) && (sl > 0)) {
	            f = false ;
	            plen = sl ;
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


static int mailmsgfile_procout(MMF *op,filebuf *fbp,int li,cc *lp,int ll,
		int f_cont) noex {
	cint		ind = op->ind ;
	int		rs = SR_OK ;
	int		wlen = 0 ;
	int		f_eol = false ;

	if ((li > 0) && (ind > 0)) {
	    rs = filebuf_writeblanks(fbp,ind) ;
	    wlen += rs ;
	}

	if (rs >= 0) {
	    cint	outlen = OUTBUFLEN ;
	    char	outbuf[OUTBUFLEN + 1] ;
	    f_eol = ((ll > 0) && (lp[ll-1] == '\n')) ;
	    if ((rs = mkdisplayable(outbuf,outlen,lp,ll)) >= 0) {
	        cint	olen = rs ;
	        if (f_cont) {
	            if ((olen > 0) && (outbuf[olen-1] == '\n')) {
	                olen -= 1 ;
		    }
	            if ((rs = filebuf_write(fbp,outbuf,olen)) >= 0) {
	                wlen += rs ;
	                f_eol = true ;
	                outbuf[0] = '¬' ;
	                outbuf[1] = '\n' ;
	                rs = filebuf_write(fbp,outbuf,2) ;
	                wlen += rs ;
	            }
	        } else {
	            f_eol = true ;
	            rs = filebuf_println(fbp,outbuf,olen) ;
	            wlen += rs ;
	        }
	    } /* end if (mkdisplayable) */
	    if ((rs >= 0) && (! f_eol)) {
	        outbuf[0] = '\n' ;
	        rs = filebuf_write(fbp,outbuf,1) ;
	        wlen += rs ;
	    }
	} /* end if (ok) */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (mailmsgfile_procout) */

static int mailmsgfile_store(MMF *op,MMF_MI *mip) noex {
	int		rs = SR_FAULT ;
	if (op && mip) {
	    cint	sz = sizeof(MMF_MI) ;
	    MMF_MI	*ep ;
	    if ((rs = uc_malloc(sz,&ep)) >= 0) {
	        hdb_dat	key ;
	        hdb_dat	val ;
	        *ep = *mip ; /* copy */
	        key.buf = ep->mid ;
	        key.len = strlen(ep->mid) ;
	        val.buf = ep ;
	        val.len = sz ;
	        rs = hdb_store(&op->files,key,val) ;
	        if (rs < 0) {
	            uc_free(ep) ;
	        }
	    } /* end if (memory-allocation) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mailmsgfile_store) */

static int mailmsgfile_checkbegin(MMF *op) noex {
	pthread_t	tid ;
	int		rs ;
	int	(*thrsub)(void *) = (int (*)(void *)) mailmsgfile_checker ;
	if ((rs = uptcreate(&tid,nullptr,thrsub,op)) >= 0) {
	    op->tid = tid ;
	    op->f.checkout = true ;
	}
	return rs ;
}
/* end subroutine (mailmsgfile_checkbegin) */

static int mailmsgfile_checkend(MMF *op) noex {
	int		rs = SR_OK ;
	if (op->f.checkout) {
	    int	trs = SR_OK ;
	    if ((rs = uptjoin(op->tid,&trs)) >= 0) {
	        op->f.checkout = false ;
	        rs = trs ;
	    }
	}
	return rs ;
}
/* end subroutine (mailmsgfile_checkend) */

static int mailmsgfile_checkout(MMF *op) noex {
	int		rs = SR_OK ;
	if (op->f.checkout && op->f_checkdone) {
	    int		trs = SR_OK ;
	    if ((rs = uptjoin(op->tid,&trs)) >= 0) {
	        op->f.checkout = false ;
	        rs = trs ;
	    }
	}
	return rs ;
}
/* end subroutine (mailmsgfile_checkout) */

static int mailmsgfile_checker(MMF *op) noex {
	int		rs = SR_OK ;
	if (op->tmpdname != nullptr) {
	    if (op->tmpdname[0] != '\0') {
		msleep(10) ; /* some time for real work to happen elsewhere */
		rs = mailmsgfile_checkerx(op) ;
	    } else {
	        rs = SR_INVALID ;
	    }
	} else {
	    rs = SR_FAULT ;
	}
	op->f_checkdone = true ;
	return rs ;
}
/* end subroutine (mailmsgfile_checker) */

static int mailmsgfile_checkerx(MMF *op) noex {
	const time_t	dt = time(nullptr) ;
	cint		to = MAILMSGFILE_FILEINT ;
	int		rs ;
	int		rs1 ;
	char		pbuf[MAXPATHLEN+1] ;
	if ((rs = malloc_mp(&pbuf)) >= 0) {
	if ((rs = mkpath1(pbuf,op->tmpdname)) >= 0) {
	    vecpstr	files ;
	    int		plen = rs ;
	    if ((rs = vecpstr_start(&files,10,100,0)) >= 0) {
		rs = mailmsgfile_checkerxx(op,&files,pbuf,plen) ;


static int mailmsgfile_checkerxx(MMF *op,&files,char *pbuf,int plen) noex {
	int		rs ;
	int		rs1 ;
	char		*ebuf{} ;
	if ((rs = malloc_mn(&ebuf)) >= 0) {
	    fsdir	dir ;
	    fsdir_ent	de ;
	    cint	rlen = rs ;
	        if ((rs = fsdir_open(&dir,op->tmpdname)) >= 0) {
	            USTAT	sb ;
	            int		pl ;
	            while ((rs = fsdir_read(&dir,&de,ebuf,elen)) > 0) {
	                if (de.name[0] != '.') {
	                    if ((rs = pathadd(pbuf,plen,de.name)) >= 0) {
	                        pl = rs ;
	                        if (u_stat(pbuf,&sb) >= 0) {
	                            if ((dt - sb.st_mtime) >= to) {
	                                rs = vecpstr_add(&files,pbuf,pl) ;
	                            }
	                        } /* end if (stat) */
	                    } /* end if (pathadd) */
		        }
	                if (rs < 0) break ;
	            } /* end while (reading entries) */
	            rs1 = fsdir_close(&dir) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (fsdir) */
	    rs1 = uc_free(ebuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
}


	        if (rs >= 0) {
	            cchar	*fp{} ;
	            for (int i = 0 ; vecpstr_get(&files,i,&fp) >= 0 ; i += 1) {
	                if (fp) {
	                    if (fp[0] != '\0') {
	                        u_unlink(fp) ;
	                    }
			}
	            } /* end for */
	        } /* end if */
	        rs1 = vecpstr_finish(&files) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (vecpstr) */
	} /* end if (mkpath) */
	    rs1 = uc_free(pbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end subroutine (mailmsgfile_checkerx) */

static int mailmsgfile_checkerxx(MMF *op,&files,char *pbuf,int plen) noex {
	int		rs ;
	int		rs1 ;
	char		*ebuf{} ;
	if ((rs = malloc_mn(&ebuf)) >= 0) {
	    fsdir	dir ;
	    fsdir_ent	de ;
	    cint	rlen = rs ;
	        if ((rs = fsdir_open(&dir,op->tmpdname)) >= 0) {
	            USTAT	sb ;
	            int		pl ;
	            while ((rs = fsdir_read(&dir,&de,ebuf,elen)) > 0) {
	                if (de.name[0] != '.') {
	                    if ((rs = pathadd(pbuf,plen,de.name)) >= 0) {
	                        pl = rs ;
	                        if (u_stat(pbuf,&sb) >= 0) {
	                            if ((dt - sb.st_mtime) >= to) {
	                                rs = vecpstr_add(&files,pbuf,pl) ;
	                            }
	                        } /* end if (stat) */
	                    } /* end if (pathadd) */
		        }
	                if (rs < 0) break ;
	            } /* end while (reading entries) */
	            rs1 = fsdir_close(&dir) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (fsdir) */
	    rs1 = uc_free(ebuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end subroutine (mailmsgfile_checkerxx) */

static int mi_start(MMF_NI *mip,cc *msgid,cc *mfname,int blen) noex {
	int		rs = SR_FAULT ;
	if (mip) {
	    cint	milen = strlen(msgid) ;
	    xint	mflen = strlen(mfname) ;
	    int		sz = 0 ;
	    char	*bp = nullptr ;
	    memclear(mip) ;
	    sz = (milen + 1 + mflen + 1) ;
	    if ((rs = uc_malloc(sz,&bp)) >= 0) {
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
	        rs1 = uc_free(mip->a) ;
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


