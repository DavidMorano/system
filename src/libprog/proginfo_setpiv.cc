/* proginfo_setpiv */

/* utility for KSH built-in commands */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	proginfo_setpiv

	Description:
	This subroutine is used for acquiring the program-root for
	a program.

	Synopsis:
	int proginfo_setpiv(proginfo *pip,cchar *pr,pivars *vars) noex

	Arguments:
	pip		program-information pointer
	pr		program root
	vars		array of program parameters

	Returns:
	>=0	length of PR
	<0	error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<ids.h>
#include	<localmisc.h>

#include	"proginfo.hh"


/* local defines */

#define	PIVARS		struct pivars

#define	PI		proginfo


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

static int	proginfo_setpiver(proginfo *,ids *,cchar *,PIVARS *) noex ;

static int	sfrootdirname(cchar *,int,cchar **) noex ;

static int	dircheck(ids *,cchar *) noex ;
static bool	isNotGoodDir(int) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int proginfo_setpiv(PI *pip,cchar *pr,PIVARS *vars) noex {
	ids		id ;
	int		rs ;
	int		rs1 ;

	if ((rs = ids_load(&id)) >= 0) {
	    {
	        rs = proginfo_setpiver(pip,&id,pr,vars) ;
	    }
	    rs1 = ids_release(&id) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (ids) */

	return rs ;
}
/* end subroutine (proginfo_setpiv) */


/* local subroutines */

int proginfo_setpiver(PI *pip,ids *idp,cchar *pr,PIVARS *vars) noex {
	cint		plen = MAXPATHLEN ;
	int		rs = SR_OK ;
	int		rs1 ;
	int		pl = -1 ;
	int		prlen = 0 ;
	cchar		*cp = NULL ;
	char		rdn[MAXPATHLEN + 1] ;

	if (pr == NULL) {
	    int		i ;
	    pl = -1 ;
	    rs1 = SR_NOTFOUND ;
	    for (i = 1 ; (rs >= 0) && isNotGoodDir(rs1) && (i <= 3) ; i += 1) {
	        cchar	*var  = NULL ;
	        switch (i) {
	        case 1:
	            var = vars->vpr1 ;
	            break ;
	        case 2:
	            var = vars->vpr2 ;
	            break ;
	        case 3:
	            var = vars->vpr3 ;
	            break ;
	        } /* end switch */
	        if ((var != NULL) && (var[0] != '\0')) {
	            if ((rs1 = proginfo_getenv(pip,var,-1,&cp)) >= 0) {
	                rs1 = dircheck(idp,cp) ;
	                if (! isNotGoodDir(rs1)) rs = rs1 ;
	            }
	        }
	    } /* end for */

	    if (rs1 >= 0) pr = cp ;
	} /* end if (straight out variables) */

	if ((rs >= 0) && (pr == NULL)) {
	    char	nn[MAXNAMELEN + 1] ;
	    char	dn[MAXHOSTNAMELEN + 1] ;

	    if ((rs1 = getnodedomain(nn,dn)) >= 0) {
	        cchar	**vpp = &pip->nodename ;
	        if ((rs = proginfo_setentry(pip,vpp,nn,-1)) >= 0) {
	            cchar	**vpp = &pip->domainname ;
	            if ((rs = proginfo_setentry(pip,vpp,dn,-1)) >= 0) {
	                rs1 = getrootdname(rdn,plen,vars->vprname,dn) ;
	                pl = rs1 ;
	            }
	        }

	        if ((rs >= 0) && (rs1 > 0)) {
	            pr = rdn ;
		}

	    } /* end if (getnodedomain) */

	} /* end if (guess program root from domain name) */

/* try to see if a path was given at invocation */

	if ((rs >= 0) && (pr == NULL)) {

	    if (pip->progdname == NULL) {
	        rs = proginfo_progdname(pip) ;
	    }

	    if ((rs >= 0) && (pip->progdname != NULL)) {
	        cchar	*cp ;
	        pl = sfrootdirname(pip->progdname,-1,&cp) ;
	        if (pl > 0) pr = cp ;
	    }

	} /* end if (set program-root from program d-name) */

/* default is a fixed string (from the initialization variables) */

	if ((rs >= 0) && (pr == NULL)) {
	    pr = vars->pr ;
	    pl = -1 ;
	}

/* enter it in if we have found it */

	if ((rs >= 0) && (pr != NULL)) {
	    rs = proginfo_setprogroot(pip,pr,pl) ;
	    prlen = rs ;
	}

	return (rs >= 0) ? prlen : rs ;
}
/* end subroutine (proginfo_setpiver) */


static int sfrootdirname(cchar *dp,int dl,cchar **rpp)
{
	int		bl ;
	int		sl = -1 ;
	int		f ;
	cchar		*sp = NULL ;
	cchar		*bp ;

	if (rpp != NULL) *rpp = NULL ;

	bl = sfbasename(dp,dl,&bp) ;

	f = ((bl == 3) && (strncmp(bp,"bin",bl) == 0)) ;

	if (! f) {
	    f = ((bl == 4) && (strncmp(bp,"sbin",bl) == 0)) ;
	}

	if (f) {
	    sl = sfdirname(dp,dl,&sp) ;

	    if ((sl >= 0) && (rpp != NULL)) *rpp = sp ;
	}

	return sl ;
}
/* end subroutine (sfrootdirname) */

static int dircheck(ids *idp,cchar *dname) noex {
	USTAT		sb ;
	int		rs ;
	if ((rs = u_stat(dname,&sb)) >= 0) {
	    if (S_ISDIR(sb.st_mode)) {
	        cint	am = (R_OK|X_OK) ;
	        rs = sperm(idp,&sb,am) ;
	    } else {
	        rs = SR_NOTDIR ;
	    }
	}
	return rs ;
}
/* end subroutine (dircheck) */

static bool isNotGoodDir(int rs) noex {
	bool		f = false ;
	f = f || isNotPresent(rs) ;
	f = f || (rs == SR_NOTDIR) ;
	return f ;
}
/* end if (isNotGoodDir) */


