/* termnote SUPPORT */
/* lang=C++20 */

/* perform terminal noticing */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This object module was originally written to create a logging
	mechanism for PCS application programs.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This is a module that operates on termials (to be determined)
	for the purposes of putting out small messages to them.
	Subroutines in this module are:

		termnote_open
		termnote_printf
		termnote_vprintf
		termnote_write
		termnote_close

	Implementation note:

	Yes, this can be written so that it performs in a more
	efficient manner.  But this thing is quite seldom used right
	now (a-days). If there is ever a real need, yes, we can
	certaintly speed this up. So it is no big deal right now.

	BUG NOTE:

	Note that the (stupid) Solaris® version of the standard
	UTMPX interface does not work properly for some (unknown)
	strange reason. This bug is quite annoying since some
	terminals are not biffed as they should be due to the bug.
	The bug somehow causes an enumeration of the UTMPX database
	file to skip over some UTMPX records. Skipped records might
	just contain a terminal that needs to be biffed. To work
	around the Solaris® bug, we use a separate UTMPX database
	interface entirely (TMPX). That interface is not too
	dissimilar to the standard interface, except in the case
	of Solaris®, it works properly!

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdarg>
#include	<cstring>		/* |strlen(3c)| */
#include	<usystem.h>
#include	<getbufsize.h>
#include	<mallocxx.h>
#include	<getnodename.h>
#include	<getusername.h>
#include	<ascii.h>
#include	<ids.h>
#include	<buffer.h>
#include	<vecstr.h>
#include	<vecobj.h>
#include	<linefold.h>
#include	<tmpx.h>
#include	<logfile.h>
#include	<strn.h>
#include	<sfx.h>
#include	<snx.h>
#include	<mkx.h>
#include	<sncpyx.h>
#include	<mkpathx.h>
#include	<termconseq.h>
#include	<ncol.h>		/* |charcols(3uc)| */
#include	<ipow.h>
#include	<xperm.h>
#include	<mkchar.h>
#include	<ischarx.h>
#include	<isnot.h>
#include	<localmisc.h>		/* |NTABCOLS| */

#include	"termnote.h"


/* local defines */

#define	TERMNOTE_SEARCHNAME	"termnote"
#define	TERMNOTE_DEBFNAME	"termnote.deb"

#ifndef	DEVDNAME
#define	DEVDNAME	"/dev"
#endif

#define	LOGDNAME	"log"

#ifndef	COLUMNS
#define	COLUMNS		80
#endif

#ifndef	TERMDEVLEN
#define	TERMDEVLEN	80
#endif

#define	TO_TMPX		(15 * 60)
#define	TO_CHECK	5
#define	TO_WRITE	30
#define	TO_LOGCHECK	5
#define	TO_LOCK		4

#define	USERTERM	struct userterm

#define	COLSTATE	struct colstate

#ifndef	TIMEBUFLEN
#define	TIMEBUFLEN	80
#endif


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef buffer		mbuf ;


/* external subroutines */

extern "C" {
    extern int	opentmpfile(cchar *,int,mode_t,char *) noex ;
    extern int	vbufprintf(char *,int,cchar *,va_list) noex ;
    extern int	writeto(int,cvoid *,int,int) noex ;
    extern int	tmpx_getuserlines(tmpx *,vecstr *,cchar *) noex ;
    static int	vcmpatime(cvoid *,cvoid *) noex ;
    extern char	*timestr_logz(time_t,char *) noex ;
}


/* external variables */


/* local structures */

struct userterm {
	time_t		atime ;
	char		termdev[MAXNAMELEN+1] ;
} ;

typedef userterm *	usertermp ;

struct colstate {
	int		ncols ;
	int		ncol ;
} ;


/* forward references */

template<typename ... Args>
static int termnote_ctor(termnote *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    memclear(op) ;		/* dangerous */
	    if ((op->idp = new(nothrow) ids) != np) {
	        if ((op->txp = new(nothrow) tmpx) != np) {
	            if ((op->lfp = new(nothrow) logfile) != np) {
			rs = SR_OK ;
		    } /* end if (new-logfile) */
		    if (rs < 0) {
		        delete op->txp ;
		        op->txp = nullptr ;
		    }
		} /* end if (new-tmpx) */
		if (rs < 0) {
		    delete op->idp ;
		    op->idp = nullptr ;
		}
	    } /* end if (new-ids) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (termnote_ctor) */

static int termnote_dtor(termnote *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (termnote_dtor) */

template<typename ... Args>
static int termnote_magic(termnote *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == TERMNOTE_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (termnote_magic) */


static int termnote_writer(termnote *,cchar **,int,int,cchar *,int) noex ;

static int termnote_txopen(termnote *,time_t) noex ;
static int termnote_txclose(termnote *) noex ;
static int termnote_lfopen(termnote *,time_t) noex ;
static int termnote_lfclose(termnote *) noex ;

static int termnote_bufline(termnote *,buffer *,cchar *,int) noex ;
static int termnote_bufextra(termnote *,buffer *,int) noex ;
static int termnote_dis(termnote *,cchar **,int,int,mbuf *) noex ;
static int termnote_disuser(termnote *,int,int,mbuf *,cchar *) noex ;
static int termnote_diswrite(termnote *,int,mbuf *,cchar *) noex ;

static int termnote_username(termnote *) noex ;
static int termnote_nodename(termnote *) noex ;

#ifdef	COMMENT
static int	colstate_load(struct colstate *,int,int) noex ;
static int	colstate_linecols(struct colstate *,cchar *,int) noex ;
#endif

static int	mkclean(char *,int,cchar *,int) noex ;

static bool	hasourbad(cchar *,int) noex ;
static bool	isourbad(int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int termnote_open(termnote *op,cchar *pr) noex {
	const time_t	dt = time(nullptr) ;
	int		rs ;
	if ((rs = termnote_ctor(op,pr)) >= 0) {
	    rs = SR_INVALID ;
	    if (pr[0]) {
	        cchar	*cp{} ;
	        if ((rs = uc_mallocstrw(pr,-1,&cp)) >= 0) {
	            op->pr = cp ;
	            if ((rs = termnote_txopen(op,dt)) >= 0) {
	                if ((rs = ids_load(op->idp)) >= 0) {
	    	            if ((rs = termnote_lfopen(op,dt)) >= 0) {
	                        op->magic = TERMNOTE_MAGIC ;
	                    }
	                    if (rs < 0) {
		                ids_release(op->idp) ;
		            }
	                } /* end if (ids_load) */
	                if (rs < 0) {
		            termnote_txclose(op) ;
		        }
	            } /* end if (termnote_txopen) */
	            if (rs < 0) {
	                uc_free(op->pr) ;
	                op->pr = nullptr ;
	            }
	        } /* end if (m-a) */
	    } /* end if (valid) */
	    if (rs < 0) {
		termnote_dtor(op) ;
	    }
	} /* end if (termnote_ctor) */
	return rs ;
}
/* end subroutine (termnote_open) */

int termnote_close(termnote *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = termnote_magic(op)) >= 0) {
	    if (op->open.lf) {
	        const time_t	dt = time(nullptr) ;
	        char		tbuf[TIMEBUFLEN+1] ;
	        timestr_logz(dt,tbuf) ;
	        logfile_printf(op->lfp,"%s done",tbuf) ;
	    }
	    {
	        rs1 = termnote_lfclose(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->username) {
	        rs1 = uc_free(op->username) ;
	        if (rs >= 0) rs = rs1 ;
	        op->username = nullptr ;
	    }
	    if (op->nodename) {
	        rs1 = uc_free(op->nodename) ;
	        if (rs >= 0) rs = rs1 ;
	        op->nodename = nullptr ;
	    }
	    {
	        rs1 = ids_release(op->idp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = termnote_txclose(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->pr) {
	        rs1 = uc_free(op->pr) ;
	        if (rs >= 0) rs = rs1 ;
	        op->pr = nullptr ;
	    }
	    {
		rs1 = termnote_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (termnote_close) */

int termnote_printf(termnote *op,cc **rpp,int n,int o,cc *fmt,...) noex {
	int		rs ;
	if ((rs = termnote_magic(op,rpp,fmt)) >= 0) {
	    va_list	ap ;
	    va_begin(ap,fmt) ;
	    rs = termnote_vprintf(op,rpp,n,o,fmt,ap) ;
	    va_end(ap) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (termnote_printf) */

/* make a log entry */
int termnote_vprintf(termnote *op,cchar **rpp,int n,int o,
		cchar *fmt,va_list ap) noex {
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	if ((rs = termnote_magic(op,rpp,fmt)) >= 0) {
	    cint	olen = TERMNOTE_BUFSIZE ;
	    char	*obuf{} ;
	    if ((rs = uc_malloc((olen+1),&obuf)) >= 0) {
	        if ((rs = vbufprintf(obuf,olen,fmt,ap)) >= 0) {
	            rs = termnote_write(op,rpp,n,o,obuf,rs) ;
	            wlen = rs ;
	        }
	        rs1 = uc_free(obuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (magic) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (termnote_vprintf) */

int termnote_check(termnote *op,time_t dt) noex {
	int		rs ;
	if ((rs = termnote_magic(op)) >= 0) {
	    if (dt == 0) dt = time(nullptr) ;
	    if ((dt - op->ti_check) >= TO_CHECK) {
	        op->ti_check = dt ;
	        if ((rs >= 0) && op->open.lf) {
	            if ((dt - op->ti_logcheck) >= TO_LOGCHECK) {
		        op->ti_logcheck = dt ;
	                rs = logfile_check(op->lfp,dt) ;
		    }
	        }
	        if ((rs >= 0) && op->open.tx) {
	            if ((dt - op->ti_tmpx) >= TO_TMPX) {
	                op->open.tx = false ;
	                rs = tmpx_close(op->txp) ;
	            } else {
	                rs = tmpx_check(op->txp,dt) ;
		    }
	        } /* end if (tmpx) */
	    } /* end if (check) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (termnote_check) */

int termnote_write(termnote *op,cc **rpp,int m,int o,cc *sbuf,int slen) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	if ((rs = termnote_magic(op,rpp,sbuf)) >= 0) {
	    if (rpp[0] != nullptr) {
	        time_t		dt = 0 ;
	        if (m < 1) m = INT_MAX ;
	        if ((rs >= 0) && op->open.lf) {
	            char	sublogid[LOGIDLEN+1] ;
	            char	tbuf[TIMEBUFLEN+1] ;
	            if ((rs = snsdd(sublogid,LOGIDLEN,op->logid,op->sn)) >= 0) {
		        if ((rs = logfile_setid(op->lfp,sublogid)) >= 0) {
		            cbool	fbel = ((o & TERMNOTE_OBELL)?1:0) ;
		            cbool	fbiff = ((o & TERMNOTE_OBIFF)?1:0) ;
		            cbool	fall = ((o & TERMNOTE_OALL)?1:0) ;
		            cchar	*fmt ;
		            if (dt == 0) dt = time(nullptr) ;
		            timestr_logz(dt,tbuf),
		            fmt = "%s bell=%u biff=%u all=%u" ;
	    	            logfile_printf(op->lfp,fmt,tbuf,fbel,fbiff,fall) ;
		        }
	            }
	        } /* end if (logging) */
	        if (slen < 0) slen = strlen(sbuf) ;
	        if ((rs >= 0) && (slen > 0)) {
        /* ok, now we really go */
	            rs = termnote_writer(op,rpp,m,o,sbuf,slen) ;
	            c = rs ;
	            if ((rs >= 0) && op->open.lf) {
	                int	n = (LOGIDLEN - 1 - strlen(op->logid)) ;
	                int	m ;
	                m = (n < 10) ? ipow(10,n) : INT_MAX ;
	                op->sn = ((op->sn + 1) % m) ;
	                rs = logfile_setid(op->lfp,op->logid) ;
	            }
	            if (rs >= 0) {
	                if (dt == 0) dt = time(nullptr) ;
	                op->ti_write = dt ;
	            }
	        } /* end if (positive) */
	    } /* end if (not-empty) */
	} /* end if (magic) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (termnote_write) */


/* private subroutines */

static int termnote_writer(termnote *op,cchar **rpp,int m,int o,
		cchar *sp,int sl) noex {
	buffer		ob ;
	cint		bsize = (sl + 40) ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = buffer_start(&ob,bsize)) >= 0) {
	    cint	maxlines = TERMNOTE_MAXLINES ;
	    int		lines = 0 ;
	    cchar	*tp ;
	    while ((tp = strnchr(sp,sl,'\n')) != nullptr) {
		if (lines < maxlines) {
	            rs = termnote_bufline(op,&ob,sp,(tp - sp)) ;
		    lines += 1 ;
		}
	        sl -= ((tp + 1) - sp) ;
	        sp = (tp + 1) ;
	        if (rs < 0) break ;
	    } /* end while */
	    if ((rs >= 0) && (sl > 0)) {
		if (lines < maxlines) {
	             rs = termnote_bufline(op,&ob,sp,sl) ;
		     lines += 1 ;
		}
	    }
	    if (rs >= 0) {
		rs = termnote_bufextra(op,&ob,o) ;
	    }
	    if (rs >= 0) {
	        cchar	*bp{} ;
	        if ((rs = buffer_get(&ob,&bp)) >= 0) {
	            cint	bl = rs ;
		    if (op->open.lf) {
			cchar	*fmt = "note lines=%u len=%u" ;
			logfile_printf(op->lfp,fmt,lines,bl) ;
		    }
		    rs = termnote_dis(op,rpp,m,o,&ob) ;
		    c = rs ;
		} /* end if (get) */
	    } /* end if */
	    rs1 = buffer_finish(&ob) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (buffer) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (termnote_writer) */

static int termnote_bufline(termnote *op,buffer *obp,cchar *lp,int ll) noex {
	cint		cols = COLUMNS ;
	cint		tmplen = COLUMNS ;
	int		rs = SR_OK ;
	int		rs1 ;
	if (op) {
	    if (ll < 0) ll = strlen(lp) ;
	    if (lp[ll-1] == '\n') ll -= 1 ;
	    if (ll > 0) {
	        linefold	lf ;
	        if ((rs = linefold_start(&lf,cols,1,lp,ll)) >= 0) {
		    auto	lg = linefold_get ;
		    int	cl ;
		    cchar	*cp ;
	            char	tmpbuf[COLUMNS+ 1] ;
	            for (int i = 0 ; (cl = lg(&lf,i,&cp)) >= 0 ; i += 1) {
	                cchar	*bp = cp ;
	                int		bl = cl ;
	                if (hasourbad(cp,cl)) {
	                    bp = tmpbuf ;
	                    bl = mkclean(tmpbuf,tmplen,cp,cl) ;
	                }
	                if ((rs = buffer_char(obp,'\r')) >= 0) {
	                    rs = buffer_strw(obp,bp,bl) ;
		        }
		        if (rs >= 0) {
		             int	tl ;
		             rs = termconseq(tmpbuf,(tmplen-2),'K') ;
		             tl = rs ;
		             if (rs >= 0) {
			        tmpbuf[tl++] = '\r' ;
			        tmpbuf[tl++] = '\n' ;
	        	        rs = buffer_strw(obp,tmpbuf,tl) ;
		             }
		        }
	                if (rs < 0) break ;
	            } /* end for (getting folded line parts) */
	            rs1 = linefold_finish(&lf) ;
		    if (rs >= 0) rs = rs1 ;
	        } /* end if (linefold) */
	    } /* end if (non-zero) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (termnote_bufline) */

static int termnote_bufextra(termnote *op,buffer *obp,int o) noex {
	int		rs = SR_FAULT ;
	if (op && obp) {
	    rs = SR_OK ;
	    if (o & TERMNOTE_OBELL) {
	        rs = buffer_char(obp,CH_BEL) ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (termnote_bufextra) */

static int termnote_dis(termnote *op,cchar **rpp,int n,int o,mbuf *mp) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	for (int i = 0 ; (rs >= 0) && rpp[i] ; i += 1) {
	    rs = termnote_disuser(op,n,o,mp,rpp[i]) ;
	    c += rs ;
	} /* end for */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (termnote_dis) */

static int termnote_disuser(termnote *op,int nmax,int o,mbuf *mp,cc *un) noex {
	vecobj		uts ;
	cint		utsize = sizeof(USERTERM) ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = vecobj_start(&uts,utsize,0,0)) >= 0) {
	    vecstr	lines ;
	    cint	tdlen = TERMDEVLEN ;
	    cchar	*devdname = DEVDNAME ;
	    char	termfname[TERMDEVLEN+1] ;
	    if ((rs = vecstr_start(&lines,0,0)) >= 0) {
	        int	nlines ;
		rs = tmpx_getuserlines(op->txp,&lines,un) ;
		nlines = rs ;
		if (op->open.lf) {
		    cchar	*lfmt = "u=%s termlines=%u" ;
		    logfile_printf(op->lfp,lfmt,un,nlines) ;
		}
	    if ((rs >= 0) && (nlines > 0)) {
		USERTERM	ut ;
		int		ll, tl ;
		cchar		*lp{} ;
		for (int i = 0 ; vecstr_get(&lines,i,&lp) >= 0 ; i += 1) {
		    if (lp == nullptr) continue ;
		    ll = strlen(lp) ;
		    if (ll > 0) {
			if ((rs = mkpath2w(termfname,devdname,lp,ll)) >= 0) {
			    tl = rs ;
			    rs1 = sncpy1w(ut.termdev,tdlen,termfname,tl) ;
			    if (rs1 >= 0) {
				USTAT	sb ;
				bool	f_go = false ;
				rs1 = u_stat(ut.termdev,&sb) ;
				if (rs1 >= 0) {
				    f_go = (sb.st_mode & S_IWGRP) ;
				    if (f_go && (o & TERMNOTE_OBIFF))
				        f_go = (sb.st_mode & S_IXUSR) ;
				}
				if ((rs1 >= 0) && f_go) {
				    rs1 = sperm(op->idp,&sb,W_OK) ;
				    if (rs1 >= 0) {
					ut.atime = sb.st_atime ;
					rs = vecobj_add(&uts,&ut) ;
				    }
				}
			    }
			} /* end if (mkpath) */
		    } /* end if (positive) */
		    if (rs < 0) break ;
		} /* end for */
	    } /* end if */
		rs1 = vecstr_finish(&lines) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (user-term lines) */
	    if (rs >= 0) {
		int		navail ;
		int		n = 0 ;
		cchar		*lfmt = "  %s (%d)" ;
		{
		    vecobj_vcf	vcf = vecobj_vcf(vcmpatime) ;
		    navail = vecobj_sort(&uts,vcf) ;
		}
		if (op->open.lf) {
		    logfile_printf(op->lfp,"  avail=%u",navail) ;
		}
		void		*vp{} ;
		cchar		*tdp ;
		for (int i = 0 ; vecobj_get(&uts,i,&vp) >= 0 ; i += 1) {
		    if (vp) {
	                USERTERM	*utp = usertermp(vp) ;
			int		rsv = 0 ;
			tdp = utp->termdev ;
		        if ((rs = termnote_diswrite(op,o,mp,tdp)) >= 0) {
			    n += 1 ;
			    c += 2 ;
			    rsv = rs ;
		        } else if (isNotPresent(rs)) {
			    rsv = rs ;
			    rs = SR_OK ;
			} else {
			    rsv = rs ;
			}
		        if (op->open.lf) {
		            logfile_printf(op->lfp,lfmt,(tdp+5),rsv) ;
		        }
		        if (n >= nmax) break ;
		    } /* end if (non-null) */
		    if (rs < 0) break ;
		} /* end for (looping through user-terms) */
	    } /* end if (ok) */
	    rs1 = vecobj_finish(&uts) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (user-term list) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (termnote_disuser) */

static int termnote_diswrite(termnote *op,int o,mbuf *mp,cc *termdev) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (op && mp && termdev) {
	    cint	of = (O_WRONLY | O_NOCTTY | O_NDELAY) ;
	    cint	to = 5 ;
	    (void) o ;
	    if ((rs = u_open(termdev,of,0666)) >= 0) {
	        cint	fd = rs ;
		cchar	*bp{} ;
		if ((rs = buffer_get(mp,&bp)) >= 0) {
	            rs = writeto(fd,bp,rs,to) ;
	            len = rs ;
		}
	        rs1 = u_close(fd) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (open terminal-device) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (termnote_diswrite) */

static int termnote_txopen(termnote *op,time_t dt) noex {
	int		rs = SR_OK ;
	if (! op->open.tx) {
	    if (dt == 0) dt = time(nullptr) ;
	    rs = tmpx_open(op->txp,nullptr,0) ;
	    op->open.tx = (rs >= 0) ;
	    if (rs >= 0) op->ti_tmpx = dt ;
	}
	return rs ;
}
/* end subroutine (termnote_txopen) */

static int termnote_txclose(termnote *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->open.tx) {
	    op->open.tx = false ;
	    rs1 = tmpx_close(op->txp) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (termnote_txclose) */

static int termnote_lfopen(termnote *op,time_t dt) noex {
	cint		of = O_RDWR ;
	cmode		om = 0666 ;
	int		rs = SR_OK ;
	int		f_opened = false ;
	if (! op->init.lf) {
	    cchar	*sn = TERMNOTE_SEARCHNAME ;
	    char	lfname[MAXPATHLEN+1] ;
	    op->init.lf = true ;
	    if (rs >= 0) {
		rs = termnote_nodename(op) ;
	    }
	    if (rs >= 0) {
		const pid_t	pid = uc_getpid() ;
		rs = mkplogid(op->logid,LOGIDLEN,op->nodename,pid) ;
	    }
	    if (rs >= 0) {
	        rs = mkpath3(lfname,op->pr,LOGDNAME,sn) ;
	    }
	    if (rs >= 0) {
		LOGFILE	*lfp = op->lfp ;
	        if ((rs = logfile_open(lfp,lfname,of,om,op->logid)) >= 0) {
		    f_opened = true ;
	            op->open.lf = true ;
		    if (rs >= 0) {
		        rs = logfile_checksize(op->lfp,TERMNOTE_LOGSIZE) ;
		    }
		    if (rs >= 0) {
		        rs = termnote_username(op) ;
		    }
		    if (rs >= 0) {
			cchar	*nn = op->nodename ;
			cchar	*un = op->username ;
			char	timebuf[TIMEBUFLEN+1] ;
	        	if (dt == 0) dt = time(nullptr) ;
			timestr_logz(dt,timebuf) ;
			logfile_printf(op->lfp,"%s %s",timebuf,sn) ;
			rs = logfile_printf(op->lfp,"%s!%s",nn,un) ;
		    }
		} else if (isNotPresent(rs)) {
		    rs = SR_OK ;
		} /* end if (logfile opened) */
	    } /* end if (ok) */
	} /* end if (needed initialization) */
	return (rs >= 0) ? f_opened : rs ;
}
/* end subroutine (termnote_lfopen) */

static int termnote_lfclose(termnote *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->open.lf) {
	    op->open.lf = false ;
	    rs1 = logfile_close(op->lfp) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (termnote_lfclose) */

static int termnote_username(termnote *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		rl = 0 ;
	if (op->username == nullptr) {
	    char	*ubuf{} ;
	    if ((rs = malloc_un(&ubuf)) >= 0) {
		cint	ulen = rs ;
	        if ((rs = getusername(ubuf,ulen,-1)) >= 0) {
		    cchar	*cp{} ;
	            rl = rs ;
		    if ((rs = uc_mallocstrw(ubuf,rl,&cp)) >= 0) {
		        op->username = cp ;
		    }
	        } /* end if (getnodename) */
		rs1 = uc_free(ubuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} else {
	   rl = strlen(op->username) ;
	}
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (termnote_username) */

static int termnote_nodename(termnote *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		rl = 0 ;
	if (op->nodename == nullptr) {
	    char	*nbuf{} ;
	    if ((rs = malloc_nn(&nbuf)) >= 0) {
		cint	nlen = rs ;
	        if ((rs = getnodename(nbuf,nlen)) >= 0) {
		    cchar	*cp{} ;
	            rl = rs ;
		    if ((rs = uc_mallocstrw(nbuf,rl,&cp)) >= 0) {
		        op->nodename = cp ;
		    }
	        } /* end if (getnodename) */
		rs1 = uc_free(nbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} else {
	    rl = strlen(op->nodename) ;
	}
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (termnote_nodename) */

#ifdef	COMMENT

static int colstate_load(colstate *csp,int ncols,int ncol) noex {
	int		rs = SR_FAULT ;
	if (csp) {
	    rs = SR_OK ;
	    csp->ncols = ncols ;
	    csp->ncol = ncol ;
	}
	return rs ;
}
/* end subroutine (colstate_load) */

/* return the number of characters that will fill the current column limit */
static int colstate_linecols(colstate *csp,cchar *sbuf,int slen) noex {
	cint		ntab = NTABCOLS ;
	int		rcols = (csp->ncols - csp->ncol) ;
	int		i ; /* used afterwards */
	for (i = 0 ; (rcols > 0) && (i < slen) ; i += 1) {
	    cint	cols = charcols(ntab,csp->ncol,sbuf[i]) ;
	    if (cols > rcols) break ;
	    csp->ncol += cols ;
	    rcols -= cols ;
	} /* end for */
	return i ;
}
/* end subroutine (colstate_linecols) */

#endif /* COMMENT */

static int mkclean(char *outbuf,int outlen,cchar *sbuf,int slen) noex {
	int		i ; /* used afterwards */
	for (i = 0 ; (i < outlen) && (i < slen) ; i += 1) {
	    outbuf[i] = sbuf[i] ;
	    if (isourbad(sbuf[i] & 0xff)) outbuf[i] = '­' ;
	} /* end for */
	return i ;
}
/* end subroutine (mkclean) */

static bool hasourbad(cchar *sp,int sl) noex {
	bool		f = false ;
	while (sl && (sp[0] != '\0')) {
	    cint	ch = (sp[0] & 0xff) ;
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
	    f = false ;
	    break ;
	default:
	    f = (! isprintlatin(ch)) ;
	    break ;
	} /* end switch */
	return f ;
}
/* end subroutine (isourbad) */

/* we want to do a reverse sort here (in descending order) */
static int vcmpatime(cvoid *v1pp,cvoid *v2pp) noex {
	USERTERM	**e1pp = (USERTERM **) v1pp ;
	USERTERM	**e2pp = (USERTERM **) v2pp ;
	int		rc = 0 ;
	{
	    USERTERM	*e1p = *e1pp ;
	    USERTERM	*e2p = *e2pp ;
	    if (e1p || e2p) {
	        if (e1p) {
	            if (e2p) {
	                rc = e2p->atime - e1p->atime ;
	            } else {
	                rc = -1 ;
		    }
	        } else {
	            rc = 1 ;
	        }
	    }
	} /* end block */
	return rc ;
}
/* end subroutine (vcmpatime) */


