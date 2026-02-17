/* basename SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* get the base file name out of a path */
/* version %I% last-modified %G% */


/******************************************************************************

	Name:
	basename

	Description:
	This routine returns the pointer in the given string of the
	start of the basename portion.

	Synopsis:
	char *basename(char *s) noex

	Arguments:
	s		c-string to operate on

	Returns:
	-		pointer to resulting base-name

******************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstring>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<localmisc.h>


/* external subroutines */

extern "C" {
    extern size_t strlen(const char *) noex ;
}


/* exported subroutines */

char *basename(char *s) noex {
	int	si ;
	int	sl = strlen(s) ;
/* remove trailing slash characters */
	while ((sl > 0) && (s[sl - 1] == '/'))  {
		sl -= 1 ;
	}
	s[sl] = '\0' ;
/* find the next previous slash character */
	for (si = sl ; si > 0 ; si -= 1) {
	    if (s[si - 1] == '/') break ;
	}
	return (s + si) ;
}
/* end subroutine (basename) */


