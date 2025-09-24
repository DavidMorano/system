/* mailmsgviewer HEADER */
/* charset=ISO8859-1 */
/* lang=C20 */

/* create and cache message content files */
/* version %I% last-modified %G% */


/* revision history:

	= 1998-04-01, David A­D­ Morano
	This code was originally written.

*/

/* Copyright © 1998 David A­D­ Morano.  All rights reserved. */

/*******************************************************************************

  	Object:
	mailmsgviewer

	Description:
	This little object supports some buffered file operations
	for low-overhead buffered I-O requirements.

*******************************************************************************/

#ifndef	MAILMSGVIEWER_INCLUDE
#define	MAILMSGVIEWER_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<usystem.h>
#include	<vecobj.h>


#define	MAILMSGVIEWER_MAGIC	0x54837492
#define	MAILMSGVIEWER		struct mailmsgviewer_head
#define	MAILMSGVIEWER_FL	struct mailmsgviewer_flags


struct mailmsgviewer_flags {
	uint		eof:1 ;
} ;

struct mailmsgviewer_head {
	vecobj		*llp ;		/* Line-List-Pointer */
	char		*fname ;
	char		*mapdata ;
	char		*bp ;
	size_t		mapsize ;
	MAILMSGVIEWER_FL fl ;
	uint		magic ;
	int		ln ;
} ;

typedef	MAILMSGVIEWER		mailmsgviewer ;
typedef	MAILMSGVIEWER_FL	mailmsgviewer_fl ;

EXTERNC_begin

extern int mailmsgviewer_open(mailmsgviewer *,cchar *) noex ;
extern int mailmsgviewer_getline(mailmsgviewer *,int,cchar **) noex ;
extern int mailmsgviewer_close(mailmsgviewer *) noex ;
extern int mailmsgviewer_seek(mailmsgviewer *,off_t,int) noex ;
extern int mailmsgviewer_tell(mailmsgviewer *,off_t *) noex ;
extern int mailmsgviewer_rewind(mailmsgviewer *) noex ;

EXTERNC_end


#endif /* MAILMSGVIEWER_INCLUDE */


