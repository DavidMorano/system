/* uc_sighold */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* interface component for UNIX® library-3c */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>
#include	<sys/types.h>
#include	<signal.h>
#include	<errno.h>
#include	<usystem.h>
#include	<localmisc.h>


/* exported subroutines */

int uc_sighold(int sn) noex {
	int		rs ;
	if ((rs = sighold(sn)) < 0) {
	    rs = (- errno) ;
	}
	return rs ;
}
/* end subroutine (uc_sighold) */


