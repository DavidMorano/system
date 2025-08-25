/* progentry SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* print entry information */
/* version %I% last-modified %G% */

#define	CF_DEBUGS 	0		/* compile-time debugging */
#define	CF_DEBUG 	0		/* run-time debugging */

/* revision history:

	= 1994-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:

	Description:
	This subroutine provides some miscellaneous information on
	the recipients in an email message.

	Synopsis:
	int progentryinfo(pip,ep,ind,ofp)
	PROGINFO	*pip ;
	EMA_ENT		*ep ;
	int		ind ;
	bfile		*ofp ;

	Arguments:
	- pip		program information pointer
	- ep		pointer to entry
	- ind		indentation level
	- ofp		output file object pointer

	Returns:
	>=0		OK
	<0		error code

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/stat.h>
#include	<unistd.h>
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<ascii.h>
#include	<bfile.h>
#include	<msg.h>
#include	<ema.h>
#include	<localmisc.h>

#include	"config.h"
#include	"defs.h"
#include	"ema_local.h"


/* local defines */

#define	MAXINDENT	7


/* external subroutines */

#if	CF_DEBUGS || CF_DEBUG
extern int	debugprintf(cchar *,...) noex ;
#endif


/* external variables */


/* local structures */


/* forward references */

static cchar	*atype(int) noex ;


/* local variables */

constexpr cpcchar	atypes[] = {
	    "R",			/* regular */
	    "P",			/* PCS list */
	    "L",			/* local alias */
	    "S",			/* system-wide alias */
	    "G",			/* ARPA group list */
	    nullptr
} ;

constexpr char		blanks[] = "              " ;


/* exported variables */


/* exported subroutines */

int progentryinfo(PROGINFO *pip,bfile *ofp,EMA_ENT *ep,int ind) noex {
	int		rs ;
	int		al = 0 ;
	int		wlen = 0 ;
	cchar		*ap = nullptr ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	debugprintf("progentryinfo: ent ind=%d\n",ind) ;
#endif

	if (pip == nullptr) return SR_FAULT ;
	if (ep == nullptr) return SR_FAULT ;

	if (ind > MAXINDENT) ind = MAXINDENT ;

	if (ep->ap != nullptr) {
	    ap = ep->ap ;
	    al = ep->al ;
	} else if (ep->rp != nullptr) {
	    ap = ep->rp ;
	    al = ep->rl ;
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	debugprintf("progentryinfo: a=>%r<\n",ap,al) ;
#endif

	if ((rs = bprintf(ofp,"%s %r",atype(ep->type),blanks,ind)) >= 0) {
	    wlen += rs ;
	    if ((rs >= 0) && (ep->rp != nullptr) && (ep->rl > 0)) {
		if ((rs = bputc(ofp,' ')) >= 0) {
		    rs = bwrite(ofp,ep->rp,ep->rl) ;
		    wlen += (rs+1) ;
		}
	    }
	    if ((rs >= 0) && (ep->ap != nullptr) && (ep->al > 0)) {
		    rs = bprintf(ofp," ­ %r",ep->ap,ep->al) ;
		    wlen += rs ;
	    }
	    if ((rs >= 0) && (ep->cp != nullptr) && (ep->cl > 0)) {
		        cint ml = MIN(ep->cl,(pip->linelen-wlen-3)) ;
		        rs = bprintf(ofp," (%r)",ep->cp,ml) ;
		        wlen += rs ;
	    }
	    if (rs >= 0) { 
		rs = bputc(ofp,'\n') ;
		wlen += 1 ;
	    }
	} /* end if */

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("progentryinfo: ret rs=%d wlen=%u\n",rs,wlen) ;
#endif

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (progentryinfo) */


int progentryaddr(PROGINFO *pip,bfile *ofp,EMA_ENT *ep,int ind)
{
	CMD_LOCAL	*lsp = pip->lsp ;
	int		rs = SR_OK ;
	int		spc = 0 ;
	int		wlen = 0 ;

#if	CF_DEBUG
	if (DEBUGLEVEL(5))
	    debugprintf("progentryaddr: ent ind=%u\n",ind) ;
#endif

	if (ep == nullptr) return SR_FAULT ;

	if (ind > MAXINDENT) ind = MAXINDENT ;

	if ((rs >= 0) && (ind > 0)) {
	    rs = bwrite(ofp,blanks,ind) ;
	    wlen += rs ;
	}

	if ((rs >= 0) && lsp->afl.original) {

#if	CF_DEBUG
	    if (DEBUGLEVEL(5))
	        debugprintf("progentryaddr: ORIG\n") ;
#endif

	    if (ep->op != nullptr) {
	        rs = bprintf(ofp,"»%s«",ep->op) ;
	        wlen += rs ;
	    }

	    spc += 1 ;
	}

	if ((rs >= 0) && lsp->afl.best) {
	    cchar	*vp ;

#if	CF_DEBUG
	    if (DEBUGLEVEL(5))
	        debugprintf("progentryaddr: BEST\n") ;
#endif

	    if (spc > 0) {
	        rs = bwrite(ofp," ",2) ;
	        wlen += rs ;
	    }

	    if (ep->rp != nullptr) {
	        vp = ep->rp ;
	    } else if (ep->ap != nullptr) {
	        vp = ep->ap ;
	    } else if (ep->cp != nullptr) {
	        vp = ep->cp ;
	    }

	    if (rs >= 0) {
	        rs = bwrite(ofp,vp,-1) ;
	        wlen += rs ;
	    }

	    spc += 1 ;
	}

	if ((rs >= 0) && lsp->afl.any) {
	    cchar	*vp ;

#if	CF_DEBUG
	    if (DEBUGLEVEL(5))
	        debugprintf("progentryaddr: ANY\n") ;
#endif

	    if (ep->ap != nullptr) { /* address */
	        vp = ep->ap ;
	    } else if (ep->rp != nullptr) { /* route */
	        vp = ep->rp ;
	    } else if (ep->cp != nullptr) { /* comment */
	        vp = ep->cp ;
	    }

	    rs = bprintf(ofp,"ª%sª",vp) ;
	    wlen += rs ;
	    spc += 1 ;
	}

	if ((rs >= 0) && lsp->afl.address) {

#if	CF_DEBUG
	    if (DEBUGLEVEL(5)) {
	        debugprintf("progentryaddr: ADDR\n") ;
	        debugprintf("progentryaddr: al=%u a=>%s<\n",
	            strlen(ep->ap),ep->ap) ;
	    }
#endif

	    if (spc > 0) {
	        rs = bputc(ofp,' ') ;
	        wlen += rs ;
	    }

	    if ((rs >= 0) && (ep->ap != nullptr)) {
	        rs = bwrite(ofp,ep->ap,-1) ;
	        wlen += rs ;
	    }

	    spc += 1 ;
	}

	if ((rs >= 0) && lsp->afl.route) {

#if	CF_DEBUG
	    if (DEBUGLEVEL(5))
	        debugprintf("progentryaddr: ROUTE\n") ;
#endif

	    if (spc > 0) {
	        rs = bputc(ofp,' ') ;
	        wlen += rs ;
	    }

	    if ((rs >= 0) && (ep->rp != nullptr)) {
	        rs = bwrite(ofp,ep->rp,-1) ;
	        wlen += rs ;
	    }

	    spc += 1 ;
	}

	if ((rs >= 0) && lsp->afl.comment) {

#if	CF_DEBUG
	    if (DEBUGLEVEL(5))
	        debugprintf("progentryaddr: COMM\n") ;
#endif

	    if (spc > 0) {
	        rs = bputc(ofp,' ') ;
	        wlen += rs ;
	    }

	    if ((rs >= 0) && (ep->cp != nullptr)) {
		cint	ml = MIN(ep->cl,(pip->linelen-wlen-2)) ;
	        rs = bprintf(ofp,"(%r)",ep->cp,ml) ;
	        wlen += rs ;
	    }

	    spc += 1 ;
	}

	if (rs >= 0) {
	    rs = bprintf(ofp,"\n") ;
	    wlen += rs ;
	}

#if	CF_DEBUG
	if (DEBUGLEVEL(5))
	    debugprintf("progentryaddr: ret rs=%d wlen=%u\n",rs,wlen) ;
#endif

	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (progentryaddr) */


/* local subroutines */

static cchar *atype(int type) noex {
	return (type < nelements(atypes)) ? atypes[type] : "U" ;
}
/* end subroutine (atype) */


