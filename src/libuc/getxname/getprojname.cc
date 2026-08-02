/* getprojname SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get the default project for a given username (UNIX® Solaris® thing) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This subroutine is an extraction of code from elsewhere.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getprojname

	Description:
	This subroutine retrieves the default project for a given
	username.

	Synopsis:
	int getprojname(char *rbuf,int rlen,cchar *un) noex

	Arguments:
	rbuf		buffer to receive the nodename 
	rlen		length of supplied buffer (should be PROJNAMELEN)
	un		username to lookup

	Returns:
	>=0		length of retrieved name
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<uclibmem.h>		/* LIBUC */
#include	<ucgetpj.h>		/* LIBUC */
#include	<bufsizeget.h>		/* LIBUC */
#include	<getusername.h>		/* LIBUC */
#include	<sncpyx.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */

#include	"getprojname.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int	getprojnamer(char *,int,cchar *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int getprojname(char *rbuf,int rlen,cchar *un) noex {
	int		rs = SR_FAULT ;
	int		rl = 0 ; /* return-value */
	if (rbuf && un) ylikely {
	    rs = SR_INVALID ;
	    if (rlen > 0) ylikely {
		rs = getprojnamer(rbuf,rlen,un) ;
		rl = rs ;
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? rl : rs ;
} /* end subroutine (getprojname) */


/* local subroutines */

local int getprojnamer(char *rbuf,int rlen,cchar *un) noex {
    	int		rs ;
	int		rs1 ;
	int		rl = 0 ; /* return-value */
        if ((rs = bufsizeget(bufsize_un)) >= 0) ylikely {
            cint        ulen = rs ;
            char        ubuf[rs + 1] ;
            if ((un[0] == '-') || (un[0] == '\0')) {
                un = ubuf ;
                rs = getusername(ubuf,ulen,-1) ;
            } /* end if */
            if (rs >= 0) ylikely {
                if (char *pjbuf ; (rs = lm_pj(&pjbuf)) >= 0) ylikely {
                    cint        pjlen = rs ;
                    if (ucentpj pj ; (rs = pj.getdef(pjbuf,pjlen,un)) >= 0) {
                        rs = sncpy(rbuf,rlen,pj.pj_name) ;
                        rl = rs ;
                    }
                    rs1 = lm_free(pjbuf) ;
                    if (rs >= 0) rs = rs1 ;
                } /* end if (m-a-f) */
            } /* end if (ok) */
        } /* end if (bufsizeget) */
	return (rs >= 0) ? rl : rs ;
} /* end subroutine (getprojnamer) */


