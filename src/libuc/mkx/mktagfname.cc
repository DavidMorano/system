/* mktagfname SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* make a tag filename */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	mktagfname

	Description:
	This subroutine makes (creates) a tag filename given a tag
	basename and a base directory name.

	Synopsis:
	int mktagfname(char *rbuf,cchar *bdname,cchar *sp,int sl) noex

	Arguments:
	rbuf		result buffer pointer
	bdname		base dir-name
	sp		file string pointer
	sl		file string length

	Returns:
	>=0		number bytes of result
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<mkpathxw.h>
#include	<localmisc.h>

#include	"mkx.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int mktagfname(char *rbuf,cchar *basedname,cchar *sp,int sl) noex {
	return mkpath2w(rbuf,basedname,sp,sl) ;
}
/* end subroutine (mktagfname) */


