/* debugprint SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* special debug printing */
/* version %I% last-modified %G% */

#define	CF_LINELEN	0		/* use |strlinelen(3dam)| */
#define	CF_USEMALLOC	0		/* do not use memory-allocation */

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

	= On the use of "uc_malloc(3uc)| and the CF_USEMALLOC
	compile-time flag

	Normally we want to use the heap for "large" buffers.  We
	need a buffer of about 2k bytes.  This used to be a "small"
	buffer in the old days when programs were mostly single-threaded.
	But now-a-days since almost all programs and subroutines
	run in a multi-thread environment, the amount of stack space
	availble for "small" buffers is not that large any longer;
	hence the need for dynamic allocation to the heap (not to
	the stack) is now desired.  Using |uc_malloc(3uc)| is the
	default, but if for some reason you need to be independent
	of that subsystem, a non-dynamic buffer version is available
	by setting the CF_USEMALLOC compile-time flag to zero (0).

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<csignal>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstdarg>
#include	<cstdio>
#include	<cstring>
#include	<new>			/* |nothrow(3c++)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<usupport.h>		/* |strwcpy(3u)| */
#include	<umem.hh>
#include	<ptm.h>
#include	<cfdec.h>
#include	<fmtstr.h>
#include	<hasx.h>
#include	<strlinelen.h>
#include	<localmisc.h>

#include	"debugprint.h"
#include	"debugline.h"

#pragma		GCC dependency		"mod/libutil.ccm"
#pragma		GCC dependency		"mod/ureserve.ccm"

import libutil ;			/* |lenstr(3u)| */
import ureserve ;			/* |is{x}(3u)| */

/* local defines */

#ifndef	NDF
#define	NDF		"debugprintf.deb"
#endif

#ifndef	FD_STDERR
#define	FD_STDERR	3
#endif

#define	FD_BADERR	4
#define	FD_MAX		256		/* maximum FDs we'll consider */

#define	DEBUGPRINT	struct debugprint_head

#define	O_FLAGS		(O_WRONLY | O_CREAT | O_APPEND)


/* imported namespaces */

using libu::strwcpy ;			/* subroutine */
using libu::umem ;			/* variable */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef volatile sig_atomic_t	vaflag ;


/* external subroutines */

extern "C" {
    int debugprint_init() noex ;
    int debugprint_fini() noex ;
    int	debugprint(cchar *,int) noex ;
    int	debugclose() noex ;
}

extern "C" {
    extern int uc_atforkrec(void_f,void_f,void_f) noex ;
    extern int uc_atforkexp(void_f,void_f,void_f) noex ;
    extern int uc_atexit(void_f) noex ;
    extern int uc_moveup(int,int) noex ;
}


/* external variables */


/* local structures */

struct debugprint_head {
	ptm		mx ;		/* data mutex */
	vaflag		f_void ;
	vaflag		f_init ;
	vaflag		f_initdone ;
	int		fd ;
	int		sz ;
} ; /* end struct */


/* forward references */

local int	debugprinters(cchar *,int) noex ;

extern "C" {
    local void	debugprint_atforkbefore() noex ;
    local void	debugprint_atforkafter() noex ;
}

local int	debugprinter(cchar *,int) noex ;
local int	snwcpyprintclean(char *,int,cchar *,int) noex ;
local int	cthexi(char *,int) noex ;

local char	*convdeci(LONG,char *) noex ;

#ifdef	COMMENT
local bool	hasprintbad(cchar *,int) noex ;
#endif /* COMMENT */


/* local variables */

static DEBUGPRINT	ef ; /* zero-initialized */

constexpr char	cthextable[] = {
	'0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 0
} ; /* end array */

constexpr fmtoptms	fmtmask ;

constexpr int		llen = LINEBUFLEN ;


/* exported variables */


/* exported subroutines */

int debugprint_init() noex {
	DEBUGPRINT	*uip = &ef ;
	int		rs = 1 ;
	if (! uip->f_init) {
	    ptm *mxp = &uip->mx ;
	    uip->f_init = true ;
	    if ((rs = mxp->create) >= 0) {
	        void_f b = debugprint_atforkbefore ;
	        void_f a = debugprint_atforkafter ;
	        if ((rs = uc_atforkrec(b,a,a)) >= 0) {
		    const void_f funexit = void_f(debugprint_fini) ;
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
		}
	    } /* end if (ptm_create) */
	    if (rs < 0) {
	        uip->f_init = false ;
	    }
	} else {
	    while ((rs >= 0) && uip->f_init && (! uip->f_initdone)) {
	        rs = msleep(1) ;
	    }
	    if ((rs >= 0) && (! uip->f_init)) {
		rs = SR_LOCKLOST ;
	    }
	} /* end if */
	return rs ;
}
/* end subroutine (debugprint_init) */

int debugprint_fini() noex {
	DEBUGPRINT	*uip = &ef ;
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
	        void_f	b = debugprint_atforkbefore ;
	        void_f	a = debugprint_atforkafter ;
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
}
/* end subroutine (debugprint_fini) */

int debugprintf(cchar *fmt,...) noex {
	va_list		ap ;
	cnullptr	nt{} ;
	int		rs = SR_FAULT ;
	int		wlen = 0 ; /* return-value */
	if (fmt) {
	    rs = SR_NOTOPEN ;
	    if (ef.fd >= 0) {
	        cint	fm = fmtmask.nooverr ; /* Format-Mode-Mask */
		rs = SR_NOMEM ;
		if (char *lbuf = new(nt) char [llen + 1] ; lbuf) {
	            va_begin(ap,fmt) ;
	            if ((rs = fmtstr(lbuf,llen,fm,fmt,ap)) >= 0) {
	                rs = debugprint(lbuf,rs) ;
	                wlen += rs ;
	            } else {
	                rs = SR_TOOBIG ;
		    }
	            va_end(ap) ;
		    delete [] lbuf ;
	        } /* end if (new-char) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (debugprintf) */

int debugvprintf(cchar *fmt,va_list ap) noex {
    	cnothrow	nt{} ;
	cint		fm = (fmtmask.clean | fmtmask.nooverr) ;
	int		rs = SR_FAULT ;
	int		wlen = 0 ; /* return-value */
	if (fmt) {
	    rs = SR_NOTOPEN ;
	    if (ef.fd >= 0) {
		rs = SR_NOMEM ;
		if (char *lbuf = new(nt) char [llen + 1] ; lbuf) {
	            if ((rs = fmtstr(lbuf,llen,fm,fmt,ap)) >= 0) {
	                rs = debugprint(lbuf,rs) ;
	                wlen += rs ;
	            } else {
	                rs = SR_TOOBIG ;
	            }
		    delete [] lbuf ;
	        } /* end if (new-char) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (debugvprintf) */

int debugprintdeci(cchar *s,int v) noex {
    	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (s) {
	    rs = SR_NOMEM ;
	    if (char *lbuf = new(nt) char [llen + 1] ; lbuf) {
	    cint	diglen = DIGBUFLEN ;
	    int		ll = llen ;
	    int		sl ;
	    cchar	*sp ;
	    char	*lp = lbuf ;
	    char	digbuf[DIGBUFLEN + 1] ;

	if (rs >= 0) {
	    sp = s ;
	    sl = lenstr(s) ;
	    if (sl <= ll) {
	        lp = strwcpy(lp,sp,sl) ;
	        ll -= sl ;
	    } else {
	        rs = SR_OVERFLOW ;
	    }
	} /* end if (ok) */

	if (rs >= 0) {
	    sp = convdeci(v,(digbuf+diglen)) ;
	    sl = intconv((digbuf + diglen) - sp) ;
	    if (sl <= ll) {
	        lp = strwcpy(lp,sp,sl) ;
	        ll -= sl ;
	    } else {
	        rs = SR_OVERFLOW ;
	    }
	} /* end if (ok) */

	if (rs >= 0) {
	    sp = "\n" ;
	    sl = 1 ;
	    if (sl <= ll) {
	        lp = strwcpy(lp,sp,sl) ;
	        ll -= sl ;
	    } else {
	        rs = SR_OVERFLOW ;
	    }
	} /* end if (ok) */

	if (rs >= 0) {
	    cint tl = intconv(lp - lbuf) ;
	    rs = debugprint(lbuf,tl) ;
	} /* end if (ok) */

		delete [] lbuf ;
	    } /* end if (new-char) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (debugprintdeci) */

int debugprinthexi(cchar *s,int v) noex {
    	cnothrow	nt{} ;
	int		rs = SR_FAULT ;
	if (s) {
	    rs = SR_NOMEM ;
	    if (char *lbuf = new(nt) char [llen + 1] ; lbuf) {
	        int	ll = llen ;
	        int	sl ;
	        char	*lp = lbuf ;
	        cchar	*sp ;
	        char	digbuf[DIGBUFLEN + 1] ;
    
	    if (rs >= 0) {
	        sp = s ;
	        sl = lenstr(s) ;
	        if (sl <= ll) {
	            lp = strwcpy(lp,sp,sl) ;
	            ll -= sl ;
	        } else {
	            rs = SR_OVERFLOW ;
	        }
	    } /* end if (ok) */
    
	    if (rs >= 0) {
	        sp = digbuf ;
	        rs = cthexi(digbuf,v) ;
	        sl = rs ;
	        if (sl <= ll) {
	            lp = strwcpy(lp,sp,sl) ;
	            ll -= sl ;
	        } else {
	            rs = SR_OVERFLOW ;
	        }
	    } /* end if (ok) */
    
	    if (rs >= 0) {
	        sp = "\n" ;
	        sl = 1 ;
	        if (sl <= ll) {
	            lp = strwcpy(lp,sp,sl) ;
	            ll -= sl ;
	        } else {
	            rs = SR_OVERFLOW ;
	        }
	    } /* end if (ok) */
    
	    if (rs >= 0) {
	        cint tl = intconv(lp - lbuf) ;
	        rs = debugprint(lbuf,tl) ;
	    } /* end if (ok) */
    
		delete [] lbuf ;
	    } /* end if (new-char) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (debugprinthexi) */

int debugprintnum(cchar *s,int v) noex {
	return debugprintdeci(s,v) ;
}
/* end subroutine (debugprintnum) */

int debugsetfd(int fd) noex {
	int		rs = SR_NOTOPEN ;

	ef.fd = 0 ; /* special case (use zero) */
	if (fd >= 0) {
	    if (ustat sb ; (fd < FD_MAX) && ((rs = u_fstat(fd,&sb)) >= 0)) {
	        ef.fd = fd ;
	        ef.sz = intconv(sb.st_size) ;
	    }
	}

	if ((rs >= 0) && (ef.fd >= 0)) {
	    u_fchmod(ef.fd ,0666) ;
	}

	return rs ;
}
/* end subroutine (debugsetfd) */

int debugopen(cchar *fname) noex {
	int		rs = SR_FAULT ;
	int		fd = -1 ;
	if (fname) {
	    rs = SR_INVALID ;
	    if (fname[0]) {
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
	            }
	        } /* end if */
	        if (rs >= 0) {
	            ef.fd = fd ;
	            ef.sz = 0 ;
	            if (ustat sb ; (rs = u_fstat(ef.fd,&sb)) >= 0) {
	                ef.sz = intconv(sb.st_size) ;
	                u_fchmod(ef.fd,0666) ;
	            }
	        } /* end if (ok) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? ef.fd : rs ;
}
/* end subroutine (debugopen) */

int debugclose() noex {
	if (ef.fd > 0) {
	    u_close(ef.fd) ;
	    ef.fd = 0 ; /* special case (use zero) */
	}
	ef.sz = 0 ;
	return 0 ;
}
/* end subroutine (debugclose) */

int debuggetfd() noex {
	return ef.fd ;
}
/* end subroutine (debuggetfd) */

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
}
/* end subroutine (debugprint) */

#if	CF_USEMALLOC

int debugprinter(cchar *sbuf,int slen) noex {
	int		rs = SR_OK ;
	int		wlen = 0 ;
	int		f_needeol = false ;
	char		*abuf = nullptr ;

#if	CF_LINELEN
	slen = strlinelen(sbuf,slen,LINEBUFLEN) ; /* some protection */
#else
	if (slen < 0) slen = lenstr(sbuf) ;
	if (slen > LINEBUFLEN) slen = LINEBUFLEN ;
#endif /* CF_LINELEN */

/* preparation and check if need EOL */

	if ((slen == 0) || (sbuf[slen-1] != '\n')) {
	    f_needeol = true ;
	} else {
	    slen -= 1 ;
	}

/* scan for bad characters */

	if (f_needeol || hasprintbad(sbuf,slen)) {
	    cint	alen = (slen+2) ; /* additional room for added EOL */
	    if ((rs = umem.mall((alen+1),&abuf)) >= 0) {
	        if ((rs = snwcpyprintclean(abuf,(alen-2),sbuf,slen)) >= 0) {
		    sbuf = abuf ;
		    slen = rs ;
		    abuf[slen++] = '\n' ;
	        }
		if (rs < 0) {
		    umem.free(abuf) ;
		    abuf = nullptr ;
		}
	    } /* end if (memory-allocation) */
	} else {
	    slen += 1 ;
	}

/* write the line-buffer out */

	if (rs >= 0) {
	    rs = debugprinters(sbuf,slen) ;
	    wlen = rs ;
	} /* end if (ok) */

	if (abuf != nullptr) uc_free(abuf) ;

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (debugprinter) */

#else /* CF_USEMALLOC */

#ifdef	lint
int debugprinter(cchar *sbuf,int slen) noex {
	cint		alen = (LINEBUFLEN+2) ; /* room for added EOL */
	int		rs = SR_OK ;
	int		wlen = 0 ;
	int		f_needeol = false ;
	char		abuf[LINEBUFLEN+3] ; /* room for added EOL */

#if	CF_LINELEN
	slen = strlinelen(sbuf,slen,LINEBUFLEN) ; /* some protection */
#else
	if (slen < 0) slen = lenstr(sbuf) ;
	if (slen > LINEBUFLEN) slen = LINEBUFLEN ;
#endif /* CF_LINELEN */

/* preparation and check if need EOL */

	if ((slen == 0) || (sbuf[slen-1] != '\n')) {
	    f_needeol = true ;
	} else {
	    slen -= 1 ;
	}

/* scan for bad characters */

	if (f_needeol || hasprintbad(sbuf,slen)) {
	    if ((rs = snwcpyprintclean(abuf,(alen-2),sbuf,slen)) >= 0) {
		sbuf = abuf ;
		slen = rs ;
		abuf[slen++] = '\n' ;
	    }
	} else {
	    slen += 1 ;
	}

/* write the line-buffer out */

	if (rs >= 0) {
	    rs = debugprinters(sbuf,slen) ;
	    wlen = rs ;
	} /* end if (ok) */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (debugprinter) */
#else /* lint */
int debugprinter(cchar *sbuf,int slen) noex {
	int		rs = SR_OK ;
	int		wlen = 0 ; /* return-value */
	int		alen ;
	int		f_needeol = false ;

#if	CF_LINELEN
	slen = strlinelen(sbuf,slen,LINEBUFLEN) ; /* some protection */
#else
	if (slen < 0) slen = lenstr(sbuf) ;
	if (slen > LINEBUFLEN) slen = LINEBUFLEN ;
#endif /* CF_LINELEN */

/* preparation and check if need EOL */

	if ((slen == 0) || (sbuf[slen-1] != '\n')) {
	    f_needeol = true ;
	} else {
	    slen -= 1 ;
	}

	alen = (slen+2) ; /* room for added EOL */
	{
	    char	abuf[alen+1] ;
	    /* scan for bad characters */
	    if (f_needeol || hasprintbad(sbuf,slen)) {
	        if ((rs = snwcpyprintclean(abuf,(alen-2),sbuf,slen)) >= 0) {
		    sbuf = abuf ;
		    slen = rs ;
		    abuf[slen++] = '\n' ;
	        }
	    } else {
	        slen += 1 ;
	    }
	    /* write the line-buffer out */
	    if (rs >= 0) {
	        rs = debugprinters(sbuf,slen) ;
	        wlen = rs ;
	    } /* end if (ok) */

	} /* end block (dynamic stack buffer allocation) */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (debugprinter) */
#endif /* lint */

#endif /* CF_USEMALLOC */


/* local subroutines */

local int debugprinters(cchar *sbuf,int slen) noex {
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	if ((rs = debugprint_init()) >= 0) {
	    DEBUGPRINT	*uip = &ef ;
	    ptm *mxp = &uip->mx ;
	    if ((rs = mxp->lockbegin) >= 0) { /* single */
		int	cmd = F_LOCK ;
		if ((rs = u_lockf(ef.fd,cmd,0z)) >= 0) {
		    if (ustat sb ; (rs = u_fstat(ef.fd,&sb)) >= 0) {
			cint fsz = intconv(sb.st_size) ;
	                if (S_ISREG(sb.st_mode) && (fsz != ef.sz)) {
	                    coff	uoff = sb.st_size ;
	                    ef.sz = fsz ;
	                    u_seek(ef.fd,uoff,SEEK_SET) ;
	                }
	                if ((rs = u_write(ef.fd,sbuf,slen)) >= 0) {
	                    wlen = rs ;
	                    ef.sz += wlen ;
	                }
		    } /* end if (u_fstat) */
		    cmd = F_UNLOCK ;
		    rs1 = u_lockf(ef.fd,cmd,0z) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (uc_lockf) */
	        rs1 = mxp->lockend ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (ptm) */
	} /* end if (debugprint_init) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (debugprinters) */

local void debugprint_atforkbefore() noex {
	DEBUGPRINT	*uip = &ef ;
	{
	    ptm *mxp = &uip->mx ;
	    mxp->lockbegin() ;
	}
}
/* end subroutine (debugprint_atforkbefore) */

local void debugprint_atforkafter() noex {
	DEBUGPRINT	*uip = &ef ;
	{
	    ptm *mxp = &uip->mx ;
	    mxp->lockend() ;
	}
}
/* end subroutine (debugprint_atforkafter) */

local char *convdeci(LONG num,char *endptr) noex {
	ULONG		unum = (ULONG) num ;
	char		*bp ;
	if (num < 0) unum = (- unum) ;
	bp = ulltostr(unum,endptr) ;
	if (num < 0) *--bp = '-' ;
	return bp ;
}
/* end subroutine (convdeci) */

local int cthexi(char *buf,int val) noex {
	cint		n = (2 * szof(int)) ;
	for (int i = (n - 1) ; i >= 0 ; i -= 1) {
	    buf[i] = cthextable[val & 0x0F] ;
	    val >>= 4 ;
	} /* end for */
	buf[n] = '\0' ;
	return n ;
}
/* end subroutine (cthexi) */

local int snwcpyprintclean(char *dbuf,int dlen,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		ch ;
	int		dl = 0 ;
	while (dlen-- && sl && *sp) {
	    ch = MKCHAR(*sp) ;
	    if (isprintbad(ch)) {
	        if (ch == '\n') {
		    ch = '¬' ;
	        } else {
		    ch = '¿' ;
		}
	    }
	    dbuf[dl++] = (char) ch ;
	    sp += 1 ;
	    sl -= 1 ;
	} /* end while */
	if ((sl != 0) && (*sp != '\0')) rs = SR_OVERFLOW ;
	dbuf[dl] = '\0' ;
	return (rs >= 0) ? dl : rs ;
}
/* end subroutine (snwcpyprintclean) */

#ifdef	COMMENT
local bool hasprintbad(cchar *sp,int sl) noex {
	int		f = false ;
	while (sl && *sp) {
	    f = isprintbad(sp[0] & 0xff) ;
	    if (f) break ;
	    sp += 1 ;
	    sl -= 1 ;
	} /* end while */
	return f ;
} /* end subroutine (hasprintbad) */
#endif /* COMMENT */


