
function(ListSources sources_list full_list)
    if (NOT ${full_list})
        set (${full_list} "" PARENT_SCOPE)
    endif()

    set(localList "")
    list(LENGTH ${sources_list} LIST_LEN)
    if(LIST_LEN GREATER 0)
        foreach(srcFile ${${sources_list}})
            set (localList "${CMAKE_CURRENT_LIST_DIR}/${srcFile}" ${localList})
        endforeach(srcFile)
    
        set (${full_list} ${localList} ${${full_list}} PARENT_SCOPE)
    endif(LIST_LEN GREATER 0)
endfunction(ListSources)

macro(ListFolders folder_list)
    foreach(folder ${${folder_list}})
        include("${CMAKE_CURRENT_LIST_DIR}/${folder}/SourcesList.cmake")
    endforeach(folder)
endmacro(ListFolders)

macro(ParseGroups sources_list)
    foreach(srcFile ${${sources_list}})
        if (${srcFile} MATCHES ".cxx" OR ${srcFile} MATCHES "ui_.*\\.h")
            source_group("Generated Files" FILES "${srcFile}")
        else ()
            string (REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" locaPath "${srcFile}")
            string (REGEX REPLACE "(.*)/[a-zA-Z0-9]*.[a-zA-Z0-9]*" "\\1" path "${locaPath}" )
            string (REPLACE "/" "\\\\" group "${path}")     
            source_group("${group}" FILES "${srcFile}")
        endif ()
    endforeach(srcFile)
endmacro(ParseGroups)
