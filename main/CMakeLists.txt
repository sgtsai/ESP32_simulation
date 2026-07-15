set(RSA_SOURCES
    "app_main.c"
    "rsa_baseline.c"
    "rsa_student.c"
)
set(RSA_REQUIRES
    esp_timer
)

if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/rsa_private.cmake")
    include("${CMAKE_CURRENT_LIST_DIR}/rsa_private.cmake")
elseif(EXISTS "${CMAKE_CURRENT_LIST_DIR}/rsa_refans.c")
    list(APPEND RSA_SOURCES "rsa_refans.c")
    set(RSA_HAS_REFANS TRUE)
endif()

idf_component_register(
    SRCS
        ${RSA_SOURCES}
      INCLUDE_DIRS
          "."
      REQUIRES
          ${RSA_REQUIRES}
)

if(RSA_HAS_REFANS)
    target_compile_definitions(${COMPONENT_LIB} PRIVATE RSA_ENABLE_REFANS=1)
endif()
