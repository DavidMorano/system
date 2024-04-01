/* logfile_printfold HEADER */
/* lang=C++20 */

/* perform logging operations on a file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-01, David A­D­ Morano
	This object module was originally written to create a logging
	mechanism for PCS application programs.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This is an extra method for LOGFILE.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstring>
#include	<usystem.h>
#include	<linefold.h>
#include	<localmisc.h>

#include	"logfile.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int logfile_printfi(logfile *,int,cchar *,int,cchar *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int logfile_printfold(logfile *lhp,cchar *pre,cchar *sp,int sl) noex {
	linefold	fo ;
	cint		pl = strlen(pre) ;
	int		rs ;
	int		rs1 ;
	int		n ;
	n = (LOGFILE_FMTLEN - pl - 2) ;
	if ((rs = linefold_start(&fo,n,0,sp,sl)) >= 0) {
	    int		c = 0 ;
	    cchar	*sp{} ;
	    for (int i = 0 ; (rs = linefold_getline(&fo,i,&sp)) > 0 ; i += 1) {
		int	sl = rs ;
		while (sl > n) {
	            rs = logfile_printfi(lhp,c++,pre,pl,sp,n) ;
		    sl -= n ;
		    sp = (sp+n) ;
		    if (rs < 0) break ;
		} /* end while */
		if ((rs >= 0) && (sl > 0)) {
	            rs = logfile_printfi(lhp,c++,pre,pl,sp,sl) ;
		} /* end if */
		if (rs < 0) break ;
	    } /* end for */
	    rs1 = linefold_finish(&fo) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (linefold) */
	return rs ;
}
/* end subroutine (logfile_printfold) */


/* local subroutines */

static int logfile_printfi(logfile *lhp,int li,cchar *pp,int pl,
		cchar *sp,int sl) noex {
	int		rs ;
	if (li == 0) {
	    cchar	*f0 = "%s| %t" ;
	    rs = logfile_printf(lhp,f0,pp,sp,sl) ;
	} else {
	    cchar	*f1 = "%*s| %t" ;
	    rs = logfile_printf(lhp,f1,pl," ",sp,sl) ;
	}
	return rs ;
}
/* end subroutine (logfile_printfi) */


