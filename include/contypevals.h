/* contypevals HRADER */
/* encoding=ISO8859-1 */
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
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


enum contypevals {
	contypeval_text,
	contypeval_plain,
	contypeval_binary,
	contypeval_overlast
} ;


extern cpcchar	contypevals[] ;


#endif /* CONTYPEVALS */


