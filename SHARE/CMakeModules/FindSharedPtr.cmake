# Module checks for shared_ptr support.
#
# Sets the following variables:
#
# SHARED_PTR_NAMESPACE: Namespace of shared_ptr (e.g. std)
# SHARED_PTR_HEADER: The name of header file supplying shared_ptr
#
# check if make_shared works
macro(find_shared_ptr)

if(NOT SHARED_PTR_FOUND)

  include(CheckCXXSourceCompiles)
  include(CMakePushCheckState)
  cmake_push_check_state()
  if(Boost_INCLUDE_DIR)
    list(APPEND CMAKE_REQUIRED_INCLUDES
         ${Boost_INCLUDE_DIR}
         )
  endif()
  if(Boost_INCLUDE_DIRS)
    list(APPEND CMAKE_REQUIRED_INCLUDES
         ${Boost_INCLUDE_DIRS}
         )
  endif()

  #search header name
   foreach(SHARED_PTR_HEADER_ "<memory>" "<tr1/memory>" "<boost/shared_ptr.hpp>")

     unset(_SHARED_PTR_HEADER_FOUND CACHE)
     check_cxx_source_compiles("
          #include <string>
          #include ${SHARED_PTR_HEADER_}

          int main(void)
          {
            return 0;
          }"
                               _SHARED_PTR_HEADER_FOUND)


    if(_SHARED_PTR_HEADER_FOUND)
      # search namespace
      foreach(SHARED_PTR_NAMESPACE_ "std" "tr1" "std::tr1" "boost")

        unset(_SHARED_PTR_FOUND CACHE)
        check_cxx_source_compiles("
        #include <string>
        #include ${SHARED_PTR_HEADER_}

        using ${SHARED_PTR_NAMESPACE_}::shared_ptr;
        int main(void)
        {
            shared_ptr<int> test_ptr;
            return 0;
        }"
                                  _SHARED_PTR_FOUND)
        if(_SHARED_PTR_FOUND)
          # save result
          set(SHARED_PTR_NAMESPACE ${SHARED_PTR_NAMESPACE_} CACHE INTERNAL "" FORCE)
          set(SHARED_PTR_HEADER ${SHARED_PTR_HEADER_} CACHE INTERNAL "" FORCE)
          set(SHARED_PTR_FOUND ${_SHARED_PTR_FOUND} CACHE INTERNAL "" FORCE)
          message(INFO " find shared_ptr  in ${SHARED_PTR_HEADER_} namespace ${SHARED_PTR_NAMESPACE}")
          return()
        endif(_SHARED_PTR_FOUND)
      endforeach(SHARED_PTR_NAMESPACE_)
    endif(_SHARED_PTR_HEADER_FOUND)
  endforeach(SHARED_PTR_HEADER_)
  cmake_pop_check_state()
endif()
endmacro(find_shared_ptr)
