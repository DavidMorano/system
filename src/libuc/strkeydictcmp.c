/* strkeydictcmp SUPPORT */
/* lang=C++20 */

/* string key comparison */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-09-10, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strkeydictcmp

	Description:
	This subroutine makes a comparison of the key of a string
	that looks like a SHELL variable assignment.  For example,
	consider the following compound string:
		A=the_dog_house
	The 'A' would be the key, and the part 'the_dog_house' is
	the value.

	Synopsis:
	int strkeydictcmp(cchar *s1,cchar *s2) noex

	Arguments:
	s1		first string
	s2		second string

	Returns:
	>0		the second key is greater than the first
	0		the keys of the strings are equal
	<0		the first key is less than the second

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<cstring>
#include	<ischarx.h>
#include	<localmisc.h>

#include	"srtkeycmp.h"


/* local defines */


/* external subroutines */

extern "C" {
    extern int	dictdiff(int,int) noex ;
}


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int strkeydictcmp(cchar *s1,cchar *s2) noex {
	int		rc = 0 ;
	if (s1 || s2) {
	    rc = 1 ;
	    if (s1) {
		rc = -1 ;
		if (s2) {
		    rc = 0 ;
                    while (*s1 && *s2) {
                        if ((*s1 == '=') || (*s2 == '=')) break ;
                        if (! isdict(*s1)) {
                            s1 += 1 ;
                            continue ;
                        }
                        if (! isdict(*s2)) {
                            s2 += 1 ;
                            continue ;
                        }
                        rc = dictdiff(*s1,*s2) ;
                        if (rc != 0) break ;
                        s1 += 1 ;
                        s2 += 1 ;
                    } /* end while */
                    if (rc == *s2) rc = 0 ;
                    if (*s1 == '=') rc = (*s2 == '\0') ? 0 : -1 ;
                    if (*s2 == '=') rc = (*s1 == '\0') ? 0 : 1 ;
		}
	    }
	}
	return rc ;
}
/* end subroutine (strkeydictcmp) */


