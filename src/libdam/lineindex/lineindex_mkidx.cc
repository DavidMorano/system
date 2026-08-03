/* lineindex_mkidx SUPPORT */
/* charset=ISO8859-1 */
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
#include	<sys/stat.h>		/* POSIX® */
#include	<sys/mman.h>		/* POSIX® Memory Management */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<ctime>			/* CSTD */
#include	<climits>		/* CSTD |INT_MAX| */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<endian.h>		/* LIBU */
#include	<intceil.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucmem.h>		/* LIBUC */
#include	<ucopen.h>		/* LIBUC */
#include	<ucdesc.h>		/* LIBUC */
#include	<sysval.hh>		/* LIBUC */
#include	<bufsizevar.hh>		/* LIBUC */
#include	<opentmp.h>		/* LIBUC |opentmpfile(3uc)| */
#include	<vecint.h>		/* LIBUC */
#include	<storebuf.h>		/* LIBUC */
#include	<mkpathx.h>		/* LIBUC */
#include	<sfx.h>			/* LIBUC */
#include	<intsat.h>		/* LIBUC */
#include	<isnot.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU |MODP2| */

#include	"lineindex.h"
#include	"lineindexhdr.h"


/* local defines */

#define	LI		lineindex
#define	LI_VERSION	LINEINDEX_FILEVERSION
#define	LI_TYPE		LINEINDEX_FILETYPE
#define	LI_FILEMAGSZ	LINEINDEX_FILEMAGICSZ
#define	LI_OV		lineindexhdr_overlast


/* imported namespaces */


/* local typedefs */


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
	int tmpbegin	() noex ;
	int tmpend	() noex ;
	int scaner	() noex ;
	int scanlines	(cchar *) noex ;
	int liner	(int,size_t) noex ;
	int add		(void *,cchar *) noex ;
	int wridx	(int) noex ;
    } ; /* end struct (idxer) */
} /* end namespace */


/* forward references */

local int	mkpatfn(char *,int,cc *,int,cc *) noex ;


/* local variables */

static sysval		pagesize(sysval_ps) ;
static bufsizevar	maxpathlen(bufsize_mp) ;

constexpr int	mmagsz		= LI_FILEMAGSZ ;
constexpr int	headsize	= mmagsz + 4 + (LI_OV * szof(int)) ;
constexpr off_t	maxfoff		= INT_MAX ;
constexpr char	tpat[]		= "liXXXXXXXXXXXX" ;


/* exported variables */


/* exported subroutines */


/* local subroutines */

namespace lineindex_ns {
    int lineindex_mkidx(LI *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    if (op->tfn) ylikely {
		rs = SR_INVALID ;
		if (op->tfn[0]) ylikely {
		    idxer	io(op) ;
		    rs = io ;
	        } /* end if (valid) */
	    }
	} /* end if (non-null) */
	return rs ;
    } /* end subroutine (lineindex_mkidx) */
} /* end namespace (lineindex_ns) */

idxer::operator int () noex {
    	int		rs ;
	int		rs1 ;
	if ((rs = tmpbegin()) >= 0) ylikely {
	    if ((rs = scaner()) >= 0) ylikely {
		rs = u_rename(tbuf,op->ifn) ;
		if (rs < 0) {
		    u_unlink(tbuf) ;
		    tbuf[0] = '\0' ;
		} /* end if (error) */
	    } /* end if (scaner) */
	    rs1 = tmpend() ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (filemap) */
    	return rs ;
} /* end method (idxer::operator) */

int idxer::tmpbegin() noex {
    	int		rs ;
	if ((rs = maxpathlen) >= 0) ylikely {
	    cint	maxpath = rs ;
	    cint	sz = ((maxpath + 1) * 2) ;
	    int		ai = 0 ;
	    if ((rs = lm_mall(sz,&a)) >= 0) ylikely {
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
	        } /* end if */
	        if (rs < 0) {
	            lm_free(a) ;
		    a = nullptr ;
	        } /* end if (error) */
	    } /* end if (memory-allocation) */
	} /* end if (bufsizevar) */
	return rs ;
} /* end method */

int idxer::tmpend() noex {
    	int		rs = SR_OK ;
	int		rs1 ;
	if (tfd >= 0) {
	    rs1 = u_close(tfd) ;
	    if (rs >= 0) rs = rs1 ;
	    tfd = -1 ;
	}
	if (a) {
	    rs1 = lm_free(a) ;
	    if (rs >= 0) rs = rs1 ;
	    a = nullptr ;
	    tbuf = nullptr ;
	    pbuf = nullptr ;
	    tlen = 0 ;
	    plen = 0 ;
	} /* end if (memory-release) */
	return rs ;
} /* end method */

int idxer::scaner() noex {
    	int		rs ;
	int		rs1 ;
	if ((rs = recs.start) >= 0) ylikely {
	    if ((rs = scanlines(op->tfn)) >= 0) ylikely {
		rs = wridx(rs) ;
	    }
	    rs1 = recs.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (vecint) */
    	return rs ;
} /* end method (idxer::scaner) */

int idxer::scanlines(cchar *fn) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		lines = 0 ;
	if (fn) ylikely {
	    rs = SR_INVALID ;
	    if (fn[0]) ylikely {
		cint	of = O_RDONLY ;
		if ((rs = uc_open(fn,of,0)) >= 0) ylikely {
		    cint	fd = rs ;
		    if (ustat sb ; (rs = uc_fstat(fd,&sb)) >= 0) ylikely {
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
} /* end method (idxer::scanlines) */

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
		if ((rs >= 0) && (ll > 0)) {
		    rs = add(md,lp) ;
		    lines += 1 ;
		}
	    } /* end if (memory-advise) */
	    rs1 = u_mmapend(md,ms) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (map-file) */
	return (rs >= 0) ? lines : rs ;
} /* end method (idxer::liner) */

int idxer::add(void *md,cchar *lp) noex {
    	int		rs ;
    	cchar		*mp = charp(md) ;
	{
	    uint off = uint(lp - mp) ;
	    rs = recs.add(off) ;
	}
	return rs ;
} /* end method (idxer::add) */

int idxer::wridx(int lines) noex {
    	lineindexhdr	hdr{} ;
	custime		dt = getustime ;
	cint		sz = (headsize + 20) ;
    	int		rs ;
	int		rs1 ;
	{
	    hdr.lines = lines ;
	    hdr.rectab = headsize ;
	    hdr.wrtime = uint(dt & UINT_MAX) ;
	    hdr.vetu[0] = LI_VERSION ;
	    hdr.vetu[1] = charconv(ENDIAN) ;
	    hdr.vetu[2] = LI_TYPE ;
	}
	if (char *hbuf ; (rs = lm_mall(sz,&hbuf)) >= 0) ylikely {
	    if ((rs = hdr.rd(hbuf,rs)) >= 0) ylikely {
		cint	hsz = rs ;
		if (int *va ; (rs = recs.getvec(&va)) >= 0) ylikely {
		    IOVEC	vec[3] = {} ;
		    vec[0].iov_base = hbuf ;
       		    vec[0].iov_len = hsz ;
        	    vec[1].iov_base = caddr_t(va) ;
        	    vec[1].iov_len = (lines * szof(int)) ;
		    rs = u_writev(tfd,vec,2) ;
		} /* end if */
	    } /* end if (lineindexhdr_rd) */
	    rs1 = lm_free(hbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
} /* end method (idxer::wridx) */

local int mkpatfn(char *rb,int rl,cc *dp,int dl,cc *fn) noex {
    	int		rs ;
	int		idx = 0 ;
    	if (storebuf sb(rb,rl) ; (rs = sb.strw(dp,dl)) >= 0) ylikely {
	    rs = sb.chr('/') ;
	    if (rs >= 0) rs = sb.str(fn) ;
	    idx = sb.idx ;
	} /* end if (storebuf) */
	return (rs >= 0) ? idx : rs ;
} /* end subroutine (mkpatfn) */


