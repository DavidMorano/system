/* ucopensysdb SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* open a system-database (SYSBD) file */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-03-24, David A­D­ Morano
	This code is newly written but modeled after some code that
	was originally written to get some standardized variable
	serialization for the Ethernet-Controller (ETHCON) development
	work.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	uc_opensysdb

	Description:
	This subroutine opens a system-database (SYSDB) type of file
	given its enumeration constant value.

	Synopsis:
	int uc_opensysdb(sysdbfiles id,int oflags,int operms) noex

	Arguments:
	id		SYSDB file ID
	oflags		open-flags
	operms		open-perms

	Returns:
	>=0		returned file-descriptor
	<0		error (system-return)

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<usystem.h>
#include	<syswords.hh>		/* |sysword(3u)| */
#include	<sysdbfiles.h>		/* |sysdbfile(uc)| */
#include	<libmallocxx.h>
#include	<mknpathx.h>
#include	<localmisc.h>


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */

static cchar	*prefix = sysword.w_sysdbdir ;


/* exported variables */


/* exported subroutines */

int uc_opensysdb(sysdbfiles id,int oflags,mode_t operms) noex {
	int		rs = SR_INVALID ;
	int		rs1 ;
	int		fd = -1 ;
	if ((id >= 0) && (id < sysdbfile_overlast)) {
	    if (char *nbuf ; (rs = libmalloc_mn(&nbuf)) >= 0) {
		cint	nlen = rs ;
		cchar	*sysdbname = sysdbfile[id] ;
		if ((rs = mknpath(nbuf,nlen,prefix,sysdbname)) >= 0) {
		    rs = uc_open(nbuf,oflags,operms) ;
		    fd = rs ;
		}
		rs1 = lm_free(nbuf) ;
		if (rs >= 0) rs = rs1 ;
		{
		    if ((rs < 0) && (fd >= 0)) uc_close(fd) ;
		}
	    } /* end if (m-a-f) */
	} /* end if (valid) */
	return (rs >= 0) ? fd : rs ;
}
/* end subroutine (uc_opensysdb) */


