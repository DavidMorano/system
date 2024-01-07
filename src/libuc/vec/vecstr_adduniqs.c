/* vecstr_adduniqs SUPPORT */
/* lang=C20 */

/* add string(s) to a vector-string object */
/* version %I% last-modified %G% */


/* revision history:

	= 2004-06-02, David A�D� Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright � 2004 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	vecstr_adduniqs

	Description:
	We add strings to the list, but we only add a given string
	once. So all of the strings in the list should end up being
	unique.

	Synopsis:
	int vecstr_adduniqs(vecstr *vlp,cchar *sp,int sl) noex

	Arguments:
	vlp		pointer to object
	sp		source string pointer
	sl		source string length

	Returns:
	>=0		number of strings entered
	<0		error

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<limits.h>
#include	<stdlib.h>
#include	<string.h>
#include	<usystem.h>
#include	<estrings.h>
#include	<vecstr.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */

extern int	sfshrink(cchar *,int,cchar **) noex ;
extern int	vecstr_adduniq(vecstr *,cchar *,int) noex ;

extern char	*strnpbrk(const char *,int,const char *) noex ;


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int vecstr_adduniqs(vecstr *qlp,cchar *sp,int sl) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (qlp && sp) {
	    int		cl ;
	    cchar	*cp ;
	    cchar	*tp ;
	    rs = SR_OK ;
	    if (sl < 0) sl = strlen(sp) ;
	    while ((tp = strnpbrk(sp,sl," ,")) != NULL) {
	        if ((cl = sfshrink(sp,(tp-sp),&cp)) > 0) {
	            rs = vecstr_adduniq(qlp,cp,cl) ;
	            c += ((rs < INT_MAX) ? 1 : 0) ;
	        }
	        sl -= ((tp+1)-sp) ;
	        sp = (tp+1) ;
	        if (rs < 0) break ;
	    } /* end while */
	    if ((rs >= 0) && (sl > 0)) {
	        if ((cl = sfshrink(sp,sl,&cp)) > 0) {
	            rs = vecstr_adduniq(qlp,cp,cl) ;
	            c += ((rs < INT_MAX) ? 1 : 0) ;
	        }
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecstr_adduniqs) */


