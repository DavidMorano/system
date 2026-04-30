/* tsystypenams SUPPORT (System-Type-Nnames) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* get the system 'type-number' and 'osnum' for a given system and release */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-04-11, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	getsystypenames

*******************************************************************************/

#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<stddef.h>		/* |nullptr_t| */
#include	<stdlib.h>
#include	<string.h>		/* |strchr(3c)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<localmisc.h>

#include	"systypenames.h"


/* local defines */


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* exported variables */

cpcchar		systypename[] = {
       	[systypename_darwin]	= "Darwin",
	[systypename_linux]	= "Linux",
	[systypename_sysv]	= "SysV",
	[systypename_overlast]	= nullptr
} ; /* end array (systypename) */


