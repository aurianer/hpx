# Copyright (c) 2019 The STE||AR-Group
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

if(HPX_WITH_EXAMPLES)
  add_hpx_pseudo_target(examples.modules.checkpoint)
  add_hpx_pseudo_dependencies(examples.modules examples.modules.checkpoint)
  if(HPX_WITH_TESTS
     AND HPX_WITH_TESTS_EXAMPLES
     AND HPX_CHECKPOINT_WITH_TESTS
  )
    add_hpx_pseudo_target(tests.examples.modules.checkpoint)
    add_hpx_pseudo_dependencies(
      tests.examples.modules tests.examples.modules.checkpoint
    )
  endif()
else()
  return()
endif()

set(example_programs)

set(example_programs ${example_programs} 1d_stencil_4_checkpoint)

set(1d_stencil_4_checkpoint_PARAMETERS THREADS_PER_LOCALITY 4)

foreach(example_program ${example_programs})
  set(sources ${example_program}.cpp)

  source_group("Source Files" FILES ${sources})

  # add example executable
  add_hpx_executable(
    ${example_program} INTERNAL_FLAGS
    SOURCES ${sources} ${${example_program}_FLAGS}
    FOLDER "Examples/Modules/Full/Checkpoint"
  )

  add_hpx_example_target_dependencies("modules.checkpoint" ${example_program})

  if(HPX_WITH_TESTS AND HPX_WITH_TESTS_EXAMPLES)
    add_hpx_example_test(
      "modules.checkpoint" ${example_program} ${${example_program}_PARAMETERS}
    )
  endif()
endforeach()
