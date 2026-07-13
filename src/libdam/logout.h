/* logout HEADER (Log Out) */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */

/* Shared-Memory-Allocation management */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	SHMALLOC_INCLUDE
#define	SHMALLOC_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>		/* POSIX® |pid_t| */
#include	<clanguage.h>		/* LIBU */
#include	<usysbase.h>		/* LIBU */


EXTERNC_begin

extern int logout(pid_t) noex ;

EXTERNC_end


#endif /* SHMALLOC_INCLUDE */


