/* snflags SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* make string version of some flags */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	snflags

	Description:
	Ths object is used in the creation of flags strings.

	See-also:
	snfsflags(3uc)
	snopenflags(3uc)
	snpollflags(3uc)
	snxtilook(3uc)
	sninetaddr(3uc)
	snsigabbr(3uc)
	snabbr(3uc)
	snshellunder(3uc)
	snfilemode(3uc)
	sntid(3uc)
	snerrabbr(3uc)
	snrealname(3uc)
	snloadavg(3uc)
	snkeyval(3uc)
	snwvprintf(3uc)
	snwprintf(3uc)
	snkeyval(3uc)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/socket.h>		/* |AF_{xx}| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<ulogerror.h>
#include	<storebuf.h>
#include	<ascii.h>
#include	<localmisc.h>

#include	"snflags.h"

import libutil ;

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int snflags_start(snflags *op,char *bp,int bl) noex {
    	SNFLAGS		*hop = op ;
	int		rs = SR_FAULT ;
	if (op && bp) ylikely {
	    rs = memclear(hop) ;
	    op->bp = bp ;
	    op->bl = bl ;
	    *bp = '\0' ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (snflags_start) */

int snflags_addstr(snflags *op,cchar *sp) noex {
	return snflags_addstrw(op,sp,-1) ;
}

int snflags_addstrw(snflags *op,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		rl = 0 ;
	if (op && sp) ylikely {
	    rs = SR_OK ;
	    if ((sl > 0) || sp[0]) ylikely {
	        if (op->c++ > 0) {
	            cint	ch_comma = CH_COMMA ;
	            rs = storebuf_chr(op->bp,op->bl,op->bi,ch_comma) ;
	            op->bi += rs ;
		    rl += rs ;
	        }
	        if (rs >= 0) {
	            rs = storebuf_strw(op->bp,op->bl,op->bi,sp,sl) ;
	            op->bi += rs ;
		    rl += rs ;
	        }
	    } /* end if (non-empty) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (snflags_addstrw) */

int snflags_count(snflags *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = op->c ;
	}
	return rs ;
}
/* end subroutine (snflags_count) */

int snflags_len(snflags *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = op->bi ;
	}
	return rs ;
}
/* end subroutine (snflags_count) */

int snflags_finish(snflags *op) noex {
	int		rs = SR_FAULT ;
	if (op) ylikely {
	    rs = op->bi ;
	    op->bp = nullptr ;
	    op->bl = 0 ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (snflags_finish) */

int snflags::start(char *bufp,int bufl) noex {
	return snflags_start(this,bufp,bufl) ;
}

int snflags::addstr(cchar *sp) noex {
	return snflags_addstrw(this,sp,-1) ;
}

int snflags::addstrw(cchar *sp,int sl) noex {
	return snflags_addstrw(this,sp,sl) ;
}

void snflags::dtor() noex {
	if (cint rs = int(finish) ; rs < 0) {
	    ulogerror("snflags",rs,"fini-finish") ;
	}
} /* end method (snflags::dtor) */

snflags_co::operator int () noex {
	int		rs = SR_BUGCHECK ;
	if (op) ylikely {
	    switch (w) {
	    case snflagsmem_count:
	        rs = snflags_count(op) ;
	        break ;
	    case snflagsmem_len:
	        rs = snflags_len(op) ;
	        break ;
	    case snflagsmem_finish:
	        rs = snflags_finish(op) ;
	        break ;
	    } /* end switch */
	} /* end if (non-null) */
	return rs ;
}
/* end method (snflags_co::operator) */


