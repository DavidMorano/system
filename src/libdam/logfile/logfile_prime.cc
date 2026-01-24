/* logfile_prime SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* perform logging operations on a file */
/* version %I% last-modified %G% */

#define	CF_CHMOD	0		/* always change file mode (if can) */
#define	CF_CREATE	0		/* always create the file? */

/* revision history:

	= 1998-02-01, David A­D­ Morano
	This object module was originally written to create a logging
	mechanism for PCS application programs.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	logfile

	Description:
	This is a module to operate on a logfile.  The subroutines
	in this module are:

		logfile_open
		logfile_write
		logfile_printf
		logfile_vprintf
		logfile_setid
		logfile_check
		logfile_checksize
		logfile_close

	Although masking interrupts during the locking of the logfile
	should NOT be necessary (since a terminated process releases
	all file locks that it may have), it is a good precaution
	against some stupid implementations of the NFS helper lock
	manager daemon.  We want to guard against a program terminating
	abruptly while its lock is retained (for a time) back at
	the server.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>
#include	<csignal>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdarg>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<getnodename.h>
#include	<sigblocker.h>
#include	<ascii.h>
#include	<lockfile.h>
#include	<opentmp.h>
#include	<strn.h>
#include	<strw.h>		/* |strwset(3uc)| */
#include	<strwcpy.h>
#include	<ncol.h>		/* |ncolchar(3uc)| */
#include	<mkx.h>
#include	<mklogid.h>
#include	<mkchar.h>
#include	<ischarx.h>
#include	<localmisc.h>		/* |NTABCOLS| + |TIMEBUFLEN| */

#include	"logfile.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| + |lenstr(3u)| */

/* local defines */

#define	LOGFILE_EXTRA	100

#undef	OUTBUFLEN
#define	OUTBUFLEN	(LOGFILE_LINELEN + 2 + LOGFILE_EXTRA)

#define	TMPBUFLEN	((2*LOGFILE_USERLEN)+40)

#if	CF_CREATE
#define	O_FLAGS1	(O_RDWR | O_APPEND | O_CREAT)
#define	O_FLAGS2	(O_RDWR | O_TRUNC | O_CREAT)
#else /* CF_CREATE */
#define	O_FLAGS1	(O_RDWR | O_APPEND)
#define	O_FLAGS2	(O_RDWR | O_TRUNC)
#endif /* CF_CREATE */

#define	TO_OPEN		(10 * 60)
#define	TO_DATA		9
#define	TO_IDLE		(2*60)
#define	TO_LOCK		4

#ifndef	F_LOCK
#define	F_ULOCK		0
#define	F_LOCK		1
#define	F_TLOCK		2
#define	F_TEST		3
#endif

#define	COLSTATE	struct colstate


/* imported namespaces */

using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	bufvprintf(char *,int,cchar *,va_list) noex ;
}

extern "C" {
    extern int uc_getpid() noex ;
    extern int uc_open(cchar *,int,mode_t) noex ;
    extern int uc_moveup(int,int) noex ;
    extern int uc_closeonexec(int,int) noex ;
    extern int uc_close(int) noex ;
    extern int uc_readln(int,void *,int) noex ;
    extern int uc_writen(int,cvoid *,int) noex ;
    extern int uc_writedesc(int,int,int) noex ;
    extern int uc_setappend(int,int) noex ;
    extern int uc_ftruncate(int,off_t) noex ;
    extern int uc_setappend(int,int) noex ;
} /* end extern */

namespace logfile_util {
    int logfile_copylock(logfile *,int) noex ;
}

using logfile_util::logfile_copylock ;	/* subroutine */


/* external variables */


/* local structures */

struct colstate {
	int		ncols ;
	int		ncol ;
} ; /* end struct */


/* forward references */

local int	logfile_loadid(logfile *,cchar *) noex ;
local int	logfile_mklogid(logfile *) noex ;
local int	logfile_fixlogid(logfile *,int) noex ;
local int	logfile_fileopen(logfile *) noex ;
local int	logfile_fileclose(logfile *) noex ;
local int	logfile_iflush(logfile *) noex ;
local int	logfile_mkentry(logfile *,time_t,cchar *,int) noex ;
local int	logfile_mkline(logfile *,cchar *,int) noex ;

local int	colstate_load(COLSTATE *,int,int) noex ;
local int	colstate_linecols(COLSTATE *,cchar *,int) noex ;

local int	mkclean(char *,int,cchar *,int) noex ;
local bool	hasourbad(cchar *,int) noex ;
local bool	isourbad(int) noex ;


/* local variables */

constexpr int		sigblockers[] = {
	SIGUSR1,
	SIGUSR2,
	SIGHUP,
	SIGTERM,
	SIGINT,
	SIGQUIT,
	SIGPIPE,
	0
} ; /* end array (sigblockers) */

constexpr int		logidlen = LOGFILE_LOGIDLEN ;
constexpr int		linelen = LOGFILE_LINELEN ;
constexpr int		userlen = LOGFILE_USERLEN ;

constexpr bool		f_chmod = CF_CHMOD ;


/* exported variables */


/* exported subroutines */

int logfile_open(logfile *op,cc *lfname,int of,mode_t operm,cc *logid) noex {
	int		rs = SR_FAULT ;
	if (op && lfname) ylikely {
	    rs = SR_INVALID ;
	    if (lfname[0]) ylikely {
	        memclear(op) ; /* dangerous */
	        op->oflags = of ;
	        op->operm = (operm & S_IAMB) ;
	        op->lfd = -1 ;
	        if (cchar *cp{} ; (rs = lm_strw(lfname,-1,&cp)) >= 0) {
	            op->fname = cp ;
	            op->bufsize = LOGFILE_BUFSIZE ;
	            op->len = 0 ;
	            if (void *vp{} ; (rs = lm_mall(op->bufsize,&vp)) >= 0) {
	                op->buf = charp(vp) ;
	                if ((rs = logfile_fileopen(op)) >= 0) {
		            if_constexpr (f_chmod) {
	                        if (op->operm) {
	                            u_fchmod(op->lfd,op->operm) ;
		                }
		            } /* end if_constexpr (f_chmod) */
		            if (rs >= 0) {
	                        if (logid && logid[0]) {
	                            rs = logfile_loadid(op,logid) ;
	                        } else {
	                            rs = logfile_mklogid(op) ;
		                }
		            } /* end if (ok) */
	                    if (rs >= 0) {
	                        op->percent = LOGFILE_PERCENT ;
		                op->magic = LOGFILE_MAGIC ;
	                    } /* end if (ok) */
	                    if (rs < 0) {
		                logfile_fileclose(op) ;
		            }
	                } /* end if (file-open) */
	                if (rs < 0) {
		            lm_free(op->buf) ;
		            op->buf = nullptr ;
	                }
	            } /* end if (memory-allocation) */
	            if (rs < 0) {
			void *vp = voidp(op->fname) ;
		        lm_free(vp) ;
		        op->fname = nullptr ;
	            }
	        } /* end if (memory-allocation) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (logfile_open) */

int logfile_close(logfile *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = logfile_magic(op)) >= 0) ylikely {
	    {
	        rs1 = logfile_fileclose(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->buf) ylikely {
	        rs1 = lm_free(op->buf) ;
	        if (rs >= 0) rs = rs1 ;
	        op->buf = nullptr ;
	    }
	    if (op->fname) ylikely {
		void *vp = voidp(op->fname) ;
	        rs1 = lm_free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        op->fname = nullptr ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (logfile_close) */

int logfile_printf(logfile *op,cchar *fmt,...) noex {
	va_list	ap ;
	int		rs ;
	if ((rs = logfile_magic(op)) >= 0) ylikely {
	    va_begin(ap,fmt) ;
	    rs = logfile_vprintf(op,fmt,ap) ;
	    va_end(ap) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (logfile_printf) */

int logfile_vprintf(logfile *op,cchar *fmt,va_list ap) noex {
    	cnullptr	np{} ;
	int		rs ;
	int		len = 0 ;
	if ((rs = logfile_magic(op,fmt,ap)) >= 0) {
	    cint	olen = OUTBUFLEN ;
	    char	obuf[OUTBUFLEN + 1] ;
	    if ((rs = bufvprintf(obuf,olen,fmt,ap)) >= 0) {
	        int	sl = rs ;
	        cchar	*sp = obuf ;
	        for (cchar *tp ; (tp = strnchr(sp,sl,'\n')) != np ; ) {
		    cint	tl = intconv(tp - sp) ;
	            {
	                rs = logfile_write(op,sp,tl) ;
	                len += rs ;
	            }
	            sl -= intconv((tp + 1) - sp) ;
	            sp = (tp + 1) ;
	            if (rs < 0) break ;
	        } /* end for */
	        if ((rs >= 0) && (sl > 0)) {
	            rs = logfile_write(op,sp,sl) ;
	            len += rs ;
	        }
	    } /* end if (bufvprintf) */
	} /* end if (magic) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (logfile_vprintf) */

int logfile_setid(logfile *op,cchar *logid) noex {
	int		rs ;
	if ((rs = logfile_magic(op,logid)) >= 0) {
	    rs = logfile_loadid(op,logid) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (logfile_setid) */

int logfile_chmod(logfile *op,mode_t operm) noex {
	int		rs ;
	if ((rs = logfile_magic(op)) >= 0) {
	    if ((rs = logfile_fileopen(op)) >= 0) {
	        if (operm > 0) {
		    mode_t	nom = (operm & S_IAMB) ;
		    rs = u_fchmod(op->lfd,nom) ;
	        }
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (logfile_chmod) */

/* exercise some (minimal) control */
/* ARGSUSED */
int logfile_control(logfile *op,int cmd,void *ap) noex {
	int		rs ;
	(void) ap ;
	if ((rs = logfile_magic(op)) >= 0) {
	    switch (cmd) {
	    case LOGFILE_CNOP:
	        rs = SR_OK ;
	        break ;
	    case LOGFILE_CSIZE:
	            if ((rs = logfile_fileopen(op)) >= 0) {
	                if (ustat sb ; (rs = u_fstat(op->lfd,&sb)) >= 0) {
	                    rs = intsat(sb.st_size) ;
			}
	            }
	        break ;
	    default:
	        rs = SR_INVALID ;
	        break ;
	    } /* end switch */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (logfile_control) */

/* maintenance a log file with respect to its length */
int logfile_checksize(logfile *op,int logsz) noex {
	int		rs ;
	int		f_oversize = false ;
	if ((rs = logfile_magic(op)) >= 0) {
	    if (logsz <= 0) {
		logsz = LOGFILE_LOGSIZE ;
	    }
	    if ((rs = logfile_fileopen(op)) >= 0) {
	        cint	loglimit = (logsz * (op->percent + 100)) / 100 ;
	        if (ustat sb ; (rs = u_fstat(op->lfd,&sb)) >= 0) {
	            if (sb.st_size > loglimit) {
	                f_oversize = true ;
	                rs = logfile_copylock(op,logsz) ;
	            } /* end if */
	        } /* end if (stat) */
	    } /* end if (file-open) */
	} /* end if (magic) */
	return (rs >= 0) ? f_oversize : rs ;
}
/* end subroutine (logfile_checksize) */

int logfile_check(logfile *op,time_t dt) noex {
	int		rs = SR_OK ;
	int		len = 0 ;
	if ((rs = logfile_magic(op)) >= 0) {
	    if (dt == 0) dt = time(nullptr) ;
	    if ((op->len > 0) && ((dt - op->ti_data) >= TO_DATA)) {
	        rs = logfile_iflush(op) ;
	        len = rs ;
	    } else if ((op->lfd >= 0) && (op->len == 0)) {
	        bool	f = true ;
	        f = f && ((dt - op->ti_open) >= TO_OPEN) ;
	        f = f && ((dt - op->ti_write) >= TO_IDLE) ;
	        if (f) {
	            rs = logfile_fileclose(op) ;
	            len = rs ;
	        }
	    } /* end if */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (logfile_check) */

int logfile_flush(logfile *op) noex {
	int		rs ;
	if ((rs = logfile_magic(op)) >= 0) {
	    if (op->len > 0) {
	        rs = logfile_iflush(op) ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (logfile_flush) */

int logfile_write(logfile *op,cchar *sp,int sl) noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = logfile_magic(op,sp)) >= 0) {
	    COLSTATE	cs ;
	    time_t	dt = time(nullptr) ;
	    cint	tmplen = TMPBUFLEN ;
	    int		clen ;
	    int		bl ;
	    cchar	*bp ;
	    char	tmpbuf[TMPBUFLEN+ 1] ;
	    if (sl < 0) sl = lenstr(sp) ;
	    if (sp[sl-1] == '\n') sl -= 1 ;
	    colstate_load(&cs,linelen,(logidlen + 1)) ;
	    clen = colstate_linecols(&cs,sp,sl) ;
	    /* perform any necessary cleanup */
	    bp = sp ;
	    bl = clen ;
	    if (hasourbad(sp,clen)) {
	        bp = tmpbuf ;
	        bl = mkclean(tmpbuf,tmplen,sp,clen) ;
	    }
	    /* OK, copy the new data into the buffer */
	    if ((rs = logfile_mkentry(op,dt,bp,bl)) >= 0) {
		len = rs ;
	        if ((dt - op->ti_data) >= TO_DATA) {
	            rs = logfile_iflush(op) ;
	        }
	        op->ti_write = dt ;
	    } /* end if (logfile_mkentry) */
	} /* end if (magic) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (logfile_write) */

int logfile_print(logfile *op,cchar *sbuf,int slen) noex {
	return logfile_write(op,sbuf,slen) ;
}
/* end subroutine (logfile_print) */


/* private subroutines */

local int logfile_loadid(logfile *op,cchar *logstr) noex {
	cint		outlen = LOGFILE_LOGIDLEN ;
	int		rs ;
	int		len = 0 ;
	for (int i = 0 ; (i < outlen) && logstr[i] ; i += 1) {
	    cint	ch = mkchar(logstr[i]) ;
	    if (isprintlatin(ch)) {
	        op->logid[len++] = logstr[i] ;
	    }
	} /* end for */
	op->logid[len] = '\0' ;
	rs = logfile_fixlogid(op,len) ;
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (logfile_loadid) */

local int logfile_mklogid(logfile *op) noex {
	int		rs ;
	int		rs1 ;
	int		ll = 0 ;
	if ((rs = uc_getpid()) >= 0) {
	    cint	pid = rs ;
	    if (char *nbuf ; (rs = lm_nn(&nbuf)) >= 0) {
	        cint	nlen = rs ;
	        if ((rs = getnodename(nbuf,nlen)) >= 0) {
	            cint	llen = LOGFILE_LOGIDLEN ;
	            char	*lbuf = op->logid ;
	            if ((rs = mklogid(lbuf,llen,nbuf,rs,pid)) >= 0) {
	                ll = rs ;
		        rs = logfile_fixlogid(op,rs) ;
	            }
	        } /* end if (getnodename) */
		rs1 = lm_free(nbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (uc_getpid) */
	return (rs >= 0) ? ll : rs ;
}
/* end subroutine (logfile_mklogid) */

local int logfile_fixlogid(logfile *op,int cl) noex {
	if (cl < NTABCOLS) {
	    strwset((op->logid + cl),' ',(NTABCOLS - cl)) ;
	    cl = NTABCOLS ;
	}
	op->logid[cl++] = '\t' ;
	op->logidlen = cl ;
	return cl ;
}
/* end subroutine (logfile_fixlogid) */

local int logfile_fileopen(logfile *op) noex {
	int		rs = SR_OK ;
	if (op->lfd < 0) {
	    cint	of = (op->oflags | O_FLAGS1) ;
	    if ((rs = uc_open(op->fname,of,op->operm)) >= 0) {
		op->lfd = rs ;
		if (op->lfd < 3) {
		    if ((rs = uc_moveup(op->lfd,3)) >= 0) {
			op->lfd = rs ;
		    }
		}
		if (rs >= 0) {
		    rs = uc_closeonexec(op->lfd,true) ;
		    op->ti_open = time(nullptr) ;
		}
		if (rs < 0) {
		    u_close(op->lfd) ;
		    op->lfd = -1 ;
		}
	    } /* end if (open) */
	} /* end if (needs to be opened) */
	return rs ;
}
/* end subroutine (logfile_fileopen) */

local int logfile_fileclose(logfile *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		len = 0 ;
	if (op->lfd >= 0) {
	    if (op->len > 0) {
	        len = logfile_iflush(op) ;
	        if (rs >= 0) rs = len ;
	    }
	    rs1 = uc_close(op->lfd) ;
	    if (rs >= 0) rs = rs1 ;
	    op->lfd = -1 ;
	}
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (logfile_fileclose) */

/* out internal verion of a flush */
local int logfile_iflush(logfile *op) noex {
	sigblocker	blocker ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		len = 0 ;
	int		f_havelock = false ;
	if (op->len > 0) {
	    if ((rs = logfile_fileopen(op)) >= 0) {
	        if ((rs = sigblocker_start(&blocker,sigblockers)) >= 0) {
	            rs1 = lockfile(op->lfd,F_WLOCK,0z,0z,TO_LOCK) ;
	            f_havelock = (rs1 >= 0) ;
		    /* write it, lock or no lock */
	            u_seek(op->lfd,0L,SEEK_END) ;
	            rs = uc_writen(op->lfd,op->buf,op->len) ;
	            len = rs ;
	            op->len = 0 ;
		    /* do we have a lock? -- unlock if we do */
	            if (f_havelock) {
			lockfile(op->lfd,F_ULOCK,0L,0L,0) ;
		    }
	    	    rs1 = sigblocker_finish(&blocker) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (sigblocker) */
	    } /* end if (file-open) */
	} /* end if (non-zero) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (logfile_iflush) */

local int logfile_mkentry(logfile *op,time_t dt,cc *sp,int sl) noex {
	int		rs = SR_OK ;
	int		rlen = (op->bufsize - op->len) ;
	int		ll = 0 ; /* return-value */
	if (sl < 0) sl = lenstr(sp) ;
	ll = (LOGFILE_LOGIDLEN + 1 + sl) ;
	{
	    if ((ll + 1) > rlen) {
		rs = logfile_iflush(op) ;
	    }
	    if (rs >= 0) {
	        if (op->len == 0) {
		    if (dt == 0) dt = time(nullptr) ;
		    op->ti_data = dt ;
	        }
	        rs = logfile_mkline(op,sp,sl) ;
	    } /* end if (ok) */
	} /* end block */
	return (rs >= 0) ? ll : rs ;
}
/* end subroutine (logfile_mkentry) */

/* create the entry right in the storage buffer itself */
local int logfile_mkline(logfile *op,cchar *sp,int sl) noex {
	int		len = 0 ;
	char		*buf = (op->buf + op->len) ;
	{
	    char	*bp = buf ;
	    bp = strwcpy(bp,op->logid,op->logidlen) ;
	    bp = strwcpy(bp,sp,min(LOGFILE_USERLEN,sl)) ;
	    bp = strwcpy(bp,"\n",1) ;
	    op->len += intconv(bp - buf) ;
	    len = intconv(bp - buf) ;
	}
	return len ;
}
/* end subroutine (logfile_mkline) */

local int colstate_load(COLSTATE *csp,int ncols,int ncol) noex {
	{
	    csp->ncols = ncols ;
	    csp->ncol = ncol ;
	}
	return SR_OK ;
}
/* end subroutine (colstate_load) */

/* return the number of characters that will fill the current column limit */
local int colstate_linecols(COLSTATE *csp,cchar *sbuf,int slen) noex {
	int		i ; /* used afterwards */
	int		cols ;
	int		rcols ;
	if (slen < 0) slen = lenstr(sbuf) ;
	rcols = (csp->ncols - csp->ncol) ;
	for (i = 0 ; (rcols > 0) && (i < slen) ; i += 1) {
	    cols = ncolchar(NTABCOLS,csp->ncol,sbuf[i]) ;
	    if (cols > rcols) break ;
	    csp->ncol += cols ;
	    rcols -= cols ;
	} /* end for */
	return i ;
}
/* end subroutine (colstate_linecols) */

local int mkclean(char *outbuf,int outlen,cchar *sbuf,int slen) noex {
	int		i ; /* used afterwards */
	for (i = 0 ; (i < outlen) && (i < slen) ; i += 1) {
	    outbuf[i] = sbuf[i] ;
	    if (isourbad(sbuf[i] & 0xff)) {
	        outbuf[i] = '¿' ;
	    }
	} /* end for */
	return i ;
}
/* end subroutine (mkclean) */

local bool hasourbad(cchar *sp,int sl) noex {
	bool		f = false ;
	while (sl && (sp[0] != '\0')) {
	    cint	ch = mkchar(sp[0]) ;
	    f = isourbad(ch) ;
	    if (f) break ;
	    sp += 1 ;
	    sl -= 1 ;
	} /* end if */
	return f ;
}
/* end subroutine (hasourbad) */

local bool isourbad(int ch) noex {
	bool		f = false ;
	switch (ch) {
	case CH_SO:
	case CH_SI:
	case CH_SS2:
	case CH_SS3:
	case '\t':
	    f = false ;
	    break ;
	default:
	    f = (! isprintlatin(ch)) ;
	    break ;
	} /* end switch */
	return f ;
}
/* end subroutine (isourbad) */


