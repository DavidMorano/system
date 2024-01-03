/* vecstr_avmkstr SUPPORT */
/* lang=C20 */

/* make the Array-Vector and the String-table */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	These routines are used when the caller wants to store a COPY of the
	passed string data into a vector.  These routines will copy and store
	the copied data in the list.  The advantage is that the caller does not
	have to keep the orginal data around in order for the list data to be
	accessed later.  String data (unlike "element" data) can not contain
	nullptr characters-bytes.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<stdlib.h>
#include	<string.h>
#include	<usystem.h>
#include	<localmisc.h>

#include	"vecstr.h"


/* local defines */


/* external subroutines */

extern int	iceil(int,int) ;

extern char	*strwcpy(char *,const char *,int) ;


/* forward references */

static int vecstr_cksize(vecstr *) noex ;


/* local variables */


/* exported subroutines */

int vecstr_avmkstr(vecstr *op,cchar **av,int avs,char *tab,int tabs) noex {
	int		rs = SR_FAULT ;
	int		c = 0 ;
	if (op && av && tab) {
	    if ((rs = vecstr_cksize(op)) >= 0) {
		int	size = iceil(op->stsize,sizeof(int)) ;
		rs = SR_OVERFLOW ;
	        if (tabs >= size) {
	            size = (op->c + 1) * sizeof(int) ;
	            if (avs >= size) {
		        char	*bp = tab ;
		        rs = SR_OK ;
	                *bp++ = '\0' ;
	                for (int i = 0 ; i < op->i ; i += 1) {
		            cchar	*ep = op->va[i] ;
	                    if (ep) {
	                        av[c++] = bp ;
	                        bp = strwcpy(bp,ep,-1) + 1 ;
		            }
	                } /* end for */
	                av[c] = nullptr ;
	            }
	        }
	    } /* end if (vecstr_cksize) */
	} /* end if (non-null) */
	return (rs >= 0) ? c : rs ;
}
/* end subroutine (vecstr_avmkstr) */


/* local subroutines */

static int vecstr_cksize(vecstr *op) noex {
	int		rs = SR_OK ;
	if (op->stsize == 0) {
	    rs = vecstr_strsize(op) ;
	}
	return rs ;
}
/* end subroutine (vecstr_cksize) */


