/* ucfileop HEADER */
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
	uc_chown	
	uc_utime	
	uc_utimes	
	uc_lstat	
	uc_pathconf	
	uc_readlink	
	uc_stat		
	uc_statvfs	

  	Description:
	Additional (or supplemental) support for UNIX® limits.

	Synopsis:
	int uc_chown	(cchar *,uid_t,gid_t) noex
	int uc_utime	(cchar *,CUTIMBUF *) noex
	int uc_utimes	(cchar *,CTIMEVAL *) noex
	int uc_lstat	(cchar *,ustat *) noex
	int uc_pathconf	(cchar *,int,long *) noex
	int uc_readlink	(cchar *,char *,int) noex
	int uc_stat	(cchar *,ustat *) noex
	int uc_statvfs	(cchar *,ustatvfs *) noex

	Arguments:
	sfd		srouce file-descriptor
	dfd		destination file-descriptor
	sfname		source file-name
	dfname		destination file-name
	len		number of byte to copy

*******************************************************************************/

#ifndef	UCFILEOP_INCLUDE
#define	UCFILEOP_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


EXTERNC_begin

extern int	uc_chown	(cchar *,uid_t,gid_t) noex ;
extern int	uc_utime	(cchar *,CUTIMBUF *) noex ;
extern int	uc_utimes	(cchar *,CTIMEVAL *) noex ;
extern int	uc_lstat	(cchar *,ustat *) noex ;
extern int	uc_pathconf	(cchar *,int,long *) noex ;
extern int	uc_readlink	(cchar *,char *,int) noex ;
extern int	uc_stat		(cchar *,ustat *) noex ;
extern int	uc_statvfs	(cchar *,ustatvfs *) noex ;

EXTERNC_end


#endif /* UCFILEOP_INCLUDE */


