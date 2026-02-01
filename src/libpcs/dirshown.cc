/* dirshown SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* module to handle the "shown" status of directories */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* run-time debugging */

/* revision history:

	= 1995-05-01, David A­D­ Morano
	This code module was completely rewritten to replace any
	original garbage that was here before.

	= 1998-11-22, David A­D­ Morano
        I did some clean-up.

*/

/* Copyright © 1995,1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	dirshown

	Description:
	This module is a simple mechanism to track what articles
	have been read by the user.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usyscalls.h>
#include	<uclibmem.h>
#include	<intsat.h>
#include	<vechand.h>
#include	<mkdirlist.h>
#include	<localmisc.h>

#include	"dirshown.h"


/* local defines */

#define	DS		dirshown
#define	DS_ENT		mkdirlist_ent


/* local typedefs */

typedef DS_ENT *	entp ;


/* external subroutines */


/* external variables */


/* local forward references */

local int	cmpdir(DS_ENT *,DS_ENT *) noex ;


/* external variables */


/* local variables */


/* exported variables */


/* exported subroutines */

int dirshown_start(DS *op) noex {
    	int		rs = SR_FAULT ;
	if (op) {
    	   cint	vn = 20 ;
	   cint	vo = 0 ;
	   rs = op->start(vn,vo) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (dirshown_start) */

int dirshown_finish(DS *op) noex {
    	int		rs = SR_FAULT ;
	if (op) {
	    rs = op->finish ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (dirshown_finish) */

/* add a directory to the list of those which have been shown already */
int dirshown_set(DS *op,DS_ENT *dsp) noex {
    	int		rs = SR_FAULT ;
	if (op) {
	    rs = op->add(dsp) ;
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (dirshown_set) */

int dirshown_already(DS *op,DS_ENT *dsp,DS_ENT **rpp) noex {
	int		rs = SR_FAULT ;
	if (op && rpp) {
	    void *vp ;
	    for (int i = 0 ; (rs = op->get(i,&vp)) >= 0 ; i += 1) {
		DS_ENT *ep = entp(vp) ;
	        if (vp) {
	            if (cmpdir(dsp,ep) == 0) {
			*rpp = ep ;
			break ;
		    }
		} /* end if (non-null) */
	    } /* end for */
	    if (rs < 0) {
	        *rpp = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (dirshown_already) */


/* local subroutines */

local int cmpdir(DS_ENT *dsp1,DS_ENT *dsp2) noex {
    	int		rc = 0 ;
	if (dsp1->dev != dsp2->dev) {
	    rc = (dsp1->dev > dsp2->dev) ? +1 : -1 ;
	} else if (dsp1->ino != dsp2->ino) {
	    rc = (dsp1->ino > dsp2->ino) ? +1 : -1 ;
	}
	return rc ;
}
/* end subroutine (cmpdir) */


