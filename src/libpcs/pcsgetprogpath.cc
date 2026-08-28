/* pcsgetprogpath SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get the path to a program that is used within the PCS system */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* compile-time debug print-outs */

/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine is originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	pcsgetprogpath

	Description:
	This subroutine is used to find a PCS related program and
	to verify that it is executable.

	Important:
	This subroutine is different from |pcsgetprog(3pcs)| in
	that this will return a full path of the found program
	whenever it is different than what was supplied. In contrast,
	the 'pcsgetprog(3pcs)' subroutine only returns the full
	path of the found program when it is not absolute and it
	is found in the PCS distribution.

	Synopsis:

	int pcsgetprogpath(cc *pcsroot,char *rbuf,cc *name) noex

	Arguments:
	pcsroot		PCS program root path
	rbuf		result buffer pointer (MAXPATHLEN)
	name		program to find

	Returns:
	>0		found the program path and this is the length
	==0		program was found w/o any path prefix
	<0		program was not found

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® */
#include	<sys/param.h>		/* POSIX® */
#include	<sys/stat.h>		/* POSIX® */
#include	<unistd.h>		/* POSIX® */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<vecstr.h>		/* LIBUC */
#include	<ids.h>			/* LIBUC */
#include	<storebuf.h>		/* LIBUC */
#include	<vardefs.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<libpr.h>		/* LIBPR |prgetprogpath(3pr)| */

#include	"pcsgetprogpath.h"

#pragma		GCC dependency		"mod/libutil.ccm"

import libutil ;			/* |lenstr(3u)| */

/* local defines */

#ifndef	CF_DEBUG
#define	CF_DEBUG	0		/* compile-time debug print-outs */
#endif


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported subroutines */

int pcsgetprogpath(cchar *pr,char *rbuf,cchar *nap) noex {
	return prgetprogpath(pr,rbuf,nap,-1) ;
} /* end subroutine (pcsgetprogpath) */


