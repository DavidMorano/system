/* passwdv_proc SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* process a name */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* run-time debugging */
#define	CF_GETPASSWORD	1		/* use 'getpassword(3dam)' */

/* revision history:

	= 1998-03-01, David A­D­ Morano
	The program was written from scratch to do what the previous
	program by the same name did.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

  	Name:
	passwdv_proc

	Description:
	This module processes a single password verification request.

	Synopsis:
	int passwdv_proc(PI *pip,pwfile *pfp,bfile *ofp,cc *name) noex

	Arguments:
	pip		proginfo-pointer
	pfp		passwd-file-pointer
	ofp		output-file-pointer
	rname		username to check on

	Returns:
	0		the password verified OK
	<0		the password did not verify (system-return)

******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<pwd.h>
#include	<ctime>
#include	<csignal>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<ucmem.h>
#include	<bfile.h>
#include	<passwdent.h>
#include	<pwfile.h>
#include	<getax.h>
#include	<getusername.h>
#include	<localmisc.h>

#include	"config.h"
#include	"defs.h"


/* local defines */

#ifndef	PI
#define	PI	proginfo
#endif


/* imported namespaces */

using libuc::mem ;			/* variable */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local int	userexists(PI *,PWFILE *,cchar *) ;


/* local variables */


/* exported subroutines */

int process(PI *pip,PWFILE *pfp,bfile *ofp,cchar *name) noex {
	int		rs = SR_OK ;
	int		f_valid = false ;
	char		ubuf[USERNAMELEN + 1] ;
	char		prompt[PROMPTLEN + 1] ;
	char		password[PASSWDLEN + 1] ;
	char		*p ;

	if (name == nullptr) return SR_FAULT ;

	if (name[0] == '\0') return SR_INVALID ;

	if (name[0] == '-') {
	    if (pip->username == nullptr) {
		rs = getusername(ubuf,USERNAMELEN,-1) ;
		if (rs >= 0)
		    rs = proginfo_setentry(pip,&pip->username,ubuf,rs) ;
	    }
	    name = pip->username ;
	}

	if (rs < 0)
	    goto ret0 ;

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("process: username=%s\n",name) ;
#endif

	if (pip->debuglevel > 0)
	    bprintf(pip->efp,"%s: username=%s\n",
		pip->progname,name) ;

	if (! userexists(pip,pfp,name)) {
	    bprintf(ofp,"username=%s does not exist\n",name) ;
	    goto ret0 ;
	}

/* we have a user entry */

	rs = bufprintf(prompt,PROMPTLEN,"password user=%s: ",
	    name) ;

#if	CF_GETPASSWORD

	if (rs >= 0)
	    rs = getpassword(prompt,password,PASSWDLEN) ;

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("process: getpassword() rs=%d password=>%s<\n",
	        rs,password) ;
#endif

#else /* CF_GETPASSWORD */

	if (rs >= 0) {

	    p = getpass(prompt) ;

	    rs = sncpy1(password,PASSWDLEN,p) ;

	}

#endif /* CF_GETPASSWORD */

	if ((rs >= 0) && pip->fl.sevenbit) {
	    for (p = password ; *p != '\0' ; p += 1) {
	        *p = *p & 0x7F ;
	    }
	} /* end if (seven bit) */

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("process: password >%s<\n",password) ;
#endif

	if (rs < 0)
	    goto ret0 ;

	if (pfp != nullptr) {
	    PWFILE_ENT	pw ;
	    PWFILE_CUR	cur ;
	    cint	pwlen = PWFILE_ENTLEN ;
	    char	pwbuf[PWFILE_ENTLEN + 1] ;

#if	CF_DEBUG
	    if (DEBUGLEVEL(3))
	        debugprintf("process: using file\n") ;
#endif

	    pwfile_curbegin(pfp,&cur) ;

	    while (pwfile_fetchuser(pfp,name,&cur,&pw,pwbuf,pwlen) >= 0) {

#if	CF_DEBUG
	        if (DEBUGLEVEL(3))
	            debugprintf("process: got one >%s<\n",pw.password) ;
#endif

	        if (passwdok(password,pw.password)) {
	            f_valid = true ;
	            break ;
	        }

	    } /* end while */

	    pwfile_curend(pfp,&cur) ;

#if	CF_DEBUG
	    if (DEBUGLEVEL(3))
	        debugprintf("process: done trying\n") ;
#endif

	} else {

#if	CF_DEBUG
	    if (DEBUGLEVEL(3))
	        debugprintf("process: using system 'checkpass()'\n") ;
#endif

	    rs = checkpass(pip,name,password,true) ;
	    f_valid = (rs > 0) ;

	} /* end if */

ret0:

#if	CF_DEBUG
	if (DEBUGLEVEL(3))
	    debugprintf("process: ret rs=%d f_valid=%u\n",rs,f_valid) ;
#endif

	return (rs >= 0) ? f_valid : rs ;
}
/* end subroutine (process) */


/* local subroutines */

/* is the username present in the password database? */
local int userexists(PI *pip,PWFILE *pfp,cchar *name) noex {
    	cnullptr	np{} ;
	int		rs = SR_FAULT ;
	int		rs1 ;
	int		f = false ;
	if (pip && name) {
	    rs = SR_INVALID ;
	    if (name[0]) {
	        if ((rs = pwentrybufsize()) >= 0) {
	            cint	pwlen = rs ;
	            if (char *pwbuf ; (rs = mem.mall((pwlen+1),&pwbuf)) >= 0) {
	                if (pfp) {
			    cauto pw_fu = pwfile_fetchuser ;
	                    PWFILE_ENT	pfe ;
	                    rs = pw_fu(pfp,name,np,&pfe,pwbuf,pwlen) ;
	                    f = (rs > 0) ;
	                } else {
	                    PASSWDENT	spw ;
	                    rs = getpw_name(&spw,pwbuf,pwlen,name) ;
	                    f = (rs > 0) ;
	                }
	                rs1 = mem.free(pwbuf) ;
	                if (rs >= 0) rs = rs1 ;
	            } /* end if (memory-allocation-free) */
	        } /* end if (valid) */
	    } /* end if (pwentrybufsize) */
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (userexists) */


