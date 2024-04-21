/* getuserterms SUPPORT */
/* lang=C++20 */

/* get a list (returned as vector of c-strings) of the logged in user terms */
/* version %I% last-modified %G% */


/* revision history:

	= 1999-01-10, David A­D­ Morano
	This subroutine was originally written.  It was prompted by
	the failure of other terminal message programs from finding
	the proper controlling terminal.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getuserterms

	Descrption:
	This subroutine will find and return a list (in a VECSTR
	object) of the logged-in terminals of the given username.

	Synopsis:
	int getuserterms(vecstr *lp,cchar *username) noex

	Arguments:
	- listp		pointer to VECSTR to receive terminals
	- username	session ID to find controlling terminal for

	Returns:
	>=	number of entries returned
	<0	error code (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<time.h>
#include	<stdlib.h>
#include	<string.h>
#include	<usystem.h>
#include	<vecstr.h>
#include	<vecitem.h>
#include	<tmpx.h>
#include	<strwcpy.h>
#include	<localmisc.h>

#include	"getuserterms.h"


/* local defines */

#ifndef	DEVDNAME
#define	DEVDNAME	"/dev/"
#endif

#define	TE		termentry


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* local structures */

struct xtermentry {
	cchar		*devpath ;
	time_t		atime ;
} ;


/* forward references */

static int	entry_start(TE *,char *,int,time_t) noex ;
static int	entry_finish(TE *) noex ;

static int	getatime(cchar *,time_t *) ;
static int	revsortfunc(TE **,TE **) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int getuserterms(vecstr *lp,cchar *username) noex {
	tmpx		utmp ;
	tmpx_ent	ue ;

	VECITEM		el ;

	time_t	ti_access ;

	int	rs ;
	int	rs1 ;
	int	i, tlen ;
	int	n = 0 ;
	int	f ;

	char	termfname[MAXPATHLEN + 1] ;


	if (lp == nullptr)
	    return SR_FAULT ;

	if ((username == nullptr) || (username[0] == '\0'))
	    return SR_INVALID ;

	rs = vecitem_start(&el,10,VECITEM_PSORTED) ;
	if (rs < 0)
	    goto bad0 ;

/* loop through */

	strcpy(termfname,DEVDNAME) ;

	if ((rs = tmpx_open(&utmp,nullptr,O_RDONLY)) >= 0) {
	    tmpx_cur	cur ;

	    if ((rs = tmpx_curbegin(&utmp,&cur)) >= 0) {

	        while (rs >= 0) {
	            rs1 = tmpx_fetchuser(&utmp,&cur,&ue,username) ;
		    if (rs1 == SR_NOTFOUND) break ;
		    rs = rs1 ;
		    if (rs < 0) break ;

		    f = false ;
	            f = f || (ue.ut_type != TMPX_TUSERPROC) ;
	            f = f || (ue.ut_line[0] == '\0') ;
#ifdef	COMMENT
	            f = f || (strncmp(username,ue.ut_user,32) != 0)
#endif
		    if (f)
	                continue ;

	            tlen = strwcpy((termfname + 5),ue.ut_line,32) - termfname ;

/* check the access time of this terminal and if it is enable for notices */

	            rs = getatime(termfname,&ti_access) ;

	            if (rs >= 0) {
	                TE	te ;

	                rs = entry_start(&te,termfname,tlen,ti_access) ;

	                if (rs >= 0) {
	                    int	size = sizeof(TE) ;
	                    rs = vecitem_add(&el,&te,size) ;
	                }

	                if (rs >= 0) {
	                    n += 1 ;
	                } else
	                    entry_finish(&te) ;

	            } /* end if (we had a better one) */

	        } /* end while (looping through entries) */

	        tmpx_curend(&utmp,&cur) ;
	    } /* end if */

	    tmpx_close(&utmp) ;
	} /* end if (UTMPX open) */

	if ((rs >= 0) && (n > 0)) {
	    TE	*ep ;

	    vecitem_sort(&el,revsortfunc) ;

	    for (i = 0 ; vecitem_get(&el,i,&ep) >= 0 ; i += 1) {
	        if (ep != nullptr) {
	            rs = vecstr_add(lp,ep->devpath,-1) ;
	            entry_finish(ep) ;
	        }
		if (rs < 0) break ;
	    } /* end for */

	} /* end if */

	{
	    TE	*ep ;
	    for (i = 0 ; vecitem_get(&el,i,&ep) >= 0 ; i += 1) {
	        if (ep != nullptr) {
	            entry_finish(ep) ;
		}
	    } /* end for */
	}

	vecitem_finish(&el) ;

bad0:
ret0:
	return (rs >= 0) ? n : rs ;
}
/* end subroutine (getuserterms) */


/* local subroutines */

static int entry_start(TE *ep,cc *name,int nlen,time_t t) noex {
	int		rs ;
	cchar		*cp ;
	ep->atime = t ;
	if ((rs = uc_mallocstrw(name,nlen,&cp)) >= 0) {
	    ep->devpath = cp ;
	}
	return rs ;
}
/* end subroutine (entry_start) */

static int entry_finish(TE *ep) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (ep) {
	    rs = SR_OK ;
	    if (ep->devpath) {
	        rs1 = uc_free(ep->devpath) ;
	        if (rs >= 0) rs = rs1 ;
	        ep->devpath = nullptr ;
	    }
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (entry_finish) */

static int getatime(cc *termdev,time_t *tp) noex {
	USTAT	sb ;
	int	rs ;
	*tp = 0 ;
	if ((rs = u_stat(termdev,&sb)) >= 0) {
	    *tp = sb.st_atime ;
	    if ((sb.st_mode & S_IWGRP) != S_IWGRP) {
	        rs = SR_RDONLY ;
	    }
	} /* end if */
	return rs ;
}
/* end subroutine (getatime) */

static int revsortfunc(TE **f1pp,RE **f2pp) noex {
	int		rc = 0 ;
	if ((f1pp == nullptr) && (f2pp == nullptr))
	    return 0 ;

	if (f1pp == nullptr)
	    return 1 ;

	if (f2pp == nullptr)
	    return -1 ;

		rc = ((*f2pp)->atime - (*f1pp)->atime) ;
	return rc ;
}
/* end subroutine (revsortfunc) */


