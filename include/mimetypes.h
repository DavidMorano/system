/* mimetypes HEADER */
/* lang=C20 */


/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

#ifndef	MIMETYPES_INCLUDE
#define	MIMETYPES_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<sys/param.h>
#include	<envstandards.h>	/* ordered first to configure */
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<clanguage.h>
#include	<hdb.h>


/* object defines */
#define	MIMETYPES		HDB
#define	MIMETYPES_CUR		HDB_CUR
#define	MIMETYPES_TYPELEN	MAXNAMELEN


EXTERNC_begin

extern int mimetypes_start(MIMETYPES *) noex ;
extern int mimetypes_finish(MIMETYPES *) noex ;
extern int mimetypes_file(MIMETYPES *,cchar *) noex ;
extern int mimetypes_curbegin(MIMETYPES *,MIMETYPES_CUR *) noex ;
extern int mimetypes_curend(MIMETYPES *,MIMETYPES_CUR *) noex ;
extern int mimetypes_enum(MIMETYPES *,MIMETYPES_CUR *,char *,char *) noex ;
extern int mimetypes_fetch(MIMETYPES *,cchar *,MIMETYPES_CUR *,char *) noex ;
extern int mimetypes_find(MIMETYPES *,char *,cchar *) noex ;
extern int mimetypes_get(MIMETYPES *,char *,cchar *) noex ;

EXTERNC_end


#endif /* MIMETYPES_INCLUDE */


