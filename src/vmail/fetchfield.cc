/* fetchfield SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* fetch a header field value */
/* version %I% last-modified %G% */

#define	CF_DEBUGS	0		/* compile-time debug print-outs */
#define	CF_DEBUG	0		/* run-time debug print-outs */

/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services
	(RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************
 
  	Name:
	fetchfield

	Description:
	Tihs subtoueine fetches a value from a mail-message header.

	Returns:
	0 if header field was found  (fvalue is the value)
 	1 if message number is too big or too small
	2 if that header is not found in the message

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<usystem.h>
#include	<bfile.h>
#include	<hmatch.h>		/* |hmatch(3pcs)| */
#include	<char.h>
#include	<localmisc.h>

#include	"config.h"
#include	"defs.h"
#include	"mb.h"


/* external subroutines */


/* external varaiables */

extern struct mailbox	mb ;


/* local structures */


/* forward references */


/* local variables */


/* exported variables */


/* exported subroutines */

int fetchfield(mn,f,fvalue,buflen)
int	mn ;
char	f[] ;
char	fvalue[] ;
int	buflen ;
{
	int	i, l, ml, flen = 0 ;
	char	field[LINEBUFLEN + 1], *fp = field ;

#if	CF_DEBUG
	if (g.debuglevel > 1)
	    debugprintf("fetchfield: trying %s\n",f) ;
#endif

	fvalue[0] = '\0' ;
	if ((mn < 0) || (mn >= mb.total)) 
		return 1 ;

	if (curr.fp == nullptr)
		return SR_FAULT ;

/* assume getting from current mailbox which is already set up */

	fseek(curr.fp,messbeg[mn],0) ;

	while (ftell(curr.fp) < messend[mn]) {

	    l = freadline(curr.fp,field,LINEBUFLEN) ;

	    if (field[0] == '\n') 
		break ;

/* check to see if we got a truncated line */

	    if (field[l - 1] == '\n') 
		field[--l] = '\0' ;

	    else 
		field[l] = '\0' ;

	    if (! hmatch(f,field)) 
		continue ;

/* got a match, fast forward to colon and first non-white after that */

	    fp = field ;
	    ml = (l < 78) ? l : 78 ;
	    i = 0 ;
	    while ((i++ < ml) && (*fp != ':')) 
		fp += 1 ;

/* skip this header if there was NO colon character */

	    if (i >= ml) 
		continue ;

/* skip the colon character */

	    fp += 1 ;

/* skip over leading white space */

	    while (CHAR_ISWHITE(*fp)) 
		fp += 1 ;

	    l = strlen(fp) ;

	    ml = MIN(l,(buflen - flen)) ;

	    strncpy(fvalue,fp,ml) ;

	    flen += ml ;
	    fvalue[flen] = '\0' ;

/* OK, get more lines until a blank line or non-white 1st character */

	    while ((ftell(curr.fp) < messend[mn]) && (flen < buflen)) {

	        l = freadline(curr.fp,field,LINEBUFLEN) ;

	        if ((l < 1) || (! CHAR_ISWHITE(field[0]))) 
			break ;

	        if (field[l - 1] == '\n') field[l - 1] = '\0' ;

	        else field[l] = '\0' ;

	        fp = field ;
	        while (CHAR_ISWHITE(*fp)) 
			fp += 1 ;

	        if (*fp != '\0') {

	            fvalue[flen++] = ' ' ;

	            l = strlen(fp) ;

	            ml = MIN(l,(buflen - flen)) ;

	            strncpy(fvalue + flen,fp,ml) ;

	            flen += ml ;
	            fvalue[flen] = '\0' ;

	        }

	    } /* end while (inner) */

#ifdef	COMMENT
	    if (flen >= buflen) return BAD ;
#endif

#if	CF_DEBUG
	    if (g.debuglevel > 1)
	        debugprintf("fetchfield: returning OK\n") ;
#endif

	    return 0 ; /* found */

	} /* end outer while */

	return 2 ; /* not found */
}
/* end subroutine (fetchfield) */


