/* progbal SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++98 */

/* program character balance */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* debugging */

/* revision history:

	= 2016-02-01, David A­D­ Morano
	This was really made from scratch.

*/

/* Copyright © 2016 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	progbal

	Description:
	We determine if the character balance in the input (given
	a piece at a time) is correct or not.

		progbal_start
		progbal_load
		progbal_finish

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<vector>		/* C++STD */
#include	<new>			/* C++STD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ascii.h>		/* LIBU */
#include	<six.h>			/* LIBUC |sichr(3uc)| */
#include	<ischarx.h>		/* LIBUC */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */

#include	"progbal.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* debugging */
#endif


/* namespaces */

using namespace		std ;		/* yes, we want punishment! */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

template<typename ... Args>
local inline int progbal_ctor(progbal *op,Args ... args) noex {
    	PROGBAL		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = memclear(hop) ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (progbal_ctor) */

local int progbal_dtor(progbal *op) noex {
	int		rs = SR_FAULT ;
	if (op) {
	    rs = SR_OK ;
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (progbal_dtor) */

template<typename ... Args>
local inline int progbal_magic(progbal *op,Args ... args) noex {
	int		rs = SR_FAULT ;
	if (op && (args && ...)) ylikely {
	    rs = (op->magval == PROGBAL_MAGIC) ? SR_OK : SR_NOTOPEN ;
	}
	return rs ;
} /* end subroutine (progbal_magic) */


/* local variables */

constexpr char	chopen[] = { 
	CH_LPAREN, CH_LBRACE, CH_LBRACK, '\0' 
} ;

constexpr char	chclose[] = {
	CH_RPAREN, CH_RBRACE, CH_RBRACK, '\0' 
} ;

cbool		f_debug		= CF_DEBUG ;


/* exported variables */


/* exported subroutines */

int progbal_start(progbal *op) noex {
	int		rs ;
	if ((rs = progbal_ctor(op)) >= 0) {
	    rs = SR_OK ;
	    if (rs < 0) {
		progbal_dtor(op) ;
	    } /* end if (error) */
	} /* end if (progbal_magic) */
	return rs ;
} /* end subroutine (progbal_start) */

/* returns if all of the input was balanced */
int progbal_finish(progbal *op) noex {
	int		rs ;
	int		rs1 ;
	int		f_bal = false ; /* return-value */
	if ((rs = progbal_magic(op)) >= 0) {
	    f_bal = (!op->f_fail) ;
	    if (f_bal) {
	        for (int i = 0 ; i < PROGBAL_NCH ; i += 1) {
		    f_bal = (op->counts[i] == 0) ;
		    if (!f_bal) break ;
	        } /* end for */
	    }
	    {
		rs1 = progbal_dtor(op) ;
		if (rs >= 0) rs = rs1 ;
	    }
	    op->magval = 0 ;
	} /* end if (progbal_magic) */
	return (rs >= 0) ? f_bal : rs ;
} /* end subroutine (progbal_finish) */

int progbal_load(progbal *op,cchar *sp,int sl) noex {
	int		rs ;
	int		f_fail = false ; /* return-value */
	DEBUGPRINTF("ent\n") ;
	if ((rs = progbal_magic(op,sp)) >= 0) {
	    if (sl < 0) sl = lenstr(sp) ;
	    while (sl-- && *sp) {
	        int	w ;
	        switch (cint ch = MKCHAR(*sp++) ; ch) {
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
	} /* end if (progbal_magic) */
	DEBUGPRINTF("ret rs=%d\n",rs) ;
	return rs ;
} /* end subroutine (progbal_load) */


/* private subroutines */


