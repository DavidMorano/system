/* inter_trans SUPPORT */
/* charset=ISO8859-1 */
/* lang=C++20 */

/* translation adjunct for the INTER object */
/* version %I% last-modified %G% */


/* revision history:

	= 1994-03-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name
	inter_trans

	Description:
	Manage character translation services for the INTER object.

*******************************************************************************/

#ifndef	INTERTRANS_INCLUDE
#define	INTERTRANS_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */

#include	"vmail_cofig.h"


EXTERNC_begin

extern int inter_transbegin(INTER *) noex ;
extern int inter_transend(INTER *) noex ;
extern int inter_transhd(INTER *) noex ;
extern int inter_transproc(INTER *,char *,int,cchar *,int,int) noex ;
 
EXTERNC_end


#endif /* INTERTRANS_INCLUDE */


