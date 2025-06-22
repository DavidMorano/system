/* progfront */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* process the back-matter information */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* compile-time debugging */
#define	CF_DEBUG	0		/* run-time debugging */
#define	CF_PROGOFF	1		/* use 'progoffwrite()' */

/* revision history:

	= 2009-04-01, David A­D­ Morano
	This subroutine was written as an enhancement for adding
	back-matter (end pages) to the output document.

*/

/* Copyright © 2009 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine just adds some code to output come end-pages
	that are just really some trailing blank pages.  This is
	supposed to end the whole document.  The document should
	end on an even page (which will form the back cover of the
	whole document).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */

#include	<sys/types.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<ctype.h>

#include	<usystem.h>
#include	<bfile.h>
#include	<localmisc.h>

#include	"config.h"
#include	"defs.h"
#include	"progoff.h"


/* local defines */


/* external subroutines */

extern int	cfdeci(cchar *,int,int *) ;
extern int	cfdecui(cchar *,int,uint *) ;
extern int	ctdeci(char *,int,int) ;
extern int	ctdecui(char *,int,uint) ;
extern int	hasalluc(cchar *,int) ;
extern int	bprintlns(bfile *,int,cchar *,int) ;
extern int	bprintln(bfile *,cchar *,int) ;


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */


int progfront(pip,ofp)
struct proginfo	*pip ;
bfile		*ofp ;
{
	int		rs = SR_OK ;
	int		rs1 ;
	int		wlen = 0 ;

	if (ofp == NULL)
	    return SR_FAULT ;

	if ((pip->frontfname != NULL) && (pip->frontfname[0] != '\0')) {
	    bfile	ffile ;			/* "front" file */
	    cchar	*lc = pip->troff.linecomment ;

	    rs = bprintf(ofp,"%s front-matter begin\n",lc) ;
	    wlen += rs ;

	    if (pip->debuglevel > 0) {
	        bprintf(pip->efp,"%s: front-matter file=%s\n",
	            pip->progname,pip->frontfname) ;
	    }

	    if ((rs = bopen(&ffile,pip->frontfname,"r",0666)) >= 0) {
	        int		len ;
	        char		lbuf[LINEBUFLEN + 1] ;

	        rs = bprintf(ofp,"%s front-matter file=%s\n",
	            pip->troff.linecomment,pip->frontfname) ;
	        wlen += rs ;

/* copy the whole input file to the output */

#if	CF_PROGOFF
	        while ((rs = breadln(&ffile,lbuf,LINEBUFLEN)) > 0) {
	            len = rs ;
	            rs = progoffwrite(pip,ofp,lbuf,len) ;
	            wlen += rs ;
	            if (rs < 0) break ;
	        } /* end while */
#else
	        rs = bcopyblock(&ffile,ofp,-1) ;
	        wlen += rs ;
#endif

/* anything else to do? */

	        if (rs >= 0) {
	            cchar	*lc = pip->troff.linecomment ;
	            rs = bprintf(ofp,"%s front-matter end\n",lc) ;
	            wlen += rs ;
	        }

	        rs1 = bclose(&ffile) ;
		if (rs >= 0) rs = rs1 ;
	    } /* end if (bfile) */

	} /* end if (non-null and non-nul) */

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutines (progfront) */


