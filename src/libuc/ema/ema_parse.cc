/* ema_parse SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* E-Mail Address */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	ema

	Description:
	This is an email address handling module object.  It can
	parse out and store hierarchically organized EMAs.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<usystem.h>
#include	<ascii.h>
#include	<char.h>
#include	<localmisc.h>

#include	"ema.h"

#pragma		GCC dependency	"mod/libutil.ccm"
#pragma		GCC dependency	"ema_entry.ccm"
#pragma		GCC dependency	"ema_asstr.ccm"
#pragma		GCC dependency	"ema_parts.ccm"

import libutil ;
import ema_entry ;
import ema_asstr ;
import ema_parts ;

/* local defines */


/* imported namespaces */


/* local typedefs */

typedef ema_ent		ent ;
typedef ema_ent	*	entp ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

namespace ema_ns {
    extern int	ema_load(ema *,cchar *,int,parts *,ema *) noex ;
}


/* local variables */


/* exported variables */


/* exported subroutines */

namespace ema_ns {
    int ema_parseit(ema *op,asstr *srcp) noex {
	int		rs ;
	int		rs1 ;
	int		n = 0 ; /* return-value */
	if (parts pb ; (rs = pb.start(si_overlast)) >= 0) {
	    ema             *nlp = nullptr ;
	    cchar           *orig ;
	    int             sz ;
	    int             olen ;
	    int             pstate = si_address ;
	    int             state ;
	    int             c_comment = 0 ;
	    bool            f_quote = false ;
	    bool            f_exit = false ;
	    /* skip over any leading white space */
	    srcp->skipwhite() ;
	    /* initialize for this entry */
	    orig = srcp->sp ;
	    /* start scanning */
	    state = si_address ;
	    for (int ch ; (! f_exit) && ((ch = srcp->get) >= 0) ; ) {
	        switch (ch) {
	        case '\\':
	            if (f_quote) {
	                ch = srcp->adv() ;
	                if (ch >= 0) {
	                    pb.addchr(state,ch) ;
	                    srcp->adv() ;
	                }
	            } else {
	                pb.addchr(state,ch) ;
	                srcp->adv() ;
	            }
	            break ;
	        case CH_DQUOTE:
	            srcp->adv() ;
	            f_quote = (! f_quote) ;
	            break ;
	        case CH_LPAREN:
	            if (! f_quote) {
	                if (c_comment == 0) {
	                    pstate = state ;
	                    state = si_comment ;
	                    if ((rs = pb.len(state)) > 0) {
	                        if ((rs = pb.getprev(state)) >= 0) {
	                            cint ch_sp = ' ' ;
	                            if (rs != ch_sp) {
	                                pb.addchr(state,ch_sp) ;
	                            }
	                        }
	                    } /* end if (non-zero positive) */
	                    srcp->adv() ;
	                } else {
	                    pb.addchr(state,ch) ;
	                    srcp->adv() ;
	                }
	                c_comment += 1 ;
	            } else {
	                pb.addchr(state,ch) ;
	                srcp->adv() ;
	            }
	            break ;
	        case CH_RPAREN:
	            if ((! f_quote) && (c_comment > 0)) {
	                c_comment -= 1 ;
	                if (c_comment == 0) {
	                    state = pstate ;
	                    srcp->adv() ;
	                } else {
	                    pb.addchr(state,ch) ;
	                    srcp->adv() ;
	                }
	            } else {
	                pb.addchr(state,ch) ;
	                srcp->adv() ;
	            }
	            break ;
	        case '<':
	            if ((state == si_address) && 
	                (! f_quote) && (c_comment == 0)) {
	                pstate = state ;
	                state = si_route ;
	                srcp->adv() ;
	            } else {
	                pb.addchr(state,ch) ;
	                srcp->adv() ;
	            }
	            break ;
	        case '>':
	            if ((state == si_route) && 
	                (! f_quote) && (c_comment == 0)) {
	                state = pstate ;
	                srcp->adv() ;
	            } else {
	                pb.addchr(state,ch) ;
	                srcp->adv() ;
	            }
	            break ;
	        case ':':
	            if ((state == si_address) &&
	                (! f_quote) && (c_comment == 0)) {
	                sz = szof(ema) ;
	                if ((rs = uc_malloc(sz,&nlp)) >= 0) {
	                    if ((rs = ema_start(nlp)) >= 0) {
	                        srcp->adv() ;
	                        rs = ema_parseit(nlp,srcp) ;
	                        if (rs < 0)
	                            ema_finish(nlp) ;
	                    }
	                    if (rs < 0) {
	                        uc_free(nlp) ;
	                        nlp = nullptr ;
	                    }
	                } /* end if (allocation) */
	            } else {
	                pb.addchr(state,ch) ;
	                srcp->adv() ;
	            }
	            break ;
	        case ';':
	            if ((state == si_address) && 
	                (! f_quote) && (c_comment == 0)) {
	                f_exit = true ;
	                olen = intconv(srcp->sp - orig) ;
	                srcp->adv() ;
	            } else {
	                pb.addchr(state,ch) ;
	                srcp->adv() ;
	            }
	            break ;
	        case ',':
	            if ((! f_quote) && (c_comment == 0) && 
	                (state == si_address)) {
	                olen = intconv(srcp->sp - orig) ;
	                if ((olen > 0) && (rs = pb.len) > 0) {
	                    n += 1 ;
	                    rs = ema_load(op,orig,olen,&pb,nlp) ;
	                }
	                if (rs >= 0) {
	                    if ((rs = pb.finish) >= 0) {
	                        nlp = nullptr ;
	                        if ((rs = pb.start(si_overlast)) >= 0) {
	                            srcp->adv() ;
	                            srcp->skipwhite() ;
	                            orig = srcp->sp ;
	                            state = si_address ;
	                        }
	                    } /* end if */
	                } /* end if (ok) */
	            } else {
	                pb.addchr(state,ch) ;
	                srcp->adv() ;
	            }
	            break ;
	        /* that these cases are just some optimizations */
	        case '\n':
	        case '\r':
	        case '\v':
	        case '\f':
	        case '\t':
	        case ' ':
	        case MKCHAR(CH_NBSP):
	            if (! f_quote) {
	                if (c_comment == 0) {
	                    int     ai = state ;
	                    if ((state == si_route) || (state == si_address)) {
	                        srcp->adv() ;
	                        break ;
	                    } else if ((rs = pb.len(ai)) >= 0) {
	                        bool f = (rs == 0) ;
	                        if (! f) {
	                            if ((rs = pb.getprev(ai)) >= 0) {
	                                f = CHAR_ISWHITE(rs) ;
	                            }
	                        }
	                        if ((rs >= 0) && f) {
	                            ch = 0 ;
	                            srcp->adv() ;
	                            break ;
	                        }
	                    } /* end if */
	                } else {
	                    if ((rs = pb.len(si_comment)) >= 0) {
	                        bool f = (rs == 0) ;
	                        if (! f) {
	                            if ((rs = pb.getprev(si_comment)) >= 0) {
	                                f = CHAR_ISWHITE(rs) ;
	                            }
	                        } /* end if (second chance) */
	                        if ((rs >= 0) && f) {
	                                ch = 0 ;
	                                srcp->adv() ;
	                                break ;
	                        }
	                    } /* end if (parts) */
	                } /* end if */
	            } /* end if (not in a quote) */
	            fallthrough ;
	            /* FALLTHROUGH */
	        default:
	            if (ch > 0) {
	                if (c_comment) {
	                    pb.addchr(si_comment,ch) ;
	                } else {
	                    pb.addchr(state,ch) ;
	                }
	            }
	            srcp->adv() ;
	            break ;
	        } /* end switch */
	        if (rs < 0) break ;
	    } /* end for */
	    if (! f_exit) {
	        olen = intconv(srcp->sp - orig) ;
	    }
	    if (rs >= 0) {
	        if ((olen > 0) && ((rs = pb.len) > 0)) {
	            n += 1 ;
	            rs = ema_load(op,orig,olen,&pb,nlp) ;
	            nlp = nullptr ;
	        } else if (rs >= 0) {
	            if (nlp) {
	                ema_finish(nlp) ;
	                uc_free(nlp) ;
	                nlp = nullptr ;
	            }
	        } /* end if */
	    } /* end if (ok) */
	    if (rs < 0) {
	        if (nlp) {
	            ema_finish(nlp) ;
	            uc_free(nlp) ;
	        }
	    } /* end if (error) */
	    op->n += n ;
	    rs1 = pb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (parts) */
	return (rs >= 0) ? n : rs ;
    } /* end subroutine (ema_parseit) */
} /* end namespace (ema_ns) */


/* local subroutines */


