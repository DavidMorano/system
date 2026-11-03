/* ucforkat HEADER (LIBUC Terminal-Control) */
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
	ucforkat

	Names:
	ucforkat_init
	ucforkat_fini
	ucforkat_trackbeg
	ucforkat_trackend
	uc_forkatrec
	uc_forkatexp

	Description:
	UNIX® operations realted to |fork(2)|.

  	Synopsis:
	int ucforkat_init() noex
	int ucforkat_fini() noex
	int ucforkat_trackbeg() noex
	int ucforkat_trackend() noex
	int uc_forkatrec(void_f b,void_f p,void_f c) noex
	int uc_forkatexp(void_f b,void_f p,void_f c) noex

	Arguments:
	b		handler becore
	p		handler parent
	c		handler child

	Returns:
	>=0		OK
	<0		error (system-return)

*******************************************************************************/

#ifndef	UCFORKAT_INCLUDE
#define	UCFORKAT_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU |void_f(3u)| */


EXTERNC_begin
extern int ucforkat_init() noex ;
extern int ucforkat_fini() noex ;
extern int ucforkat_trackbeg() noex ;
extern int ucforkat_trackend() noex ;
EXTERNC_end

EXTERNC_begin
extern int uc_forkatrec(void_f b,void_f p,void_f c) noex ;
extern int uc_forkatexp(void_f b,void_f p,void_f c) noex ;
EXTERNC_end

EXTERNC_begin
local inline int uc_forkatrecord(void_f b,void_f p,void_f c) noex {
	return uc_forkatrec(b,p,c) ;
} /* end */
local inline int uc_forkatexpunge(void_f b,void_f p,void_f c) noex {
	return uc_forkatexp(b,p,c) ;
} /* end */
EXTERNC_end


#endif /* UCFORKAT_INCLUDE */


