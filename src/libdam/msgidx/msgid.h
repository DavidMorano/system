/* msgid HEADER */
/* encoding=ISO8859-1 */
/* lang=C20 */

/* manage message-id storage */
/* version %I% last-modified %G% */


/* revision history:

	= 2003-12-10, David A­D­ Morano
	Originally written for Rightcore Network Services.

*/

/* Copyright © 2003 David A­D­ Morano.  All rights reserved. */

#ifndef	MSGID_INCLUDE
#define	MSGID_INCLUDE


#include	<envstandards.h>	/* MUST be first to configure */
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>

#include	"msgide.h"


#define	MSGID_MAGIC		1092847456
#define	MSGID			struct msgid_head
#define	MSGID_FL		struct msgid_flags
#define	MSGID_FM		struct msgid_filemagic
#define	MSGID_FH		struct msgid_filehead
#define	MSGID_KEY		struct msgid_keyer
#define	MSGID_CUR		struct msgid_cursor
#define	MSGID_BUF		struct msgid_buffer
#define	MSGID_ENT		MSGIDE_ALL
#define	MSGID_MAGICSIZE		16
/* other defines */
#define	MSGID_FILEPATH		"/tmp/msgid"
#define	MSGID_FILEVERSION	0
#define	MSGID_ENDIAN		1	/* big endian */


struct msgid_buffer {
	char		*mbuf ;		/* fixed buffer pointer */
	uint		mlen ;		/* fixed buffer length */
	uint		off ;		/* file offset of valid area */
	uint		len ;		/* length of valid area */
} ;

/* decoded file magic */
struct msgid_filemagic {
	char		magic[MSGID_MAGICSIZE] ;
	uchar		vetu[4] ;
} ;

/* decoded file header values */
struct msgid_filehead {
	uint		wcount ;
	uint		wtime ;
	uint		nentries ;
} ;

struct msgid_flags {
	uint		fileinit:1 ;		/* file init'ed */
	uint		writable:1 ;
	uint		readlocked:1 ;
	uint		writelocked:1 ;
	uint		cursorlockbroken:1 ;	/* cursor lock broken */
	uint		cursoracc:1 ;		/* accessed while cursored? */
	uint		remote:1 ;		/* remote mounted file */
} ;

struct msgid_head {
	cchar		*fname ;
	MSGID_FL	fl ;
	MSGID_FH	h ;
	MSGID_BUF	b ;		/* file buffer */
	time_t		opentime ;	/* file open time */
	time_t		accesstime ;	/* file access time */
	time_t		mtime ;		/* file modification time */
	mode_t		operm ;
	uint		magic ;
	int		oflags ;
	int		maxentry ;
	int		pagesize ;
	int		filesize ;
	int		fd ;
	int		cursors ;
	int		entsz ;		/* entry buffer size */
} ;

struct msgid_cursor {
	int		i ;
} ;

struct msgid_keyer {
	cchar		*recip ;
	cchar		*from ;
	cchar		*mid ;
	time_t		mtime ;
	int		reciplen ;
	int		midlen ;
} ;

typedef MSGID		msgid ;
typedef	MSGID_FL	msgid_fl ;
typedef	MSGID_FM	msgid_fm ;
typedef	MSGID_FH	msgid_fh ;
typedef MSGID_CUR	msgid_cur ;
typedef MSGID_ENT	msgid_ent ;
typedef MSGID_KEY	msgid_key ;
typedef	MSGID_BUF	msgid_buf ;

EXTERNC_begin

extern int msgid_open(msgid *,cchar *,int,mode_t,int) noex ;
extern int msgid_check(msgid *,time_t) noex ;
extern int msgid_curbegin(msgid *,msgid_cur *) noex ;
extern int msgid_curend(msgid *,msgid_cur *) noex ;
extern int msgid_curenum(msgid *,msgid_cur *,char *,int,msgid_ent *) noex ;
extern int msgid_match(msgid *,time_t,msgid_key *,cc *,int,msgid_ent *) noex ;
extern int msgid_update(msgid *,time_t,msgid_key *,cc *,int,msgid_ent *) noex ;
extern int msgid_matchid(msgid *,time_t,cchar *,int,msgid_ent *) noex ;
extern int msgid_write(msgid *,int,msgid_ent *) noex ;
extern int msgid_close(msgid *) noex ;

#ifdef	COMMENT
extern int msgid_txbegin(msgid *) noex ;
extern int msgid_txabort(msgid *,int) noex ;
extern int msgid_txcommit(msgid *,int) noex ;
extern int msgid_fetchsvc(msgid *,cchar *,msgid_cur *,msgid_ent *) noex ;
extern int msgid_fetchhostsvc(msgid *,uint,cc *,msgid_cur *,msgid_ent *) noex ;
extern int msgid_fetchhostpid(msgid *,uint,int,msgid_cur *,msgid_ent *) noex ;
#endif /* COMMENT */

EXTERNC_end


#endif /* MSGID_INCLUDE */


