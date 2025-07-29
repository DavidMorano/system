/* mkfdfname SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* make a FD file-name (for BFILE and others) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mkfdfname

	Description:
	This subroutine creates a string that represents a file
	that is actually already open on a given file-descriptor
	(FD).  The filename looks like:
		*<fd>
	where:
	fd		is the decimal representation of the FD

	Synopsis:
	int mkfdfname(char *dbuf,int dlen,int fd) noex

	Arguments:
	dbuf		destination buffer pointer
	dlen		destination buffer length
	int		fd ;

	Returns:
	>=0		length of created string
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<bufsizevar.hh>
#include	<ctdec.h>
#include	<localmisc.h>

#include	"mkfdfname.h"


/* local defines */

#define	CHX_STAR	'*'


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* forward references */

static int		getdlen(int) noex ;


/* local variables */

static bufsizevar	maxnamelen(getbufsize_mn) ;


/* exported variables */


/* exported subroutines */

int mkfdfname(char *dbuf,int dlen,int fd) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (dbuf) ylikely {
	    rs = SR_INVALID ;
	    if (fd >= 0) ylikely {
		if ((rs = getdlen(dlen)) >= 0) ylikely {
	            dbuf[i++] = CHX_STAR ;
	            rs = ctdeci((dbuf+i),(dlen-i),fd) ;
	            i += rs ;
	        } /* end if (getdlen) */
	    } /* end if (valid FD) */
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (mkfdfname) */


/* local subroutines */

static int getdlen(int dlen) noex {
	int		rs = SR_OK ;
	if (dlen < 2) {
	    rs = SR_INVALID ;
	    if (dlen < 0) {
	        rs = maxnamelen ;
	    }
	}
	return rs ;
}
/* end subroutine (getdlen) */


