/* cvttemperature HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* temperature conversion subroutines */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	CVTTEMPERATURE_INCLUDE
#define	CVTTEMPERATURE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


EXTERNC_begin

extern double	cvtfahtocen(double) noex ;
extern double	cvtcentofah(double) noex ;

EXTERNC_end


#endif /* CVTTEMPERATURE_INCLUDE */


