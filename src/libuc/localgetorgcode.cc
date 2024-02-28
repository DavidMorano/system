/* localgetorgcode SUPPORT */
/* lang=C++20 */

/* get the LOCAL organization-code (ORGCODE) */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* compile-time debugging */

/* revision history:

	= 1998-05-01, David A­D­ Morano
	This subroutine is originally written. This is a minimal
	implementation.

	= 2018-09-14, David A.D. Morano
	I modified this to use |snabbr(3dam)| instead of a local
	custom thing which did the same thing, only not as well.
	So we go with the library solution instead of the local
	custom one. Note for future: might want to refactor a bit
	to get a loop of attempts to find the ORGCODE rather than
	the current (older) list-like code pattern.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	localgetorgcode

	Description:
	This subroutine is used to (try to) get the LOCAL software
	distribution organization-code (ORGCODE). An (so-called)
	ORGCODE is normally an abbreviation of an organiztion name
	(itself normally multiple words) consisting of the first
	letter of each word, but capitalized.

	Synopsis:
	int localgetorgcode(cchar *pr,char *rbuf,int rlen,cchar *un) noex

	Arguments:
	pr		program root
	rbuf		caller supplied buffer to place result in
	rlen		length of caller supplied buffer
	un		username

	Returns:
	>=0		length of returned ID
	<0		error in process of creating ID (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<filereadln.h>
#include	<localmisc.h>

#include	"localget.h"


/* local defines */

#ifndef	VARORGCODE
#define	VARORGCODE	"ORGCODE"
#endif

#ifndef	ORGLEN
#define	ORGLEN		MAXNAMELEN
#endif

#define	ETCDNAME	"etc"
#define	ORGCODEFNAME	"orgcode"


/* external subroutines */

extern int	sncpy1(char *,int,cchar *) ;
extern int	snabbr(char *,int,cchar *,int) ;
extern int	mkpath2(char *,cchar *,cchar *) ;
extern int	mkpath3(char *,cchar *,cchar *,cchar *) ;
extern int	nextfield(cchar *,int,cchar **) ;
extern int	getuserhome(char *,int,cchar *) ;
extern int	getuserorg(char *,int,cchar *) ;
extern int	localgetorg(cchar *,char *,int,cchar *) ;
extern int	touc(int) ;
extern int	isNotPresent(int) ;


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int localgetorgcode(cchar *pr,char *rbuf,int rlen,cchar *un) noex {
	int		rs = SR_OK ;
	int		len = 0 ;
	cchar		*etcdname = ETCDNAME ;
	cchar		*ocfname = ORGCODEFNAME ;
	cchar		*orgcode = getenv(VARORGCODE) ;
	char		tfname[MAXPATHLEN+1] ;

	if (pr == NULL) return SR_FAULT ;
	if (rbuf == NULL) return SR_FAULT ;

	if (pr[0] == '\0') return SR_INVALID ;

	rbuf[0] = '\0' ;

/* user environment */

	if ((len <= 0) && ((rs >= 0) || isNotPresent(rs))) {
	    if ((orgcode != NULL) && (orgcode[0] != '\0')) {
	        rs = sncpy1(rbuf,rlen,orgcode) ;
	        len = rs ;
	    }
	}

/* user configuration */

	if ((len <= 0) && ((rs >= 0) || isNotPresent(rs))) {
	    cint	hlen = MAXPATHLEN ;
	    char	hbuf[MAXPATHLEN+1] ;
	    if ((un == NULL) || (un[0] == '\0')) un = "-" ;
	    if ((rs = getuserhome(hbuf,hlen,un)) >= 0) {
		if ((rs = mkpath3(tfname,hbuf,etcdname,ocfname)) >= 0) {
		    rs = filereadln(tfname,rbuf,rlen) ;
		    len = rs ;
		}
	    } /* end if (getuserhome) */
	}

/* software facility (LOCAL) configuration */

	if ((len <= 0) && ((rs >= 0) || isNotPresent(rs))) {
	    if ((rs = mkpath3(tfname,pr,etcdname,ocfname)) >= 0) {
	        rs = filereadln(tfname,rbuf,rlen) ;
	        len = rs ;
	    }
	}

/* any operating system configuration (in '/etc') */

	if ((len <= 0) && ((rs >= 0) || isNotPresent(rs))) {
	    if ((rs = mkpath3(tfname,"/",etcdname,ocfname)) >= 0) {
	        rs = filereadln(tfname,rbuf,rlen) ;
	        len = rs ;
	    }
	}

/* create it out of the abbreviation of the organization name */

	if ((len <= 0) && ((rs >= 0) || isNotPresent(rs))) {
	    cint	orglen = ORGLEN ;
	    char	orgbuf[ORGLEN+1] ;
	    if ((un == NULL) || (un[0] == '\0')) un = "-" ;
	    rs = getuserorg(orgbuf,orglen,un) ;
	    if ((rs == SR_NOENT) || (rs == 0)) {
	        rs = localgetorg(pr,orgbuf,orglen,un) ;
	    }
	    if (rs > 0) {
		rs = snabbr(rbuf,rlen,orgbuf,rs) ;
		len = rs ;
	    }
	}

/* get out (nicely as possible in our case) */

	if ((rs < 0) && isNotPresent(rs)) {
	    rs = SR_OK ;
	    len = 0 ;
	}

#if	CF_DEBUGS
	debugprintf("localgetorgcode: ret rs=%d org=>%s<\n",rs,rbuf) ;
#endif

	return (rs >= 0) ? len : rs ;
}
/* end subroutine (localgetorgcode) */


/* local subroutines */

