macro(set_compiller_flags)
    if("${CMAKE_CXX_COMPILER_ID}" MATCHES "(Apple)?[Cc]lang")
        message("Using Clang compiller settings")
        set(CMAKE_CXX_FLAGS "-std=c++1z \
                             -Weverything \
                             -Werror \
                             -Wno-c++98-compat \
                             -Wno-c++11-extensions \
                             -Wno-padded \
                             -Wno-weak-vtables \
                             -Wno-exit-time-destructors \
                             -Wno-unknown-pragmas \
                             -pthread \
                             -fPIC")
        set(CMAKE_CXX_FLAGS_DEBUG "-g")
        set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
        set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -DNDEBUG")
    elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
        message("Using GNU compiller settings")
        set(CMAKE_CXX_FLAGS "-std=c++17 \
                             -Wall \
                             -Wextra \
                             -Wpedantic \
                             -Werror \
                             -Wfloat-equal \
                             -Wshadow \
                             -Wconversion \
                             -Wno-unknown-pragmas \
                             -pthread \
                             -fPIC")
        set(CMAKE_CXX_FLAGS_DEBUG "-g")
        set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
        set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -DNDEBUG")
    elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
        message("Using Visual Studio C++ compiller settings")
        set(CMAKE_CXX_FLAGS "/std:c++17 \
                             /EHsc \
                             /W4 \
                             /WX")
        set(CMAKE_CXX_FLAGS_DEBUG "/Od /ZI")
        set(CMAKE_CXX_FLAGS_RELEASE "/O2 /DNDEBUG")
        set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/O2 /DNDEBUG")
    endif()
endmacro()

