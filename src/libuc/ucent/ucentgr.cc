/* ucentgr SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

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
	These subroutines manage some simple tasks for the GROUP
	object, referenced as 'struct group'. This object is defined
	by UNIX® standards.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<utypealiases.h>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<storeitem.h>
#include	<sbuf.h>
#include	<vechand.h>
#include	<strn.h>
#include	<sfx.h>
#include	<intceil.h>
#include	<cfdec.h>
#include	<localmisc.h>

#include	"ucentgr.h"
#include	"ucentxx.hh"


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

static int ucentgr_parseusers(GRE *,SI *,cchar *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int ucentgr::parse(char *grbuf,int grlen,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (this && grbuf && sp) {
	    GROUP *gep = this ;
	    if (sl < 0) sl = strlen(sp) ;
	    memclear(gep) ; /* shallow copy */
	    if (storeitem si ; (rs = si.start(grbuf,grlen)) >= 0) {
	        int	fi = 0 ;
	        cchar	*tp ;
	        while ((tp = strnchr(sp,sl,':')) != nullptr) {
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
	                rs = cfdeci(sp,(tp-sp),&v) ;
	                gr_gid = v ;
	                break ;
	            case 3:
	                rs = ucentgr_parseusers(this,&si,sp,(tp-sp)) ;
	                break ;
	            } /* end switch */
		    if ((rs >= 0) && vpp) {
	    	        cchar	*cp{} ;
	    	        if (int cl ; (cl = sfshrink(sp,(tp-sp),&cp)) >= 0) {
	        	    rs = si.strw(cp,cl,vpp) ;
	    	        }
		    }
	            sl -= ((tp+1)-sp) ;
	            sp = (tp+1) ;
	            if (rs < 0) break ;
	        } /* end while */
	        if (rs >= 0) {
		    if ((fi == 3) && sl && sp[0]) {
		        fi += 1 ;
		        rs = ucentgr_parseusers(this,&si,sp,sl) ;
		    }
		    if ((rs >= 0) && (fi < 3)) rs = SR_BADFMT ;
	        }
	        rs1 = si.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (storeitem) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucentgr::parse) */

int ucentgr::load(char *grbuf,int grlen,CGRE *sgrp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (this && grbuf && sgrp) {
	    GROUP *gep = this ;
	    *gep = *sgrp ; /* shallow copy */
	    if (storeitem si ; (rs = si.start(grbuf,grlen)) >= 0) {
	        if (sgrp->gr_mem) {
	            int		n ; /* used-afterwards */
	            for (n = 0 ; sgrp->gr_mem[n] ; n += 1) ;
	            if (void **ptab{} ; (rs = si.ptab(n,&ptab)) >= 0) {
	                int	i ; /* used-afterwards */
	                char	**tab = charpp(ptab) ;
	                gr_mem = tab ;
	                for (i = 0 ; (rs >= 0) && sgrp->gr_mem[i] ; i += 1) {
			    char	**rpp = (gr_mem + i) ;
	                    rs = si_copystr(&si,rpp,sgrp->gr_mem[i]) ;
	                } /* end for */
	                gr_mem[i] = nullptr ;
	            } /* end if (storeitem-ptab) */
	        } else {
	            gr_mem = nullptr ;
	        }
	        si_copystr(&si,&gr_name,sgrp->gr_name) ;
	        si_copystr(&si,&gr_passwd,sgrp->gr_passwd) ;
	        rs1 = si.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (storeitem) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucentgr::load) */

int ucentgr::format(char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (this && rbuf) {
	    if (sbuf b ; (rs = b.start(rbuf,rlen)) >= 0) {
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
}
/* end subroutine (ucentgr::format) */

int ucentgr::size() noex {
	int		rs = SR_FAULT ;
	if (this) {
	    int		sz = 1 ;
	    if (gr_name) {
	        sz += (strlen(gr_name)+1) ;
	    }
	    if (gr_passwd) {
	        sz += (strlen(gr_passwd)+1) ;
	    }
	    if (gr_mem) {
	        int	i = 0 ;
	        while (gr_mem[i]) {
	            sz += (strlen(gr_mem[i++])+1) ;
	        } /* end for */
	        sz += ((i+1)*szof(cchar *)) ;
	    } /* end if (group members) */
	    rs = iceil(sz,szof(cchar *)) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucentgr::size) */


int ucentgr::getent(char *grbuf,int grlen) noex {
	return uc_getgrent(this,grbuf,grlen) ;
}

int ucentgr::getnam(char *grbuf,int grlen,cchar *name) noex {
	return uc_getgrnam(this,grbuf,grlen,name) ;
}

int ucentgr::getgid(char *grbuf,int grlen,gid_t gid) noex {
	return uc_getgrgid(this,grbuf,grlen,gid) ;
}


/* local subroutines */

static int ucentgr_parseusers(ucentgr *grp,SI *sip,cchar *sp,int sl) noex {
	int		rs ;
	int		rs1 ;
	if (vechand u ; (rs = u.start(8,0)) >= 0) {
	    if ((rs = si_loadnames(sip,&u,sp,sl)) > 0) {
	        cint	n = rs ;
	        if (void **ptab{} ; (rs = sip->ptab(n,&ptab)) >= 0) {
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
}
/* end subroutine (ucentgr_parseusers) */


