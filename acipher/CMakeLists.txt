project (optee_example_acipher C)
add_library(image_loader INTERFACE)
target_sources(image_loader INTERFACE /home/yl9ds/optee-qemuv8/optee_examples/acipher/sod_img_reader.h)
set (SRC host/main.c)

add_executable (${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} PRIVATE image_loader)
target_include_directories(${PROJECT_NAME}
			   PRIVATE ta/include
			   PRIVATE include
			   PUBLIC /home/yl9ds/optee-qemuv8/optee_examples/acipher/ta/include)

target_link_libraries (${PROJECT_NAME} PRIVATE teec)
	
install (TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})
