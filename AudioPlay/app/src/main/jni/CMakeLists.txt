cmake_minimum_required(VERSION 3.4.1)

find_library( log-lib
              log )

set(ffmpeg_lib_DIR ${CMAKE_SOURCE_DIR}/../../../libs/${ANDROID_ABI})

add_library( avutil-54
             SHARED
             IMPORTED )
set_target_properties( avutil-54
                       PROPERTIES IMPORTED_LOCATION
                       ${ffmpeg_lib_DIR}/libavutil-54.so )

add_library( swresample-1
             SHARED
             IMPORTED )
set_target_properties( swresample-1
                       PROPERTIES IMPORTED_LOCATION
                       ${ffmpeg_lib_DIR}/libswresample-1.so )

add_library( avcodec-56
             SHARED
             IMPORTED )
set_target_properties( avcodec-56
                       PROPERTIES IMPORTED_LOCATION
                       ${ffmpeg_lib_DIR}/libavcodec-56.so )

add_library( avfilter-5
             SHARED
             IMPORTED)
set_target_properties( avfilter-5
                       PROPERTIES IMPORTED_LOCATION
                       ${ffmpeg_lib_DIR}/libavfilter-5.so )

add_library( swscale-3
             SHARED
             IMPORTED)
set_target_properties( swscale-3
                       PROPERTIES IMPORTED_LOCATION
                       ${ffmpeg_lib_DIR}/libswscale-3.so )

add_library( avdevice-56
             SHARED
             IMPORTED)
set_target_properties( avdevice-56
                       PROPERTIES IMPORTED_LOCATION
                       ${ffmpeg_lib_DIR}/libavdevice-56.so )

add_library( avformat-56
             SHARED
             IMPORTED)
set_target_properties( avformat-56
                       PROPERTIES IMPORTED_LOCATION
                       ${ffmpeg_lib_DIR}/libavformat-56.so )

add_library( postproc-53
             SHARED
             IMPORTED)
set_target_properties( postproc-53
                       PROPERTIES IMPORTED_LOCATION
                       ${ffmpeg_lib_DIR}/libpostproc-53.so )

include_directories(${CMAKE_SOURCE_DIR}/include)

add_library( audio-play
             SHARED
             audioplay.c )

target_link_libraries( audio-play
                       avutil-54
                       swresample-1
                       avcodec-56
                       avfilter-5
                       swscale-3
                       avdevice-56
                       avformat-56
                       postproc-53
                       ${log-lib} )