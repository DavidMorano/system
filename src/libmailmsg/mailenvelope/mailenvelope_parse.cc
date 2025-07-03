/* mailenvelope_parse SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* parse out a mail message envelope */
/* version %I% last-modified %G% */


/*revision history:

	= 1998-11-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/******************************************************************************

  	Name:
	mailenvelope

  	Description:
	I do not know!

	Synopsis:
	int mailenvelope_parse(mailenvelope *mep,char *linebuf) noex

	Arguments:
	mep		object pointer
	linebuf		buffer containing data to parse

	Returns:
	>=0		OK
	<0		error (system-return)

******************************************************************************/

#include	<envstandards.h>	/* must be ordered first to configure */
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>
#include	<strings.h>
#include	<usystem.h>
#include	<sfx.h>			/* |sfsub(3uc)| */
#include	<strwcpy.h>
#include	<char.h>
#include	<localmisc.h>

#include	"mailenvelope.h"


/* local defines */

#define	SPACETAB(c)	(((c) == ' ') || ((c) == '\t'))


/* external subroutines */


/* external variables */


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int mailenvelope_parse(MAILENVELOPE *mep,char *linebuf) noex {
	int	i ;
	int	len ;

	cchar	*rp ;
	cchar	*cp, *cp1 ;

	if (linebuf == NULL)
	    return BAD ;

	if (mep != NULL) {

	    mep->f_escape = FALSE ;
	    mep->address[0] = '\0' ;
	    mep->date[0] = '\0' ;
	    mep->remote[0] = '\0' ;

	}

	cp = linebuf ;
	if (*cp == '>') {

	    cp += 1 ;
	    if (mep != NULL) 
		mep->f_escape = TRUE ;

	}

	if ((strncmp(cp,"From",4) != 0) || (! SPACETAB(cp[4])))
	    return BAD ;

	if (mep == NULL) 
		return OK ;

/* search for the address */

	cp = linebuf + 5 ;
	while (CHAR_ISWHITE(*cp)) 
		cp += 1 ;

/* is it a super short envelope? */

	if (*cp == '\0') 
		return OK ;

	cp1 = cp ;
	while (*cp && (! CHAR_ISWHITE(*cp))) 
		cp += 1 ;

	len = MIN((cp - cp1),MAILENVELOPE_ADDRESSLEN) ;
	strwcpy(mep->address,cp1,len) ;

/* search for the start of a date */

	while (CHAR_ISWHITE(*cp)) 
		cp += 1 ;

	if (*cp == '\0') 
		return OK ;

	if ((i = sfsub(cp,-1,"remote from",&rp)) >= 0) {

	    len = MIN(i,MAILENVELOPE_DATELEN) ;
	    strwcpy(mep->date,cp,len) ;

	    cp += (i + 11) ;
	    while (CHAR_ISWHITE(*cp)) 
		cp += 1 ;

	    if (*cp) {

	        cp1 = cp ;
	        while (*cp && (! CHAR_ISWHITE(*cp))) 
			cp += 1 ;

	        len = MIN((cp - cp1),MAILENVELOPE_REMOTELEN) ;
	        strwcpy(mep->remote,cp1,len) ;

	    }

	} else
	    strwcpy(mep->date,cp,MAILENVELOPE_DATELEN) ;

	return OK ;
}
/* end subroutine (mailenvelope_parse) */


