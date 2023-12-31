/* strdcpyopaque */
/* lang=C20 */

/* special (excellent) string-copy type of subroutine! */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-08, David A�D� Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strdcpyopaque

	Description:
	This is essentially the same as the |strdcpy1(3dam)|
	subroutine except that white-space characters not copied
	over to the result.

	Synopsis:
	char *strdcpyopaque(char *dp,int dl,cchar *sp,int sl) noex

	Arguments:
	dp		destination string buffer
	dl		destination string buffer length
	sp		source string
	sl		source string length

	Returns:
	-		pointer to end of destination string

	See-also:
	strdcpy1(3dam),
	strdcpycompact(3dam),
	strdcpyclean(3dam),

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<usystem.h>
#include	<char.h>
#include	<mkchar.h>
#include	<localmisc.h>

#include	"strdcpy.h"


/* external subroutines */


/* external variables */


/* exported subroutines */

char *strdcpyopaque(char *dp,int dl,const char *sp,int sl) noex {
	int		ch ;
	while (sl && *sp)  {
	    ch = mkchar(*sp) ;
	    if (! CHAR_ISWHITE(ch)) {
	        if (dl-- == 0) break ;
	        *dp++ = (char) ch ;
	    } /* end if */
	    sp += 1 ;
	    sl -= 1 ;
	} /* end while */
	*dp = '\0' ;
	return dp ;
}
/* end subroutine (strdcpyopaque) */


