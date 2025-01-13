/* checklockfile SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* check if we still own the lock file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*****************************************************************************

  	Name:
	checklockfile

	Description:
	We want to check that we still own the lock file and also
	update it by writting a current time into it.

*****************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<sys/wait.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<bfile.h>
#include	<cfdec.h>
#include	<timestr.h>
#include	<localmisc.h>		/* |TIEBUFLEN| + |DIGBUFLEN| */

#include	"proginfo.hh"


/* local defines */

#define	PI		proginfo


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	progchecker(PI *,bfile *,time_t) noex ;


/* local variables */

constexpr int	dlen = DIGBUFLEN ;


/* exported variables */


/* exported subroutines */

int progchecklock(PI *pip,bfile *fp,cc *fn,cc *bn,cc *dt) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		rpid = 0 ;
	if (pip && fp && fn) {
	    if ((rs = bseek(fp,0z,SEEK_SET)) >= 0) {
	        char	dbuf[dlen + 1] ;
	        if ((rs = breadln(fp,dbuf,dlen)) >= 0)  {
		    if (int opid  ; (rs = cfdec(dbuf,rs,&opid)) >= 0)  {
		        if ((rpid = pip->pid) == opid) {
			    cint	cmd = BC_STAT ;
	    		    if (USTAT sb ; (rs = bcontrol(fp,cm,&sb) >= 0) {
				if (USTAT sbo ; (rs = uc_stat(fn,&sbo)) >= 0) {
				    bool	f = false ;
				    f = f || (sb.st_dev != svo.st_dev) ;
				    f = f || (sb.st_ino != svo.st_ino) ;
				    rs = SR_LOCKLOST ;
				    if (f) {
				        rs = progchecker(pip,fp,dt) ;
				    }
				} /* end if (uc_stat) */
			    } /* end if (bcontrol) */
		        } /* end if (PID comparison) */
		    } /* end if (cfdec) */
		} /* end if (breadln) */
	    } /* end if (bseek) */
	} /* end if (non-null) */
	return (rs >= 0) ? rpid : rs ;
}
/* end subroutine (progchecklockfile) */

static int progchecker(PI *pip,bfile *fp,time_t dt) noex {
    	cnullptr	np{} ;
	int		rs ;
	if (char *lbuf ; (rs = malloc_ml(&lbuf)) >= 0) {
	    if ((rs = breadln(fp,lbuf,rs)) >= 0) { /* <- throw away */
		/* write an updated time at this offset in the file */
		char	timebuf[TIMEBUFLEN + 1] ;
		if (timestr_logz(dt,timebuf) != np) {
		    if ((rs = bprintf(fp,"%s",timebuf)) >= 0) {
		        rs = bflush(fp) ;
		    }
		} /* end if (timestr_logz) */
	    } /* end if (breadln) */
	    rs = rsfree(rs,lbuf) ;
	} /* end if (m-a-f) */
	return rs ;
}
/* end subroutine (progchecker) */


