/* dwd_quoted SUPPORT */

/* parse out a quoted field */
/* version %I% last-modified %G% */


/* revision history:

	= David A-D- Morano, November 1991
	This code was originally written.


*/


/*****************************************************************************

	This subroutine is used to parse out a quoted field from
	a buffer, which is passed as an argument.  The resultant
	(parsed out) field is placed into the return buffer provided.

	Arguments:
	- buffer to receive resultant parsed out string
	- length of buffer receiving parsed out string
	- buffer containing the string to be parsed
	- length of string to be parsed

	Returns:
	- length of parsed out string

*****************************************************************************/

#include	<envstandards.h>	/* ordered first to configure */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<utypedefs.h>		/* LIBU */
#include	<utypealiases.h>	/* LIBU */
#include	<usysdefs.h>		/* LIBU */
#include	<usysrets.h>		/* LIBU */
#include	<usyscalls.h>		/* LIBU */
#include	<ascii.h>
#include	<baops.h>
#include	<localmisc.h>		/* LIBU */


/* local variables */

constexpr char		terms[] = {
	0x00, 0x02, 0x00, 0x00,
	0x77, 0x10, 0x00, 0x7C,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
} ; /* end array */


/* exported variables */


/* exported subroutines */

int quoted(char *rbuf,int rlen,cchar *buf,int len) noex {
	int	elen = 0 ;
	int	quote = '\0' ;
	char	*rbp = rbuf ;
	char	*bp = buf ;

/* fast forward to a non-blank type character */

	while ((len > 0) && ((*bp == C_SP) || (*bp == C_TAB))) {

	    bp += 1 ;
	    len -= 1 ;
	}

	if (len == 0) return 0 ;

/* check if the non-white-space type character is a quote or not */

	if ((*bp == '\'') || (*bp == '\"')) {

	    quote = *bp++ ;
	    len -= 1 ;
	}

	if (len == 0) return 0 ;

#ifdef	COMMENT
	rlen -= 1 ;			/* reserve for zero terminator */
#endif

	while ((len > 0) && (elen < rlen)) {

	    if ((quote == '\0') && BATST(terms,(unsigned int) *bp)) 
			return elen ;

	    switch ((int) *bp) {

	    case '"':
	        if (quote == '\"') return elen ;

	        *rbp++ = *bp++ ;
	        elen += 1 ;
	        len -= 1 ;
	        break ;

	    case '\'':
	        if (quote == '\'') return elen ;

	        *rbp++ = *bp++ ;
	        elen += 1 ;
	        len -= 1 ;
	        break ;

	    case '\\':
	        bp += 1 ;
	        len -= 1 ;
	        if (len == 0) return elen ;

	        switch ((int) *bp) {

	        case 'n':
	            *rbp++ = '\n' ;
	            bp += 1 ;
	            break ;

	        case 'r':
	            *rbp++ = '\r' ;
	            bp += 1 ;
	            break ;

	        case 'e':
	            *rbp++ = '\033' ;
	            bp += 1 ;
	            break ;

	        case '"':
	            *rbp++ = '\"' ;
	            bp += 1 ;
	            break ;

	        case '\'':
	            *rbp++ = '\'' ;
	            bp += 1 ;
	            break ;

	        default:
	            *rbp++ = *bp++ ;

	        } /* end switch */

	        elen += 1 ;
	        len -= 1 ;
	        break ;

	    default:
	        *rbp++ = *bp++ ;
	        elen += 1 ;
	        len -= 1 ;

	    } /* end switch */

	} /* end while */

	return elen ;
}
/* end subroutine (quoted) */



