###############################################################################
# Swig and PCRE do not support blank spaces in install directory namespaces
# This means that ITK cannot be built in a path that contains spaces
# See http://www.swig.org/Doc1.3/Windows.html
if("${CMAKE_BINARY_DIR}" MATCHES "^.* .*$")
  message(FATAL_ERROR "Swig and PCRE do not support paths with space characters. Please change build directory name.")
endif()

###############################################################################
# Build swig

option(ITK_USE_SYSTEM_SWIG "Use system swig. If OFF, swig is built as an external project." OFF)
mark_as_advanced(ITK_USE_SYSTEM_SWIG)

# Minimal swig version
if(WIN32)
  set(swig_version_min 4.0.2)
  set(ITK_SWIG_VERSION 4.0.2)
  set(swigwin_hash  "b8f105f9b9db6acc1f6e3741990915b533cd1bc206eb9645fd6836457fd30789b7229d2e3219d8e35f2390605ade0fbca493ae162ec3b4bc4e428b57155db03d")
else()
  set(ITK_SWIG_VERSION 4.0.2)
  set(swig_version_min 4.0.2)
  set(swig_hash     "05e7da70ce6d9a733b96c0bcfa3c1b82765bd859f48c74759bbf4bb1467acb1809caa310cba5e2b3280cd704fca249eaa0624821dffae1d2a75097c7f55d14ed")
endif()

if(WIN32)
  set(swig_ep "${CMAKE_CURRENT_BINARY_DIR}/swigwin-${ITK_SWIG_VERSION}/swig.exe")
else()

    # follow the standard EP_PREFIX locations
    set ( swig_binary_dir ${CMAKE_CURRENT_BINARY_DIR}/swig-prefix/src/swig-build )
    set ( swig_source_dir ${CMAKE_CURRENT_BINARY_DIR}/swig-prefix/src/swig )
    set ( swig_install_dir ${CMAKE_CURRENT_BINARY_DIR}/swig )

  set(swig_ep "${swig_install_dir}/bin/swig")
endif()

if(ITK_USE_SYSTEM_SWIG)

  # the path set for the EP build prevents find_package to do its job
  if("${SWIG_EXECUTABLE}" STREQUAL "${swig_ep}")
    unset(SWIG_DIR CACHE)
    unset(SWIG_EXECUTABLE CACHE)
    unset(SWIG_VERSION CACHE)
  endif()

  # Look for system swig
  find_package(SWIG REQUIRED)

  # Check for the swig version
  if(${SWIG_VERSION} VERSION_LESS ${swig_version_min})
    message(WARNING "Swig version less than ${swig_version_min}: \"${SWIG_VERSION}\".")
  endif()

else()
  if(NOT TARGET swig)
    # Install swig ourselves
    set(SWIG_VERSION ${ITK_SWIG_VERSION})

    include(ExternalProject)
    if(WIN32)
      # If we are building ITK
      if(ITK_BINARY_DIR)
        itk_download_attempt_check(SWIG)
      endif()
      ExternalProject_Add(swig
        URL "https://data.kitware.com/api/v1/file/hashsum/sha512/${swigwin_hash}/download"
        URL_HASH SHA512=${swigwin_hash}
        SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/swigwin-${SWIG_VERSION}
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
        )
      set(SWIG_DIR ${CMAKE_CURRENT_BINARY_DIR}/swigwin-${SWIG_VERSION})
    else()
      # From PCRE configure
      # Some influential environment variables:
      #  CC          C compiler command
      #  CFLAGS      C compiler flags
      #  LDFLAGS     linker flags, e.g. -L<lib dir> if you have libraries in a
      #              nonstandard directory <lib dir>
      #  LIBS        libraries to pass to the linker, e.g. -l<library>
      #  CPPFLAGS    (Objective) C/C++ preprocessor flags, e.g. -I<include dir> if
      #              you have headers in a nonstandard directory <include dir>
      #  CXX         C++ compiler command
      #  CXXFLAGS    C++ compiler flags
      #  CPP         C preprocessor
      #  CXXCPP      C++ preprocessor

      # build swig as an external project

      # If we are building ITK
      if(ITK_BINARY_DIR)
        itk_download_attempt_check(PCRE)
      endif()
      set(pcre_env)
      if(NOT CMAKE_CROSSCOMPILING)
        set(pcre_env
          env
            "AR=${CMAKE_AR}"
            "CC=${CMAKE_C_COMPILER_LAUNCHER} ${CMAKE_C_COMPILER}"
            "CFLAGS=${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_RELEASE} -w"
            "LDFLAGS=$ENV{LDFLAGS}"
            "LIBS=$ENV{LIBS}"
            "CPPFLAGS=$ENV{CPPFLAGS}"
            "CXX=${CMAKE_CXX_COMPILER_LAUNCHER} ${CMAKE_CXX_COMPILER}"
            "CXXFLAGS=${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE} -w"
            "CPP=$ENV{CPP}"
            "CXXPP=$ENV{CXXPP}"
          )
        if(APPLE)
          # If building on OS X, the compiler must know what version of the OS X SDK to use
          # without SDKROOT set, configuring PCRE fails.  The deployment target is set to
          # ensure the built library is compatible with the correct OS X version.  This may
          # not be strictly necessary for configure, but the compiler determines which
          # header files to use based on both of these settings.  Adding it for safety.
          set(pcre_env ${pcre_env}
            "SDKROOT=${CMAKE_OSX_SYSROOT}"
            "MACOSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET}"
          )
        endif()
      endif()
      set(extra_external_project_commands)
      if(APPLE)
        # If building on OS X, we have to set the SDKROOT and DEPOLYMENT_TARGET environment variables
        # so that XCode's compilers know which version of the OS X SDK to use.
        list(APPEND extra_external_project_commands
          BUILD_COMMAND
            env
              "SDKROOT=${CMAKE_OSX_SYSROOT}"
               "MACOSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET}"
            make
          INSTALL_COMMAND
            env
              "SDKROOT=${CMAKE_OSX_SYSROOT}"
              "MACOSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET}"
            make
              install
        )
      endif()
      set(pcre_hash "abac4c4f9df9e61d7d7761a9c50843882611752e1df0842a54318f358c28f5953025eba2d78997d21ee690756b56cc9f1c04a5ed591dd60654cc78ba16d9ecfb")
      ExternalProject_Add(PCRE
        URL "https://data.kitware.com/api/v1/file/hashsum/sha512/${pcre_hash}/download"
        URL_HASH SHA512=${pcre_hash}
        CONFIGURE_COMMAND
          ${pcre_env}
          ../PCRE/configure
          --prefix=${CMAKE_CURRENT_BINARY_DIR}/PCRE
          --enable-shared=no
        ${extra_external_project_commands}
        )

      # swig uses bison find it by cmake and pass it down
      find_package ( BISON )
      set ( BISON_FLAGS "" CACHE STRING "Flags used by bison" )
      mark_as_advanced ( BISON_FLAGS )


      # From SWIG configure
      # Some influential environment variables:
      #  CC          C compiler command
      #  CFLAGS      C compiler flags
      #  LDFLAGS     linker flags, e.g. -L<lib dir> if you have libraries in a
      #              nonstandard directory <lib dir>
      #  LIBS        libraries to pass to the linker, e.g. -l<library>
      #  CPPFLAGS    (Objective) C/C++ preprocessor flags, e.g. -I<include dir> if
      #              you have headers in a nonstandard directory <include dir>
      #  CXX         C++ compiler command
      #  CXXFLAGS    C++ compiler flags
      #  CPP         C preprocessor
      #  PCRE_CONFIG config script used for pcre
      #  PCRE_CFLAGS CFLAGS used for pcre
      #  PCRE_LIBS   LIBS used for pcre
      #  YACC        The `Yet Another C Compiler' implementation to use. Defaults to
      #              the first program found out of: `bison -y', `byacc', `yacc'.
      #  YFLAGS      The list of arguments that will be passed by default to $YACC.
      #              This script will default YFLAGS to the empty string to avoid a
      #              default value of `-d' given by some make applications.

      # If we are building ITK
      if(ITK_BINARY_DIR)
        itk_download_attempt_check(SWIG)
      endif()
      # Swig configure step
      # Run in a CMake script because it will be flagged as a false-positive
      # warning when executed with CTEST_USE_LAUNCHERS
      set(swig_env)
      if(NOT CMAKE_CROSSCOMPILING)
        set(swig_env
"
set(ENV{CC} \"${CMAKE_C_COMPILER_LAUNCHER} ${CMAKE_C_COMPILER}\")
set(ENV{CFLAGS} \"${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_RELEASE} -w\")
set(ENV{LDFLAGS} \"$ENV{LDFLAGS}\")
set(ENV{LIBS} \"$ENV{LIBS}\")
set(ENV{CPPFLAGS} \"$ENV{CPPFLAGS}\")
set(ENV{CXX} \"${CMAKE_CXX_COMPILER_LAUNCHER} ${CMAKE_CXX_COMPILER}\")
set(ENV{CXXFLAGS} \"${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE} -w\")
set(ENV{CPP} \"$ENV{CPP}\")
set(ENV{YACC} \"${BISON_EXECUTABLE}\")
set(ENV{YFLAGS} \"${BISON_FLAGS}\")
"
          )
      endif()
      set(_swig_configure_script ${CMAKE_CURRENT_BINARY_DIR}/swig_configure_step.cmake)
      file(WRITE ${_swig_configure_script} "
      ${swig_env}
execute_process(COMMAND ../swig/configure
        \"--prefix=${CMAKE_CURRENT_BINARY_DIR}/swig\"
        \"--with-pcre-prefix=${CMAKE_CURRENT_BINARY_DIR}/PCRE\"
  WORKING_DIRECTORY \"${CMAKE_CURRENT_BINARY_DIR}/swig-prefix/src/swig-build\"
  RESULT_VARIABLE result
  OUTPUT_VARIABLE output
  ERROR_VARIABLE error
  )

set(output_file \"${CMAKE_CURRENT_BINARY_DIR}/swig_configure_output.txt\")
file(WRITE \${output_file} \${output})

set(error_file \"${CMAKE_CURRENT_BINARY_DIR}/swig_configure_error.txt\")
file(WRITE \${error_file} \${error})

if(NOT \${result} EQUAL 0)
  message(STATUS \"Swig configure errors detected - See below.\n\${output}\n\${error}\")
  message(FATAL_ERROR \"Swig configure error. See \${output_file} and \${error_file}\")
endif()

message(STATUS \"Swig configure successfully completed.\")
")
      set(extra_swig_configure_env)
      if(APPLE)
        # If building on OS X, the compiler must know what version of the OS X SDK to use
        # without SDKROOT set, configuring swig fails.  The deployment target is set to
        # ensure the built library is compatible with the correct OS X version.  This may
        # not be strictly necessary for configure, but the compiler determines which
        # header files to use based on both of these settings.  Adding it for safety.
        list(APPEND extra_swig_configure_env
          env
            "SDKROOT=${CMAKE_OSX_SYSROOT}"
            "MACOSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET}"
        )
      endif()

      ExternalProject_Add(swig
        URL "https://data.kitware.com/api/v1/file/hashsum/sha512/${swig_hash}/download"
        URL_HASH SHA512=${swig_hash}
        CONFIGURE_COMMAND
          ${extra_swig_configure_env} ${CMAKE_COMMAND} -P "${_swig_configure_script}"
        ${extra_external_project_commands}
        DEPENDS PCRE
        )
    endif()

    set(SWIG_DIR ${CMAKE_CURRENT_BINARY_DIR}/swig/share/swig/${SWIG_VERSION} CACHE FILEPATH "swig directory." FORCE)
    mark_as_advanced(SWIG_DIR)
    set(SWIG_EXECUTABLE ${swig_ep} CACHE FILEPATH "swig executable." FORCE)

  endif()
  mark_as_advanced(SWIG_EXECUTABLE)
endif()


set(PYGCCXML_DIR ${ITK_CMAKE_DIR}/../Modules/ThirdParty/pygccxml/src CACHE INTERNAL "pygccxml path")


###############################################################################
# store the current dir, so it can be reused later
set(ITK_WRAP_SWIGINTERFACE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" CACHE INTERNAL "swig interface source dir")
set(ITK_WRAP_SWIGINTERFACE_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}" CACHE INTERNAL "swig interface binary dir")

set(WRAPPER_MASTER_INDEX_OUTPUT_DIR "${ITK_DIR}/Wrapping/Typedefs" CACHE INTERNAL "typedefs dir")
set(IGENERATOR  "${CMAKE_CURRENT_SOURCE_DIR}/igenerator.py" CACHE INTERNAL "igenerator.py path" FORCE)

macro(itk_wrap_module_swig_interface library_name)
  # store the content of the mdx file
  set(SWIG_INTERFACE_MDX_CONTENT )
  # store the content of the .i file for the module - a set of import of all the .i files generated for the module
  set(SWIG_INTERFACE_MODULE_CONTENT )
  # build a list of modules to create the igenerator custom command in
  # itk_end_wrap_module_swig_interface
  set(SWIG_INTERFACE_MODULES )
endmacro()


macro(itk_end_wrap_module_swig_interface)
  # Loop over the extra swig input files and copy them to the Typedefs directory
  foreach(source ${WRAPPER_LIBRARY_SWIG_INPUTS})
    file(COPY "${source}"
         DESTINATION "${WRAPPER_MASTER_INDEX_OUTPUT_DIR}")
    get_filename_component(basename ${source} NAME)
    set(dest "${WRAPPER_MASTER_INDEX_OUTPUT_DIR}/${basename}")
  endforeach()

  # prepare dependencies
  set(DEPS )
  foreach(dep ${WRAPPER_LIBRARY_DEPENDS})
    set(SWIG_INTERFACE_MDX_CONTENT "${dep}.mdx\n${SWIG_INTERFACE_MDX_CONTENT}")
  endforeach()

  # add some libs required by this module
  set(swig_libs )
  foreach(swig_lib ${WRAPPER_SWIG_LIBRARY_FILES})
    get_filename_component(basename ${swig_lib} NAME)
    list(APPEND swig_libs --swig-include ${basename})
    file(COPY "${swig_lib}"
      DESTINATION "${WRAPPER_MASTER_INDEX_OUTPUT_DIR}")
    set(dest "${WRAPPER_MASTER_INDEX_OUTPUT_DIR}/${basename}")
  endforeach()

  # the list of files generated for the module
  set(i_files )
  set(xml_files )
  set(idx_files )
  set(typedef_in_files )
  set(typedef_files )
  set(mdx_file "${WRAPPER_MASTER_INDEX_OUTPUT_DIR}/${WRAPPER_LIBRARY_NAME}.mdx")
  set(module_interface_file "${WRAPPER_MASTER_INDEX_OUTPUT_DIR}/${WRAPPER_LIBRARY_NAME}.i")
  set(module_interface_ext_file "${WRAPPER_MASTER_INDEX_OUTPUT_DIR}/${WRAPPER_LIBRARY_NAME}_ext.i")

  if(${module_prefix}_WRAP_PYTHON)
    set(ITK_STUB_DIR "${ITK_DIR}/Wrapping/Generators/Python/itk-stubs")
    set(ITK_STUB_PYI_FILES)
  else()
    unset(ITK_STUB_DIR)
    unset(ITK_STUB_PYI_FILES)
  endif()

  foreach(module ${SWIG_INTERFACE_MODULES})
    # create the swig interface
    list(APPEND i_files "${WRAPPER_MASTER_INDEX_OUTPUT_DIR}/${module}.i")
    list(APPEND xml_files "${WRAPPER_LIBRARY_OUTPUT_DIR}/${module}.xml")
    list(APPEND idx_files "${WRAPPER_MASTER_INDEX_OUTPUT_DIR}/${module}.idx")
    list(APPEND typedef_in_files "${WRAPPER_LIBRARY_OUTPUT_DIR}/${module}SwigInterface.h.in")
    list(APPEND typedef_files "${WRAPPER_MASTER_INDEX_OUTPUT_DIR}/${module}SwigInterface.h")
    if(${module_prefix}_WRAP_PYTHON)
      list(APPEND ITK_STUB_PYI_FILES "${ITK_STUB_DIR}/${module}.pyi")
    endif()


    if(${module_prefix}_WRAP_EXPLICIT)
      list(APPEND opts --include "${WRAPPER_LIBRARY_NAME}Explicit.h")
    endif()
  endforeach()

  # the master idx file (mdx file)
  set(mdx_opts )
  set(deps_imports )

  list(APPEND mdx_opts --mdx "${WRAPPER_MASTER_INDEX_OUTPUT_DIR}/${WRAPPER_LIBRARY_NAME}.mdx")

  foreach(dep ${WRAPPER_LIBRARY_DEPENDS})
    list(APPEND mdx_opts --mdx "${WRAP_ITK_TYPEDEFS_DIRECTORY}/${dep}.mdx")
    list(APPEND deps_imports "%import ${dep}.i\n")
  endforeach()
  set(CONFIG_INDEX_FILE_CONTENT "${SWIG_INTERFACE_MDX_CONTENT}")
  configure_file("${ITK_WRAP_SWIGINTERFACE_SOURCE_DIR}/Master.mdx.in" "${mdx_file}"
     @ONLY)

  set(CONFIG_MODULE_INTERFACE_CONTENT ) #"${deps_imports}${SWIG_INTERFACE_MODULE_CONTENT}")
  configure_file("${ITK_WRAP_SWIGINTERFACE_SOURCE_DIR}/module.i.in" "${module_interface_file}"
    @ONLY)

  set(WRAPPING_CONFIG_WORKING_DIR "${ITK_DIR}/Wrapping/WorkingDirectory")
  list(LENGTH i_files number_interface_files)
  if(number_interface_files GREATER 0)

      FILE(MAKE_DIRECTORY "${WRAPPING_CONFIG_WORKING_DIR}")
      if(${module_prefix}_WRAP_PYTHON)
        set(ITK_STUB_DIR "${ITK_DIR}/Wrapping/Generators/Python/itk-stubs")
        set(ITK_STUB_PYI_FILE "${ITK_STUB_DIR}/${WRAPPER_LIBRARY_NAME}.pyi")
        # NOTE:  snake_case_config_file is both an input and an output to this command.
        #        the ${IGENERATOR} script appends to this file.
        # NOTE: The Configuration files should be placed in the itk package directory.
        set(ITK_WRAP_PYTHON_SNAKE_CONFIG_DIR
          "${WRAPPER_LIBRARY_OUTPUT_DIR}/Generators/Python/itk/Configuration"
        )
        set(snake_case_config_file
            "${ITK_WRAP_PYTHON_SNAKE_CONFIG_DIR}/${WRAPPER_LIBRARY_NAME}_snake_case.py")
        add_custom_command(
        OUTPUT ${i_files} ${typedef_files} ${idx_files} ${snake_case_config_file} ${ITK_STUB_PYI_FILES}
        COMMAND ${Python3_EXECUTABLE} ${IGENERATOR}
          ${mdx_opts}
          ${swig_libs}
          -w1 -w3 -w51 -w52 -w53 -w54
          -A protected -A private
          -p ${PYGCCXML_DIR}
          -g ${CASTXML_EXECUTABLE}
          --snake-case-file "${snake_case_config_file}"
          --interface-output-dir "${WRAPPER_MASTER_INDEX_OUTPUT_DIR}"
          --library-output-dir "${WRAPPER_LIBRARY_OUTPUT_DIR}"
          --submodule-order "${WRAPPER_SUBMODULE_ORDER}"
          --pyi_dir "${ITK_STUB_DIR}"
          DEPENDS ${ITK_WRAP_DOC_DOCSTRING_FILES} ${xml_files} ${IGENERATOR} ${typedef_in_files}
          WORKING_DIRECTORY "${WRAPPING_CONFIG_WORKING_DIR}" # Arguments to WORKING_DIRECTORY may use generator expressions
        VERBATIM
        )
      else()
        unset(ITK_STUB_DIR)
        unset(ITK_STUB_PYI_FILE)
        unset(snake_case_config_file)
        add_custom_command(
          OUTPUT ${i_files} ${typedef_files} ${idx_files}
          COMMAND ${Python3_EXECUTABLE} ${IGENERATOR}
            ${mdx_opts}
            ${swig_libs}
            -w1 -w3 -w51 -w52 -w53 -w54
            -A protected -A private
            -p ${PYGCCXML_DIR}
            -g ${CASTXML_EXECUTABLE}
            --interface-output-dir "${WRAPPER_MASTER_INDEX_OUTPUT_DIR}"
            --library-output-dir "${WRAPPER_LIBRARY_OUTPUT_DIR}"
            --submodule-order "${WRAPPER_SUBMODULE_ORDER}"
            DEPENDS ${ITK_WRAP_DOC_DOCSTRING_FILES} ${xml_files} ${IGENERATOR} ${typedef_in_files}
            WORKING_DIRECTORY "${WRAPPING_CONFIG_WORKING_DIR}" # Arguments to WORKING_DIRECTORY may use generator expressions
          VERBATIM
          )
      endif()

  endif()

  # the ${WRAPPER_LIBRARY_NAME}Swig target
  if(NOT TARGET ${WRAPPER_LIBRARY_NAME}Swig)
    add_custom_target(${WRAPPER_LIBRARY_NAME}Swig DEPENDS ${mdx_file} ${i_files} ${typedef_files} ${idx_files})
    add_dependencies(${WRAPPER_LIBRARY_NAME}Swig ${WRAPPER_LIBRARY_NAME}CastXML)
  endif()

  if(NOT EXTERNAL_WRAP_ITK_PROJECT)
    # don't depend on the targets from wrapitk in external projects
    foreach(dep ${WRAPPER_LIBRARY_DEPENDS})
      add_dependencies(${WRAPPER_LIBRARY_NAME}Swig ${dep}Swig)
    endforeach()
  endif()

  set(${WRAPPER_LIBRARY_NAME}IdxFiles ${idx_files} CACHE INTERNAL "Internal ${WRAPPER_LIBRARY_NAME}Idx file list.")
  set(${WRAPPER_LIBRARY_NAME}SwigFiles ${i_files} CACHE INTERNAL "Internal ${WRAPPER_LIBRARY_NAME}Swig file list.")

endmacro()


macro(itk_wrap_include_swig_interface include_file)
  list(APPEND SWIG_INTERFACE_INCLUDES ${include_file})
endmacro()


macro(itk_wrap_submodule_swig_interface module)
  # store the content of the SwigInterface.h files - a set of #includes for that module
  set(SWIG_INTERFACE_INCLUDES )
  # typedefs for swig
  set(SWIG_INTERFACE_TYPEDEFS )
endmacro()

macro(itk_end_wrap_submodule_swig_interface module)
  # variables used:
  # WRAPPER_LIBRARY_NAME
  # WRAPPER_LIBRARY_OUTPUT_DIR
  # WRAPPER_LIBRARY_DEPENDS
  # WRAPPER_MASTER_INDEX_OUTPUT_DIR
  # MODULE_INCLUDES

  set(SWIG_INTERFACE_INCLUDES_CONTENT )
  if(SWIG_INTERFACE_INCLUDES)
    list(REMOVE_DUPLICATES SWIG_INTERFACE_INCLUDES)
    foreach(include_file ${SWIG_INTERFACE_INCLUDES})
      if("${include_file}" MATCHES "<.*>")
        set(SWIG_INTERFACE_INCLUDES_CONTENT "${SWIG_INTERFACE_INCLUDES_CONTENT}#include ${include_file}\n")
      else()
        set(SWIG_INTERFACE_INCLUDES_CONTENT "${SWIG_INTERFACE_INCLUDES_CONTENT}#include \"${include_file}\"\n")
      endif()
    endforeach()
  endif()
  # create the file which store all the includes
  set(includes_file "${WRAPPER_LIBRARY_OUTPUT_DIR}/${module}SwigInterface.h.in")
  configure_file("${ITK_WRAP_SWIGINTERFACE_SOURCE_DIR}/module.includes.in"
     ${includes_file}
     @ONLY)

  # store the path of the idx file to store it in the mdx file
  set(SWIG_INTERFACE_MDX_CONTENT "${SWIG_INTERFACE_MDX_CONTENT}${module}.idx\n")

  set(SWIG_INTERFACE_MODULE_CONTENT "${SWIG_INTERFACE_MODULE_CONTENT}%import ${module}.i\n")

  list(APPEND SWIG_INTERFACE_MODULES ${module})
endmacro()


macro(itk_wrap_one_type_swig_interface wrap_method wrap_class swig_name)
  # Add one  typedef to WRAPPER_TYPEDEFS
  # 'wrap_method' is the one of the valid WRAPPER_WRAP_METHODS from itk_wrap_class,
  # 'wrap_class' is the fully-qualified C++ name of the class
  # 'swig_name' is what the swigged class should be called
  # The optional last argument is the template parameters that should go between
  # the < > brackets in the C++ template definition.
  # Only pass 3 parameters to wrap a non-templated class
  #
  # Global vars used: none
  # Global vars modified: WRAPPER_TYPEDEFS

  # get the base C++ class name (no namespaces) from wrap_class:
  string(REGEX REPLACE "(.*::)" "" base_name "${wrap_class}")

  set(wrap_pointer 0)
  set(template_parameters "${ARGV3}")
  if(template_parameters)
    set(full_class_name "${wrap_class}< ${template_parameters} >")
  else()
    set(full_class_name "${wrap_class}")
  endif()

  # itk_wrap_one_type_all_generators("${wrap_method}" "${wrap_class}" "${swig_name}" "${ARGV3}")

  # Add a typedef for the class. We have this funny looking full_name::base_name
  # thing (it expands to, for example "typedef itk::Foo<baz, 2>::Foo") used
  # for gccxml typedefs

  if("${wrap_method}" MATCHES "2_SUPERCLASSES")
    itk_wrap_simple_type_swig_interface("${full_class_name}::Superclass::Superclass" "${swig_name}_Superclass_Superclass")
    itk_wrap_simple_type_swig_interface("${full_class_name}::Superclass::Superclass::Pointer" "${swig_name}_Superclass_Superclass_Pointer")
  endif()

  if("${wrap_method}" MATCHES "SUPERCLASS")
    itk_wrap_simple_type_swig_interface("${full_class_name}::Superclass" "${swig_name}_Superclass")
    itk_wrap_simple_type_swig_interface("${full_class_name}::Superclass::Pointer" "${swig_name}_Superclass_Pointer")
  endif()

  if("${wrap_method}" MATCHES "CONST_POINTER")
    # add a const pointer typedef if we are so asked
    itk_wrap_simple_type_swig_interface("${full_class_name}::ConstPointer" "${swig_name}_ConstPointer")
  endif()

  # the same output with or without FORCE_INSTANTIATE
  itk_wrap_simple_type_swig_interface("${full_class_name}" "${swig_name}")

  if("${wrap_method}" MATCHES "POINTER")
    if("${wrap_method}" STREQUAL "AUTOPOINTER")
      # add a pointer typedef if we are so asked
      itk_wrap_simple_type_swig_interface("${full_class_name}::SelfAutoPointer" "${swig_name}_AutoPointer")
    else()
      # add a pointer typedef if we are so asked
      itk_wrap_simple_type_swig_interface("${full_class_name}::Pointer" "${swig_name}_Pointer")
    endif()
  endif()

endmacro()


macro(itk_wrap_simple_type_swig_interface wrap_class swig_name)
  set(SWIG_INTERFACE_TYPEDEFS "${SWIG_INTERFACE_TYPEDEFS}typedef ${wrap_class} ${swig_name};\n")
endmacro()

include_directories("${WRAPPER_MASTER_INDEX_OUTPUT_DIR}")
