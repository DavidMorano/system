/* gettid HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* get a cannical host-name */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	GETTID_INCLUDE
#define	GETTID_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


EXTERNC_begin

extern pthread_t gettid() noex ;


EXTERNC_end


#endif /* GETTID_INCLUDE */


