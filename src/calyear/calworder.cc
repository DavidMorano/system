/* calworder */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* word management */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* non-switchable debug print-outs */

/* revision history:

	- 2008-10-01, David A­D­ Morano
	This object module was originally written.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	We manage words, for calendars.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<estrings.h>
#include	<sbuf.h>
#include	<sfx.h>
#include	<localmisc.h>

#include	"calworder.h"


/* local defines */


/* external subroutines */

extern int	snsds(char *,int,cchar *,cchar *) ;
extern int	snwcpy(char *,int,cchar *,int) ;
extern int	sncpy1(char *,int,cchar *) ;
extern int	mkpath2(char *,cchar *,cchar *) ;
extern int	mkpath3(char *,cchar *,cchar *,cchar *) ;
extern int	mkfnamesuf1(char *,cchar *,cchar *) ;
extern int	mkfnamesuf2(char *,cchar *,cchar *,cchar *) ;
extern int	sibrk(cchar *,int,cchar *) ;
extern int	siskipwhite(cchar *,int) ;
extern int	matstr(cchar **,cchar *,int) ;
extern int	matcasestr(cchar **,cchar *,int) ;
extern int	matocasestr(cchar **,int,cchar *,int) ;
extern int	matpcasestr(cchar **,int,cchar *,int) ;
extern int	nleadstr(cchar *,cchar *,int) ;
extern int	cfdeci(cchar *,int,int *) ;
extern int	cfdecui(cchar *,int,uint *) ;
extern int	isdigitlatin(int) ;

#if	CF_DEBUGS
extern int	debugprintf(cchar *,...) ;
extern int	strlinelen(cchar *,int,int) ;
#endif


/* local structures */


/* forward references */


/* exported variables */


/* local variables */


/* exported subroutines */


int calworder_start(CALWORDER *wp,cchar *md,CALENT *ep)
{
	CALENT_LINE	*lines = ep->lines ;
	wp->i = 0 ;
	wp->nlines = ep->i ;
	wp->lines = ep->lines ;
	wp->md = md ;
	if (lines != NULL) {
	    wp->sp = (md + lines[0].loff) ;
	    wp->sl = (lines[0].llen) ;
	}

	return SR_OK ;
}
/* end subroutine (calworder_start) */


/* ARGSUSED */
int calworder_finish(CALWORDER *wp)
{
	if (wp == NULL) return SR_FAULT ;
	return SR_OK ;
}
/* end subroutine (calworder_finish) */


int calworder_get(CALWORDER *wp,cchar **rpp)
{
	int		cl = 0 ;
	cchar	*cp = NULL ; /* ¥ GCC is stupid! */

	while (wp->i < wp->nlines) {
	    if ((cl = nextfield(wp->sp,wp->sl,&cp)) > 0) {
	        wp->sl -= ((cp + cl) - wp->sp) ;
	        wp->sp = (cp + cl) ;
	    } else {
	        wp->i += 1 ;
	        if (wp->i < wp->nlines) {
	            wp->sp = (wp->md + wp->lines[wp->i].loff) ;
	            wp->sl = (wp->lines[wp->i].llen) ;
	        }
	    }
	    if (cl > 0) break ;
	} /* end while */

	if (rpp != NULL)
	    *rpp = cp ;

	return cl ;
}
/* end subroutine (calworder_get) */


