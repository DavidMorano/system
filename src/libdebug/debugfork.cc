/* debugfork SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* debug-fork stubs */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	1		/* compile-time debugforkging */
#define	CF_UCOPEN	0		/* use 'uc_openprog(3uc)' */

/* revision history:

	= 1998-08-15, David A­D­ Morano
	This was written to debugfork the REXEC program.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Description:
	This modeule provides debugging support for the |uc_fork(3uc)|.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<csignal>
#include	<cstdlib>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<strlinelen.h>
#include	<localmisc.h>		/* |COLUMNS| + |TIMEBUFLEN| */

#define		<debugprint.h>
#define		<debugline.h>


/* local defines */

#define	RBUFLEN		100


/* external subroutines */

extern "C" {
    extern int	debugforkprintf(cchar *,...) noex ;
    extern int	debugforkprint(cchar *,int) noex ;
}


/* local structures */


/* forward subroutines */


/* external variables */


/* local variables */


/* exported variables */


/* exported subroutines */

int debugfork(cchar *s) noex {
	cint		of = O_RDWR ;
	int		rs ;
	cchar		*pf = "/home/dam/rje/proghello" ;

#if	CF_UCOPEN
	debugprintf("debugfork: uc_openprog() %s\n",s) ;
	rs = uc_openprog(pf,of,NULL,NULL) ;
	debugprintf("debugfork: uc_openprog() rs=%d\n",rs) ;
	if (rs >= 0) {
	    cint	rlen = RBUFLEN ;
	    cint	opts = 0 ;
	    cint	to = 4 ;
	    cint	fd = rs ;
	    char	rbuf[RBUFLEN+1] ;
	    while ((rs = uc_reade(fd,rbuf,rlen,to,opts)) > 0) {
	        int	len = rs ;
	        debugprintf("debugfork: l=>%r<\n",
	            rbuf,strlinelen(rbuf,len,40)) ;
	    } /* end while */
	    u_close(fd) ;
	}
#else /* CF_UCOPEN */
	debugprintf("debugfork: uc_fork() %s\n",s) ;
	rs = uc_fork() ;
	if (rs == 0) {
	    debugprintf("debugfork: child %s\n",s) ;
	    u_exit(0) ;
	}
#endif /* CF_UCOPEN */

	debugprintf("debugfork: ret rs=%d (parent)\n",rs) ;
	return rs ;
}
/* end subroutines (debugfork) */


