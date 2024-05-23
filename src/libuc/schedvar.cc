/* SV SUPPORT */
/* lang=C++20 */

/* creates the substitution varaiables for 'scheduled' type operations */
/* version %I% last-modified %G% */

#define	CF_SEARCH	0		/* use searching? */

/* revision history:

	= 1998-09-01, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	schedvar

	Description:
	This little object is used to set up the substitution
	variables for use by 'scheduled' types of operations.  These
	are operations that take a schedule of templates to follow
	in carrying out their objective.  A good example of a
	'scheduled' operation is provided by the 'permsched'
	subroutine.  Check out that subroutine for more information.

	Notes:
	Note that the local subroutine |mkvarstr()| is identical
	to |sncpy3w(3dam)| with the middle string being '='.  Just
	saying ....

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstring>		/* |strlen(3c)| + |strchr(3c)| */
#include	<usystem.h>
#include	<vecstr.h>
#include	<storebuf.h>
#include	<sbuf.h>
#include	<snx.h>
#include	<sncpyx.h>
#include	<snwcpy.h>
#include	<vstrkeycmpx.h>
#include	<localmisc.h>

#include	"schedvar.h"


/* local defines */

#define	SCHEDVAR_NE	8		/* default number of entries */

#define	SV		schedvar
#define	SV_C		schedvar_cur

#define	KEYBUF(c)	(keybuf[0] = (c),keybuf[1] = '\0',keybuf)

#ifndef	CF_SEARCH
#define	CF_SEARCH	0		/* use searching? */
#endif


/* external subroutines */


/* local structures */


/* forward references */

static int schedvar_exper(SV *,char *,int,cc *,int) noex ;


/* local variables */

constexpr bool		f_search = CF_SEARCH ;


/* exported variables */


/* exported subroutines */

int schedvar_start(SV *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    cint	ve = SCHEDVAR_NE ;
	    cint	vo = VECSTR_OSORTED ;
	    rs = vecstr_start(op,ve,vo) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (schedvar_start) */

int schedvar_finish(SV *op) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (op) {
	    rs = SR_OK ;
	    {
	        rs1 = vecstr_finish(op) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (schedvar_finish) */

int schedvar_add(SV *op,cc *key,cc *vp,int vl) noex {
	int		rs = SR_FAULT ;
	if (op && key && vp) {
	    rs = vecstr_envset(op,key,vp,vl) ;
	}
	return rs ;
}
/* end subroutine (schedvar_add) */

int schedvar_curbegin(SV *op,SV_C *curp) noex {
	int		rs = SR_FAULT ;
	if (op && curp) {
	    rs = SR_OK ;
	    curp->i = -1 ;
	}
	return rs ;
}
/* end subroutine (schedvar_curbegin) */

int schedvar_curend(SV *op,SV_C *curp) noex {
	int		rs = SR_FAULT ;
	if (op && curp) {
	    rs = SR_OK ;
	    curp->i = -1 ;
	}
	return rs ;
}
/* end subroutine (schedvar_curend) */

int schedvar_enum(SV *op,SV_C *curp,char *kbuf,int klen,
		char *vbuf,int vlen) noex {
	int		rs = SR_FAULT ;
	int		vl = 0 ;
	if (op && curp && kbuf) {
	    cnullptr	np{} ;
	    int		i = (curp->i >= 0) ? (curp->i + 1) : 0 ;
	    cchar	*cp ;
	    kbuf[0] = '\0' ;
	    if (vbuf) vbuf[0] = '\0' ;
	    while (((rs = vecstr_get(op,i,&cp)) >= 0) && (cp == np)) {
	        i += 1 ;
	    }
	    if (rs >= 0) {
	        int	kl = -1 ;
		cchar	*tp ;
	        if ((tp = strchr(cp,'=')) != nullptr) {
	            kl = (tp - cp) ;
	        }
	        if ((rs = snwcpy(kbuf,klen,cp,kl)) >= 0) {
	    	    if (vbuf && tp) {
	                cp = (tp + 1) ;
	                rs = sncpy1(vbuf,vlen,cp) ;
	                vl = rs ;
	    	    }
		}
	    } /* end if (ok) */
	    if (rs >= 0) curp->i = i ;
	} /* end if (non-null) */
	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (schedvar_enum) */

int schedvar_findkey(SV *op,cc *key,cc **rpp) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    if_constexpr (f_search) {
		rs = vecstr_search(op,key,vstrkeycmp,rpp) ;
	    } else {
		rs = vecstr_finder(op,key,vstrkeycmp,rpp) ;
	    }
	}
	return rs ;
}
/* end subroutine (schedvar_findkey) */

int schedvar_del(SV *op,cc *key) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    if ((rs = vecstr_finder(op,key,vstrkeycmp,nullptr)) >= 0) {
	        rs = vecstr_del(op,rs) ;
	    }
	}
	return rs ;
}
/* end subroutine (schedvar_del) */

int schedvar_expand(SV *op,char *dbuf,int dlen,cc *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (op && dbuf && sp) {
	    rs = SR_TOOBIG ;
	    dbuf[0] = '\0' ;
	    if (dlen > 0) {
	        if (sl < 0) sl = strlen(sp) ;
		rs = schedvar_exper(op,dbuf,dlen,sp,sl) ;
		len = rs ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (schedvar_expand) */


/* private subroutines */

static int schedvar_exper(SV *op,char *dbuf,int dlen,cc *sp,int sl) noex {
	sbuf		b ;
	int		rs ;
	int		len = 0 ;
        if ((rs = sbuf_start(&b,dbuf,dlen)) >= 0) {
	    cnullptr	np{} ;
            cchar	*lfp = (sp + sl) ;
            char	keybuf[2] ;
            for (cc *fp = sp ; (fp < lfp) && *fp && (rs >= 0) ; fp += 1) {
                if (*fp == '%') {
                    fp += 1 ;
                    if (! *fp) break ;
                    if (*fp == '%') {
                        rs = sbuf_char(&b,'%') ;
                    } else {
			auto	vcf = vstrkeycmp ;
			cchar	*cp ;
			if_constexpr (f_search) {
                            rs = vecstr_search(op,KEYBUF(*fp),vcf,&cp) ;
			} else {
                            rs = vecstr_finder(op,KEYBUF(*fp),vcf,&cp) ;
			}
                        if (rs >= 0) {
                            rs = 0 ;
                            if (cchar *tp ; (tp = strchr(cp,'=')) != np) {
                                rs = sbuf_strw(&b,(tp+1),-1) ;
                            } /* end if (it had a value) */
                        } /* end if */
                    } /* end if (tried to expand a key) */
                } else {
                    rs = sbuf_char(&b,*fp) ;
                } /* end if */
            } /* end for */
            len = sbuf_finish(&b) ;
            if (rs >= 0) rs = len ;
        } /* end if (sbuf) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (schedvar_exper) */

#ifdef	COMMENT
static int mkvarstr(char *dbuf,int dlen,cc *k,cc *vp,int vl) noex {
	int		rs = SR_OK ;
	int		i = 0 ;
	if (rs >= 0) {
	    rs = storebuf_strw(dbuf,dlen,i,key,-1) ;
	    i += rs ;
	}
	if (rs >= 0) {
	    rs = storebuf_chr(dbuf,dlen,i,'=') ;
	    i += rs ;
	}
	if (rs >= 0) {
	    rs = storebuf_strw(dbuf,dlen,i,vp,vl) ;
	    i += rs ;
	}
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (mkvarstr) */
#endif /* COMMENT */


