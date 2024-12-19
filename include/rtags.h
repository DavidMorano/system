/* rtags HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 (conformance reviewed) */


/* Copyright © 1999 David A­D­ Morano.  All rights reserved. */

#ifndef	RTAGS_INCLUDE
#define	RTAGS_INCLUDE

#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/types.h>
#include	<sys/param.h>
#include	<hdb.h>
#include	<vecobj.h>
#include	<ptm.h>


#define	RTAGS		struct rtags_head
#define	RTAGS_CUR	struct rtags_cursor
#define	RTAGS_TAG	struct rtags_tager
#define	RTAGS_TE	struct rtags_test
#define	RTAGS_MAGIC	0x99662223


struct rtags_tager {
	char		*fname ;
	uint		hash ;
	uint		recoff ;
	uint		reclen ;
} ;

struct rtags_test {
	int		fi ;
	uint		recoff ;
	uint		reclen ;
} ;

struct rtags_head {
	uint		magic ;
	vecobj		*flp ;		/* file-list-pointer */
	vecobj		*tlp ;		/* tag-list-pointer */
	hdb		*hdp ;		/* has-data-pointer */
	ptm		*mxp ;		/* mutex-pointer */
	uint		magic ;
	int		nfiles ;
} ;

struct rtags_cursor {
	int		i ;
} ;

typedef	RTAGS		rtags ;
typedef	RTAGS_CUR	rtags_cur ;
typedef	RTAGS_TAG	rtags_tag ;
typedef	RTAGS_TE	rtags_te ;

EXTERNC_begin

typedef int (*rtags_f)(cvoid **,cvoid **) noex ;

extern int	rtags_start(rtags *,int) noex ;
extern int	rtags_add(rtags *,rtags_tag *) noex ;
extern int	rtags_sort(rtags *,rtags_f) noex ;
extern int	rtags_curbegin(rtags *,rtags_cur *) noex ;
extern int	rtags_curend(rtags *,rtags_cur *) noex ;
extern int	rtags_curdump(rtags *,rtags_cur *) noex ;
extern int	rtags_enum(rtags *,rtags_cur *,rtags_tag *) noex ;
extern int	rtags_count(rtags *) noex ;
extern int	rtags_finish(rtags *) noex ;

EXTERNC_end


#endif /* RTAGS_INCLUDE */


