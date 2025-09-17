/* filerm_name SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* process a name */
/* version %I% last-modified %G% */


/* revision history:

	= 1996-03-01, David A­D­ Morano
	The subroutine was adapated from other programs that do
	similar types of functions.

*/

/* Copyright © 1996 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

  	Name:
	filerm_name

	Description:
	This subroutine processes just one name at a time, but it
	can be either a regular file or a directory.

******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<csignal>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<baops.h>
#include	<bfile.h>
#include	<field.h>
#include	<wdt.h>
#include	<removename.h>
#include	<localmisc.h>

#include	"config.h"
#include	"defs.h"


/* local defines */


/* external subroutines */

extern int	checkname(cchar *,USTAT *,proginfo *) noex ;


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int filerm_name(proginfo *pip,cchar *name) noex {
	int		rs = SR_FAULT ;
	if (pip && name) {
	    if (USTAT sb ; (rs = u_lstat(name,&sb)) >= 0) {
	        int	wopts = 0 ;
	        bool	f_dir = S_ISDIR(sb.st_mode) ;
	        if ((! f_dir) && S_ISLNK(sb.st_mode) && pip->fl.follow) {
	            if (u_stat(name,&sb) >= 0) {
	                f_dir = S_ISDIR(sb.st_mode) ;
	            }
	        } /* end if */
	        if (f_dir) {
	            wopts |= (pip->fl.follow) ? WDT_MFOLLOW : 0 ;
	            rs = wdt(name,wopts,checkname,pip) ;
	        } else {
	            rs = checkname(name,&sb,pip) ;
	        }
	        /* finish up */
	        if ((rs >= 0) && (! pip->fl.no)) {
	            cint	ro = pip->rn_opts ;
	            rs = removename(name,ro,pip->rvp pip->bcount) ;
	            if ((rs < 0) && isNotPresent(rs)) rs = SR_OK ;
	        } /* end if (remove-all) */
	    } /* end if (lstat) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (filerm_name) */


