/* termtime_main (termtime) */
/* lang=C20 */

/* part of TERMCHAR program */
/* version %I% last-modified %G% */

#define	CF_CHARSETS	0		/* set character sets */
#define	CF_ANSILEVEL	0		/* set ANSI conformance level */
#define	CF_DECHEBREW	0		/* use DEC Hebrew */
#define	CF_DECSUP	0		/* use DEC Supplemental */
#define	CF_HEBREW	0		/* use DEC Hebrew */
#define	CF_ANSIREC	1		/* use ANSI recommendation */

/* revision history:

	= 2004-06-24, David A­D­ Morano
	I rewrote this from scratch.  The previous version of this
	program was a hack.

*/

/* Copyright © 2004 David A­D­ Morano.  All rights reserved. */

/**************************************************************************

	Name:
	termtime

	Description:
	This program prints out the time-of-day to the upper right
	corner of the terminal display.

	Synopsis:
	$ termtime [arguments]

*****************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<climits>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstdio>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<strop.h>
#include	<ascii.h>
#include	<exitcodes.h>
#include	<localmisc.h>


/* local defines */

#ifndef	CBUFLEN
#define	CBUFLEN		100
#endif


/* external subroutines */


/* external variable */


/* forward references */


/* local variables */

static cchar	ss2[] = "\033N" ;
static cchar	ss3[] = "\033O" ;

static cchar	ls0[] = "\017" ;		/* also SI */
static cchar	ls1[] = "\016" ;		/* also SO */
static cchar	ls2[] = "\033n" ;
static cchar	ls3[] = "\033o" ;

static cchar	ls1r[] = "\033~" ;
static cchar	ls2r[] = "\033}" ;
static cchar	ls3r[] = "\033|" ;


/* exported subroutines */

int main(int argc,mainv,mainv) {
	FILE		*fp = stdout ;
	int		rs = SR_OK ;
	int		ex = EX_SUCCESS ;


	if ((ex == EX_SUCCESS) && (rs < 0)) ex = EX_DATAERR ;
	return ex ;
} /* end subroutine (main) */


