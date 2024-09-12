/* fork1 SUPPORT */
/* lang=C++20 */

/* a |fork1(2)|if you do not already have it! */
/* version %I% last-modified %G% */


/* revision history:

	= 1989-03-01, David A.D. Morano
	This code was originally written.

*/

/* Copyright © 1989 David A­D­ Morano.  All rights reserved. */

#include	<sys/types.h>
#include	<unistd.h>
#include	<utypedefs.h>
#include	<clanguage.h>


/* exported subroutines */

pid_t fork1() noex {
	return fork() ;
}


