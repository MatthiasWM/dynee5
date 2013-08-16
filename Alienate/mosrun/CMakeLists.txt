cmake_minimum_required (VERSION 2.6)
project (mosrun)
add_definitions(-Wno-multichar -Wno-unused-result)

add_executable(mosrun
./names.cpp
./main.cpp
./log.cpp
./memory.cpp
./breakpoints.cpp
./fileio.cpp
./resourcefork.cpp
./cpu.cpp
./rsrc.cpp
./traps.cpp
./filename.cpp
./systemram.cpp
./MosResource.cpp
./musashi331/m68kops.c
./musashi331/m68kopac.c
./musashi331/m68kopnz.c
./musashi331/m68kdasm.c
./musashi331/m68kcpu.c
./musashi331/m68kopdm.c)
set_target_properties(mosrun PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
install(TARGETS mosrun RUNTIME DESTINATION bin)
