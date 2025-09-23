/* pcspolls HEADER  */
/* charset=ISO8859-1 */
/* lang=C20 */

/* access manager interface to PCS loadable-object polling */
/* version %I% last-modified %G% */


/* revision history:

	- 2008-10-07, David A­D­ Morano
        This module was originally written to allow for the main part of the
        PCS-poll facility to be a loadable module.

*/

/* Copyright © 2008 David A­D­ Morano.  All rights reserved. */

#ifndef	PCSPOLLS_INCLUDE
#define	PCSPOLLS_INCLUDE


#include	<envstandards.h>	/* ordered first to configure */
#include	<sys/types.h>
#include	<pthread.h>
#include	<thrcomm.h>
#include	<pcsconf.h>		/* need def for PCSCONF */
#include	<localmisc.h>


#define	PCSPOLLS	struct pcspolls_head
#define	PCSPOLLS_OBJ	struct pcspolls_obj
#define	PCSPOLLS_INFO	struct pcspolls_information
#define	PCSPOLLS_NAME	struct pcspolls_n
#define	PCSPOLLS_THREAD	struct pcspolls_thread
#define	PCSPOLLS_FL	struct pcspolls_flags
#define	PCSPOLLS_MAGIC	0x88773421

#define	PCSPOLLS_POLLCNAME	"pcspolls"


struct pcspolls_n {
	cchar		*name ;
	uint		objsize ;
	uint		infosize ;
} ;

struct pcspolls_obj {
	cchar		*name ;
	int		objsize ;
	int		infosize ;
} ;

struct pcspolls_information {
	int		dummy ;
} ;

struct pcspolls_flags {
	uint		working:1 ;
} ;

struct pcspolls_thread {
	PCSPOLLS	*op ;
	pcsconf		*pcp ;
	cchar		*sn ;
	cchar 		*pr ;
	cchar		**envv ;
	THRCOMM		tc ;
	pid_t		pid ;
	pthread_t	tid ;
	int		trs ;
	volatile int	f_exiting ;
} ;

struct pcspolls_head {
	pcsconf		*pcp ;
	cchar		*a ;		/* memory-allocation */
	cchar		*sn ;
	cchar 		*pr ;
	mainv		envv ;
	PCSPOLLS_THREAD	t ;
	PCSPOLLS_FL	fl ;
	uint		magic ;
} ;

typedef	PCSPOLLS	pcspolls ;
typedef	PCSPOLLS_INFO	pcspolls_info ;

EXTERNC_begin

extern int	pcspolls_start(pcspolls *,pcsconf *,cchar *) noex ;
extern int	pcspolls_info(pcspolls *,pcspolls_info *) noex ;
extern int	pcspolls_cmd(pcspolls *,int) noex ;
extern int	pcspolls_finish(pcspolls *) noex ;

EXTERNC_end


#endif /* PCSPOLLS_INCLUDE */


