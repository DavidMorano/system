/* securefile SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* check if a given file name is SUID and owned by our effective UID */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* compile-time debugging */

/* revistion history:

	= 1998-09-01, David A­D­ morano
	This was originally made for the PCSPOLL program.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	securefile

	Description:
	This subroutine will check if a given file name is SUID and
	owned by our effective UID.

	Synopsis:
	int securefile(cchar *name,uid_t euid,gid_t egid) noex

	Arguments:
	name		filename
	euid		current EUID
	egid		current EGID

	Returns:
	>0		secure
	==0		not secure
	<0		error and also not secure (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"securefile.h"


/* local defines */


/* local namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local bool isux(mode_t fm) noex {
        return ((fm & S_IXUSR) && (fm & S_ISUID)) ;
} /* end subroutine */
local bool isgx(mode_t fm) noex {
        return ((fm & S_IXGRP) && (fm & S_ISGID)) ;
} /* end subroutine */


/* local variables */


/* exported variables */


/* exported subroutines */

int securefile(cchar *name,uid_t euid,gid_t egid) noex {
	int		rs = SR_FAULT ;
	int		f = false ; /* return-value */
	if (name) {
	    if (ustat sb ; (rs = u_stat(name,&sb)) >= 0) {
	        f = f || ((sb.st_uid == euid) && isux(sb.st_mode)) ;
	        f = f || ((sb.st_gid == egid) && isgx(sb.st_mode)) ;
	    } /* end if */
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (securefile) */


