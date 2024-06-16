/* random HEADER */
/* lang=C20 */
 
 
/* revision history:

	= 2000-05-14, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2000 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	This is a total hack.  I tried to find out what is supposed
	to be in here and made up guesses (based on standards printed
	on the web).

*******************************************************************************/

#ifndef _RANDOM_H_
#define _RANDOM_H_	1


#include	<envstandards.h>	/* ordered first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<usyscalls.h>
#include	<localmisc.h>

#define	GRAND_RANDOM	(1<<1)
#define	GRAND_NONBLOCK	(1<<2)

EXTERNC_begin

extern int	getrandom(void *,size_t,unsigned int) noex ;
extern int	getentropy(void *,size_t) noex ;

EXTERNC_end


#endif /* _RANDOM_H_ */


