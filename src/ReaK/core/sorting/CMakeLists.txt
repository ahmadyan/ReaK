
#set(SORTING_SOURCES "${SRCROOT}${RKROOTFINDERSDIR}/.cpp")

set(SORTING_HEADERS 
  "${RKSORTINGDIR}/bisection_method.hpp"
  "${RKSORTINGDIR}/broyden_method.hpp"
  "${RKSORTINGDIR}/newton_raphson_method.hpp"
  "${RKSORTINGDIR}/secant_method.hpp"
)

#add_library(reak_sorting STATIC ${SORTING_SOURCES})
#setup_custom_target(reak_sorting "${SRCROOT}${RKSORTINGDIR}")

setup_headers("${SORTING_HEADERS}" "${RKSORTINGDIR}")

add_executable(test_sorting_perf "${SRCROOT}${RKSORTINGDIR}/test_sorting_perf.cpp")
setup_custom_target(test_sorting_perf "${SRCROOT}${RKSORTINGDIR}")
target_link_libraries(test_sorting_perf reak_rtti ${EXTRA_SYSTEM_LIBS})

add_executable(unit_test_sorting "${SRCROOT}${RKSORTINGDIR}/unit_test_sorting.cpp")
setup_custom_test_program(unit_test_sorting "${SRCROOT}${RKSORTINGDIR}")
target_link_libraries(unit_test_sorting reak_rtti)
target_link_libraries(unit_test_sorting ${Boost_LIBRARIES} ${EXTRA_SYSTEM_LIBS})






