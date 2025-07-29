/* snopenflags SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* make string version of the open-call flags */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	snopenflags

	Description:
	Ths subroutine creates in the result string a symbolic
	representation of open-file flags.

	Synopsis:
	int snopenflags(char *dbuf,int dlen,int flags) noex

	Arguments:
	dbuf		destination string buffer
	dlen		destination string buffer length
	flags		open-flags

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
#include	<unistd.h>
#include	<fcntl.h>
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


/* external subroutines */


/* external variables */


/* local structures */

struct flagstrs {
	int		fl ;
	cchar		*s ;
} ;


/* forward references */


/* local variables */

constexpr flagstrs	fs_open[] = {
	{ O_APPEND, "APPEND" },
	{ O_CREAT, "CREAT" },
	{ O_EXCL, "EXCL" },
	{ O_TRUNC, "TRUNC" },
	{ O_NOCTTY, "NOCTTY" },
	{ O_SYNC, "SYNC" },
	{ O_DSYNC, "DSYNC" },
#ifdef	O_RSYNC
	{ O_RSYNC, "RSYNC" },
#endif
	{ O_NDELAY, "NDELAY" },
	{ O_NONBLOCK, "NONBLOCK" },
#ifdef	O_LARGEFILE
	{ O_LARGEFILE, "LARGE" },
#endif
#ifdef	O_DIRECTORY
	{ O_DIRECTORY, "DIRECTORY" },
#endif
#ifdef	O_DIRECT
	{ O_DIRECT, "DIRECT" },
#endif
#ifdef	O_TMPFILE
	{ O_TMPFILE, "TMPFILE" },
#endif
#ifdef	O_PRIV
	{ O_PRIV, "PRIV" },
#endif
#ifdef	O_CLOEXEC
	{ O_CLOEXEC, "CLOEXEC" },
#endif
#ifdef	O_MINMOD
	{ O_MINMOD, "MINMOD" },
#endif
#ifdef	O_MINFD
	{ O_MINFD, "MINFD" },
#endif
#ifdef	O_NETWORK
	{ O_NETWORK, "NETWORK" },
#endif
	{ 0, nullptr }
} ; /* end struct (flagstrs) */


/* exported variables */


/* exported subroutines */

int snopenflags(char *dbuf,int dlen,int flags) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (dbuf) ylikely {
	    if (snflags	ss ; (rs = ss.start(dbuf,dlen)) >= 0) ylikely {
	        cint	am = (flags & O_ACCMODE) ;
	        cchar	*ms = nullptr ;
	        switch (am) {
	        case O_RDONLY:
		    ms = "RDONLY" ;
		    break ;
	        case O_WRONLY:
		    ms = "WRONLY" ;
		    break ;
	        case O_RDWR:
		    ms = "RDWR" ;
		    break ;
	        default:
		    ms = "ACCINV" ; /* access invalid */
		    break ;
	        } /* end switch */
	        if (ms) {
		    rs = ss.addstr(ms) ;
	        }
	        for (int i = 0 ; (rs >= 0) && fs_open[i].fl ; i += 1) {
	            if (flags & fs_open[i].fl) {
	                rs = ss.addstr(fs_open[i].s) ;
		    }
	        } /* end for */
	        rs1 = ss.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (snflags) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (snopenflags) */


