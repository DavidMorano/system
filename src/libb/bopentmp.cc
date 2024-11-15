/* bopentmp SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* open a tempory file */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* compile-time debugging */

/* revision history:

	= 2004-04-13, David A�D� Morano
	This program was originally written.

*/

/* Copyright � 2004 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	This subroutine will open up a temporary file.  If successful,
	the file is deleted before this subroutine returns.

	Synopsis:

	int bopentmp(fp,tname,ostr,om)
	bfile		*fp ;
	const char	tname[] ;
	const char	ostr[] ;
	mode_t		om ;

	Arguments:

	fp		object pointer
	tname		template file name
	ostr		file-open mode
	om		file permissions

	Returns:

	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<sigblocker.h>
#include	<mkpathx.h>
#include	<mkdirs.h>
#include	<mkfile.h>
#include	<localmisc.h>

#include	"bfile.h"


/* local defines */

#ifndef	VARTMPDNAME
#define	VARTMPDNAME	"TMPDIR"
#endif

#ifndef	TMPDNAME
#define	TMPDNAME	"/tmp"
#endif

#ifndef	BFILE_TEMPLATE
#define	BFILE_TEMPLATE	"btmpXXXXXXXXXX"
#endif


/* external subroutines */

extern int	mktmpfile(char *,mode_t,const char *) ;
extern int	sfdirname(const char *,int,const char **) ;


/* external variables */


/* local structures */


/* forward references */

static int mkxfn(char *,cchar *) ;
static int mktmpdirs(const char *,mode_t) ;


/* local variables */


/* exported subroutines */


int bopentmp(bfile *fp,cchar *tname,cchar *ostr,mode_t om)
{
	int		rs ;
	char		xfname[MAXPATHLEN + 1] ;

	if (fp == NULL) return SR_FAULT ;
	if (ostr == NULL) return SR_FAULT ;

	if (ostr[0] == '\0') return SR_INVALID ;

	if ((tname == NULL) || (tname[0] == '\0')) {
	    tname = BFILE_TEMPLATE ;
	}

	if ((rs = mkxfn(xfname,tname)) >= 0) {
	    if ((rs = mktmpdirs(xfname,om)) >= 0) {
	        SIGBLOCK	b ;
	        if ((rs = sigblocker_start(&b,NULL)) >= 0) {
		    char	tbuf[MAXPATHLEN + 1] ;
    	
	            if ((rs = mktmpfile(tbuf,om,xfname)) >= 0) {
	                rs = bopen(fp,tbuf,ostr,om) ;
	                u_unlink(tbuf) ;
	            } /* end if (tmp-file) */
    
	            sigblocker_finish(&b) ;
	        } /* end if (sigblock) */
	    } /* end if (mktmpdirs) */
	} /* end if (ok) */

	return rs ;
}
/* end subroutine (bopentmp) */


/* local subroutines */


static int mkxfn(char *xfname,cchar *tname)
{
	int		rs ;
	if (tname[0] != '/') {
	    cchar	*tmpdname = getenv(VARTMPDNAME) ;
	    if (tmpdname == NULL) tmpdname = TMPDNAME ;
	    rs = mkpath2(xfname,tmpdname,tname) ;
	} else {
	    rs = mkpath1(xfname,tname) ;
	}
	return rs ;
}
/* end subroutine (mkxfn) */


static int mktmpdirs(const char *xfname,mode_t om)
{
	int		rs = SR_OK ;
	int		cl ;
	cchar		*cp ;
	if ((cl = sfdirname(xfname,-1,&cp)) > 0) {
	    char	dname[MAXPATHLEN+1] ;
	    if ((rs = mkpath1w(dname,cp,cl)) >= 0) {
		rs = mkdirs(dname,om) ;
	    }
	}
	return rs ;
} 
/* end subrouine (mktmpdirs) */


