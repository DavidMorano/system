/* useraccdb SUPPORT */
/* lang=C++20 */

/* user-access (user-access-logging) data-base management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	useraccdb

	Description:
	This object implements a small database for storing the
	last time a user accesses something (usually a program).

	Format of file database records:

	- eight digits of a decimal count number field
	- the time of the last entry update
	- the nodename and username
	- an optional real or fullname of the user

	Format example:

0         1         2         3         4
01234567890123456789012345678901234567890
     989 111115_1137:06_EST      TOTAL
     775 111115_1137:06_EST      rca!local (LOCAL)
     200 111001_2015:04_EDT      rca!dam (David A­D­ Morano)
      14 110103_0922:32_EST      rca!genserv (GENSERV)


	Note that a special user field with the value of "TOTAL"
	maintains a total count of all program invocations separately
	from the user counts.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<sys/timeb.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<ctime>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<bufsizevar.hh>
#include	<mallocxx.h>
#include	<linebuffer.h>
#include	<estrings.h>
#include	<ascii.h>
#include	<filer.h>
#include	<storeitem.h>
#include	<linebuffer.h>
#include	<initnow.h>
#include	<dater.h>
#include	<timestr.h>
#include	<cfdec.h>
#include	<ctdec.h>
#include	<sfx.h>
#include	<strwcmp.h>
#include	<localmisc.h>		/* |DIGBUFLEN| */

#include	"useraccdb.h"


/* local defines */

#define	USERACCDB_LOGDNAME	"var/log"
#define	USERACCDB_MAGIC		0x11359299
#define	USERACCDB_INTCHECK	5

#define	UAFILE_SUF		"users"
#define	UAFILE_LCOUNT		8
#define	UAFILE_LDATE		(32-9)
#define	UAFILE_MAXUSERLEN	128
#define	UAFILE_MAXNAMELEN	128
#define	UAFILE_RECLEN		\
	(UAFILE_LCOUNT + 1 + UAFILE_LDATE + 1 + \
	UAFILE_MAXUSERLEN + 2 + UAFILE_MAXNAMELEN + 2) 

#define	UAD		useraccdb
#define	UAD_CUR		useraccdb_cur
#define	UAD_ENT		useraccdb_ent
#define	UAD_REC		useraccdb_rec
#define	UAD_ITEM	useraccdb_item

#define	UPINFO		upinfo
#define	UPINFO_REC		struct upinfo_rec

#define	TO_CHECK	5		/* check interval */
#define	TO_LOCK		4		/* time-out */

#define	TOTALNAME	"TOTAL"


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct upinfo_rec {
	off_t		ro ;		/* record offset */
	uint		count ;
	uint		found:1 ;
} ;

struct upinfo {
	UPINFO_REC	user, total ;
	useraccdb	*op ;
	cchar		*arguser ;
	cchar		*argname ;
	time_t		utime ;
	char		tbuf[UAFILE_LDATE+1] ;
} ;

struct useraccdb_item {
	cchar		*sp ;
	int		sl ;
} ;

struct useraccdb_rec {
	UAD_ITEM	countstr ;
	UAD_ITEM	datestr ;
	UAD_ITEM	userstr ;
	UAD_ITEM	namestr ;
	time_t		atime ;
	uint		count ;
} ;


/* forward references */

template<typename ... Args>
static int useraccdb_ctor(UAD *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    op->magic = 0 ;
	    if ((op->dmp = new(nothrow) dater) != np) {
		rs = SR_OK ;
	    } /* end if (new-dater) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (useraccdb_ctor) */

static int useraccdb_dtor(UAD *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->dmp) {
		delete op->dmp ;
		op->dmp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (useraccdb_dtor) */

template<typename ... Args>
static int useraccdb_magic(UAD *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == USERACCDB_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (useraccdb_magic) */

static int	useraccdb_fileopen(UAD *) noex ;
static int	useraccdb_openlock(UAD *) noex ;
static int	useraccdb_fileclose(UAD *) noex ;
static int	useraccdb_lock(UAD *,int) noex ;
static int	useraccdb_recproc(UAD *,UAD_REC *) noex ;
static int	useraccdb_doit(UAD *,time_t *,cchar *,int) noex ;

static int	upinfo_start(UPINFO *,UAD *,cchar *,cchar *) noex ;
static int	upinfo_match(UPINFO *,off_t,UAD_REC *) noex ;
static int	upinfo_update(UPINFO *) noex ;
static int	upinfo_finish(UPINFO *) noex ;
static int	upinfo_upone(UPINFO *,UPINFO_REC *,int) noex ;
static int	upinfo_mkrec(UPINFO *,UPINFO_REC *,char *,int,int) noex ;

static int	rec_parse(UAD_REC *,cc *,int) noex ;

static int	entry_load(UAD_ENT *,char *,int,UAD_REC *) noex ;

static int	mkts(char *,int,time_t) noex ;


/* local variables */

static bufsizevar	maxpathlen(getbufsize_mp) ;

constexpr int		diglen = DIGBUFLEN ;

constexpr cchar		totaluser[] = TOTALNAME ;


/* exported variables */


/* exported subroutines */

int useraccdb_open(UAD *op,cchar *pr,cchar *dbname) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = useraccdb_ctor(op,pr,dbname)) >= 0) {
	    rs = SR_INVALID ;
	    if (pr[0] && dbname[0]) {
		if ((rs = maxpathlen) >= 0) {
		    cint	sz = (2 * (rs + 1)) ;
		    cint	maxpath = rs ;
		    caddr_t	a{} ;
		    if ((rs = uc_malloc(sz,&a)) >= 0) {
		        cchar	*suf = UAFILE_SUF ;
	                cchar	*logdname = USERACCDB_LOGDNAME ;
	                char	*cname = (a + (0 * (maxpath + 1))) ;
	                op->eo = -1 ;
	                op->fd = -1 ;
	                if ((rs = snsds(cname,maxpath,dbname,suf)) >= 0) {
	                    char	*fname = (a + (1 * (maxpath + 1))) ;
	                    if ((rs = mkpath(fname,pr,logdname,cname)) >= 0) {
		                cint	pl = rs ;
		                cchar	*cp ;
		                if ((rs = uc_mallocstrw(fname,pl,&cp)) >= 0) {
		                    op->fname = cp ;
		                    if ((rs = useraccdb_fileopen(op)) >= 0) {
			                op->magic = USERACCDB_MAGIC ;
		                    }
		                    if (rs < 0) {
	    		                uc_free(op->fname) ;
	    		                op->fname = nullptr ;
		                    }
		                } /* end if (memory-allocation) */
	                    } /* end if (mkpath) */
	                } /* end if (make-component) */
		        rs1 = uc_free(a) ;
			if (rs >= 0) rs = rs1 ;
		    } /* end if (m-a-f) */
		} /* end if (maxpathlen) */
	    } /* end if (valid) */
	    if (rs < 0) {
		useraccdb_dtor(op) ;
	    }
	} /* end if (useraccdb_ctor) */
	return rs ;
}
/* end subroutine (useraccdb_open) */

int useraccdb_close(UAD *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = useraccdb_magic(op)) >= 0) {
	    if (op->f.dater) {
	        op->f.dater = false ;
	        rs1 = dater_finish(op->dmp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = useraccdb_fileclose(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->fname) {
	        rs1 = uc_free(op->fname) ;
	        if (rs >= 0) rs = rs1 ;
	        op->fname = nullptr ;
	    }
	    {
		rs1 = useraccdb_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (useraccdb_close) */

int useraccdb_find(UAD *op,UAD_ENT *ep,char *ebuf,int elen,
		cchar *user) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = useraccdb_magic(op,ep,ebuf,user)) >= 0) {
	    rs = SR_INVALID ;
	    if (op->eo < 0) {
	        if ((rs = useraccdb_openlock(op)) >= 0) {
		    linebuffer	lb ;
		    if ((rs = lb.start) >= 0) {
	                filer	b ;
	                if ((rs = filer_start(&b,op->fd,0L,0,0)) >= 0) {
	                    UAD_REC	rec ;
	                    cint	llen = lb.llen ;
	                    char	*lbuf = lb.lbuf ;
	                    while ((rs1 = filer_readln(&b,lbuf,llen,-1)) > 0) {
	                        cint	ll = rs ;
	                        if ((rs = rec_parse(&rec,lbuf,ll)) >= 0) {
	                            cchar	*sp = rec.userstr.sp ;
	                            cint	sl = rec.userstr.sl ;
	                            if (strwcmp(user,sp,sl) == 0) {
					auto	uad_pc = useraccdb_recproc ;
	                                if ((rs = uad_pc(op,&rec)) >= 0) {
	                                    rs = entry_load(ep,ebuf,elen,&rec) ;
			                }
	                                break ;
	                            }
	                        }
	                        if (rs < 0) break ;
	                    } /* end while (reading lines) */
	                    if ((rs >= 0) && (rs1 <= 0)) {
	                        rs = (rs1 < 0) ? rs1 : SR_NOTFOUND ;
	                    }
	                    rs1 = filer_finish(&b) ;
	                    if (rs >= 0) rs = rs1 ;
	                } /* end if (filer) */
		        rs1 = lb.finish ;
		        if (rs >= 0) rs = rs1 ;
		    } /* end if (linebuffer) */
	            rs1 = useraccdb_lock(op,false) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (lock) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (useraccdb_find) */

static int useraccdb_updater(UAD *,UPINFO *) noex ;

int useraccdb_update(UAD *op,cchar *user,cchar *name) noex {
	int		rs ;
	int		rs1 ;
	int		rv = 0 ;
	if ((rs = useraccdb_magic(op,user)) >= 0) {
	    rs = SR_INVALID ;
	    if (op->eo < 0) {
		UPINFO	ui ;
		if ((rs = upinfo_start(&ui,op,user,name)) >= 0) {
	    	    if ((rs = useraccdb_openlock(op)) >= 0) {
			if ((rs = useraccdb_updater(op,&ui)) >= 0) {
			    rv = rs ;
			    rs1 = upinfo_update(&ui) ;
			    if (rs >= 0) rs = rs1 ;
		        } /* end if (updater) */
	    		rs1 = useraccdb_lock(op,false) ;
	    		if (rs >= 0) rs = rs1 ;
	            } /* end if (lock) */
	            rs1 = upinfo_finish(&ui) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (upinfo) */
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? rv : rs ;
}
/* end subroutine (useraccdb_update) */

int useraccdb_updater(UAD *op,UPINFO *uip) noex {
	int		rs ;
	int		rs1 ;
	char		*lbuf{} ;
	if ((rs = malloc_ml(&lbuf)) >= 0) {
	    filer	b ;
	    cint	llen = rs ;
	    if ((rs = filer_start(&b,op->fd,0L,0,0)) >= 0) {
		UAD_REC	rec ;
		off_t	ro = 0z ;
		while ((rs = filer_readln(&b,lbuf,llen,-1)) > 0) {
		    cint	ll = rs ;
	            if ((rs = rec_parse(&rec,lbuf,ll)) >= 0) {
			rs = upinfo_match(uip,ro,&rec) ;
			if (rs > 0) break ;
		    }
		    if (rs < 0) break ;
		    ro += ll ;
		} /* end while (reading lines) */
		rs1 = filer_finish(&b) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (filer) */
	    rs1 = uc_free(lbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end subroutine (useraccdb_updater) */

int useraccdb_curbegin(UAD *op,UAD_CUR *curp) noex {
	int		rs ;
	if ((rs = useraccdb_magic(op,curp)) >= 0) {
	    memclear(curp) ;
	    curp->eo = -1 ;
	    op->eo = 0 ;
	    rs = useraccdb_openlock(op) ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (useraccdb_curbegin) */

int useraccdb_curend(UAD *op,UAD_CUR *curp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = useraccdb_magic(op,curp)) >= 0) {
	    if (curp->eo >= 0) {
	        curp->eo = -1 ;
	        rs1 = filer_finish(&curp->b) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
	        rs1 = useraccdb_lock(op,false) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->eo = -1 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (useraccdb_curend) */

namespace {
    struct sub_enum {
	UAD		*op ;
	UAD_CUR		*curp ;
	UAD_ENT		*ep ;
	char		*ebuf ;
	int		elen ;
	sub_enum(UAD *o,UAD_CUR *c,UAD_ENT *e,char *b,int l) noex {
	    op = o ;
	    curp = c ;
	    ep = e ;
	    ebuf = b ;
	    elen = l ;
	} ;
	operator int () noex {
	    int		rs ;
	    if ((rs = prelude()) >= 0) {
		rs = read() ;
	    }
	    return rs ;
	} ;
	int prelude() noex {
	    int		rs = SR_OK ;
	    if (op->fd >= 0) {
		sub_enum	so(op,curp,ep,ebuf,elen) ;
		rs = so ;
	    }
	    return rs ;
	} ;
 	int read() noex {
	    off_t	eo = curp->eo ;
	    int		rs ;
	    int		rs1 ;
	    char	*lbuf{} ;
	    if ((rs = malloc_ml(&lbuf)) >= 0) {
		cint	llen = rs ;
	        if ((rs = filer_readln(&curp->b,lbuf,llen,-1)) >= 0) {
		    cint	ll = rs ;
	            if (ll > 0) {
	                UAD_REC		rec ;
	                if ((rs = rec_parse(&rec,lbuf,ll)) >= 0) {
	                    if ((rs = useraccdb_recproc(op,&rec)) >= 0) {
	                        rs = entry_load(ep,ebuf,elen,&rec) ;
			    }
		        }
	            } else {
	                rs = SR_NOTFOUND ;
		    }
	            if (rs >= 0) {
	                curp->eo = (eo + ll) ;
		    }
	        } /* end if (filer_readln) */
		rs1 = uc_free(lbuf) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	    return rs ;
	} ;
    } ; /* end struct (sub_enum) */
}

int useraccdb_enum(UAD *op,UAD_CUR *curp,UAD_ENT *ep,char *ebuf,int elen) noex {
	int		rs ;
	if ((rs = useraccdb_magic(op,curp,ep,ebuf)) >= 0) {
	    rs = SR_INVALID ;
	    if (op->fd >= 0) {
		sub_enum	so(op,curp,ep,ebuf,elen) ;
		rs = so ;
	    }
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (useraccdb_enum) */

int useraccdb_check(UAD *op,time_t ti_now) noex {
	int		rs ;
	int		f_changed = false ;
	if ((rs = useraccdb_magic(op)) >= 0) {
	    if (ti_now == 0) ti_now = time(nullptr) ;
	    if (ti_now == 1) f_changed = true ; /* fake out LINT */
	} /* end if (magic) */
	return (rs >= 0) ? f_changed : rs ;
}
/* end subroutine (useraccdb_check) */


/* private subroutines */

static int useraccdb_openlock(UAD *op) noex {
	int		rs = SR_OK ;
	if (op->fd < 0) {
	    rs = useraccdb_fileopen(op) ;
	}
	if (rs >= 0) {
	    rs = useraccdb_lock(op,true) ;
	}
	return rs ;
}
/* end subroutine (useraccdb_openlock) */

static int useraccdb_fileopen(UAD *op) noex {
	int		rs = SR_OK ;
	if (op->fd < 0) {
	    cmode	om = 0666 ;
	    cint	of = (O_RDWR | O_CREAT) ;
	    if ((rs = uc_open(op->fname,of,om)) >= 0) {
	        op->fd = rs ;
	        if ((rs = uc_closeonexec(op->fd,true)) >= 0) {
		    USTAT	sb ;
		    if ((rs = u_fstat(op->fd,&sb)) >= 0) {
		        op->dev = sb.st_dev ;
		        op->ino = sb.st_ino ;
		        op->ti_mod = sb.st_mtime ;
		    }
	        } /* end if (close-on-exec) */
	        if (rs < 0) {
		    u_close(op->fd) ;
		    op->fd = -1 ;
	        }
	    } /* end if (open) */
	} /* end if (open was needed) */
	return rs ;
}
/* end subroutine (useraccdb_fileopen) */

static int useraccdb_fileclose(UAD *op) noex {
	int		rs = SR_OK ;
	if (op->fd >= 0) {
	    rs = u_close(op->fd) ;
	    op->fd = -1 ;
	}
	return rs ;
}
/* end subroutine (useraccdb_fileclose) */

static int useraccdb_lock(UAD *op,int f) noex {
	int		rs = SR_OK ;
	if (f) {
	    if (! op->f.locked) {
	        rs = u_rewind(op->fd) ;
	        if (rs >= 0) {
	            rs = uc_lockf(op->fd,F_LOCK,0L) ;
	            if (rs >= 0) op->f.locked = true ;
	        }
	    }
	} else {
	    if (op->f.locked) {
	        rs = u_rewind(op->fd) ;
	        if (rs >= 0) {
	            op->f.locked = false ;
	            rs = uc_lockf(op->fd,F_UNLOCK,0L) ;
	        }
	    }
	} /* end if */
	return rs ;
}
/* end subroutine (useraccdb_lock) */

static int useraccdb_recproc(UAD *op,UAD_REC *recp) noex {
	int		rs = SR_OK ;
	int		sl = recp->datestr.sl ;
	cchar		*sp = recp->datestr.sp ;
	if (sp) {
	    rs = useraccdb_doit(op,&recp->atime,sp,sl) ;
	} else {
	    rs = SR_INVALID ;
	}
	if (rs >= 0) {
	    sl = recp->countstr.sl ;
	    sp = recp->countstr.sp ;
	    if (sp != nullptr) {
	        rs = cfdecui(sp,sl,&recp->count) ;
	    } else {
	        rs = SR_INVALID ;
	    }
	}
	return rs ;
}
/* end subroutine (useraccdb_recproc) */

static int useraccdb_doit(UAD *op,time_t *timep,cc *tsp,int tsl) noex {
	int		rs = SR_OK ;
	if (! op->f.dater) {
	    TIMEB	now ;
	    cint	zlen = DATER_ZNAMELEN ;
	    char	zbuf[DATER_ZNAMELEN+1] ;
	    if ((rs = initnow(&now,zbuf,zlen)) >= 0) {
	        rs = dater_start(op->dmp,&now,zbuf,zlen) ;
	        op->f.dater = (rs >= 0) ;
	    }
	}
	if (rs >= 0) {
	    rs = dater_setlogz(op->dmp,tsp,tsl) ;
	}
	if (rs >= 0) {
	    rs = dater_gettime(op->dmp,timep) ;
	}
	return rs ;
}
/* end subroutine (useraccdb_doit) */

static int upinfo_start(UPINFO *uip,UAD *op,cc *au,cc *an) noex {
	memclear(uip) ;
	uip->op = op ;
	uip->utime = time(nullptr) ;
	uip->arguser = au ;
	uip->argname = an ;
	uip->utime = time(nullptr) ;
	return mkts(uip->tbuf,UAFILE_LDATE,uip->utime) ;
}
/* end subroutine (upinfo_start) */

static int upinfo_finish(UPINFO *uip) noex {
	int		rs = SR_FAULT ;
	if (uip) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (upinfo_finish) */

static int upinfo_match(UPINFO *uip,off_t ro,UAD_REC *recp) noex {
	UPINFO_REC	*urp = nullptr ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		sl = recp->userstr.sl ;
	int		rc = 0 ;
	cchar		*sp = recp->userstr.sp ;
	if (sp) {
	    if (strwcmp(uip->arguser,sp,sl) == 0) {
	        urp = &uip->user ;
	    } else if (strwcmp(totaluser,sp,sl) == 0) {
	        urp = &uip->total ;
	    } /* end if */
	    if ((urp != nullptr) && (! urp->found)) {
	        urp->found = true ;
	        urp->ro = ro ;
	        urp->count = 0 ;
	        sl = recp->countstr.sl ;
	        sp = recp->countstr.sp ;
	        if (sp != nullptr) {
	            uint	c ;
	            rs1 = cfdecui(sp,sl,&c) ;
	            if (rs1 >= 0) urp->count = c ;
	        }
	    }
	    rc = (uip->user.found && uip->total.found) ;
	} /* end if (non-null) */
	return (rs >= 0) ? rc : rs ;
}
/* end subroutine (upinfo_match) */

static int upinfo_update(UPINFO	 *uip) noex {
	UPINFO_REC	*urp ;
	int		rs = SR_OK ;
	if (rs >= 0) {
	    urp = &uip->total ;
	    rs = upinfo_upone(uip,urp,0) ;
	}
	if (rs >= 0) {
	    urp = &uip->user ;
	    rs = upinfo_upone(uip,urp,1) ;
	}
	return rs ;
}
/* end subroutine (upinfo_update) */

static int upinfo_upone(UPINFO *uip,UPINFO_REC *urp,int type) noex {
	cint		rlen = UAFILE_RECLEN ;
	int		rs ;
	char		rbuf[UAFILE_RECLEN+1] ;
	if ((rs = upinfo_mkrec(uip,urp,rbuf,rlen,type)) >= 0) {
	    USERACCDB	*op = uip->op ;
	    int		rl = rs ;
	    if (urp->found) {
	        rs = u_pwrite(op->fd,rbuf,rl,urp->ro) ;
	    } else {
	        if ((rs = u_seek(op->fd,0L,SEEK_END)) >= 0) {
	            rs = u_write(op->fd,rbuf,rl) ;
		}
	    }
	} /* end if */
	return rs ;
}
/* end subroutine (upinfo_upone) */

static int upinfo_mkrec(UPINFO *uip,UPINFO_REC *urp,char *rbuf,int rlen,
		int type) noex {
	int		rs = SR_OVERFLOW ;
	int		rl = 0 ;
	if (rlen <= UAFILE_RECLEN) {
	    int		dbl ;
	    char	digbuf[diglen +1] ;
	    char	*dbp = digbuf ;
	    char	*rbp = rbuf ;
	    if ((rs = ctdecui(digbuf,diglen,(urp->count+1))) >= 0) {
	        cint	n = (UAFILE_LCOUNT - rs) ;
	        dbl = rs ;
	        if (dbl > UAFILE_LCOUNT) { /* truncate from left as necessary */
	            int	dld = (dbl-UAFILE_LCOUNT) ;
	            dbp += dld ;
	            dbl -= dld ;
	        }
	        if (n > 0) rbp = strnset(rbp,' ',n) ;
	        rbp = strwcpy(rbp,dbp,dbl) ;
	    }
	    if (rs >= 0) {
	        *rbp++ = ' ' ;
	    }
	    if (rs >= 0) {
	        rbp = strwcpy(rbp,uip->tbuf,UAFILE_LDATE) ;
	        rl = (rbp - rbuf) ;
	    }
	    if ((rs >= 0) && (! urp->found)) {
	        cchar	*up = (type) ? uip->arguser : totaluser ;
	        *rbp++ = ' ' ;
	        rbp = strwcpy(rbp,up,UAFILE_MAXUSERLEN) ;
	        if (type && (uip->argname != nullptr)) {
	            *rbp++ = ' ' ;
	            *rbp++ = CH_LPAREN ;
	            rbp = strwcpy(rbp,uip->argname,UAFILE_MAXNAMELEN) ;
	            *rbp++ = CH_RPAREN ;
	        }
	        *rbp++ = '\n' ;
	        rl = (rbp - rbuf) ;
	    }
	} /* end if (valid) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (upinfo_mkrec) */

static int rec_parse(UAD_REC *recp,cchar *lp,int ll) noex {
	int		rs = SR_OK ;
	int		cl ;
	cchar	*tp, *cp ;
	memclear(recp) ;
#ifdef	COMMENT
	cl = sfnext(lp,ll,&cp) ;
	recp->countstr.sp = cp ;
	recp->countstr.sl = cl ;
	ll = ((lp+ll)-(cp+cl)) ;
	lp = (cp+cl) ;
	cl = sfnext(lp,ll,&cp) ;
	recp->datestr.sp = cp ;
	recp->datestr.sl = cl ;
	ll = ((lp+ll)-(cp+cl)) ;
	lp = (cp+cl) ;
#else
	recp->countstr.sp = lp ;
	recp->countstr.sl = UAFILE_LCOUNT ;
	lp += (UAFILE_LCOUNT + 1) ;
	ll -= (UAFILE_LCOUNT + 1) ;
	recp->datestr.sp = lp ;
	recp->datestr.sl = UAFILE_LDATE ;
	lp += (UAFILE_LDATE + 1) ;
	ll -= (UAFILE_LDATE + 1) ;
#endif /* COMMENT */
	cl = sfnext(lp,ll,&cp) ;
	recp->userstr.sp = cp ;
	recp->userstr.sl = MIN(cl,UAFILE_MAXUSERLEN) ;
	ll = ((lp+ll)-(cp+cl)) ;
	lp = (cp+cl) ;
	if ((tp = strnchr(lp,ll,CH_LPAREN)) != nullptr) {
	    ll = ((lp+ll)-(tp+1)) ;
	    lp = (tp+1) ;
	    cp = lp ;
	    if ((tp = strnchr(lp,ll,CH_RPAREN)) != nullptr) {
	        cl = (tp-lp) ;
	        recp->namestr.sp = cp ;
	        recp->namestr.sl = MIN(cl,UAFILE_MAXNAMELEN) ;
	    }
	}
	return rs ;
}
/* end subroutine (rec_parse) */

static int entry_load(UAD_ENT *ep,char *ebuf,int elen,
		UAD_REC *recp) noex {
	storeitem	s ;
	int		rs ;
	int		rs1 ;
	memclear(ep) ;
	if ((rs = storeitem_start(&s,ebuf,elen)) >= 0) {
	    UAD_ITEM	*ip ;
	    cchar	*cp ;
	    ep->atime = recp->atime ;
	    ep->count = recp->count ;
	    ip = &recp->userstr ;
	    rs = storeitem_strw(&s,ip->sp,ip->sl,&cp) ;
	    ep->user = cp ;
	    if (rs >= 0) {
	        ip = &recp->namestr ;
	        rs = storeitem_strw(&s,ip->sp,ip->sl,&cp) ;
	        ep->name = cp ;
	    }
	    rs1 = storeitem_finish(&s) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (storeitem) */
	return rs ;
}
/* end subroutine (entry_load) */

static int mkts(char *tbuf,int tlen,time_t t) noex {
	int		tl = 0 ;
	timestr_logz(t,tbuf) ;
	tl = strlen(tbuf) ;
	if (tl < tlen) {
	    char	*bp = (tbuf + tl) ;
	    int		bl = (tlen - tl) ;
	    strnset(bp,' ',bl) ;
	}
	return tl ;
}
/* end subroutine (mkts) */


