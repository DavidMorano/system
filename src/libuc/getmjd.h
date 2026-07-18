/* getmjd HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* get (calculate) a Modified-Julian-Day (MJD) */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETMJD_INCLUDE
#define	GETMJD_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


enum mons {
	mon_jan,
	mon_feb,
	mon_mar,
	mon_apr,
	mon_may,
	mon_jun,
	mon_jul,
	mon_aug,
	mon_sep,
	mon_oct,
	mon_nov,
	mon_dec,
	mon_overlast
} ; /* end enum (mons) */

EXTERNC_begin

extern int	getmjd(int,int,int) noex ;
extern int	getyrd(int,int,int) noex ;

EXTERNC_end


#endif /* GETMJD_INCLUDE */


