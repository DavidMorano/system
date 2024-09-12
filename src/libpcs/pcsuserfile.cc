/* pcsuserfile SUPPORT */
/* lang=C++20 */

/* update a record in a PCS userfile */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	pcsuserfile

	Description:
	This subroutine maintains PCS "users" files.  These files
	are used to maintain the names and counts of the users that
	invoke certain PCS progams.  This is generally for development
	debugging purposes for the programs that log the user
	invocations.

	Synopsis:
	int pcsuserfile(cc *pcs,cc *urfn,cc *nn,cc *un,cc *name) noex

	Arguments:
	pcs		PCS programroot
	ufn		name of file relative to programroot
	nn		machine node name
	un		users login un
	name		real or fullname of the user

	Returns:
	==1		record created
	==0		record written
	<0		error code (system-return)


	Format of file records:

	- eight digits of a decimal count number field
	- the time of the last entry update
	- the nn and un
	- an optional real or fullname of the user

	Format example:

     989 111115_1137:06_EST      TOTAL
     775 111115_1137:06_EST      rca!local (LOCAL)
     200 111001_2015:04_EDT      rca!dam (David A­D­ Morano)
      14 110103_0922:32_EST      rca!genserv (GENSERV)


	Note that a special user field with the value of "TOTAL"
	maintains a total count of all program invocations separately
	from the user counts.  Oh, and finally, since file record
	locking sucks (that is: it is very often BROKEN) on the
	stupid Sun platforms, we attempt to lock the file to be a
	nice guy but we have relatively small timeouts after which
	we proceed anyway!  This strategy is applied to all record
	locks used throughout this subroutine.


*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<mallocxx.h>
#include	<useraccdb.h>
#include	<nulstr.h>
#include	<strn.h>
#include	<sfx.h>
#include	<snx.h>
#include	<localmisc.h>

#include	"pcsuserfile.h"


/* local defines */


/* external subroutines */


/* external variables */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int pcsuserfile(cc *pr,cc *ufn,cc *nn,cc *un,cc *name) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		f_created = false ;
	if (pr && ufn && nn && un) {
	    rs = SR_INVALID ;
	    if (pr[0] && ufn[0] && nn[0] && un[0]) {
	        int	bnl ;
	        cchar	*bnp ;
	        if ((bnl = sfbasename(ufn,-1,&bnp)) > 0) {
	            cchar	*tp = strnrchr(bnp,bnl,'.') ;
	            char	*ubuf{} ;
	            if (tp) bnl = (tp-bnp) ;
	            if ((rs = malloc_un(&ubuf)) >= 0) {
		        cint	ulen = rs ;
	                if ((rs = sncpy(ubuf,ulen,nn,"!",un)) >= 0) {
		            nulstr	dbs ;
		            cchar	*dbn ;
	                    if ((rs = nulstr_start(&dbs,bnp,bnl,&dbn)) >= 0) {
	                        useraccdb	udb ;
	                        if ((rs = useraccdb_open(&udb,pr,dbn)) >= 0) {
			            {
		                        f_created = (rs > 0) ;
		                        rs = useraccdb_update(&udb,ubuf,name) ;
			            }
	                            rs1 = useraccdb_close(&udb) ;
	                            if (rs >= 0) rs = rs1 ;
	                        } /* end if (useraccdb) */
	                        rs1 = nulstr_finish(&dbs) ;
	                        if (rs >= 0) rs = rs1 ;
	                    } /* end if (nulstr) */
	                } /* end if (sncpy) */
		        rs1 = uc_free(ubuf) ;
		        if (rs >= 0) rs = rs1 ;
		    } /* end if (m-a-f) */
	        } /* end if (sfbasename) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? f_created : rs ;
}
/* end subroutine (pcsuserfile) */


