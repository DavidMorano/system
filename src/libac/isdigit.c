/* isdigit SUPPORT */
/* lang=C++20 */

/* character classification test */
/* version %I% last-modified %G% */


/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

#include	<sys/types.h>
#include	<utypedefs.h>
#include	<clanguage.h>


/* exported subroutines */

int isdigit(int ch) noex {
	return ((ch >= '0') && (ch <= '9')) ;
}
/* end subroutine (isdigit) */


