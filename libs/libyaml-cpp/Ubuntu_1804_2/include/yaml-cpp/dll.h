
#ifndef YAML_CPP_API_H
#define YAML_CPP_API_H

#ifdef YAML_CPP_STATIC_DEFINE
#  define YAML_CPP_API
#  define YAML_CPP_NO_EXPORT
#else
#  ifndef YAML_CPP_API
#    ifdef yaml_cpp_EXPORTS
        /* We are building this library */
#      define YAML_CPP_API 
#    else
        /* We are using this library */
#      define YAML_CPP_API 
#    endif
#  endif

#  ifndef YAML_CPP_NO_EXPORT
#    define YAML_CPP_NO_EXPORT 
#  endif
#endif

#ifndef YAML_CPP_DEPRECATED
#  define YAML_CPP_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef YAML_CPP_DEPRECATED_EXPORT
#  define YAML_CPP_DEPRECATED_EXPORT YAML_CPP_API YAML_CPP_DEPRECATED
#endif

#ifndef YAML_CPP_DEPRECATED_NO_EXPORT
#  define YAML_CPP_DEPRECATED_NO_EXPORT YAML_CPP_NO_EXPORT YAML_CPP_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef YAML_CPP_NO_DEPRECATED
#    define YAML_CPP_NO_DEPRECATED
#  endif
#endif

#endif /* YAML_CPP_API_H */
