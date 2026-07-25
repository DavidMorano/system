/* ucatfork HEADER (LIBUC Terminal-Control) */
/* charset=ISO8859-1 */
/* lang=C20 */

/* LIBUC Teerminal-Control functions */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	ucatfork

	Names:
	uc_atforkrec
	uc_atforkexp
	ucatfork_init
	ucatfork_fini
	ucatfork_trackbegin

	Description:
	UNIX® operations realted to |fork(2)|.

  	Synopsis:
	int uc_atforkrec(void_f b,void_f p,void_f c) noex
	int uc_atforkexp(void_f b,void_f p,void_f c) noex
	int ucatfork_init() noex
	int ucatfork_fini() noex
	int ucatfork_trackbegin() noex

	Arguments:
	b		handler becore
	p		handler parent
	c		handler child

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#ifndef	UCATFORK_INCLUDE
#define	UCATFORK_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU |void_f(3u)| */


EXTERNC_begin

extern int uc_atforkrec(void_f b,void_f p,void_f c) noex ;
extern int uc_atforkexp(void_f b,void_f p,void_f c) noex ;
extern int ucatfork_init() noex ;
extern int ucatfork_fini() noex ;
extern int ucatfork_trackbegin() noex ;

local inline int uc_atforkrecord(void_f b,void_f p,void_f c) noex {
	return uc_atforkrec(b,p,c) ;
}
local inline int uc_atforkexpunge(void_f b,void_f p,void_f c) noex {
	return uc_atforkexp(b,p,c) ;
}

EXTERNC_end


#endif /* UCATFORK_INCLUDE */


