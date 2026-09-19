/* usysdefs_sigmask HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* these are (a multitude) of various UNIX® system pre-processor defines */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-09, David A-D- Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A-D- Morano.  All rights reserved. */

/*******************************************************************************

	Description:
	This file defines various preprocessor variables for use
	in setting up the UNIX®-system default for various buffer
	sizes.  The real run-time system buffer sizes are (indeed)
	determined at runtime where they may exist.

	Usage:
	Among the most important uses for the data contained herein
	is the BUFSIZE management facility.  It uses the data below
	to set default buffer sizes for those systems that do not
	have dynamicly managed buffer size support in the system
	kernel.

*******************************************************************************/

#ifndef	USYSDEFSSIGMASK_INCLUDE
#define	USYSDEFSSIGMASK_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<signal.h>		/* CSTD |SIG{x}| */


#ifndef	SIG_SET
#define	SIG_SET		SIG_SETMASK
#endif


#endif /* USYSDEFSSIGMASK_INCLUDE */


