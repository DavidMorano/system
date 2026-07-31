/* email_pathtrack SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* track paths (for finding programs) */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* non-switchable print-outs */
#define	CF_DEBUG	0		/* switchable print-outs */
#define	CF_DEBUGFORK	0		/* debug-fork */
#define	CF_LOGID	1		/* use a special LOGID */
#define	CF_SETRUID	1		/* use 'setreuid(2)' */
#define	CF_SETEUID	0		/* already done in 'main()' */

/* revision history:

	= 2008-09-01, David A­D­ Morano
	This subroutine was borrowed and modified from previous
	generic front-end 'main' subroutines!

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Description:
	Prepare to do some servicing.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<strx.h>
#include	<vstrcmp.h>		/* |vstrkeycmp(3uc)| */
#include	<localmisc.h>


/* local defines */


/* external subroutines */

#if	CF_DEBUGS || CF_DEBUG 
extern int	debugprintf(cchar *,...) ;
extern int	strlinelen(cchar *,int,int) ;
extern int	progexports(struct proginfo *,cchar *) ;
#endif /* CF_DEBUGS */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

constexpr cpcchar	prbins[] = {
	"bin",
	"sbin",
	nullptr
} ;

constexpr cpcchar	prlibs[] = {
	"lib",
	nullptr
} ;


/* exported subroutines */


/* static subroutines */

local int loadpath(pip,plp,varname,prdirs,defpath)
struct proginfo	*pip ;
vecstr		*plp ;
cchar	*varname ;
cchar	**prdirs ;
cchar	*defpath ;
{
	VECSTR	*elp = &pip->exports ;

	int	rs = SR_OK ;
	int	c = 0 ;

	cchar	*pp ;

/* system-administrative environment */

#if	CF_DEBUG
	if (DEBUGLEVEL(4)) {
	    debugprintf("progprocess/loadpath: exports> \n") ;
	    progexports(pip,"loadpath0") ;
	}
#endif

	if ((rs = vecstr_search(elp,varname,vstrkeycmp,&pp)) >= 0) {
	    cchar	*tp ;

	    if ((tp = strchr(pp,'=')) != nullptr) {
	        rs = loadpathcomp(pip,plp,(tp + 1)) ;
	        c += rs ;
	    }

/* our program root */

	    if (rs >= 0) {
	        rs = loadpathpr(pip,plp,prdirs) ;
	        c += rs ;
	    }

/* system-default path */

	    if ((rs >= 0) && (defpath != nullptr)) {
	        rs = loadpathcomp(pip,plp,defpath) ;
	        c += rs ;
	    }

/* process environment */

	    if ((rs >= 0) && ((tp = getenv(varname)) != nullptr)) {
	        rs = loadpathcomp(pip,plp,tp) ;
	        c += rs ;
	    }

	} /* end if (search-found) */

#if	CF_DEBUG
	if (DEBUGLEVEL(4)) {
	    debugprintf("progprocess/loadpath: ret exports> \n") ;
	    progexports(pip,"loadpath1") ;
	}
#endif

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("progprocess/loadpath: ret rs=%d c=%u\n",rs,c) ;
#endif

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (loadpath) */


local int loadpathpr(pip,plp,prdirs)
struct proginfo	*pip ;
vecstr		*plp ;
cchar	**prdirs ;
{
	int	rs = SR_OK ;
	int	i ;
	int	c = 0 ;


	for (i = 0 ; prdirs[i] != nullptr ; i += 1) {
	    rs = loadpathprdir(pip,plp,prdirs[i]) ;
	    c += rs ;
	    if (rs < 0) break ;
	} /* end for */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (loadpathpr) */


local int loadpathprdir(pip,plp,bname)
struct proginfo	*pip ;
vecstr		*plp ;
cchar	bname[] ;
{
	int	rs = SR_OK ;
	int	pl ;
	int	c = 0 ;

	char	tmpfname[MAXPATHLEN + 1] ;


	if ((rs = mkpath2(tmpfname,pip->pr,bname)) >=0) {
	    pl = rs ;
	    rs = vecstr_adduniq(plp,tmpfname,pl) ;
	    if (rs < INT_MAX) c += 1 ;
	}

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (loadpathprdir) */


local int loadpathcomp(pip,plp,pp)
struct proginfo	*pip ;
vecstr		*plp ;
cchar	*pp ;
{
	int	rs = SR_OK ;
	int	c = 0 ;
	cchar	*tp ;

	while ((tp = strbrk(pp,":;")) != nullptr) {
	    rs = loadpather(pip,plp,pp,(tp - pp)) ;
	    pp = (tp + 1) ;
	    if (rs < 0) break ;
	} /* end while */

	if ((rs >= 0) && (pp[0] != '\0')) {
	    rs = loadpather(pip,plp,pp,-1) ;
	} /* end if (trailing one) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (loadpathcomp) */


local int loadpather(pip,plp,pbuf,plen)
struct proginfo	*pip ;
vecstr		*plp ;
cchar	pbuf[] ;
int		plen ;
{
	int	rs = SR_OK ;
	int	rs1 ;
	int	pl ;
	int	c = 0 ;

	char	tmpfname[MAXPATHLEN + 1] ;


	if ((pl = pathclean(tmpfname,pbuf,plen)) > 0) {

	    rs1 = vecstr_findn(plp,tmpfname,pl) ;
	    if (rs1 == SR_NOTFOUND) {
	        c += 1 ;
	        rs = vecstr_add(plp,tmpfname,pl) ;
	    }

	} /* end if (pathclean) */

	return (rs >= 0) ? c : rs ;
}
/* end subroutine (loadpather) */


