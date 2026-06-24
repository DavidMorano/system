/* contypevals HRADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* email content-type values */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-02-03, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CONTYPEVALS
#define	CONTYPEVALS


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysrets.h>		/* LIBU */


enum contypevals {
	contypeval_text,
	contypeval_plain,
	contypeval_binary,
	contypeval_overlast
} ; /* end enum (contypevals) */


extern cpcchar	contypenames[] ;


#endif /* CONTYPEVALS */


