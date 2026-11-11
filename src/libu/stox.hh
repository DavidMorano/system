/* stox HEADER (String-To-Integer) */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* C++ |string| to integer conversion */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-16, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Group:
	stox

	Description:
	These subroutines convert the C++ |string| type to integers
	(of various sizes).

*******************************************************************************/

#ifndef	STOX_INCLUDE
#define	STOX_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<string>		/* C++STD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<strtox.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */


sint		stosi	(con std::string&,std::size_t * = nullptr,int = 10) ;
slong		stosl	(con std::string&,std::size_t * = nullptr,int = 10) ;
slonglong	stosll	(con std::string&,std::size_t * = nullptr,int = 10) ;

uint		stoui	(con std::string&,std::size_t * = nullptr,int = 10) ;
ulong		stoul	(con std::string&,std::size_t * = nullptr,int = 10) ;
ulonglong	stoull	(con std::string&,std::size_t * = nullptr,int = 10) ;


#endif /* __cplusplus */
#endif /* STOX_INCLUDE */


