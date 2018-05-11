
add_executable(stripsbin main.c)
target_link_libraries(stripsbin strips strips-io strips-curl strips-fs)

set_target_properties(stripsbin PROPERTIES 
    OUTPUT_NAME "strips"
)

