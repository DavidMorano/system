/* comparse SUPPORT */
/* lang=C++20 */

/* comment-separate (parse) a mail header field value */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-05-29, David A­D­ Morano
	This code was adapted from the EMA (E-Mail Address) code
	(which has code that does a similar function).

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine separates comment characters from the value
	characters in a mail message header field value.  This is
	not meant for use with mail addresses (although they certainly
	need to be comment-separated) but rather with shorter
	fixed-length header field values.  This subroutine was
	especially created to separate out the comments from the
	DATE header field value!

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<cstdlib>
#include	<cstring>		/* <- for |strlen(3c)| */
#include	<usystem.h>
#include	<usupport.h>		/* <- for |memclear(3u)| */
#include	<ascii.h>
#include	<buffer.h>
#include	<char.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"comparse.h"


/* local defines */

#define	COMPARSE_DEFSIZE	80
#define	COMPARSE_SVALUE		0
#define	COMPARSE_SCOMMENT	1
#define	COMPARSE_SOVERLAST	2


/* external subroutines */


/* external variables */


/* forward references */

template<typename ... Args>
static inline int comparse_magic(comparse *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == COMPARSE_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (comparse_magic) */

static int	comparse_bake(comparse *,cchar *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int comparse_start(comparse *cpp,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		vl = 0 ;
	if (cpp && sp) {
	if (sl < 0) sl = strlen(sp) ;
	    memclear(cpp) ;		/* dangerous */
	    while ((sl > 0) && CHAR_ISWHITE(*sp)) {
	        sp += 1 ;
	        sl -= 1 ;
	    }
	    if ((rs = comparse_bake(cpp,sp,sl)) >= 0) {
	        vl = rs ;
	        cpp->magic = COMPARSE_MAGIC ;
	    }
	} /* end if (magic) */
	return (rs >= 0) ? vl : rs ;
}
/* end subroutine (comparse_start) */

int comparse_finish(comparse *cpp) noex {
	int		rs ;
	int		rs1 ;
	if ((rs = comparse_magic(cpp)) >= 0) {
	    if (cpp->val.sp) {
	        rs1 = uc_free(cpp->val.sp) ;
	        if (rs >= 0) rs = rs1 ;
	        cpp->val.sp = nullptr ;
	    }
	    if (cpp->com.sp) {
	        rs1 = uc_free(cpp->com.sp) ;
	        if (rs >= 0) rs = rs1 ;
	        cpp->com.sp = nullptr ;
	    }
	    cpp->magic = 0 ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (comparse_finish) */

int comparse_getval(comparse *cpp,cchar **rpp) noex {
	int		rs ;
	if ((rs = comparse_magic(cpp)) >= 0) {
	    if (rpp) *rpp = cpp->val.sp ;
	    if (cpp->val.sp) rs = cpp->val.sl ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (comparse_getval) */

int comparse_getcom(comparse *cpp,cchar **rpp) noex {
	int		rs ;
	if ((rs = comparse_magic(cpp)) >= 0) {
	    if (rpp) *rpp = cpp->com.sp ;
	    if (cpp->com.sp) rs = cpp->com.sl ;
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (comparse_getcom) */


/* private subroutines */

int comparse_bake(comparse *cpp,cchar *sp,int sl) noex {
	buffer		as[COMPARSE_SOVERLAST] ;
	cint		defsize = COMPARSE_DEFSIZE ;
	int		rs ;
	int		rs1 ;
	int		vl = 0 ;
	while ((sl > 0) && CHAR_ISWHITE(*sp)) {
	    sp += 1 ;
	    sl -= 1 ;
	}
	if ((rs = buffer_start(&as[0],defsize)) >= 0) {
	    if ((rs = buffer_start(&as[1],defsize)) >= 0) {
	        int	pstate = COMPARSE_SVALUE ;
	        int	state = COMPARSE_SVALUE ;
		int	pc ;
	        int	c_comment = 0 ;
		bool	f_quote = false ;
	        while ((sl != 0) && (*sp != '\0')) {
	            cint	ch = mkchar(*sp) ;
	            switch (ch) {
	            case '\\':
	                if (f_quote) {
	                    sp += 1 ;
	                    sl -= 1 ;
	                    if ((sl > 0) && (sp[0] != '\0')) {
	                        buffer_char((as + state),*sp++) ;
	                        sl -= 1 ;
	                    }
	                } else {
	                    buffer_char((as + state),*sp++) ;
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
	                        if ((pc >= 0) && (! CHAR_ISWHITE(pc))) {
	                            buffer_char((as + state),' ') ;
				}
	                        pstate = state ;
	                        state = COMPARSE_SCOMMENT ;
	                        pc = buffer_getprev(as + state) ;
	                        if ((pc >= 0) && (! CHAR_ISWHITE(pc))) {
	                            buffer_char((as + state),' ') ;
				}
	                        sp += 1 ;
	                        sl -= 1 ;
	                    } else {
	                        buffer_char((as + state),*sp++) ;
	                        sl -= 1 ;
	                    }
	                    c_comment += 1 ;
	                } else {
	                    buffer_char((as + state),*sp++) ;
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
	                        buffer_char((as + state),*sp++) ;
	                        sl -= 1 ;
	                    }
	                } else {
	                    buffer_char((as + state),*sp++) ;
	                    sl -= 1 ;
	                }
	                break ;
	            case ' ':
	            case '\t':
	                if (! f_quote) {
	                    cint	cl = buffer_get((as+state),nullptr) ;
	                    pc = buffer_getprev(as+state) ;
	                    if ((cl == 0) || ((pc >= 0) && CHAR_ISWHITE(pc))) {
	                        sp += 1 ;
	                        sl -= 1 ;
	                        break ;
	                    }
	                } /* end if (not in a quote) */
			fallthrough ;
			/* FALLTHROUGH */
	            default:
	                if (c_comment > 0) {
	                    buffer_char((as + COMPARSE_SCOMMENT),*sp++) ;
	                } else {
	                    buffer_char((as + state),*sp++) ;
			}
	                sl -= 1 ;
	                break ;
	            } /* end switch */
	        } /* end while (scanning characters) */
	        if (rs >= 0) {
	            cchar	*cp ;
	            cchar	*bp ;
	            int		bl ;
	            int		w = COMPARSE_SCOMMENT ;
	            if ((rs = buffer_get((as+w),&bp)) >= 0) {
	                bl = rs ;
	                while (bl && CHAR_ISWHITE(bp[bl-1])) bl -= 1 ;
	                if ((rs = uc_mallocstrw(bp,bl,&cp)) >= 0) {
	                    cpp->com.sp = cp ;
	                    cpp->com.sl = bl ;
	                    w = COMPARSE_SVALUE ;
	                    if ((rs = buffer_get((as+w),&bp)) >= 0) {
	                        bl = rs ;
	                        while (bl && CHAR_ISWHITE(bp[bl-1])) bl -= 1 ;
	                        if ((rs = uc_mallocstrw(bp,bl,&cp)) >= 0) {
	                            cpp->val.sp = cp ;
	                            cpp->val.sl = bl ;
	                        } /* end if (memory-allocation) */
	                    }
	                    if (rs < 0) {
	                        uc_free(cpp->com.sp) ;
	                        cpp->com.sp = nullptr ;
	                    }
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


