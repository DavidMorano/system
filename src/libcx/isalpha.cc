/* isalpha SUPPORT */
/* lang=C++20 */

/* charcter classification test */
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

int isalpha(int ch) noex {
	int	mch = (ch | 040) ;
	return ((mch >= 'a') && (mch <= 'z')) ;
}
/* end subroutine (isalpha) */


