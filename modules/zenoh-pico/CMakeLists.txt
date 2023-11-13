# SPDX-License-Identifier: Apache-2.0
if (CONFIG_ZENOH_PICO)

zephyr_library_named(zenoh-pico)

target_compile_definitions(zenoh-pico PUBLIC
  ZENOH_ZEPHYR
  ZENOH_DEBUG=3
  Z_FEATURE_QUERYABLE=1
  Z_FEATURE_QUERY=1
  Z_FEATURE_SUBSCRIPTION=1
  Z_FEATURE_PUBLICATION=1
  Z_MULTI_THREAD=1
  Z_DYNAMIC_MEMORY_ALLOCATION=1
  Z_LINK_TCP=1
  Z_LINK_UDP_UNICAST=1
  Z_LINK_SERIAL=0
  Z_LINK_WS=0
  Z_LINK_BLUETOOTH=0
  Z_SCOUTING_UDP=1
  Z_LINK_UDP_MULTICAST=1
  )
target_compile_options(zenoh-pico PRIVATE -Wno-unused-variable -Wno-implicit-function-declaration -Wno-int-conversion)

zephyr_include_directories(
  ${ZEPHYR_CURRENT_MODULE_DIR}/include
  )

zephyr_library_include_directories(
  ${ZEPHYR_CURRENT_MODULE_DIR}/include
  )

file(GLOB_RECURSE Sources
  "${ZEPHYR_CURRENT_MODULE_DIR}/src/api/*.c"
  "${ZEPHYR_CURRENT_MODULE_DIR}/src/collections/*.c"
  "${ZEPHYR_CURRENT_MODULE_DIR}/src/link/*.c"
  "${ZEPHYR_CURRENT_MODULE_DIR}/src/net/*.c"
  "${ZEPHYR_CURRENT_MODULE_DIR}/src/protocol/*.c"
  "${ZEPHYR_CURRENT_MODULE_DIR}/src/session/*.c"
  "${ZEPHYR_CURRENT_MODULE_DIR}/src/transport/*.c"
  "${ZEPHYR_CURRENT_MODULE_DIR}/src/utils/*.c"
  "${ZEPHYR_CURRENT_MODULE_DIR}/src/system/zephyr/*.c"
)

zephyr_library_sources(
  ${Sources}
)

endif()
