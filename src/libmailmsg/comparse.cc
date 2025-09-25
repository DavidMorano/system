/* comparse SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* comment-separate (parse) a mail header field value */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-05-29, David A­D­ Morano
	This code was adapted from the EMA (E-Mail Address) code
	(which has code that does a similar function).  Both the
	EMA code and this code were based on previous code I wrote
	for PCS-mailer address parsing back around 1992 or so.

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	comparse

	Description:
	This subroutine separates comment characters from the value
	characters in a mail message header field value.  This is
	not meant for use with mail addresses (although they certainly
	need to be comment-separated) but rather with shorter
	fixed-length header field values.  This subroutine was
	especially created to separate out the comments from the
	DATE header field value!

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<getbufsize.h>
#include	<ascii.h>
#include	<buffer.h>
#include	<mkchar.h>
#include	<char.h>		/* |CHAR_ISWHITE(3uc)| */
#include	<localmisc.h>

#include	"comparse.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |getlenstr(3u)| */

/* local defines */


/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

enum states {
    	state_value,
	state_comment,
	state_overlast
} ; /* end enum (states) */

namespace {
    struct vars {
	int		mailaddrlen ;
	int		mailcommlen ;
	operator int () noex ;
    } ; /* end struct (vars) */
} /* end namespace */


/* forward references */

template<typename ... Args>
static inline int comparse_magic(comparse *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == COMPARSE_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}

static inline int chiswh(int ch) noex {
    	return CHAR_ISWHITE(ch) ;
}

static int	comparse_bake(comparse *,cchar *,int) noex ;


/* local variables */

static vars		var ;


/* exported variables */


/* exported subroutines */

int comparse_start(comparse *op,cchar *sp,int µsl) noex {
    	COMPARSE	*hop = op ;
	int		rs = SR_FAULT ;
	int		vl = 0 ;
	if (op && sp) {
	    static cint		rsv = var ;
	    memclear(hop) ;
	    if ((rs = rsv) >= 0) {
		if (int sl ; (sl = getlenstr(sp,µsl)) >= 0) {
		    op->mailaddrlen = var.mailaddrlen ;
		    op->mailcommlen = var.mailcommlen ;
	            while ((sl > 0) && chiswh(*sp)) {
	                sp += 1 ;
	                sl -= 1 ;
	            }
	            if ((rs = comparse_bake(op,sp,sl)) >= 0) {
	                vl = rs ;
	                op->magic = COMPARSE_MAGIC ;
	            } /* end if (comparse_bake) */
		} /* end if (getlenstr) */
	    } /* end if (vars) */
	} /* end if (magic) */
	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (comparse_start) */

int comparse_finish(comparse *op) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = comparse_magic(op)) >= 0) {
	    if (op->val.sp) {
		void *vp = voidp(op->val.sp) ;
	        rs1 = libmem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        op->val.sp = nullptr ;
	    }
	    if (op->com.sp) {
		void *vp = voidp(op->com.sp) ;
	        rs1 = libmem.free(vp) ;
	        if (rs >= 0) rs = rs1 ;
	        op->com.sp = nullptr ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (comparse_finish) */

int comparse_getval(comparse *op,cchar **rpp) noex {
	int		rs ;
	if ((rs = comparse_magic(op)) >= 0) {
	    if (rpp) *rpp = op->val.sp ;
	    if (op->val.sp) rs = op->val.sl ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (comparse_getval) */

int comparse_getcom(comparse *op,cchar **rpp) noex {
	int		rs ;
	if ((rs = comparse_magic(op)) >= 0) {
	    if (rpp) *rpp = op->com.sp ;
	    if (op->com.sp) rs = op->com.sl ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (comparse_getcom) */


/* private subroutines */

int comparse_bake(comparse *op,cchar *sp,int sl) noex {
    	cnullptr	np{} ;
	buffer		as[state_overlast] ;
	cint		defsz = COMPARSE_DEFSZ ;
	int		rs ;
	int		rs1 ;
	int		vl = 0 ;
	while ((sl > 0) && chiswh(*sp)) {
	    sp += 1 ;
	    sl -= 1 ;
	}
	if ((rs = buffer_start(&as[0],defsz)) >= 0) {
	    if ((rs = buffer_start(&as[1],defsz)) >= 0) {
	        int	pstate = state_value ;
	        int	state = state_value ;
		int	pc ;
	        int	c_comment = 0 ;
		bool	f_quote = false ;
	        while ((sl != 0) && (*sp != '\0')) {
	            switch (cint ch = mkchar(*sp) ; ch) {
	            case CH_BSLASH:
	                if (f_quote) {
	                    sp += 1 ;
	                    sl -= 1 ;
	                    if ((sl > 0) && (sp[0] != '\0')) {
	                        rs = buffer_chr((as + state),*sp++) ;
	                        sl -= 1 ;
	                    }
	                } else {
	                    rs = buffer_chr((as + state),*sp++) ;
	                    sl -= 1 ;
	                }
	                break ;
	            case CH_DQUOTE:
	                f_quote = (! f_quote) ;
	                sp += 1 ;
	                sl -= 1 ;
	                break ;
	            case CH_LPAREN:
	                if (! f_quote) {
	                    if (c_comment == 0) {
	                        pc = buffer_getprev(as + state) ;
	                        if ((pc >= 0) && (! chiswh(pc))) {
	                            buffer_chr((as + state),' ') ;
				}
	                        pstate = state ;
	                        state = state_comment ;
	                        pc = buffer_getprev(as + state) ;
	                        if ((pc >= 0) && (! chiswh(pc))) {
	                            buffer_chr((as + state),' ') ;
				}
	                        sp += 1 ;
	                        sl -= 1 ;
	                    } else {
	                        rs = buffer_chr((as + state),*sp++) ;
	                        sl -= 1 ;
	                    }
	                    c_comment += 1 ;
	                } else {
	                    rs = buffer_chr((as + state),*sp++) ;
	                    sl -= 1 ;
	                }
	                break ;
	            case CH_RPAREN:
	                if ((! f_quote) && (c_comment > 0)) {
	                    c_comment -= 1 ;
	                    if (c_comment == 0) {
	                        state = pstate ;
	                        sp += 1 ;
	                        sl -= 1 ;
	                    } else {
	                        rs = buffer_chr((as + state),*sp++) ;
	                        sl -= 1 ;
	                    }
	                } else {
	                    rs = buffer_chr((as + state),*sp++) ;
	                    sl -= 1 ;
	                }
	                break ;
	            case CH_SP:
	            case CH_TAB:
	                if (! f_quote) {
			    if ((rs = buffer_get((as+state),np)) >= 0) {
	                        cint	cl = rs ;
	                        pc = buffer_getprev(as+state) ;
	                        if ((cl == 0) || ((pc >= 0) && chiswh(pc))) {
	                            sp += 1 ;
	                            sl -= 1 ;
	                            break ;
	                        }
			    } /* end if (buffer_get) */
	                } /* end if (not in a quote) */
			fallthrough ;
			/* FALLTHROUGH */
	            default:
	                if (c_comment > 0) {
	                    rs = buffer_chr((as + state_comment),*sp++) ;
	                } else {
	                    rs = buffer_chr((as + state),*sp++) ;
			}
	                sl -= 1 ;
	                break ;
	            } /* end switch */
		    if (rs < 0) break ;
	        } /* end while (scanning characters) */
	        if (rs >= 0) {
	            int		w = state_comment ;
	            if (cchar *bp ; (rs = buffer_get((as+w),&bp)) >= 0) {
	                int	bl = rs ;
	                while (bl && chiswh(bp[bl-1])) {
			    bl -= 1 ;
			}
	                if (cchar *cp ; (rs = libmem.strw(bp,bl,&cp)) >= 0) {
	                    op->com.sp = cp ;
	                    op->com.sl = bl ;
	                    w = state_value ;
	                    if ((rs = buffer_get((as+w),&bp)) >= 0) {
	                        bl = rs ;
	                        while (bl && chiswh(bp[bl-1])) {
				    bl -= 1 ;
				}
	                        if ((rs = libmem.strw(bp,bl,&cp)) >= 0) {
	                            op->val.sp = cp ;
	                            op->val.sl = bl ;
	                        } /* end if (memory-allocation) */
	                    } /* end if (buffer_get) */
	                    if (rs < 0) {
				void *vp = voidp(op->com.sp) ;
	                        libmem.free(vp) ;
	                        op->com.sp = nullptr ;
	                    } /* end if (error) */
	                } /* end if (memory-allocation) */
	            } /* end if (buffer_get) */
	        } /* end if (finishing-up) */
	        rs1 = buffer_finish(&as[1]) ;	/* comment */
		if (rs >= 0) rs = rs1 ;
	    } /* end if (buffer) */
	    vl = buffer_finish(&as[0]) ;	/* value */
	    if (rs >= 0) rs = vl ;
	} /* end if (buffer) */
	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (comparse_bake) */

int comparse::start(cchar *sp,int sl) noex {
	return comparse_start(this,sp,sl) ;
}

int comparse::getval(cchar **rpp) noex {
	return comparse_getval(this,rpp) ;
}

int comparse::getcom(cchar **rpp) noex {
	return comparse_getcom(this,rpp) ;
}

void comparse::dtor() noex {
	if (cint rs = finish ; rs < 0) {
	    ulogerror("comparse",rs,"fini-finish") ;
	}
}

comparse_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) {
	    switch (w) {
	    case comparsemem_finish:
	        rs = comparse_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (comparse_co::operator) */

vars::operator int () noex {
    	int		rs ;
	if ((rs = getbufsize(getbufsize_mailaddr)) >= 0) {
	    mailaddrlen = rs ;
	    if ((rs = getbufsize(getbufsize_ml)) >= 0) {
		mailcommlen = rs ;
	    }
	}
	return rs ;
}
/* end method (vars::operator) */


