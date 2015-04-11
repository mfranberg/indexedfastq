project( indexfastq )

cmake_minimum_required( VERSION 2.7 )

include( CheckIncludeFiles )

set( VERSION 2.0 )
check_include_files( dlfcn.h HAVE_DLFCN_H )
check_include_files( getopt.h HAVE_GETOPT_H )
check_include_files( math.h HAVE_MATH_H )
check_include_files( check.h HAVE_LIBCHECK )
check_include_files( memory.h HAVE_MEMORY_H )
check_include_files( stdint.h HAVE_STDINT_H )
check_include_files( strings.h HAVE_STRINGS_H )
check_include_files( string.h HAVE_STRING_H )
check_include_files( sys/stat.h HAVE_SYS_STAT_H )
check_include_files( sys/types.h HAVE_SYS_TYPES_H )
check_include_files( unistd.h HAVE_UNISTD_H )

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h )

include_directories( "lib/cmph/src/" )
include_directories( "lib/bgzf/" )
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

set( CMPH_LIST
lib/cmph/src/bdz.c
lib/cmph/src/bdz_ph.c
lib/cmph/src/bm_numbers.c
lib/cmph/src/bmz.c
lib/cmph/src/bmz8.c
lib/cmph/src/brz.c
lib/cmph/src/buffer_entry.c
lib/cmph/src/buffer_manager.c
lib/cmph/src/chd.c
lib/cmph/src/chd_ph.c
lib/cmph/src/chm.c
lib/cmph/src/cmph.c
lib/cmph/src/cmph_benchmark.c
lib/cmph/src/cmph_structs.c
lib/cmph/src/compressed_rank.c
lib/cmph/src/compressed_seq.c
lib/cmph/src/fch.c
lib/cmph/src/fch_buckets.c
lib/cmph/src/graph.c
lib/cmph/src/hash.c
lib/cmph/src/jenkins_hash.c
lib/cmph/src/linear_string_map.c
lib/cmph/src/main.c
lib/cmph/src/miller_rabin.c
lib/cmph/src/select.c
lib/cmph/src/vqueue.c
lib/cmph/src/vstack.c
)

add_library( cmph ${CMPH_LIST} )

add_executable( indexfastq indexfastq.c indexing.c lib/bgzf/bgzf.c )
target_link_libraries( indexfastq cmph z )

add_executable( findfastq findfastq.c indexing.c lib/bgzf/bgzf.c )
target_link_libraries( findfastq cmph z )
