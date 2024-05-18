add_executable(
	WebApp
	WebApp.cpp
)

target_include_directories(
	WebApp PUBLIC
	../dependencies/inja/include
	../dependencies/inja/third_party/include
)

find_package(jwt-cpp CONFIG REQUIRED)

target_link_libraries(
	WebApp PUBLIC
	bserv
	jwt-cpp::jwt-cpp
)
