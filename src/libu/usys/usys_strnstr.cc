/* strnstr SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* find a substring within a larger string w/ a maximum byte count */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-23, David A­D­ Morano
	This code was originally written.

	= 2002-11-12, David A­D­ Morano
	No change to the code below, but a new note in the comments
	(below).  Hint: FreeBSD 4.5 has put this very subroutine
	into their standard C-language library (in 2002, this year).

*/

/* Copyright © 1998,2002 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strnstr

	Description:
	This subroutine determines if the parameter string (argument
	'ss') is or is not a substring specified by the first
	argument.  This subroutine either returns a pointer to the
	the begining of the found substring or NULL if not found.
	If the substring is of zero-length, then a pointer to the
	source test string is returned (as it there was a match).

	Synopsis:
	char *strnstr(cchar *sp,cchar *ss,size_t n) noex

	Arguments:
	sp		test c-string pointer
	ss		null terminated substring to search for
	n		maximum number of characters to conider

	Returns:
	-		pointer to found substring
	nullptr		substring was not found
	-		pointer to 'sp' if 'ss' is zero length

	Notes:
	1.  The "FreeBSD 4.5" finally included this subroutine in
	there stadnard C-language library.  My version is identical
	to theirs.  So not change was needed.  It (this subroutine) is
	sort of based on the standard (C-lagnauage library) subroutine
	|strstr(3c)| so it was not hard to guess what this subroutine
	would do when someone else created one.
	2. Note that very few systems have this subroutine now and
	might not have it for years or decades (as things so), so
	in some places my own (other subroutine |strsub(3u)| is
	usually used instead of this.
	3. Not really strictly related to this subroutine, but my
	own family of subroutines, like |strsub(3u)|, which pretty
	much do what this and similar subroutines do (but w/ a
	different API), have been used since the early 1980s in my
	work -- and they were originally written in assembly language
	(M68k).

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<localmisc.h>		/* LIBU */

#include	"usys_strnstr.h"


/* local defines */


/* imported namespaces */


/* local typedefs */


/* external subroutines */

extern "C" {
    extern size_t strlen	(ccharp) noex ;
    extern size_t strnlen	(ccharp,size_t) noex ;
} /* end extern (C) */

extern "C" {
    extern int	strncmp(ccharp,ccharp,size_t) noex ;
} /* end extern (C) */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

#ifndef	SYSHAS_STRNSTR
/******************************************************************************/

/* ** not needed ** */

/******************************************************************************/
#else /* SYSHAS_STRNSTR */
/******************************************************************************/

char *strnstr(cchar *sp,cchar *ss,size_t n) noex {
    	char		*rp = nullptr ;
	if (sp && ss) ylikely {
	    rp = cast_const<charp>(sp) ;
	    if (csize sslen = strlen(ss) ; sslen > 0) {
	        if (csize sl = strnlen(sp,n) ; sl >= sslen) {
		    {
		        char *ep = rp + (sl - sslen) ;
		        bool f = false ;
		        for ( ; rp < ep ; rp += 1) {
			    f = (strncmp(rp,ss,sslen) == 0) ;
			    if (f) break ;
		        } /* end for */
		        if (! f) rp = nullptr ;
		    } /* end block */
	        } else {
	            rp = nullptr ;
	        } /* end */
	    } /* end if (positive) */
	} /* end if (non-null) */
	return rp ;
} /* end subroutine (strnstr) */

/******************************************************************************/
#endif /* USTSSYSHAS_STRNSTR */


