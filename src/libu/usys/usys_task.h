/* usys_task HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* Apple Darwin operating system support */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-03-21, David A­D­ Morano
	This module was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Name:
	usys_task

	Description:
	This file contains the UNIX® system types that the brain-damaged
	MacOS operating system does NOT have.  We are trying in a very
	small way to make up for some of the immense brain-damage within
	the Apple Darwin operating system.

*******************************************************************************/

#ifndef	USYSTASK_INCLUDE
#define	USYSTASK_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* system-types + |pid_t| */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#if	defined(SYSHAS_TASK) && (SYSHAS_TASK > 0)
/******************************************************************************/


#include	<sys/task.h>


/******************************************************************************/
#else /* defined(SYSHAS_TASK) && (SYSHAS_TASK > 0) */
/******************************************************************************/


#ifndef	TYPEDEF_TASKID
#define	TYPEDEF_TASKID
typedef int	taskid_t ;
#endif

#ifndef	SUBROUTINE_GETTASKID
#define	SUBROUTINE_GETTASKID
EXTERNC_begin
extern taskid_t gettaskid() noex ;
EXTERNC_end
#endif /* SUBROUTINE_GETTASKID */


/******************************************************************************/
#endif /* defined(SYSHAS_TASK) && (SYSHAS_TASK > 0) */


#endif /* USYSTASK_INCLUDE */


