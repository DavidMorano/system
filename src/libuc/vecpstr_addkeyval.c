/* vecpstr_addkeyval */
/* lang=C20 */

/* vector-packed-string object (add environment variable) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-24, David A­D­ Morano
	This object module was morphed from some previous one.
	I do not remember what the previous one was.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	vecpstr_addkeyval

	Description:
	This is a little add-on to the VECPSTR object to add
	an environment variable in the form of a key-value pair.

	Synopsis:
	int vecpstr_addkeyval(vecpstr *op,cc *kp,int kl,cc *vp,int vl) noex

	Arguments:
	op		object pointer
	kp		key pointer
	kl		key length
	vp		value pointer
	kl		value length

	Returns:
	>=0		the total length of the filled up vecpstr so far!
	<0		error

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<unistd.h>
#include	<stdlib.h>
#include	<string.h>
#include	<usystem.h>
#include	<localmisc.h>

#include	"vecpstr.h"


/* local defines */


/* external subroutines */

extern char	*strwcpy(char *,cchar *,int) noex ;


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int vecpstr_addkeyval(vecpstr *op,cchar *kp,int kl,cchar *vp,int vl) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		i = 0 ;
	if (op && kp) {
	    int		size = 1 ;		/* terminating NUL */
	    char	*ep ;
	    if (kl < 0) kl = strlen(kp) ;
	    size += (kl+1) ;
	    if (vp != NULL) {
	        if (vl < 0) vl = strlen(vp) ;
	        size += vl ;
	    }
	    if ((rs = uc_libmalloc(size,&ep)) >= 0) {
	        int	el ;
	        char	*bp = ep ;
	        {
	            bp = strwcpy(bp,kp,kl) ;
	            *bp++ = '=' ;
	            bp = strwcpy(bp,vp,vl) ;
	            el = (bp-ep) ;
	            rs = vecpstr_add(op,ep,el) ;
		    i = rs ;
	        }
	        rs1 = uc_libfree(ep) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (memory-allocation) */
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (vecpstr_addkeyval) */


