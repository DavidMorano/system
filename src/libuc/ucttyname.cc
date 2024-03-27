/* ucttyname SUPPORT */
/* lang=C++20 */

/* interface component for UNIX® library-3c */
/* get the filename (path) of a terminal device */


/* revision history:

	= 1998-03-01, David A­D­ Morano
        This is written to get a portable (reentrant) version of TTYNAME.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine gets the filepath of a terminal device under
	'/dev/'.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<unistd.h>
#include	<fcntl.h>
#include	<cerrno>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstring>
#include	<usystem.h>
#include	<usysflag.h>


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* forward references */

static int	ucttyname(int,char *,int) noex ;


/* local variables */

constexpr bool		f_sunos = F_SUNOS ;
constexpr bool		f_darwin = F_DARWIN ;


/* exported variables */


/* exported subroutines */

int uc_ttyname(int fd,char *dbuf,int dlen) noex {
	int		rs = SR_FAULT ;
	int		len = 0 ;
	if (dbuf) {
	    rs = SR_BADF ;
	    if ((fd >= 0) && (dlen > 0)) {
		if ((rs = ucttyname(fd,dbuf,dlen)) >= 0) {
	    	    len = strnlen(dbuf,dlen) ;
		}
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (uc_ttyname) */


/* local subroutines */

static int ucttyname(int fd,char *dbuf,int dlen) noex {
	int		rs = SR_OK ;
	if constexpr (f_sunos) {
	    cnullptr	np{} ;
	    char	*p ;
	    errno = 0 ; /* clear before next call */
	    p = reinterpret_cast<char *>(ttyname_r(fd,dbuf,dlen)) ;
	    if (p == np) {
		rs = (- errno) ;
	    }
	} else {
	    if ((rs = ttyname_r(fd,dbuf,dlen)) != 0) {
		rs = (- rs) ;
	    }
	}
	return rs ;
}
/* end subhroutine (ucttyname) */


