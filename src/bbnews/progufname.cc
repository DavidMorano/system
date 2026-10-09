/* progufname */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* program user-newsrc file */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* non-switchable */
#define	CF_DEBUG	0		/* run-time debug print-outs */

/* revision history:

	= 1995-05-01, David A­D­ Morano
	This code module was completely rewritten to replace any
	original garbage that was here before.

	= 1998-11-22, David A­D­ Morano
        I did some clean-up.

*/

/* Copyright © 1995,1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	We figure ot the user-newsrc file here.

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<isch.h>
#include	<localmisc.h>
#include	<libdebug.h>		/* LIBDEBUG |DEBUGPRINTF(3debug)| */
#include	<bfile.h>

#include	"config.h"
#include	"defs.h"


/* local defines */


/* external subroutines */

extern int	snsds(char *,int,cchar *,cchar *) ;
extern int	sncpy1(char *,int,cchar *) ;
extern int	sncpy2(char *,int,cchar *,cchar *) ;
extern int	mkpath2(char *,cchar *,cchar *) ;
extern int	matstr(cchar **,cchar *,int) ;
extern int	matostr(cchar **,int,cchar *,int) ;
extern int	matocasestr(cchar **,int,cchar *,int) ;
extern int	cfdeci(cchar *,int,int *) ;


/* external variables */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */


int progufname(PROGINFO *pip,cchar ufname[])
{
	int		rs = SR_OK ;
	int		unl = -1 ;
	cchar		*unp = ufname ;
	char		tbuf[MAXPATHLEN+1] ;

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	debugprintf("progufname: ent ufn=%s\n",ufname) ;
#endif

	if (ufname == NULL) {
	    cchar	*def = DEFNEWSRC ;
	    rs = mkpath2(tbuf,pip->homedname,def) ;
	    unl = rs ;
	    unp = tbuf ;
	}

	if (rs >= 0) {
	   cchar	**vpp = &pip->ufname ;
	   rs = proginfo_setentry(pip,vpp,unp,unl) ;
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	debugprintf("progufname: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (progufname) */


