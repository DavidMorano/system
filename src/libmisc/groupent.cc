/* groupent SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* subroutines for simple GROUP object (from UNIX® library-3c) management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-16, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	groupent

	Description:
	These subroutines manage some simple tasks for the GROUP
	object, referenced as 'struct group'. This object is defined
	by UNIX® standards.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<grp.h>
#include	<cstddef>
#include	<cstdlib>
#include	<cstring>		/* for |lenstr(3c)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<storeitem.h>
#include	<sbuf.h>
#include	<vechand.h>
#include	<sfx.h>
#include	<strn.h>
#include	<cfdec.h>
#include	<intceil.h>
#include	<localmisc.h>

#include	"groupent.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */

#define	GRE	GROUPENT
#define	CGRE	CGROUPENT
#define	SI	storeitem


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int groupent_parseusers(GROUP *,SI *,cchar *,int) noex ;
local int groupent_formatusers(GROUP *,sbuf *) noex ;

local int si_copystr(SI *,char **,cchar *) noex ;

local int storeitem_loadusers(SI *,vechand *,cchar *,int) noex ;
local int storeitem_loaduser(SI *,vechand *,cchar *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int groupent_parse(GROUP *grp,char *grbuf,int grlen,cchar *sp,int sl) noex {
    	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (grp && grbuf && sp) ylikely {
	    storeitem	ib, *ibp = &ib ;
	    if (sl < 0) sl = lenstr(sp) ;
	    memclear(grp) ;
	    if ((rs = storeitem_start(ibp,grbuf,grlen)) >= 0) ylikely {
	        int	fi = 0 ;
	        for (cchar *tp ; (tp = strnchr(sp,sl,':')) != np ; ) {
		    cint	tl = intconv(tp - sp) ;
	            int		v = 0 ;
	            cchar	**vpp = nullptr ;
	            switch (fi++) {
	            case 0:
	                vpp = ccharpp(&grp->gr_name) ;
	                break ;
	            case 1:
	                vpp = ccharpp(&grp->gr_passwd) ;
	                break ;
	            case 2:
	                rs = cfdeci(sp,tl,&v) ;
	                grp->gr_gid = v ;
	                break ;
	            case 3:
	                rs = groupent_parseusers(grp,ibp,sp,tl) ;
	                break ;
	            } /* end switch */
		    if ((rs >= 0) && vpp) {
	    	        cchar	*cp ;
	    	        if (int cl ; (cl = sfshrink(sp,tl,&cp)) >= 0) {
	        	    rs = storeitem_strw(ibp,cp,cl,vpp) ;
	    	        }
		    }
	            sl -= intconv((tp + 1) - sp) ;
	            sp = (tp + 1) ;
	            if (rs < 0) break ;
	        } /* end for */
	        if (rs >= 0) ylikely {
		    if ((fi == 3) && sl && sp[0]) {
		        fi += 1 ;
		        rs = groupent_parseusers(grp,ibp,sp,sl) ;
		    }
		    if ((rs >= 0) && (fi < 3)) rs = SR_BADFMT ;
	        } /* end if (ok) */
	        rs1 = storeitem_finish(ibp) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (storeitem) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (groupent_parse) */

int groupent_load(GROUP *grp,char *grbuf,int grlen,CGROUP *sgrp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (grp && grbuf && sgrp) ylikely {
	    memcopy(grp,sgrp,szof(GROUP)) ;
	    storeitem	ib ;
	    if ((rs = storeitem_start(&ib,grbuf,grlen)) >= 0) ylikely {
	        if (sgrp->gr_mem != nullptr) {
	            cint	n = lenstrarr(sgrp->gr_mem) ;
	            if (void **ptab ; (rs = storeitem_ptab(&ib,n,&ptab)) >= 0) {
	                int	i = 0 ; /* used-afterwards */
	                char	**tab = charpp(ptab) ;
	                grp->gr_mem = tab ;
	                for (i = 0 ; (rs >= 0) && sgrp->gr_mem[i] ; i += 1) {
			    char	**rpp = (grp->gr_mem + i) ;
	                    rs = si_copystr(&ib,rpp,sgrp->gr_mem[i]) ;
	                } /* end for */
	                grp->gr_mem[i] = nullptr ;
	            } /* end if (storeitem-ptab) */
	        } else {
	            grp->gr_mem = nullptr ;
	        }
	        si_copystr(&ib,&grp->gr_name,sgrp->gr_name) ;
	        si_copystr(&ib,&grp->gr_passwd,sgrp->gr_passwd) ;
	        rs1 = storeitem_finish(&ib) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (storeitem) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (groupent_load) */

int groupent_format(GROUP *grp,char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (grp && rbuf) {
	    if (sbuf b ; (rs = sbuf_start(&b,rbuf,rlen)) >= 0) {
	        for (int i = 0 ; i < 4 ; i += 1) {
		    int		v ;
	            if (i > 0) rs = sbuf_char(&b,':') ;
	            if (rs >= 0) {
	                switch (i) {
	                case 0:
	                    rs = sbuf_strw(&b,grp->gr_name,-1) ;
	                    break ;
	                case 1:
	                    rs = sbuf_strw(&b,grp->gr_passwd,-1) ;
	                    break ;
	                case 2:
	                    v = grp->gr_gid ;
	                    rs = sbuf_deci(&b,v) ;
	                    break ;
	                case 3:
	                    rs = groupent_formatusers(grp,&b) ;
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
/* end subroutine (groupent_format) */

int groupent_size(CGROUP *grp) noex {
	int		rs = SR_FAULT ;
	if (grp) {
	    int		sz = 1 ;
	    if (grp->gr_name) {
	        sz += (lenstr(grp->gr_name)+ 1) ;
	    }
	    if (grp->gr_passwd) {
	        sz += (lenstr(grp->gr_passwd)+ 1) ;
	    }
	    if (grp->gr_mem) {
	        int	i = 0 ;
	        while (grp->gr_mem[i]) {
	            sz += (lenstr(grp->gr_mem[i++])+ 1) ;
	        } /* end for */
	        sz += ((i+ 1) * szof(cchar *)) ;
	    } /* end if (group members) */
	    rs = iceil(sz,szof(cchar *)) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (groupent_size) */


/* local subroutines */

local int groupent_parseusers(GROUP *grp,SI *ibp,cchar *sp,int sl) noex {
	vechand		u ;
	int		rs ;
	int		rs1 ;
	if ((rs = vechand_start(&u,8,0)) >= 0) {
	    if ((rs = storeitem_loadusers(ibp,&u,sp,sl)) > 0) {
	        int	n = rs ;
	        if (void **ptab ; (rs = storeitem_ptab(ibp,n,&ptab)) >= 0) {
		    int		i{} ; /* used-afterwards */
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
} /* end subroutine (groupent_parseusers) */

local int groupent_formatusers(GROUP *grp,sbuf *bp) noex {
	int		rs = SR_OK ;
	if (grp->gr_mem) {
	    for (int i = 0 ; grp->gr_mem[i] ; i += 1) {
	        if (cchar *un = grp->gr_mem[i] ; un[0]) {
	            if (i > 0) rs = sbuf_char(bp,',') ;
		    if (rs >= 0) rs = sbuf_strw(bp,un,-1) ;
		} /* end if */
	        if (rs < 0) break ;
	    } /* end for */
	} /* end if (non-null members) */
	return rs ;
} /* end subroutine (groupent_formatusers) */

local int storeitem_loadusers(SI *ibp,vechand *ulp,cchar *sp,int sl) noex {
    	cnullptr	np{} ;
	int		rs = SR_OK ;
	int		c = 0 ;
	for (cchar *tp ; (tp = strnchr(sp,sl,',')) != np ; ) {
	    if (cint tl = intconv(tp - sp) ; tl > 0) {
		c += 1 ;
		rs = storeitem_loaduser(ibp,ulp,sp,tl) ;
	    } /* end if (non-zero) */
	    sl -= intconv((tp + 1) - sp) ;
	    sp = (tp + 1) ;
	    if (rs < 0) break ;
	} /* end for */
	if ((rs >= 0) && sl && sp[0]) {
	    c += 1 ;
	    rs = storeitem_loaduser(ibp,ulp,sp,sl) ;
	}
	return (rs >= 0) ? c : rs ;
} /* end subroutine (storeitem_loadusers) */

local int storeitem_loaduser(SI *ibp,vechand *ulp,cchar *sp,int sl) noex {
	int		rs ;
	cchar		*cp{} ;
	if ((rs = storeitem_strw(ibp,sp,sl,&cp)) >= 0) {
	    rs = vechand_add(ulp,cp) ;
	}
	return rs ;
} /* end subroutine (storeitem_loaduser) */

local int si_copystr(SI *ibp,char **pp,cchar *p1) noex {
	int		rs = SR_OK ;
	cchar		**cpp = (cchar **) pp ;
	*cpp = nullptr ;
	if (p1) {
	    rs = storeitem_strw(ibp,p1,-1,cpp) ;
	}
	return rs ;
} /* end subroutine (si_copystr) */


