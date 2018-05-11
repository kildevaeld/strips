
set(SOURCES
    curl.c
    #client.c
    request.c
    curl-request.c
    utils.c
    #form.c
    #header.c
)

add_library(strips-curl ${SOURCES})
target_link_libraries(strips-curl strips strips-io curl)