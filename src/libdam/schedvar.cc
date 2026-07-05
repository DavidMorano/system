/* schedvar SUPPORT (libuc-permx) */
/* charset=ISO8859-1 */
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

	Object:
	schedvar

	Description:
	This little object is used to set up the substitution
	variables for use by 'scheduled' types of operations.  These
	are operations that take a schedule of templates to follow
	in carrying out their objective.  A good example of a
	'scheduled' operation is provided by the 'permsched'
	subroutine.  Check out that subroutine for more information.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD |strchr(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<vecstr.h>		/* LIBUC */
#include	<sbuf.h>		/* LIBUC */
#include	<snx.h>			/* LIBUC |snkeyval(3uc)| */
#include	<sncpyx.h>		/* LIBUC */
#include	<snwcpy.h>		/* LIBUC */
#include	<vstrkeycmp.h>		/* LIBUC |vstrkeycmp(3uc)| */
#include	<localmisc.h>		/* LIBU */

#include	"schedvar.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#define	SCHEDVAR_NE	8		/* default number of entries */

#define	SV		schedvar
#define	SV_C		schedvar_cur

#define	KEYBUF(c)	(keybuf[0] = (c),keybuf[1] = '\0',keybuf)

#ifndef	CF_SEARCH
#define	CF_SEARCH	0		/* use searching? */
#endif


/* imported namespaces */


/* local typedefs */

typedef vecstr *	vecstrp ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
inline int schedvar_magic(schedvar *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == SCHEDVAR_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (schedvar_magic) */

local int schedvar_exper(SV *,char *,int,cc *,int) noex ;


/* local variables */

cbool		f_search = CF_SEARCH ;


/* exported variables */


/* exported subroutines */

int schedvar_start(SV *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    cint	osz = szof(vecstr) ;
	    op->magval = 0 ;
	    if (void *vp ; (rs = lm_mall(osz,&vp)) >= 0) ylikely {
		vecstr	*slp = vecstrp(vp) ;
	        cint	ve = SCHEDVAR_NE ;
	        cint	vo = vecstrm.sorted ;
	        if ((rs = slp->start(ve,vo)) >= 0) ylikely {
		    op->slp = slp ;
		    op->magval = SCHEDVAR_MAGIC ;
		}
		if (rs < 0) {
		    lm_free(slp) ;
		}
	    } /* end if (object allocation) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (schedvar_start) */

int schedvar_finish(SV *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = schedvar_magic(op)) >= 0) ylikely {
	    if (op->slp) ylikely {
	        {
	            rs1 = vecstr_finish(op->slp) ;
	            if (rs >= 0) rs = rs1 ;
	        }
	        {
		    rs1 = lm_free(op->slp) ;
		    if (rs >= 0) rs = rs1 ;
		    op->slp = nullptr ;
	        }
	    }
	    op->magval = 0 ;
	} /* end if (magic) */
	return rs ;
} /* end subroutine (schedvar_finish) */

int schedvar_add(SV *op,cc *key,cc *vp,int vl) noex {
	int		rs ;
	if ((rs = schedvar_magic(op,key,vp)) >= 0) ylikely {
	    rs = vecstr_envset(op->slp,key,vp,vl) ;
	}
	return rs ;
} /* end subroutine (schedvar_add) */

int schedvar_curbegin(SV *op,SV_C *curp) noex {
	int		rs ;
	if ((rs = schedvar_magic(op,curp)) >= 0) ylikely {
	    curp->i = -1 ;
	}
	return rs ;
} /* end subroutine (schedvar_curbegin) */

int schedvar_curend(SV *op,SV_C *curp) noex {
	int		rs ;
	if ((rs = schedvar_magic(op,curp)) >= 0) ylikely {
	    curp->i = -1 ;
	}
	return rs ;
} /* end subroutine (schedvar_curend) */

int schedvar_curenum(SV *op,SV_C *curp,char *kbuf,int klen,
		char *vbuf,int vlen) noex {
	cnullptr	np{} ;
	int		rs ;
	int		vl = 0 ;
	if ((rs = schedvar_magic(op,curp,kbuf)) >= 0) ylikely {
	    vecstr	*slp = op->slp ;
	    int		i = (curp->i >= 0) ? (curp->i + 1) : 0 ;
	    cchar	*cp{} ;
	    kbuf[0] = '\0' ;
	    if (vbuf) vbuf[0] = '\0' ;
	    while (((rs = slp->get(i,&cp)) >= 0) && (cp == np)) {
	        i += 1 ;
	    }
	    if (rs >= 0) ylikely {
	        int	kl = -1 ;
		cchar	*vap = nullptr ;
		if (cchar *tp ; (tp = strchr(cp,'=')) != nullptr) {
	            kl = intconv(tp - cp) ;
		    vap = (tp + 1) ;
	        }
	        if ((rs = snwcpy(kbuf,klen,cp,kl)) >= 0) ylikely {
	    	    if (vbuf && vap) {
	                rs = sncpy1(vbuf,vlen,vap) ;
	                vl = rs ;
	    	    }
		}
	    } /* end if (ok) */
	    if (rs >= 0) curp->i = i ;
	} /* end if (magic) */
	return (rs >= 0) ? vl : rs ;
} /* end subroutine (schedvar_curenum) */

int schedvar_findkey(SV *op,cc *key,cc **rpp) noex {
	int		rs ;
	if ((rs = schedvar_magic(op,key)) >= 0) ylikely {
	    vecstr	*slp = op->slp ;
	    if_constexpr (f_search) {
		rs = slp->search(key,vstrkeycmp,rpp) ;
	    } else {
		rs = slp->finder(key,vstrkeycmp,rpp) ;
	    }
	} /* end if (magic) */
	return rs ;
} /* end subroutine (schedvar_findkey) */

int schedvar_del(SV *op,cc *key) noex {
	int		rs ;
	if ((rs = schedvar_magic(op,key)) >= 0) ylikely {
	    vecstr	*slp = op->slp ;
	    if ((rs = slp->finder(key,vstrkeycmp)) >= 0) {
	        rs = slp->del(rs) ;
	    }
	}
	return rs ;
} /* end subroutine (schedvar_del) */

int schedvar_expand(SV *op,char *dbuf,int dlen,cc *sp,int sl) noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = schedvar_magic(op,dbuf,sp)) >= 0) ylikely {
	    rs = SR_TOOBIG ;
	    dbuf[0] = '\0' ;
	    if (dlen > 0) ylikely {
	        if (sl < 0) sl = lenstr(sp) ;
		rs = schedvar_exper(op,dbuf,dlen,sp,sl) ;
		len = rs ;
	    } /* end if (valid) */
	} /* end if (magic) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (schedvar_expand) */


/* private subroutines */

local int schedvar_exper(SV *op,char *dbuf,int dlen,cc *sp,int sl) noex {
	cnullptr	np{} ;
	int		rs ;
	int		len = 0 ;
	if (sbuf b ; (rs = b.start(dbuf,dlen)) >= 0) ylikely {
	    vecstr	*slp = op->slp ;
            cchar	*lfp = (sp + sl) ;
            char	keybuf[2] = {} ;
            for (cc *fp = sp ; (fp < lfp) && *fp && (rs >= 0) ; fp += 1) {
                if (*fp == '%') {
                    fp += 1 ;
                    if (! *fp) break ;
                    if (*fp == '%') {
                        rs = b.chr('%') ;
                    } else {
			auto	vcf = vstrkeycmp ;
			cchar	*cp ;
			if_constexpr (f_search) {
                            rs = slp->search(KEYBUF(*fp),vcf,&cp) ;
			} else {
                            rs = slp->finder(KEYBUF(*fp),vcf,&cp) ;
			}
                        if (rs >= 0) ylikely {
                            if (cchar *tp ; (tp = strchr(cp,'=')) != np) {
                                rs = b.str(tp+1) ;
                            } /* end if (it had a value) */
                        } /* end if */
                    } /* end if (tried to expand a key) */
                } else {
                    rs = b.chr(*fp) ;
                } /* end if */
            } /* end for */
            len = b.finish ;
            if (rs >= 0) rs = len ;
        } /* end if (sbuf) */
	return (rs >= 0) ? len : rs ;
} /* end subroutine (schedvar_exper) */


