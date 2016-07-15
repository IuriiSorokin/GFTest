# - find a ROOT installation
# it defines the following variables
# ROOT_FOUND         - if ROOT was found
# ROOT_INCLUDE_DIRS  - include directories for ROOT
# ROOT_LIBRARIES     - common libraries to link against
# ROOT_GUI_LIBRARIES - gui and common libraries to link agains

include(CMakeParseArguments)
include(FindPackageHandleStandardArgs)

find_program(ROOT_CONFIG_EXECUTABLE root-config PATHS $ENV{ROOTSYS}/bin)
find_program(ROOT_CINT_EXECUTABLE rootcint PATHS $ENV{ROOTSYS}/bin)

if(NOT ROOT_CONFIG_EXECUTABLE)
    set(ROOT_INCLUDE_DIR   ROOT_INCLUDE_DIR-NOTFOUND)
    set(ROOT_LIBRARIES     ROOT_LIBRARIES-NOTFOUND)
    set(ROOT_GUI_LIBRARIES ROOT_GUI_LIBRARIES-NOTFOUND)
    set(ROOT_VERSION       ROOT_VERSION-NOTFOUND)
else()
    execute_process(
        COMMAND ${ROOT_CONFIG_EXECUTABLE} --incdir
        OUTPUT_VARIABLE ROOT_INCLUDE_DIR
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(
        COMMAND ${ROOT_CONFIG_EXECUTABLE} --libdir
        OUTPUT_VARIABLE ROOT_LIBRARY_DIR
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(
        COMMAND ${ROOT_CONFIG_EXECUTABLE} --libs
        OUTPUT_VARIABLE ROOT_LIBRARIES
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(
        COMMAND ${ROOT_CONFIG_EXECUTABLE} --glibs
        OUTPUT_VARIABLE ROOT_GUI_LIBRARIES
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(
        COMMAND ${ROOT_CONFIG_EXECUTABLE} --version 
        OUTPUT_VARIABLE ROOT_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE)

    # test additional ROOT components
    foreach(_component_name ${ROOT_FIND_COMPONENTS})
        # see if the feature exists
        string(TOLOWER ${_component_name} _component_name_lower)
        execute_process(
            COMMAND ${ROOT_CONFIG_EXECUTABLE} --has-${_component_name_lower}
            OUTPUT_VARIABLE _component_found
            OUTPUT_STRIP_TRAILING_WHITESPACE)
        # see if there is also a corresponding library that we need to link to
        find_library(
            _component_lib ${_component_name}
            HINTS ${ROOT_LIBRARY_DIR})
        # add requested components to the list of libraries
        if(_component_lib)
            set(ROOT_LIBRARIES "${ROOT_LIBRARIES} ${_component_lib}")
            set(ROOT_GUI_LIBRARIES "${ROOT_GUI_LIBRARIES} ${_component_lib}")
        endif()
        # set per-component output variables
        set(ROOT_${_component_name}_FOUND ${_component_found})
        set(ROOT_${_component_name}_LIBRARY ${_component_lib})
    endforeach()
endif()

find_package_handle_standard_args(
    ROOT
    REQUIRED_VARS ROOT_CONFIG_EXECUTABLE
                  ROOT_INCLUDE_DIR
                  ROOT_LIBRARY_DIR
                  ROOT_LIBRARIES
                  ROOT_GUI_LIBRARIES
    VERSION_VAR ROOT_VERSION
    HANDLE_COMPONENTS)

set(ROOT_INCLUDE_DIRS ${ROOT_INCLUDE_DIR})

mark_as_advanced(ROOT_CONFIG_EXECUTABLE
                 ROOT_CINT_EXECUTABLE
                 ROOT_INCLUDE_DIR
                 ROOT_LIBRARY_DIR)

#----------------------------------------------------------------------------
# function ROOT_GENERATE_DICTIONARY( dictionary   
#                                    header1 header2 ... 
#                                    LINKDEF linkdef1 ... 
#                                    OPTIONS opt1...)
function(ROOT_GENERATE_DICTIONARY dictionary)
  CMAKE_PARSE_ARGUMENTS(ARG "" "" "LINKDEF;OPTIONS" "" ${ARGN})
  #---Get the list of header files-------------------------
  set(headerfiles)
  foreach(fp ${ARG_UNPARSED_ARGUMENTS})
    file(GLOB files ${fp})
    if(files)
      foreach(f ${files})
        if(NOT f MATCHES LinkDef)
          set(headerfiles ${headerfiles} ${f})
        endif()
      endforeach()
    else()
      set(headerfiles ${headerfiles} ${fp})
    endif()
  endforeach()
  #---Get the list of include directories------------------
  get_directory_property(incdirs INCLUDE_DIRECTORIES)
  # HACK rootcint seems to have a problem w/ '/usr/include' as an argument
  list(REMOVE_ITEM incdirs "/usr/include")
  set(includedirs)
  foreach( d ${incdirs})
   set(includedirs ${includedirs} -I${d})
  endforeach()
  #---Get LinkDef.h file------------------------------------
  set(linkdefs)
  foreach( f ${ARG_LINKDEF})
    if( IS_ABSOLUTE ${f})
      set(linkdefs ${linkdefs} ${f})
    else() 
      if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/inc/${f})
        set(linkdefs ${linkdefs} ${CMAKE_CURRENT_SOURCE_DIR}/inc/${f})
      else()
        set(linkdefs ${linkdefs} ${CMAKE_CURRENT_SOURCE_DIR}/${f})
      endif()
    endif()
  endforeach()
  #---call rootcint------------------------------------------
  add_custom_command(
    OUTPUT ${dictionary}.cxx ${dictionary}.h
    COMMAND ${ROOT_CINT_EXECUTABLE} -cint -f ${dictionary}.cxx
            -c ${ARG_OPTIONS} ${includedirs} ${headerfiles} ${linkdefs}
    DEPENDS ${headerfiles} ${linkdefs})
endfunction()
