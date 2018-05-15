
add_executable(stripsbin main.c)
target_link_libraries(stripsbin strips strips-io strips-curl strips-fs)

set_target_properties(stripsbin PROPERTIES 
    OUTPUT_NAME "strips"
)


add_executable(stripsbin2 main.cpp)
target_link_libraries(stripsbin2 strips strips++ strips-io strips-curl strips-fs csystem++)

set_target_properties(stripsbin2 PROPERTIES 
    OUTPUT_NAME "zap"
)

