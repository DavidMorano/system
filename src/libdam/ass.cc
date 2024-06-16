/* ass SUPPORT */
/* lang=C++20 */

/* experimenal string manipulation object */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */


#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>
#include	<localmisc.h>

#include	"ass.h"


/* local defines */

#define	ASS_ADDLEN	100


/* local variables */


/* exported variables */


/* exported subroutines */

int ass_start(ASS *asp) noex {
	int		rs = SR_FAULT ;
	if (asp) {
	    asp->len = 0 ;
	    asp->e = 0 ;
	    asp->s = nullptr ;
	} /* end if (non-null) */
	return rs ;
}

int ass_add(ASS *asp,int c) noex {
	int		rs = SR_FAULT ;
	int		sz ;
	if (asp) {
	    caddr_t	p ;
	    if (asp->s) {
	        asp->len = 0 ;
	        asp->e = ASS_ADDLEN ;
	        sz = asp->e ;
	        rs = uc_malloc(sz,&p) ;
	    } else if (asp->e == asp->len) {
	        asp->e += ASS_ADDLEN ;
	        sz = asp->e ;
	        rs = uc_realloc(asp->s,sz,&p) ;
	    }
	    if (rs >= 0) {
	        asp->s = p ;
	        asp->s[(asp->len)++] = c ;
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? asp->len : rs ;
}

int ass_finish(ASS *asp) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (asp) {
	    rs = SR_OK ;
	    if (asp->s) {
	        rs1 = uc_free(asp->s) ;
	        if (rs >= 0) rs = rs1 ;
	        asp->s = nullptr ;
	    }
	    asp->len = 0 ;
	    asp->e = 0 ;
	} /* end if (non-null) */
	return rs ;
}


