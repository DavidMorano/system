/* issue HEADER */
/* lang=C++20 */


/* Copyright � 1998 David A�D� Morano.  All rights reserved. */

#ifndef	ISSUE_INCLUDE
#define	ISSUE_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<ptm.h>
#include	<lockrw.h>
#include	<paramfile.h>
#include	<vechand.h>
#include	<localmisc.h>


#define	ISSUE_MAGIC	0x75648941
#define	ISSUE		struct issue_head
#define	ISSUE_MAPPER	struct issue_mapper
#define	ISSUE_FL	struct issue_flags

#define	ISSUE_DEFKEYNAME	"login"


struct issue_mapper {
	uint		magic ;
	LOCKRW		rwm ;
	PARAMFILE	dirsfile ;
	vechand		mapdirs ;
	const char	*fname ;
	time_t		ti_mtime ;
	time_t		ti_check ;
} ;

struct issue_flags {
	uint		sorted:1 ;
} ;
struct issue_head {
	uint		magic ;
	ISSUE_FL	f ;
	ISSUE_MAPPER	mapper ;
	PTM		m ;		/* this is for all of the data */
	const char	**envv ;
	const char	*pr ;
	const char	*fe ;
	time_t		ti_lastcheck ;
	int		nmaps ;
	int		nenv ;
} ;

#ifdef	__cplusplus
extern "C" {
#endif

extern int	issue_open(ISSUE *,cchar *) noex ;
extern int	issue_check(ISSUE *,time_t) noex ;
extern int	issue_process(ISSUE *,cchar *,cchar **,int) noex ;
extern int	issue_audit(ISSUE *) noex ;
extern int	issue_close(ISSUE *) noex ;

#ifdef	__cplusplus
}
#endif


#endif /* ISSUE_INCLUDE */


