/* usys_project HEADER */
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
	usys_project

	Description:
	This file contains the UNIX® system types that the brain-damaged
	MacOS operating system does NOT have.  We are trying in a very
	small way to make up for some of the immense brain-damage within
	the Apple Darwin operating system.

*******************************************************************************/

#ifndef	USYSPROJECT_INCLUDE
#define	USYSPROJECT_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>		/* system-types */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>


#ifndef	PROJECT
#define	PROJECT		struct project
#endif
#ifndef	CPROJECT
#define	CPROJECT	const struct project
#endif


#if	defined(SYSHAS_PROJECT) && (SYSHAS_PROJECT > 0)
/******************************************************************************/


#include	<project.h>


/******************************************************************************/
#else /* defined(SYSHAS_PROJECT) && (SYSHAS_PROJECT > 0) */
/******************************************************************************/


#ifndef	TYPEDEF_PROJID
#define	TYPEDEF_PROJID
typedef int	projid_t ;
#endif

#ifndef	SUBROUTINE_GETPROJID
#define	SUBROUTINE_GETPROJID
EXTERNC_begin
extern projid_t getprojid() noex ;
extern projid_t getprojidbyname(cchar *) noex ;
EXTERNC_end
#endif /* SUBROUTINE_GETPROJID */

#ifndef	SUBROUTINE_INPROJ
#define	SUBROUTINE_INPROJ
EXTERNC_begin
extern int inproj(cchar *,cchar *,void *,size_t) noex ;
EXTERNC_end
#endif /* SUBROUTINE_INPROJ */

#ifndef STRUCT_PROJECT
#define STRUCT_PROJECT
struct project {
        char            *pj_name ;
        char            *pj_comment ;
        char            *pj_attr ;
        char            **pj_users ;
        char            **pj_groups ;
        projid_t        pj_projid ;
} ; /* end struct (project) */
#endif /* STRUCT_PROJECT */

#ifndef	SUBROUTINE_GETPJ
#define	SUBROUTINE_GETPJ
/* Solaris® implementation */
EXTERNC_begin
extern void setprojent(void) noex ;
extern void endprojent(void) noex ;
extern CPROJECT *getprojent(PROJECT *,void *,size_t) noex ;
extern CPROJECT *getprojbyname(cchar *,PROJECT *,void *,size_t) noex ;
extern CPROJECT *getprojbyid(projid_t,PROJECT *,void *,size_t) noex ;
extern CPROJECT *getdefaultproj(cchar *,PROJECT *,void *,size_t) noex ;
extern int inproj(cchar *,cchar *,void *,size_t) noex ;
extern projid_t getprojidbyname(cchar *) noex ;
EXTERNC_end
#endif	/* SUBROUTINE_GETPJ */


/******************************************************************************/
#endif /* defined(SYSHAS_PROJECT) && (SYSHAS_PROJECT > 0) */


#endif /* USYSPROJECT_INCLUDE */


