/* ow (Open Weather) */
/* charset=ISO8859-1 */
/* lang=C20 (conformance reviewed) */


#ifndef	OW_INCLUDE
#define	OW_INCLUDE


#include	<envstandards.h>	/* MUST be ordered first to configure */
#include	<sys/types.h>
#include	<clanguage.h>
#include	<utypedefs.h>
#include	<utypealiases.h>
#include	<usysdefs.h>
#include	<usysrets.h>
#include	<vecstr.h>
#include	<paramfile.h>
#include	<logfile.h>
#include	<logsys.h>


#define	OW_SEARCHNAME	"weather"
#define	OW_CFNAME	"conf"
#define	OW_VDNAME	"var"
#define	OW_LDNAME	"log"
#define	OW_METARDNAME	"metars"
#define	OW_INTSHORT	(2*60)
#define	OW_INTLONG	(15*60)
#define	OW_WHOST	"weather.noaa.gov"
#define	OW_WPREFIX	"pub/data/observations/metar/decoded"
#define	OW_WS		"kbos"
#define	OW_TO		5		/* web-access time-out */
#define	OW_WEBPORT	"www"
#define	OW_LOGFACILITY	"daemon"

#define	OW		struct ow
#define	OW_FL		struct ow_flags

#include	"owconfig.h"


struct ow_flags {
	uint		stores:1 ;
	uint		svars:1 ;
	uint		lf:1 ;		/* LOGFILE */
	uint		ls:1 ;		/* LOGSYS */
} ;

struct ow {
	OW_FL		f, open ;
	vecstr		stores ;
	vecstr		svars ;
	OWCONFIG	c ;
	logfile		lf ;
	locsys		ls ;
	cchar		*pr ;		/* from arguments */
	cchar		*sn ;		/* from static config */
	cchar		*vd ;		/* from static config */
	cchar		*ws ;		/* from arguments */
	cchar		*rn ;		/* root-name (derived) */
	cchar		*ld ;		/* log-file directory component name */
	cchar		*nodename ;
	cchar		*domainname ;
	cchar		*cfname ;
	cchar		*whost ;	/* from configuration */
	cchar		*wprefix ; 	/* from configuration */
	cchar		*defws ;	/* from configuration */
	cchar		*logfname ;	/* for LOGFILE */
	cchar		*logfacility ;	/* for LOGSYS */
	time_t		daytime ;
	time_t		ti_weather ;	/* weather-file m-time */
	uid_t		pruid ;
	gid_t		prgid ;
	pid_t		pid ;
	int		intpoll ;	/* from configuration */
	int		oflags ;	/* from arguments */
	int		to ;		/* from arguments */
	int		wmin ;		/* from current time-of-day */
	int		wfd ;		/* "weather" FD */
	char		logid[LOGFILE_LOGIDLEN+1] ;
} ;

typedef	OW		ow ;
typedef	OW_FL		ow_fl ;

EXTERNC_begin

extern int	ow_start(ow *,cchar *,cchar *,cchar *,
			cchar *,time_t,int,int) noex ;
extern int	ow_finish(ow *,int) noex ;
extern int	ow_setentry(ow *,cchar **,cchar *,int) noex ;
extern int	ow_nodedomain(ow *) noex ;
extern int	ow_setmin(ow *) noex ;
extern int	ow_prid(ow *) noex ;
extern int	ow_isvalid(ow *,time_t) noex ;
extern int	ow_logprintf(ow *,cchar *,...) noex ;

EXTERNC_end


#endif /* OW_INCLUDE */



