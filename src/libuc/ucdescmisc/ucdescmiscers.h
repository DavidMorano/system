/* ucdescmiscers HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* additional UNIX® support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

	= 2017-08-01, David A­D­ Morano
	Updated for lack of interfaces in Apple-Darwin.

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Names:
	uc_seeko
	uc_fcntl

  	Description:
	Additional (or supplemental) support for UNIX® limits.

*******************************************************************************/

#ifndef	UCDESCMISCERS_INCLUDE
#define	UCDESCMISCERS_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<unistd.h>		/* POSIX® types? */
#include	<fcntl.h>		/* POSIX® types? */
#include	<stddef.h>		/* CSTD */
#include	<stdlib.h>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */

#include	<uccloseonexec.h>
#include	<ucfminmod.h>
#include	<ucfstat.h>
#include	<ucftruncate.h>
#include	<ucnonblock.h>

EXTERNC_begin

extern int	uc_fcntl	(int,int,...)			noex ;
extern int	uc_ioctl	(int,int,...)			noex ;
extern int	uc_seeko	(int,off_t,int,off_t *)		noex ;
extern int	uc_tell		(int,off_t *)			noex ;
extern int	uc_fchown	(int,uid_t,gid_t)		noex ;
extern int	uc_fchmod	(int,mode_t)			noex ;
extern int	uc_fpathconf	(int,int,long *)		noex ;
local inline int	uc_seek		(int fd,off_t wo,int wh) noex {
    		return uc_seeko(fd,wo,wh,nullptr) ;
} /* end subroutine */
local inline int	uc_rewind	(int fd)		noex {
    		return uc_seek(fd,0z,SEEK_SET) ;
} /* end subroutine */

EXTERNC_end


#endif /* UCDESCMISCERS_INCLUDE */


