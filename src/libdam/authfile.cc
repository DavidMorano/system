/* authfile SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* subroutine to read an authorization file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-12, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	authfile

	Description:
	This subroutine reads the authorization file (if supplied
	by the invoking user) and returns (possibly) a username, a
	password, one, none, or both of these.

	Synospsis:
	int authfile(char *username,char *password,cchar *fname) noex

	Arguments:
	username	result servname
	password	result password
	fname		authorization file

	Returns:
	>0		success
	==		not-successful
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<fcntl.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<linebuffer.h>		/* CSTD */
#include	<fieldterminit.hh>	/* LIBUC */
#include	<field.h>		/* LIBUC */
#include	<strdcpyx.h>		/* LIBUC */
#include	<mkchar.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU |LINEBUFLEN| */
#include	<bfile.h>		/* LIBB */

#include	"authfile.h"


/* local defines */

#ifndef	MAXAUTHLEN
#define	MAXAUTHLEN	64
#endif


/* external subroutines */


/* external variables */


/* local structures */

namespace {
    struct suber {
	char		*username ;
	char		*password ;
	linebuffer	lb ;
	suber(char *up,char *pp) noex : username(up), password(pp) { } ;
	int start	() noex ;
	int finish	() noex ;
	int procfile	(cchar *) noex ;
	int procline	(int) noex ;
    } ; /* end struct (suber) */
} /* end namespace */


/* forward references */


/* local variables */

constexpr fieldterminit		ft("\t #=") ;


/* exported variables */


/* exported subroutines */

int authfile(char *username,char *password,cchar *fn) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (username && password) {
	    rs = SR_INVALID ;
	    username[0] = '\0' ;
	    password[0] = '\0' ;
	    if (fn[0]) {
		if (suber so(username,password) ; (rs = so.start()) >= 0) {
		    {
			rs = so.procfile(fn) ;
		    } /* end block */
		    rs1 = so.finish() ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (so) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
} /* end subroutine (authfile) */


/* local subroutines */

int suber::start() noex {
	return lb.start ;
} /* end method */

int suber::finish() noex {
	return lb.finish ;
} /* end method */

int suber::procfile(cchar *fn) noex {
	int		rs ;
	int		rs1 ;
	if (bfile af ; (rs = bopen(&af,fn,"r",0666)) >= 0) {
	    while ((rs = breadln(&af,lb.lbuf,lb.llen)) > 0) {
		{
		    rs = procline(rs) ;
		}
	        if ((username[0] != '\0') && (password[0] != '\0')) break ;
	        if (rs < 0) break ;
	    } /* end while (reading lines) */
	    rs1 = bclose(&af) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (bfile) */
	return rs ;
} /* end method (suber::procfile) */

int suber::procline(int len) noex {
	int		rs ;
	int		rs1 ;
	if (field fsb ; (rs = field_start(&fsb,lb.lbuf,len)) >= 0) {
	    int		fl ;
	    cchar	*fp ;
	    while ((rs = field_get(&fsb,ft.terms,&fp)) >= 0) {
		if (rs > 0) {
		    cint	type = mkchar(fp[0]) ;
		    if ((rs = field_get(&fsb,ft.terms,&fp)) > 0) {
			fl = rs ;
	                switch (type) {
	                case 'u':
	                case 'l':
	                    strdcpy1w(username,MAXAUTHLEN,fp,fl) ;
	                    break ;
	                case 'p':
	                    strdcpy1w(password,MAXAUTHLEN,fp,fl) ;
	                    break ;
	                } /* end switch */
		    } /* end if (had a value for the key) */
		} /* end if (positive) */
		if (fsb.term == '#') break ;
		if (rs < 0) break ;
	    } /* end while (parsing fields) */
	    rs1 = field_finish(&fsb) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (field) */
	return rs ;
} /* end method (suber::procline) */


