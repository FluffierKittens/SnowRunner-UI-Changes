link_directories(SRUIC lib)

add_library (SRUIC SHARED "SRUIC.cpp" "Logging.cpp" "version.rc")

target_link_libraries(SRUIC detours)