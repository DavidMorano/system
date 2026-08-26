/* securefile SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* check if a given file name is SUID and owned by our effective UID */
/* version %I% last-modified %G% */


/* revistion history:

	= 1998-09-01, Dave morano
	This was originally made for the PCSPOLL program.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

	Name:
	checksecure

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
	<0		error and also not secure

******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"checksecure.h"

/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */

local bool isxu(cmode fm) noex {
    	return (fm & S_IXUSR) && (fm & S_ISUID) ;
} /* end */
local bool isxg(cmode fm) noex {
    	return (fm & S_IXGRP) && (fm & S_ISGID) ;
} /* end */


/* local variables */


/* exported variables */


/* exported subroutines */

int securefile(cchar *name,uid_t euid,gid_t egid) noex {
	int		rs = SR_FAULT ;
	int		f = false ; /* return-value */
	if (name) ylikely {
	    rs = SR_INVALID ;
	    if (name[0]) ylikely {
	        if (ustat sb ; (rs = u_stat(name,&sb)) >= 0) ylikely {
	            cmode fm = sb.st_mode ;
	            f = f || ((sb.st_uid == euid) && isxu(fm)) ;
	            f = f || ((sb.st_gid == egid) && isxg(fm)) ;
	        } /* end if */
	    } /* end if (valid) */
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
} /* end subroutine (securefile) */


