/* shio SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* the SHell-IO hack */
/* version %I% last-modified %G% */

#define	CF_FLUSHPART	0		/* partial flush */
#define	CF_SFSWAP	0		/* use 'sfswap(3ast)' */
#define	CF_STOREFNAME	0		/* store the file-name */

/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	shio

	Desfription:
	This is a hack to make the SFIO stuff useable in an
	interchangeable way when not in an SFIO environment.  This
	generally occurs when a subroutine is used both in a SHELL
	builtin as well as stand-alone.

	Notes:

	1. If we are *outside* of KSH, we want to switch to using
	the BFILE (our own) I-O even if the file we are to operate
	on is one of the "standard" (in, out, err) files.  We do
	this so that we do not have to use the crazy SFIO where we
	do not really need it.  When we are *inside* KSH, we have
	to use SFIO because KSH passes us special "memory-implemented"
	(my term) files.  We need to use the memory based files
	passed us from KSH in order for the KSH variable assignments
	and other special operations for input and output to work
	correctly.  We implement knowing whether we are "inside"
	or "outside" of KSH by using the |shio_outside()| subroutine.
	That subroutine finds out whether we are inside or outside
	only once (that is what it is all about) so that we remember
	that condition for the remainder of our stay loaded into
	this nice computer.  We increasingly live in a
	terminate-and-stay-resident world; we have to start programming
	for that.

	2. How would we ever be "outside" of KSH?  You may ask?  We
	are just a shared-object.  What if someone other than KSH
	calls us?  In that case, we would be executing, but we would
	be executing "outside" of KSH.  This does happen.  Servers
	and other programs call us (not uncommonly) without KSH
	being in the mix anywhere.  For example, when any of the
	"commands" are called from outside of KSH, like from another
	shell that is not KSH, we will be executing "outside" of
	KSH.  So this is actually much more common than one might
	think at first.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */

#if	defined(SFIO) && (SFIO > 0)
#define	CF_SFIO	1
#else
#define	CF_SFIO	0
#endif

#if	(defined(KSHBUILTIN) && (KSHBUILTIN > 0))
#include	<shell.h>
#endif

#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<dlfcn.h>
#include	<climits>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdarg>
#include	<cstring>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<mallocxx.h>
#include	<linebuffer.h>
#include	<ascii.h>
#include	<sigblocker.h>
#include	<tcx.h>
#include	<bfile.h>
#include	<outstore.h>
#include	<strn.h>
#include	<snwcpy.h>
#include	<sncpyx.h>
#include	<mktmp.h>
#include	<mkpathx.h>
#include	<matstr.h>
#include	<fmtstr.h>
#include	<isoneof.h>
#include	<localmisc.h>

#include	"shio.h"


/* local defines */

#define	SHIO_MODESTRLEN	20
#define	SHIO_KSHSYM	"sh_main"
#define	SHIO_PERSISTENT	struct shio_persistent

#ifndef	SFIO_OFF
#define	SFIO_OFF	0
#endif

#ifndef	SFIO_ON
#define	SFIO_ON		1
#endif

#define	SFIO_MAXREADLEN	100

#ifndef	LINEBUFLEN
#ifdef	LINE_MAX
#define	LINEBUFLEN	MAX(LINE_MAX,2048)
#else
#define	LINEBUFLEN	2048
#endif
#endif

#ifndef	VARTMPDNAME
#define	VARTMPDNAME	"TMPDIR"
#endif

#ifndef	TMPDNAME
#define	TMPDNAME	"/tmp"
#endif

#ifndef	nullptrDEV
#define	nullptrDEV		"/dev/null"
#endif

#define	ISCONT(b,bl)	\
	(((bl) >= 2) && ((b)[(bl) - 1] == '\n') && ((b)[(bl) - 2] == '\\'))

#define	NDF		"shio.deb"


/* imported namespaces */


/* local typedefs */

typedef bfile *		bfilep ;


/* external subroutines */

#if	CF_SFIO
extern int	sfreadline(Sfio_t *,char *,int) ;
extern int	sfreadlinetimed(Sfio_t *,char *,int,int) ;
extern int	sfisterm(Sfio_t *) ;
#endif


/* local structures */

struct shio_persistent {
	volatile uint	f_init ;
	volatile uint	f_outside ;
} ;


/* forward references */

template<typename ... Args>
static int shio_ctor(shio *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_OK ;

	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (shio_ctor) */

static int shio_dtor(shio *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (shio_dtor) */

template<typename ... Args>
static inline int shio_magic(shio *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == SHIO_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (shio_magic) */

static int	shio_bopene(SHIO *,int,cchar *,cchar *,mode_t,int) noex ;
static int	shio_bclose(SHIO *) noex ;

#if	CF_SFIO
static int	shio_outside() noex ;
static int	shio_sfiscook(SHIO *) noex ;
static int	shio_sfflush(SHIO *) noex ;
static int	shio_shprintln(SHIO *,cchar *,int) noex ;
static int	shio_sfwrite(SHIO *,cchar *,int) noex ;
static int	shio_sfcookline(SHIO *op,int f) noex ;
static int	shio_sfcookbegin(SHIO *) noex ;
static int	shio_sfcookend(SHIO *) noex ;
static int	shio_sfcheckwrite(SHIO *,cchar *,int) noex ;
static int	shio_sfcookdump(SHIO *) noex ;
static int	shio_sfcookflush(SHIO *) noex ;
static int	shio_sfcookwrite(SHIO *,cchar *,int) noex ;
#endif /* CF_SFIO */

static bool	hasnl(cchar *,int) noex ;
static bool	isNotSeek(int) noex ;
static bool	isInterrupt(volatile int **) noex ;


/* static writable data */

#if	CF_SFIO
static struct shio_persistent	shio_data ; /* initialized to all zeros */
#endif /* CF_SFIO */


/* local variables */

enum stdfnames {
	stdfname_stdin,
	stdfname_stdout,
	stdfname_stderr,
	stdfname_stdnull,
	stdfname_oldnull,
	stdfname_overlast
} ;

constexpr cpcchar	stdfnames[] = {
	STDFNIN,
	STDFNOUT,
	STDFNERR,
	STDFNNULL,
	"*nullptr*",
	nullptr
} ;

constexpr int		seekrs[] = {
	SR_NOTSEEK,
	0
} ;


/* exported variables */


/* exported subroutines */

int shio_opene(SHIO *op,cchar *fname,cchar *ms,mode_t om,int to) noex {
	int		rs = SR_OK ;
	int		fni ;
#if	CF_SFIO
	Sfio_t		*fp = nullptr ;
#endif
	cchar	*ofname = nullptr ;

	if (op == nullptr) return SR_FAULT ;
	if (fname == nullptr) return SR_FAULT ;
	if (ms == nullptr) return SR_FAULT ;

	if (fname[0] == '\0') return SR_INVALID ;
	if (ms[0] == '\0') return SR_INVALID ;

/* start in */

	memclear(op) ;

	fni = matstr(stdfnames,fname,-1) ;

	op->fl.stdfname = (fni >= 0) ;
	switch (fni) {
	case stdfname_stdin:
	    ofname = BFILE_STDIN ;
#if	CF_SFIO
	    fp = sfstdin ;
#endif
	    break ;
	case stdfname_stdout:
	    ofname = BFILE_STDOUT ;
#if	CF_SFIO
	    fp = sfstdout ;
#endif
	    break ;
	case stdfname_stderr:
	    ofname = BFILE_STDERR ;
#if	CF_SFIO
	    fp = sfstderr ;
#endif
	    break ;
	case stdfname_stdnull:
	case stdfname_oldnull:
	    op->fl.nullfile = true ;
	    break ;
	default:
	    ofname = fname ;
	    break ;
	} /* end switch */

	if (op->fl.nullfile) goto ret1 ;

#if	CF_SFIO

	if (fp) {
	    if (shio_outside()) fp = nullptr ; /* if outside switch to BFILE */
	}

	if ((fni >= 0) && (fp != nullptr)) {
	    int		rs1 ;

	    op->fl.sfio = true ;
#if	CF_SFSWAP
	    rs1 = sfsync(fp) ;
	    if (rs1 >= 0) {
	        op->fp = sfswap(fp,nullptr) ;
	        rs = (op->fp != nullptr) ? SR_OK : SR_MFILE ;
	    } else
	        rs = SR_BADF ;
#else
	    op->fp = fp ;
#endif /* CF_SFSWAP */

	    if (rs >= 0) {
	        if ((rs1 = sffileno(op->fp)) >= 0) {
	            if (isatty(rs1)) {
	                op->fl.terminal = true ;
	                op->fl.bufline = true ;
	            }
	        }
	        if (op->fl.bufline) {
	            rs = shio_sfcookbegin(op) ;
	        } /* end if (buffered) */
	    } /* end if (ok) */

	} else {

	    rs = shio_bopene(op,fni,fname,ms,om,to) ;

	} /* end if */

#else /* CF_SFIO */

	rs = shio_bopene(op,fni,ofname,ms,om,to) ;

#endif /* CF_SFIO */

#if	CF_SFIO
#else

	if (rs >= 0) {
	    rs = bcontrol(op->fp,BC_ISLINEBUF,0) ;
	    op->fl.bufline = (rs > 0) ;
	}

	if (rs >= 0) {
	    rs = bcontrol(op->fp,BC_ISTERMINAL,0) ;
	    op->fl.terminal = (rs > 0) ;
	}

#endif /* CF_SFIO */

#if	CF_STOREFNAME
	if (rs >= 0) {
	    cchar	*cp ;
	    if ((rs = uc_mallocstrw(ofname,-1,&cp)) >= 0) {
	        op->fname = cp ;
	    }
	}
#endif /* CF_STOREFNAME */

ret1:
	if (rs >= 0)
	    op->magic = SHIO_MAGIC ;

	return rs ;
}
/* end subroutine (shio_opene) */

int shio_open(SHIO *op,cchar *fname,cchar *ms,mode_t om) noex {
	return shio_opene(op,fname,ms,om,-1) ;
}
/* end subroutine (shio_open) */

int shio_opentmp(SHIO *op,mode_t om) noex {
	int		rs ;
	cchar	*tfn = "shioXXXXXXXXXX" ;
	cchar	*tmpdname = getenv(VARTMPDNAME) ;
	char		tpat[MAXPATHLEN+1] ;

	if (tmpdname == nullptr) tmpdname = TMPDNAME ;

	if ((rs = mkpath2(tpat,tmpdname,tfn)) >= 0) {
	    SIGBLOCK	blocker ;
	    if ((rs = sigblocker_start(&blocker,nullptr)) >= 0) {
	        char	tbuf[MAXPATHLEN+1] ;
	        if ((rs = mktmpfile(tbuf,om,tpat)) >= 0) {
	            rs = shio_opene(op,tbuf,"rw",om,-1) ;
	            uc_unlink(tbuf) ;
	        } /* end if (mktmpfile) */
	        sigblocker_finish(&blocker) ;
	    } /* end if (sigblock) */
	} /* end if (mkpath) */

	return rs ;
}
/* end subroutine (shio_opentmp) */

int shio_close(SHIO *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != SHIO_MAGIC) return SR_NOTOPEN ;

	if (op->fl.nullfile) goto ret1 ;

#if	CF_SFIO
	if (op->outstore != nullptr) {
	    rs1 = shio_sfcookend(op) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (outstore) */
#endif /* CF_SFIO */

#if	CF_STOREFNAME
	if (op->fname != nullptr) {
	    rs1 = uc_free(op->fname) ;
	    if (rs >= 0) rs = rs1 ;
	    op->fname = nullptr ;
	}
#endif /* CF_STOREFNAME */

#if	CF_SFIO
	if (op->fl.sfio) {
	    int	rs1 = SR_BADF ;
	    if (op->fp != nullptr) {
#if	CF_SFSWAP
	        rs1 = sfclose(op->fp) ;
#else
	        rs1 = sfsync(op->fp) ;
#endif
	    }
	    rs = (rs1 >= 0) ? SR_OK : SR_BADF ;
	} else {
	    rs1 = shio_bclose(op) ;
	    if (rs >= 0) rs = rs1 ;
	}
#else
	rs1 = shio_bclose(op) ;
	if (rs >= 0) rs = rs1 ;
#endif /* CF_SFIO */

ret1:
	op->fp = nullptr ;
	op->magic = 0 ;
	return rs ;
}
/* end subroutine (shio_close) */

int shio_reade(SHIO *op,void *abuf,int alen,int to,int opts) noex {
	int		rs = SR_OK ;
	int		rlen = alen ;
	char		*rbuf = (char *) abuf ;

	if (op == nullptr) return SR_FAULT ;
	if (abuf == nullptr) return SR_FAULT ;

	if (op->magic != SHIO_MAGIC) return SR_NOTOPEN ;

	if (op->fl.nullfile) goto ret0 ;

	if (rlen == 0) goto ret0 ;

#if	CF_SFIO
	if (op->fl.sfio) {
	    Sfio_t	*fp = op->fp ;
	    Sfio_t	*streams[2] ;
	    time_t	ti_now = time(nullptr) ;
	    time_t	ti_start ;

	    rbuf[0] = '\0' ;
	    ti_start = ti_now ;
	    streams[0] = fp ;
	    streams[1] = nullptr ;
	    while (rs >= 0) {
	        if ((rs = sfpoll(streams,1,1000)) < 0) rs = SR_INTR ;
	        if (rs > 0) {
	            int v = sfvalue(fp) ;
	            if (v & SF_READ) {
	                if (op->fl.bufline || op->fl.bufnone) {
	                    char	*p ;
	                    p = sfgetr(op->fp,'\n',0) ;
	                    if (p != nullptr) {
	                        if ((v = sfvalue(fp)) < 0) rs = SR_HANGUP ;
	                        if (rs >= 0) {
	                            rs = snwcpy(rbuf,rlen,p,v) ;
				}
	                    } else {
	                        p = sfgetr(op->fp,'\n',SF_LASTR) ;
	                        if (p != nullptr) {
	                            if ((v = sfvalue(fp)) < 0) rs = SR_HANGUP ;
	                            if (rs >= 0)
	                                rs = snwcpy(rbuf,rlen,p,v) ;
	                        } else {
	                            rs = SR_OK ;
	                            break ;
	                        }
	                    }
	                } else {
	                    if ((rs = sfread(op->fp,rbuf,rlen)) < 0) {
	                        rs = SR_HANGUP ;
			    }
	                }
	                break ;
	            } else {
	                msleep(10) ;
		    }
	        } else if (rs < 0) {
	            break ;
		}
	        if (to >= 0) {
	            ti_now = time(nullptr) ;
	            if ((ti_now - ti_start) >= to) {
	                rs = SR_TIMEDOUT ;
	                break ;
	            }
	        }
	    } /* end while */
	} else {
	    rs = breade(op->fp,rbuf,rlen,to,opts) ;
	}
#else /* CF_SFIO */
	rs = breade(op->fp,rbuf,rlen,to,opts) ;

#endif /* CF_SFIO */

ret0:

	return rs ;
}
/* end subroutine (shio_reade) */

int shio_read(SHIO *op,void *ubuf,int ulen) noex {
	return shio_reade(op,ubuf,ulen,-1,0) ;
}
/* end subroutine (shio_read) */

int shio_readlinetimed(SHIO *op,char *lbuf,int llen,int to) noex {
	int		rs = SR_OK ;
	int		rl = 0 ;

	if (op == nullptr) return SR_FAULT ;
	if (lbuf == nullptr) return SR_FAULT ;

	if (op->magic != SHIO_MAGIC) return SR_NOTOPEN ;

	if (op->fl.nullfile) goto ret0 ;

#if	CF_SFIO
	if (op->fl.sfio) {
	    rl = sfreadlinetimed(op->fp,lbuf,llen,to) ;
	    rs = (rl >= 0) ? rl : SR_HANGUP ;
	} else {
	    rs = breadlnto(op->fp,lbuf,llen,to) ;
	    rl = rs ;
	}
#else /* CF_SFIO */
	rs = breadlnto(op->fp,lbuf,llen,to) ;
	rl = rs ;
#endif /* CF_SFIO */

ret0:
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (shio_readlinetimed) */

int shio_readline(SHIO *op,char *lbuf,int llen) noex {
	return shio_readlinetimed(op,lbuf,llen,-1) ;
}
/* end subroutine (shio_readline) */

int shio_readlines(SHIO *fp,char *lbuf,int llen,int *lcp) noex {
	int		rs = SR_OK ;
	int		i = 0 ;
	int		lines = 0 ;
	int		f_cont = false ;

	if (fp == nullptr) return SR_FAULT ;
	if (lbuf == nullptr) return SR_FAULT ;

	lbuf[0] = '\0' ;
	while ((lines == 0) || (f_cont = ISCONT(lbuf,i))) {

	    if (f_cont) i -= 2 ;

	    rs = shio_readline(fp,(lbuf + i),(llen-i)) ;
	    if (rs <= 0) break ;
	    i += rs ;

	    lines += 1 ;
	} /* end while */

	if (lcp != nullptr) *lcp = lines ;

	return (rs >= 0) ? i : rs ;
}
/* end subroutine (shio_readlines) */

int shio_write(SHIO *op,cvoid *lbuf,int llen) noex {
	int		rs = SR_OK ;
	int		wlen = 0 ;

	if (op == nullptr) return SR_FAULT ;
	if (lbuf == nullptr) return SR_FAULT ;

	if (op->magic != SHIO_MAGIC) return SR_NOTOPEN ;

	if (op->fl.nullfile) {
	    wlen = llen ;
	    goto ret0 ;
	}

#if	CF_SFIO
	if (op->fl.sfio) {
	    if (llen < 0) llen = lenstr(lbuf) ;
	    rs = shio_sfcheckwrite(op,lbuf,llen) ;
	    wlen = rs ;
	} else {
	    rs = bwrite(op->fp,lbuf,llen) ;
	    wlen = rs ;
#if	CF_FLUSHPART
	    if ((rs >= 0) && op->fl.bufline && hasnl(lbuf,llen)) {
	        rs = bflush(op->fp) ;
	    }
#endif
	}
#else /* CF_SFIO */
	rs = bwrite(op->fp,lbuf,llen) ;
	wlen = rs ;
#if	CF_FLUSHPART
	if ((rs >= 0) && op->fl.bufline && hasnl(lbuf,llen)) {
	    rs = bflush(op->fp) ;
	}
#endif
#endif /* CF_SFIO */

ret0:
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (shio_write) */

int shio_println(SHIO *op,cchar *lbuf,int llen) noex {
	int		rs = SR_OK ;
	int		wlen = 0 ;

	if (op == nullptr) return SR_FAULT ;
	if (lbuf == nullptr) return SR_FAULT ;

	if (op->magic != SHIO_MAGIC) return SR_NOTOPEN ;

	if (! op->fl.nullfile) {

#if	CF_SFIO
	if (op->fl.sfio) {
	    if (llen < 0) llen = lenstr(lbuf) ;
	    rs = shio_shprintln(op,lbuf,llen) ;
	    wlen += rs ;
	} else {
	    rs = bprintln(op->fp,lbuf,llen) ;
	    wlen += rs ;
	}
#else /* CF_SFIO */
	rs = bprintln(op->fp,lbuf,llen) ;
	wlen += rs ;
#endif /* CF_SFIO */

	} else {
	    wlen = llen ;
	}

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (shio_println) */

int shio_print(SHIO *op,cchar *lbuf,int llen) noex {
	return shio_println(op,lbuf,llen) ;
}
/* end subroutine (shio_print) */

int shio_printline(SHIO *op,cchar *lbuf,int llen) noex {
	return shio_println(op,lbuf,llen) ;
}
/* end subroutine (shio_printline) */

int shio_printf(SHIO *op,cchar *fmt,...) noex {
	int		rs ;
	{
	    va_list	ap ;
	    va_begin(ap,fmt) ;
	    rs = shio_vprintf(op,fmt,ap) ;
	    va_end(ap) ;
	}
	return rs ;
}
/* end subroutine (shio_printf) */

int shio_vprintf(SHIO *op,cchar *fmt,va_list ap) noex {
	cint	llen = LINEBUFLEN ;
	int		rs = SR_OK ;
	int		wlen = 0 ;
	char		lbuf[LINEBUFLEN + 1] ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != SHIO_MAGIC) return SR_NOTOPEN ;

	if (op->fl.nullfile) goto ret0 ;

	if ((rs = fmtstr(lbuf,llen,0,fmt,ap)) > 0) {
	    int	len = rs ;

#if	CF_SFIO
	    if (op->fl.sfio) {
	        rs = shio_sfcheckwrite(op,lbuf,len) ;
	        wlen = rs ;
	        if ((rs >= 0) && op->fl.bufline && hasnl(lbuf,len)) {
	            sfsync(op->fp) ;
	        }
	    } else {
	        if ((rs = bwrite(op->fp,lbuf,len)) >= 0) {
	            wlen = rs ;
	            if (op->fl.bufline && hasnl(lbuf,len)) {
	                rs = bflush(op->fp) ;
		    }
	        }
	    }
#else /* CF_SFIO */
	    if ((rs = bwrite(op->fp,lbuf,len)) >= 0) {
	        wlen = rs ;
	        if (op->fl.bufline && hasnl(lbuf,len)) {
	            rs = bflush(op->fp) ;
		}
	    }
#endif /* CF_SFIO */

	} /* end if */

ret0:
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (shio_vprintf) */

int shio_putc(SHIO *op,int ch) noex {
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != SHIO_MAGIC) return SR_NOTOPEN ;

	if (op->fl.nullfile) goto ret0 ;

#if	CF_SFIO
	if (op->fl.sfio) {
	    char	buf[2] ;
	    buf[0] = ch ;
	    rs = shio_sfcheckwrite(op,buf,1) ;
	} else {
	    rs = bputc(op->fp,ch) ;
	    if ((rs >= 0) && op->fl.bufline && (ch == '\n')) {
	        bflush(op->fp) ;
	    }
	}
#else /* CF_SFIO */
	if ((rs = bputc(op->fp,ch)) >= 0) {
	    if (op->fl.bufline && (ch == '\n')) {
	        bflush(op->fp) ;
	    }
	}
#endif /* CF_SFIO */

ret0:
	return (rs >= 0) ? 1 : rs ;
}
/* end subroutine (shio_putc) */

int shio_seek(SHIO *op,off_t o,int w) noex {
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != SHIO_MAGIC) return SR_NOTOPEN ;

	if (op->fl.nullfile) goto ret0 ;

#if	CF_SFIO
	if (op->fl.sfio) {
	    if ((rs = shio_sfcookflush(op)) >= 0) {
	        Sfoff_t	sfo ;
	        sfo = sfseek(op->fp,(Sfoff_t) o,w) ;
	        rs = (sfo >= 0) ? ((int) (sfo&INT_MAX)) : SR_NOTSEEK ;
	    }
	} else
	    rs = bseek(op->fp,o,w) ;
#else
	rs = bseek(op->fp,o,w) ;
#endif /* CF_SFIO */

ret0:
	return rs ;
}
/* end subroutine (shio_seek) */

int shio_flush(SHIO *op) noex {
	int		rs = SR_OK ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != SHIO_MAGIC) return SR_NOTOPEN ;

	if (! op->fl.nullfile) {

#if	CF_SFIO
	    if (op->fl.sfio) {
	        rs = shio_sfcookflush(op) ;
	    } else
	        rs = bflush(op->fp) ;
#else
	    rs = bflush(op->fp) ;
#endif

	} /* end if (output enabled) */

	return rs ;
}
/* end subroutine (shio_flush) */

int shio_control(SHIO *op,int cmd,...) noex {
	int		rs = SR_OK ;
	int		fd ;
	int		f ;

	if (op == nullptr) return SR_FAULT ;

	if (op->magic != SHIO_MAGIC) return SR_NOTOPEN ;

	{
	    va_list	ap ;
	    va_begin(ap,cmd) ;
	    switch (cmd) {
	    case SHIO_CNOP:
	        break ;
	    case SHIO_CSETBUFWHOLE:
	        if (! op->fl.nullfile) {
	            f = (int) va_arg(ap,int) ;
#if	CF_SFIO
	            if (op->fl.sfio) {
	                if ((rs = shio_sfcookline(op,false)) >= 0) {
	                    int	flags = SF_WHOLE ;
	                    int	sfcmd = (f) ? SFIO_ON : SFIO_OFF ;
	                    rs = sfset(op->fp,flags,sfcmd) ;
	                }
	            } else {
	                rs = bcontrol(op->fp,BC_SETBUFWHOLE,true) ;
		    }
#else
	            rs = bcontrol(op->fp,BC_SETBUFWHOLE,true) ;
#endif /* CF_SFIO */
	        }
	        break ;
	    case SHIO_CSETBUFLINE:
	        if (! op->fl.nullfile) {
	            f = (int) va_arg(ap,int) ;
	            op->fl.bufline = f ;
#if	CF_SFIO
	            if (op->fl.sfio) {
	                if ((rs = shio_sfcookline(op,f)) >= 0) {
	                    int	flags = SF_LINE ;
	                    int	sfcmd = (f) ? SFIO_ON : SFIO_OFF ;
	                    rs = sfset(op->fp,flags,sfcmd) ;
	                }
	            } else {
	                rs = bcontrol(op->fp,BC_SETBUFLINE,f) ;
		    }
#else
	            rs = bcontrol(op->fp,BC_SETBUFLINE,f) ;
#endif /* CF_SFIO */
	        }
	        break ;
	    case SHIO_CSETBUFNONE:
	        if (! op->fl.nullfile) {
	            op->fl.bufnone = true ;
	            op->fl.bufline = false ;
#if	CF_SFIO
	            if (op->fl.sfio) {
	                if ((rs = shio_sfcookline(op,false)) >= 0) {
	                    int	flags = (SF_LINE | SF_WHOLE) ;
	                    rs = sfset(op->fp,flags,SFIO_OFF) ;
	                }
	            } else {
	                rs = bcontrol(op->fp,BC_SETBUFNONE,true) ;
		    }
#else
	            rs = bcontrol(op->fp,BC_SETBUFNONE,true) ;
#endif /* CF_SFIO */
	        }
	        break ;
	    case SHIO_CSETBUFDEF:
	        if (! op->fl.nullfile) {
	            f = (int) va_arg(ap,int) ;
	            op->fl.bufline = op->fl.terminal ;
#if	CF_SFIO
	            if (op->fl.sfio) {
	                if ((rs = shio_sfcookline(op,true)) >= 0) {
	                    int	flags ;
	                    flags = SF_WHOLE ;
	                    if (! op->fl.terminal) flags |= SF_LINE ;
	                    int	sfcmd = (f) ? SFIO_ON : SFIO_OFF ;
	                    rs = sfset(op->fp,flags,sfcmd) ;
	                }
	            } else {
	                rs = bcontrol(op->fp,BC_SETBUFDEF,true) ;
		    }
#else
	            rs = bcontrol(op->fp,BC_LINEBUF,0) ;
#endif /* CF_SFIO */
	        }
	        break ;
	    case SHIO_CSETFLAGS:
	        if (! op->fl.nullfile) {
	            f = (int) va_arg(ap,int) ;
	            op->fl.bufline = f ;
#if	CF_SFIO
	            if (op->fl.sfio) {
	                int	flags = SF_LINE ;
	                int	sfcmd = (f) ? SFIO_ON : SFIO_OFF ;
	                rs = sfset(op->fp,flags,sfcmd) ;
	            } else {
	                rs = bcontrol(op->fp,BC_LINEBUF,0) ;
		    }
#else
	            rs = bcontrol(op->fp,BC_LINEBUF,0) ;
#endif /* CF_SFIO */
	        }
	        break ;
	    case SHIO_CFD:
	        if (! op->fl.nullfile) {
#if	CF_SFIO
	            if (op->fl.sfio) {
	                rs = sffileno(op->fp) ;
	                if (rs < 0)
	                    rs = SR_NOTOPEN ;
	            } else {
	                rs = bcontrol(op->fp,BC_FD,&fd) ;
	                if (rs >= 0)
	                    rs = fd ;
	            }
#else
	            if ((rs = bcontrol(op->fp,BC_FD,&fd)) >= 0) {
	                rs = fd ;
		    }
#endif /* CF_SFIO */
	            {
	                int	*aip = (int *) va_arg(ap,int *) ;
	                if (aip != nullptr) *aip = rs ;
	            }
	        }
	        break ;
	    case SHIO_CNONBLOCK:
	        if (! op->fl.nullfile) {
	            int	v = (int) va_arg(ap,int) ;
	            int	f ;
	            f = (v > 0) ;
#if	CF_SFIO
	            if (op->fl.sfio) {
	                rs = SR_OK ;
	            } else {
	                rs = bcontrol(op->fp,BC_NONBLOCK,f) ;
	            }
#else
	            rs = bcontrol(op->fp,BC_NONBLOCK,f) ;
#endif /* CF_SFIO */
	        }
	        break ;
	    case SHIO_CSTAT:
	        {
	            USTAT *sbp = (USTAT *) va_arg(ap,void *) ;
	            if (! op->fl.nullfile) {
#if	CF_SFIO
	                if (op->fl.sfio) {
	                    rs = SR_OK ;
	                } else {
	                    rs = bcontrol(op->fp,BC_STAT,sbp) ;
	                }
#else
	                rs = bcontrol(op->fp,BC_STAT,sbp) ;
#endif /* CF_SFIO */
	            } else {
	                memclear(sbp) ;
		    }
	        }
	        break ;
	    default:
	        rs = SR_INVALID ;
	        break ;
	    } /* end switch */
	    va_end(ap) ;
	} /* end block */

	return rs ;
}
/* end subroutine (shio_control) */

int shio_getfd(SHIO *op) noex {
	int		rs ;
	int		fd = -1 ;
	if ((rs = shio_magic(op,fname)) >= 0) {
	    if (! op->fl.nullfile) {
#if	CF_SFIO
	        if (op->fl.sfio) {
	            rs = sffileno(op->fp) ;
	            fd = rs ;
	            if (rs < 0) {
	                rs = SR_NOTOPEN ;
		    }
	        } else {
	            rs = bcontrol(op->fp,BC_FD,&fd) ;
	        }
#else /* CF_SFIO */
	        rs = bcontrol(op->fp,BC_FD,&fd) ;
#endif /* CF_SFIO */
	    } else {
	        rs = SR_NOSYS ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (shio_getfd) */

int shio_getlines(SHIO *op) noex {
	int		rs ;
	if ((rs = shio_isterm(op)) > 0) {
	   if ((rs = shio_getfd(op)) >= 0) {
	        rs = tcgetlines(rs) ;
	   }
	}
	return rs ;
}
/* end subroutine (shio_getlines) */

int shio_readintr(SHIO *op,void *ubuf,int ulen,int to,volatile int **ipp) noex {
	int		rs ;
	if (to < 0) to = INT_MAX ;
	if ((rs = shio_magic(op)) >= 0) {
	    if (! op->fl.nullfile) {
	        while (rs >= 0) {
	            cint	rto = (to >= 0) ? MIN(to,1) : to ;
	            if (isInterrupt(ipp)) break ;
	            rs = shio_reade(op,ubuf,ulen,rto,FM_TIMED) ;
	            if (rs != SR_TIMEDOUT) break ;
	            if (to > 0) to -= 1 ;
	            if (to == 0) break ;
	            rs = SR_OK ;
	        } /* end while */
	        if (rs >= 0) {
	            if (isInterrupt(ipp)) rs = SR_INTR ;
	        }
	    } /* end if (enabled) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (shio_readintr) */

int shio_reserve(SHIO *op,int amount) noex {
	int		rs ;
	if ((rs = shio_magic(op)) >= 0) {
#if	CF_SFIO
	    if (op->fl.sfio) {
	        rs = SR_OK ;
	    } else {
	        rs = breserve(op->fp,amount) ;
	    }
#else
	    rs = breserve(op->fp,amount) ;
#endif /* CF_SFIO */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (shio_reserve) */

int shio_isterm(SHIO *op) noex {
	int		rs ;
	if ((rs = shio_magic(op)) >= 0) {
	    if (! op->fl.nullfile) {
#if	CF_SFIO
	        if (op->fl.sfio) {
	            int	rc = sfisterm(op->fp) ;
	            rs = (rc >= 0) ? rc : SR_BADF ;
	        } else {
	            rs = bisterm(op->fp) ;
	        }
#else
	        rs = bisterm(op->fp) ;
#endif /* CF_SFIO */
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (shio_isterm) */

int shio_isseekable(SHIO *op) noex {
	int		rs ;
	int		f = false ;
	if ((rs = shio_magic(op)) >= 0) {
	    cint	w = SEEK_CUR ;
	    if ((rs = shio_seek(op,0L,w)) >= 0) {
	        f = true ;
	    } else if (isNotSeek(rs)) {
	        rs = SR_OK ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (shio_isseekable) */

int shio_stat(SHIO *op,USTAT *sbp) noex {
	int		rs = SR_OK ;
	if ((rs = shio_magic(op,sbp)) >= 0) {
	   rs = shio_control(op,SHIO_CSTAT,sbp) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (shio_stat) */

int shio_writefile(SHIO *op,cchar *fname) noex {
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	if ((rs = shio_magic(op,fname)) >= 0) {
	    linebuffer	lb ;
	    if ((rs = lb.start) >= 0) {
	        bfile	ifile, *ifp = &ifile ;
	        cmode	om = 0664 ;
	        if ((rs = bopen(ifp,fname,"r",om)) >= 0) {
	            while ((rs = bread(ifp,lb.lbuf,lb.llen)) > 0) {
	                rs = shio_write(op,lb.lbuf,rs) ;
	                wlen += rs ;
	                if (rs < 0) break ;
	            } /* end while */
	            rs1 = bclose(ifp) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (bopen) */
	        rs1 = lb.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (linebuffer) */
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (shio_writefile) */


/* private subroutines */

static int shio_bopene(SHIO *op,int fni,cc *fname,cc *ms,
		mode_t om,int to) noex {
	cint		osz = szof(bfile) ;
	int		rs ;
	char		nms[SHIO_MODESTRLEN + 1] ;
	void		*vp ;
	if ((rs = uc_malloc(osz,&vp)) >= 0) {
	    op->bfp = bfilep(vp) ;
	    op->fp = vp ;
	    if (fni >= 0) {
	        if ((rs = sncpy2(nms,SHIO_MODESTRLEN,"d",ms)) >= 0) {
	            rs = bopene(op->fp,fname,nms,om,to) ;
	        }
	    } else {
	        rs = bopene(op->fp,fname,ms,om,to) ;
	    }
	    if (rs < 0) {
	        uc_free(op->bfp) ;
	        op->bfp = nullptr ;
	        op->fp = nullptr ;
	    }
	} /* end if (memory-allocation) */
	return rs ;
}
/* end subroutine (shio_bopene) */

static int shio_bclose(SHIO *op) noex {
	int		rs = SR_OK ;
	if (op->fp != nullptr) {
	    rs = bclose(op->fp) ;
	    if (op->bfp != nullptr) {
	        uc_free(op->bfp) ;
	        op->bfp = nullptr ;
	    }
	}
	return rs ;
}
/* end subroutine (shio_bclose) */

#if	CF_SFIO
static int shio_shprintln(SHIO *op,cchar *lbuf,int llen) noex {
	int		rs ;
	int		wlen = 0 ;
	if (llen < 0) llen = lenstr(lbuf) ;
	if ((rs = shio_sfiscook(op)) > 0) {
	    if ((rs = shio_sfcookwrite(op,lbuf,llen)) >= 0) {
	        wlen += rs ;
	        if ((llen == 0) || (lbuf[llen-1] != '\n')) {
		    rs = shio_sfcookwrite(op,"\n",1) ;
	            wlen += rs ;
		}
	    }
	} else if (rs == 0) {
	    if (llen > 0) {
	        rs = shio_sfwrite(op,lbuf,llen) ;
	        wlen += rs ;
	    }
	    if ((rs >= 0) && ((llen == 0) || (lbuf[llen-1] != '\n'))) {
	        char	eol[2] ;
	        eol[0] = '\n' ;
	        eol[1] = '\0' ;
	        shio_sfwrite(op,eol,1) ;
	        wlen += rs ;
	    }
	}
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (shio_shprintln) */
#endif /* CF_SFIO */

#if	CF_SFIO

static int shio_sfiscook(SHIO *op) noex {
	int		rs = SR_OK ;
	int		f = false ;
	f = f || op->fl.terminal ;
	f = f || op->fl.bufline ;
	f = f || op->fl.bufnone ;
	if (f) {
	    rs = shio_sfcookline(op,f) ;
	}
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (shio_sfiscook) */

static int shio_sfflush(SHIO *op) noex {
	sfsync(op->fp) ;
	return SR_OK ;
}
/* end subroutine (shio_sfflush) */

static int shio_sfcookline(SHIO *op,int f) noex {
	int		rs ;
	if (f) {
	    rs = shio_sfcookbegin(op) ;
	} else {
	    rs = shio_sfcookend(op) ;
	}
	return rs ;
}
/* end subroutine (shio_sfcookline) */

static int shio_sfcookbegin(SHIO *op) noex {
	int		rs = SR_OK ;
	if (op->outstore == nullptr) {
	    cint	size = szof(OUTSTORE) ;
	    void	*p ;
	    if ((rs = uc_malloc(size,&p)) >= 0) {
	        OUTSTORE	*osp = (OUTSTORE *) p ;
	        op->outstore = p ;
	        rs = outstore_start(osp) ;
	        if (rs < 0) {
	            uc_free(op->outstore) ;
	            op->outstore = nullptr ;
	        }
	    } /* end if (m-a) */
	} /* end if (needed) */
	return rs ;
}
/* end subroutine (shio_sfcookbegin) */

static int shio_sfcookend(SHIO *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->outstore != nullptr) {
	    OUTSTORE	*osp = op->outstore ;
	    {
	        rs1 = shio_sfcookdump(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = outstore_finish(osp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs = uc_free(op->outstore) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->outstore = nullptr ;
	}
	return rs ;
}
/* end subroutine (shio_sfcookend) */

static int shio_sfcookdump(SHIO *op) noex {
	int		rs = SR_OK ;
	if (op->outstore != nullptr) {
	    OUTSTORE	*osp = (OUTSTORE *) op->outstore ;
	    cchar	*cp ;
	    if ((rs = outstore_get(osp,&cp)) > 0) {
	        if ((rs = shio_sfwrite(op,cp,rs)) >= 0) {
	            rs = outstore_clear(osp) ;
	        }
	    }
	}
	return rs ;
}
/* end subroutine (shio_sfcookflush) */

static int shio_sfcookflush(SHIO *op) noex {
	int		rs = SR_OK ;
	if (op->outstore != nullptr) {
	    if ((rs = shio_sfcookdump(op)) >= 0) {
	        rs = shio_sfflush(op) ;
	    }
	} else {
	    rs = shio_sfflush(op) ;
	}
	return rs ;
}
/* end subroutine (shio_sfcookflush) */

static int shio_sfcheckwrite(SHIO *op,cchar *lbuf,int llen) noex {
	int		rs ;
	if (llen < 0) llen = lenstr(lbuf) ;
	if ((rs = shio_sfiscook(op)) > 0) {
	    rs = shio_sfcookwrite(op,lbuf,llen) ;
	} else if (rs == 0) {
	    rs = shio_sfwrite(op,lbuf,llen) ;
	}
	return (rs >= 0) ? llen : rs ;
}
/* end subroutine (shio_sfcheckwrite) */

static int shio_sfcookwrite(SHIO *op,cchar *lbuf,int llen) noex {
	int		rs = SR_OK ;
	if ((op->outstore != nullptr) && (llen > 0)) {
	    OUTSTORE	*osp = (OUTSTORE *) op->outstore ;
	    int		ll = llen ;
	    cchar	*tp ;
	    cchar	*lp = lbuf ;
	    if ((tp = strnchr(lp,ll,CH_NL)) != nullptr) {
	        if ((rs = outstore_strw(osp,lp,((tp+1)-lp))) >= 0) {
	            cchar	*cp ;
	            if ((rs = outstore_get(osp,&cp)) > 0) {
	                if ((rs = shio_sfwrite(op,cp,rs)) >= 0) {
	                    rs = outstore_clear(osp) ;
	                }
	            }
	            ll -= ((tp+1)-lp) ;
	            lp = (tp+1) ;
	            if ((rs >= 0) && (ll > 0)) {
	                while ((tp = strnchr(lp,ll,CH_NL)) != nullptr) {
	                    rs = shio_sfwrite(op,lp,(tp+1-lp)) ;
	                    ll -= ((tp+1)-lp) ;
	                    lp = (tp+1) ;
	                    if (rs < 0) break ;
	                } /* end while */
	            } /* end if (handle remaining) */
		    if ((rs >= 0) && op->fl.bufline) {
	        	rs = shio_sfflush(op) ;
		    }
	        } /* end if (outstore_strw) */
	    } /* end if (had a NL) */
	    if ((rs >= 0) && (ll > 0)) {
	        rs = outstore_strw(osp,lp,ll) ;
	    }
	} else if (llen > 0) {
	    rs = shio_sfwrite(op,lbuf,llen) ;
	}
	return (rs >= 0) ? llen : rs ;
}
/* end subroutine (shio_sfcookwrite) */
#endif /* CF_SFIO */

#if	CF_SFIO
static int shio_sfwrite(SHIO *op,cchar *lbuf,int llen) noex {
	int		rs ;
	if ((rs = sfwrite(op->fp,lbuf,llen)) < 0) rs = SR_PIPE ;
	return rs ;
}
/* end subroutine (shio_sfwrite) */
#endif /* CF_SFIO */

#if	CF_SFIO
/* are we outside of KSH? */
static int shio_outside() noex {
	SHIO_PERSISTENT	*pdp = &shio_data ;
	int		f = pdp->f_outside ;
	if (! pdp->f_init) { /* race is OK here */
	    if (dlsym(RTLD_DEFAULT,SHIO_KSHSYM) == nullptr) {
	        pdp->f_outside = true ;
	        f = true ;
	    }
	    pdp->f_init = true ;
	} /* end if (needed initialization) */
	return f ;
}
/* end subroutine (shio_outside) */
#endif /* CF_SFIO */

static bool hasnl(cchar *sp,int sl) noex {
	return (strnrchr(sp,sl,'\n') != nullptr) ;
}
/* end subroutine (hasnl) */

static int isNotSeek(int rs) noex {
	return isOneOf(seekrs,rs) ;
}
/* end subroutine (isNotSeek) */

static bool isInterrupt(volatile int **ipp) noex {
	bool		f = false ;
	if (ipp != nullptr) {
	    int	i = 0 ;
	    for (i = 0 ; ipp[i] != nullptr ; i += 1) {
	        f = (ipp[i][0] != 0) ;
	        if (f) break ;
	    }
	}
	return f ;
}
/* end subroutine (isInterrupt) */


