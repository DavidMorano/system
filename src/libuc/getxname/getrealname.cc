/* getrealname SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* try to find the real-name of a user (given a username) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-10-01, David A­D­ Morano
	This was written to consolidate this type of code that was
	previously done separately in different places.  This code
	only makes sense if access to the user UCENTPW record is
	*only* needed to get the GECOS "name" field.  If more general
	access of the user UCENTPW record is needed, something other
	than this should be used.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getrealname

	Description:
	This subroutine will try to find a real-name given a username.  If just
	queries the UCENTPW-GECOS information.

	Synopsis:
	int getrealname(char *rbuf,int rlen,cchar *un) noex

	Arguments:
	rbuf		supplied buffer to receive the program-root
	rlen		length of supplied buffer
	un		username

	Returns:
	>=0		length of result
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<getax.h>		/* LIBUC */
#include	<getpwx.h>		/* LIBUC */
#include	<getusername.h>		/* LIBUC */
#include	<mkx.h>			/* LIBUC |getgecosname(3uc)| */
#include	<localmisc.h>		/* LIBU |REALNAMELEN| */

#include	"getrealname.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int	getpwname(ucentpwx *,char *,int,cchar *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int getrealname(char *rbuf,int rlen,cchar *un) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rl = 0 ;
	if (rbuf) ylikely {
	    rs = SR_INVALID ;
	    if (rlen > 0) ylikely {
	        if (char *pwbuf ; (rs = lm_pw(&pwbuf)) >= 0) ylikely {
	            ucentpwx	pw ;
	            cint	pwlen = rs ;
	            if ((rs = getpwname(&pw,pwbuf,pwlen,un)) >= 0) ylikely {
			cauto gc = getgecosname ;
			cchar *gecos = pw.pw_gecos ;
	                if (cchar *gp ; (rs = gc(gecos,-1,&gp)) > 0) {
		            rs = mkrealname(rbuf,rlen,gp,rs) ;
			    rl = rs ;
	                } /* end if (getgecosname) */
	            } /* end if (getpwname) */
	            rs1 = lm_free(pwbuf) ;
	            if (rs >= 0) rs = rs1 ;
	        } /* end if (m-a-f) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
} /* end subroutine (getrealname) */


/* local subroutines */

local int getpwname(ucentpwx *pwp,char *pwbuf,int pwlen,cchar *un) noex {
	int		rs ;
	if (un && (un[0] != '\0') && (un[0] != '-')) {
	    rs = pwp->nam(pwbuf,pwlen,un) ;
	} else {
	    rs = getpwusername(pwp,pwbuf,pwlen,-1) ;
	}
	return rs ;
} /* end subroutine (getpwname) */


