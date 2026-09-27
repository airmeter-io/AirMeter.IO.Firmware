cmake_minimum_required(VERSION 3.16)

# ESP-IDF's wifi_provisioning component is not used by this project (it has
# its own captive-portal provisioning) and does not compile when every
# protocomm security version is disabled in sdkconfig, as it is here.
set(EXCLUDE_COMPONENTS wifi_provisioning)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(main)
