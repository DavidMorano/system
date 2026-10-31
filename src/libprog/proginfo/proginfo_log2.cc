/* proginfo_log2 MODULE (implementation) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* process the service names given us */
/* version %I% last-modified %G% */


/* revision history:

	= 2008-09-01, David A­D­ Morano
	This subroutine was adopted from the DWD program.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	proglogout

	Description:
	This subroutine prints out the standard-output or standard-error
	of a spawned process to the log-file service.

	Returns:
	OK	may not really matter in the current implementation!

*******************************************************************************/

module ;

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* LIBUC */
#include	<sys/param.h>		/* LIBUC */
#include	<unistd.h>		/* LIBUC */
#include	<climits>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ucmem.h>		/* LIBUC */
#include	<linefold.h>		/* LIBUC */
#include	<logfile.h>		/* LIBUC */
#include	<rmx.h>			/* LIBUC */
#include	<isch.h>		/* LIBUC */
#include	<localmisc.h>		/* LIBU */
#include	<bfile.h>		/* LIBB */

#include	<proginfo.hh>

#pragma		GCC dependency		"mod/libutil.ccm"

module proginfo.log ;

import libutil ;			/* |lenstr(3u)| */
import uconstants ;			/* |syswords(3u)| */
import proginfo.base ;

/* local defines */

#ifndef	MAXOUTLEN
#define	MAXOUTLEN	64
#endif


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* externals variables */


/* local structures */


/* forward references */

local int procfile	(PI *,cchar *,int,cchar *) noex ;
local int procline	(PI *,int,cchar *,int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int proglog_out(PI *pip,cchar *msgstr,cchar *fname) noex {
	int		rs = SR_OK ;
	int		wlen = 0 ; /* return-value */
	if (fname) ylikely {
	    if (fname[0]) ylikely {
	        if (pip->pfopen.logprog) {
	    	    int msglen = 0 ;
	            if (msgstr) {
	                msglen = rmtrailchr(msgstr,-1,'>') ;
	            } /* end if (non-null) */
	            rs = procfile(pip,msgstr,msglen,fname) ;
	            wlen = rs ;
	        } /* end if (logprog) */
	    } /* end if (non-nul) */
	} /* end if (fname) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (proglog_out) */


/* local subroutines */

local int procfile(PI *pip,cchar *sp,int sl,cchar *fname) noex {
	cint		columns = LOGFILE_FMTLEN ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ; /* return-value */
	if (char *lbuf ; (rs = mem.ml(&lbuf)) >= 0) {
	    cint llen = rs ;
	    if (bfile of ; (rs = of.open(fname,"r",0666)) >= 0) {
	        int	line = 0 ;
	        while ((rs = of.readln(lbuf,llen)) > 0) {
	            if (cint len = rmeol(lbuf,rs) ; len > 0) {
	                if ((line++ == 0) && sp) {
	                    rs = proglog_printf(pip,"%r>",sp,sl) ;
	                    wlen += rs ;
		        } /* end */
	                if (rs >= 0) {
	                    rs = procline(pip,columns,lbuf,len) ;
	                    wlen += rs ;
	                } /* end if (ok) */
		    } /* end if (non-zero) */
		    if (rs < 0) break ;
	        } /* end while (reading line) */
	        rs1 = of.close ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (server output-file) */
	    rs1 = mem.free(lbuf) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (procfile) */

cint		indent = 2 ;
cint		leadlen = 4 ;

local int procline(PI *pip,int columns,cchar *lp,int ll) noex {
	int		rs = SR_OK ;
	int		rs1 ;
	int		textlen = (columns - leadlen) ;
	int		wlen = 0 ; /* return-value */
	if (textlen > 0) {
	    if (linefold lf ; (rs = lf.start(textlen,indent,lp,ll)) >= 0) {
		ccharp blanks = sysword.w_blanks ;
	        int	cl ;
	        int	ind = 0 ;
		cchar	*fmt = "| %r%r" ;
	        cchar	*cp ;
	        for (int i = 0 ; (cl = lf.get(i,&cp)) >= 0 ; i += 1) {
		    wlen += cl ;
	            rs1 = proglog_printf(pip,fmt,blanks,ind,cp,cl) ;
	            ind = indent ;
		    if (rs1 < 0) break ;
	        } /* end for */
	        rs1 = lf.finish ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (linefold) */
	} /* end if (non-zero) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (procline) */


