/* testpack */


#include	<iostream>

#include	<envstandards.h>
#include	<utypedefs.h>
#include	<clanguage.h>
#include	<usysrets.h>
#include	<sncpyx.h>
#include	<sncpyxw.h>

using std::cout ;

template<typename ... A>
int thingx(cchar *,int,A ... arg,int) noex {
    return 0 ;
}
int thingx(cchar *,int,int,ccp,ccp,int) noex {
    return 0 ;
}

template<typename ... Args>
static int magic(Args ... args) noex {
	return (args && ...) ;
}

template<typename ... Args>
static int thingw(char *dp,int dl,Args ... args,int other) noex {
	nullptr_t	n{} ;
	cint		na = npack(Args) ;
	int		rs = SR_OK ;
	switch (na) {
	case 0:
	    rs = thingx(dp,dl,na,args ...,n,n,n,other) ;
	    break ;
	case 1:
	    rs = thingx(dp,dl,na,args ...,n,n,other) ;
	    break ;
	case 2:
	    rs = thingx(dp,dl,na,args ...,n,other) ;
	    break ;
	case 3:
	    rs = thingx(dp,dl,na,args ...,other) ;
	    break ;
	} /* end switch */
	return rs ;
}


int main(int,cchar **,cchar **) {
	int	ex = 0 ;

	{
	    int	v1 = 1 ;
	    int	v2 = 2 ;
	    ex = magic(v1,v2) ;
	    if (ex) {
	        cout << "yes\n" ;
	    } else {
	        cout << "no\n" ;
	    }
	}

	{
		constexpr int dlen = 100 ;
		cint	other = 12 ;
		cchar	*s1 = "Hello " ;
		cchar	*s2 = "world!\n" ;
		char	dbuf[dlen+1] ;
		sncpy(dbuf,dlen,s1,s2) ;
		sncpyw(dbuf,dlen,s1,s2,other) ;
	}
	return ex ;
}
/* end subroutine (main) */


