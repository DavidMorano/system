/* authfile SUPPORT */
/* lang=C++20 */

/* subroutine to read an authorization file */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-12-12, David A­D­ Morano
	This subroutine was originally written.

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
#include	<sys/types.h>
#include	<sys/param.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<linebuffer.h>
#include	<bfile.h>
#include	<field.h>
#include	<strdcpyx.h>
#include	<mkchar.h>
#include	<localmisc.h>		/* |LINEBUFLEN| */

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
	cchar		*fname ;
	linebuffer	lb ;
	suber(char *up,char *pp,cchar *fn) noex : username(up), password(pp) {
	    fname = fn ;
	} ;
	int start() noex ;
	int finish() noex ;
	int procfile(cchar *) noex ;
	int procline(int) noex ;
    } ; /* end struct (suber) */
}


/* forward references */


/* local variables */

constexpr cchar		fterms[32] = {
	0x00, 0x02, 0x00, 0x00,
	0x09, 0x00, 0x00, 0x20,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
} ;


/* exported variables */


/* exported subroutines */

int authfile(char *username,char *password,cchar *fname) noex {
	int		rs = SR_FAULT ;
	int		rs1 ;
	if (fname && username && password) {
	    rs = SR_INVALID ;
	    username[0] = '\0' ;
	    password[0] = '\0' ;
	    if (fname[0]) {
		suber	so(username,password,fname) ;
		if ((rs = so.start()) >= 0) {
		    {
			rs = so.procfile(fname) ;
		    } /* end block */
		    rs1 = so.finish() ;
		    if (rs >= 0) rs = rs1 ;
		} /* end if (so) */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return rs ;
}
/* end subroutine (authfile) */


/* local subroutines */

int suber::start() noex {
	return lb.start ;
}

int suber::finish() noex {
	return lb.finish ;
}

int suber::procfile(cchar *fname) noex {
	int		rs ;
	int		rs1 ;
	bfile		afile, *afp = &afile ;
	if ((rs = bopen(afp,fname,"r",0666)) >= 0) {
	    while ((rs = breadln(afp,lb.lbuf,lb.llen)) > 0) {
		{
		    rs = procline(rs) ;
		}
	        if ((username[0] != '\0') && (password[0] != '\0')) break ;
	        if (rs < 0) break ;
	    } /* end while (reading lines) */
	    rs1 = bclose(afp) ;
	    if (rs >= 0) rs = rs1 ;
	} /* end if (bfile) */
	return rs ;
}
/* end method (suber::procfile) */

int suber::procline(int len) noex {
	field		fsb ;
	int		rs ;
	int		rs1 ;
	if ((rs = field_start(&fsb,lb.lbuf,len)) >= 0) {
	    int		fl ;
	    cchar	*fp ;
	    while ((rs = field_get(&fsb,fterms,&fp)) >= 0) {
		if (rs > 0) {
		    cint	type = mkchar(fp[0]) ;
		    if ((rs = field_get(&fsb,fterms,&fp)) > 0) {
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
}
/* end method (suber::procline) */


