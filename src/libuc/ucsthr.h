/* ucsthr HEADER */
/* lang=C20 */

/* interface component for UNIX® library-3c */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-13, David A-D- Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	UCSTHR_INCLUDE
#define	UCSTHR_INCLUDE


#include	<envstandards.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usupport.h>


#ifdef	__cplusplus
extern "C" {
#endif

int uc_sthrcreate(caddr_t,size_t,int (*)(void *),cvoid *,long) noex ;
int uc_sthrexit(intex) noex ;
int uc_sthrjoin(thread_t,int *) noex ;
int uc_sthrsuspend(thread_t) noex ;
int uc_sthrcontinue(thread_t) noex ;
int uc_sthrminstack() noex ;
int uc_sthrkill(thread_t,int) noex ;
int uc_sthrmain() noex ;
int uc_sthrself() noex ;
int uc_sthryield() noex ;
int uc_sthrsigsetmask(int,const sigset_t *,sigset_t *) noex ;
int uc_sthrstksegment(stack_t *) noex ;
int uc_sthrkeycreate(thread_key_t *,void (*)(void *)) noex ;
int uc_sthrsetspecific(thread_key_t,void *) noex ;
int uc_sthrgetspecific(thread_key_t,void **) noex ;
int uc_sthrgetconcurrency() noex ;
int uc_sthrsetconcurrency(int) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* UCSTHR_INCLUDE */


