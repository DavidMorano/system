/* mkgecosname SUPPORT */
/* lang=C++20 */

/* fix the GECOS name as it comes out of the password file ('/etc/passwd') */
/* version %I% last-modified %G% */

#define	CF_GECOSHYPHEN	1		/* handle hyphen characters in name */

/* revision history:

	= 1998-08-12, David A­D­ Morano
	This subroutine was originally adopted from a similar
	subroutine that was a part of the PCS package distribution.
	However, the original was garbage so this version is
	essentially a complete rewrite of the original. Also the
	name of the subroutine is changed from the original.  The
	original name of the subroutine was GECOSNAME.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************
									
	Name:
	mkgecosname

	Description:
	This subroutine gets the GECOS version of a user name (a
	type of a real name) from the fifth field of the system
	password file ('/etc/passwd').

	Synopsis:
	int mkgecosname(char *rbuf,int rflen,cchar *gf) noex

	Arguments:
	rbuf		user supplied result buffer to contain the full-name
	rlen		length of the user supplied result buffer
	gf		input buffer containing the GECOS field string

	Returns:
	>=0	length of GECOS name
	<0	failed (system-return)


*	SUBROUTINES CALLED:						
*		
		getgecosname(3dam)
*									
*	GLOBAL VARIABLES USED:						
*		None!  AS IT SHOULD BE!
*									

	Extra Notes:

	The GECOS field of the PASSWD database should be formatted
	in one of the following ways:

	    name,office,work_phone,home_phone
	    organization-name(account,bin)office,work_phone,home_phone,printer
	    name(account)office,work_phone,home_phone,printer
	    name

	Note also that an ampersand character ('&') that appears
	anywhere in the GCOS field is to be logically replaced by
	the corresponding username of the entry.

	The 'name' part of the GCOS entry may contain hyphen ('-')
	characters.

	The original AT&T GECOS field contained:

	    department-name(account,bin)

	and was further put into a 'struct comment' with fields:

	    c_dept
	    c_name
	    c_acct
	    c_bin

	Some modern possibilities for the GECOS field are:

	    org_dept-name(account,bin)office,work_phone,home_phone
	    org_dept-name(account,bin)office,work_phone,home_phone,printer

	Actual examples:

	    XNR64430-d.a.morano(126483,BIN8221)
	    rockwell-d.a.morano(126283,BIN8221)4B-411,5336,6175679484,hp0

	Design note:

	Note that in the |getgecosname(3dam)| subroutine below, we
	use the subroutine |strnlen(3dam)| to get the length of the
	supplied GECOS string length if one was not also given.
	This is done in order to guard against the problem of an
	indefinitely long GECOS string being given to us (in order
	words: garbage) for whatever reason.

	I do not really know how an indefinitely long GECOS string
	-- and one that doesn't have traditional GECOS special
	characters in it -- could get passed to us from reasonable
	callers.  But then again, not all of our callers are always
	reasonable!

	Our goal is to prevent stray memory references -- in order
	to avoid core dumps!  Someone else can have core dumps if
	they want them -- but we do not!  Deal with it!

	If everyone guarded their subroutines the way that we try
	to do, maybe there would not be so much crap out there --
	like all of the core dumping that is going on!

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<climits>
#include	<cstring>		/* for |strlen(3c)| */
#include	<usystem.h>
#include	<strn.h>
#include	<sncpyx.h>
#include	<snwcpy.h>
#include	<localmisc.h>

#include	"mkgecosname.h"


/* local defines */

#ifndef	CH_LPAREN
#define	CH_LPAREN	0x28
#endif

#ifndef	MAXGECOSLEN
#define	MAXGECOSLEN	1024
#endif

#ifndef	CF_GECOSHYPHEN
#define	CF_GECOSHYPHEN	1		/* handle hyphen characters in name */
#endif


/* local namespaces */

using std::nullptr_t ;			/* type */


/* local typedefs */


/* external subroutines */


/* local structures */


/* forward references */

int		getgecosname(cchar *,int,cchar **) noex ;


/* local variables */

constexpr bool	f_hyphen = CF_GECOSHYPHEN ;


/* exported variables */


/* exported subroutines */

int mkgecosname(char *rbuf,int rlen,cchar *gf) noex {
	int		rs = SR_FAULT ;
	if (rbuf && gf) {
	    cchar	*cp{} ;
	    rs = SR_NOTFOUND ;
	    if ((rs = getgecosname(gf,-1,&cp)) >= 0) {
		cint	cl = rs ;
	        if constexpr (f_hyphen) {
	            rs = snwcpyhyphen(rbuf,rlen,cp,cl) ;
	        } else {
	            rs = snwcpy(rbuf,rlen,cp,cl) ;
	        }
	    } /* end if (getgecosname) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (mkgecosname) */

int getgecosname(cchar *gbuf,int glen,cchar **rpp) noex {
	int		rs = SR_FAULT ;
	int		cl = 0 ;
	cchar		*cp = nullptr ;
	if (gbuf) {
	    nullptr_t	np{} ;
	    cint	sch = CH_LPAREN ;
	    cchar	*tp ;
	    bool	f = true ;
	    if (glen < 0) glen = strnlen(gbuf,MAXGECOSLEN) ;
	    f = f && ((cp = strnchr(gbuf,glen,'-')) != np) ;
	    f = f && ((tp = strnchr(cp,(glen - (cp - gbuf)),sch)) != np) ;
	    if (f) {
	        cp += 1 ;
	        cl = (tp - cp) ;
	    } else if ((tp = strnchr(gbuf,glen,sch)) != nullptr) {
	        cp = gbuf ;
	        cl = (tp - gbuf) ;
	    } else if ((tp = strnchr(gbuf,glen,'-')) != nullptr) {
	        cp = (tp + 1) ;
	        cl = glen - ((tp + 1) - gbuf) ;
	    } else {
	        cp = gbuf ;
	        cl = glen ;
	    } /* end if */
	} /* end if (non-null) */
	if (rpp) *rpp = cp ;
	return (rs >= 0) ? cl : rs ;
}
/* end subroutine (getgecosname) */


