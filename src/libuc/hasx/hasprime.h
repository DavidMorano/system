/* hasprime INCLUDE */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* does a counted c-string some characteristic? */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-10-10, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	has{x}

	Description:
	These subroutines check if a specified c-string has any of
	some characteristic we are looking for.


	Name:
	hsempty

	Description:
        This subroutine determines if a given c-string is empty or not.

	Synopsis:
	bool hasempty(cchar *sp,int sl) noex

	Arguments:
	sp		test c-string pointer
	sl		test c-string length

	Returns:
	1		true (empty)
	0		false (not empty)


	Name:
	haseoh

	Description:
	Determine if the given c-string consists of an End-Of-Header
	(EOH) sequence.  An EOH is a leadering blank line of two
	sorts:
	<NL>
	<CR><NL>

	Synopsis:
	bool haseoh(cchar *sp,int sl) noex

	Arguments:
	sp		test c-string pointer
	sl		test c-string length

	Returns:
	false		assertion fails
	true		assertion succeeds


	Name:
	hasalnum

	Description:
	Are all of the characters Alpha-Num?

	Synopsis:
	bool hasalnum(cchar *sp,int sl) noex

	Arguments:
	sp		test c-string pointer
	sl		test c-string length

	Returns:
	false		c-string does not have all digits
	true		c-string has all digits in it

*******************************************************************************/

#ifndef	HASPRIME_INCLUDE
#define	HASPRIME_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>


EXTERNC_begin

extern bool hasempty(cchar *,int) noex ;
extern bool hasdots(cchar *,int) noex ;
extern bool haseoh(cchar *,int) noex ;
extern bool hasdoublewhite(cchar *,int) noex ;
extern bool hascdpath(cchar *,int) noex ;
extern bool hasmacro(cchar *,int) noex ;
extern bool hasvalidmagic(cchar *,int,cchar *) noex ;

EXTERNC_end


#endif /* HASPRIME_INCLUDE */


