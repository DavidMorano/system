/* readln HEADER */
/* lang=C++20 */

/* convert a single character to a given case */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-10, David A�D� Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright (c) 1998 David A�D� Morano.  All rights reserved. */

#ifndef	READLN_INCLUDE
#define	READLN_INCLUDE
#ifdef	__cplusplus


#include	<envstandards.h>	/* MUST be first to configure */
#include	<istream>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<localmisc.h>


extern int readln(std::istream &,char *,int,int = eol) noex ;


#endif /* __cplusplus */
#endif /* READLN_INCLUDE */


