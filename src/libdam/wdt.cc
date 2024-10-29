/* wdt SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* walk directory tree */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-17, David A­D­ Morano
	This was made to get around some 'ftw(3c)' limitations.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	wdt

	Description:
	This subroutine is similar to the |ftw(3c)| subroutine in
	that it "walks" a directory hierarchy.  Note that unlike
	|ftw(3c)| this subroutine is NOT recursive!

	Synopsis:
	typedef	int	(*wdt_f)(cchar *,USTAT *,void *) noex
	int wdt(cchar *basedir,int wopts,wdt_f auf,void *uarg) noex

	Arguments:
	basedir		directory at top of tree
	wopts		options for usage (FOLLOW links or not)
	auf		user function to call
	uarg		user argument (usually a pointer)

	Returns:
	>=0		OK
	<0		error (system-return)

	Name:
	checkname

	Description:
	This is the user-defined function that is called for each
	name encountered.

	Synopsis:
	int checkname(cchar *name,struct ustat *sbp,void *uarg)

	Arguments:
	name		name (string)
	sbp		pointer to STAT structure
	uarg		user-supplied argument

	Returns:
	>0		exit enumberation w/ count
	==0		continue
	<0		error and exit enumeration with given error

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<fifostr.h>
#include	<fsdir.h>
#include	<mkpathx.h>
#include	<pathadd.h>
#include	<hasx.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"wdt.h"


/* local defines */

#define	SI	subinfo


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct subinfo {
	wdt_f		uf ;
	void		*uarg ;
	fifostr		fs ;
	int		wopts ;
	bool		f_exit ;
} ;


/* forward references */

static int subinfo_start(SI *,int,wdt_f,void *) noex ;
static int subinfo_finish(SI *) noex ;
static int subinfo_procdir(SI *,char *,int) noex ;
static int subinfo_procdirents(SI *,char *,int) noex ;
static int subinfo_procname(SI *,cchar *,int) noex ;
static int subinfo_procout(SI *,cchar *,USTAT *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int wdt(cchar *basedir,int wopts,wdt_f auf,void *uarg) noex {
    	cint		rsn = SR_NOTFOUND ;
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (basedir && auf) {
	    rs = SR_INVALID ;
	    if (basedir[0]) {
	        SI	si, *sip = &si ;
	        if ((rs = subinfo_start(sip,wopts,auf,uarg)) >= 0) {
		    fifostr	*fsp = &sip->fs ;
	            if (char *dbuf{} ; (rs = malloc_mp(&dbuf)) >= 0) {
			cint	dlen = rs ;
	                if ((rs = mkpath1(dbuf,basedir)) >= 0) {
	                    if ((rs = subinfo_procdir(sip,dbuf,rs)) >= 0) {
	                        c += rs ;
	                        while ((rs >= 0) && (! sip->f_exit)) {
				    auto fsr = fifostr_rem ;
	                            if ((rs = fsr(fsp,dbuf,dlen)) >= 0) {
	                                rs = subinfo_procdir(sip,dbuf,rs) ;
	                                c += rs ;
				    } else if (rs == rsn) {
					rs = SR_OK ;
					break ;
	                            }
	                        } /* end while */
	                    } /* end if (subinfo_procdir) */
	                } /* end if (mkpath) */
		    	rs = rsfree(rs,dbuf) ;
		    } /* end if (m-a-f) */
	            rs1 = subinfo_finish(sip) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (subinfo) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (wdt) */


/* private subroutines */

static int subinfo_start(SI *sip,int wopts,wdt_f uf,void *uarg) noex {
	int		rs = SR_FAULT ;
	if (sip) {
	    memclear(sip) ;
	    sip->wopts = wopts ;
	    sip->uf = uf ;
	    sip->uarg = uarg ;
	    rs = fifostr_start(&sip->fs) ;
	}
	return rs ;
}
/* end subroutine (subinfo_start) */

static int subinfo_finish(SI *sip) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (sip) {
	    rs = SR_OK ;
	    {
	        rs1 = fifostr_finish(&sip->fs) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (subinfo_finish) */

static int subinfo_procdir(SI *sip,char *dbuf,int dlen) noex {
	int		rs ;
	int		c = 0 ;
	if ((rs = subinfo_procdirents(sip,dbuf,dlen)) >= 0) {
	    USTAT	sb ;
	    c += rs ;
	    if ((rs = uc_stat(dbuf,&sb)) >= 0) {
	        rs = subinfo_procout(sip,dbuf,&sb) ;
		c += rs ;
	    }
	} /* end if (subinfo_procdirents) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (subinfo_procdir) */

static int subinfo_procdirents(SI *sip,char *dbuf,int dlen) noex {
	int		rs ;
	int		rs1 ;
	int		c = 0 ;
	if (char *nbuf{} ; (rs = malloc_mn(&nbuf)) >= 0) {
	    fsdir	d ;
	    fsdir_ent	de ;
	    cint	nlen = rs ;
	    if ((rs = fsdir_open(&d,dbuf)) >= 0) {
	        while ((rs = fsdir_read(&d,&de,nbuf,nlen)) > 0) {
	            if (hasNotDots(de.name,-1)) {
	                if ((rs = pathadd(dbuf,dlen,de.name)) >= 0) {
	                    rs = subinfo_procname(sip,dbuf,rs) ;
	                    c += rs ;
	                }
	            }
	            if (sip->f_exit) break ;
	            if (rs < 0) break ;
	        } /* end while */
	        rs1 = fsdir_close(&d) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (fsdir) */
	    dbuf[dlen] = '\0' ;
	    rs1 = uc_free(nbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (subinfo_procdirents) */

static int subinfo_procname(SI *sip,cchar *dbuf,int dlen) noex {
	USTAT		sb ;
	int		rs ;
	if ((rs = lstat(dbuf,&sb)) >= 0) {
	    if (S_ISDIR(sb.st_mode)) {
	        rs = fifostr_add(&sip->fs,dbuf,dlen) ;
	    } else if (S_ISLNK(sb.st_mode)) {
	        if (sip->wopts & WDT_MFOLLOW) {
	            struct ustat	sbb ;
	            if ((rs = u_stat(dbuf,&sbb)) >= 0) {
	                if (S_ISDIR(sbb.st_mode)) {
	                    rs = fifostr_add(&sip->fs,dbuf,dlen) ;
	                } else {
	                    rs = subinfo_procout(sip,dbuf,&sbb) ;
	                }
	            } else if (isNotAccess(rs)) {
	                sb = {} ;
	                rs = subinfo_procout(sip,dbuf,&sb) ;
	            }
	        } else {
	            rs = subinfo_procout(sip,dbuf,&sb) ;
	        }
	    } else {
	            rs = subinfo_procout(sip,dbuf,&sb) ;
	    }
	} else if (rs == SR_NOENT) {
	    sb = {} ;
	    rs = subinfo_procout(sip,dbuf,&sb) ;
	}
	return rs ;
}
/* end subroutine (subinfo_procname) */

static int subinfo_procout(SI *sip,cchar *dbuf,USTAT *sbp) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	if (sip->uf) {
	    wdt_f	uf = wdt_f(sip->uf) ;
	    if ((rs = uf(dbuf,sbp,sip->uarg)) >= 0) {
		c = 1 ;
	        if (rs > 0) sip->f_exit = true ;
	    }
	}
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (subinfo_procout) */


