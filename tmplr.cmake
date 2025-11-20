set(TMPLR_VERSION "1.3")
set(TMPLR_URL
    "https://github.com/open-s4c/tmplr/archive/refs/tags/v${TMPLR_VERSION}.tar.gz"
)
set(TMPLR_SHA256
    "SHA256=435579ac6891d62f79f599c64a639a6185682370b5178281326a4c7a2d026eb1")

if(DEFINED TMPLR_PROGRAM AND EXISTS "${TMPLR_PROGRAM}")
  # Cached values are valid; nothing further needed.
  return()
endif()

unset(TMPLR_PROGRAM CACHE)

find_program(_tmplr tmplr)
if(_tmplr)
  # check version
  execute_process(
    COMMAND ${TMPLR_PROGRAM} -h
    OUTPUT_VARIABLE _tmplr_version
    RESULT_VARIABLE _tmplr_version_rc)

  if(_tmplr_version_rc EQUAL 0 AND _tmplr_version MATCHES ".*${VERSION}.*")
    set(TMPLR_PROGRAM
        "${_tmplr}"
        CACHE INTERNAL "tmplr executable")
    return()
  endif()
endif()

set(_tmplr_archive "${CMAKE_BINARY_DIR}/tmplr-${TMPLR_VERSION}.tar.gz")
set(_tmplr_source_dir "${CMAKE_BINARY_DIR}/tmplr-${TMPLR_VERSION}")
set(_tmplr_binary "${_tmplr_source_dir}/tmplr")

if(NOT EXISTS "${_tmplr_archive}")
  file(
    DOWNLOAD "${TMPLR_URL}" "${_tmplr_archive}"
    SHOW_PROGRESS
    EXPECTED_HASH "${TMPLR_SHA256}")
endif()

if(NOT EXISTS "${_tmplr_binary}")
  file(REMOVE_RECURSE "${_tmplr_source_dir}")
  execute_process(
    COMMAND ${CMAKE_COMMAND} -E tar xzf "${_tmplr_archive}"
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    RESULT_VARIABLE _tmplr_extract_rv)
  if(NOT _tmplr_extract_rv EQUAL 0)
    message(FATAL_ERROR "Failed to extract tmplr ${TMPLR_VERSION}")
  endif()

  execute_process(
    COMMAND ${CMAKE_MAKE_PROGRAM}
    WORKING_DIRECTORY "${_tmplr_source_dir}"
    RESULT_VARIABLE _tmplr_build_rv)
  if(NOT _tmplr_build_rv EQUAL 0)
    message(FATAL_ERROR "Failed to compile tmplr ${TMPLR_VERSION}")
  endif()
endif()

if(NOT EXISTS "${_tmplr_binary}")
  message(
    FATAL_ERROR "Expected tmplr binary at ${_tmplr_binary} was not created")
endif()

set(TMPLR_PROGRAM
    "${_tmplr_binary}"
    CACHE INTERNAL "tmplr executable")

execute_process(
  COMMAND ${TMPLR_PROGRAM} -h
  OUTPUT_VARIABLE _tmplr_version
  RESULT_VARIABLE _tmplr_version_rc)

message(STATUS "tmplr v${TMPLR_VERSION}: ${TMPLR_PROGRAM}")
