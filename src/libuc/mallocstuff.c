/* mallocstuff SUPPORT */
/* lang=C20 */

/* miscellaneous |malloc(3c)| related */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-06-11, David A­D­ Morano
	These subroutines were was originally written (inspired by others).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*****************************************************************************

        These subroutines allocate a fixed amount of memory and then copy the
        user supplied thing into it.


****************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<stdlib.h>
#include	<string.h>
#include	<usystem.h>
#include	<strwcp.h>
#include	<localmisc.h>


/* local defines */


/* external subroutines */


/* forward references */

char	*mallocbuf(void *,int) noex ;
char	*mallocstr(const char *) noex ;
char	*mallocstrw(const char *,int) noex ;
char	*malloctest(int) noex ;


/* exported subroutines */

char *mallocbuf(void *b,int blen) noex {
	char		*rp = NULL ;
	if (b == NULL) return NULL ;
	if (blen < 0) return NULL ;
	if (uc_malloc((blen + 1),&rp) < 0) rp = NULL ;
	if (rp != NULL) memcpy(rp,b,blen) ;
	return rp ;
}
/* end subroutine (mallocbuf) */

char *mallocstr(cosnt char *s) noex {
	int		slen ;
	char		*rp = NULL ;
	if (s == NULL) return NULL ;
	slen = strlen(s) ;
	if (uc_malloc((slen+1),&rp) < 0) rp = NULL ;
	if (rp != NULL) strcpy(rp,s) ;
	return rp ;
}
/* end subroutine (mallocstr) */

char *mallocstrw(char *sp,int sl) noex {
	char		*rp = NULL ;
	bool		f = FALSE ;
	if (sp == NULL) return NULL ;
	if (sl < 0) {
	    f = TRUE ;
	    sl = strlen(sp) ;
	}
	if (uc_malloc((sl + 1),&rp) < 0) rp = NULL ;
	if (rp != NULL) {
	    if (f) {
	        memcpy(rp,sp,sl) ;
		sp[sl] = '\0' ;
	    } else {
	        strwcpy(rp,sp,sl) ;
	    }
	} /* end if */
	return rp ;
}
/* end subroutine (mallocstrw) */

char *mallocint(v) noex {
	int		len = sizeof(int) ;
	char		*rp = NULL ;
	if (uc_malloc((len+1),&rp) < 0) rp = NULL ;
	if (rp) {
	    memcpy(rp,(char *) &v,sizeof(int)) ;
	}
	return rp ;
}
/* end subroutine (mallocint) */


