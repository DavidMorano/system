/* vecstrx_loadgrusers SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* find and load UNIX® users who have the given group as their default */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-01-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	vecstrx_loadgrusers

	Description:
	This subroutine finds all users who have the given specified
	group as their default group.

	Synopsis:
	int vecstrx_loadgrusers(vecstrx *,gid_t) noex

	Returns:
	>=0	ok
	<0	error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/mman.h>
#include	<unistd.h>
#include	<fcntl.h>		/* |O_{xx}| */
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<strn.h>
#include	<cfdec.h>
#include	<ascii.h>
#include	<numsign.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"vecstrx.hh"


/* local defines */

#ifndef	SYSPASSWD
#define	SYSPASSWD	"/sysdb/passwd"
#endif


/* external subroutines */


/* local structures */

namespace {
   struct subinfo {
	vecstrx		*vsp ;
	void		*mapdata{} ;
	size_t		mapsize ;
	size_t		fsize ;
	gid_t		sgid ;
	int	start(vecstrx *,gid_t) noex ;
	int	pwmapbegin() noex ;
	int	pwmapload() noex ;
	int	pwmapend() noex ;
	int	finish() noex ;
    } ; /* end struct (subinfo) */
}


/* forward references */


static int	pwentparse(cchar *,int,gid_t *) noex ;


/* local template-subroutines */


/* local variables */


/* local subroutines */


/* exported variables */


/* exported subroutines */

int vecstrx::loadgrusers(gid_t sgid) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		c = 0 ;
	if (this) {
	    if (numsign(sgid)) sgid = getgid() ;
	    if (subinfo si ; (rs = si.start(this,sgid)) >= 0) {
		{
	            rs = si.pwmapload() ;
	            c = rs ;
		}
	        rs1 = si.finish() ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (subinfo) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecstrx_loadgrusers) */


/* local subroutines */

int subinfo::start(vecstrx *p,gid_t g) noex {
    	int		rs = SR_BUGCHECK ;
	if (vsp) {
	    rs = SR_OK ;
	    vsp = p ;
	    sgid = g ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (subinfo::start) */

int subinfo::finish() noex {
	int		rs = SR_FAULT ;
	if (this) {
	    rs = SR_OK ;
	}
	return rs ;
}
/* end subroutine (subinfo::finish) */

int subinfo::pwmapload() noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if ((rs = pwmapbegin()) >= 0) {
	    int		ml = intconv(fsize) ;
	    cchar	*mp = charp(mapdata) ;
	    cchar	*tp ;
	    while ((tp = strnchr(mp,ml,CH_NL)) != nullptr) {
	        cint	len = intconv(tp - mp) ;
	        if (gid_t gid{} ; (rs = pwentparse(mp,len,&gid)) > 0) {
		    cint	ul = rs ;
		    if (sgid == gid) {
	                c += 1 ;
	                rs = vsp->adduniq(mp,ul) ;
	            }
	        } /* end if (pwentparse) */
	        ml -= intconv((tp + 1) - mp) ;
	        mp = (tp + 1) ;
	        if (rs < 0) break ;
	    } /* end while (reading lines) */
	    rs1 = pwmapend() ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (grmems-pwmap) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (subinfo::pwmapload) */

int subinfo::pwmapbegin() noex {
	cnullptr	np{} ;
	int		rs = SR_OK ;
	int		rs1 ;
	if (mapdata == nullptr) {
	    cmode	om = 0666 ;
	    cint	of = O_RDONLY ;
	    cchar	*fn = SYSPASSWD ;
	    if ((rs = u_open(fn,of,om)) >= 0) {
	        cint	fd = rs ;
	        if ((rs = u_fsize(fd)) >= 0) {
	            csize	ms = size_t(rs) ;
	            int		mp = PROT_READ ;
	            int		mf = MAP_SHARED ;
	            fsize = rs ;
	            if (void *md ; (rs = u_mmap(np,ms,mp,mf,fd,0z,&md)) >= 0) {
	                cint		madv = MADV_SEQUENTIAL ;
			const caddr_t	ma = caddr_t(md) ;
	                if ((rs = u_madvise(ma,ms,madv)) >= 0) {
	                    mapdata = md ;
	                    mapsize = ms ;
	                } /* end if (advise) */
	                if (rs < 0) {
	                    u_munmap(md,ms) ;
			}
	            } /* end if (mmap) */
	        } /* end if (file-size) */
	        rs1 = u_close(fd) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (file-open) */
	} /* end if (need mapping) */
	return rs ;
}
/* end subroutine (subinfo::pwmapbegin) */

int subinfo::pwmapend() noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (mapdata) {
	    csize	ms = mapsize ;
	    void	*md = mapdata ;
	    rs1 = u_munmap(md,ms) ;
	    if (rs >= 0) rs = rs1 ;
	    mapdata = nullptr ;
	    mapsize = 0 ;
	}
	return rs ;
}
/* end subroutine (subinfo::pwmapend) */

/* PASSWD entry parsing */
static int pwentparse(cchar *lbuf,int llen,gid_t *gp) noex {
	int		rs = SR_OK ;
	int		fi{} ;		/* <- used afterwards */
	int		ll = llen ;
	int		ul = 0 ;
	cchar		*lp = lbuf ;
	for (fi = 0 ; fi < 4 ; fi += 1) {
	    if (cchar *tp ; (tp = strnchr(lp,ll,':')) != nullptr) {
	        switch (fi) {
	        case 0:
	            ul = intconv(tp - lp) ;
	            break ;
	        case 3:
	            {
			cint	tl = intconv(tp - lp) ;
	                if (int v{} ; (rs = cfdeci(lp,tl,&v)) >= 0) {
	                    *gp = gid_t(v) ;
	                } else if (isNotValid(rs)) {
			    rs = SR_OK ;
	                    ul = 0 ;
			}
	            } /* end block */
	            break ;
	        } /* end switch */
	        ll -= intconv((tp+1)-lp) ;
	        lp = (tp+1) ;
	    } /* end if (had separator) */
	} /* end for (looping through fields) */
	if (fi < 4) ul = 0 ;
	return (rs >= 0) ? ul : rs ;
}
/* end subroutine (pwentparse) */


