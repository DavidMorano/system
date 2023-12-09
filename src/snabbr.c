/* snabbr */
/* lang=C20 */

/* copy an abbreviation of a groups of words */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	snabbr

	Description:
	Copy an abbreviation of the given string to the destination.

	Synopsis:
	int snabbr(char *dp,int dl,cchar *sp,int sl) noex

	Arguments:
	dp		destination string buffer
	dl		destination string buffer length
	sp		source string
	sl		source string length

	Returns:
	>=0		number of bytes in result
	<0		error

	See-also:
	snwcpy(3dam),
	snwcpylatin(3dam), 
	snwcpyopaque(3dam), 
	snwcpycompact(3dam), 
	snwcpyclean(3dam), 
	snwcpyhyphen(3dam), 
	snwcpylc(3dam),
	snwcpyuc(3dam),
	snwcpyfc(3dam),

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<limits.h>
#include	<string.h>
#include	<usystem.h>
#include	<sfx.h>
#include	<toxc.h>


/* external subroutines */


/* external variables */


/* exported subroutines */

int snabbr(char *dp,int dl,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ;
	if (dp && sp) {
	    int		cl ;
	    cchar	*cp ;
	    rs = SR_OK ;
	    if (dl < 0) dl = INT_MAX ;
	    if (sl < 0) sl = strlen(sp) ;
	    while ((cl = sfnext(sp,sl,&cp)) > 0) {
	        if (i < dl) {
	            dp[i++] = touc(cp[0]) ;
	        } else {
		    rs = SR_OVERFLOW ;
	        }
	        sl -= ((cp+cl)-sp) ;
	        sp = (cp+cl) ;
	        if (rs < 0) break ;
	    } /* end while */
	    dp[i] = '\0' ;
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (snabbr) */


