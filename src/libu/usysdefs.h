/* usysdefs HEADER */
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

#ifndef	USYSDEFS_INCLUDE
#define	USYSDEFS_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */

#include	<usysdefs_data.h>	/* miscellaneous system data */
#include	<usysdefs_af.h>		/* address families */
#include	<usysdefs_pf.h>		/* protocol families */
#include	<usysdefs_fcntl.h>
#include	<usysdefs_ioctl.h>
#include	<usysdefs_len.h>
#include	<usysdefs_lim.h>
#include	<usysdefs_lock.h>
#include	<usysdefs_max.h>
#include	<usysdefs_mlockp.h>
#include	<usysdefs_netdb.h>
#include	<usysdefs_ni.h>
#include	<usysdefs_of.h>
#include	<usysdefs_pc.h>
#include	<usysdefs_poll.h>
#include	<usysdefs_rtld.h>
#include	<usysdefs_sig.h>

#include	<memord.hh>
#include	<cmpord.hh>


#endif /* USYSDEFS_INCLUDE */


