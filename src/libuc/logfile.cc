/* logfile SUPPORT */
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

	This is a module to operate on a logfile. The subroutines
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
	manager daemon. We want to guard against a program terminating
	abruptly while its lock is retained (for a time) back at
	the server.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<csignal>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdarg>
#include	<cstring>		/* |strlen(3c)| */
#include	<usystem.h>
#include	<sigblocker.h>
#include	<ascii.h>
#include	<getnodename.h>
#include	<lockfile.h>
#include	<strn.h>
#include	<strw.h>
#include	<strwcpy.h>
#include	<mkx.h>
#include	<mkchar.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"logfile.h"


/* local defines */

#define	LOGFILE_EXTRA	100

#ifndef	NTABCOLS
#define	NTABCOLS	8
#endif

#undef	OUTBUFLEN
#define	OUTBUFLEN	(LOGFILE_LINELEN + 2 + LOGFILE_EXTRA)

#define	TMPBUFLEN	((2*LOGFILE_USERLEN)+40)

#ifndef	TIMEBUFLEN
#define	TIMEBUFLEN	80
#endif

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


/* external subroutines */

extern "C" {
    int		logfile_write(logfile *,cchar *,int) noex ;
    int		logfile_vprintf(logfile *,cchar *,va_list) noex ;
}

extern int	opentmpfile(cchar *,int,mode_t,char *) ;
extern int	opentmp(cchar *,int,mode_t) ;
extern int	vbufprintf(char *,int,cchar *,va_list) ;
extern int	charcols(int,int,int) ;


/* external variables */


/* local structures */

struct colstate {
	int		ncols ;
	int		ncol ;
} ;


/* forward references */

static int	logfile_loadid(logfile *,cchar *) noex ;
static int	logfile_mklogid(logfile *) noex ;
static int	logfile_fixlogid(logfile *,int) noex ;
static int	logfile_fileopen(logfile *) noex ;
static int	logfile_fileclose(logfile *) noex ;
static int	logfile_iflush(logfile *) noex ;
static int	logfile_copylock(logfile *,int) noex ;
static int	logfile_mkentry(logfile *,time_t,cchar *,int) noex ;
static int	logfile_mkline(logfile *,cchar *,int) noex ;

static int	colstate_load(COLSTATE *,int,int) noex ;
static int	colstate_linecols(COLSTATE *,cchar *,int) noex ;

static int	mkclean(char *,int,cchar *,int) noex ;
static bool	hasourbad(cchar *,int) noex ;
static bool	isourbad(int) noex noex ;


/* local variables */

static constexpr int	sigblockers[] = {
	SIGUSR1,
	SIGUSR2,
	SIGHUP,
	SIGTERM,
	SIGINT,
	SIGQUIT,
	SIGPIPE,
	0
} ;


/* exported variables */


/* exported subroutines */

int logfile_open(logfile *op,cc *lfname,int of,mode_t operm,cc *logid) noex {
	int		rs ;
	cchar		*cp ;

	if (op == NULL) return SR_FAULT ;
	if (lfname == NULL) return SR_FAULT ;

	if (lfname[0] == '\0') return SR_INVALID ;

	memclear(op) ;			/* dangerous */
	op->oflags = of ;
	op->operm = (operm & S_IAMB) ;
	op->lfd = -1 ;

	if ((rs = uc_mallocstrw(lfname,-1,&cp)) >= 0) {
	    void	*p ;
	    op->fname = cp ;
	    op->bufsize = LOGFILE_BUFSIZE ;
	    op->len = 0 ;
	    if ((rs = uc_malloc(op->bufsize,&p)) >= 0) {
	        op->buf = p ;
	        if ((rs = logfile_fileopen(op)) >= 0) {
#if	CF_CHMOD
	            if (pip->operm >= 0)
	                u_fchmod(op->lfd,pip->operm) ;
#endif
	            if ((logid != NULL) && (logid[0] != '\0')) {
	                rs = logfile_loadid(op,logid) ;
	            } else {
	                rs = logfile_mklogid(op) ;
		    }
	            if (rs >= 0) {
	                op->percent = LOGFILE_PERCENT ;
		        op->magic = LOGFILE_MAGIC ;
	            }
	            if (rs < 0)
		        logfile_fileclose(op) ;
	        } /* end if (file-open) */
	        if (rs < 0) {
		    uc_free(op->buf) ;
		    op->buf = NULL ;
	        }
	    } /* end if (memory-allocation) */
	    if (rs < 0) {
		uc_free(op->fname) ;
		op->fname = NULL ;
	    }
	} /* end if (memory-allocation) */

	return (rs >= 0) ? op->lfd : rs ;
}
/* end subroutine (logfile_open) */

int logfile_close(logfile *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != LOGFILE_MAGIC) return SR_NOTOPEN ;

	rs1 = logfile_fileclose(op) ;
	if (rs >= 0) rs = rs1 ;

	if (op->buf != NULL) {
	    rs1 = uc_free(op->buf) ;
	    if (rs >= 0) rs = rs1 ;
	    op->buf = NULL ;
	}

	if (op->fname != NULL) {
	    rs1 = uc_free(op->fname) ;
	    if (rs >= 0) rs = rs1 ;
	    op->fname = NULL ;
	}

	op->magic = 0 ;
	return rs ;
}
/* end subroutine (logfile_close) */

int logfile_printf(logfile *op,cchar *fmt,...) noex {
	int		rs ;
	{
	    va_list	ap ;
	    va_begin(ap,fmt) ;
	    rs = logfile_vprintf(op,fmt,ap) ;
	    va_end(ap) ;
	} /* end block */
	return rs ;
}
/* end subroutine (logfile_printf) */

int logfile_vprintf(logfile *op,cchar *fmt,va_list ap) noex {
	int		rs = SR_OK ;
	int		sl ;
	int		ol ;
	int		len = 0 ;
	cchar	*tp, *sp ;
	char		outbuf[OUTBUFLEN + 1] ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != LOGFILE_MAGIC) return SR_BADF ;

/* format the user's portion */

	ol = vbufprintf(outbuf,OUTBUFLEN,fmt,ap) ;
	if (ol < 0) ol = 0 ;

#ifdef	COMMENT
	if (ol > LOGFILE_USERLEN)
	    ol = LOGFILE_USERLEN ;
#endif

	sp = outbuf ;
	sl = ol ;
	while ((tp = strnchr(sp,sl,'\n')) != NULL) {

	    rs = logfile_write(op,sp,(tp - sp)) ;
	    len += rs ;

	    sl -= ((tp + 1) - sp) ;
	    sp = (tp + 1) ;

	    if (rs < 0) break ;
	} /* end while */

	if ((rs >= 0) && (sl > 0)) {
	    rs = logfile_write(op,sp,sl) ;
	    len += rs ;
	}

	return (rs >= 0) ? len : rs ;
}
/* end subroutine (logfile_vprintf) */

int logfile_setid(logfile *op,cchar *logid) noex {
	int		rs ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != LOGFILE_MAGIC) return SR_NOTOPEN ;

	if (logid == NULL) logid = "*null*" ;

	rs = logfile_loadid(op,logid) ;

	return rs ;
}
/* end subroutine (logfile_setid) */

int logfile_chmod(logfile *op,mode_t operm) noex {
	int		rs ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != LOGFILE_MAGIC) return SR_NOTOPEN ;

	if ((rs = logfile_fileopen(op)) >= 0) {
	    if (operm > 0) {
		mode_t	nom = (operm & S_IAMB) ;
		rs = u_fchmod(op->lfd,nom) ;
	    }
	}

	return rs ;
}
/* end subroutine (logfile_chmod) */

/* exercise some (minimal) control */
/* ARGSUSED */
int logfile_control(logfile *op,int cmd,void *ap) noex {
	int		rs = SR_OK ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != LOGFILE_MAGIC) return SR_NOTOPEN ;

	switch (cmd) {
	case LOGFILE_CNOP:
	    rs = SR_OK ;
	    break ;
	case LOGFILE_CSIZE:
	    {
	        if ((rs = logfile_fileopen(op)) >= 0) {
	            USTAT	sb ;
	            rs = u_fstat(op->lfd,&sb) ;
	            if (rs >= 0) rs = (sb.st_size & INT_MAX) ;
	        }
	    }
	    break ;
	default:
	    rs = SR_INVALID ;
	    break ;
	} /* end switch */

	return rs ;
}
/* end subroutine (logfile_control) */

/* maintenance a log file with respect to its length */
int logfile_checksize(logfile *op,int logsize) noex {
	int		rs ;
	int		f_oversize = FALSE ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != LOGFILE_MAGIC) return SR_NOTOPEN ;

	if (logsize <= 0)
	    logsize = LOGFILE_LOGSIZE ;

	if ((rs = logfile_fileopen(op)) >= 0) {
	    USTAT	sb ;
	    cint	loglimit = (logsize * (op->percent + 100)) / 100 ;
	    if ((rs = u_fstat(op->lfd,&sb)) >= 0) {
	        if (sb.st_size > loglimit) {
	            f_oversize = TRUE ;
	            rs = logfile_copylock(op,logsize) ;
	        } /* end if */
	    } /* end if (stat) */
	} /* end if (file-open) */

	return (rs >= 0) ? f_oversize : rs ;
}
/* end subroutine (logfile_checksize) */

int logfile_check(logfile *op,time_t daytime) noex {
	int		rs = SR_OK ;
	int		len = 0 ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != LOGFILE_MAGIC) return SR_NOTOPEN ;

	if (daytime == 0) daytime = time(NULL) ;

	if ((op->len > 0) && ((daytime - op->ti_data) >= TO_DATA)) {
	    rs = logfile_iflush(op) ;
	    len = rs ;
	} else if ((op->lfd >= 0) && (op->len == 0)) {
	    int	f = TRUE ;
	    f = f && ((daytime - op->ti_open) >= TO_OPEN) ;
	    f = f && ((daytime - op->ti_write) >= TO_IDLE) ;
	    if (f) {
	        rs = logfile_fileclose(op) ;
	        len = rs ;
	    }
	} /* end if */

	return (rs >= 0) ? len : rs ;
}
/* end subroutine (logfile_check) */

int logfile_flush(logfile *op) noex {
	int		rs = SR_OK ;

	if (op == NULL) return SR_FAULT ;

	if (op->magic != LOGFILE_MAGIC) return SR_NOTOPEN ;

	if (op->len > 0) {
	    rs = logfile_iflush(op) ;
	}

	return rs ;
}
/* end subroutine (logfile_flush) */

int logfile_write(logfile *op,cchar *sbuf,int slen) noex {
	COLSTATE	cs ;
	time_t		daytime = time(NULL) ;
	cint	tmplen = TMPBUFLEN ;
	int		rs = SR_OK ;
	int		clen ;
	int		bl ;
	int		len = 0 ;
	cchar	*bp ;
	char		tmpbuf[TMPBUFLEN+ 1] ;

	if (op == NULL) return SR_FAULT ;
	if (sbuf == NULL) return SR_FAULT ;

	if (op->magic != LOGFILE_MAGIC) return SR_NOTOPEN ;

	if (slen < 0)
	    slen = strlen(sbuf) ;

	if (sbuf[slen-1] == '\n')
	    slen -= 1 ;

	colstate_load(&cs,LOGFILE_LINELEN,(LOGFILE_LOGIDLEN + 1)) ;

	clen = colstate_linecols(&cs,sbuf,slen) ;

/* perform any necessary cleanup */

	bp = sbuf ;
	bl = clen ;
	if (hasourbad(sbuf,clen)) {
	    bp = tmpbuf ;
	    bl = mkclean(tmpbuf,tmplen,sbuf,clen) ;
	}

/* OK, copy the new data into the buffer */

	rs = logfile_mkentry(op,daytime,bp,bl) ;
	len = rs ;

	if ((rs >= 0) && ((daytime - op->ti_data) >= TO_DATA)) {
	    rs = logfile_iflush(op) ;
	}

	if (rs >= 0) op->ti_write = daytime ;

	return (rs >= 0) ? len : rs ;
}
/* end subroutine (logfile_write) */

int logfile_print(logfile *op,cchar *sbuf,int slen) noex {
	return logfile_write(op,sbuf,slen) ;
}
/* end subroutine (logfile_print) */


/* private subroutines */

static int logfile_loadid(logfile *op,cchar *logstr) noex {
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

static int logfile_mklogid(logfile *op) noex {
	const pid_t	pid = uc_getpid() ;
	cint		nlen = NODENAMELEN ;
	int		rs ;
	int		ll = 0 ;
	char		nbuf[NODENAMELEN + 1] ;

	if ((rs = getnodename(nbuf,nlen)) >= 0) {
	    cint	llen = LOGFILE_LOGIDLEN ;
	    int		v = pid ;
	    char	*lbuf = op->logid ;
	    if ((rs = mklogid(lbuf,llen,nbuf,rs,v)) >= 0) {
	        ll = rs ;
		rs = logfile_fixlogid(op,rs) ;
	    }
	} /* end if (getnodename) */

	return (rs >= 0) ? ll : rs ;
}
/* end subroutine (logfile_mklogid) */

static int logfile_fixlogid(logfile *op,int cl) noex {
	if (cl < NTABCOLS) {
	    strwset((op->logid + cl),' ',(NTABCOLS - cl)) ;
	    cl = NTABCOLS ;
	}
	op->logid[cl++] = '\t' ;
	op->logidlen = cl ;
	return cl ;
}
/* end subroutine (logfile_fixlogid) */

static int logfile_fileopen(logfile *op) noex {
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
		    rs = uc_closeonexec(op->lfd,TRUE) ;
		    op->ti_open = time(NULL) ;
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

static int logfile_fileclose(logfile *op) noex {
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
static int logfile_iflush(logfile *op) noex {
	sigblocker	blocker ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		len = 0 ;
	int		f_havelock = FALSE ;

	if (op->len > 0) {
	    if ((rs = logfile_fileopen(op)) >= 0) {
	        if ((rs = sigblocker_start(&blocker,sigblockers)) >= 0) {

	            rs1 = lockfile(op->lfd,F_WLOCK,0L,0L,TO_LOCK) ;
	            f_havelock = (rs1 >= 0) ;

/* write it, lock or no lock */

	            u_seek(op->lfd,0L,SEEK_END) ;

	            rs = uc_writen(op->lfd,op->buf,op->len) ;
	            len = rs ;
	            op->len = 0 ;

/* do we have a lock? -- unlock if we do */

	            if (f_havelock) lockfile(op->lfd,F_ULOCK,0L,0L,0) ;

	    	    sigblocker_finish(&blocker) ;
	        } /* end if (sigblocker) */
	    } /* end if (file-open) */
	} /* end if (non-zero) */

	return (rs >= 0) ? len : rs ;
}
/* end subroutine (logfile_iflush) */

static int logfile_copylock(logfile *op,int logsize) noex {
	sigblocker	blocker ;
	int		rs ;
	int		rs1 ;

	if ((rs = sigblocker_start(&blocker,sigblockers)) >= 0) {
	    if ((rs = lockfile(op->lfd,F_WLOCK,0L,0L,TO_LOCK)) >= 0) {

	        if ((rs = opentmp(NULL,0,0644)) >= 0) {
		    off_t	uoff = (- logsize) ;
		    off_t	foff ;
		    cint	llen = LINEBUFLEN ;
	            int		fd = rs ;
		    int		ll ;
		    char	lbuf[LINEBUFLEN + 1] ;

	            if ((rs = u_seek(op->lfd,uoff,SEEK_END)) >= 0) {
	                while ((rs = uc_readline(op->lfd,lbuf,llen)) > 0) {
	                    ll = rs ;
	                    if (lbuf[ll - 1] == '\n') break ;
	                } /* end while */
	            } /* end if (seek) */

/* copy the rest of the file to the temporary file */

	            if (rs >= 0) {
	                if ((rs = uc_writedesc(fd,op->lfd,-1)) >= 0) {

	                u_rewind(op->lfd) ;

	                u_rewind(fd) ;

/* shut off any APPEND mode if there is any */

	                if ((rs1 = uc_setappend(op->lfd,FALSE)) >= 0) {
	                    int	f_append = (rs1 > 0) ;

/* copy the temporary data back to the original file */

	                    if ((rs = uc_writedesc(op->lfd,fd,-1)) >= 0) {
	                        ll = rs ;
	                        foff = ll ;
	                        if (foff > 0)
	                            uc_ftruncate(op->lfd,foff) ;
			    }

/* turn on any APPEND mode if there it was on */

	                    if (f_append) uc_setappend(op->lfd,TRUE) ;
	                } /* end if (append-mode) */

	            } /* end if (successfull-copy) */
		    } /* end if (ok) */

	            u_close(fd) ;
	        } /* end if (open-tmp) */

	        rs1 = lockfile(op->lfd,F_ULOCK,0L,0L,TO_LOCK) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (lock-file) */
	    sigblocker_finish(&blocker) ;
	} /* end if (sigblocker) */

	return rs ;
}
/* end subroutine (logfile_copylock) */

static int logfile_mkentry(logfile *op,time_t daytime,cc *sbuf,int slen) noex {
	int		rs = SR_OK ;
	int		rlen = (op->bufsize-op->len) ;
	int		ll ;
	if (slen < 0) slen = strlen(sbuf) ;
	ll = (LOGFILE_LOGIDLEN+1+slen) ;
	    if ((ll+1) > rlen) rs = logfile_iflush(op) ;
	    if (rs >= 0) {
	        if (op->len == 0) {
		    if (daytime == 0) daytime = time(NULL) ;
		    op->ti_data = daytime ;
	        }
	        rs = logfile_mkline(op,sbuf,slen) ;
	    } /* end if */
	return (rs >= 0) ? ll : rs ;
}
/* end subroutine (logfile_mkentry) */

/* create the entry right in the storage buffer itself */
static int logfile_mkline(logfile *op,cchar *sbuf,int slen) noex {
	char		*buf = (op->buf + op->len) ;
	char		*bp ;

	bp = buf ;
	bp = strwcpy(bp,op->logid,op->logidlen) ;
	bp = strwcpy(bp,sbuf,MIN(LOGFILE_USERLEN,slen)) ;
	bp = strwcpy(bp,"\n",1) ;

	op->len += (bp-buf) ;
	return (bp-buf) ;
}
/* end subroutine (logfile_mkline) */

static int colstate_load(COLSTATE *csp,int ncols,int ncol) noex {
	csp->ncols = ncols ;
	csp->ncol = ncol ;
	return SR_OK ;
}
/* end subroutine (colstate_load) */

/* return the number of characters that will fill the current column limit */
static int colstate_linecols(COLSTATE *csp,cchar *sbuf,int slen) noex {
	int		i ; /* used afterwards */
	int		cols ;
	int		rcols ;
	if (slen < 0) slen = strlen(sbuf) ;
	rcols = (csp->ncols - csp->ncol) ;
	for (i = 0 ; (rcols > 0) && (i < slen) ; i += 1) {
	    cols = charcols(NTABCOLS,csp->ncol,sbuf[i]) ;
	    if (cols > rcols) break ;
	    csp->ncol += cols ;
	    rcols -= cols ;
	} /* end for */
	return i ;
}
/* end subroutine (colstate_linecols) */

static int mkclean(char *outbuf,int outlen,cchar *sbuf,int slen) noex {
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

static bool hasourbad(cchar *sp,int sl) noex {
	bool		f = FALSE ;
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

static bool isourbad(int ch) noex {
	bool		f = false ;
	switch (ch) {
	case CH_SO:
	case CH_SI:
	case CH_SS2:
	case CH_SS3:
	case '\t':
	    f = FALSE ;
	    break ;
	default:
	    f = (! isprintlatin(ch)) ;
	    break ;
	} /* end switch */
	return f ;
}
/* end subroutine (isourbad) */


