/* bopenmod SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* "Basic I-O" package similiar to some other thing whose initials is "stdio" */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-07-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	bopenmod

  	Description:
	We open with a minimum mode equal to the mode passed to the
	open call.  Otherwise everything is identical to |bopen(3b)|.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstring>		/* |strchr(3c)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<conallof.h>		/* |conallof(3uc)| */
#include	<localmisc.h>
#include	<libdebug.h>		/* LIBDEBUG */

#include	"bfile.h"


/* local defines */


/* external subroutines */

extern "C" {
    extern int uc_fminmod(int,mode_t) noex ;
}


/* external variables */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int bopenmod(bfile *fp,cchar *fname,cchar *of,mode_t om) noex {
    	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	if (fp && fname && of) {
	    if ((rs = bopen(fp,fname,of,om)) >= 0) {
	        if (strchr(of,'M') == np) { /* not already done! */
	            if ((rs = conallof(of,-1,"wc")) > 0) {
	                fp->of |= O_MINMODE ;
	                rs = uc_fminmod(fp->fd,om) ;
	            }
	        } /* end if (requested) */
	        if (rs < 0) {
	            bclose(fp) ;
	        }
	    } /* end if (bopen) */
	} /* end if (non-null) */
	return rs ;
}
/* end routine (bopenmod) */


