/* datestr_header SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* create a mail date-str for the RFC922 header */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-01-01, David A­D­ Morano
	This subroutine was written to replace the previous one.
	This version simply calls 'timestr_hdate()'.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************
 
  	Name:
	datestr_header

	Description:
	  Places today's date and time into the string "datestr".  
	  The format is RFC822 standard, for example:
		   27 Nov 1981 13:17 EST
		    1 Mar 1982  7:53 EDT

	This kind of date string is used in the DATE header of the message.

*******************************************************************************/

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<ctime>
#include	<cstddef>		/* |nullptr_t| */
#include	<cstdlib>		/* |getenv(3c)| */
#include	<clanguage.h>
#include	<usysbase.h>
#include	<timestr.h>
#include	<localmisc.h>

#include	"datestr.h"


/* external subroutines */


/* external variables */


/* local (static) variables */


/* exported variables */


/* exported subroutines */

char *datestr_header(time_t date,char *datestr) noex {
	return timestr_hdate(date,datestr) ;
}
/* end subroutine (datestr_header) */


