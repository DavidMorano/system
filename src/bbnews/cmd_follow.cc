/* cmd_follow SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* "follow-up" on an article */
/* version %I% last-modified %G% */


/* revision history:

	= 1994-02-01, David A�D� Morano
	I wrote this from scratch when I took over the code.  The
	previous code was a mess (still is in many places!).

	= 1998-11-22, David A�D� Morano
        I did some clean-up.

*/

/* Copyright � 1994,1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	cmd_follow

	Description:
	This subroutine performs some sort of command.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<getfiledirs.h>
#include	<mallocstuff.h>
#include	<estrings.h>
#include	<bfile.h>
#include	<vecstr.h>
#include	<char.h>
#include	<localmisc.h>

#include	"artlist.h"
#include	"config.h"
#include	"defs.h"


/* local defines */


/* external subroutines */

extern int	mkpath3(char *,const char *,const char *,const char *) ;
extern int	mktmpfile(char *,mode_t,const char *) ;
extern int	bbcpy(char *,const char *) ;

extern int	uc_system(cchar *) ;


/* external variables */


/* exported subroutines */


int cmd_follow(pip,ap,ngdir,afname, hv_subject)
struct proginfo	*pip ;
ARTLIST_ENT	*ap ;
const char	ngdir[] ;
const char	afname[] ;
const char	hv_subject[] ;
{
	bfile		afile, *afp = &afile ;
	bfile		tmpfile, *tfp = &tmpfile ;
	const int	llen = LINEBUFLEN ;
	int		rs = SR_OK ;
	int		len ;
	const char	*tp ;
	char		lbuf[LINEBUFLEN + 1] ;
	char		tmpfname[MAXPATHLEN + 1] ;
	char		cmdbuf[(2*MAXPATHLEN) + 1], *cbp ;
	char		newsgroup[MAXPATHLEN + 1] ;
	char		tngdname[MAXPATHLEN+1] ;

/* can we do this? */

	if (pip->prog_bbpost == NULL)
	    goto badbbpost ;

	if (pip->prog_bbpost[0] == '/') {

	    if (u_access(pip->prog_bbpost,X_OK) < 0)
	        goto badbbpost ;

	} else if (getfiledirs(NULL,pip->prog_bbpost,"x",NULL) < 1) {

	    mkpath3(tmpfname, pip->pr,"bin",pip->prog_bbpost) ;

	    if (u_access(tmpfname,X_OK) < 0)
	        goto badbbpost ;

	    pip->prog_bbpost = mallocstr(tmpfname) ;

	} /* end if (finding the BBPOST program) */

/* use only the first ngdir if there are multiple ones specified */

	if ((tp = strchr(ngdir,',')) != NULL) {
	    mkpath1w(tngdname,ngdir,(tp-ngdir)) ;
	    ngdir = tngdname ;
	}

/* create the newsgroup name for the given newsgroup directory */

	bbcpy(newsgroup,ngdir) ;

/* make an attachment file if we have it */

	tmpfname[0] = '\0' ;
	rs = OK ;
	if ((afname[0] != '\0') && (bopen(afp,afname,"r",0666) >= 0)) {

	    rs = mktmpfile(tmpfname,0600,"/tmp/bbattXXXXXXXXX") ;

		if (rs < 0)
			goto bad1 ;

	    rs = bopen(tfp,tmpfname,"wct",0600) ;

	    if (rs < 0)
			goto bad2 ;

	    bprintf(tfp,"previous article was ...\n") ;

	    while ((len = breadln(afp,lbuf,llen)) > 0) {

	        if (lbuf[len - 1] == '\n') 
			len -= 1 ;

	        if ((rs = bprintf(tfp,"> %W\n",lbuf,len)) < (len + 2))
	            goto bad3 ;

	    } /* end while */

	    bclose(afp) ;

	    bclose(tfp) ;

	} /* end if */

	if (rs < 0) 
		return rs ;

/* create the command */

	cbp = cmdbuf ;
	cbp += sprintf(cbp,"%s %s",
	    pip->prog_bbpost,newsgroup) ;

	if (tmpfname[0] != '\0')
	    cbp += sprintf(cbp," -a %s",tmpfname) ;

	if (hv_subject[0] != '\0')
	    cbp += sprintf(cbp," -s \"%s\"",hv_subject) ;

#ifdef	COMMENT
	*cbp++ = '\0' ;
#endif

#if	CF_DEBUG
	if (pip->f.debug)
	    debugprintf("follow: CMD> %s\n",cmdbuf) ;
#endif

	uc_system(cmdbuf) ;

	if (tmpfname[0] != '\0') 
		u_unlink(tmpfname) ;

	return OK ;

/* bad stuff */
bad3:
	rs = BAD ;

bad2:
	u_unlink(tmpfname) ;

bad1:
	bclose(afp) ;

bad0:
	return rs ;

badbbpost:
	bprintf(pip->efp,"%s: could not find the BBPOST program\n",
	    pip->progname) ;

	goto bad0 ;
}
/* end subroutine (follow) */



