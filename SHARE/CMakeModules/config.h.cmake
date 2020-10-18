#ifndef @PROJECT_NAME@_CONFIG_H
#define @PROJECT_NAME@_CONFIG_H

#cmakedefine HAVE_LIBEXECINFO
#cmakedefine HAVE_BACKTRACE_SYMBOLS

#cmakedefine HAVE_DBGHELP

#cmakedefine HAVE_CXXABI_H
#cmakedefine HAVE_DEX
#cmakedefine HAVE_DLFCN
#cmakedefine HAVE_SIGNAL_H
#cmakedefine HAVE_SIGACTION
#cmakedefine HAVE_UNISTD_H
#cmakedefine HAVE_SYMLINK
#cmakedefine HAVE_STATEMENTS_WITH_INITIALIZER
#cmakedefine HAVE_INET_NTOP
#cmakedefine HAVE_IPHLPAPI_H
#cmakedefine HAVE_WINERROR_H
#cmakedefine HAVE_GETIFADDRS
#cmakedefine HAVE_WINSOCK_H
#cmakedefine HAVE_FUTEX_H
#cmakedefine HAVE_POSIX_SEMAPHORES
#cmakedefine SIZEOF_PTHREAD_MUTEX_T @SIZEOF_PTHREAD_MUTEX_T@
#cmakedefine SIZEOF_PTHREAD_COND_T @SIZEOF_PTHREAD_COND_T@
#cmakedefine SIZEOF_SEM_T @SIZEOF_SEM_T@
#cmakedefine CMAKE_DEBUG_POSTFIX "@CMAKE_DEBUG_POSTFIX@"
#cmakedefine CMAKE_RELEASE_POSTFIX "@CMAKE_RELEASE_POSTFIX@"
#cmakedefine HAVE_LIBADVAPI32
#cmakedefine HAVE_X86INTRIN_H
#cmakedefine HAVE_INTRIN_H

#cmakedefine SHARED_PTR_NAMESPACE @SHARED_PTR_NAMESPACE@
#cmakedefine SHARED_PTR_HEADER @SHARED_PTR_HEADER@
#cmakedefine CAN_USE_SFINAE_METHODS_DETECTOR
#cmakedefine SHARE_BIGENDIAN
#cmakedefine HAVE_SYS_TYPES_H
#cmakedefine HAVE_SYS_SOCKET_H

#cmakedefine SIZE_OF_LONG_DOUBLE
#cmakedefine SIZE_OF_LONG_LONG_INT

#endif /* @PROJECT_NAME@_CONFIG_H */
