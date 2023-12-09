/* strnndictcmp */
/* lang=C20 */

/* string compare using dictionary order */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Name:
	strnndictcmp

	Description:
	This subroutine is similar to the 'strcmp(3c)' subroutine
	except that with this subroutine all comparisions are done
	using "dictionary" order.  Dictionary order only compares
	characters that are:
		letters
		digits
	Also, upper and lower case are mostly ignored except that
	upper case still comes before lower case.

	Synopsis:
	int strnndictcmp(cchar *s1,cchar *s2) noex

	Arguments:
	s1	one string
	s2	second string

	Returns:
	>0	the first string is bigger than the second
	0	both strings are equal (as compared)
	<0	first string is less than the second

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<string.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<char.h>
#include	<localmisc.h>

#include	"strnxcmp.h"


/* local defines */

#define	DCH	struct dictchar


/* external subroutines */

extern int	strwcmp(cchar *,cchar *,int) noex ;
extern int	isalnumlatin(int) noex ;
extern int	isdict(int) noex ;


/* external variables */


/* local structures */

struct dictchar {
	const uchar	*sp ;
	int		sl ;
} ;


/* forward references */

static int	dch_start(DCH *,const char *,int) noex ;
static int	dch_finish(DCH *) noex ;
static int	dch_get(DCH *) noex ;

static int	strdocmp(cchar *,cchar *,int) noex ;


/* exported subroutines */

int strnndictcmp(cchar *s1,int s1len,cchar *s2,int s2len) noex {
	DCH		dc1, dc2 ;
	int		rs ;
	int		ch1, ch2 ;
	int		fch1, fch2 ;
	int		rc = 0 ;
	if (s1len < 0) s1len = strlen(s1) ;
	if (s2len < 0) s2len = strlen(s2) ;
	if ((rs = dch_start(&dc1,s1,s1len)) >= 0) {
	    if ((rs = dch_start(&dc2,s2,s2len)) >= 0) {
		int	i ;
	        for (i = 0 ; rc == 0 ; i += 1) {
	            ch1 = dch_get(&dc1) ;
	            ch2 = dch_get(&dc2) ;
	            fch1 = CHAR_TOFC(ch1) ;
	            fch2 = CHAR_TOFC(ch2) ;
	            rc = (fch1 - fch2) ;
	            if ((ch1 == 0) && (ch2 == 0)) /* end-of-string for both */
	                break ;
	        } /* end while */
	        if (rc == 0) {
	            rc = (s1len - s2len) ; /* more determinism */
		}
	        if (rc == 0) {
	            rc = strdocmp(s1,s2,i) ; /* break ties so far */
		}
	        if (rc == 0) {
	            while (s1len && (! isdict(s1[s1len-1]))) s1len -= 1 ;
	            while (s2len && (! isdict(s2[s2len-1]))) s2len -= 1 ;
	            rc = (s1len - s2len) ; /* more determinism */
	        }
	        if (rc == 0) {
	            rc = strncmp(s1,s2,s2len) ; /* absolute determinism (?) */
		}
	        dch_finish(&dc2) ;
	    } /* end if (dc2) */
	    dch_finish(&dc1) ;
	} /* end if (dc1) */
	return rc ;
}
/* end subroutine (strnndictcmp) */


/* local subroutines */

static int dch_start(DCH *dcp,const char *sp,int sl) noex {
	dcp->sp = (const uchar *) sp ;
	dcp->sl = sl ;
	return 0 ;
}
/* end subroutine (dch_start) */

static int dch_finish(DCH *dcp) noex {
	int		sl = dcp->sl ;
	return sl ;
}
/* end subroutine (dch_finish) */

static int dch_get(DCH *dcp) noex {
	int		doch = 0 ;
	forever {
	    doch = 0 ;
	    if (dcp->sl == 0) break ;
	    doch = (dcp->sp[0] & UCHAR_MAX) ;
	    if (doch == 0) break ;
	    dcp->sp += 1 ;
	    dcp->sl -= 1 ;
	    if (isdict(doch))
	        break ;
	} /* end forever */
	return doch ;
}
/* end subroutine (dch_get) */

static int strdocmp(const char *s1,const char *s2,int slen) noex {
	int		do1 ;
	int		do2 ;
	int		rc = 0 ;
	for (int i = 0 ; (i < slen) && *s1 && *s2 ; i += 1) {
	    do1 = 0 ;
	    while (*s1 && ((do1 = CHAR_DICTORDER(*s1)) == 0)) s1 += 1 ;
	    do2 = 0 ;
	    while ((i < slen) && *s2 && ((do2 = CHAR_DICTORDER(*s2)) == 0)) {
	        s2 += 1 ;
	        i += 1 ;
	    }
	    rc = (do1 - do2) ;

	    if (rc != 0)
	        break ;
	    if (*s1) s1 += 1 ;
	    if (*s2) s2 += 1 ;
	} /* end for */
	return rc ;
}
/* end subroutine (strdocmp) */


