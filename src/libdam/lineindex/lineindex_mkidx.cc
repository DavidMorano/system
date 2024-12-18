/* lineindex_mkidx SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* line indexing object */
/* version %I% last-modified %G% */


/* revision history:

	= 2003-06-11, David A­D­ Morano
	I snarfed this object (flavor) from the SS-Hammock processing
	and lookup (access management) code.  We will see how it
	works out!

*/

/* Copyright © 2003 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Object:
	lineindex

	Description:
	This object module manages a line-index database.  It can
	also create such a database if it is opened with the O_CREAT
	option.  Note that line indexing is so fast that trying to
	super-optimize anything here is not really worth it.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>
#include	<sys/mman.h>		/* Memory Management */
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>		/* |INT_MAX| */
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* <- for |memchr(3c)| */
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<sysval.hh>
#include	<bufsizevar.hh>
#include	<getbufsize.h>
#include	<opentmp.h>		/* |opentmpfile(3uc)| */
#include	<endian.h>
#include	<vecint.h>
#include	<storebuf.h>
#include	<mkpathx.h>
#include	<sfx.h>
#include	<intceil.h>
#include	<intsat.h>
#include	<isnot.h>
#include	<localmisc.h>		/* |MODP2| */

#include	"lineindex.h"
#include	"lineindexhdr.h"


/* local defines */

#define	LI		lineindex
#define	LI_VERSION	LINEINDEX_FILEVERSION
#define	LI_TYPE		LINEINDEX_FILETYPE

#define	TO_FILECOME	2
#define	TO_OPEN		(60 * 60)
#define	TO_MAP		(1 * 60)
#define	TO_CHECK	4
#define	TO_ACCESS	(1 * 60)


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef mode_t		om_t ;		/* open-mode */
typedef uint *		uintp ;


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct idxer {
	lineindex	*op ;
	vecint		recs ;
	char		*a{} ;
	char		*tbuf{} ;
	char		*pbuf{} ;
	int		tlen ;
	int		plen ;
	int		tfd = -1 ;
	idxer(lineindex *p) noex : op(p) { } ;
	operator int () noex ;
	int tmpbegin() noex ;
	int tmpend() noex ;
	int scaner() noex ;
	int scanlines(cchar *) noex ;
	int liner(int,size_t) noex ;
	int add(void *,cchar *) noex ;
	int wridx(int) noex ;
    } ; /* end struct (idxer) */
}


/* forward references */

static int	mkpatfn(char *,int,cc *,int,cc *) noex ;


/* local variables */

static sysval		pagesize(sysval_ps) ;

static bufsizevar	maxpathlen(getbufsize_mp) ;

constexpr int	magicsize = LINEINDEXHDR_MAGICSIZE ;
constexpr int	headsize = magicsize + 4 + (lineindexhdr_overlast * szof(int)) ;

constexpr off_t	maxfoff = INT_MAX ;

constexpr char	tpat[] = "liXXXXXXXXXXXX" ;


/* exported variables */


/* exported subroutines */


/* local subroutines */

namespace lineindex_ns {
    int lineindex_mkidx(LI *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    if (op->tfn) {
		rs = SR_INVALID ;
		if (op->tfn[0]) {
		    idxer	io(op) ;
		    rs = io ;
	        } /* end if (valid) */
	    }
	} /* end if (non-null) */
	return rs ;
    }
} /* end namespace (lineindex_ns) */

idxer::operator int () noex {
    	int		rs ;
	int		rs1 ;
	if ((rs = tmpbegin()) >= 0) {
	    if ((rs = scaner()) >= 0) {
		rs = u_rename(tbuf,op->ifn) ;
		if (rs < 0) {
		    u_unlink(tbuf) ;
		    tbuf[0] = '\0' ;
		}
	    } /* end if (scaner) */
	    rs1 = tmpend() ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (filemap) */
    	return rs ;
}
/* end method (idxer::operator) */

int idxer::tmpbegin() noex {
    	int		rs ;
	if ((rs = maxpathlen) >= 0) {
	    cint	maxpath = rs ;
	    cint	sz = ((maxpath + 1) * 2) ;
	    int		ai = 0 ;
	    if ((rs = uc_malloc(sz,&a)) >= 0) {
		pbuf = (a + ((maxpath + 1) * ai++)) ;
		tbuf = (a + ((maxpath + 1) * ai++)) ;
		plen = maxpath ;
		tlen = maxpath ;
	        cchar	*cp{} ;
	        if (int cl ; (cl = sfdirname(op->ifn,-1,&cp)) >= 0) {
		    if (cl == 0) {
	                cp = "." ;
	                cl = 1 ;
		    }
	            if ((rs = mkpatfn(pbuf,plen,cp,cl,tpat)) >= 0) {
			cint	of = O_WRONLY ;
			cmode	om = 0664 ;
			if ((rs = opentmpfile(pbuf,of,om,tbuf)) >= 0) {
			    tfd = rs ;
			}
		    } /* end if (mkpatfn) */
	        }
	        if (rs < 0) {
	            uc_free(a) ;
		    a = nullptr ;
	        }
	    } /* end if (memory-allocation) */
	} /* end if (bufsizevar) */
	return rs ;
}

int idxer::tmpend() noex {
    	int		rs = SR_OK ;
	int		rs1 ;
	if (tfd >= 0) {
	    rs1 = u_close(tfd) ;
	    if (rs >= 0) rs = rs1 ;
	    tfd = -1 ;
	}
	if (a) {
	    rs1 = uc_free(a) ;
	    if (rs >= 0) rs = rs1 ;
	    a = nullptr ;
	    tbuf = nullptr ;
	    pbuf = nullptr ;
	    tlen = 0 ;
	    plen = 0 ;
	}
	return rs ;
}

int idxer::scaner() noex {
    	int		rs ;
	int		rs1 ;
	if ((rs = recs.start) >= 0) {
	    if ((rs = scanlines(op->tfn)) >= 0) {
		rs = wridx(rs) ;
	    }
	    rs1 = recs.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (vecint) */
    	return rs ;
}
/* end method (idxer::scaner) */

int idxer::scanlines(cchar *fn) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		lines = 0 ;
	if (fn) {
	    rs = SR_INVALID ;
	    if (fn[0]) {
		cint	of = O_RDONLY ;
		if ((rs = uc_open(fn,of,0)) >= 0) {
		    cint	fd = rs ;
		    if (USTAT sb ; (rs = uc_fstat(fd,&sb)) >= 0) {
			csize	fsz = size_t(sb.st_size) ;
			rs = SR_NOTSUP ;
		        if (S_ISREG(sb.st_mode)) {
			    rs = SR_OK ;
			    if (fsz > 0) {
			        rs = liner(fd,fsz) ;
			        lines = rs ;
			    }
			} /* end if (regular file) */
		    } /* end if (stat) */
		    rs1 = uc_close(fd) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (open) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? lines : rs ;
}
/* end method (idxer::scanlines) */

int idxer::liner(int fd,size_t ms) noex {
	cint		mp = PROT_READ ;
	cint		mf = MAP_SHARED ;
	int		rs ;
	int		rs1 ;
	int		lines = 0 ;
	cnullptr	np{} ;
	if (void *md{} ; (rs = u_mmapbegin(np,ms,mp,mf,fd,0z,&md)) >= 0) {
	    cint	cmd = MADV_SEQUENTIAL ;
	    if ((rs = u_madvise(md,ms,cmd)) >= 0) {
		size_t	ll = ms ;
		cchar	*lp = charp(md) ;
		cchar	*tp ;
		while ((tp = charp(memchr(lp,'\n',ll))) != np) {
		    csize	si = ((tp + 1) - lp) ;
		    rs = add(md,lp) ;
		    lines += 1 ;
		    ll -= si ;
		    lp += si ;
		    if (rs < 0) break ;
		} /* end while */
		if (ll > 0) {
		    rs = add(md,lp) ;
		    lines += 1 ;
		}
	    } /* end if (memory-advise) */
	    rs1 = u_mmapend(md,ms) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (map-file) */
	return (rs >= 0) ? lines : rs ;
}
/* end method (idxer::liner) */

int idxer::add(void *md,cchar *lp) noex {
    	int		rs ;
    	cchar		*mp = charp(md) ;
	{
	    uint off = uint(lp - mp) ;
	    rs = recs.add(off) ;
	}
	return rs ;
}
/* end method (idxer::add) */

int idxer::wridx(int lines) noex {
    	lineindexhdr	hdr{} ;
	custime		dt = getustime ;
	cint		sz = (headsize + 20) ;
    	int		rs ;
	hdr.lines = lines ;
	hdr.rectab = headsize ;
	hdr.wrtime = uint(dt & UINT_MAX) ;
	hdr.vetu[0] = LI_VERSION ;
	hdr.vetu[1] = ENDIAN ;
	hdr.vetu[2] = LI_TYPE ;
	if (char *hbuf{} ; (rs = uc_malloc(sz,&hbuf)) >= 0) {
	    if ((rs = hdr.rd(hbuf,rs)) >= 0) {
		cint	hsz = rs ;
		if (int *va ; (rs = recs.getvec(&va)) >= 0) {
		    IOVEC	vec[3] = {} ;
		    vec[0].iov_base = hbuf ;
       		    vec[0].iov_len = hsz ;
        	    vec[1].iov_base = caddr_t(va) ;
        	    vec[1].iov_len = (lines * szof(uint)) ;
		    rs = u_writev(tfd,vec,2) ;
		} /* end if */
	    } /* end if (lineindexhdr_rd) */
	    rs = rsfree(rs,hbuf) ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end method (idxer::wridx) */

static int mkpatfn(char *rb,int rl,cc *dp,int dl,cc *fn) noex {
    	int		rs ;
	int		idx = 0 ;
    	if (storebuf sb(rb,rl) ; (rs = sb.strw(dp,dl)) >= 0) {
	    rs = sb.chr('/') ;
	    if (rs >= 0) rs = sb.str(fn) ;
	    idx = sb.idx ;
	}
	return (rs >= 0) ? idx : rs ;
}
/* end subroutine (mkpatfn) */


