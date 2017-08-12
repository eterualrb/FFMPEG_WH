cmake_minimum_required(VERSION 3.4.1)

find_library( log-lib
              log )

set(ffmpeg_lib_DIR ${CMAKE_SOURCE_DIR}/../../../libs/${ANDROID_ABI})

add_library( avutil-55
             SHARED
             IMPORTED )
set_target_properties( avutil-55
                       PROPERTIES IMPORTED_LOCATION
                       ${ffmpeg_lib_DIR}/libavutil-55.so )

add_library( swresample-2
             SHARED
             IMPORTED )
set_target_properties( swresample-2
                       PROPERTIES IMPORTED_LOCATION
                       ${ffmpeg_lib_DIR}/libswresample-2.so )

add_library( avcodec-57
             SHARED
             IMPORTED )
set_target_properties( avcodec-57
                       PROPERTIES IMPORTED_LOCATION
                       ${ffmpeg_lib_DIR}/libavcodec-57.so )

add_library( avfilter-6
             SHARED
             IMPORTED)
set_target_properties( avfilter-6
                       PROPERTIES IMPORTED_LOCATION
                       ${ffmpeg_lib_DIR}/libavfilter-6.so )

add_library( swscale-4
             SHARED
             IMPORTED)
set_target_properties( swscale-4
                       PROPERTIES IMPORTED_LOCATION
                       ${ffmpeg_lib_DIR}/libswscale-4.so )

add_library( avdevice-57
             SHARED
             IMPORTED)
set_target_properties( avdevice-57
                       PROPERTIES IMPORTED_LOCATION
                       ${ffmpeg_lib_DIR}/libavdevice-57.so )

add_library( avformat-57
             SHARED
             IMPORTED)
set_target_properties( avformat-57
                       PROPERTIES IMPORTED_LOCATION
                       ${ffmpeg_lib_DIR}/libavformat-57.so )

add_library( gperf
             SHARED
             IMPORTED)
set_target_properties( gperf
                       PROPERTIES IMPORTED_LOCATION
                       ${ffmpeg_lib_DIR}/libgperf.so )

include_directories(${CMAKE_SOURCE_DIR}/include)

add_library( ffmpeg-test
             SHARED
             ffmpeg-test.c )

target_link_libraries( ffmpeg-test
                       avutil-55
                       swresample-2
                       avcodec-57
                       avfilter-6
                       swscale-4
                       avdevice-57
                       avformat-57
                       gperf
                       ${log-lib} )