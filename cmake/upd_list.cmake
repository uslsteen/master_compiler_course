macro(UPD_LIST TARGET LIST)
    list(APPEND ${LIST} ${TARGET})
    set(${LIST} ${${LIST}} PARENT_SCOPE)
endmacro()