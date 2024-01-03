/* varsub_dumpfd */
/* lang=C20 */

/* dump the substitution variables of the object */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-05-23, David A­D­ Morano
        This subroutine was written to provide some helper support for
        substitution setup.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>
#include	<sys/types.h>
#include	<usystem.h>
#include	<varsub.h>
#include	<localmisc.h>


/* external subroutines */

extern int	fdprintf(int,...) noex ;


/* exported subroutines */

int varsub_dumpfd(varsub *vshp,int fd) noex {
	if (vshp->vsa != NULL) {
	    varsub_ent	*vsa = vshp->vsa ;
	    int	i ;
	    if (fd < 0) fd = egetfd() ;
	    for (i = 0 ; i < vshp->i ; i += 1) {
	        if (vsa[i].kp == NULL) {
	            fdprintf(fd,"varsub_dumpfd NULL\n") ;
	        } else {
	            fdprintf(fd,"varsub_dumpfd key=%t val=%t\n",
	            vsa[i].kp,vsa[i].klen,
	            vsa[i].vp,vsa[i].vlen) ;
	        }
	    } /* end for */
	} /* end if (non-null) */

	return 0 ;
}
/* end subroutine (varsub_dumpfd) */


