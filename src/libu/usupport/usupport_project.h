/* usupport_project HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* UNIX® kernel support subroutines */
/* version %I% last-modified %G% */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	USUPPORTPROJECT_INCLUDE
#define	USUPPORTPROJECT_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<time.h>		/* |time_t| */
#include	<string.h>		/* |memset(3c)| + |memcpy(3c)| */
#include	<stdint.h>
#include	<clanguage.h>
#include	<usysbase.h>
#include	<usys.h>		/* <- auxillary OS support */
#include	<utimeout.h>
#include	<ulogerror.h>
#include	<usysutility.hh>
#include	<timecount.hh>
#include	<filetypes.h>
#include	<aflag.hh>
#include	<stdintx.h>
#include	<intx.h>


#if	defined(SYSHAS_PROJECT) && (SYSHAS_PROJECT > 0)
#else /* defined(SYSHAS_PROJECT) && (SYSHAS_PROJECT > 0) */

#ifndef	TYPEDEF_PROJID
#define	TYPEDEF_PROJID
typedef int	projid_t ;
#endif

#ifndef	SUBROUTINE_GETPROJID
#define	SUBROUTINE_GETPROJID
EXTERNC_begin
extern projid_t getprojid() noex ;
EXTERNC_end
#endif /* SUBROUTINE_GETPROJID */

#endif /* defined(SYSHAS_PROJECT) && (SYSHAS_PROJECT > 0) */


#endif /* USUPPORTPROJECT_INCLUDE */


