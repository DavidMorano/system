/* strpack_envstore SUPPORT */
/* lang=C++20 */

/* packed-string object (add environment variable) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-24, David A­D­ Morano
        This object module was morphed from some previous one. I do not remember
        what the previous one was.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strpack_envstore

	Description:
	This is a little add-on to the STRPACK object to add an
	environment variable in the form of a key-value pair.

	Synopsis:
	int strpack_envstore(SP *op,cc *kp,cc *vp,int vl,cc **rpp) noex

	Arguments:
	op		object pointer
	kp		key pointer
	vp		value pointer
	kl		value length
	rpp		result pointer

	Returns:
	>=0		the total length of the filled up strpack so far!
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<unistd.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
#include	<usystem.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"strpack.h"


/* local defines */

#define	SP	strpack


/* external subroutines */

extern "C" {
    int strpack_envstorer(SP *,cchar *,int,cchar *,int,cchar **) noex ;
}


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int strpack_envstore(SP *op,cc *kp,cc *vp,int vl,cc **rpp) noex {
	return strpack_envstorer(op,kp,-1,vp,vl,rpp) ;
}
/* end subroutine (strpack_envstore) */

int strpack_envstorer(SP *op,cc *kp,int kl,cc *vp,int vl,cc **rpp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		len = 0 ;
	if (op && kp) {
	    int		sz = 1 ;
	    char	*ep ;
	    if (kl < 0) kl = strlen(kp) ;
	    sz += (kl+1) ;
	    if (vp) {
	        if (vl < 0) vl = strlen(vp) ;
	        sz += vl ;
	    }
	    if ((rs = uc_malloc(sz,&ep)) >= 0) {
		{
	            int		el ;
	            char	*bp = ep ;
	            bp = strwcpy(bp,kp,kl) ;
	            *bp++ = '=' ;
	            bp = strwcpy(bp,vp,vl) ;
	            el = (bp-ep) ;
	            rs = strpack_store(op,ep,el,rpp) ;
		    len = rs ;
		}
	        rs1 = uc_free(ep) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (memory-allocation) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (strpack_envstorer) */


