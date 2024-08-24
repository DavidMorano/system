/* makedate_date */

/* subroutine to get the date out of the 'makedate' string */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-03-01, David A­D­ Morano
	This code module was originally written.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

        This subroutine is a utility for extracting the date from a 'makedate'
        string (which just about ever program is compiled with).


******************************************************************************/


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<stdlib.h>
#include	<string.h>
#include	<usystem.h>
#include	<char.h>
#include	<mkchar.h>
#include	<ascii.h>
#include	<localmisc.h>


/* local defines */


/* external routines */

extern int	isalphalatin(int) ;


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int makedate_date(cchar *md,cchar **rpp) noex {
	int		rs = SR_NOENT ;
	int		ch ;
	const char	*sp ;
	const char	*cp ;
	if (rpp != NULL) *rpp = NULL ;
	if ((cp = strchr(md,CH_RPAREN)) != NULL) {
	    while (CHAR_ISWHITE(*cp)) cp += 1 ;
	    ch = MKCHAR(*cp) ;
	    if (! isdigitlatin(ch)) {
	        while (*cp && (! CHAR_ISWHITE(*cp))) cp += 1 ;
	        while (CHAR_ISWHITE(*cp)) cp += 1 ;
	    } /* end if (skip over the name) */
	    sp = cp ;
	    if (rpp != NULL) *rpp = cp ;
	    while (*cp && (! CHAR_ISWHITE(*cp))) {
	        cp += 1 ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? (cp - sp) : rs ;
}
/* end subroutine (makedate_get) */


