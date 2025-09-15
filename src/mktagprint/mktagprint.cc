/* mktagprint SUPPORT */
/* lang=C++20 */

/* make (process) key tags */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/***********************************************************************

	Description:
	This subroutine processes a single file.

	Synopsis:
	int mktagprint(pip,aip,basedname,outfmt,outfname)
	struct proginfo	*pip ;
	struct arginfo	*aip ;
	const char	basedname[] ;
	const char	outfmt[] ;
	const char	outfname[] ;

	Arguments:
	- pip		program information pointer

	Returns:
	>=0		OK
	<0		error code (system-return)

***********************************************************************/

#include	<envstandards.h>	/* must be before others */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<climits>
#include	<unistd.h>
#include	<cstdlib>
#include	<cstring>
#include	<ctype.h>

#include	<usystem.h>
#include	<baops.h>
#include	<bfile.h>
#include	<field.h>
#include	<localmisc.h>

#include	"biblebook.h"
#include	"config.h"
#include	"defs.h"
#include	"outfmt.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	proctagfile(struct proginfo *,const char *,BIBLEBOOK *,
			int, bfile *, const char *) ;


/* local variables */


/* exported subroutines */

int mktagprint(pip,aip,basedname,outfmt,outfname)
struct proginfo	*pip ;
struct arginfo	*aip ;
const char	basedname[] ;
const char	outfmt[] ;
const char	outfname[] ;
{
	BIBLEBOOK	bb ;

	bfile	outfile, *ofp = &outfile ;

	int	rs = SR_OK ;
	int	pan ;
	int	ofi ;
	int	ai ;
	int	c_tagref = 0 ;
	int	f_biblebook = FALSE ;
	int	f ;

	char	*cp ;


	if (aip == NULL)
	    return SR_FAULT ;

	ofi = -1 ;
	if ((outfmt != NULL) && (outfmt[0] != '\0'))
	    ofi = matostr(outfmts,2,outfmt,-1) ;

	if (ofi == outfmt_bible) {
	    rs = biblebook_open(&bb,pip->pr,pip->ndbname) ;
	    f_biblebook = (rs >= 0) ;
	}

	if (rs < 0)
	    goto ret0 ;

/* initialize the data structures we need */

	if ((outfname != NULL) && (outfname[0] != '\0')) {
	    rs = bopen(ofp,outfname,"wct",0666) ;
	} else
	    rs = bopen(ofp,BFILE_STDOUT,"dwct",0666) ;

	if (rs < 0)
	    goto retoutopen ;

/* process the positional arguments */

	pan = 0 ;
	for (ai = 1 ; ai < aip->argc ; ai += 1) {

	    f = (ai <= aip->ai_max) && BATST(aip->argpresent,ai) ;
	    f = f || (ai > aip->ai_pos) ;
	    if (! f) continue ;

	    cp = aip->argv[ai] ;

	    pan += 1 ;
	    rs = proctagfile(pip,basedname,&bb,ofi,ofp,cp) ;

	    if (rs > 0)
	        c_tagref += rs ;

	    if (rs < 0) {

	        if (*cp == '-')
	            cp = "*stdinput*" ;

	        bprintf(pip->efp,"%s: error processing tag file (%d)\n",
	            pip->progname,rs) ;

	        bprintf(pip->efp,"%s: errored file=%s\n",
	            pip->progname,cp) ;

	    } /* end if */

	    if (rs < 0)
	        break ;

	} /* end for (processing positional arguments) */

/* process any tags in the argument list file */

	if ((rs >= 0) && (aip->argfname != NULL) && 
	    (aip->argfname[0] != '\0')) {

	    bfile	argfile ;


	    if (strcmp(aip->argfname,"-") != 0)
	        rs = bopen(&argfile,aip->argfname,"r",0666) ;

	    else
	        rs = bopen(&argfile,BFILE_STDIN,"dr",0666) ;

	    if (rs >= 0) {

	        int	len ;

	        char	linebuf[LINEBUFLEN + 1] ;


	        while ((rs = breadln(&argfile,linebuf,LINEBUFLEN)) > 0) {

	            len = rs ;
	            if (linebuf[len - 1] == '\n')
	                len -= 1 ;

	            linebuf[len] = '\0' ;
	            cp = linebuf ;

	            if ((cp[0] == '\0') || (cp[0] == '#'))
	                continue ;

	            pan += 1 ;
	            rs = proctagfile(pip,basedname,&bb,ofi,ofp,cp) ;

	            if (rs > 0)
	                c_tagref += rs ;

	            if (rs < 0) {

	                if (*cp == '-')
	                    cp = "*stdinput*" ;

	                bprintf(pip->efp,
	                    "%s: error processing tag file (%d)\n",
	                    pip->progname,rs) ;

	                bprintf(pip->efp,"%s: errored file=%s\n",
	                    pip->progname,cp) ;

	            } /* end if */

	            if (rs < 0)
	                break ;

	        } /* end while (reading lines) */

	        bclose(&argfile) ;
	    } else if (! pip->fl.quiet) {
	        bprintf(pip->efp,
	            "%s: could not open the argument list file (%d)\n",
	            pip->progname,rs) ;
	        bprintf(pip->efp,"%s: argfile=%s\n",
	            pip->progname,aip->argfname) ;
	    }

	} /* end if (argument file) */

	if ((rs >= 0) && (pan == 0)) {

	    cp = pip->infname ;
	    if (cp == NULL)
	        cp = "-" ;

	    pan += 1 ;
	    rs = proctagfile(pip,basedname,&bb,ofi,ofp,cp) ;
	    if (rs >= 0)
	        c_tagref += rs ;

	} /* end if (input file) */

	if (rs >= 0) {

	    if (pip->debuglevel > 0)
	        bprintf(pip->efp,"%s: tag references=%u\n",
	            pip->progname,c_tagref) ;

	} /* end if */

/* close the output files */
ret2:
	bclose(ofp) ;

retoutopen:
ret1:
	if (f_biblebook)
	    biblebook_close(&bb) ;

ret0:
	return (rs >= 0) ? pan : rs ;
}
/* end subroutine (mktagprint) */


/* local subroutines */


static int proctagfile(pip,basedname,bbp,ofi,ofp,tagfname)
struct proginfo	*pip ;
const char	basedname[] ;
BIBLEBOOK	*bbp ;
int		ofi ;
bfile		*ofp ;
const char	tagfname[] ;
{
	bfile	tagfile, *tfp = &tagfile ;

	int	rs = SR_OK ;
	int	len ;
	int	fnl ;
	int	c_tagref = 0 ;
	int	f_bol, f_eol ;

	char	linebuf[LINEBUFLEN + 1] ;
	char	*cp ;

	if (tagfname == NULL)
	    return SR_FAULT ;

	if (strcmp(tagfname,"-") != 0) {
	    rs = bopen(tfp,tagfname,"r",0666) ;
	} else
	    rs = bopen(tfp,BFILE_STDIN,"dr",0666) ;

	if (rs < 0)
	    goto ret0 ;

	f_bol = TRUE ;
	while ((rs = breadln(tfp,linebuf,LINEBUFLEN)) > 0) {

	    len = rs ;
	    f_eol = (linebuf[len - 1] == '\n') ;
	    if (f_eol)
	        len -= 1 ;

	    linebuf[len] = '\0' ;
	    cp = linebuf ;

	    if ((cp[0] == '\0') || (cp[0] == '#'))
	        continue ;

	    if (f_bol) {
	        rs = progtagprint(pip,basedname,bbp,ofi,ofp,cp) ;
	        if (rs > 0) c_tagref += rs ;
	    } /* end if */

	    if (rs < 0) {

	        if (*cp == '-') {
	            fnl = -1 ;
	            cp = "*stdinput*" ;
	        } else {
	            fnl = sibrk(cp,-1,":") ;
		}

	        bprintf(pip->efp,
	            "%s: error processing tag reference file (%d)\n",
	            pip->progname,rs) ;

	        bprintf(pip->efp,"%s: errored file=%r\n",
	            pip->progname,cp,fnl) ;

	    } /* end if */

	    if (rs < 0)
	        break ;

	    f_bol = f_eol ;

	} /* end while (reading lines) */

	bclose(tfp) ;

ret0:
	return (rs >= 0) ? c_tagref : rs ;
}
/* end subroutine (proctagfile) */



