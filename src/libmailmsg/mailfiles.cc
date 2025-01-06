/* mailfiles SUPPORT */
/* encoding=ISO8859-1 */
/* lang=C++20 */

/* manage mail files (mailboxes) */
/* version %I% last-modified %G% */

#define	CF_MAILBOXZERO	1

/* revision history:

	= 1988-02-01, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1988 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

  	Name:
	mailfiles

	Description:
	This object module is used to manage a set of mail files.

******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>		/* memset(3c)| */
#include	<usystem.h>
#include	<vecitem.h>
#include	<mallocstuff.h>
#include	<strwcpy.h>
#include	<strn.h>
#include	<localmisc.h>

#include	"mailfiles.h"


/* external subroutines */


/* external variables */


/* forward references */

static int	entry_init(MAILFILES_ENT *,const char *,int) ;
static int	entry_free(MAILFILES_ENT *) ;


/* exported subroutines */


int mailfiles_init(lp)
MAILFILES	*lp ;
{
	int	rs ;


	rs = vecitem_start(lp,5,VECITEM_PNOHOLES) ;

	return rs ;
}


int mailfiles_add(lp,path,pathlen)
MAILFILES	*lp ;
const char	path[] ;
int		pathlen ;
{
	MAILFILES_ENT		e ;

	struct ustat		sb ;

	int	rs, cl ;


	if ((lp == NULL) || (path == NULL))
	    return SR_FAULT ;

	cl = strnlen(path,pathlen) ;

	rs = entry_init(&e,path,cl) ;

	if (rs < 0)
	    goto bad0 ;

	e.lasttime = -1 ;
	e.lastsize = -1 ;
	rs = u_stat(e.mailfname,&sb) ;

#if	CF_MAILBOXZERO

	if (rs >= 0) {

	e.lasttime = sb.st_mtime ;
	e.lastsize = sb.st_size ;

	}

#else /* CF_MAILBOXZERO */

	if (rs < 0)
	    goto bad1 ;

	e.lasttime = sb.st_mtime ;
	e.lastsize = sb.st_size ;

#endif /* CF_MAILBOXZERO */

	rs = vecitem_add(lp,&e,szof(MAILFILES_ENT)) ;

	if (rs < 0)
	    goto bad1 ;

ret0:
	return rs ;

/* bad stuff */
bad1:
	entry_free(&e) ;

bad0:
	return rs ;
}


int mailfiles_addpath(lp,path,pathlen)
MAILFILES	*lp ;
const char	path[] ;
int		pathlen ;
{
	int	rs = SR_OK, n ;
	int	sl, cl ;

	char	*sp, *cp ;


	sp = (char *) path ;
	sl = strnlen(path,pathlen) ;

	if ((cp = strnrchr(sp,sl,'?')) != NULL)
	    sl -= ((sp + sl) - cp) ;

	n = 0 ;
	while ((cp = strnchr(sp,sl,':')) != NULL) {

	    cl = cp - sp ;
	    cp = sp ;

	    rs = mailfiles_add(lp,cp,cl) ;

	    if (rs < 0)
	        break ;

	    sl -= (cl + 1) ;
	    sp += (cl + 1) ;

	    n += 1 ;

	} /* end while */

	if ((rs >= 0) && (sl > 0)) {

	    rs = mailfiles_add(lp,sp,sl) ;

	    if (rs >= 0)
	        n += 1 ;

	}

	return (rs >= 0) ? n : rs ;
}
/* end subroutine (mailfiles_addpath) */


int mailfiles_get(lp,i,epp)
MAILFILES	*lp ;
int		i ;
MAILFILES_ENT	**epp ;
{
	int	rs ;


	rs = vecitem_get(lp,i,epp) ;

	return rs ;
}


int mailfiles_free(lp)
MAILFILES	*lp ;
{
	MAILFILES_ENT	*ep ;
	int		rs = SR_OK :
	int		rs1 ;
	int		i ;

	for (i = 0 ; vecitem_get(lp,i,&ep) >= 0 ; i += 1) {
	    if (ep != NULL) {
	        rs1 = entry_free(ep) ;
	        if (rs >= 0) rs = rs1 ;
	    }
	} /* end for */

	rs1 = vecitem_finish(lp) ;
	if (rs >= 0) rs = rs1 ;

	return rs ;
}


int mailfiles_count(lp)
MAILFILES	*lp ;
{
	int	rs ;


	rs = vecitem_count(lp) ;

	return rs ;
}


int mailfiles_check(lp)
MAILFILES	*lp ;
{
	MAILFILES_ENT	*ep ;

	struct ustat	sb ;

	int	rs, i ;
	int	changed = 0 ;


	if (lp == NULL)
	    return SR_FAULT ;

	for (i = 0 ; (rs = vecitem_get(lp,i,&ep)) >= 0 ; i += 1) {

	    if (ep == NULL) continue ;

	    if (u_stat(ep->mailfname,&sb) >= 0) {

	        if (! ep->f_changed) {

	            if (sb.st_size > ep->lastsize) {

	                ep->f_changed = TRUE ;
	                changed += 1 ;

	            }

	        } else {

	            if (sb.st_size < ep->lastsize)
	                ep->f_changed = FALSE ;

	        }

#ifdef	COMMENT
	        if ((changed == 0) && (sb.st_mtime > ep->lasttime))
	            f_changed = TRUE ;
#endif /* COMMENT */

	        ep->lasttime = sb.st_mtime ;
	        ep->lastsize = sb.st_size ;

	    } /* end if */

	} /* end for */

	if ((rs >= 0) && (changed > 0))
	    rs = changed ;

	return rs ;
}
/* end subroutine (mailfiles_check) */



/* INTERNAL SUBROUTINES */



static int entry_init(ep,path,pathlen)
MAILFILES_ENT	*ep ;
const char	path[] ;
int		pathlen ;
{
	int	rs ;
	char	*cp ;

	memclear(ep) ;
	ep->f_changed = FALSE ;
	rs = uc_malloc((pathlen + 1),&ep->mailfname) ;

	if (rs >= 0) {
	    rs = strwcpy(ep->mailfname,path,pathlen) - path ;
	}

	return rs ;
}


static int entry_free(ep)
MAILFILES_ENT	*ep ;
{


	if (ep->mailfname != NULL)
	    free(ep->mailfname) ;

	return 0 ;
}



