set(RSA_SOURCES
    "app_main.c"
    "rsa_baseline.c"
    "rsa_student.c"
    "rsa_hw_accel.c"
)

if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/rsa_refans.c")
    list(APPEND RSA_SOURCES "rsa_refans.c")
    set(RSA_HAS_REFANS TRUE)
endif()

idf_component_register(
    SRCS
        ${RSA_SOURCES}
    INCLUDE_DIRS
        "."
    REQUIRES
        esp_timer
        mbedtls
)

if(RSA_HAS_REFANS)
    target_compile_definitions(${COMPONENT_LIB} PRIVATE RSA_ENABLE_REFANS=1)
endif()
