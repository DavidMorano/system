/* dprintf HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* version %I% last-modified %G% */
/* C-language defines */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	DPRINTF_INCLUDE
#define	DPRINTF_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<stdio.h>


#define DPRINTF(FMT, ...) 						\
    if_constexpr (f_debug) {						\
      fprintf(stderr,"%s: ",__func__) ;					\
      fprintf(stderr,FMT __VA_OPT__(,) __VA_ARGS__) ;			\
    }


#endif /* DPRINTF_INCLUDE */


