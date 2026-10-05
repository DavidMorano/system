/* setfname */

/* set a filename (?) */
/* version %I% last-modified %G% */


#define	CF_DEBUGS	0		/* non-switchable debug print-outs */
#define	CF_DEBUG	0		/* switchable at invocation */
#define	CF_GETEXECNAME	1		/* get the 'exec(2)' name */
#define	CF_CPUSPEED	1		/* calculate CPU speed */


/* revision history:

	= 1989-03-01, David A­D­ Morano
	This code was originally written.  

	= 1998-06-01, David A­D­ Morano
	I enhanced the program a little to print out some other
	information.

	= 1999-03-01, David A­D­ Morano
	I enhanced the program a little to to do something (I forget
	what).

	= 2004-01-10, David A­D­ Morano
	The KSH program switched to using a fakey "large file" (64-bit
	fake-out mode) compilation mode on Solaris.  This required
	some checking to see if any references to 'u_stat()' had to be
	updated to work with the new KSH.  Although we call 'u_stat()'
	here, its structure is not passed to other subroutines expecting
	the regular 32-bit structure.

	= 2005-04-20, David A­D­ Morano
	I changed the program so that the configuration file is consulted
	even if the program is not run in daemon-mode.	Previously, the
	configuration file was only consulted when run in daemon-mode.
	The thinking was that running the program in regular (non-daemon)
	mode should be quick.  The problem is that the MS file had to
	be guessed without the aid of consulting the configuration file.
	Although not a problem in most practice, it was not aesthetically
	appealing.  It meant that if the administrator changed the MS file
	in the configuration file, it also had to be changed by specifying
	it explicitly at invocation in non-daemon-mode of the program.
	This is the source of some confusion (which the world really
	doesn't need).	So now the configuration is always consulted.
	The single one-time invocation is still fast enough for the
	non-smoker aged under 40 ! :-) :-)

*/

/* Copyright © 1998,2004,2005 David A­D­ Morano.  All rights reserved. */

/**************************************************************************

	This subroutine sets a filename in some way.


*****************************************************************************/


#include	<envstandards.h>	/* ordered first to configure */

#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<climits>
#include	<unistd.h>
#include	<netdb.h>
#include	<ctime>
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<baops.h>
#include	<paramfile.h>
#include	<logfile.h>
#include	<msfile.h>
#include	<kinfo.h>
#include	<lfm.h>
#include	<exitcodes.h>
#include	<localmisc.h>

#include	"config.h"
#include	"defs.h"
#include	"shio.h"


/* local defines */

#define	VBUFLEN		(2 * MAXPATHLEN)
#define	EBUFLEN		(3 * MAXPATHLEN)

#define	DEBUGFNAME	"/tmp/msu.deb"

#ifndef	DEVTTY
#define	DEVTTY		"/dev/tty"
#endif

#ifndef	DEBUGLEVEL
#define	DEBUGLEVEL(n)	(pip->debuglevel >= (n))
#endif


/* external subroutines */

extern int	snsd(char *,int,cchar *,uint) ;
extern int	snsds(char *,int,cchar *,cchar *) ;
extern int	sncpy1(char *,int,cchar *) ;
extern int	sncpy2(char *,int,cchar *,cchar *) ;
extern int	sncpy3(char *,int,cchar *,cchar *,cchar *) ;
extern int	mkfnamesuf1(char *,cchar *,cchar *) ;
extern int	mkpath1(char *,cchar *) ;
extern int	mkpath2(char *,cchar *,cchar *) ;
extern int	mkpath3(char *,cchar *,cchar *,cchar *) ;
extern int	sfdirname(cchar *,int,cchar **) ;
extern int	sfshrink(cchar *,int,cchar **) ;
extern int	matstr(cchar **,char *,int) ;
extern int	matstr2(cchar **,char *,int) ;
extern int	cfdeci(cchar *,int,int *) ;
extern int	cfdecti(cchar *,int,int *) ;
extern int	mkdirs(cchar *,mode_t) ;
extern int	perm(cchar *,uid_t,gid_t,gid_t *,int) ;

extern char	*strwcpy(char *,cchar *,int) ;


/* external variables */

#if	(! CF_SFIO)
extern char	**environ ;
#endif


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */


int setfname(pip,fname,ebuf,el,f_def,dname,name,suf)
PROGINFO	*pip ;
char		fname[] ;
cchar	ebuf[] ;
cchar	dname[], name[], suf[] ;
int		el ;
int		f_def ;
{
	int		rs = 0 ;
	int		ml ;
	char		tmpname[MAXNAMELEN + 1], *np ;

	if ((f_def && (ebuf[0] == '\0')) ||
	    (strcmp(ebuf,"+") == 0)) {

	    np = (char *) name ;
	    if ((suf != NULL) && (suf[0] != '\0')) {

	        np = (char *) tmpname ;
	        mkfnamesuf1(tmpname,name,suf) ;

	    }

	    if (np[0] != '/') {

	        if ((dname != NULL) && (dname[0] != '\0')) {
	            rs = mkpath3(fname,pip->pr,dname,np) ;

	        } else
	            rs = mkpath2(fname, pip->pr,np) ;

	    } else
	        rs = mkpath1(fname, np) ;

	} else if (strcmp(ebuf,"-") == 0) {

	    fname[0] = '\0' ;

	} else if (ebuf[0] != '\0') {

	    np = (char *) ebuf ;
	    if (el >= 0) {

	        np = tmpname ;
	        ml = MIN(MAXPATHLEN,el) ;
	        strwcpy(tmpname,ebuf,ml) ;

	    }

	    if (ebuf[0] != '/') {

	        if (strchr(np,'/') != NULL) {

	            rs = mkpath2(fname,pip->pr,np) ;

	        } else {

	            if ((dname != NULL) && (dname[0] != '\0')) {
	                rs = mkpath3(fname,pip->pr,dname,np) ;

	            } else
	                rs = mkpath2(fname,pip->pr,np) ;

	        } /* end if */

	    } else
	        rs = mkpath1(fname,np) ;

	} /* end if */

	return rs ;
}
/* end subroutine (setfname) */


