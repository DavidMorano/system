/* termcharset SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* set the terminal character sets */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This object package is finally finished!

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	termcharset

	Description:
	This subroutine formulates a string that constitutes a
	terminal escape sequence to set the terminal character sets.

	Synopsis:
	int termcharset(char *dp,int dl,int setnum,int f96,cc *fontname) noex

	Arguments:
	dp		destination buffer pointer
	dl		destination buffer length
	setnum		number of set to set:
				0	G0
				1	G1
				2	G2
				3	G3
	f96		flag (TRUE or FALSE) on whether specified set is a
			96-character character set
	fontname	string representing font to have set in place

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>		/* |INT_MAX| */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<storebuf.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

static cint	inter[4][2] = {
	{ '\050', 0 },			/* G0 */
	{ '\051', '-' },		/* G1 */
	{ '*', '.' },			/* G2 */
	{ '+', '/' }			/* G3 */
} ; /* intermediate characters */


/* exported variables */


/* exported subroutines */

int termcharset(char *dp,int dl,int setnum,int f96,cchar *fontname) noex {
	int		rs = SR_FAULT ;
	int		i = 0 ; /* return-value */
	if (dl < 0) dl = INT_MAX ;
	if (dp && fontname) ylikely {
	    rs = SR_INVALID ;
	    if ((setnum >= 0) && (setnum < 4)) ylikely {
	        if (f96 != 0) f96 = 1 ; /* make safe as an array index */
		/* setting a 96-character set to G0 is invalid */
	        if ((setnum != 0) || (! f96)) ylikely {
		    rs = SR_OK ;
		    /* construct escape sequence */
	            if (rs >= 0) {
	                cchar	*sp = "\033" ;
	                rs = storebuf_strw(dp,dl,i,sp,1) ;
	                i += rs ;
	            }
	            if (rs >= 0) {
	                cint	ich = inter[setnum][f96] ;
	                rs = storebuf_chr(dp,dl,i,ich) ;
	                i += rs ;
	            }
	            if (rs >= 0) {
	                rs = storebuf_strw(dp,dl,i,fontname,-1) ;
	                i += rs ;
	            }
	        } /* end if (valid) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? i : rs ;
}
/* end subroutine (termcharset) */


