/* hasfnamespecial */
/* lang=C20 */

/* test if a given file-name is special */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-24, David A­D­ Morano
	This code was adopted from the SHCAT program, which in turn
	had been adopted from programs with a lineage dating back
	(from the previous notes in this space) from 1989! I deleted
	the long list of notes here to clean this up.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	We test if a given file-name is special.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<limits.h>
#include	<usystem.h>
#include	<localmisc.h>


/* local defines */

#ifndef	STDFNIN
#define	STDFNIN	"*STDIN*"
#endif

#ifndef	STDFNOUT
#define	STDFNOUT	"*STDOUT*"
#endif

#ifndef	STDFNERR
#define	STDFNERR	"*STDERR*"
#endif

#ifndef	STDFNNULL
#define	STDFNNULL	"*STDNULL*"
#endif


/* external subroutines */

extern int	matstr(const char **,const char *,int) ;


/* external variables */


/* global variables */


/* local structures */


/* forward references */


/* local variables */

static const char	*fnames[] = {
	STDFNIN,
	STDFNOUT,
	STDFNERR,
	STDFNNULL,
	NULL
} ;


/* exported subroutines */

int hasfnamespecial(const char *fp,int fl) noex {
	return (matstr(fnames,fp,fl) >= 0) ;
}
/* end subroutine (hasfnamespecial) */


