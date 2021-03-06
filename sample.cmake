MACRO(CHECK_DEFINED VAR)
    IF (NOT DEFINED ${VAR})
        MESSAGE(FATAL_ERROR "NOT DEFINED ${VAR}")
    ENDIF()
ENDMACRO()

CHECK_DEFINED(SERVER_WORKDIR)
CHECK_DEFINED(SERVER_NODENUM)

EXECUTE_PROCESS(
        COMMAND python ${CMAKE_CURRENT_LIST_DIR}/config/sample.py ${SERVER_WORKDIR} ${SERVER_NODENUM}
        ERROR_VARIABLE COMMAND_ERROR_RESULT
)

IF (COMMAND_ERROR_RESULT)
    MESSAGE(FATAL_ERROR COMMAND_ERROR_RESULT)
ENDIF()

