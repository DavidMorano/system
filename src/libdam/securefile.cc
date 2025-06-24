/* securefile SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* check if a given file name is SUID and owned by our effective UID */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* compile-time debugging */

/* revistion history:

	= 1998-09-01, David A�D� morano
	This was originally made for the PCSPOLL program.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	securefile

	Description:
	This subroutine will check if a given file name is SUID and
	owned by our effective UID.

	Synopsis:
	int securefile(name,euid,egid)
	const char	name[] ;
	uid_t		euid ;
	gid_t		egid ;

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
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* exported variables */


/* exported subroutines */

int securefile(cchar *name,uid_t euid,gid_t egid) noex {
	int		rs = SR_FAULT ;
	int		f = false ;
	if (name) {
	    if (USTAT sb ; (rs = u_stat(name,&sb)) >= 0) {

	    f = f || ((sb.st_uid == euid) && 
		(sb.st_mode & S_IXUSR) && (sb.st_mode & S_ISUID)) ;

	    f = f || ((sb.st_gid == egid) && 
		(sb.st_mode & S_IXGRP) && (sb.st_mode & S_ISGID)) ;

	    } /* end if */
	} /* end if (non-null) */
	return (rs >= 0) ? f : rs ;
}
/* end subroutine (securefile) */


