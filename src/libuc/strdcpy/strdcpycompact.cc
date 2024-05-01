/* strdcpycompact SUPPORT */
/* lang=C++20 */

/* counted-string copy while compacting white-space from the source */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-10 David A.D. Morano
	This was written from scratch.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strdcpycompact

	Description:
	Similar to |snwcpy(3dam)| except that we copy the source
	to the destination while removing extra white-space from
	the source.

	Synopsis:
	int strdcpycompact(char *dbuf,int dlen,cchar *sp,int sl) noex

	Arguments:
	dbuf		result buffer
	dlen		length of supplied result buffer
	sp		source string
	sl		source string length

	Returns:
	-		pointer to character at end of result string

	See also:
	strdcpy1(3dam),
	strdcpyopaque(3dam),
	strdcpyclean(3dam),

	Notes:
	+ The semantic here is to fill up the destination buffer
	as much as possible. There is no overflow indication returned
	to the caller. But since we are looping on chunks of source
	string separated by white-space, what if we get a chunk
	that is too big to fix into the remaining destriantion? We
	take an extra step to detect this situation and compensate.
	+ We could have used either 'sbuf(3dam)' or 'storebuf(3dam)'
	or a few other subroutines of this ilk, but this subroutine
	was written at a time before resort to those interfaces was
	automatic. It's a little messy, but it works just fine as
	it is!

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstring>		/* <- |strlen(3c)| */
#include	<algorithm>		/* <- |min(3c++)| */
#include	<usysrets.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<ascii.h>
#include	<sfx.h>
#include	<strmgr.h>
#include	<nleadstr.h>

#include	"strdcpy.h"


/* local defines */


/* imported namespaces */

using std::min ;			/* subroutine-template */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

char *strdcpycompact(char *dbuf,int dlen,cchar *sp,int sl) noex {
	char		*rp = nullptr ;
	if (dbuf && sp) {
	    strmgr	m ;
	    int		rs ;
	    int		rs1 ;
	    int		dl = 0 ;
	    if (dlen < 0) dlen = INT_MAX ;
	    if (sl < 0) sl = strlen(sp) ;
	    if ((rs = strmgr_start(&m,dbuf,dlen)) >= 0) {
		int	cl ;
	        cchar	*cp{} ;
	        while ((cl = sfnext(sp,sl,&cp)) > 0) {
		    if (dl > 0) {
	                if ((rs = strmgr_chr(&m,CH_SP)) >= 0) {
		            dl += 1 ;
		        }
		    }
		    if (rs >= 0) {
	                if ((rs = strmgr_str(&m,cp,cl)) >= 0) {
		            dl += cl ;
		        } else if (rs == SR_OVERFLOW) {
			    if ((rs = strmgr_rem(&m)) > 0) {
			        cint	ml = min(rs,cl) ;
	            	        if ((rs = strmgr_str(&m,cp,ml)) >= 0) {
				    dl += ml ;
			        }
			    }
		        }
	            }
	            sl -= ((cp+cl) - sp) ;
	            sp = (cp+cl) ;
	            if (rs < 0) break ;
	        } /* end while (looping through string pieces) */
	        rs1 = strmgr_finish(&m) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (strmgr) */
	    dbuf[dl] = '\0' ;
	    rp = (rs >= 0) ? (dbuf+dl) : nullptr ;
	} /* end if (non-null) */
	return rp ;
}
/* end subroutine (strdcpycompact) */


