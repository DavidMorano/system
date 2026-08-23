/* ucentgr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* UCENTGR object management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	uc_entgr{x}

	Description:
	These subroutines facilitate read-nnly access to the the
	system GROUP database.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<new>			/* C++STD |nothrow(3c++)| */
#include	<algorithm>		/* C++STD |min(3c++)| + |max(3c++)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<intceil.h>		/* LIBU */
#include	<storeitem.h>		/* LIBUC */
#include	<sbuf.h>		/* LIBUC */
#include	<vechand.h>		/* LIBUC */
#include	<strn.h>		/* LIBUC */
#include	<sfx.h>			/* LIBUC */
#include	<cfdec.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"ucgetgr.h"		/* |uc_getgr{x}(3uc)| */
#include	"ucentgr.h"
#include	"ucentxx.hh"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	GRE	ucentgr
#define	CGRE	const ucentgr
#define	SI	storeitem


/* imported namespaces */

using ucent::si_loadnames ;		/* local group subroutine */
using ucent::si_loadname ;		/* local group subroutine */
using ucent::si_copystr ;		/* local group subroutine */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int ucentgr_parseusers(GRE *,SI *,cchar *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int ucentgr::parse(char *grbuf,int grlen,cchar *sp,int sl) noex {
    	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (grbuf && sp) ylikely {
	    GROUP *gep = this ;
	    if (sl < 0) sl = lenstr(sp) ;
	    memclear(gep) ; /* shallow copy */
	    if (storeitem si ; (rs = si.start(grbuf,grlen)) >= 0) ylikely {
	        int	fi = 0 ;
	        for (cchar *tp ; (tp = strnchr(sp,sl,':')) != np ; ) {
		    cint	tl = intconv(tp - sp) ;
	            int		v = -1 ;
	            cchar	**vpp = nullptr ;
	            switch (fi++) {
	            case 0:
	                vpp = ccharpp(&gr_name) ;
	                break ;
	            case 1:
	                vpp = ccharpp(&gr_passwd) ;
	                break ;
	            case 2:
	                rs = cfdeci(sp,tl,&v) ;
	                gr_gid = v ;
	                break ;
	            case 3:
	                rs = ucentgr_parseusers(this,&si,sp,tl) ;
	                break ;
	            } /* end switch */
		    if ((rs >= 0) && vpp) {
	    	        cchar	*cp{} ;
	    	        if (int cl ; (cl = sfshrink(sp,tl,&cp)) >= 0) {
	        	    rs = si.strw(cp,cl,vpp) ;
	    	        }
		    } /* end if */
	            sl -= intconv((tp + 1) - sp) ;
	            sp = (tp + 1) ;
	            if (rs < 0) break ;
	        } /* end for */
	        if (rs >= 0) ylikely {
		    if ((fi == 3) && sl && sp[0]) {
		        fi += 1 ;
		        rs = ucentgr_parseusers(this,&si,sp,sl) ;
		    }
		    if ((rs >= 0) && (fi < 3)) rs = SR_BADFMT ;
	        } /* end if (ok) */
	        rs1 = si.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (storeitem) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (ucentgr::parse) */

int ucentgr::load(char *grbuf,int grlen,CGRE *sgrp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (grbuf && sgrp) ylikely {
	    GROUP *gep = this ;
	    *gep = *sgrp ; /* shallow copy */
	    if (storeitem si ; (rs = si.start(grbuf,grlen)) >= 0) ylikely {
	        if (sgrp->gr_mem) {
	            cint	n = lenstrarr(sgrp->gr_mem) ;
	            if (void **ptab{} ; (rs = si.ptab(n,&ptab)) >= 0) {
	                int	i ; /* used-afterwards */
	                gr_mem = charpp(ptab) ;
	                for (i = 0 ; (rs >= 0) && sgrp->gr_mem[i] ; i += 1) {
			    char	**rpp = (gr_mem + i) ;
	                    rs = si_copystr(&si,rpp,sgrp->gr_mem[i]) ;
	                } /* end for */
	                gr_mem[i] = nullptr ;
	            } /* end if (storeitem-ptab) */
	        } else {
	            gr_mem = nullptr ;
	        }
		if (rs >= 0) ylikely {
	            si_copystr(&si,&gr_name,sgrp->gr_name) ;
	            si_copystr(&si,&gr_passwd,sgrp->gr_passwd) ;
		} /* end if (ok) */
	        rs1 = si.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (storeitem) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (ucentgr::load) */

int ucentgr::format(char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (rbuf) ylikely {
	    if (sbuf b ; (rs = b.start(rbuf,rlen)) >= 0) ylikely {
	        for (int i = 0 ; i < 4 ; i += 1) {
	            if (i > 0) rs = b.chr(':') ;
	            if (rs >= 0) {
		        int	v ;
	                switch (i) {
	                case 0:
	                    rs = b.str(gr_name) ;
	                    break ;
	                case 1:
	                    rs = b.str(gr_passwd) ;
	                    break ;
	                case 2:
	                    v = gr_gid ;
	                    rs = b.dec(v) ;
	                    break ;
	                case 3:
			    if (gr_mem) {
				cchar	**sv = ccharpp(gr_mem) ;
			        rs = b.strs(',',sv) ;
			    }
	                    break ;
	                } /* end switch */
	            } /* end if */
	            if (rs < 0) break ;
	        } /* end for */
	        rs1 = b.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (sbuf) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (ucentgr::format) */

int ucentgr::size() noex {
	int		rs = SR_OK ;
	    int		sz = 1 ;
	    if (gr_name) {
	        sz += (lenstr(gr_name) + 1) ;
	    }
	    if (gr_passwd) {
	        sz += (lenstr(gr_passwd) + 1) ;
	    }
	    if (gr_mem) {
	        int	i = 0 ;
	        while (gr_mem[i]) {
	            sz += (lenstr(gr_mem[i++]) + 1) ;
	        } /* end while */
	        sz += ((i + 1) * szof(cchar *)) ;
	    } /* end if (group members) */
	    rs = iceil(sz,szof(cchar *)) ;
	return rs ;
} /* end subroutine (ucentgr::size) */

int ucentgr::getent(char *grbuf,int grlen) noex {
	return uc_getgrent(this,grbuf,grlen) ;
} /* end method */

int ucentgr::getnam(char *grbuf,int grlen,cchar *name) noex {
	return uc_getgrnam(this,grbuf,grlen,name) ;
} /* end method */

int ucentgr::getgid(char *grbuf,int grlen,gid_t gid) noex {
	return uc_getgrgid(this,grbuf,grlen,gid) ;
} /* end method */


/* local subroutines */

local int ucentgr_parseusers(ucentgr *grp,SI *sip,cchar *sp,int sl) noex {
	int		rs ;
	int		rs1 ;
	if (vechand u ; (rs = u.start(8,0)) >= 0) ylikely {
	    if ((rs = si_loadnames(sip,&u,sp,sl)) > 0) ylikely {
	        cint	n = rs ;
	        if (void **ptab{} ; (rs = sip->ptab(n,&ptab)) >= 0) ylikely {
		    int		i ; /* used afterwards */
	            void	*vp{} ;
	            grp->gr_mem = charpp(ptab) ;
		    for (i = 0 ; u.get(i,&vp) >= 0 ; i += 1) {
	                grp->gr_mem[i] = charp(vp) ;
	            } /* end for */
	            grp->gr_mem[i] = nullptr ;
	        } /* end if (storeitem-ptab) */
	    } else {
	        grp->gr_mem = nullptr ;
	    }
	    rs1 = u.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (vechand) */
	return rs ;
} /* end subroutine (ucentgr_parseusers) */


