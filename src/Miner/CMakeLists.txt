option(ANDROID_CROSS_COMPILE "Build with android NDK" OFF)

# Add an executable called TRRXITTEminer with main.cpp as the entrypoint
add_executable(TRRXITTEminer main.cpp GetConfig.cpp)

if (OPENSSL_FOUND)
    target_link_libraries(TRRXITTEminer ${OPENSSL_LIBRARIES})

    if (MSVC)
        target_link_libraries(TRRXITTEminer ws2_32 gdi32 advapi32 crypt32 user32)
    endif()
endif()

# Link TRRXITTEminer to the libraries it uses
target_link_libraries(TRRXITTEminer
    ArgonVariants
    Argon2
    Backend
    Blake2
    Config
    MinerManager
    PoolCommunication
    Types
    Utilities)

# Need to link against pthreads on non windows
if (NOT MSVC AND NOT ANDROID_CROSS_COMPILE)
    find_package(Threads REQUIRED)
    target_link_libraries(TRRXITTEminer Threads::Threads)
endif()
