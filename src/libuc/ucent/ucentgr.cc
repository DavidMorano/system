/* ucentgr SUPPORT */
/* lang=C++20 */

/* UCENTGR object management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	These subroutines manage some simple tasks for the GROUP
	object, referenced as 'struct group'. This object is defined
	by UNIX® standards.


*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<storeitem.h>
#include	<sbuf.h>
#include	<vechand.h>
#include	<strn.h>
#include	<sfx.h>
#include	<intceil.h>
#include	<cfdec.h>
#include	<localmisc.h>

#include	"ucentgr.h"


/* local defines */

#define	GRE	ucentgr
#define	CGRE	const ucentgr
#define	SI	storeitem


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int ucentgr_parseusers(GRE *,SI *,cchar *,int) noex ;

static int si_loadnames(SI *,vechand *,cchar *,int) noex ;
static int si_loadname(SI *,vechand *,cchar *,int) noex ;
static int si_copystr(SI *,char **,cchar *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int ucentgr::parse(char *grbuf,int grlen,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (this && grbuf && sp) {
	    storeitem	ib, *ibp = &ib ;
	    if (sl < 0) sl = strlen(sp) ;
	    memclear(this) ;		/* potentially dangerous */
	    if ((rs = storeitem_start(ibp,grbuf,grlen)) >= 0) {
	        int	fi = 0 ;
	        cchar	*tp ;
	        while ((tp = strnchr(sp,sl,':')) != nullptr) {
	            int		v = -1 ;
	            cchar	**vpp = nullptr ;
	            switch (fi++) {
	            case 0:
	                vpp = (cchar **) &gr_name ;
	                break ;
	            case 1:
	                vpp = (cchar **) &gr_passwd ;
	                break ;
	            case 2:
	                rs = cfdeci(sp,(tp-sp),&v) ;
	                gr_gid = v ;
	                break ;
	            case 3:
	                rs = ucentgr_parseusers(this,ibp,sp,(tp-sp)) ;
	                break ;
	            } /* end switch */
		    if ((rs >= 0) && vpp) {
	    	        int	cl ;
	    	        cchar	*cp{} ;
	    	        if ((cl = sfshrink(sp,(tp-sp),&cp)) >= 0) {
	        	    rs = storeitem_strw(ibp,cp,cl,vpp) ;
	    	        }
		    }
	            sl -= ((tp+1)-sp) ;
	            sp = (tp+1) ;
	            if (rs < 0) break ;
	        } /* end while */
	        if (rs >= 0) {
		    if ((fi == 3) && sl && sp[0]) {
		        fi += 1 ;
		        rs = ucentgr_parseusers(this,ibp,sp,sl) ;
		    }
		    if ((rs >= 0) && (fi < 3)) rs = SR_BADFMT ;
	        }
	        rs1 = storeitem_finish(ibp) ;
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
	    storeitem	ib ;
	    memcpy(this,sgrp,sizeof(ucentgr)) ;
	    if ((rs = storeitem_start(&ib,grbuf,grlen)) >= 0) {
	        if (sgrp->gr_mem) {
	            int		n ;
	            void	**ptab{} ;
	            for (n = 0 ; sgrp->gr_mem[n] ; n += 1) ;
	            if ((rs = storeitem_ptab(&ib,n,&ptab)) >= 0) {
	                int	i = 0 ;
	                char	**tab = (char **) ptab ;
	                gr_mem = tab ;
	                for (i = 0 ; (rs >= 0) && sgrp->gr_mem[i] ; i += 1) {
			    char	**rpp = (gr_mem + i) ;
	                    rs = si_copystr(&ib,rpp,sgrp->gr_mem[i]) ;
	                } /* end for */
	                gr_mem[i] = nullptr ;
	            } /* end if (storeitem-ptab) */
	        } else {
	            gr_mem = nullptr ;
	        }
	        si_copystr(&ib,&gr_name,sgrp->gr_name) ;
	        si_copystr(&ib,&gr_passwd,sgrp->gr_passwd) ;
	        rs1 = storeitem_finish(&ib) ;
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
	    sbuf	b ;
	    if ((rs = sbuf_start(&b,rbuf,rlen)) >= 0) {
	        for (int i = 0 ; i < 4 ; i += 1) {
	            if (i > 0) rs = sbuf_chr(&b,':') ;
	            if (rs >= 0) {
		        int	v ;
	                switch (i) {
	                case 0:
	                    rs = sbuf_strw(&b,gr_name,-1) ;
	                    break ;
	                case 1:
	                    rs = sbuf_strw(&b,gr_passwd,-1) ;
	                    break ;
	                case 2:
	                    v = gr_gid ;
	                    rs = sbuf_deci(&b,v) ;
	                    break ;
	                case 3:
			    if (gr_mem) {
				cchar	**sv = (cchar **) gr_mem ;
			        rs = sbuf_strs(&b,',',sv) ;
			    }
	                    break ;
	                } /* end switch */
	            } /* end if */
	            if (rs < 0) break ;
	        } /* end for */
	        rs1 = sbuf_finish(&b) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (sbuf) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucentgr::format) */

int ucentgr::size() noex {
	int		rs = SR_FAULT ;
	if (this) {
	    int		size = 1 ;
	    if (gr_name) {
	        size += (strlen(gr_name)+1) ;
	    }
	    if (gr_passwd) {
	        size += (strlen(gr_passwd)+1) ;
	    }
	    if (gr_mem) {
	        int	i = 0 ;
	        while (gr_mem[i]) {
	            size += (strlen(gr_mem[i++])+1) ;
	        } /* end for */
	        size += ((i+1)*sizeof(cchar *)) ;
	    } /* end if (group members) */
	    rs = iceil(size,sizeof(cchar *)) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucentgr::size) */


/* local subroutines */

static int ucentgr_parseusers(ucentgr *grp,SI *ibp,cchar *sp,int sl) noex {
	vechand		u ;
	int		rs ;
	int		rs1 ;
	if ((rs = vechand_start(&u,8,0)) >= 0) {
	    if ((rs = si_loadnames(ibp,&u,sp,sl)) > 0) {
	        cint	n = rs ;
	        void	**ptab{} ;
	        if ((rs = storeitem_ptab(ibp,n,&ptab)) >= 0) {
		    int		i = 0 ; /* used afterwards */
	            void	*vp{} ;
	            grp->gr_mem = (char **) ptab ;
		    for (i = 0 ; vechand_get(&u,i,&vp) >= 0 ; i += 1) {
	                grp->gr_mem[i] = (char *) vp ;
	            } /* end for */
	            grp->gr_mem[i] = nullptr ;
	        } /* end if (storeitem-ptab) */
	    } else {
	        grp->gr_mem = nullptr ;
	    }
	    rs1 = vechand_finish(&u) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (vechand) */
	return rs ;
}
/* end subroutine (ucentgr_parseusers) */

static int si_loadnames(SI *ibp,vechand *ulp,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	cchar		*tp ;
	while ((tp = strnpbrk(sp,sl," ,")) != nullptr) {
	    if ((tp-sp) > 0) {
		c += 1 ;
		rs = si_loadname(ibp,ulp,sp,(tp-sp)) ;
	    } /* end if (non-zero) */
	    sl -= ((tp+1)-sp) ;
	    sp = (tp+1) ;
	    if (rs < 0) break ;
	} /* end while */
	if ((rs >= 0) && sl && sp[0]) {
	    c += 1 ;
	    rs = si_loadname(ibp,ulp,sp,sl) ;
	}
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (si_loadnames) */

static int si_loadname(SI *ibp,vechand *ulp,cchar *sp,int sl) noex {
	int		rs ;
	cchar		*cp ;
	if ((rs = storeitem_strw(ibp,sp,sl,&cp)) >= 0) {
	    rs = vechand_add(ulp,cp) ;
	}
	return rs ;
}
/* end subroutine (si_loadname) */

static int si_copystr(SI *ibp,char **pp,cchar *p1) noex {
	int		rs = SR_OK ;
	cchar		**cpp = (cchar **) pp ;
	*cpp = nullptr ;
	if (p1) {
	    rs = storeitem_strw(ibp,p1,-1,cpp) ;
	}
	return rs ;
}
/* end subroutine (si_copystr) */


