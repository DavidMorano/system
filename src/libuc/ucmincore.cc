/* ucmincore SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* interface components for UNIX® library-3c */
/* provide some memory UNIX® operations */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-08-20, David A­D­ Morano
	This is new code.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	uc_mincoreset

	Description:
	Set bits in the supplied byte-array corresponding to the
	resident pages in memory for the present process.

	Synosis:
	int uc_mincoreset(void *ca,size_t cs,char *bits) noex ;

	Arguments:
	ca		core address
	cs		core size
	bits		pointer to array of |char| to receive result

	Return:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<climits>		/* |CHAR_BIT| */
#include	<cstdlib>
#include	<cstring>		/* |memset(3c)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<sysval.hh>
#include	<intfloor.h>
#include	<intceil.h>
#include	<localmisc.h>

#include	"ucmincore.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |memclear(3u)| */

/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* local structures */


/* forward references */

local size_t		getnpages(int ps,void *ca,size_t cs) noex ;


/* local variables */

static int		pagesz(sysval_ps) ;


/* exported variables */


/* exported subroutines */

int uc_mincoreset(void *ca,size_t cs,char *bits) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (ca && bits) ylikely {
	    rs = SR_INVALID ;
	    if (cs > 0) ylikely {
		if ((rs = pagesz) >= 0) ylikely {
		    csize	npages = getnpages(rs,ca,cs) ;
		    size_t	nbytes ;
		    int		asz ;
		    nbytes = (npages / CHAR_BIT) ;
		    memset(bits,0,nbytes) ;
		    asz = intconv(npages) ;
		    if (char *ba ; (rs = lm_mall(asz,&ba)) >= 0) ylikely {
			if ((rs = u_mincore(ca,cs,ba)) >= 0) ylikely {
			    for (size_t bi = 0uz ; bi < npages ; bi += 1) {
				csize bidx = (bi / CHAR_BIT) ;
				csize aidx = (bi & (CHAR_BIT - 1)) ;
			        if (ba[bi]) {
				    bits[aidx] |= (1 << bidx) ;
				}
			   } /* end for */
			} /* end if (u_mincore) */
			rs1 = lm_free(ba) ;
			if (rs >= 0) rs = rs1 ;
		    } /* end if (m-a-f) */
		} /* end if (pagesz) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (uc_mincoreset) */


/* local subroutines */

local size_t getnpages(int ps,void *ma,size_t ms) noex {
	uintptr_t	ua = uintptr_t(ma) ;
	uintptr_t	fa, ea, ca ;
	fa = uipfloor(ua,ps) ;
	ea = (ua + ms) ;
	ca = uipceil(ea,ps) ;
	return ((ca - fa) / ps) ;
}
/* end subroutine (getnpages) */


