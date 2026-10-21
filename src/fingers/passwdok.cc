/* passwdok SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* check a password for validity */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* compile-time debugging */
#define	CF_FCRYPT	0		/* use |des_fcrypt(3des)| */

/* revision history:

	= 1996-03-01, David A­D­ Morano
	The subroutine was written from scratch to do what the
	previous program by the same name did.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	passwdok

	Description:
	This subroutine will take as input a cleartext passwd and
	an encypted password.  It will return a boolean about whether
	or not the password is valid on the current system.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<des.h>			/* ?? */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD |strcmp(3c)| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"passwdok.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#ifndef	BUFLEN
#define	BUFLEN	MAXPATHLEN
#endif

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* compile-time debugging */
#endif
#ifndef	CF_FCRYPT
#define	CF_FCRYPT	0		/* use |des_fcrypt(3des)| */
#endif


/* external subroutines */


/* external variables */


/* forward references */


/* external variables */


/* local variables */

cbool		f_debug		= CF_DEBUG ;
cbool		f_fcrypt	= CF_FCRYPT ;


/* exported variables */


/* exported subroutines */

int passwdok(cchar *password,cchar *pw_stored) noex {
	int	c1, c2 ;
	cchar	*pw_given ;
	bool	f_passed = false ; /* return-value */
	bool	f_cmp ;

#if	CF_DEBUG
	DEBUGPRINTF("password=>%s<\n",password) ;
	DEBUGPRINTF("pw_stored=>%s<\n",pw_stored) ;
#endif

	c1 = lenstr(password) ;
	c2 = lenstr(pw_stored) ;

#if	CF_FCRYPT
	{
	    char	cryptbuf[40] ;
	    pw_given = des_fcrypt(password,pw_stored,cryptbuf) ;
	}
#else
	pw_given = crypt(password,pw_stored) ;
#endif /* CF_FCRYPT */

#if	CF_DEBUG
	DEBUGPRINTF("pw_given=>%s<\n",pw_given) ;
#endif

	f_cmp = (strcmp(pw_stored,pw_given) == 0) ;

/* if both the supplied and the stored passwords are zero length, allow it */

	if (f_cmp || ((c1 == 0) && (c2 == 0))) {
	    f_passed = true ;
	}

#if	CF_DEBUG
	DEBUGPRINTF("ret f_passed=%u\n",f_passed) ;
#endif

	return f_passed ;
} /* end subroutine (passwdok) */


