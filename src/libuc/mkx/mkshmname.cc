/* mkshmname SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* make a shared-object filename from components */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-12-03, David A­D­ Morano
	This code was born out of frustration with cleaning up bad
	legacy code (of which there is quite a bit -- like almost
	all of it).

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	mkshmname

	Description:
	This subroutine constructs a shared-memory (SHM) name 
	from various specified
	components.

	Synopsis:
	int mkshmname(char *shmbuf,cchar *fp,int fl,cchar *dp,int dl) noex

	Arguments:
	shmbuf		result buffer pointer
	fp		something pointer
	fl		something length
	dp		something pointer
	dl		something length

	Returns:
	>=0		length of resulting c-string
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
#include	<bufsizevar.hh>		/* |maxnamelen(3uc)| */
#include	<storebuf.h>
#include	<strn.h>		/* |strnnlen(3uc)| */
#include	<localmisc.h>

#include	"mkx.h"

import libutil ;

/* local defines */

#ifndef	SHMPREFIXLEN
#define	SHMPREFIXLEN	8
#endif
#ifndef	SHMPOSTFIXLEN
#define	SHMPOSTFIXLEN	4
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

static bufsizevar	maxnamelen(getbufsize_mn) ;


/* exported variables */


/* exported subroutines */

int mkshmname(char *shmbuf,cchar *fp,int fl,cchar *dp,int dl) noex {
	int		rs ;
	int		len = 0 ;
	if ((rs = maxnamelen) >= 0) ylikely {
	    if (storebuf sb(shmbuf,rs) ; (rs = sb.chr('/')) >= 0) ylikely {
	        int	ml ;
	        if (fp[0] == '/') {
	            if (fl < 0) fl = lenstr(fp) ;
	            fp += 1 ;
	            fl -= 1 ;
	        }
	        ml = strnnlen(fp,fl,SHMPREFIXLEN) ;
	        rs = sb.strw(fp,ml) ;
	        if (rs >= 0) ylikely {
	            rs = sb.chr('$') ;
	        }
	        if (rs >= 0) ylikely {
	            ml = strnnlen(dp,dl,SHMPOSTFIXLEN) ;
	            rs = sb.strw(dp,ml) ;
	        }
	        len = sb.idx ;
	    } /* end if (storebuf) */
	} /* end if (maxnamelen) */
	return (rs >= 0) ? len : rs ;
}
/* end subroutine (mkshmname) */


