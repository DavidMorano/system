/* bbspec */
/* lang=C20 */

/* load a bible-book-specification */
/* version %I% last-modified %G% */


/* revision history:

	= 2013-02-07, David A­D­ Morano
	This code was originally written.  

*/

/* Copyright © 2013 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This object parses and loads a given bible-book-specification
	string into itself.  The given day-specification string
	looks like:
		<bookname>[:]<chapter>[:<verse>]
	or
		<booknum>:<chapter>[:<verse>]

	Examples are:
		mat5:1
		mat:5:1
		40:5:1

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<limits.h>
#include	<stdlib.h>
#include	<string.h>
#include	<ctype.h>
#include	<usystem.h>
#include	<char.h>
#include	<mkchar.h>
#include	<estrings.h>
#include	<localmisc.h>

#include	"bbspec.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local typedefs */

#ifndef	TYPEDEF_CCHAR
#define	TYPEDEF_CCHAR
typedef cchar	cchar ;
#endif


/* local structures */


/* forward references */

static int	siourbrk(cchar *,int,int) noex ;


/* local variables */


/* exported subroutines */

int bbspec_load(BBSPEC *op,cchar *sp,int sl) noex {
	int		rs = SR_OK ;
	int		ch ;
	int		v ;
	int		si ;
	int		tl ;
	cchar	*tp ;

	if (op == NULL) return SR_FAULT ;
	if (sp == NULL) return SR_FAULT ;

	memset(op,0,sizeof(BBSPEC)) ;
	op->v = 1 ;

	if (sl < 0) sl = strlen(sp) ;

	if ((tl = sfshrink(sp,sl,&tp)) > 0) {
	    sp = tp ;
	    sl = tl ;
	}

	if (sl > 0) {
	    ch = mkchar(sp[0]) ;
	    if (isalphalatin(ch)) {
	        if ((si = siourbrk(sp,sl,TRUE)) > 0) {
		    op->np = sp ;
	  	    op->nl = si ;
		    sp += si ;
		    sl -= si ;
		    if (sl > 0) {
			ch = mkchar(sp[0]) ;
		 	if (ch == ':') {
			    sp += 1 ;
			    sl -= 1 ;
			}
		    }
	        }
	    } else if (isdigitlatin(ch)) {
	        if ((si = siourbrk(sp,sl,TRUE)) > 0) {
		    rs = cfdeci(sp,si,&v) ;
		    op->b = v ;
	        }
	    } else
	        rs = SR_DOM ;
	    if ((rs >= 0) && (sl > 0)) {
		if ((tp = strnchr(sp,sl,':')) != NULL) {
		    if ((rs = cfdeci(sp,(tp-sp),&v)) >= 0) {
			op->b = v ;
			sl -= ((tp+1)-sp) ;
			sp = (tp+1) ;
			if (sl > 0) {
		    	    rs = cfdeci(sp,sl,&v) ;
		    	    op->v = v ;
			}
		    }
		} else {
		    rs = cfdeci(sp,sl,&v) ;
		    op->b = v ;
		}
	    } else
	        rs = SR_DOM ;
	} else
	    rs = SR_DOM ;

#if	CF_DEBUGS
	debugprintf("bbspec_parse: ret rs=%d\n",rs) ;
	debugprintf("bbspec_parse: ret b=%d c=%d v=%d\n",op->v,op->c,op->v) ;
#endif

	return rs ;
}
/* end subroutine (bbspec_load) */


/* local subroutines */

static int siourbrk(cchar *sp,int sl,int f_dig) noex {
	int	i = -1 ;
	int	ch ;
	int	f = FALSE ;
	for (i = 0 ; i < sl ; i += 1) {
	    ch = (sp[i] & 0xff) ;
	    if (f_dig) {
		f = isdigitlatin(ch) ;
	    } else
		f = isalphalatin(ch) ;
	    f = f || (ch == ':') ;
	    if (f) break ;
	} /* end for */
	return (f) ? i : -1 ;
}
/* end subroutine (siourbrk) */


