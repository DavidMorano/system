/* stdfiles HEADER */
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

#ifndef	STDFILES_INCLUDE
#define	STDFILES_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */


enum stdfiles {
	stdfile_in,
	stdfile_out,
	stdfile_err,
	stdfile_log,
	stdfile_null,
	stdfile_zero,
	stdfile_minus,
	stdfile_overlast
} ; /* end enum (stdfiles) */

#ifdef	__cplusplus

struct stdfiler {
	cchar	*name[stdfile_overlast + 1] = {} ;
	consteval stdfiler() noex ;
} ; /* end struct (stdfiler) */

extern const stdfiler	stdfile ;

#endif /* __cplusplus */


#endif /* STDFILES_INCLUDE */


