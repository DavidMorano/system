/* finduid SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* find a username given a UID by various means */
/* version %I% last-modified %G% */

#define	CF_TMPXEARLY	0		/* open TMPX early */

/* revision history:

	= 2004-01-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	finduid_lookup

	Description:
	This object is used to find a username by a given UID.  This
	object is thread-safe (safe to use in a multi-threaded
	environment).

	Synopsis:
	int finduid_lookup(finduid *op,char *ubuf,int ulen,uid_t uid) noex

	Arguments:
	op	object pointer
	ubuf	result buffer pointer
	ulen	result buffer length
	uid	UID to lookup

	Returns:
	>0	found
	==0	not-found
	<0	error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<mallocxx.h>
#include	<getpwd.h>
#include	<snx.h>
#include	<mkpath.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"finduid.h"


/* local defines */

#define	INTUTOPEN	(1*60*60)
#define	INTUTCHECK	(5*60)

#ifndef	CF_TMPXEARLY
#define	CF_TMPXEARLY	0		/* open TMPX early */
#endif


/* imported namespaces */

using std::nullptr_t ;			/* type */
using std::nothrow ;			/* constant */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static int finduid_ctor(finduid *op,Args ... args) noex {
    	FINDUID		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    cnullptr	np{} ;
	    rs = SR_NOMEM ;
	    memclear(hop) ;
	    if ((op->mxp = new(nothrow) ptm) != np) {
	        if ((op->utp = new(nothrow) tmpx) != np) {
	            if ((op->ucp = new(nothrow) pwcache) != np) {
			rs = SR_OK ;
		    } /* end if (new-pwcache) */
		    if (rs < 0) {
		        delete op->utp ;
		        op->utp = nullptr ;
		    }
	        } /* end if (new-tmpx) */
		if (rs < 0) {
		    delete op->mxp ;
		    op->mxp = nullptr ;
		}
	    } /* end if (new-ptm) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (finduid_ctor) */

static int finduid_dtor(finduid *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	    if (op->ucp) {
		delete op->ucp ;
		op->ucp = nullptr ;
	    }
	    if (op->utp) {
		delete op->utp ;
		op->utp = nullptr ;
	    }
	    if (op->mxp) {
		delete op->mxp ;
		op->mxp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (finduid_dtor) */

template<typename ... Args>
static int finduid_magic(finduid *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == FINDUID_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (finduid_magic) */

static int	finduid_lookuper(finduid *,char *,int,uid_t) noex ;
static int	finduid_tmpxopen(finduid *) noex ;
static int	finduid_tmpxclose(finduid *) noex ;


/* local variables */

constexpr bool	f_tmpxearly = CF_TMPXEARLY ;


/* exported variables */


/* exported subroutines */

int finduid_start(finduid *op,int nmax,int ttl) noex {
	int		rs ;
	if (nmax < FINDUID_DEFMAX) nmax = FINDUID_DEFMAX ;
	if (ttl < FINDUID_DEFTTL) ttl = FINDUID_DEFTTL ;
	if ((rs = finduid_ctor(op)) >= 0) {
	    if ((rs = ptm_create(op->mxp,nullptr)) >= 0) {
		if ((rs = pwcache_start(op->ucp,nmax,ttl)) >= 0) {
		    if_constexpr (f_tmpxearly) {
		        rs = finduid_tmpxopen(op) ;
		    } /* end if_constexpr (f_tmpxearly) */
		    if (rs >= 0) {
			op->nmax = nmax ;
			op->ttl = ttl ;
			op->magic = FINDUID_MAGIC ;
		    }
		}
		if (rs < 0) {
		    ptm_destroy(op->mxp) ;
		}
	    } /* end if (ptm-create) */
	    if (rs < 0) {
		finduid_dtor(op) ;
	    }
	} /* end if (finduid_ctor) */
	return rs ;
}
/* end subroutine (finduid_start) */

int finduid_finish(finduid *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = finduid_magic(op)) >= 0) {
	    if (op->open.ut) {
	        op->open.ut = false ;
	        rs1 = finduid_tmpxclose(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->ucp) {
	        rs1 = pwcache_finish(op->ucp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    if (op->mxp) {
	        rs1 = ptm_destroy(op->mxp) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    {
		rs1 = finduid_dtor(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (finduid_finish) */

int finduid_lookup(finduid *op,char *ubuf,int ulen,uid_t uid) noex {
	int		rs ;
	int		rs1 ;
	int		ul = 0 ;
	if ((rs = finduid_magic(op,ubuf)) >= 0) {
	    op->s.total += 1 ;
	    ubuf[0] = '\0' ;
	    if ((rs = ptm_lock(op->mxp)) >= 0) {
	        if (! op->open.ut) {
		    rs = finduid_tmpxopen(op) ;
	        }
	        if (rs >= 0) {
		    rs = finduid_lookuper(op,ubuf,ulen,uid) ;
		    ul = rs ;
	        } /* end if (ok) */
	        rs1 = ptm_unlock(op->mxp) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ptm) */
	} /* end if (magic) */
	return (rs >= 0) ? ul : rs ;
}
/* end subroutine (finduid_lookup) */

int finduid_check(finduid *op,time_t dt) noex {
	int		rs ;
	int		rs1 ;
	int		f = false ;
	if ((rs = finduid_magic(op)) >= 0) {
	    if (dt == 0) dt = getustime ;
	    if ((rs = ptm_lock(op->mxp)) >= 0) {
	        if (op->open.ut) {
	            if ((dt - op->ti_utopen) >= INTUTOPEN) {
	                rs = finduid_tmpxclose(op) ;
		    }
	        }
	        if ((rs >= 0) && op->open.ut) {
	            if ((dt - op->ti_utcheck) >= INTUTCHECK) {
	                op->ti_utcheck = dt ;
	                rs = tmpx_check(op->utp,dt) ;
	                f = (rs > 0) ;
	            }
	        }
	        rs1 = ptm_unlock(op->mxp) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (ptm) */
	} /* end if (magic) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (finduid_check) */

int finduid_stats(finduid *op,finduid_st *sp) noex {
	int		rs ;
	if ((rs = finduid_magic(op,sp)) >= 0) {
	    *sp = op->s ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (finduid_stats) */


/* private subroutines */

static int finduid_lookuper(finduid *op,char *ubuf,int ulen,uid_t uid) noex {
	int		rs ;
	int		rs1 = SR_OK ;
	int		ul = 0 ;
	if (char *pwbuf{} ; (rs = malloc_pw(&pwbuf)) >= 0) {
	    cint	utype = TMPX_TPROCUSER ;
	    int		pwlen = rs ;
	    if (tmpx_cur uc{} ; (rs = tmpx_curbegin(op->utp,&uc)) >= 0) {
	    	tmpx_ent	ue ;
		pwcache		*pwc = op->ucp ;
		ucentpw		pw ;
		time_t		ti_create = 0 ;
		time_t		ut ;
	        while (rs >= 0) { /* loop finding latest */
		    rs1 = tmpx_curenum(op->utp,&uc,&ue) ;
		    if (rs1 == SR_NOTFOUND) break ;
		    ut = ue.ut_tv.tv_sec ;
	            if ((ue.ut_type == utype) && (ut > ti_create)) {
			auto	pwl = pwcache_lookup ;
		        if ((rs = sncpy(ubuf,ulen,ue.ut_user)) >= 0) {
			    cint	tl = rs ;
		            if ((rs = pwl(pwc,&pw,pwbuf,pwlen,ubuf)) >= 0) {
				if (pw.pw_uid == uid) {
				    ti_create = ut ;
			            ul = tl ;	/* <- set found */
			        }
		            } else if (rs == SR_NOTFOUND) {
				rs = SR_OK ;
			    }
			} /* end if (sncpy) */
		    } /* end if (got a user process) */
	        } /* end while (finding latest entry) */
	        if ((rs >= 0) && (rs1 != SR_NOTFOUND)) rs = rs1 ;
		rs1 = tmpx_curend(op->utp,&uc) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (TMPX cursor) */
	    rs1 = uc_free(pwbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? ul : rs ;
}
/* end subroutine (finduid_lookuper) */

static int finduid_tmpxopen(finduid *op) noex {
    	cnullptr	np{} ;
	int		rs = SR_OK ;
	if (! op->open.ut) {
	    cint	of = O_RDONLY ;
	    if ((rs = tmpx_open(op->utp,np,of)) >= 0) {
	        op->open.ut = true ;
	        op->ti_utopen = getustime ;
	    }
	}
	return rs ;
}
/* end subroutine (finduid_tmpxopen) */

static int finduid_tmpxclose(finduid *op) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (op->open.ut) {
	    op->open.ut = false ;
	    rs1 = tmpx_close(op->utp) ;
	    if (rs >= 0) rs = rs1 ;
	}
	return rs ;
}
/* end subroutine (finduid_tmpxclose) */


