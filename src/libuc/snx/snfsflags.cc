/* snfsflags SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* make string version of the file-system flags */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	snfsflags

	Description:
	Ths subroutine is similar to |sncpy1(3uc)| but it takes a
	counted string for the source rather than only a NUL-terminated
	string.

	Synopsis:
	int snfsflags(char *dbuf,int dlen,ulong flags) noex

	Arguments:
	dbuf		destination string buffer
	dlen		destination string buffer length
	flags		FS-flags

	Returns:
	>=0		number of bytes in result
	<0		error (system-return)

	See-also:
	snfsflags(3uc)
	snopenflags(3uc)
	snpollflags(3uc)
	snxtilook(3uc)
	sninetaddr(3uc)
	snsigabbr(3uc)
	snabbr(3uc)
	snshellunder(3uc)
	snfilemode(3uc)
	sntid(3uc)
	snerrabbr(3uc)
	snrealname(3uc)
	snloadavg(3uc)
	snkeyval(3uc)
	snwvprintf(3uc)
	snwprintf(3uc)
	snkeyval(3uc)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/statvfs.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<localmisc.h>

#include	"snflags.h"
#include	"snx.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct flagstrs {
	int		fl ;
	cchar		*s ;
} ;


/* forward references */


/* local variables */

constexpr flagstrs	fs_vstat[] = {
	{ ST_RDONLY, "RDONLY" },
	{ ST_NOSUID, "NOSUID" },
#ifdef	ST_NOTRUNC
	{ ST_NOTRUNC, "NOTRUNC" },
#endif
	{ 0, nullptr }
} ; /* end array (fs_vstat) */


/* exported variables */


/* exported subroutines */

int snfsflags(char *dbuf,int dlen,ulong flags) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (dbuf) ylikely {
	    if (snflags ss ; (rs = ss.start(dbuf,dlen)) >= 0) ylikely {
	        for (int i = 0 ; (rs >= 0) && fs_vstat[i].fl ; i += 1) {
	            if (flags & fs_vstat[i].fl) {
	                rs = ss.addstr(fs_vstat[i].s) ;
		    }
	        } /* end for */
	        rs1 = ss.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (snflags) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (snfsflags) */


