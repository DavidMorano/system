/* eigendb SUPPORT */
/* lang=C++20 */

/* store eigen words in a database */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This module stores eigen words in a database for later
	convenient access.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/stat.h>
#include	<sys/mman.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<estrings.h>
#include	<strpack.h>
#include	<hdb.h>
#include	<filer.h>
#include	<naturalwords.h>
#include	<intceil.h>
#include	<strn.h>
#include	<six.h>
#include	<sfx.h>
#include	<sif.hh>
#include	<strwcpyx.h>
#include	<linebuffer.h>
#include	<char.h>
#include	<hasx.h>
#include	<localmisc.h>

#include	"eigendb.h"


/* local defines */

#define	EIGENDB_DEFENT		150
#define	EIGENDB_CHUNKSIZE	720
#define	EIGENDB_MAXFILESIZE	(40 * 1024 * 1024)

#ifndef	KEYBUFLEN
#ifdef	NATURALWORDLEN
#define	KEYBUFLEN	NATURALWORDLEN
#else
#define	KEYBUFLEN	80
#endif
#endif

#define	TO_READ		5


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static int eigendb_ctor(eigendb *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    op->magic = 0 ;
	    if ((op->spp = new(nothrow) strpack) != np) {
	        if ((op->dbp = new(nothrow) hdb) != np) {
		    rs = SR_OK ;
	        } /* end if (new-hdb) */
		if (rs < 0) {
		    delete op->spp ;
		    op->spp = nullptr ;
		}
	    } /* end if (new-strapck) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (eigendb_ctor) */

static int eigendb_dtor(eigendb *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->dbp) {
		delete op->dbp ;
		op->dbp = nullptr ;
	    }
	    if (op->spp) {
		delete op->spp ;
		op->spp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (eigendb_dtor) */

template<typename ... Args>
static int eigendb_magic(eigendb *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == EIGENDB_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (eigendb_magic) */

static int	eigendb_fileparse(eigendb *,cchar *) noex ;
static int	eigendb_fileparsereg(eigendb *,int,int) noex ;
static int	eigendb_fileparsemap(eigendb *,int,int) noex ;
static int	eigendb_fileline(eigendb *,cchar *,int) noex ;


/* local variables */

constexpr int	keybuflen = KEYBUFLEN ;


/* exported variables */


/* exported subroutines */

int eigendb_open(eigendb *op,cchar *fname) noex {
	int		rs ;
	if ((rs = eigendb_ctor(op)) >= 0) {
	    cint	ne = EIGENDB_DEFENT ;
	    cint	chsz = EIGENDB_CHUNKSIZE ;
	    if ((rs = strpack_start(op->spp,chsz)) >= 0) {
		cnullptr	np{} ;
	        if ((rs = hdb_start(op->dbp,ne,1,np,np)) >= 0) {
	            op->magic = EIGENDB_MAGIC ;
	            if (fname) {
		        if (fname[0] != '\0') {
	                    rs = eigendb_fileparse(op,fname) ;
		        } else {
			    rs = SR_INVALID ;
			}
	            } /* end if (non-null) */
	            if (rs < 0) {
	                hdb_finish(op->dbp) ;
	            }
	        } /* end if (hdb-start) */
	        if (rs < 0) {
	            strpack_finish(op->spp) ;
	            op->magic = 0 ;
	        }
	    } /* end if (strpack-start) */
	    if (rs < 0) {
		eigendb_dtor(op) ;
	    }
	} /* end if (eigendb_ctor) */
	return rs ;
}
/* end subroutine (eigendb_open) */

int eigendb_close(eigendb *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = eigendb_magic(op)) >= 0) {
	    {
	        rs1 = hdb_finish(op->dbp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = strpack_finish(op->spp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = eigendb_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (eigendb_close) */

int eigendb_addfile(eigendb *op,cchar *fname) noex {
	int		rs ;
	if ((rs = eigendb_magic(op,fname)) >= 0) {
	    rs = eigendb_fileparse(op,fname) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (eigendb_addfile) */

int eigendb_addword(eigendb *op,cchar *wp,int wl) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = eigendb_magic(op,wp)) >= 0) {
	    if (wl < 0) wl = strlen(wp) ;
	    if (wl > 0) {
		cnullptr	np{} ;
		cint		rsn = SR_NOTFOUND ;
	        hdb_dat		key ;
	        hdb_dat		value ;
	        char		keybuf[keybuflen + 1] ;
	        if (hasuc(wp,wl)) {
	            if (wl > keybuflen) wl = keybuflen ;
	            strwcpylc(keybuf,wp,wl) ;
	            wp = keybuf ;
	        }
	        key.buf = wp ;
	        key.len = wl ;
	        if ((rs = hdb_fetch(op->dbp,key,np,np)) == rsn) {
		    cchar	*ep ;
	            if ((rs = strpack_store(op->spp,wp,wl,&ep)) >= 0) {
		        c = 1 ;
	                key.buf = ep ;
	                key.len = wl ;
		        value.buf = ep ;
		        value.len = wl ;
	                rs = hdb_store(op->dbp,key,value) ;
		    } /* end if (strpack-store) */
	        } /* end if (hdb-fetch) */
	    } /* end if (non-zero word) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (eigendb_addword) */

int eigendb_exists(eigendb *op,cchar *wp,int wl) noex {
	int		rs ;
	if ((rs = eigendb_magic(op,wp)) >= 0) {
	    if (wl < 0) wl = strlen(wp) ;
	    if (wl > 0) {
		cnullptr	np{} ;
	        hdb_dat		key ;
	        char		keybuf[keybuflen + 1] ;
	        if (hasuc(wp,wl)) {
	            if (wl > keybuflen) wl = keybuflen ;
	            strwcpylc(keybuf,wp,wl) ;
	            wp = keybuf ;
	        }
	        key.buf = wp ;
	        key.len = wl ;
	        rs = hdb_fetch(op->dbp,key,np,np) ;
	    } else {
	        rs = SR_INVALID ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (eigendb_exists) */

int eigendb_count(eigendb *op) noex {
	int		rs ;
	if ((rs = eigendb_magic(op)) >= 0) {
	    rs = hdb_count(op->dbp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (eigendb_count) */

int eigendb_curbegin(eigendb *op,eigendb_cur *curp) noex {
	int		rs ;
	if ((rs = eigendb_magic(op,curp)) >= 0) {
	    rs = SR_NOMEM ;
	    if ((curp->hcp = new(hdb_cur)) != nullptr) {
		rs = hdb_curbegin(op->dbp,curp->hcp) ;
	    } /* end if (new-hdb_cur) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (eigendb_curbegin) */

int eigendb_curend(eigendb *op,eigendb_cur *curp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = eigendb_magic(op,curp)) >= 0) {
	    {
	        rs1 = hdb_curend(op->dbp,curp->hcp) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
		delete curp->hcp ;
		curp->hcp = nullptr ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (eigendb_curend) */

int eigendb_enum(eigendb *op,eigendb_cur *curp,cchar **rpp) noex {
	int		rs ;
	int		len = 0 ;
	cchar		*rp = nullptr ;
	if ((rs = eigendb_magic(op,curp)) >= 0) {
	    rs = SR_BUGCHECK ;
	    if (curp->hcp) {
		hdb_cur		*hcp = curp->hcp ;
	        hdb_dat 	key ;
	        hdb_dat		val ;
	        if (rpp) *rpp = nullptr ;
	        if ((rs = hdb_enum(op->dbp,hcp,&key,&val)) >= 0) {
	            len = key.len ;
		    rp = charp(key.buf) ;
	        }
	    } /* end if (valid HDB cursor) */
	} /* end if (magic) */
	if (rpp) *rpp = rp ;
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (eigendb_enum) */


/* private subroutines */

static int eigendb_fileparse(eigendb *op,cchar *fname) noex {
	cint		of = O_RDONLY ;
	int		rs ;
	int		rs1 ;
	cmode		om = 0666 ;
	if ((rs = uc_open(fname,of,om)) >= 0) {
	    USTAT	sb ;
	    cint	fd = rs ;
	    if ((rs = u_fstat(fd,&sb)) >= 0) {
	        if (! S_ISDIR(sb.st_mode)) {
	            csize	mfsize = EIGENDB_MAXFILESIZE ;
	            csize	fsize = sb.st_size ;
		    if (fsize > 0) {
			cint	fsz = int(fsize) ;
	                if (S_ISREG(sb.st_mode) && (fsize <= mfsize)) {
	                    rs = eigendb_fileparsemap(op,fd,fsz) ;
	                } else {
	                    rs = eigendb_fileparsereg(op,fd,fsz) ;
			}
		    } /* end if (non-zero positive) */
	 	} else {
		    rs = SR_ISDIR ;
		} /* end if (not-directory) */
	    } /* end if (stat) */
	    rs1 = u_close(fd) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (open-file) */
	return rs ;
}
/* end subroutine (eigendb_fileparse) */

static int eigendb_fileparsereg(eigendb *op,int fd,int fsize) noex {
	linebuffer	lb ;
	cint		to = TO_READ ;
	int		rs ;
	int		rs1 ;
	int		bsize = 0 ;
	int		c = 0 ;
	if (fsize >= 0) {
	    bsize = iceil(fsize,1024) ;
	}
	if ((rs = lb.start) >= 0) {
	    filer	fb ;
	    if ((rs = filer_start(&fb,fd,0L,bsize,0)) >= 0) {
	        cint	llen = lb.llen ;
	        char	*lbuf = lb.lbuf ;
	        while ((rs = filer_readln(&fb,lbuf,llen,to)) > 0) {
		    rs = eigendb_fileline(op,lbuf,rs) ;
		    c += rs ;
		    if (rs < 0) break ;
	        } /* end while (lines) */
	        rs1 = filer_finish(&fb) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (filer) */
	    rs1 = lb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (linebuffer) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (eigendb_fileparsereg) */

static int eigendb_fileparsemap(eigendb *op,int fd,int fsize) noex {
	cnullptr	np{} ;
	csize		ms = size_t(fsize) ;
	int		rs ;
	int		rs1 ;
	int		mp = PROT_READ ;
	int		mf = MAP_SHARED ;
	int		c = 0 ;
	void		*md{} ;
	if ((rs = u_mmapbegin(np,ms,mp,mf,fd,0z,&md)) >= 0) {
	    char	*sp = charp(md) ;
	    int		sl = int(ms) ;
	    int		si ;
	    while ((si = siochr(sp,sl,'\n')) >= 0) {
		rs = eigendb_fileline(op,sp,si) ;
		c += rs ;
		sl -= (si+1) ;
		sp += (si+1) ;
		if (rs < 0) break ;
	    } /* end while */
	    rs1 = u_mmapend(md,ms) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (map-file) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (eigendb_fileparsemap) */

static int eigendb_fileline(eigendb *op,cchar *lbuf,int llen) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	cchar		*sp{} ;
	if (int sl ; (sl = sfcontent(lbuf,llen,&sp)) > 0) {
	    sif		sfo(sp,sl) ;
	    int		cl ;
	    cchar	*cp{} ;
	    while ((cl = sfo.next(&cp)) > 0) {
		c += 1 ;
		rs = eigendb_addword(op,cp,cl) ;
		if (rs < 0) break ;
	    } /* end while */
	} /* end if (sfcontent) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (eigendb_fileline) */


