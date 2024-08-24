/* isfnamespecial SUPPORT */
/* lang=C++20 */

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
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<matstr.h>


/* local defines */

#ifndef	STDFNIN
#define	STDFNIN		"*STDIN*"
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


/* external variables */


/* global variables */


/* local structures */


/* forward references */


/* local variables */

static constexpr cchar	*fnames[] = {
	STDFNIN,
	STDFNOUT,
	STDFNERR,
	STDFNNULL,
	NULL
} ;


/* exported variables */


/* exported subroutines */

int isfnamespecial(cchar *fp,int fl) noex {
	return (matstr(fnames,fp,fl) >= 0) ;
}
/* end subroutine (isfnamespecial) */


