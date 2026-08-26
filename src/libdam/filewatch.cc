/* filewatch SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* watch a file for changes and report */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	filewatch

  	Description:
        This module watches a single file for changes and prints out the
        trailing changes to a file.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<ctime>			/* CSTD |time_t| */
#include	<climits>		/* CSTD |UCHAR_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD |strcmp(3c)| */
#include	<new>			/* C++STD placement-new */
#include	<algorithm>		/* C++STD |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ulogerror.h>		/* LIBU */
#include	<stdfnames.h>		/* LIBU |STDFNIN| */
#include	<intsat.h>		/* LIBU */
#include	<ascii.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<linefilter.h>		/* LIBUC */
#include	<linefold.h>		/* LIBUC */
#include	<lineclean.h>		/* CSTD line-cleaning options */
#include	<linecleanopt.h>	/* CSTD line-cleaning options */
#include	<ischarx.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<bfile.h>		/* LIBB */

#include	"filewatch.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;

/* local defines */

#define	FW		filewatch
#define	FW_ARGS		filewatch_args

#define	LF		linefilter

#define	BF		bfile

#define	TO_OPEN		60


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using libuc::mem ;			/* variable */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
local int filewatch_ctor(filewatch *op,Args ... args) noex {
    	FILEWATCH	*hop = op ;
    	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    memclear(hop) ;
	    rs = SR_NOMEM ;
	    if ((op->wfp = new(nothrow) bfile) != np) ylikely {
		rs = SR_OK ;
	    } /* end if (new-bfile) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (filewatch_ctor) */

local int filewatch_dtor(filewatch *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = SR_OK ;
	    if (op->wfp) ylikely {
		delete op->wfp ;
		op->wfp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (filewatch_dtor) */

template<typename ... Args>
local inline int filewatch_magic(filewatch *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == FILEWATCH_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (filewatch_magic) */

local int	filewatch_fileready(FW *,time_t,cchar *,int) noex ;
local int	filewatch_fileopen(FW *,time_t,cchar *,int) noex ;
local int	filewatch_fileclose(FW *) noex ;
local int	filewatch_stat(FW *,ustat *) noex ;
local int	filewatch_putout(FW *,bfile *,char *,int) noex ;
local int	filewatch_putoutln(FW *,bfile *,int,cchar *,int) noex ;

local int	bopenx(bfile *,cchar *) noex ;


/* local variables */


/* exported variables */

cint	filewatchms::carriage	= (1 << filewatcho_carriage) ;
cint	filewatchms::clean	= (1 << filewatcho_clean) ;

const filewatchms	filewatchm ;


/* exported subroutines */

local int filewatch_starts(FW *op,FW_ARGS *ap) noex ;

int filewatch_start(FW *op,FW_ARGS *ap,LF *lfp,cc *fn) noex {
	int		rs ;
	if ((rs = filewatch_ctor(op,ap,lfp,fn)) >= 0) {
	    rs = SR_INVALID ;
	    if (fn[0]) {
		custime		dt = getustime ;
		op->lfp = lfp ;
		if (char *bp ; (rs = mem.ps(&bp)) >= 0) {
		    op->bufp = bp ;
		    op->bufl = rs ;
		    op->lastcheck = dt ;
		    if ((rs = filewatch_fileopen(op,dt,fn,-1)) >= 0) {
		        if (cc *cp ; (rs = mem.strw(fn,-1,&cp)) >= 0) {
			    op->fname = cp ;
			        if ((rs = filewatch_starts(op,ap)) >= 0) {
				    op->magval = FILEWATCH_MAGIC ;
			        }
			    if (rs < 0) {
				voidp vp = voidp(op->fname) ;
				mem.free(vp) ;
				op->fname = nullptr ;
			    } /* end if (error) */
			} /* end if (memory-acquire) */
			if (rs < 0) {
			    bclose(op->wfp) ;
			} /* end if (error) */
		    } /* end if (filewatch_open) */
		    if (rs < 0) {
			if (op->bufp) {
	    		    mem.free(op->bufp) ;
	    		    op->bufp = nullptr ;
			    op->bufl = 0 ;
			} /* end if (memory-release) */
		    } /* end if (error) */
		} /* end if (memory-acquire) */
	    } /* end if (valid) */
	    if (rs < 0) {
		filewatch_dtor(op) ;
	    } /* end if (error) */
	} /* end if (filewatch_ctor) */
	return rs ;
} /* end subroutine (filewatch_start) */

local int filewatch_starts(FW *op,FW_ARGS *ap) noex {
    	int		rs = SR_FAULT ;
	if (op && ap) {
	    op->interval = (ap->interval > 1) ? ap->interval : 1 ;
	    op->cut = (ap->cut > 0) ? ap->cut : 0 ;
	    op->columns	= max(ap->columns,3) ;
	    op->indent	= max(ap->indent,0) ;
	    op->opts = ap->opts ;
	    op->fl.carriage	= !!(ap->opts & filewatchm.carriage) ;
	    op->fl.clean	= !!(ap->opts & filewatchm.clean) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (filewatch_starts) */

int filewatch_finish(FW *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = filewatch_magic(op)) >= 0) {
	    if (op->fname) {
	        voidp vp = voidp(op->fname) ;
	        rs1 = mem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        op->fname = nullptr ;
	    } /* end if (memory-release) */
	    {
		rs1 = filewatch_fileclose(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->bufp) {
	        rs1 = mem.free(op->bufp) ;
	        if (rs >= 0) rs = rs1 ;
	        op->bufp = nullptr ;
		op->bufl = 0 ;
	    } /* end if (memory-release) */
	    {
	        rs1 = filewatch_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (filewatch_finish) */

local int filewatch_checks(FW *,off_t,bfile *) noex ;
local int filewatch_checkrest(FW *,time_t) noex ;

/* check if our file has changed */
int filewatch_check(FW *op,time_t dt,BF *ofp) noex {
	int		rs = SR_FAULT ;
	int		wlen = 0 ; /* return-value */
	if (op && ofp) ylikely {
	    if (dt < 0) dt = getustime ;
	    if ((dt - op->lastcheck) >= op->interval) {
		op->lastcheck = dt ;
		if (ustat sb ; (rs = filewatch_stat(op,&sb)) >= 0) ylikely {
		    csize	fsize = sb.st_size ;
		    csize	szoff = size_t(op->offset) ;
		    custime	mtime = sb.st_mtime ;
		    coff	foff = sb.st_size ;
		    bool	fchanged = false ;
		    fchanged = fchanged || (mtime > op->lastchange) ;
		    fchanged = fchanged || (foff != op->offset) ;
		    if (fchanged) {
			cchar	*fn = op->fname ;
			int	off = intsat(szoff) ;
			if ((rs = filewatch_fileready(op,dt,fn,off)) >= 0) {
			    if ((rs = filewatch_checks(op,fsize,ofp)) >= 0) {
			        wlen += rs ;
				rs = filewatch_checkrest(op,dt) ;
			    }
			} /* end if (fileready) */
		    } /* end if (condition activated) */
		} /* end if (filewatch_stat) */
	    } /* end if (timeout) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (filewatch_check) */

local int filewatch_checkgt(FW *op,off_t,bfile *) noex ;
local int filewatch_checklt(FW *op,off_t) noex ;

local int filewatch_checks(FW *op,off_t fsize,bfile *ofp) noex {
    	int		rs = SR_OK ;
	int		wlen = 0 ;
	if (fsize > op->offset) {
	    rs = filewatch_checkgt(op,fsize,ofp) ;
	} else if (fsize < op->offset) {
	    rs = filewatch_checklt(op,fsize) ;
	}
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (filewatch_checks) */

local int filewatch_checkgt(FW *op,off_t fsize,bfile *ofp) noex {
    	int		rs = SR_OK ;
	int		rs1 ;
	int		len ;
	int		wlen = 0 ; /* return-value */
	bool		f ;
	(void) fsize ;
        forever {
            int         bl = (op->bufl - op->bi) ;
            char        *bp = (op->bufp + op->bi) ;
            rs = breadln(op->wfp,bp,bl) ;
            len = rs ;
            if (rs <= 0) break ;
            op->bi += len ;
            if ((len >= op->bufl) || (op->bufp[op->bi - 1] == '\n')) {
                f = true ;
                if (op->lfp != nullptr) {
                    rs1 = linefilter_check(op->lfp,op->bufp,op->bi) ;
                    f = (rs1 > 0) ;
                } /* end if */
                if (f) {
                    rs = filewatch_putout(op,ofp,op->bufp,op->bi) ;
                    wlen += rs ;
                    if (rs < 0) break ;
                } /* end if */
                op->bi = 0 ;
            } /* end if (any output needed) */
            op->offset += len ;
        } /* forever (processing lines) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (filewatch_checkgt) */

local int filewatch_checklt(FW *op,off_t fsize) noex {
    	bfile		*wfp = op->wfp ;
    	int		rs ;
	if ((rs = wfp->seek(fsize,SEEK_SET)) >= 0) {
	    op->offset = fsize ;
	}
	return rs ;
} /* end subroutine (filewatch_checklt) */

local int filewatch_checkrest(FW *op,time_t dt) noex {
    	cint		to = TO_OPEN ;
    	int		rs ;
	if (op->fl.open && ((dt - op->opentime) > to)) {
	    bool	f = false ;
	    f = f || (strcmp(op->fname,"-") == 0) ;
	    f = f || (strcmp(op->fname,STDFNIN) == 0) ;
	    if (! f) {
	        if (ustat sb ; (rs = u_stat(op->fname,&sb)) >= 0) {
	            f = f || (sb.st_ino != op->ino) ;
		    f = f || (sb.st_dev != op->dev) ;
		    if (f) {
	                rs = filewatch_fileclose(op) ;
		    }
		} else if (isNotPresent(rs)) {
	            rs = filewatch_fileclose(op) ;
		}
	    } /* end if (not STDIN) */
	} /* end if (timed-out) */
	return rs ;
} /* end subroutine (filewatch_checkrest) */

local int filewatch_readhave(FW *,char *,int) noex ;
local int filewatch_readget(FW *,char *,int,time_t) noex ;

int filewatch_readln(FW *op,time_t dt,char *lbuf,int llen) noex {
	int		rs = SR_FAULT ;
	int		wlen = 0 ; /* return-value */
	if (op && lbuf) ylikely {
	    rs = SR_INVALID ;
	    if (llen >= 0) ylikely {
		if (op->ll > 0) {
		    rs = filewatch_readhave(op,lbuf,llen) ;
		    wlen = rs ;
		} else {
		    rs = filewatch_readget(op,lbuf,llen,dt) ;
		    wlen = rs ;
		}
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (filewatch_readln) */

local int filewatch_readhave(FW *op,char *lbuf,int llen) noex {
    	int		rs = SR_OK ;
	int		wlen = 0 ;
	if (op->ll > 0) {
	    cint ml = min(llen,op->ll) ;
	    memcopy(lbuf,op->lp,ml) ;
	    op->ll -= ml ;
	    wlen = ml ;
	}
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (filewatch_readhave) */

local int filewatch_readproc(FW *,off_t,char *,int) noex ;

local int filewatch_readget(FW *op,char *lbuf,int llen,time_t dt) noex {
    	int		rs = SR_OK ;
	int		wlen = 0 ;
	if ((dt - op->lastcheck) >= op->interval) {
	    op->lastcheck = dt ;
	    if (ustat sb ; (rs = filewatch_stat(op,&sb)) >= 0) {
		bool f = false ;
		f = f || (sb.st_mtime > op->lastchange) ;
		f = f || (sb.st_size != op->offset) ;
		if (f) {
		    coff	fsize = sb.st_size ;
	    	    cchar	*fn = op->fname ;
		    if ((rs = filewatch_fileready(op,dt,fn,-1z)) >= 0) {
			rs = filewatch_readproc(op,fsize,lbuf,llen) ;
			wlen = rs ;
			op->lastchange = dt ;
		    } /* end if (filewatch_fileready) */
		} /* end if (change) */
	    } /* end if (stat) */
	} /* end if (interfacel timeout) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (filewatch_readget) */

local int filewatch_readproc(FW *op,off_t fsize,char *lbuf,int llen) noex {
    	int		rs = SR_OK ;
	int		wlen = 0 ;
	    if (fsize > op->offset) {
	        int	bl = (op->bufl - op->bi) ;
	        char	*bp = (op->bufp + op->bi) ;
	        if ((rs = breadln(op->wfp,bp,bl)) > 0) {
	            cint	len = rs ;
	            op->offset += len ;
	            op->bi += len ;
	            if ((len >= op->bufl) ||
	                (op->bufp[op->bi - 1] == '\n')) {
	                cint ml = min(llen,op->bi) ;
	                memcopy(lbuf,op->bufp,ml) ;
	                wlen += ml ;
	                op->lp = op->bufp + ml ;
	                op->ll = op->bi - ml ;
	                op->bi = 0 ;
	            } /* end if (got a line) */
	        } /* end if (got some data) */
	    } else if (fsize < op->offset) {
	        if ((rs = bseek(op->wfp,fsize,SEEK_SET)) >= 0) {
	            op->offset = fsize ;
		}
	    } /* end if */
	    return (rs >= 0) ? wlen : rs ;
} /* end subroutine (filewatch_readproc) */


/* private subroutines */

local int filewatch_fileready(FW *op,time_t dt,cc *fn,int off) noex {

	int		rs = SR_OK ;
	if (! op->fl.open) {
	    rs = filewatch_fileopen(op,dt,fn,off) ;
	}
	return rs ;
} /* end subroutine (filewatch_fileready) */

local int filewatch_fileopen(FW *op,time_t dt,cc *fn,int off) noex {
	int		rs = SR_OK ;
	if (! op->fl.open) {
	    bfile *wfp = op->wfp ;
	    if ((rs = bopenx(wfp,fn)) >= 0) {
		op->fl.open = true ;
		if ((rs = wfp->control(BC_LINEBUF,0)) >= 0) {
		    off_t	offset ;
		    off_t	boffset ;
		    op->offset = off ;
		    if (off >= 0) {
	    	        boffset = off_t(off) ;
	    	        rs = wfp->seek(boffset,SEEK_SET) ;
		    } else {
	    	        if ((rs = wfp->seek(0z,SEEK_END)) >= 0) {
	        	    rs = wfp->tell(&boffset) ;
	        	    offset = boffset ;
	        	    op->offset = offset ;
	    	        }
	            } /* end if */
		    if (rs >= 0) {
			cint cmd = BC_STAT ;
			if (ustat sb ; (rs = wfp->control(cmd,&sb)) >= 0) {
			    op->lastchange = sb.st_mtime ;
			    op->ino = sb.st_ino ;
			    op->dev = sb.st_dev ;
			    op->opentime = dt ;
			} /* end if (bcontrol) */
		    } /* end if (OK) */
		} /* end if (bcontrol) */
	    } /* end if (bopenx) */
	} /* end if (needed opening) */
	return rs ;
} /* end subroutine (filewatch_fileopen) */

local int filewatch_fileclose(FW *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->fl.open) {
	    op->fl.open = false ;
	    rs1 = bclose(op->wfp) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
} /* end subroutine (filewatch_fileclose) */

local int filewatch_stat(FW *op,ustat *sbp) noex {
    	int		rs ;
	if (op->fl.open) {
	    rs = bcontrol(op->wfp,BC_STAT,sbp) ;
	} else {
	    rs = u_stat(op->fname,sbp) ;
	}
	return rs ;
} /* end subroutine (filewatch_stat) */

local int filewatch_putout(FW *op,BF *ofp,char *bufp,int bufl) noex {
    	cint		cols = op->columns ;
	cint		inds = op->indent ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		wlen = 0 ; /* return-value */
	if (bufl > op->cut) {
	    cint	ll = (bufl - op->cut) ;
	    cchar	*lp = (bufp + op->cut) ;
	    if (ll > 0) {
	        if (linefold lf ; (rs = lf.start(cols,inds,lp,ll)) >= 0) {
	            int		sl ;
	            cchar	*sp ;
	            for (int i = 0 ; (sl = lf.get(i,&sp)) >= 0 ; i += 1) {
	                if (sl > 0) {
	                    rs = filewatch_putoutln(op,ofp,i,sp,sl) ;
	                    wlen += rs ;
		        }
		        if (rs < 0) break ;
	            } /* end for */
	            rs1 = lf.finish ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (linefold) */
	        if (rs >= 0) {
	            rs = bflush(ofp) ;
		} /* end if (ok) */
	    } /* end if (non-zero positive) */
	} /* end if (soething) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (filewatch_putout) */

local int filewatch_putoutln(FW *op,BF *ofp,int nline,cc *lp,int ll) noex {
    	cint		m = (linecleanoptm.nonsub | linecleanoptm.nulsub) ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ; /* return-value */
	cchar		*cp ;
	if (lineclean lc ; (rs = lc.start(lp,ll,m,&cp)) >= 0) {
	    cint	cl = rs ;
	    if ((rs >= 0) && op->fl.carriage && (cp[0] != '\r')) {
	        rs = ofp->putc('\r') ;
	        wlen += rs ;
	    }
	    if ((rs >= 0) && (nline > 0) && (op->indent > 0)) {
	        rs = ofp->writeblanks(op->indent) ;
	        wlen += rs ;
	    }
	    if (rs >= 0) {
	        rs = ofp->write(cp,cl) ;
	        wlen += rs ;
	    }
	    if ((rs >= 0) && op->fl.carriage) {
	        rs = ofp->putc('\r') ;
	        wlen += rs ;
	    }
	    if (rs >= 0) {
	        rs = ofp->putc('\n') ;
	        wlen += rs ;
	    }
	    rs1 = lc.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (filewatch_putoutln) */

int filewatch::check(time_t dt,BF *ofp) noex {
	return filewatch_check(this,dt,ofp) ;
} /* end method */

int filewatch::readln(time_t dt,char *lbuf,int llen) noex {
	return filewatch_readln(this,dt,lbuf,llen) ;
} /* end method */

void filewatch::dtor() noex {
	if (cint rs = int(finish) ; rs < 0) {
	    ulogerror("filewatch",rs,"fini-finish") ;
	}
} /* end method (filewatch::dtor) */

filewatch_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case filewatchmem_finish:
	        rs = filewatch_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
} /* end method (filewatch_co::operator) */

local int bopenx(bfile *wfp,cchar *fn) noex {
	if (strcmp(fn,"-") == 0) fn = STDFNIN ;
	return bopen(wfp,fn,"r",0) ;
} /* end subroutine (bopenx) */


