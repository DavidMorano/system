/* debugprint SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* special debug printing */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */
#define	CF_LINELEN	0		/* use |strlinelen(3dam)| */

/* revision history:

	= 1983-03-07, David A­D­ Morano
	This subroutine was written for PPI development.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	debugprintf

	Description:
	This subroutine does a printf-like function but for the
	special error output facility.  It calls |format(3dam)| as
	might be expected but besides that it tries to be as simple
	as possible, so that it depends on as little other stuff
	as possible.

	Notes about recent UNIX® deficiencies:

	On most more recent UNIX® systems, writes (even |write(2)|
	writes) are not atomic when the file descriptor is in APPEND
	mode.  This stupid behavior that came about with more recent
	versions of UNIX®, like F*ckSolaris® for example.  This tupid
	behavior was first notices (long ago) when writes in APPEND
	mode were made to files located on Network File System (NFS)
	mounted file-systems.  But since those early days, this
	behavior has somehow spread to even mess up when writing
	to files on local file-systems.

	Notes:

	Q. Does this subroutine need to be multi-thread-safe?

	A. Of course!

	Q. Why do we need any mutex-locks in here at all? Is nt
	this code completely multi-thread-safe already?!

	A. We need a mutex-lock around the |write(2)| call because
	that call is not atomic on most UNIX®!  Specifically, the
	implementation does not update the file-pointer atomically
	along with the associated write of data to the file.  Yes,
	Virginia, many UNIX® are actually quite messed up when
	it comes to multi-thread-safety! We fix the flaws in most
	all OSes (all OSes at this time are indeed flawed; Solaris®
	is aware of the problem and may or may not fix their
	implementation) by using our own mutex-lock around the
	|write(2)| call.

        Q. Why are all UNIX® that exist in the world right now not
        multi-thread-safe with their own OS system calls?

	A. Your guess is as good as mine, but the short answer is
	that I did not write the code for the various UNIX® in the
	world -- even though I should have! It really does seem
	that if I did not write a certain piece of code, that code
	is likely buggy -- regardless of how many people have already
	suffered due to those bugs.

	Normally we want to use the heap for "large" buffers.  We
	need a buffer of about 2k bytes.  This used to be a "small"
	buffer in the old days when programs were mostly single-threaded.
	But now-a-days since almost all programs and subroutines
	run in a multi-thread environment, the amount of stack space
	availble for "small" buffers is not that large any longer;
	hence the need for dynamic allocation to the heap (not to
	the stack) is now desired.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* POSIX */
#include	<sys/stat.h>		/* POSIX */
#include	<unistd.h>		/* POSIX */
#include	<fcntl.h>		/* POSIX */
#include	<csignal>		/* CSTD */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstdarg>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<new>			/* C++STD |nothrow(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<usupport.h>		/* LIBU |strwcpy(3u)| */
#include	<umem.hh>		/* LIBU */
#include	<usysutility.hh>	/* LIBU */
#include	<ptm.h>			/* LIBU */
#include	<strnul.hh>		/* LIBU */
#include	<ucopen.h>		/* LIBUC */
#include	<ucdesc.h>		/* LIBUC */
#include	<ucproc.h>		/* LIBUC */
#include	<cfdec.h>		/* LIBUC */
#include	<fmtstr.h>		/* LIBUC */
#include	<hasx.h>		/* LIBUC |hasprintbad(3uc)| */
#include	<strlinelen.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<dprint.hh>		/* LIBU |DPRINTF(3u)| */

#include	"debugprint.h"
#include	"debugline.h"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/ureserve.ccm"

import libutil ;			/* |lenstr(3u)| */
import ureserve ;			/* |is{x}(3u)| */

/* local defines */

#ifndef	FD_STDERR
#define	FD_STDERR	3
#endif

#define	FD_BADERR	4
#define	FD_MAX		256		/* maximum FDs we will consider */

#define	DEBUGMGR	debugmgr_head

#define	O_FLAGS		(O_WRONLY | O_CREAT | O_APPEND)

#ifndef	LINEBUFLEN
#define	LINEBUFLEN	(2 * 1024)
#endif
 
#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif
#ifndef	CF_LINELEN
#define	CF_LINELEN	0		/* use |strlinelen(3dam)| */
#endif


/* imported namespaces */

using libu::sncpy ;			/* subroutine-template */
using libu::snprintf ;			/* subroutine */
using libu::snvprintf ;			/* subroutine */
using libu::strwcpy ;			/* subroutine */
using libu::umem ;			/* variable */


/* local typedefs */

typedef volatile sig_atomic_t	vaflag ;


/* external subroutines */

extern "C" {
    int debugmgr_init() noex ;
    int debugmgr_fini() noex ;
    int	debugclose() noex ;
} /* end */


/* external variables */


/* local structures */

namespace {
    struct debugmgr_fl {
	uint		seekable:1 ;
    } ; /* end struct (debugmgr_fl) */
    struct debugmgr_head {
	ptm		mx ;		/* data mutex */
	vaflag		f_void ;
	vaflag		f_init ;
	vaflag		f_initdone ;
	debugmgr_fl	fl ;
	int		fd ;
	int		sz ;
	int advend() noex ;
    } ; /* end struct (debugmgr_head) */
} /* end namespace */


/* forward references */

extern "C" {
    local void	debugmgr_atforkbefore() noex ;
    local void	debugmgr_atforkafter() noex ;
} /* end */

local int	debugprinter(cchar *,int) noex ;
local int	snwcpyprintclean(char *,int,cchar *,int) noex ;

#ifdef	COMMENT
local int	cthexi(char *,int) noex ;
local char	*convdeci(long,char *) noex ;
#endif /* COMMENT */

#ifdef	COMMENT
local bool	hasprintbad(cchar *,int) noex ;
#endif /* COMMENT */


/* local variables */

static DEBUGMGR	ef ; /* zero-initialized */

constexpr char	cthextable[] = {
	'0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 0
} ; /* end array */

constexpr fmtoptms	fmtmask ;

constexpr int		llen		= LINEBUFLEN ;
constexpr bool		f_debug		= CF_DEBUG ;
constexpr bool		f_linelen	= CF_LINELEN ;


/* exported variables */


/* exported subroutines */

int debugmgr_init() noex {
	DEBUGMGR	*uip = &ef ;
	int		rs = 1 ;
	if (! uip->f_init) {
	    ptm *mxp = &uip->mx ;
	    uip->f_init = true ;
	    if ((rs = mxp->create) >= 0) {
	        void_f b = debugmgr_atforkbefore ;
	        void_f a = debugmgr_atforkafter ;
	        if ((rs = uc_atforkrec(b,a,a)) >= 0) {
		    const void_f funexit = void_f(debugmgr_fini) ;
	            if ((rs = uc_atexit(funexit)) >= 0) {
	                rs = 0 ;
	                uip->f_initdone = true ;
	            }
	            if (rs < 0) {
	                uc_atforkexp(b,a,a) ;
		    }
	        } /* end if (uc_atfork) */
	        if (rs < 0) {
	            mxp->destroy() ;
		} /* end if (error) */
	    } /* end if (ptm_create) */
	    if (rs < 0) {
	        uip->f_init = false ;
	    } /* end if (error) */
	} else {
	    while ((rs >= 0) && uip->f_init && (! uip->f_initdone)) {
	        rs = msleep(1) ;
	    }
	    if ((rs >= 0) && (! uip->f_init)) {
		rs = SR_LOCKFAIL ;
	    }
	} /* end if */
	return rs ;
} /* end subroutine (debugmgr_init) */

int debugmgr_fini() noex {
	DEBUGMGR	*uip = &ef ;
	int		rs = SR_OK ;
	int		rs1 ;
	if (uip->f_initdone) {
	    uip->f_initdone = false ;
	    if (uip->fd > 0) {
	        rs1 = u_close(uip->fd) ;
		if (rs >= 0) rs = rs1 ;
	        uip->fd = 0 ; /* special case (use zero) */
	    }
	    {
	        void_f	b = debugmgr_atforkbefore ;
	        void_f	a = debugmgr_atforkafter ;
	        rs1 = uc_atforkexp(b,a,a) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    {
	    	ptm *mxp = &uip->mx ;
	        rs1 = mxp->destroy ;
		if (rs >= 0) rs = rs1 ;
	    }
	    memclear(uip) ;
	} /* end if (was initialized) */
	return rs ;
} /* end subroutine (debugmgr_fini) */

int debugprintf(cchar *fmt,...) noex {
	va_list		ap ;
	int		rs = SR_FAULT ;
	int		wlen = 0 ; /* return-value */
	DPRINTF("ent fd=%d fmt=>%s<\n",ef.fd,fmt) ;
	if (fmt) {
	    rs = SR_NOTOPEN ;
	    if (ef.fd > 0) {
	        va_begin(ap,fmt) ;
		{
		    rs = debugvprintf(fmt,ap) ;
		    wlen = rs ;
		}
	        va_end(ap) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	DPRINTF("ret rs=%d wlen=%d\n",rs,wlen) ;
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (debugprintf) */

/* special for |DEBUGPRINTF(3debug)| */
int debugprintx(cchar *fun,cchar *fmt,...) noex {
	va_list		ap ;
    	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	int		wlen = 0 ; /* return-value */
	if (fmt) {
	    rs = SR_NOTOPEN ;
	    if (ef.fd > 0) {
		cint flen = (lenstr(__func__) + 2 + lenstr(fmt)) ;
		rs = SR_NOMEM ;
		if (char *fbuf = new(nt) char [flen + 1] ; fbuf) {
		    if ((rs = sncpy(fbuf,flen,fun,": ",fmt)) >= 0) {
	                va_begin(ap,fmt) ;
	                rs = debugvprintf(fbuf,ap) ;
	                wlen += rs ;
	                va_end(ap) ;
	            } /* end if (sncpy) */
		    delete [] fbuf ;
	        } /* end if (new-char) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (debugvprintx) */

int debugvprintf(cchar *fmt,va_list ap) noex {
    	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	int		wlen = 0 ; /* return-value */
	if (fmt) {
	    DPRINTF("fmt=>%s<\n",fmt) ;
	    rs = SR_NOTOPEN ;
	    if (ef.fd > 0) {
		rs = SR_NOMEM ;
		if (char *lbuf = new(nt) char [llen + 1] ; lbuf) {
	            if ((rs = snvprintf(lbuf,llen,fmt,ap)) >= 0) {
			DPRINTF("fmtstr() rs=%d\n",rs) ;
	        	rs = debugprinter(lbuf,rs) ;
	                wlen += rs ;
	            } else if (rs == SR_OVERFLOW) {
	                rs = SR_TOOBIG ;
			lbuf[0] = '\0' ;
	            } /* end if */
		    delete [] lbuf ;
	        } /* end if (new-char) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (debugvprintf) */

int debugprintdeci(cchar *s,int v) noex {
    	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (s) {
	    rs = SR_NOMEM ;
	    if (char *lbuf = new(nt) char [llen + 1] ; lbuf) {
		if ((rs = snprintf(lbuf,llen,"%s %d\n",s,v)) >= 0) {
	            rs = debugwrite(lbuf,rs) ;
	        } /* end if (snprintf) */
		delete [] lbuf ;
	    } /* end if (new-char) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (debugprintdeci) */

int debugprinthexi(cchar *s,int v) noex {
    	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (s) {
	    rs = SR_NOMEM ;
	    if (char *lbuf = new(nt) char [llen + 1] ; lbuf) {
		if ((rs = snprintf(lbuf,llen,"%s %08x\n",s,v)) >= 0) {
	            rs = debugwrite(lbuf,rs) ;
	        } /* end if (snprintf) */
		delete [] lbuf ;
	    } /* end if (new-char) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (debugprinthexi) */

int debugprintnum(cchar *s,int v) noex {
	return debugprintdeci(s,v) ;
} /* end subroutine (debugprintnum) */

int debugsetfd(int fd) noex {
	int		rs = SR_NOTOPEN ;
	ef.fd = 0 ; /* special case (use zero) */
	if (fd >= 0) {
	    if (ustat sb ; (fd < FD_MAX) && ((rs = uc_fstat(fd,&sb)) >= 0)) {
	        ef.fd = fd ;
	        ef.sz = intconv(sb.st_size) ;
	    } /* end if (uc_fstat) */
	} /* end if (was open) */
	if ((rs >= 0) && (ef.fd >= 0)) {
	    uc_fchmod(ef.fd ,0666) ;
	} /* end if */
	return rs ;
} /* end subroutine (debugsetfd) */

int debugopen(cchar *fname) noex {
	int		rs = SR_FAULT ;
	int		fd = -1 ;
	if (fname) ylikely {
	    rs = SR_INVALID ;
	    if (fname[0]) ylikely {
	        debugclose() ;
	        if (hasalldig(fname,-1)) {
	            if (uint v ; (rs = cfdecui(fname,-1,&v)) >= 0) {
	                fd = v ;
	            }
	        } else {
	            if ((rs = u_open(fname,O_FLAGS,0666)) >= 0) {
	                fd = rs ;
	                if ((rs = uc_moveup(fd,3)) >= 0) {
	                    fd = rs ;
	                } else {
	                    u_close(fd) ;
	                }
	            } /* end if (u_open) */
	        } /* end if */
	        if (rs >= 0) ylikely {
	            ef.fd = fd ;
	            ef.sz = 0 ;
	            if (ustat sb ; (rs = uc_fstat(ef.fd,&sb)) >= 0) {
	                ef.sz = intconv(sb.st_size) ;
	                uc_fchmod(ef.fd,0666) ;
			ef.fl.seekable = true ;
		    } else if (rs == SR_NOTSEEK) {
			rs = SR_OK ;
	            }
	        } /* end if (ok) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? ef.fd : rs ;
} /* end subroutine (debugopen) */

int debugclose() noex {
    	int		rs = SR_OK ;
	int		rs1 ;
	if (ef.fd > 0) {
	    rs1 = u_close(ef.fd) ;
	    if (rs >= 0) rs = rs1 ;
	    ef.fd = 0 ; /* special case (use zero) */
	} /* end if (was open) */
	ef.sz = 0 ;
	return rs ;
} /* end subroutine (debugclose) */

int debuggetfd() noex {
	return ef.fd ;
} /* end subroutine (debuggetfd) */

/* low level debug-print function */
int debugprint(cchar *sbuf,int slen) noex {
	int		rs = SR_FAULT ;
	if (sbuf) {
	    rs = SR_NOTOPEN ; 
	    if (ef.fd >= 0) {
	        rs = debugprinter(sbuf,slen) ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (debugprint) */

local int getlen(cchar *sp,int sl) noex {
    	int	al = 0 ;
    	if_constexpr (f_linelen) {
	    al = strlinelen(sp,sl,llen) ; /* some protection */
	} else {
	    if (al = lenstr(sp,sl) ; al > llen) {
		al = llen ;
	    }
	} /* end if_constexpr (f_linelen) */
    	return al ;
} /* end subroutine (getlen) */

int debugwrite(cchar *sbuf,int µslen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		wlen = 0 ;
	DPRINTF("ent slen=%d\n",µslen) ;
	if (int slen ; sbuf && ((slen = getlenstr(sbuf,µslen)) >= 0)) {
	    DPRINTF("getlen() slen=%d\n",slen) ;
	    rs = SR_NOTOPEN ;
	    if (ef.fd > 0) {
	        if (((rs = debugmgr_init()) >= 0) && (slen > 0)) {
	            DEBUGMGR	*uip = &ef ;
	            ptm *mxp = &uip->mx ;
	            if ((rs = mxp->lockbegin) >= 0) { /* single */
		        int	cmd = F_LOCK ;
		        if ((rs = u_lockf(ef.fd,cmd,0z)) >= 0) {
			    if ((rs = ef.advend()) >= 0) {
				if ((rs = u_write(ef.fd,sbuf,slen)) >= 0) {
				    if_constexpr (f_debug) {
				        cc *str = "u_write() rs=%d\n" ;
				        DPRINTF(str,rs) ;
				    }
				    wlen = rs ;
				    ef.sz += wlen ;
				} /* end if (u_write) */
		            } /* end if (advend) */
		            cmd = F_UNLOCK ;
		            rs1 = u_lockf(ef.fd,cmd,0z) ;
		            if (rs >= 0) rs = rs1 ;
		        } /* end if (uc_lockf) */
	                rs1 = mxp->lockend ;
		        if (rs >= 0) rs = rs1 ;
	            } /* end if (ptm) */
	        } /* end if (debugmgr_init) */
	    } /* end if (valid) */
	} /* end if (getlenstr) */
	DPRINTF("ret rs=%d wlen=%d\n",rs,wlen) ;
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (debugwrite) */


/* local subroutines */

local int debugprinter(cchar *sbuf,int µslen) noex {
    	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	int		wlen = 0 ; /* return-value */
	DPRINTF("ent slen=%d\n",µslen) ;
	if (int slen ; sbuf && ((slen = getlen(sbuf,µslen)) >= 0)) {
	    cint clen = slen ;
	    rs = SR_NOMEM ;
	    DPRINTF("getlen() slen=%d\n",slen) ;
	    if (char *cbuf = new(nt) char[clen + 1] ; cbuf) {
		    int		kl = slen ; /* check-length */
	            bool	f_needeol = false ;
	            bool	f_cleaned = false ;
		    rs = SR_OK ;
		    if (slen > 0) {
	                if (sbuf[slen - 1] != '\n') {
	                    f_needeol = true ;
			} else {
			    kl -= 1 ;
			}
		    } else {
	                f_needeol = true ;
		    } /* end if */
	            /* scan for bad characters */
	            if ((kl > 0) && hasprintbad(sbuf,kl)) {
			cauto sncl = snwcpyprintclean ;
			DPRINTF("-> sncl kl=%d\n",kl) ;
	                if ((rs = sncl(cbuf,(clen - 1),sbuf,kl)) >= 0) {
			    f_cleaned = true ;
			    DPRINTF("sncl() rs=%d\n",rs) ;
		            sbuf = cbuf ;
		            slen = rs ;
	                } /* end if */
		    } /* end if (switched buffer) */
		    if ((rs >= 0) && f_needeol) {
			if (! f_cleaned) {
			    strwcpy(cbuf,sbuf,slen) ;
			}
			cbuf[slen++] = '\n' ;
		        cbuf[slen] = '\0' ;
			sbuf = cbuf ;
		    } /* end if (switched buffer) */
	            /* write the line-buffer out */
	            if (rs >= 0) {
			DPRINTF("-> dwr slen=%d\n",slen) ;
			if_constexpr (f_debug) {
			    strnul ps(sbuf,slen) ;
			    DPRINTF("s=>%s<",ccp(ps)) ;
			}
	                rs = debugwrite(sbuf,slen) ;
	                wlen = rs ;
	            } /* end if (ok) */
		delete [] cbuf ;
	    } /* end if (m-a-f) */
	} /* end if (getlenstr) */
	DPRINTF("ret rs=%d wlen=%d\n",rs,wlen) ;
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (debugprinter) */

local void debugmgr_atforkbefore() noex {
	DEBUGMGR	*uip = &ef ;
	{
	    ptm *mxp = &uip->mx ;
	    mxp->lockbegin() ;
	}
} /* end subroutine (debugmgr_atforkbefore) */

local void debugmgr_atforkafter() noex {
	DEBUGMGR	*uip = &ef ;
	{
	    ptm *mxp = &uip->mx ;
	    mxp->lockend() ;
	}
} /* end subroutine (debugmgr_atforkafter) */

int debugmgr_head::advend() noex {
    	int		rs = SR_OK ;
	if (fl.seekable) {
	    if (ustat sb ; (rs = uc_fstat(ef.fd,&sb)) >= 0) {
		cint fsz = intconv(sb.st_size) ;
		if (S_ISREG(sb.st_mode) && (fsz != ef.sz)) {
		    ef.sz = fsz ;
		    rs = uc_seek(ef.fd,0z,SEEK_END) ;
		} /* end if (regular file) */
	    } /* end if (uc_fstat) */
	} /* end if (seekable) */
	return rs ;
} /* end if (debugmgr_head::advend) */

#ifdef	COMMENT
local char *convdeci(long num,char *endptr) noex {
	ulong		unum = ulong(num) ;
	char		*bp ;
	if (num < 0) unum = (- unum) ;
	bp = ultostr(unum,endptr) ;
	if (num < 0) *--bp = '-' ;
	return bp ;
} /* end subroutine (convdeci) */
#endif /* COMMENT */

#ifdef	COMMENT
local int cthexi(char *buf,int val) noex {
	cint		n = (2 * szof(int)) ;
	for (int i = (n - 1) ; i >= 0 ; i -= 1) {
	    buf[i] = cthextable[val & 0x0F] ;
	    val >>= 4 ;
	} /* end for */
	buf[n] = '\0' ;
	return n ;
} /* end subroutine (cthexi) */
#endif /* COMMENT */

local int snwcpyprintclean(char *dbuf,int dlen,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		dl = 0 ; /* return-value */
	for (int ch ; dlen-- && sl && ((ch = MKCHAR(*sp))) ; sp += 1) {
	    if (isprintbad(ch)) {
	        if (ch == '\n') {
		    ch = '¬' ;
	        } else {
		    ch = '¿' ;
		}
	    } /* end if (was print-bad) */
	    dbuf[dl++] = char(ch) ;
	    sl -= 1 ;
	} /* end for */
	if ((sl != 0) && *sp) {
	    rs = SR_OVERFLOW ;
	} /* end if */
	dbuf[dl] = '\0' ;
	return (rs >= 0) ? dl : rs ;
} /* end subroutine (snwcpyprintclean) */

#ifdef	COMMENT
local bool hasprintbad(cchar *sp,int sl) noex {
	bool		f = false ;
	while (sl && *sp) {
	    f = isprintbad(sp[0] & UCHAR_MAX) ;
	    if (f) break ;
	    sp += 1 ;
	    sl -= 1 ;
	} /* end while */
	return f ;
} /* end subroutine (hasprintbad) */
#endif /* COMMENT */


