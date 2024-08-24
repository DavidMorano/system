/* movc SUPPORT */
/* lang=C++20 */

/* subroutine to move a counted character string */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */


/* exported subroutines */

char *movc(int len,const char *src,char *dst) noex {
	for (int i = 0 ; i < len ; i += 1) {
	    *dst++ = *src++ ;
	}
	return dst ;
}
/* end subroutine (movc) */


