/* search SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* search for something */
/* version %I% last-modified %G% */

#define	CF_DEBUG	0		/* run-time debug print-outs */

/* revision history:

	= 1998-11-01, David A­D­ Morano
	This subroutine was written for Rightcore Network Services (RNS).

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Description:
	This function executes 'function(mn)' on each mn in the
	current mailbox that matches.  A message matches if the
	logical expression set up by "leparse" evaluates to true.
	Each term in the expression consists of HEADER:value  where
	value matches if it is equal to any word within the
	corresponding  .

	Thus "SUBJECT:rdmail" as an expression matches:
		"SUBJECT: program rdmail now available" .

	The expression is contained in the global variables:
		isop, etoken, hvalpt     
	which are set up by |leparse()|.

	Looping is done on external messages since that is what
	|func()| expects.  However, evaluation and matching must
	be done on internal.  Returns 0 if can do matching, 1 if
	expression improperly formed.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<sys/stat.h>
#include	<unistd.h>
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<cstring>		/* CSTD */
#include	<cstdio>		/* CSTD */
#include	<cstddef>		/* CSTD */
#include	<cstdlib>		/* CSTD */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */
#include	<localmisc.h>		/* LIBU */
#include	<bfile.h>		/* LIBB */

#include	"vmail_config.h"
#include	"vmail_defs.h"
#include	"mb.h"


/* local defines */

#ifndef	PI
#define	PI	proginfo
#endif


/* forward references */

local int	wmatch() noex ;
local void	tokeval(PI *,int,char *) noex ;


/* exported variables */


/* exported subroutines */

int search(pip,dsp,mbp,func)
PI	*pip ;
DS		*dsp ;
struct mailbox	*mbp ;
int		(*func)() ;
{
	int	mn ;

	char	boolexp[LINEBUFLEN] ;


/* initial processing */

	tokeval(pip,messord[1],boolexp) ;

	switch (booleval(dsp,boolexp)) {

	case 1:   
	    (*func)(1) ;

	case 0:   
	    break ;

	case -1:  
	    return 1 ;		/* invalid expression */

	} /* end switch */

/* find rest of matches and call func */

	for (mn = 2 ; mn <= mbp->total ; mn += 1) {

	    tokeval(pip,messord[mn],boolexp) ;

	    if (booleval(dsp,boolexp)) 
		(*func)(mn) ;

	} /* end for */

	return 0 ;
} /* end subroutine (search) */


/* local subroutines */

/***************************************************************************

  	Description:
	generates boolean expression of logical expression for this
	mn.  ie evaluates matching of the specified field values
	into 0s and 1s.  logical expression is in global variables.
	boolexp returned as character string in boolexp.  mn is
	internal mn.

****************************************************************************/

local void tokeval(PI *pip,int mn,char *boolexp) noex {
	int	b = 0 ;
	int	letok ;
	char	mhval[LINEBUFLEN] ;

	/* just copy through operators */
	for (letok=0; letok < numletok; letok += 1) {
	    if (isop[letok] == 1) {
	        boolexp[b++] = operator[etoken[letok]] ;
	        continue ;		/*next token*/
	    }
	    /* have HEADER:value token so get value */
	    if (fetchfield(mn,header[etoken[letok]],mhval,LINEBUFLEN) != 0) {
		/* try special check for non-sendmail From */
	        if (etoken[letok] == 0) {
	            fetchfrom(pip,mn,mhval,LINEBUFLEN) ;
		} else {
		    /* message missing so eval to '0' */
	            boolexp[b++] = '0' ;
	            continue ;	/*next token*/
	        }
	    } /* end if */
	    /* does message value match logexpr  value? */
	    if (wmatch (hvalpt[letok],mhval)) {
	        boolexp[b++] = '1' ;
	    } else {
		boolexp[b++] = '0' ;
	    }
	} /* next token */
	boolexp[b] = eos ;
} /* end subroutine (tokeval) */

/* returns true iff word occurs within string */
local int wmatch(char *word,char *strval) noex {
	char text[LINEBUFLEN], *tp ;

	strcpy(text,strval) ;
	/* note -' not delimeters */
	tp = strtok (text," .,:;\"()/?!~*_&[]@");  
	while (tp != nullptr) {
	    if (strcasecmp(word,tp)) {
	        return(1) ;
	    } else {
		tp = strtok(0," .,:;\"()/?!~*_&[]@") ;
	    }
	} /* end while */
	/* special cases exclude delimeters:    .!@ (FROM),  /: (DATE) */

	strcpy (text,strval) ;
	tp = strtok (text," ,;\"()?~*_&[]");  /*note  -'  not delimeters*/
	while (tp != nullptr) {
	    if (strcasecmp(word,tp)) {
	        return(1) ;
	    } else {
		tp = strtok (0," ,;\"()?~*_&[]") ;
	    }
	}
	/* no match */
	return 0 ;
} /* end subroutine (wmatch) */


