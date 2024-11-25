/* lfm SUPPORT (Lock File Manager) */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* manage file-lock operations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	lfm

	Description:
	We want to check that we still own the lock file and also
	update it by writting a current time into it.

	Synopsis:
	int lfm_start(lfm *op,cc *fname,int type,int to,lfm_ch *lcp,
		cc *nn,cc *un,cc *banner) noex

	Arguments:
	op		object pointer
	fname		fname of lock file
	type		type of lock file
	timeout		timeout on waiting for it
	lcp		optional pointer to lock check object
	nn		node name
	un		user name
	banner		program banner

	Returns:
	>=0		OK
	<0		error (system-return)

	= Notes:

	Lock types:
	LFM_TRECORD		0		record lock
	LFM_TCREATE		1		create file 0444
	LFM_TEXCLUSIVE		2		exclusive open

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* system types */
#include	<sys/param.h>
#include	<sys/stat.h>		/* |USTAT| */
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
#include	<cstdarg>
#include	<algorithm>		/* |min(3c++)| + |max(3c++)| */
#include	<usystem.h>
#include	<ucgetpid.h>
#include	<estrings.h>
#include	<mallocxx.h>
#include	<getnodename.h>
#include	<getusername.h>
#include	<absfn.h>
#include	<linebuffer.h>
#include	<filer.h>
#include	<storeitem.h>
#include	<mkdirs.h>
#include	<mkpathx.h>
#include	<sfx.h>
#include	<rmx.h>
#include	<strn.h>
#include	<strwcpy.h>
#include	<cfdec.h>
#include	<timestr.h>
#include	<isnot.h>
#include	<iserror.h>
#include	<localmisc.h>		/* |TIMEBUFLEN| */

#include	"lfm.h"


/* local defines */

#define	SB		storebuffer
#define	SI		storeitem


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::min ;			/* subroutine-template */
using std::max ;			/* subroutine-template */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	fmtstr(char *,int,int,cchar *,va_list) noex ;
}


/* external variables */


/* local structures */

struct lfm_lin { /* lock-info */
	cchar		*nn ;
	cchar		*un ;
	cchar		*bn ;
	time_t		dt ;
} ; /* end struct (lfm_lin) */


/* forward references */

template<typename ... Args>
static int lfm_ctor(lfm *op,Args ... args) noex {
    	LFM		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = memclear(hop) ;
	    op->lfd = -1 ;
	    op->pid_lock = -1 ;
	    op->tocheck = LFM_TOMINCHECK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (lfm_ctor) */

static int lfm_dtor(lfm *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (lfm_dtor) */

template<typename ... Args>
static inline int lfm_magic(lfm *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == LFM_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (lfm_magic) */

static int	lfm_startcheck(lfm *,time_t) noex ;
static int	lfm_startopen(lfm *,lfm_lin *) noex ;

static int	lfm_fnbegin(lfm *,cchar *) noex ;
static int	lfm_fnend(lfm *) noex ;

static int	lfm_lockload(lfm *,lfm_ch *) noex ;
static int	lfm_locklost(lfm *,lfm_ch *,filer *) noex ;
static int	lfm_lockwrite(lfm *,lfm_lin *,int) noex ;
static int	lfm_lockwriter(lfm *,lfm_lin *,int) noex ;
static int	lfm_lockwritedate(lfm *,time_t) noex ;
static int	lfm_lockreadpid(lfm *) noex ;
static int	lfm_lockbegin(lfm *) noex ;
static int	lfm_lockend(lfm *) noex ;
static int	lfm_checklock(lfm *,time_t) noex ;
static int	lfm_ourdevino(lfm *,USTAT *) noex ;

static int	check_init(lfm_ch *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

namespace {
    struct starter {
	lfm		*op ;
	lfm_ch		*lcp ;
	char		*nnbuf{} ;
	char		*unbuf{} ;
	cchar		*fname{} ;
	cchar		*nn ;
	cchar		*un ;
	cchar		*bn ;
	int		nnlen = 0 ;
	int		unlen = 0 ;
	starter(lfm *o,lfm_ch *p,cc *n,cc *u,cc *b) noex {
	    op = o ;
	    lcp = p ;
	    nn = n ;
	    un = u ;
	    bn = b ;
	} ; /* end ctor */
	operator int () noex ;
	int allocbegin() noex ;
	int allocend() noex ;
	int opener() noex ;
	~starter() {
	    (void) allocend() ;
	} ; /* end if (dtor) */
    } ; /* end struct (starter) */
}

int lfm_start(lfm *op,cc *fname,int type,int to,lfm_ch *lcp,
		cc *nn,cc *un,cc *bn) noex {
	int		rs ;
	if (to < 0) to = LFM_TOLOCK ;
	if ((rs = lfm_ctor(op,fname)) >= 0) {
	    if ((rs = ucpid) >= 0) {
		op->pid = rs ;
	        op->type = type ;
	        op->tolock = to ;
	        rs = SR_INVALID ;
	        if (fname[0] && (type >= 0) && (type < LFM_TOVERLAST)) {
		    if ((rs = lfm_fnbegin(op,fname)) >= 0) {
			{
	            	    starter	so(op,lcp,nn,un,bn) ;
		    	    rs = so ;
			}
			if (rs < 0) {
			    lfm_fnend(op) ;
			}
		    } /* end if (lfm_fnbegin) */
	        } /* end if (valid) */
	    } /* end if (ucpid) */
	    if (rs < 0) {
		lfm_dtor(op) ;
	    }
	} /* end if (lfm_ctor) */
	return rs ;
}
/* end subroutine (lfm_start) */

starter::operator int () noex {
    	int		rs = SR_OK ;
	int		rs1 ;
	if ((rs = check_init(lcp)) >= 0) {
	    if ((rs = allocbegin()) >= 0) {
	        {
		    rs = opener() ;
		    if (rs < 0) {
	    	        if (lcp) lfm_lockload(op,lcp) ;
		    }
	        }
	        rs1 = allocend() ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (alloc-) */
	} /* end if (check_init) */
	return rs ;
}
/* end method (starter::operator) */

int starter::allocbegin() noex {
    	int		rs = SR_OK ;
	if ((rs >= 0) && (nn == nullptr)) {
	    if ((rs = malloc_nn(&nnbuf)) >= 0) {
		nnlen = rs ;
	        nn = nnbuf ;
	        rs = getnodename(nnbuf,nnlen) ;
	    } /* end if (memory-allocation) */
	}
	if ((rs >= 0) && (un == nullptr)) {
	    if ((rs = malloc_un(&unbuf)) >= 0) {
		unlen = rs ;
	        un = unbuf ;
	        rs = getusername(unbuf,unlen,-1) ;
	    } /* end if (memory-allocation) */
	}
	return rs ;
}
/* end method (starter::allocbegin) */

int starter::allocend() noex {
    	int		rs = SR_OK ;
	int		rs1 ;
	if (unbuf) {
	    rs1 = uc_free(unbuf) ;
	    if (rs >= 0) rs = rs1 ;
	    unbuf = nullptr ;
	    unlen = 0 ;
	}
	if (nnbuf) {
	    rs1 = uc_free(nnbuf) ;
	    if (rs >= 0) rs = rs1 ;
	    nnbuf = nullptr ;
	    nnlen = 0 ;
	}
	return rs ;
}
/* end method (starter::allocend) */

int starter::opener() noex {
    	int		rs = SR_OK ;
	cchar		*fn = op->lfname ;
	cchar		*dnp{} ;
	if (int dnl ; (dnl = sfdirname(fn,-1,&dnp)) > 0) {
            if (char *dbuf{} ; (rs = malloc_mp(&dbuf)) >= 0) {
                if ((rs = mkpath1w(dbuf,dnp,dnl)) >= 0) {
                    lfm_lin	li{} ;
                    custime 	dt = getustime ;
                    cint    	rsn = SR_NOENT ;
                    cint    	am = (X_OK | W_OK) ;
                    li.dt = dt ;
                    li.nn = nn ;
                    li.un = un ;
                    li.bn = bn ;
                    if ((rs = u_access(dbuf,am)) >= 0) {
                        if ((rs = lfm_startcheck(op,dt)) >= 0) {
                            rs = lfm_startopen(op,&li) ;
                        }
                    } else if (rs == rsn) {
                        cmode       dm = 0777 ;
                        if ((rs = mkdirs(dbuf,dm)) >= 0) {
                            rs = lfm_startopen(op,&li) ;
                        }
                    }
                } /* end if (mkpath) */
                rs = rsfree(rs,dbuf) ;
            } /* end if (m-a-f) */
	} /* end if (sfdirname) */
	return rs ;
}
/* end method (starter::opener) */

int lfm_finish(lfm *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = lfm_magic(op)) >= 0) {
	    if (op->lfd >= 0) {
	        rs1 = u_close(op->lfd) ;
	        if (rs >= 0) rs = rs1 ;
	        op->lfd = -1 ;
	    }
	    {
		rs1 = lfm_fnend(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = lfm_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (lfm_finish) */

int lfm_setpoll(lfm *op,int tocheck) noex {
    	int		rs ;
	if ((rs = lfm_magic(op)) >= 0) {
	    op->tocheck = tocheck ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (lfm_setpoll) */

int lfm_getinfo(lfm *op,lfm_in *ip) noex {
    	int		rs ;
	if ((rs = lfm_magic(op,ip)) >= 0) {
	    rs = SR_BUGCHECK ;
	    if (op->lfname) {
		rs = SR_OK ;
	        ip->dev = op->dev ;
	        ip->ino = op->ino ;
	        ip->tocheck = op->tocheck ;
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (lfm_getinfo) */

int lfm_check(lfm *op,lfm_ch *cip,time_t dt) noex {
	int		rs ;
	if ((rs = lfm_magic(op)) >= 0) {
	    rs = SR_BUGCHECK ;
	    if (op->lfname) {
		if ((rs = check_init(cip)) >= 0) {
	            if (op->tocheck >= 0) {
	                if (dt == 0) dt = getustime ;
	                if ((dt - op->ti_check) >= op->tocheck) {
			    bool	f = false ;
	                    op->ti_check = dt ;
	                    op->pid_lock = -1 ;
		            /* do a check on the INODE (if it is the same) */
			    f = f || ((dt - op->ti_stat) >= LFM_TOMINSTAT) ;
			    f = f || (op->type == LFM_TCREATE) ;
			    if (f) {
			        cchar	*fn = op->lfname ;
	                        if (USTAT sb ; (rs = u_stat(fn,&sb)) >= 0) {
	                            op->ti_stat = dt ;
	                            rs = lfm_ourdevino(op,&sb) ;
	                        }
	                    } /* end if (stat check) */
	                    if (rs >= 0) {
	                        rs = lfm_checklock(op,dt) ;
	                    } /* end if (ok) */
	                    if ((rs < 0) && cip) {
	                        lfm_lockload(op,cip) ;
	                        cip->status = rs ;
	                    }
	                } /* end if */
	            } /* end if (time-out enabled) */
		} /* end if (check_init) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (lfm_check) */

int lfm_printf(lfm *op,cchar *fmt,...) noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = lfm_magic(op,fmt)) >= 0) {
	    rs = SR_BUGCHECK ;
	    if (op->lfname) {
		rs = SR_OK ;
	        if (op->lfd < 0) {
	            if ((rs = u_open(op->lfname,O_RDWR,0)) >= 0) {
	                op->lfd = rs ;
		    }
	        }
	        if (rs >= 0) {
	            /* seek up to the proper place to start writing */
	            if ((rs = u_seek(op->lfd,op->owrite,SEEK_SET)) >= 0) {
	                va_list		ap ;
		        if (char *lbuf{} ; (rs = malloc_ml(&lbuf)) >= 0) {
	                    va_begin(ap,fmt) ;
		            cint	llen = rs ;
	                    if ((rs = fmtstr(lbuf,llen,0,fmt,ap)) >= 0) {
	                        if ((rs = u_write(op->lfd,lbuf,rs)) >= 0) {
				    len = rs ;
	                            op->owrite += rs ;
	                        }
	                    } /* end if (fmtstr) */
			    rs = rsfree(rs,lbuf) ;
	                    va_end(ap) ;
		        } /* end if (m-a-f) */
	            } /* end if (u_seek) */
	        } /* end if (ok) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (lfm_printf) */

int lfm_rewind(lfm *op) noex {
    	int		rs ;
	if ((rs = lfm_magic(op)) >= 0) {
	    rs = SR_BUGCHECK ;
	    if (op->lfname) {
		op->owrite = op->orewind ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (lfm_rewind) */

int lfm_flush(lfm *op) noex {
	int		rs ;
	if ((rs = lfm_magic(op)) >= 0) {
	    rs = SR_BUGCHECK ;
	    if (op->lfname) {
		rs = SR_OK ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (lfm_flush) */

int lfm_getpid(lfm *op,pid_t *rp) noex {
	int		rs ;
	if ((rs = lfm_magic(op)) >= 0) {
	    rs = SR_BUGCHECK ;
	    if (op->lfname) {
		rs = SR_OK ;
	        if (rp) {
	            *rp = op->pid ;
	        }
		rs = (op->pid & INT_MAX) ;
	    } /* end if (not bugcheck) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (lfm_getpid) */


/* private subroutines */

static int lfm_fnbegin(lfm *op,cchar *fname) noex {
    	int		rs = SR_OK ;
	int		rs1 ;
	cchar		*fnp{} ;
	if (int fnl ; (fnl = sfnamecomp(fname,0,&fnp)) > 0) {
	    cchar	*afn{} ;
	    if (absfn af ; (rs = af.start(fnp,fnl,&afn)) >= 0) {
	        if (cchar *cp{} ; (rs = uc_mallocstrw(afn,rs,&cp)) >= 0) {
		    op->lfname = cp ;
		} /* end if (memory-allocation) */
		rs1 = af.finish ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (absfn) */
	} /* end if (sfnamecomp) */
	return rs ;
}
/* end subroutine (lfm_fnbegin) */

static int lfm_fnend(lfm *op) noex {
    	int		rs = SR_OK ;
	int		rs1 ;
	if (op->lfname) {
	    if ((op->type >= LFM_TCREATE) && (op->lfname[0] != '\0')) {
	        u_unlink(op->lfname) ;
	    }
	    {
	        rs1 = uc_free(op->lfname) ;
	        if (rs >= 0) rs = rs1 ;
	        op->lfname = nullptr ;
	    }
	}
	return rs ;
}
/* end subroutine (lfm_fnend) */

static int lfm_startcheck(lfm *op,time_t dt) noex {
	cint		type = op->type ;
	int		rs ;
	cchar		*lfn = op->lfname ;
	if (USTAT sb ; (rs = u_stat(lfn,&sb)) >= 0) {
	    if (S_ISREG(sb.st_mode)) {
	        cint	to = op->tolock ;
	        op->ti_check = dt ;
	        op->ti_stat = dt ;
	        if (type >= LFM_TCREATE) {
	            if ((dt - sb.st_mtime) >= to) {
	                u_unlink(op->lfname) ;
	            } else {
	                rs = SR_LOCKED ;
	            }
	        } /* end if (good stat) */
	    } else {
	        rs = SR_ISDIR ;
	    }
	} else if (isNotPresent(rs)) {
	    rs = SR_OK ;
	} /* end if (u_stat) */
	return rs ;
}
/* end subroutine (lfm_startcheck) */

static int lfm_startopen(lfm *op,lfm_lin *lip) noex {
	cint		type = op->type ;
	int		rs ;
	int		of = (O_RDWR | O_CREAT) ;
	mode_t		om = 0664 ;
	if (type >= LFM_TEXCLUSIVE) of |= O_EXCL ;
	if (type >= LFM_TCREATE) om = 0444 ;
	if ((rs = u_open(op->lfname,of,om)) >= 0) {
	    cint	lfd = rs ;
	    cint	cmd = F_TLOCK ;
	    if ((rs = uc_lockf(lfd,cmd,0z)) >= 0) {
	        if ((rs = lfm_lockwrite(op,lip,lfd)) >= 0) {
	            if (USTAT sb ; (rs = u_fstat(lfd,&sb)) >= 0) {
	                op->lfd = lfd ;
	                op->magic = LFM_MAGIC ;
	                op->dev = sb.st_dev ;
	                op->ino = sb.st_ino ;
	            }
	        }
	    } else if (rs == SR_ACCESS) {
	        rs = SR_LOCKED ;
	    } /* end if (uc_lockf) */
	    if (rs < 0) {
	        switch (rs) {
	        case SR_EXIST:
	        case SR_ACCESS:
	            rs = SR_LOCKED ;
	            break ;
	        } /* end switch */
	    } /* end if (error-handle) */
	} /* end if (u_open) */
	return rs ;
}
/* end subroutie (lfm_startopen) */

static int lfm_lockload(lfm *op,lfm_ch *lcp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = check_init(lcp)) > 0) {
	    if (char *lbuf{} ; (rs = malloc_ml(&lbuf)) >= 0) {
	        cint	llen = rs ;
	        cint	of = O_RDONLY ;
	        cmode	om = 0666 ;
	        if ((rs = u_open(op->lfname,of,om)) >= 0) {
	            cint	lfd = rs ;
		    cint	bsz = 512 ;
	            if (filer b ; (rs = b.start(lfd,0z,bsz,0)) >= 0) {
	                if ((rs = b.readln(lbuf,llen,0)) > 0) {
	                    cint	len = rmeol(lbuf,rs) ;
	                    if (int v{} ; (rs = cfdeci(lbuf,len,&v)) >= 0) {
	                        op->pid_lock = pid_t(v) ;
			    } else if (isNotValid(rs)) {
	                        op->pid_lock = pid_t(-1) ;
			    }
			    /* resume with the rest of the data gathering */
			    if (rs >= 0) {
	                        rs = lfm_locklost(op,lcp,&b) ;
			    }
	                } else {
	                    rs = SR_LOCKLOST ;
	                }
	                rs1 = b.finish ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (filer) */
	            rs1 = u_close(lfd) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (opened file) */
		rs = rsfree(rs,lbuf) ;
	    } /* end if (m-a-f) */
	} /* end if (check_init) */
	return rs ;
}
/* end subroutine (lfm_lockload) */

namespace {
    lfm		*op ;
    lfm_ch	*lcp ;
    linebuffer	*lbp ;
    storeitem	*sip ;
    struct parser {
	parser(lfm *aop,lfm_ch *alcp,linebuffer *albp,storeitem *asip) noex {
	    op = aop ;
	    lcp = alcp ;
	    lbp = albp ;
	    sip = asip ;
	} ; /* end ctor */
	int pnodeuser(int) noex ;
	int ptimebanner(int) noex ;
    } ; /* end struct (parser) */
}

int parser::pnodeuser(int len) noex {
    	cnullptr	np{} ;
	int		rs = SR_OK ;
	int		sl = len ;
	int		cl ;
	cchar		*sp = lbp->lbuf ;
	cchar		*cp ;
	if (cc *tp{} ; (tp = strnchr(sp,sl,'!')) != np) {
	    sl = (tp - lbp->lbuf) ;
	    if ((cl = sfshrink(sp,sl,&cp)) > 0) {
		if (cc *ip{} ; (rs = sip->strw(cp,cl,&ip)) >= 0) {
		    lcp->nodename = ip ;
		}
		sl = len - ((tp + 1) - sp) ;
		sp = (tp + 1) ;
	    } /* end if (sfshrink) */
	} /* end if */
	if (rs >= 0) {
	    if ((cl = sfshrink(sp,sl,&cp)) >= 0) { /* zero-len allowed? */
	        if (cc *ip{} ; (rs = sip->strw(cp,cl,&ip)) >= 0) {
	            lcp->username = ip ;
	        }
	    }
	} /* end if */
	return rs ;
}
/* end subroutine (parser::pnodeuser) */

int parser::ptimebanner(int len) noex {
    	cnullptr	np{} ;
    	int		rs = SR_OK ;
	if (cc *tp{} ; (tp = strnpbrk(lbp->lbuf,len," \t")) != np) {
	    cchar	*sp = (tp + 1) ;
	    cint	sl = len - ((tp + 1) - lbp->lbuf) ;
	    cchar	*cp ;
	    if (int cl ; (cl = sfshrink(sp,sl,&cp)) >= 0) {
		if (cc *ip{} ; (rs = sip->strw(cp,cl,&ip)) >= 0) {
		    lcp->banner = ip ;
		}
	    }
	}
	return rs ;
}
/* end subroutine (lfm_ptimebanner) */

/* read the lock information on a failure */
static int lfm_locklost(lfm *op,lfm_ch *lcp,filer *fp) noex {
	cint		buflen = LFM_CHBUFLEN ;
	int		rs = SR_BUGCHECK ;
	int		rs1 ;
	if (lcp && fp) {
	    lcp->pid = op->pid_lock ;
	    if (linebuffer lb ; (rs = lb.start) >= 0) {
	        if (storeitem cb ; (rs = cb.start(lcp->buf,buflen)) >= 0) {
		    parser	po(op,lcp,&lb,&cb) ;
	            if ((rs = fp->readln(lb.lbuf,lb.llen,0)) > 0) {
	                int	len = rmeol(lb.lbuf,rs) ;
			if ((rs = po.pnodeuser(len)) >= 0) {
	                    if ((rs = fp->readln(lb.lbuf,lb.llen,0)) > 0) {
	                        len = rmeol(lb.lbuf,rs) ;
				rs = po.ptimebanner(len) ;
			    }
	                } /* end if (reading time-banner) */
	            } /* end if (reading continued) */
	            rs1 = cb.finish ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (storeitem) */
		rs = lb.finish ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (linebuffer) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (lfm_locklost) */

static int lfm_lockwrite(lfm *op,lfm_lin *lip,int lfd) noex {
	int		rs ;
	if ((rs = lfm_lockwriter(op,lip,lfd)) >= 0) {
	    coff	woff = rs ;
	    if ((rs = uc_ftruncate(lfd,woff)) >= 0) {
	        op->owrite = woff ;
	        op->orewind = woff ;
	        rs = int(woff) ;
	    }
	} /* end if (lfm_lockwriter) */
	return rs ;
}
/* end subroutine (lfm_lockwrite) */

static int lfm_lockwriter(lfm *op,lfm_lin *lip,int lfd) noex {
	int		rs ;
	int		rs1 ;
	int		woff = 0 ;
	if (filer b ; (rs = b.start(lfd,0z,512,0)) >= 0) {
	    if (rs >= 0) {
	        rs = b.printf("%u\n",op->pid) ;
	        woff += rs ;
	    }
	    if (rs >= 0) {
	        rs = b.printf("%s!%s\n",lip->nn,lip->un) ;
	        woff += rs ;
	    }
	    if (rs >= 0) {
	        cchar	*bn = (lip->bn) ? lip->bn : "" ;
	        char	tbuf[TIMEBUFLEN+1] ;
	        op->odate = woff ;
	        timestr_logz(lip->dt,tbuf) ;
	        rs = b.printf("%s %s\n",tbuf,bn) ;
	        woff += rs ;
	    }
	    rs1 = b.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (filer) */
	return (rs >= 0) ? woff : rs ;
}
/* end subroutine (lfm_lockwriter) */

static int lfm_lockreadpid(lfm *op) noex {
    	cnullptr	np{} ;
	cint		lfd = op->lfd ;
	int		rs ;
	int		v = 0 ;
	if ((rs = u_rewind(lfd)) >= 0) {
	    if (char *lbuf{} ; (rs = malloc_ml(&lbuf)) >= 0) {
	        cint	llen = rs ;
	        if ((rs = u_read(lfd,lbuf,llen)) >= 0) {
	            int		len = rs ;
	            if (cchar *tp{} ; (tp = strnchr(lbuf,len,'\n')) != np) {
			len = (tp - lbuf) ;
	            }
	            rs = cfdeci(lbuf,len,&v) ;
	            v &= INT_MAX ;
	        } /* end if (u_read) */
		rs = rsfree(rs,lbuf) ;
	    } /* end if (m-a-f) */
	} /* end if (u_rewind) */
	return (rs >= 0) ? v : rs ;
}
/* end subroutine (lfm_lockreadpid) */

static int lfm_lockwritedate(lfm *op,time_t dt) noex {
	int		rs ;
	if ((rs = u_seek(op->lfd,op->odate,SEEK_SET)) >= 0) {
	    int		tl ;
	    char	tbuf[TIMEBUFLEN+2] ;
	    timestr_logz(dt,tbuf) ;
	    tl = strlen(tbuf) ;
	    rs = u_write(op->lfd,tbuf,tl) ;
	} /* end if (u_seek) */
	return rs ;
}
/* end subroutine (lfm_lockwritedate) */

static int lfm_lockbegin(lfm *op) noex {
	int		rs = SR_OK ;
	int		f_open = false ;
	if (op->lfd < 0) {
	    cint	of = O_RDWR ;
	    cmode	om = 0666 ;
	    f_open = true ;
	    rs = u_open(op->lfname,of,om) ;
	    op->lfd = rs ;
	}
	return (rs >= 0) ? f_open : rs ;
}
/* end subroutine (lfm_lockbegin) */

static int lfm_lockend(lfm *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		f_open = false ;
	if (op->lfd >= 0) {
	    f_open = true ; /* was *previosuly* open */
	    rs1 = u_close(op->lfd) ;
	    if (rs >= 0) rs = rs1 ;
	    op->lfd = -1 ;
	}
	return (rs >= 0) ? f_open : rs ;
}
/* end subroutine (lfm_lockend) */

static int lfm_checklock(lfm *op,time_t dt) noex {
	int		rs ;
	int		rs1 ;
	int		v = 0 ;
	if ((rs = lfm_lockbegin(op)) >= 0) {
	    bool	f_open = (rs > 0) ;
	    if ((rs = lfm_lockreadpid(op)) >= 0) {
	        const pid_t	pid_lock = rs ;
	        if (pid_lock != op->pid)  {
	            rs = SR_LOCKLOST ;
	        }
	        if (rs >= 0) {
	            v = pid_lock ;
	            rs = lfm_lockwritedate(op,dt) ;
	        }
	    } /* end if (lfm_lockreadpid) */
	    if (f_open) {
		rs1 = lfm_lockend(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (lfm_lock) */
	return (rs >= 0) ? v : rs ;
}
/* end subroutine (lfm_checklock) */

static int lfm_ourdevino(lfm *op,USTAT *sbp) noex {
	int		rs = SR_OK ;
	if ((sbp->st_dev != op->dev) || (sbp->st_ino != op->ino)) {
	    rs = SR_LOCKLOST ;
	}
	return rs ;
}
/* end subroutine (lfm_ourdevino) */

static int check_init(lfm_ch *lcp) noex {
    	int		rs = SR_OK ;
	if (lcp) {
	    rs = 1 ;
	    memclear(lcp) ;
	    lcp->nodename = nullptr ;
	    lcp->username = nullptr ;
	    lcp->banner = nullptr ;
	    lcp->pid = -1 ;
	    lcp->status = 0 ;
	    lcp->buf[0] = '\0' ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (check_init) */


