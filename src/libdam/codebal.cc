/* codebal SUPPORT [*/
/* charset=ISO8859-1 */
/* lang=C++98 */

/* program character balance */
/* version %I% last-modified %G% */


/* revision history:

	= 2016-06-29, David A­D­ Morano
	This was really made from scratch.

*/

/* Copyright © 2016 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Names:
	codebal_start
	codebal_load
	codebal_finish

	Description:
	We determine if the character balance in the input (given
	a piece at a time) is correct or not.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |lenstr(3c)| */
#include	<new>
#include	<vector>
#include	<usystem.h>
#include	<ascii.h>
#include	<six.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"codebal.h"


/* local defines */


/* namespaces */

using namespace		std ;		/* yes, we want punishment! */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
static int codebal_ctor(codebal *op,Args ... args) noex {
	CODEBAL		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = memclear(hop) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (codebal_ctor) */

static int codebal_dtor(codebal *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (codebal_dtor) */

template<typename ... Args>
static inline int codebal_magic(codebal *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) {
	    rs = (op->magic == CODEBAL_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
}
/* end subroutine (codebal_magic) */


/* local variables */

constexpr char	chopen[] = { CH_LPAREN, CH_LBRACE, CH_LBRACK, '\0' } ;
constexpr char	chclose[] = { CH_RPAREN, CH_RBRACE, CH_RBRACK, '\0' } ;


/* exported variables */


/* exported subroutines */

int codebal_start(codebal *op) noex {
	int		rs ;
	if ((rs = codebal_ctor(op)) >= 0) {
	    rs = SR_OK ;
	    if (rs < 0) {
		codebal_dtor(op) ;
	    }
	} /* end if (codebal_ctor) */
	return rs ;
}
/* end subroutine (codebal_start) */

int codebal_finish(codebal *op) noex {
	int		rs ;
	int		rs1 ;
	int		f_bal = false ;
	if ((rs = codebal_magic(op)) >= 0) {
	    f_bal = (! op->f_fail) ;
	    if (f_bal) {
	        for (int i = 0 ; i < CODEBAL_NCH ; i += 1) {
		    f_bal = (op->counts[i] == 0) ;
		    if (!f_bal) break ;
	        } /* end for */
	    }
	    {
		rs1 = codebal_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magic = 0 ;
	} /* end if (magic) */
	return (rs >= 0) ? f_bal : rs ;
}
/* end subroutine (codebal_finish) */

int codebal_load(codebal *op,cchar *sp,int sl) noex {
	int		rs ;
	bool		f_fail = false ;
	if ((rs = codebal_magic(op,sp)) >= 0) {
	    if (sl < 0) sl = lenstr(sp) ;
	    while (sl-- && *sp) {
	        cint	ch = mkchar(*sp++) ;
	        int	w ;
	        switch (ch) {
	        case CH_LPAREN:
	        case CH_LBRACE:
	        case CH_LBRACK:
		    w = sichr(chopen,-1,ch) ;
		    op->counts[w] += 1 ;
		    break ;
	        case CH_RPAREN:
	        case CH_RBRACE:
	        case CH_RBRACK:
		    w = sichr(chclose,-1,ch) ;
		    if (op->counts[w] > 0) {
		        op->counts[w] -= 1 ;
		    } else {
		        f_fail = true ;
		    }
		    break ;
	        } /* end switch */
	        if (f_fail) break ;
	    } /* end while */
	} /* end if (magic) */
	return rs ;
}
/* end subroutine (codebal_load) */


/* private subroutines */


