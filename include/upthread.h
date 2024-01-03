/* upthread HEADER */
/* lang=C20 */

/* additional UNIX® limits support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

	= 2017-08-01, David A­D­ Morano
	Updated for lack of interfaces in Apple-Darwin

*/

/* Copyright © 1998,2017 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

	Additional (or supplemental) support for UNIX® limits.

*******************************************************************************/

#ifndef	UPTHREAD_INCLUDE
#define	UPTHREAD_INCLUDE


#include	<envstandards.h>
#include	<sys/types.h>
#include	<signal.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>


/* POSIX threads stuff */

#ifdef	__cplusplus
extern "C" {
#endif

#ifndef	TYPEDEF_VOIDF
#define	TYPEDEF_VOIDF
typedef void (*void_f)() noex ;
#endif


extern int	pt_sigmask(int,sigset_t *,sigset_t *) noex ;
extern int	pt_atfork(void_f,void_f,void_f) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* UPTHREAD_INCLUDE */


