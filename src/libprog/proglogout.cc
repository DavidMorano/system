/* proglogout SUPPORT */
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

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* |strlen(3c)| */
#include	<usystem.h>
#include	<malloccc.h>
#include	<bfile.h>
#include	<linefold.h>
#include	<logfile.h>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"config.h"
#include	"defs.h"


/* local defines */

#define	PI		proginfo

#ifndef	NULLFNAME
#define	NULLFNAME	"/dev/null"
#endif

#ifndef	MAXOUTLEN
#define	MAXOUTLEN	64
#endif


/* imported namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern int	proglog_printf(PI *,cchar *,...) noex ;
    extern int	proglog_flush(PI *) noex ;
}


/* externals variables */


/* local structures */


/* forward references */

static int	procfile(PI *,cchar *,int,cchar *) noex ;
static int	procline(PI *,int,cchar *,int) noex ;


/* local variables */

constexpr cchar	blanks[] = "        " ;


/* exported variables */


/* exported subroutines */

int proglogout(PI *pip,cchar *msgstr,cchar *fname) noex {
	int		rs = SR_OK ;
	int		wlen = 0 ;
	if (fname != nullptr) {
	    if (fname[0] != '\0') {
	        if (pip->open.logprog) {
	    	    int		msglen = 0 ;
	            if (msgstr != nullptr) {
	                msglen = strlen(msgstr) ;
	                while (msglen && (msgstr[msglen-1] == '>')) {
	                    msglen -= 1 ;
			}
	            } /* end if (non-null) */
	            rs = procfile(pip,msgstr,msglen,fname) ;
	            wlen = rs ;
	        } /* end if (logprog) */
	    } /* end if (non-nul) */
	} /* end if (fname) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (proglogout) */


/* local subroutines */

static int procfile(PI *pip,cchar *sp,int sl,cchar *fname) noex {
	cint		columns = LOGFILE_FMTLEN ;
	int		rs ;
	int		rs1 ;
	int		wlen = 0 ;
	if (char *lbuf{} ; (rs = malloc_ml(&lbuf)) >= 0) {
	    bfile		ofile, *fp = &ofile ;
	    if ((rs = bopen(fp,fname,"r",0666)) >= 0) {
	        int		len ;
	        int		line = 0 ;
	        while ((rs = breadln(fp,lbuf,llen)) > 0) {
	            len = rs ;
	            if (lbuf[len - 1] == '\n') len -= 1 ;
	            if (len > 0) {
	                if ((line == 0) && sp) {
	                    proglog_printf(pip,"%r>",sp,sl) ;
		        }
	                if (rs >= 0) {
	                    rs = procline(pip,columns,lbuf,len) ;
	                    wlen += rs ;
	                }
		    } /* end if (non-zero) */
		    line += 1 ;
		    if (rs < 0) break ;
	        } /* end while (reading line) */
	        rs1 = bclose(fp) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (server output-file) */
	    rs = rsfree(rs,lbuf) ;
	} /* end if (m-a-f) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (procfile) */

static int procline(PI *pip,int columns,cchar *lp,int ll) noex {
	cint		indent = 2 ;
	cint		leadlen = 4 ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		textlen ;
	int		wlen = 0 ;
	textlen = (columns - leadlen) ;
	if (textlen > 0) {
	    linefold	lfd, *lfdp = &lfd ;
	    if ((rs = linefold_start(lfdp,textlen,indent,lp,ll)) >= 0) {
	        int	cl ;
	        int	ind = 0 ;
		cchar	*fmt = "| %r%r\n" ;
	        cchar	*cp ;
		auto lf_get = linefold_get ;
	        for (int i = 0 ; (cl = lf_get(lfdp,i,&cp)) >= 0 ; i += 1) {
		    wlen += cl ;
	            rs1 = proglog_printf(pip,fmt,blanks,ind,cp,cl) ;
	            ind = indent ;
		    if (rs1 < 0) break ;
	        } /* end for */
	        rs1 = linefold_finish(lfdp) ;
	        if (rs >= 0) rs = rs1 ;
	    } /* end if (linefold) */
	} /* end if (non-zero) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (procline) */


