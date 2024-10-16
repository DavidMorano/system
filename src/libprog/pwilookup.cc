/* pwilookup SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* wrapper functon dealing with the PWI object */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A­D­ Morano
	Module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	pwilookup

	Description:
	This little subroutine provides a bridge between those
	applications that want to lookup a single name (real-name)
	in the IPASSWD DB but do not want to use either the PWI or
	the IPASSWD objects directly (for whatever reason).  Note
	that we use a PWI object method that does not allow for
	multiple DB responses.  This is the desired behavior for
	some applications and is actually the most popular behavior
	desired so far.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<usystem.h>
#include	<localmisc.h>

#include	"pwi.h"


/* local defines */


/* external subroutines */


/* local variables */


/* exported variables */


/* exported subroutines */

int pwilookup(cchar *pr,cchar *dbname,char *rbuf,int rlen,cchar *name) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rl = 0 ;
	if (pr && rbuf && name) {
	    rs = SR_INVALID ;
	    rbuf[0] = '\0' ;
	    if (name[0]) {
	        if (pwi index ; (rs = pwi_open(&index,pr,dbname)) >= 0) {
	            {
	                rs = pwi_lookup(&index,rbuf,rlen,name) ;
	                rl = rs ;
	            }
	            rs1 = pwi_close(&index) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (open) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
}
/* end subroutine (pwilookup) */


