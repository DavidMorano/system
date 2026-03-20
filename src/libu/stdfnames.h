/* stdfnames HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* standard byte-order serializations */
/* version %I% last-modified %G% */


/* revision history:

	= 2001-03-24, David A­D­ Morano
	This code is newly written but modeled after some code that
	was originally written to get some standardized variable
	serialization for the Ethernet-Controller (ETHCON) development
	work.

*/

/* Copyright © 2001 David A­D­ Morano.  All rights reserved. */

#ifndef	STDFNAMES_INCLUDE
#define	STDFNAMES_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<stdfiles.h>


#ifndef	STDFNIN
#define	STDFNIN		stdfnames[stdfile_in]
#endif

#ifndef	STDFNOUT
#define	STDFNOUT	stdfnames[stdfile_out]
#endif

#ifndef	STDFNERR
#define	STDFNERR	stdfnames[stdfile_err]
#endif

#ifndef	STDFNLOG
#define	STDFNLOG	stdfnames[stdfile_log]
#endif

#ifndef	STDFNNULL
#define	STDFNNULL	stdfnames[stdfile_null]
#endif

#ifndef	STDFNZERO
#define	STDFNZERO	stdfnames[stdfile_zero]
#endif

#ifndef	STDFNMINUX
#define	STDFNMINUS	stdfnames[stdfile_minus]
#endif


extern cchar	*const stdfnames[] ;


#endif /* STDFNAMES_INCLUDE */


