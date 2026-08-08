/* snfilemode SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* make string version of the file-mode flags */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	snfilemode

	Description:
	Ths subroutine creates in the result string a symbolic
	representation of file-mode settings.

	Synopsis:
	int snfilemode(char *dbuf,int dlen,mode_t fm) noex

	Arguments:
	dbuf		destination string buffer
	dlen		destination string buffer length
	fm		file-mode

	Returns:
	>=0		number of bytes in result
	<0		error (system-return)

	See-also:
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
#include	<sys/stat.h>		/* POSIX |S_IF{xx}| */
#include	<unistd.h>		/* POSIX */
#include	<fcntl.h>		/* POSIX */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<ctoct.h>		/* LIBUC */
#include	<snflager.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU |OCTBUFLEN| */

#include	"snx.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */

struct flent {
	int		fl ;
	cchar		*s ;
} ; /* end struct (flent) */


/* forward references */


/* local variables */

constexpr flent		fl_mode[] = {
	{ S_ISUID, "SUID" },
	{ S_ISGID, "SGID" },
	{ S_ISVTX, "SAVETXT" },
	{ 0, nullptr }
} ; /* end array (fl_mode) */


/* exported variables */


/* exported subroutines */

int snfilemode(char *dbuf,int dlen,mode_t fm) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (dbuf) ylikely {
	    if (snflager fr ; (rs = fr.start(dbuf,dlen)) >= 0) ylikely {
	        cint	ft = (fm & S_IFMT) ;
	        cchar	*ms = nullptr ;
	        switch (ft) {
	        case S_IFIFO:
	            ms = "FIFO" ;
		    break ;
	        case S_IFCHR:
	            ms = "CHAR" ;
		    break ;
	        case S_IFDIR:
	            ms = "DIR" ;
		    break ;
	        case S_IFNAM:
	            ms = "NAME" ;
		    break ;
	        case S_IFBLK:
	            ms = "BLOCK" ;
		    break ;
	        case S_IFREG:
	            ms = "REG" ;
		    break ;
	        case S_IFLNK:
	            ms = "LINK" ;
		    break ;
	        case S_IFSOCK:
	            ms = "SOCK" ;
		    break ;
	        case S_IFDOOR:
	            ms = "DOOR" ;
		    break ;
	        default:
		    ms = "UNKNOWN" ;
		    break ;
	        } /* end switch */
	        if (ms) ylikely {
		    rs = fr.addstr(ms) ;
	        }
	        for (int i = 0 ; (rs >= 0) && fl_mode[i].s ; i += 1) {
	            if (fm & fl_mode[i].fl) {
	                rs = fr.addstr(fl_mode[i].s) ;
		    }
	        } /* end for */
	        if (rs >= 0) ylikely {
		    cint	n = 4 ; /* last characters */
		    cint	v = int(fm & S_IAMB) ;
		    cint	plen = OCTBUFLEN ;
		    char	pbuf[OCTBUFLEN+1] ;
		    if ((rs = ctoct(pbuf,plen,v)) >= 0) ylikely {
		        cchar	*cp = ((rs > n) ? (pbuf + (rs - n)) : pbuf) ;
	                rs = fr.addstr(cp) ;
		    } /* end if (ctoct) */
	        } /* end if (ok) */
	        rs1 = fr.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (snflags) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (snfilemode) */


