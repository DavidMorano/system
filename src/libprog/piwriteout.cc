/* progdebugout SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* write out the output files from the executed program */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-09-07, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<linefold.h>		/* LIBUC */
#include	<vardefs.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */
#include	<bfile.h>		/* LIBB */
#include	<proginfo.hh>		/* LIBPI */

#include	"piwriteout.hh"		/* LIBPI */

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#ifndef	PI
#define	PI	proginfo
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int	procline(PI *,int,cchar *,int) noex ;


/* local variables */

constexpr char	blanks[] = "        " ;


/* exported variables */


/* exported subroutines */

int piwriteout(PI *pip,cchar *s,cchar *fname) noex {
	int		rs = SR_OK ;
	int		rs1 = SR_OK ;
	int		wlen = 0 ;

	if (fname == nullptr) return SR_FAULT ;

	if (fname[0] == '\0') return SR_INVALID ;

	if (pip->efp != nullptr) {
	    int		columns = -1 ;
	    char	*cp ;

/* determine the column-width of STDERR */

	    if ((cp = getenv(VARCOLUMNS)) != nullptr) {
	        rs1 = cfdeci(cp,-1,&columns) ;
	        if (rs1 < 0) columns = -1 ;
	    }

	    if (columns < 0)
	        columns = COLUMNS ;

	    if (columns > 0) {
	        bfile	ofile, *ofp = &ofile ;

	        if ((rs = bopen(ofp,fname,"r",0666)) >= 0) {
	            cint	llen = LINEBUFLEN ;
		    bool	f_title = false ;
	            int		lines = 0 ;
	            int		len ;
	            cchar	*pn = pip->progname ;
	            char	lbuf[LINEBUFLEN + 1] ;

	            rs1 = SR_OK ;
	            while ((rs = breadln(ofp,lbuf,llen)) > 0) {
	                len = rs ;

	                if (lbuf[len - 1] == '\n')
	                    lbuf[--len] = '\0' ;

	                if ((! f_title) && (len > 0)) {
	                    if ((s != nullptr) && (s[0] != '\0')) {
	                        cchar	*fmt = "%s: %s>\n" ;
	                        f_title = true ;
	                        rs1 = shio_printf(pip->efp,fmt,pn,s) ;
	                        wlen += rs1 ;
	                    }
	                }

	                if (rs1 >= 0) {
	                    rs1 = procline(pip,columns,lbuf,len) ;
	                    wlen += rs1 ;
	                }

	                lines += 1 ;

	                if (rs1 < 0) break ;
	            } /* end while (reading lines) */

	            if (rs1 < 0) wlen = 0 ;

	            bclose(ofp) ;
	        } /* end if (bfile) */

	    } /* end if (positive columns) */

	} /* end if (non-null) */

	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (piwriteout) */


/* local subroutines */

local int procline(PI *pip,int columns,cchar *lp,int ll) noex {
	SHIO		*fp = pip->efp ;
	cint		indent = 2 ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		leadlen ;
	int		textlen ;
	int		wlen = 0 ; /* return-value */
	cchar		*pn = pip->progname ;
	leadlen = (lenstr(pn) + 4) ;
	textlen = (columns - leadlen) ;
	if (textlen > 0) {
	    if (linefold lf ; (rs = lf.start(textlen,indent,lp,ll)) >= 0) {
	        int	ind = 0 ;
	        int	cl ;
	        ccharp	fmt = "%s: | %r%r\n" ;
	        cchar	*cp ;
	        for (int i = 0 ; (cl = lf.get(i,&cp)) >= 0 ; i += 1) {
	            rs = shio_printf(fp,fmt,pn,blanks,ind,cp,cl) ;
	            wlen += rs ;
		    if (rs < 0) break ;
	            ind = indent ;
	        } /* end for */
	        rs1 = lf.finish ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (linefold) */
	} /* end if (non-zero) */
	return (rs >= 0) ? wlen : rs ;
} /* end subroutine (procline) */


