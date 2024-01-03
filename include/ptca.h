/* ptca HEADER */
/* lang=C20 */

/* POSIX® Thread (Pthread) Condition-Vvariable Attributes */
/* version %I% last-modified %G% */


/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

#ifndef	PTCA_INCLUDE
#define	PTCA_INCLUDE


#include	<envstandards.h>
#include	<sys/types.h>
#include	<sys/param.h>
#include	<pthread.h>
#include	<usystem.h>
#include	<localmisc.h>


#define	PTCA		pthread_condattr_t

typedef PTCA		ptca ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int	ptca_create(ptca *) noex ;
extern int	ptca_destroy(ptca *) noex ;
extern int	ptca_setpshared(ptca *,int) noex ;
extern int	ptca_getpshared(ptca *,int *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* PTCA_INCLUDE */


