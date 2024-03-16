/* ucentpr SUPPORT */
/* lang=C++20 */

/* UCENTPR object management */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	I provide the normal (usual) subroutines for managing the
	UCENTPR (UNIX® |PROTOENT| database entries) object.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<cerrno>
#include	<unistd.h>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<storeitem.h>
#include	<sbuf.h>
#include	<vechand.h>
#include	<strn.h>
#include	<sfx.h>
#include	<six.h>
#include	<intceil.h>
#include	<cfdec.h>
#include	<localmisc.h>

#include	"ucentpr.h"


/* local defines */

#define	PRE	ucentpr
#define	CPRE	const ucentpr
#define	SI	storeitem


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int ucentpr_parsestrs(PRE *,SI *,cchar *,int) noex ;

static int si_loadnames(SI *,vechand *,cchar *,int) noex ;
static int si_loadname(SI *,vechand *,cchar *,int) noex ;
static int si_copystr(SI *,char **,cchar *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int ucentpr::parse(char *ebuf,int elen,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (this && ebuf && sp) {
	    storeitem	ib, *ibp = &ib ;
	    if (sl < 0) sl = strlen(sp) ;
	    memclear(this) ;		/* potentially dangerous */
	    if ((rs = storeitem_start(ibp,ebuf,elen)) >= 0) {
	        int	fi = 0 ;
		int	si ;
		int	cl ;
	        cchar	*cp{} ;
		if ((si = sichr(sp,sl,'#')) >= 0) sl = si ;
	        while ((cl = sfnext(sp,sl,&cp)) > 0) {
	            int		v = -1 ;
	            cchar	**vpp = nullptr ;
	            switch (fi++) {
	            case 0:
	                vpp = (cchar **) &p_name ;
	                break ;
	            case 1:
	                rs = cfdeci(cp,cl,&v) ;
	                p_proto = v ;
	                break ;
	            case 2:
	                rs = ucentpr_parsestrs(this,ibp,cp,cl) ;
	                break ;
	            } /* end switch */
		    if ((rs >= 0) && vpp) {
	        	rs = storeitem_strw(ibp,cp,cl,vpp) ;
		    }
	            sl -= ((cp+cl)-sp) ;
	            sp = (cp+cl) ;
	            if (rs < 0) break ;
	        } /* end while */
	        rs1 = storeitem_finish(ibp) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (storeitem) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucentpr::parse) */

int ucentpr::load(char *rbuf,int rlen,const ucentpr *cprp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (this && rbuf && cprp) {
	    storeitem	ib ;
	    memcpy(this,cprp,sizeof(ucentpr)) ;
	    if ((rs = storeitem_start(&ib,rbuf,rlen)) >= 0) {
	        if (cprp->p_aliases) {
	            int		n = 0 ;
	            void	**tab{} ;
	            for (n = 0 ; cprp->p_aliases[n] ; n += 1) ;
	            if ((rs = storeitem_ptab(&ib,n,&tab)) >= 0) {
		        cchar	**aliases = (cchar **) cprp->p_aliases ;
		        int	i = 0 ;
	                p_aliases = (char **) tab ;
	                for (i = 0 ; cprp->p_aliases[i] ; i += 1) {
	                    rs = si_copystr(&ib,(p_aliases + i),aliases[i]) ;
	                    if (rs < 0) break ;
	                } /* end for */
	                p_aliases[i] = nullptr ;
	            } /* end if (storeitem-ptab) */
	        } else {
		    p_aliases = nullptr ;
	        } /* end if (aliases) */
		if (rs >= 0) rs = si_copystr(&ib,&p_name,cprp->p_name) ;
	        rs1 = storeitem_finish(&ib) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (storeitem) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucentpr::load) */

int ucentpr::format(char *rbuf,int rlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (this && rbuf) {
	    sbuf	b ;
	    if ((rs = sbuf_start(&b,rbuf,rlen)) >= 0) {
	        for (int i = 0 ; i < 3 ; i += 1) {
	            if (i > 0) rs = sbuf_char(&b,' ') ;
	            if (rs >= 0) {
		        int	v ;
	                switch (i) {
	                case 0:
	                    rs = sbuf_strw(&b,p_name,-1) ;
	                    break ;
	                case 1:
	                    v = p_proto ;
	                    rs = sbuf_deci(&b,v) ;
	                    break ;
	                case 2:
	    		    if (p_aliases) {
				cchar	**sv = (cchar **) p_aliases ;
	                        rs = sbuf_strs(&b,' ',sv) ;
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
/* end subroutine (ucentpr::format) */

int ucentpr::size() noex {
	int		rs = SR_FAULT ;
	if (this) {
	    int		size = 1 ;
	    if (p_name) {
	        size += (strlen(p_name)+1) ;
	    }
	    if (p_aliases) {
	        int	i = 0 ;
	        for (i = 0 ; p_aliases[i] ; i += 1) {
	            size += (strlen(p_aliases[i])+1) ;
	        } /* end for */
	        size += ((i+1)*sizeof(cchar *)) ;
	    } /* end if (group members) */
	    rs = iceil(size,sizeof(cchar *)) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (ucentpr::size) */


/* local subroutines */

static int ucentpr_parsestrs(ucentpr *prp,SI *ibp,cchar *sp,int sl) noex {
	vechand		u ;
	int		rs ;
	int		rs1 ;
	if ((rs = vechand_start(&u,8,0)) >= 0) {
	    if ((rs = si_loadnames(ibp,&u,sp,sl)) > 0) {
	        cint	n = rs ;
	        void	**ptab{} ;
	        if ((rs = storeitem_ptab(ibp,n,&ptab)) >= 0) {
		    int		i = 0 ;
	            void	*vp{} ;
	            prp->p_aliases = (char **) ptab ;
		    for (i = 0 ; vechand_get(&u,i,&vp) >= 0 ; i += 1) {
	                prp->p_aliases[i] = (char *) vp ;
	            } /* end for */
	            prp->p_aliases[i] = nullptr ;
	        } /* end if (storeitem-ptab) */
	    } else {
	        prp->p_aliases = nullptr ;
	    }
	    rs1 = vechand_finish(&u) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (vechand) */
	return rs ;
}
/* end subroutine (ucentpr_parsestrs) */

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
	cchar		*cp{} ;
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


