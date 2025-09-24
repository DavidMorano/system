/* complex1 MODULE (module-implementation-unit) */
/* charset=ISO8859-1 */
/* lang=C++20 (conformance reviewed) */


//   complex.h - declaration of template
//   of complex number
//
//   The code is property of LIBROW
//   You can use it on your own
//   When utilizing credit LIBROW website

module ;

module complex ;


//   Imaginary unity constants
template<class T> const complex<T> complex<T>::i(T(0.0), T(1.0)) ;
template<class T> const complex<T> complex<T>::j(T(0.0), T(1.0)) ;


