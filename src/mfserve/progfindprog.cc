/* progfindprog */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* find the program to execute */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* debug print-outs (non-switchable) */
#define	CF_DEBUG	0		/* debug print-outs switchable */

/* revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	progfindprog

	Synopsis:
	int progfindprog()

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<baops.h>
#include	<keyopt.h>
#include	<bfile.h>
#include	<ids.h>
#include	<vecstr.h>
#include	<strx.h>
#include	<vstrxcmp.h>		/* |vstrkeycmp(3uc)| */
#include	<exitcodes.h>
#include	<localmisc.h>

#include	"config.h"
#include	"defs.h"
#include	"envs.h"


/* local defines */

#ifndef	NOFILE
#define	NOFILE		20
#endif

#ifndef	BUFLEN
#define	BUFLEN		((2 * MAXPATHLEN) + 20)
#endif

#ifndef	ARCHBUFLEN
#define	ARCHBUFLEN	80
#endif

#ifndef	DIGBUFLEN
#define	DIGBUFLEN	40		/* can hold int128_t in decimal */
#endif

#define	VARBUFLEN	(20 * MAXPATHLEN)

#ifndef	elementsof
#define	elementsof(a)	(sizeof(a) / sizeof((a)[0]))
#endif

#ifndef	VARPATH
#define	VARPATH	"PATH"
#endif

#ifndef	VARAST
#define	VARAST	"AST"
#endif

#undef	DEBUGFNAME
#define	DEBUGFNAME	"/tmp/lsh.nd"


/* external subroutines */

extern int	progdefprog(struct proginfo *,cchar **) ;


/* external variables */


/* local structures */


/* forward references */

static int	procsearch(struct proginfo *,vecstr *,char *,cchar *) ;

static int	loadpathlist(struct proginfo *,vecstr *,vecstr *) ;
static int	loadpathcomp(struct proginfo *,vecstr *,cchar *) ;


/* local variables */


/* exported variables */


/* exported subroutines */

int progfindprog(pip,progfname,pn)
struct proginfo	*pip ;
char		progfname[] ;
cchar	pn[] ;
{
	vecstr		*elp ;

	int	rs = SR_OK ;
	int	f ;

	cchar	*pnp = pn ;


#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	debugprintf("progfindprog: pn=>%s<\n",pn) ;
#endif

#if	CF_DEBUGN
	nprintf(DEBUGFNAME,"progfindprog: pn=%s\n",pn) ;
#endif

	elp = &pip->exports ;
	progfname[0] = '\0' ;
	if (pn == NULL) {

	    rs = progdefprog(pip,&pnp) ;

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	debugprintf("progfindprog: progdefprog() rs=%d\n",rs) ;
#endif

	} /* end if */

#if	CF_DEBUG
	if (DEBUGLEVEL(3) && (rs >= 0))
	debugprintf("progfindprog: pnp=%s\n",pnp) ;
#endif

	if ((rs >= 0) && ((pnp == NULL) || (pnp[0] == '\0')))
	    rs = SR_NOENT ;

	if (rs < 0)
	    goto ret0 ;

	if (strchr(pnp,'/') == NULL) {

	    rs = procsearch(pip,elp,progfname,pnp) ;

	    if ((rs < 0) && (pn == NULL)) {
		f = FALSE ;
		f = f || (rs == SR_NOENT) ;
		if (f) {
		    pnp = DEFPROGFNAME ;
	            rs = mkpath1(progfname,pnp) ;
		}
	    }

	} else
	    rs = mkpath1(progfname,pnp) ;

 ret0:

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	debugprintf("progfindprog: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (progfindprog) */


/* local subroutines */


static int procsearch(pip,elp,progfname,pn)
struct proginfo	*pip ;
vecstr		*elp ;
char		progfname[] ;
cchar	pn[] ;
{
	vecstr	pathlist ;

	int	rs ;


#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	debugprintf("progfindprog/procsearch: pn=>%s<\n",pn) ;
#endif

	rs = vecstr_start(&pathlist,10,0) ;
	if (rs < 0)
	    goto ret0 ;

	rs = loadpathlist(pip,&pathlist,elp) ;
	if (rs < 0)
	    goto ret1 ;

#if	CF_DEBUG
	if (DEBUGLEVEL(2)) {
		int	i ;
		char	*cp ;
		for (i = 0 ; vecstr_get(&pathlist,i,&cp) >= 0 ; i += 1)
	debugprintf("progfindprog/procsearch: pc=%s\n",cp) ;
	}
#endif

	rs = getprogpath(&pip->id,&pathlist,progfname,pn,-1) ;

#if	CF_DEBUG
	if (DEBUGLEVEL(2))
	debugprintf("progfindprog/procsearch: getprogpath() rs=%d\n",rs) ;
#endif

	if (rs == SR_NOENT) {

	    rs = xfile(&pip->id,pn) ;
	    if (rs >= 0)
	        rs = mkpath1(progfname,pn) ;

	}

ret1:
	vecstr_finish(&pathlist) ;

ret0:
	return rs ;
} 
/* end subroutine (procsearch) */


static int loadpathlist(pip,plp,elp)
struct proginfo	*pip ;
vecstr		*plp ;
vecstr		*elp ;
{
	int	rs = SR_OK ;

	cchar	*varpath = VARPATH ;
	cchar	*tp ;
	cchar	*pp ;


	rs = vecstr_search(elp,varpath,vstrkeycmp,&pp) ;
	if (rs < 0)
	    goto ret0 ;

	rs = SR_NOENT ;
	if ((tp = strchr(pp,'=')) != NULL)
	    rs = loadpathcomp(pip,plp,(tp + 1)) ;

ret0:
	return rs ;
} 
/* end subroutine (loadpathlist) */


static int loadpathcomp(pip,lp,pp)
struct proginfo	*pip ;
vecstr		*lp ;
cchar	*pp ;
{
	int	rs = SR_OK, rs1 ;
	int	c, cl ;

	char	tmpfname[MAXPATHLEN + 1] ;
	char	*cp ;


	c = 0 ;
	while ((cp = strbrk(pp,":;")) != NULL) {

	    cl = pathclean(tmpfname,pp,(cp - pp)) ;

	    rs1 = vecstr_findn(lp,tmpfname,cl) ;
	    if (rs1 == SR_NOTFOUND) {
	        c += 1 ;
		rs = vecstr_add(lp,tmpfname,cl) ;
	    }

	    if (rs < 0)
		break ;

	    pp = (cp + 1) ;

	} /* end while */

	if ((rs >= 0) && (pp[0] != '\0')) {

	    cl = pathclean(tmpfname,pp,-1) ;

	    rs1 = vecstr_findn(lp,tmpfname,cl) ;
	    if (rs1 == SR_NOTFOUND) {
	        c += 1 ;
	        rs = vecstr_add(lp,tmpfname,cl) ;
	    }

	} /* end if (trailing one) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (loadpathcomp) */


static int xfile(idp,fname)
IDS		*idp ;
cchar	fname[] ;
{
	ustat	sb ;

	int	rs ;


	rs = u_stat(fname,&sb) ;
	if (rs >= 0) {

	    rs = SR_NOTFOUND ;
	    if (S_ISREG(sb.st_mode))
	        rs = permid(idp,&sb,X_OK) ;

	}

	return rs ;
}
/* end subroutine (xfile) */


