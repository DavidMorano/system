/* emit_mailbox SUPPORT (bbnews) */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* emit (process) an article */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* compile-time debugging */
#define	CF_DEBUG	0		/* run-time debugging */

/* revision history:

	= 1994-11-01, David A­D­ Morano
	- added a mode to intercept for mailbox use

	= 1994-12-01, David A­D­ Morano
	Modified to only print out header fields that a user is
	normally interested in.

	= 1995-07-01, David A­D­ Morano
	- extensively modified to add:
		article follow-up capability
		article previous
		article printing
		article piping & redirecting

	= 1998-11-22, David A­D­ Morano
        I did some clean-up.

*/

/* Copyright © 1994,1995,1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Description:
	This subroutine is one of the "EMIT" subroutines used for
	"emitting" articles in different ways.

	Synopsis:
	int emit_mailbox(pip,dsp,ai,aep,ngdir,af)
	struct proginfo	*pip ;
	MKDIRLIST_ENT	*dsp ;
	int		ai ;
	ARTLIST_ENT	*aep ;
	char		ngdir[] ;
	char		af[] ;

	Arguments:
	pip		program information pointer
	dsp		user structure pointer
	ai		article index within newsgroup
	aep		article ARTLIST_ENT pointer
	ngdir		directory (relative to spool directory) of article
	af		article base file name

	Returns:
	<0		error
	>=0		EMIT-code

*******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<pwd.h>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<strings.h>
#include	<bfile.h>
#include	<headerkeys.h>		/* LIBMAILMSG */
#include	<mkdirlist.h>		/* LIBPCS */
#include	<artlist.h>		/* LIBPCS */
#include	<localmisc.h>
#include	<libdebug.h>		/* LIBDEBUG */

#include	"config.h"
#include	"defs.h"


/* local defines */

#define	MSGENVDATELEN	100
#define	MSGENVLEN	(MSGENVDATELEN+100)


/* external subroutines */


/* external variables */


/* forward references */

local int procmsgenv(proginfo *,bfile *,bfile *) noex ;


/* local variables */


/* exported variables */


/* exported subroutines */

int emit_mailbox(pip,dsp,ai,ap,ngdir,af)
proginfo	*pip ;
MKDIRLIST_ENT	*dsp ;
int		ai ;
ARTLIST_ENT	*ap ;
cchar	ngdir[] ;
cchar	af[] ;
{
	bfile	*ofp = pip->ofp ;
	int	rs = SR_OK ;
	int	wlen = 0 ;
	cchar	*nd = pip->newsdname ;
	char	afname[MAXPATHLEN + 1] ;

	if (ngdir == nullptr) return EMIT_DONE ;
	if (af == nullptr) return EMIT_OK ;

#if	CF_DEBUG
	if (DEBUGLEVEL(4)) {
	    debugprintf("emit_mailbox: ng=%s\n",ngdir) ;
	    debugprintf("emit_mailbox: af=%s\n",af) ;
	}
#endif

	if ((rs = mkpath3(afname,nd,ngdir,af)) >= 0) {
	    bfile	afile, *afp = &afile ;
	    if ((rs = bopen(afp,afname,"r",0666)) >= 0) {
		cint	llen = LINEBUFLEN ;
		int		len ;
		int		line = 0 ;
		char		lbuf[LINEBUFLEN+1] ;

		while ((rs = breadln(afp,lbuf,llen)) > 0) {
		    len = rs ;
		    if (line++ == 0) {
			if (strncmp(lbuf,"From ",5) != 0) {
			    rs = procmsgenv(pip,ofp,afp) ;
			    wlen += rs ;
#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("emit_mailbox: procmsgenv() rs=%d\n",rs) ;
#endif
			}
		    }
		    if (rs >= 0) {
	                rs = bwrite(ofp,lbuf,len) ;
			wlen += rs ;
		    }

		    if (rs < 0) break ;
		} /* end while */

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("emit_mailbox: while-out rs=%d\n",rs) ;
#endif

		bclose(afp) ;
	    } /* end if (bfile) */
#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("emit_mailbox: bfile-out rs=%d\n",rs) ;
#endif
	} /* end if */

#if	CF_DEBUG
	if (DEBUGLEVEL(4))
	    debugprintf("emit_mailbox: ret rs=%d\n",rs) ;
#endif

	return rs ;
}
/* end subroutine (emit_mailbox) */


/* local subroutines */

local int procmsgenv(profinfo *pip,bfile *ofp,bfile *afp) noex {
	int	rs ;
	int	wlen = 0 ;
	if (ustat sb ; (rs = bstat(afp,&sb)) >= 0) {
	    cint	ulen = USERNAMELEN ;
	    char	ubuf[USERNAMELEN+1] ;
	    if ((rs = getusername(ubuf,ulen,sb.st_uid)) >= 0) {
		cint	dlen = MSGENVDATELEN ;
		char		dbuf[MSGENVDATELEN+1] ;
		if ((rs = progmsgenv_envstr(pip,dbuf,dlen)) >= 0) {
		    cint	elen = MSGENVLEN ;
		    cchar	*fmt = "From %s %s" ;
		    char	ebuf[MSGENVLEN+1] ;
		    if ((rs = bufprintf(ebuf,elen,fmt,ubuf,dbuf)) >= 0) {
			rs = bprintln(ofp,ebuf,rs) ;
			wlen += rs ;
		    }
		}
	    } /* end if (getusername) */
	} /* end if (bstat) */
	return (rs >= 0) ? wlen : rs ;
}
/* end subroutine (procmsgenv) */


