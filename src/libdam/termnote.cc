/* termnote SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* perform terminal noticing */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This object module was originally written to create a logging
	mechanism for PCS application programs.

	= 2024-04-03, David A­D­ Morano
	I updated the comments below about the use of the TMPX
	database.

*/

/* Copyright © 1998,2024 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	termnote

	Description:
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
	now (a-days).  If there is ever a real need, yes, we can
	certaintly speed this up. So it is no big deal right now.

	BUG NOTE:

	Note that the (stupid) Solaris® version of the standard
	UTMPX interface does not work properly for some (unknown)
	strange reason.  This bug is quite annoying since some
	terminals are not biffed as they should be due to the bug.
	The bug somehow causes an enumeration of the UTMPX database
	file to skip over some UTMPX records.  Skipped records might
	just contain a terminal that needs to be biffed.  To work
	around the Solaris® bug, we use a separate UTMPX database
	interface entirely (TMPX).  That interface is not too
	dissimilar to the standard interface, except in the case
	of Solaris®, it works properly!

	Updated note (2024-04-03, David A-D- Morano):
	1. Just for the knowledge of the reader: The TMPX object
	database is used in the original code (below) and is still
	used now.  But how the TMPX database is actually implemented
	now-a-days is likely changed since the original.  Originally,
	the TMPX database read the underlying UTMPX database file
	(whatever that was) directly.  I do not know if this is
	still the case today.  If the TMPX databse depends on the
	hosting operating system UTMPX database system, bugs could
	still be in that system code.
	2. In a separate matter, the use (existence) of this object
	is not as important as it once was.  The more modern idea
	about putting messages on user terminals is to have it done
	synchronously (some how).  Rather than just splashing some
	foreign message (from who knows from where) onto a user
	terminal, rather the newer idea is to make a pending message
	known the the user shell or other program, and then have
	the user program (one or more) then display the message
	synchronously to the user (on the terminal display).
	3. I would be remiss to not note that the use of (old)
	terminals is pretty much passay already.

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
#include	<mklogid.h>
#include	<sncpyx.h>
#include	<mkpathx.h>
#include	<termconseq.h>
#include	<ncol.h>		/* |charcols(3uc)| */
#include	<pow.h>
#include	<xperm.h>
#include	<timestr.h>
#include	<mkchar.h>
#include	<ischarx.h>
#include	<isnot.h>
#include	<localmisc.h>		/* |NTABCOLS| + |COLUMNS| */

#include	"termnote.h"


/* local defines */

#define	TERMNOTE_SEARCHNAME	"termnote"

#ifndef	DEVDNAME
#define	DEVDNAME	"/dev"
#endif

#define	LOGDNAME	"log"

#ifndef	TERMDEVLEN
#define	TERMDEVLEN	80
#endif

#define	TO_TMPX		(15 * 60)
#define	TO_CHECK	5
#define	TO_WRITE	30
#define	TO_LOGCHECK	5
#define	TO_LOCK		4

#define	COLSTATE	struct colstate


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */

typedef buffer		mbuf ;


/* external subroutines */

extern "C" {
    extern int	bufvprintf(char *,int,cchar *,va_list) noex ;
    extern int	writeto(int,cvoid *,int,int) noex ;
}


/* external variables */


/* local structures */

namespace {
    struct userterm {
	static cint	tdlen ;
	time_t		atime ;
	char		tdbuf[TERMDEVLEN + 1] ;
	userterm() noex : atime(0) {
	    tdbuf[0] = '\0' ;
	} ;
	int load(cchar *sp,int sl = -1) noex {
	    return sncpy1w(tdbuf,tdlen,sp,sl) ;
	} ;
   } ; /* end struct (userterm) */
}

typedef userterm *	usertermp ;

struct colstate {
	int		ncols ;
	int		ncol ;
} ;


/* forward references */

template<typename ... Args>
static int termnote_ctor(termnote *op,Args ... args) noex {
    	TERMNOTE	*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    memclear(hop) ;
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
	    if (op->lfp) {
		delete op->lfp ;
		op->lfp = nullptr ;
	    }
	    if (op->txp) {
		delete op->txp ;
		op->txp = nullptr ;
	    }
	    if (op->idp) {
		delete op->idp ;
		op->idp = nullptr ;
	    }
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
static int termnote_lfopener(termnote *,time_t,cchar *,cchar *) noex ;
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
static int	vcmpatime(cvoid **,cvoid **) noex ;

static bool	hasourbad(cchar *,int) noex ;
static bool	isourbad(int) noex ;


/* local variables */

cint	userterm::tdlen = TERMDEVLEN ;


/* exported variables */


/* exported subroutines */

int termnote_open(termnote *op,cchar *pr) noex {
	const time_t	dt = getustime ;
	int		rs ;
	if ((rs = termnote_ctor(op,pr)) >= 0) {
	    rs = SR_INVALID ;
	    if (pr[0]) {
	        if (cchar *cp{} ; (rs = uc_mallocstrw(pr,-1,&cp)) >= 0) {
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
	        const time_t	dt = getustime ;
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
int termnote_vprintf(termnote *op,cc **rpp,int n,int o,
		cc *fmt,va_list ap) noex {
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	if ((rs = termnote_magic(op,rpp,fmt)) >= 0) {
	    if (char *obuf{} ; (rs = malloc_ml(&obuf)) >= 0) {
		cint	olen = rs ;
	        if ((rs = bufvprintf(obuf,olen,fmt,ap)) >= 0) {
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
	    if (dt == 0) dt = getustime ;
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
		            if (dt == 0) dt = getustime ;
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
	                if (dt == 0) dt = getustime ;
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
	                int	bl = cl ;
	                if (hasourbad(cp,cl)) {
	                    bp = tmpbuf ;
	                    bl = mkclean(tmpbuf,tmplen,cp,cl) ;
	                }
	                if ((rs = buffer_chr(obp,'\r')) >= 0) {
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
	        rs = buffer_chr(obp,CH_BEL) ;
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

namespace {
    struct disuser {
	termnote	*op ;
	vecobj		*tlp ;
	disuser(termnote *tp,vecobj *vp) noex : op(tp), tlp(vp) { } ;
	int loadterms(int,cchar *) noex ;
	int loadtermsx(vecstr *,int) noex ;
	int writeterms(int,int,mbuf *) noex ;
    } ; /* end struct (disuser) */
}

static int termnote_disuser(termnote *op,int nmax,int o,mbuf *mp,cc *un) noex {
	vecobj		uts ;
	cint		utsize = szof(userterm) ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = vecobj_start(&uts,utsize,0,0)) >= 0) {
	    disuser	diso(op,&uts) ;
	    if ((rs = diso.loadterms(o,un)) >= 0) {
		rs = diso.writeterms(nmax,o,mp) ;
		c = rs ;
	    } /* end if (disuser::loadterms) */
	    rs1 = vecobj_finish(&uts) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (user-term list) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (termnote_disuser) */

int disuser::loadterms(int o,cchar *un) noex {
	vecstr		lines ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = vecstr_start(&lines,0,0)) >= 0) {
	    if ((rs = tmpx_getuserlines(op->txp,&lines,un)) >= 0) {
		cint	nlines = rs ;
		if (op->open.lf) {
		    cchar	*lfmt = "u=%s termlines=%u" ;
		    logfile_printf(op->lfp,lfmt,un,nlines) ;
		}
	        if ((rs >= 0) && (nlines > 0)) {
		    rs = loadtermsx(&lines,o) ;
		    c = rs ;
	        } /* end if */
	    } /* end if (tmpx_getuserlines) */
	    rs1 = vecstr_finish(&lines) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (user-term lines) */
	return (rs >= 0) ? c : rs ;
}
/* end method (disuser::loadterms) */

int disuser::loadtermsx(vecstr *llp,int o) noex {
	userterm	ut ;
	int		rs = SR_OK ;
	int		rs1 ;
        int             ll ;
        cchar           *lp{} ;
	cchar		*devdname = DEVDNAME ;
	char		termfname[TERMDEVLEN+1] ;
        for (int i = 0 ; vecstr_get(llp,i,&lp) >= 0 ; i += 1) {
            if (lp == nullptr) continue ;
            ll = strlen(lp) ;
            if (ll > 0) {
                if ((rs = mkpath2w(termfname,devdname,lp,ll)) >= 0) {
                    cint	tl = rs ;
                    if ((rs1 = ut.load(termfname,tl)) >= 0) {
                        USTAT   sb ;
                        bool    f_go = false ;
                        rs1 = u_stat(ut.tdbuf,&sb) ;
                        if (rs1 >= 0) {
                            f_go = (sb.st_mode & S_IWGRP) ;
                            if (f_go && (o & TERMNOTE_OBIFF)) {
                                f_go = (sb.st_mode & S_IXUSR) ;
                            }
                        }
                        if ((rs1 >= 0) && f_go) {
                            rs1 = sperm(op->idp,&sb,W_OK) ;
                            if (rs1 >= 0) {
                                ut.atime = sb.st_atime ;
                                rs = vecobj_add(tlp,&ut) ;
                            }
                        }
                    } /* end if (userterm::load) */
                } /* end if (mkpath) */
            } /* end if (positive) */
            if (rs < 0) break ;
        } /* end for */
	return rs ;
}
/* end method (disuser::loadtermsx) */

int disuser::writeterms(int nmax,int o,mbuf *mp) noex {
        vecobj_vcf	vcf = vecobj_vcf(vcmpatime) ;
	int		rs ;
	int		c = 0 ;
	if ((rs = vecobj_sort(tlp,vcf)) >= 0) {
            cint	navail = rs ;
            cchar	*lfmt = "  %s (%d)" ;
            void	*vp{} ;
            if (op->open.lf) {
                logfile_printf(op->lfp,"  avail=%u",navail) ;
            }
            for (int i = 0 ; vecobj_get(tlp,i,&vp) >= 0 ; i += 1) {
                if (vp) {
                    userterm        *utp = usertermp(vp) ;
                    int             rsv = 0 ;
                    cchar           *tdp = utp->tdbuf ;
                    if ((rs = termnote_diswrite(op,o,mp,tdp)) >= 0) {
                        c += 1 ;
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
                    if (c >= nmax) break ;
                } /* end if (non-null) */
                if (rs < 0) break ;
            } /* end for (looping through user-terms) */
	} /* end if (vecobj_sort) */
	return (rs >= 0) ? c : rs ;
}
/* end method (disuser::writeterms) */

static int termnote_diswrite(termnote *op,int o,mbuf *mp,cc *termdev) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (op && mp && termdev) {
	    cint	of = (O_WRONLY | O_NOCTTY | O_NDELAY) ;
	    cint	to = TO_WRITE ;
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
	    if (dt == 0) dt = getustime ;
	    if ((rs = tmpx_open(op->txp,nullptr,0)) >= 0) {
	        op->open.tx = true ;
	        op->ti_tmpx = dt ;
	    }
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
	int		rs = SR_OK ;
	int		rs1 ;
	int		f_opened = false ;
	if (! op->init.lf) {
	    op->init.lf = true ;
	    if (rs >= 0) {
		rs = termnote_nodename(op) ;
	    }
	    if (rs >= 0) {
		if ((rs = ucpid) >= 0) {
		    cint	pid = rs ;
		    rs = mklogidpre(op->logid,LOGIDLEN,op->nodename,pid) ;
		}
	    }
	    if (rs >= 0) {
	        if (char *lfname{} ; (rs = malloc_mp(&lfname)) >= 0) {
	  	    cchar	*sn = TERMNOTE_SEARCHNAME ;
	            if ((rs = mkpath3(lfname,op->pr,LOGDNAME,sn)) >= 0) {
		        rs = termnote_lfopener(op,dt,lfname,sn) ;
		        f_opened = (rs > 0) ;
		    }
		    rs1 = uc_free(lfname) ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (m-a-f) */
	    } /* end if (ok) */
	} /* end if (needed initialization) */
	return (rs >= 0) ? f_opened : rs ;
}
/* end subroutine (termnote_lfopen) */

static int termnote_lfopener(termnote *op,time_t dt,cc *lfname,cc *sn) noex {
	logfile		*lfp = op->lfp ;
	int		rs ;
	int		fopened = false ;
	cint		of = O_RDWR ;
	cmode		om = 0666 ;
        if ((rs = logfile_open(lfp,lfname,of,om,op->logid)) >= 0) {
            fopened = true ;
            op->open.lf = true ;
            if (rs >= 0) {
                rs = logfile_checksize(lfp,TERMNOTE_LOGSIZE) ;
            }
            if (rs >= 0) {
                rs = termnote_username(op) ;
            }
            if (rs >= 0) {
                cchar   *nn = op->nodename ;
                cchar   *un = op->username ;
                char    timebuf[TIMEBUFLEN+1] ;
                if (dt == 0) dt = getustime ;
                timestr_logz(dt,timebuf) ;
                if ((rs = logfile_printf(lfp,"%s %s",timebuf,sn)) >= 0) {
                    rs = logfile_printf(lfp,"%s!%s",nn,un) ;
		}
            } /* end if (ok) */
        } else if (isNotPresent(rs)) {
            rs = SR_OK ;
        } /* end if (logfile opened) */
	return (rs >= 0) ? fopened : rs ;
}
/* end subroutine (termnote_lfopener) */

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
	    if (char *ubuf{} ; (rs = malloc_un(&ubuf)) >= 0) {
		cint	ulen = rs ;
	        if ((rs = getusername(ubuf,ulen,-1)) >= 0) {
	            rl = rs ;
		    if (cc *cp{} ; (rs = uc_mallocstrw(ubuf,rl,&cp)) >= 0) {
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
	    if (char *nbuf{} ; (rs = malloc_nn(&nbuf)) >= 0) {
		cint	nlen = rs ;
	        if ((rs = getnodename(nbuf,nlen)) >= 0) {
	            rl = rs ;
		    if (cc *cp{} ; (rs = uc_mallocstrw(nbuf,rl,&cp)) >= 0) {
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

static int mkclean(char *obuf,int olen,cchar *sp,int sl) noex {
	int		i ; /* used afterwards */
	for (i = 0 ; (i < olen) && (i < sl) ; i += 1) {
	    cint	ch = mkchar(sp[i]) ;
	    obuf[i] = (isourbad(ch)) ? '¿' : ch ;
	} /* end for */
	return i ;
}
/* end subroutine (mkclean) */

static bool hasourbad(cchar *sp,int sl) noex {
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
static int vcmpatime(cvoid **v1pp,cvoid **v2pp) noex {
	userterm	**e1pp = (userterm **) v1pp ;
	userterm	**e2pp = (userterm **) v2pp ;
	int		rc = 0 ;
	{
	    userterm	*e1p = *e1pp ;
	    userterm	*e2p = *e2pp ;
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


