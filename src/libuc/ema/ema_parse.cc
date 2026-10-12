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
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ascii.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<char.h>		/* LIBUC |CHAR_ISWHITE(3uc)| */
#include	<localmisc.h>		/* LIBU */

#include	"ema.h"

#pragma		GCC dependency		"ema_entry.ccm"
#pragma		GCC dependency		"ema_asstr.ccm"
#pragma		GCC dependency		"ema_parts.ccm"

import ema_entry ;
import ema_asstr ;
import ema_parts ;

/* local defines */


/* imported namespaces */

using libuc::libmem ;			/* variable */


/* local typedefs */

typedef ema_ent		ent ;
typedef ema_ent	*	entp ;


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

namespace ema_ns {
    extern int	ema_load(ema *,cchar *,int,parts *,ema *) noex ;
} /* end namespace (ema_ns) */


/* local variables */


/* exported variables */


/* exported subroutines */

namespace ema_ns {
    int ema_parseit(ema *op,asstr *srcp) noex {
	emaparts	nstates = emapart_overlast ;
	int		rs ;
	int		rs1 ;
	int		n = 0 ; /* return-value */
	if (parts pb ; (rs = pb.start(nstates)) >= 0) ylikely {
	    emaparts	pstate = emapart_address ;
	    emaparts	state = emapart_address ;
	    ema             *nlp = nullptr ;
	    cchar           *orig ;
	    int             sz ;
	    int             olen ;
	    int             c_comment = 0 ;
	    bool            f_quote = false ;
	    bool            f_exit = false ;
	    /* skip over any leading white space */
	    srcp->skipwhite() ;
	    /* initialize for this entry */
	    orig = srcp->sp ;
	    /* start scanning */
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
	                    state = emapart_comment ;
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
	                } /* end */
	                c_comment += 1 ;
	            } else {
	                pb.addchr(state,ch) ;
	                srcp->adv() ;
	            } /* end */
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
	                } /* end */
	            } else {
	                pb.addchr(state,ch) ;
	                srcp->adv() ;
	            } /* end */
	            break ;
	        case '<':
	            if ((state == emapart_address) && 
	                (! f_quote) && (c_comment == 0)) {
	                pstate = state ;
	                state = emapart_route ;
	                srcp->adv() ;
	            } else {
	                pb.addchr(state,ch) ;
	                srcp->adv() ;
	            } /* end */
	            break ;
	        case '>':
	            if ((state == emapart_route) && 
	                (! f_quote) && (c_comment == 0)) {
	                state = pstate ;
	                srcp->adv() ;
	            } else {
	                pb.addchr(state,ch) ;
	                srcp->adv() ;
	            } /* end */
	            break ;
	        case ':':
	            if ((state == emapart_address) &&
	                (! f_quote) && (c_comment == 0)) {
	                sz = szof(ema) ;
	                if ((rs = libmem.mall(sz,&nlp)) >= 0) {
	                    if ((rs = ema_start(nlp)) >= 0) {
	                        srcp->adv() ;
	                        rs = ema_parseit(nlp,srcp) ;
	                        if (rs < 0) {
	                            ema_finish(nlp) ;
				}
	                    } /* end */
	                    if (rs < 0) {
	                        libmem.free(nlp) ;
	                        nlp = nullptr ;
	                    } /* end */
	                } /* end if (allocation) */
	            } else {
	                pb.addchr(state,ch) ;
	                srcp->adv() ;
	            } /* end */
	            break ;
	        case ';':
	            if ((state == emapart_address) && 
	                (! f_quote) && (c_comment == 0)) {
	                f_exit = true ;
	                olen = intconv(srcp->sp - orig) ;
	                srcp->adv() ;
	            } else {
	                pb.addchr(state,ch) ;
	                srcp->adv() ;
	            } /* end */
	            break ;
	        case ',':
	            if ((! f_quote) && (c_comment == 0) && 
	                (state == emapart_address)) {
	                olen = intconv(srcp->sp - orig) ;
	                if ((olen > 0) && (rs = pb.len) > 0) {
	                    n += 1 ;
	                    rs = ema_load(op,orig,olen,&pb,nlp) ;
	                } /* end */
	                if (rs >= 0) {
	                    if ((rs = pb.finish) >= 0) {
	                        nlp = nullptr ;
	                        if ((rs = pb.start(nstates)) >= 0) {
	                            srcp->adv() ;
	                            srcp->skipwhite() ;
	                            orig = srcp->sp ;
	                            state = emapart_address ;
	                        } /* end */
	                    } /* end if */
	                } /* end if (ok) */
	            } else {
	                pb.addchr(state,ch) ;
	                srcp->adv() ;
	            } /* end */
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
			    bool fstate = false ;
	                    fstate = fstate || (state == emapart_route) ;
			    fstate = fstate || (state == emapart_address) ;
			    if (fstate) {
	                        srcp->adv() ;
	                        break ;
	                    } else if ((rs = pb.len(ai)) >= 0) {
	                        bool f = (rs == 0) ;
	                        if (! f) {
	                            if ((rs = pb.getprev(ai)) >= 0) {
	                                f = CHAR_ISWHITE(rs) ;
	                            }
	                        } /* end */
	                        if ((rs >= 0) && f) {
	                            ch = 0 ;
	                            srcp->adv() ;
	                            break ;
	                        } /* end */
	                    } /* end if */
	                } else {
	                    if ((rs = pb.len(emapart_comment)) >= 0) {
	                        bool f = (rs == 0) ;
	                        if (! f) {
				    emaparts si = emapart_comment ;
	                            if ((rs = pb.getprev(si)) >= 0) {
	                                f = CHAR_ISWHITE(rs) ;
	                            }
	                        } /* end if (second chance) */
	                        if ((rs >= 0) && f) {
	                            ch = 0 ;
	                            srcp->adv() ;
	                            break ;
	                        } /* end */
	                    } /* end if (parts) */
	                } /* end if */
	            } /* end if (not in a quote) */
	            falldown ;
	        default:
	            if (ch > 0) {
	                if (c_comment) {
	                    pb.addchr(emapart_comment,ch) ;
	                } else {
	                    pb.addchr(state,ch) ;
	                } /* end */
	            } /* end */
	            srcp->adv() ;
	            break ;
	        } /* end switch */
	        if (rs < 0) break ;
	    } /* end for */
	    if (! f_exit) {
	        olen = intconv(srcp->sp - orig) ;
	    }
	    if (rs >= 0) ylikely {
	        if ((olen > 0) && ((rs = pb.len) > 0)) {
	            n += 1 ;
	            rs = ema_load(op,orig,olen,&pb,nlp) ;
	            nlp = nullptr ;
	        } else if (rs >= 0) {
	            if (nlp) {
	                ema_finish(nlp) ;
	                libmem.free(nlp) ;
	                nlp = nullptr ;
	            } /* end */
	        } /* end if */
	    } /* end if (ok) */
	    if (rs < 0) {
	        if (nlp) {
	            ema_finish(nlp) ;
	            libmem.free(nlp) ;
	        } /* end */
	    } /* end if (error) */
	    op->n += n ;
	    rs1 = pb.finish ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (parts) */
	return (rs >= 0) ? n : rs ;
    } /* end subroutine (ema_parseit) */
} /* end namespace (ema_ns) */


/* local subroutines */


