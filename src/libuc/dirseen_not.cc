/* dirseen_not SUPPORT */
/* lang=C++20 */

/* some extra utility subroutines (methods) for the DIRSEEN object */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	dirseen_not

	Description:
	This pair of subroutines makes finding previously not seen
	directories easier.

	Synopsis:
	int dirseen_notseen(dirseen *op,USTAT *sbp,cchar *dbuf,int dlen) noex

	Arguments:
	op		pointer to object
	sbp		pointer to USTAT structure
	dbuf		directory buffer
	dlen		length of string in directory buffer

	Returns:
	>0		not yet seen
	==0		previously seen
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<limits.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<pathclean.h>
#include	<isnot.h>
#include	<localmisc.h>

#include	"dirseen.h"


/* local defines */


/* external subroutines */

extern "C" {
    extern int dirseen_notseen(DIRSEEN *,USTAT *,cchar *,int) noex ;
    extern int dirseen_notadd(DIRSEEN *,USTAT *,cchar *,int) noex ;
}

/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int dirseen_notseen(DIRSEEN *dsp,USTAT *sbp,cchar *dbuf,int dlen) noex {
	cint		nrs = SR_NOENT ;
	int		rs = SR_FAULT ;
	int		f = FALSE ;
	if (dsp && sbp && dbuf) {
	    sbp->st_ino = 0 ;
	    sbp->st_dev = 0 ;
	    if ((rs = dirseen_havename(dsp,dbuf,dlen)) == nrs) {
	        if ((rs = u_stat(dbuf,sbp)) >= 0) {
	            if (S_ISDIR(sbp->st_mode)) {
	    	        if ((rs = dirseen_havedevino(dsp,sbp)) == nrs) {
			    rs = SR_OK ;
			    f = TRUE ;
		        }
		    }
	        } else if (isNotPresent(rs)) {
		    rs = SR_OK ;
	        }
	    }
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (dirseen_notseen) */

int dirseen_notadd(DIRSEEN *dsp,USTAT *sbp,cchar *dbuf,int dlen) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (dsp && sbp && dbuf) {
	    char	*cbuf{} ;
	    if ((rs = malloc_mp(&cbuf)) >= 0) {
	        if ((rs = pathclean(cbuf,dbuf,dlen)) > 0) {
	            rs = dirseen_add(dsp,cbuf,rs,sbp) ;
	        }
	        rs1 = uc_free(cbuf) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (m-a-f) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (dirseen_notadd) */


