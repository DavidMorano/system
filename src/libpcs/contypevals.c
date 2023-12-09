/* contypevals (for Email) */
/* lang=C20 */

/* content type values */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-01, David A�D� Morano
	The program was written from scratch to do what the previous
	program by the same name did.

*/

/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

/*******************************************************************************

	We store some content-type values.

*******************************************************************************/

#include	<envstandards.h>
#include	<sys/types.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<localmisc.h>


/* exported variables */

cchar	*contypevals[] = {
	"text",
	"plain",
	"binary",
	nullptr
} ;


