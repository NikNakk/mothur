cmake_minimum_required (VERSION 2.8)
enable_testing()
add_executable(uchime
  addtargets2.cpp
  alignchime.cpp
  alignchimel.cpp
  alnparams.cpp
  alpha.cpp
  alpha2.cpp
  fractid.cpp
  getparents.cpp
  globalalign2.cpp
  make3way.cpp
  mx.cpp
  myutils.cpp
  path.cpp
  searchchime.cpp
  seqdb.cpp
  setnucmx.cpp
  sfasta.cpp
  tracebackbit.cpp
  uchime_main.cpp
  usort.cpp
  viterbifast.cpp
  writechhit.cpp
)
add_definitions(-DUCHIMES=1)

install(TARGETS uchime DESTINATION bin)

add_test(uchimeUsage uchime)
set_tests_properties(uchimeUsage
  PROPERTIES
  PASS_REGULAR_EXPRESSION "Usage"
)