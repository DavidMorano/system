/* isdirempty SUPPORT */
/* lang=C20 */

/* is the given directory empty? */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* compile-time debug print-outs */
#define	CF_DEBUGN	0		/* special debugging */

/* revision history:

	= 1998-03-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	isdirempty

	Description:
	We test if the given directory is empty.

	Synopsis:
	int isdirempty(cchar *dname)

	Arguments:
	dname		directory name (as a string)

	Returns:
	>0		is empty
	==		not empty
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<stdlib.h>
#include	<string.h>
#include	<usystem.h>
#include	<fsdir.h>
#include	<mkpathx.h>
#include	<hasx.h>
#include	<ischarx.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */

#if	CF_DEBUGS
extern int	debugprintf(cchar *,...) ;
extern int	strlinelen(cchar *,int,int) ;
#endif


/* external variables */


/* global variables */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int isdirempty(cchar *dname) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		f = TRUE ;
	if (dname) {
	    rs = SR_INVALID ;
	    if (dname[0]) {
	         fsdir		d ;
	         fsdir_ent	de ;
	         if ((rs = fsdir_open(&d,dname)) >= 0) {
	             while ((rs = fsdir_read(&d,&de)) > 0) {
	                if (hasNotDots(de.name,rs) > 0) {
		             f = FALSE ;
		         } /* end (not dots) */
		         if (!f) break ;
	             } /* end while */
	             rs1 = fsdir_close(&d) ;
	             if (rs >= 0) rs = rs1 ;
	         } /* end if (fsdir) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (isdirempty) */


