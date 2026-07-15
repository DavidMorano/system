/* bopenroot SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* open a file name according to rules */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* compile-time debugging */

/* revision history:

	= 1998-09-01, David A­D­ Morano
	This program was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	bopenroot

	Description:
	This subroutine will form a file name according to some rules.
	The rules are roughly:

	+ attempt to open it directly if it is already rooted
	+ open it if it is already in the root area
	+ attempt to open it as it is if it already exists
	+ attempt to open or create it located in the root area
	+ attempt to open or create it as it is

	Synopsis:
	int bopenroot(bfile *fp,cc *pr,cc *fn,char *rbuf,
		int of,mode_t om) noex

	Arguments:
	fp		pointer to 'bfile' object
	pr		path of program root directory
	fn		file-name to find and open
	rbuf		supplied buffer to hold possible resulting name
	of		file open flags
	om		file ooms to use in the open 

	Returns:
	>=0		success (same as 'bopen()')
	<0		error (same as 'bopen()')

	rbuf		1. zero length string if no new name was needed
			2. will contain the path of the file that was opened

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD |strchr(3c)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<outbuf.h>
#include	<mkpathx.h>
#include	<permx.h>
#include	<localmisc.h>

#include	"bfile.h"

/* local defines */


/* external subroutines */


/* external variables */


/* forward references */


/* local structures */


/* local variables */


/* exported variables */


/* exported subroutines */

int bopenroot(bfile *fp,cc *pr,cc *fn,char *rbuf,cc *of,mode_t om) noex {
	outbuf		ob ;
	int		rs = SR_OK ;
	int		imode ;
	int		f_outbuf = FALSE ;
	cchar	*mp ;
	char		*onp = NULL ;

	if (fp == NULL) return SR_FAULT ;
	if (fn == NULL) return SR_FAULT ;

	if (fn[0] == '\0') return SR_INVALID ;

	f_outbuf = (rbuf != NULL) ;

	imode = 0 ;
	for (mp = mode ; *mp ; mp += 1) {
	    cint	sch = mkchar(*mp) ;
	    switch (sch) {
	    case 'r':
	        imode += R_OK ;
	        break ;
	    case 'w':
	        imode += W_OK ;
	        break ;
	    case 'x':
	        imode += X_OK ;
	        break ;
	    } /* end switch */


	if (fn[0] == '/') {

	    if (f_outbuf)
	        rbuf[0] = '\0' ;

	    rs = bopen(fp,fn,mode,oom) ;

	    goto ret0 ;

	} /* end if */

	rs = outbuf_start(&ob,rbuf,-1) ;
	if (rs < 0)
	    goto ret0 ;

	if (pr != NULL) {

	    rs = outbuf_get(&ob,&onp) ;
	    if (rs < 0)
	        goto done ;

	    rs = mkpath2(onp, pr,fn) ;

	    if (rs >= 0)
	        rs = perm(onp,-1,-1,NULL,imode) ;

	    if (rs >= 0)
	        rs = bopen(fp,onp,mode,oom) ;

	    if (rs >= 0)
	        goto done ;

	} /* end if (we had a pr) */

	if ((perm(fn,-1,-1,NULL,imode) >= 0) &&
	    ((rs = bopen(fp,fn,mode,oom)) >= 0)) {

	    if (f_outbuf)
	        rbuf[0] = '\0' ;

	    goto done ;
	}

	if ((pr != NULL) &&
	    (strchr(fn,'/') != NULL)) {

	    rs = bopen(fp,onp,mode,oom) ;

	    if (rs >= 0)
	        goto done ;

	}

	if (f_outbuf)
	    rbuf[0] = '\0' ;

	rs = bopen(fp,fn,mode,oom) ;

done:
	outbuf_finish(&ob) ;

ret0:
	return rs ;
} /* end subroutine (bopenroot) */


