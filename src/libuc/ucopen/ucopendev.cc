/* ucopendev SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* open special overlay mount under the '/dev' directory*/
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-10, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	uc_opendev

	Description:
	This subroutine attempts to open a special overlay mounted
	resource of some kind under the '/dev' directory.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucopen.h>		/* LIBUC */
#include	<ucdesc.h>		/* LIBUC |uc_closeonexec(3uc)| */
#include	<getx.h>		/* LIBUC */
#include	<opensysfs.hh>		/* LIBUC */
#include	<mkpathx.h>		/* LIBUC */
#include	<strn.h>		/* LIBUC |strnchr(3c)| */
#include	<strwcpy.h>		/* LIBUC */
#include	<matstr.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"ucopeninfo.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	OPENDEV_DEVDNAME	"/dev"


/* imported namespaces */

using libuc::opensysfs ;		/* subroutine */
using libuc::libmem ;			/* variable */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	dialtcp(cc *,cchar *,int,int,int) noex ;
    extern int	dialtcpnls(cc *,cchar *,int,cchar *,int,int) noex ;
    extern int	dialtcpmux(cc *,cchar *,int,cchar *,cchar **,int,int) noex ;
    extern int	dialudp(cc *,cchar *,int,int,int) noex ;
} /* end extern (C) */


/* local structures */

struct argitem {
	cchar		*p ;
	int		l ;
} ; /* end struct */

enum das {
	da_proto,
	da_af,
	da_host,
	da_svc,
	da_overlast
} ; /* end struct */

struct inetargs {
	argitem		ia[da_overlast] ;
	char		*a ;		/* memory allocation */
} ; /* end struct */


/* forward references */

local int opendev_default(cchar *,int,mode_t) noex ;
local int opendev_inet(int,cchar *,int,int,int) noex ;

local int inetargs_start(inetargs *,cchar *,int) noex ;
local int inetargs_finish(inetargs *) noex ;


/* local variables */

enum devnames {
	devname_users,
	devname_groups,
	devname_projects,
	devname_tcp,
	devname_udp,
	devname_overlast
} ; /* end enum */

constexpr cpcchar		subnames[] = {
	"users",
	"groups",
	"projects",
	"tcp",
	"udp",
	nullptr
} ; /* end array */

constexpr opensysdbs		whiches[] = {
	opensysdb_usernames,
	opensysdb_groupnames,
	opensysdb_projectnames
} ; /* end array */


/* exported variables */


/* exported subroutines */

int uc_opendev(cchar *fname,int of,mode_t om,mainv envv,int to,int opts) noex {
	int		rs = SR_FAULT ;
	int		fd = -1 ; /* return-value */
	(void) envv ;
	(void) opts ;
	if (fname) ylikely {
	    rs = SR_INVALID ;
	    if (fname[0]) ylikely {
		int fl = -1 ;
		bool f_more = false ;
	        while (fname[0] == '/') fname += 1 ;
	        if (cchar *tp = strchr(fname,'/') ; tp) {
	            fl = intconv(tp - fname) ;
	            while (fl && (fname[fl-1] == '/')) fl -= 1 ;
	            while (tp[0] == '/') tp += 1 ;
	            f_more = (tp[0] != '\0') ;
	        } /* end if */
	        if (int fi ; (fi = matstr(subnames,fname,fl)) >= 0) ylikely {
	            switch (fi) {
	            case devname_users:
	            case devname_groups:
	            case devname_projects:
		        {
	                    opensysdbs	w = whiches[fi] ;
	                    rs = opensysfs(w,of,-1) ;
			    fd = rs ;
		        }
		        break ;
	            case devname_tcp:
	            case devname_udp:
		        {
		            if (f_more) {
		                rs = opendev_inet(fi,fname,of,to,opts) ;
			        fd = rs ;
		            } else {
			        rs = opendev_default(fname,of,om) ;
			        fd = rs ;
		            }
		        }
		        break ;
	            default:
		        rs = opendev_default(fname,of,om) ;
			fd = rs ;
		        break ;
	            } /* end switch */
	        } else {
	            rs = opendev_default(fname,of,om) ;
		    fd = rs ;
	        }
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? fd : rs ;;
} /* end subroutine (uc_opendev) */


/* private subroutines */

local int opendev_default(cchar *fname,int of,mode_t om) noex {
	int		rs ;
	int		rs1 ;
	int		sz = 0 ;
	int		fd = -1 ;
	cchar		*devdname = OPENDEV_DEVDNAME ;
	sz += intconv(lenstr(devdname) + 1) ;
	sz += 1 ;
	sz += intconv(lenstr(fname) + 1) ;
	sz += 1 ;
	if (char *fnbuf ; (rs = lm_mall(sz,&fnbuf)) >= 0) ylikely {
	    if ((rs = mkpath2(fnbuf,devdname,fname)) >= 0) ylikely {
	        rs = u_open(fnbuf,of,om) ;
		fd = rs ;
	    }
	    rs1 = lm_free(fnbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (memory-acquire) */
	if ((rs < 0) && (fd >= 0)) u_close(fd) ;
	return (rs >= 0) ? fd : rs ;
} /* end subroutine (opendev_default) */

local int opendev_inet(int fi,cchar *fname,int of,int to,int ne) noex {
	inetargs	ia ;
	int		rs ;
	int		fd = -1 ;
	if ((rs = inetargs_start(&ia,fname,-1)) >= 0) ylikely {
	    cchar	*a = ia.ia[da_af].p ;
	    cchar	*h = ia.ia[da_host].p ;
	    cchar	*s = ia.ia[da_svc].p ;
	    if ((h != nullptr) && (h[0] != '\0')) {
	        int	af = AF_UNSPEC ;
		if ((a != nullptr) && (a[0] != '\0')) af = getaf(a) ;
		switch (fi) {
	        case devname_tcp:
	            rs = dialtcp(h,s,af,to,ne) ;
		    fd = rs ;
		    break ;
	        case devname_udp:
	            rs = dialudp(h,s,af,to,ne) ;
		    fd = rs ;
		    break ;
		} /* end switch */
	    } else {
		rs = SR_INVALID ;
	    }
	    inetargs_finish(&ia) ;
	} /* end if (inetargs) */

	if ((rs >= 0) && (of & O_CLOEXEC)) {
	    rs = uc_closeonexec(fd,true) ;
	    if (rs < 0) {
		u_close(fd) ;
	    }
	}

	return (rs >= 0) ? fd : rs ;
} /* end subroutine (opendev_inet) */

local int inetargs_start(inetargs *iap,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	if (sl < 0) sl = lenstr(sp) ;
	memclear(iap) ;
	while (sl && (sp[0] == '/')) {
	    sp += 1 ;
	    sl -= 1 ;
	} /* end while */
	iap->ia[da_proto].l = sl ;
	iap->ia[da_proto].p = sp ;
	if (cchar *tp = strnchr(sp,sl,'/') ; tp) {
	    int		cl ;
	    cchar	*cp ;
	    iap->ia[da_host].l = intconv(tp - sp) ;
	    sl -= intconv((tp + 1) - sp) ;
	    sp = (tp + 1) ;
	    iap->ia[da_host].l = sl ;
	    iap->ia[da_host].p = sp ;
	    if ((tp = strnchr(sp,sl,'/')) != nullptr) {
	        iap->ia[da_host].l = intconv(tp - sp) ;
	        sl -= intconv((tp + 1) - sp) ;
	        sp = (tp + 1) ;
	  	cl = sl ;
		cp = sp ;
	        if ((tp = strnchr(sp,sl,'/')) != nullptr) {
	            cl = intconv(tp - sp) ;
		}
	        iap->ia[da_svc].l = cl ;
	        iap->ia[da_svc].p = cp ;
		if ((tp = strnchr(cp,cl,'.')) != nullptr) {
	            iap->ia[da_af].l = intconv(tp - cp) ;
	            iap->ia[da_af].p = cp ;
	            iap->ia[da_svc].l = intconv((cp + cl) - (tp + 1)) ;
	            iap->ia[da_svc].p = (tp + 1) ;
		}
	    }
	}

	{
	    int		sz = 0 ;
	    for (int i = 0 ; i < da_overlast ; i += 1) {
		if ((iap->ia[i].l < 0) && (iap->ia[i].p != nullptr)) {
		    iap->ia[i].l = lenstr(iap->ia[i].p) ;
		}
	    } /* end for */
	    for (int i = 0 ; i < da_overlast ; i += 1) {
		sz += (iap->ia[i].p != nullptr) ? (iap->ia[i].l+1) : 1 ;
	    } /* end for */
	    if (char *bp ; (rs = lm_mall(sz,&bp)) >= 0) {
		cchar	*asp ;
		iap->a = bp ;
	        for (int i = 0 ; i < da_overlast ; i += 1) {
		    asp = bp ;
		    if (iap->ia[i].p != nullptr) {
			bp = (strwcpy(bp,iap->ia[i].p,iap->ia[i].l) + 1) ;
		    } else {
			*bp++ = '\0' ;
		    }
		    iap->ia[i].p = asp ;
		} /* end for */
	    } /* end if (memory-acquire) */
	} /* end block */
	return rs ;
} /* end subroutine (inetargs_start) */

local int inetargs_finish(inetargs *iap) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	if (iap->a) {
	    rs1 = lm_free(iap->a) ;
	    if (rs >= 0) rs = rs1 ;
	    iap->a = nullptr ;
	} /* end if (memory-release) */
	return rs ;
} /* end subroutine (inetargs_finish) */


