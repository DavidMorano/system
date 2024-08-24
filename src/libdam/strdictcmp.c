/* strdictcmp SUPPOER */
/* lang=C++20 */

/* string compare using dictionary order */
/* version %I% last-modified %G% */

#define	CF_CHAR		1		/* use 'CHAR_XX(3dam)' */

/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strdictcmp

	Description:
	This subroutine is similar to the |strcmp(3c)| subroutine
	except that with this subroutine all comparisions are done
	using "dictionary" order.  Dictionary order only compares
	characters that are:

		letters
		digits
	
	Also, upper and lower case mostly ignored except that upper
	case still comes before lower case.

	Synopsis:
	int strdictcmp(cchar *s1,cchar *s2) noex

	Arguments:
	s1	one string
	s2	second string

	Returns:
	>0	the first string is bigger than the second
	0	both strings are equal (as compared)
	<0	first string is less than the second

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstring>
#include	<char.h>
#include	<localmisc.h>


/* external subroutines */

extern "C" {
    extern int	strnndictcmp(cchar *,int,cchar *,int) noex ;
    extern int	dictdiff(int,int) noex ;
    extern int	isdict(int) noex ;
}


/* external variables */

#if	CF_CHAR
#define	tolc(c)		CHAR_TOLC(c)
#define	touc(c)		CHAR_TOUC(c)
#else /* CF_CHAR */
extern int	tolc(int) ;
extern int	touc(int) ;
#endif /* CF_CHAR */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int strdictcmp(cchar *s1,cchar *s2) noex {
	return strnndictcmp(s1,-1,s2,-1) ;
}
/* end subroutine (strdictcmp) */


