/* bminmod SUPPORT */
/* lang=C++20 */

/* "Basic I-O" package similiar to some other thing whose initials is "stdio" */
/* version %I% last-modified %G% */

#define	CF_MEMCPY	1		/* use 'memcpy(3c)' */

/* revision history:

	= 1998-07-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Sset a minimum mode on the opened file.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<unistd.h>
#include	<fcntl.h>
#include	<cstring>
#include	<usystem.h>
#include	<localmisc.h>

#include	"bfile.h"


/* local defines */


/* external subroutines */


/* external variables */


/* forward references */


/* local variables */


/* exported subroutines */

int bminmod(bfile *fp,mode_t om) noex {
	int		rs = SR_OK ;

	if (fp == NULL) return SR_FAULT ;

	if (fp->magic != BFILE_MAGIC) return SR_NOTOPEN ;

	if (fp->fd >= 0) {
	    rs = uc_fminmod(fp->fd,om) ;
	}

	return rs ;
}
/* end routine (bminmod) */


