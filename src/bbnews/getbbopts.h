/* getbbopts HEADER */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* the user bbinterface (command bbinterpreter) for VMAIL */
/* version %I% last-modified %G% */


/* revision history:

	= 2009-01-20, David A­D­ Morano
	This is a complete rewrite of the trash that performed this
	function previously.

*/

/* Copyright © 2009 David A­D­ Morano.  All rights reserved. */

#ifndef	GETBBOPTS_INCLUDE
#define	GETBBOPTS_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<usysbase.h>


EXTERNC_begin

extern int	getbbopts(proginfo *,vecstr *) noex ;

EXTERNC_end


#endif /* GETBBOPTS_INCLUDE */



