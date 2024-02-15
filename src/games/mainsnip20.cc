/* bal2 */
/* lang=C++20 */

#include	<sys/types.h>
#include	<climits>
#include	<new>
#include	<functional>
#include	<utility>
#include	<string>
#include	<stack>
#include	<iostream>
#include	<iomanip>


using namespace	std ;


static char compchar(int w) noex {
	char	rch = '\0' ;
	switch (w) {
	case 0: 
	    rch = ')' ; 
	    break ;
	case 1: 
	    rch = '}' ; 
	    break ;
	case 2: 
	    rch = ']' ; 
	    break ;
	} /* end switch */
	return rch ;
}

struct Solution {
	bool isValid(string s) {
	    stack<char>	hist ;
	    cint	n = s.length() ;
	    int		c[3] = { 0, 0, 0 } ;
	    int		ct = 0 ;
	    bool	f = true ;
	    bool	f_open ;
	    for (int i = 0 ; i < n ; i += 1) {
	        cint	ch = (s[i] & UCHAR_MAX) ;
		int	w = 0 ;
	        f_open = false ;
	        switch (ch) {
	        case '(':
	            f_open = true ;
	            w = 0 ;
	            break ;
	        case '{':
	            f_open = true ;
	            w = 1 ;
	            break ;
	        case '[':
	            f_open = true ;
	            w = 2 ;
	            break ;
	        case ')':
	            w = 0 ;
	            break ;
	        case '}':
	            w = 1 ;
	            break ;
	        case ']':
	            w = 2 ;
	            break ;
	        } /* end switch */
	        if (f_open) { /* open */
	            if ((c[w] < (n-i)) && (ct < (n-i))) {
	                char	cc = compchar(w) ;
	                hist.push(cc) ;
	                c[w] += 1 ;
	                ct += 1 ;
	            } else {
	                f = false ;
	                break ;
	            }
	        } else { /* close */
	            if (c[w] > 0) {
	                int	pch = (hist.top() & UCHAR_MAX) ;
		        if (ch == pch) {
	                    c[w] -= 1 ;
	                    ct -= 1 ;
	                    hist.pop() ;
			} else {
	                    f = false ;
	                    break ;
			}
	            } else {
	                f = false ;
	                break ;
	            }
	        }
	        if (!f) break ;
	    } /* end for */
	    if (f) {
 		for (int w = 0 ; w < 3 ; w += 1) {
		    f = (c[w] == 0) ;
		    if (!f) break ;
		}
	    }
	    return f ;
	} /* end method (isValid) */
} ; /* end structure */

int main() {
	Solution	sol ;
	string		s = "(){[[{}]]()}{}" ;
	bool		f ;
	f = sol.isValid(s) ;
	cout << f << endl ;
	return 0 ;
}
/* end subroutine (main) */


