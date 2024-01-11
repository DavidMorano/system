/* mkfdfname SUPPORT */
/* lang=C++20 */

/* make a FD file-name (for BFILE and others) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was originally written.

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
	<0		error

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<usystem.h>
#include	<ctdec.h>
#include	<bufsizevar.hh>
#include	<localmisc.h>

#include	"mkfdfname.h"


/* local defines */

#define	CHX_STAR	'*'


/* external subroutines */



/* forward references */

static int		getdlen(int) noex ;


/* local variables */

static bufsizevar	maxpathlen(getbufsize_mp) ;


/* exported subroutines */

int mkfdfname(char *dbuf,int dlen,int fd) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (dbuf) {
	    rs = SR_INVALID ;
	    if (fd >= 0) {
		if ((rs = getdlen(dlen)) >= 0) {
	            dbuf[i++] = CHX_STAR ;
	            rs = ctdeci((dbuf+i),(dlen-i),fd) ;
	            i += rs ;
	        }
	    }
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
	        rs = maxpathlen ;
	    }
	}
	return rs ;
}
/* end subroutine (getdlen) */


