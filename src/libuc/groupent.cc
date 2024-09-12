/* groupent SUPPORT */
/* lang=C++20 */

/* subroutines for simple GROUP object (from UNIX® library-3c) management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-16, David A­D­ Morano
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
#include	<cstring>		/* for |strlen(3c)| */
#include	<grp.h>
#include	<usystem.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<storeitem.h>
#include	<sbuf.h>
#include	<vechand.h>
#include	<sfx.h>
#include	<strn.h>
#include	<cfdec.h>
#include	<intceil.h>
#include	<localmisc.h>

#include	"groupent.h"


/* local defines */

#define	GRE	GROUPENT
#define	CGRE	CGROUPENT
#define	SI	storeitem


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int groupent_parseusers(GROUP *,SI *,cchar *,int) noex ;
static int groupent_formatusers(GROUP *,sbuf *) noex ;

static int si_copystr(SI *,char **,cchar *) noex ;

static int storeitem_loadusers(SI *,vechand *,cchar *,int) noex ;
static int storeitem_loaduser(SI *,vechand *,cchar *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int groupent_parse(GROUP *grp,char *grbuf,int grlen,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (grp && grbuf && sp) {
	    storeitem	ib, *ibp = &ib ;
	    if (sl < 0) sl = strlen(sp) ;
	    memset(grp,0,sizeof(GROUP)) ;
	    if ((rs = storeitem_start(ibp,grbuf,grlen)) >= 0) {
	        int	fi = 0 ;
	        cchar	*tp ;
	        while ((tp = strnchr(sp,sl,':')) != nullptr) {
	            int		v = 0 ;
	            cchar	**vpp = nullptr ;
	            switch (fi++) {
	            case 0:
	                vpp = (const char **) &grp->gr_name ;
	                break ;
	            case 1:
	                vpp = (const char **) &grp->gr_passwd ;
	                break ;
	            case 2:
	                rs = cfdeci(sp,(tp-sp),&v) ;
	                grp->gr_gid = v ;
	                break ;
	            case 3:
	                rs = groupent_parseusers(grp,ibp,sp,(tp-sp)) ;
	                break ;
	            } /* end switch */
		    if ((rs >= 0) && vpp) {
	    	        int	cl ;
	    	        cchar	*cp ;
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
		        rs = groupent_parseusers(grp,ibp,sp,sl) ;
		    }
		    if ((rs >= 0) && (fi < 3)) rs = SR_BADFMT ;
	        }
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
	if (grp && grbuf && sgrp) {
	    storeitem	ib ;
	    memcpy(grp,sgrp,sizeof(GROUP)) ;
	    if ((rs = storeitem_start(&ib,grbuf,grlen)) >= 0) {
	        if (sgrp->gr_mem != nullptr) {
	            int		n ;
	            void	**ptab{} ;
	            for (n = 0 ; sgrp->gr_mem[n] != nullptr ; n += 1) ;
	            if ((rs = storeitem_ptab(&ib,n,&ptab)) >= 0) {
	                int	i = 0 ;
	                char	**tab = (char **) ptab ;
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
	    sbuf	b ;
	    if ((rs = sbuf_start(&b,rbuf,rlen)) >= 0) {
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
	    int		size = 1 ;
	    if (grp->gr_name) {
	        size += (strlen(grp->gr_name)+1) ;
	    }
	    if (grp->gr_passwd) {
	        size += (strlen(grp->gr_passwd)+1) ;
	    }
	    if (grp->gr_mem) {
	        int	i = 0 ;
	        while (grp->gr_mem[i]) {
	            size += (strlen(grp->gr_mem[i++])+1) ;
	        } /* end for */
	        size += ((i+1)*sizeof(cchar *)) ;
	    } /* end if (group members) */
	    rs = iceil(size,sizeof(cchar *)) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (groupent_size) */


/* local subroutines */

static int groupent_parseusers(GROUP *grp,SI *ibp,cchar *sp,int sl) noex {
	vechand		u ;
	int		rs ;
	int		rs1 ;
	if ((rs = vechand_start(&u,8,0)) >= 0) {
	    if ((rs = storeitem_loadusers(ibp,&u,sp,sl)) > 0) {
	        int	n = rs ;
	        void	**ptab{} ;
	        if ((rs = storeitem_ptab(ibp,n,&ptab)) >= 0) {
		    int		i{} ;
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
/* end subroutine (groupent_parseusers) */

static int groupent_formatusers(GROUP *grp,sbuf *bp) noex {
	int		rs = SR_OK ;
	if (grp->gr_mem) {
	    for (int i = 0 ; grp->gr_mem[i] ; i += 1) {
	        cchar	*un = grp->gr_mem[i] ;
		if (un[0]) {
	            if (i > 0) rs = sbuf_char(bp,',') ;
		    if (rs >= 0) rs = sbuf_strw(bp,un,-1) ;
		}
	        if (rs < 0) break ;
	    } /* end for */
	} /* end if (non-null members) */
	return rs ;
}
/* end subroutine (groupent_formatusers) */

static int storeitem_loadusers(SI *ibp,vechand *ulp,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		c = 0 ;
	cchar		*tp ;
	while ((tp = strnchr(sp,sl,',')) != nullptr) {
	    if ((tp-sp) > 0) {
		c += 1 ;
		rs = storeitem_loaduser(ibp,ulp,sp,(tp-sp)) ;
	    } /* end if (non-zero) */
	    sl -= ((tp+1)-sp) ;
	    sp = (tp+1) ;
	    if (rs < 0) break ;
	} /* end while */
	if ((rs >= 0) && sl && sp[0]) {
	    c += 1 ;
	    rs = storeitem_loaduser(ibp,ulp,sp,sl) ;
	}
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (storeitem_loadusers) */

static int storeitem_loaduser(SI *ibp,vechand *ulp,cchar *sp,int sl) noex {
	int		rs ;
	cchar		*cp{} ;
	if ((rs = storeitem_strw(ibp,sp,sl,&cp)) >= 0) {
	    rs = vechand_add(ulp,cp) ;
	}
	return rs ;
}
/* end subroutine (storeitem_loaduser) */

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


