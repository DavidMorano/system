/* hasnonpath */
/* lang=C++20 */

/* determine if the given string represent a non-path filename */


/* revision history:

	= 1998-04-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	hasnonpath

	Description:
	We test a string to see if it is a floating or non-path filename.

	Synopsis:
	int hasnonpath(cchar *pp,int pl) noex

	Arguments:
	- pp	pointer to path string
	- pl	length of given path string

	Returns:
	<0	error
	==0	no
	>0	yes and this is the index of the type of non-path

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<string.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */

extern int	sichr(const char *,int,int) ;

extern char	*strnpbrk(const char *,int,const char *) ;


/* external variables */


/* local structures */


/* forward references */


/* local variables */

static cchar		*nonpaths = "/¥§~" ;

enum nonpaths {
	nonpath_not,
	nonpath_dialer,
	nonpath_fsvc,
	nonpath_usvc,
	nonpath_overlast
} ;


/* exported subroutines */

int hasnonpath(cchar *fp,int fl) noex {
	int		f = (fp[0] != '/') ;
	if (fl < 0) fl = strlen(fp) ;
	if (f) {
	    cchar	*tp = strnpbrk(fp,fl,nonpaths) ;
	    f = FALSE ;
	    if ((tp != NULL) && ((tp-fp) > 0) && (tp[1] != '\0')) {
	        f = sichr(nonpaths,-1,*tp) ;
	    }
	}
	return f ;
}
/* end subroutine (hasnonpath) */


