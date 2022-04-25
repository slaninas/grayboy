cmake_minimum_required(VERSION 3.16)

include_directories("../src/")

add_executable(tests-blargg tests-blargg.cc)
target_link_libraries(tests-blargg emulator)

add_test("01-special.gb" tests-blargg "${CMAKE_SOURCE_DIR}/gb-test-roms/cpu_instrs/individual/01-special.gb" "1500000" "01-special


Passed
")

add_test("02-interrupts.gb" tests-blargg "${CMAKE_SOURCE_DIR}/gb-test-roms/cpu_instrs/individual/02-interrupts.gb" "300000" "02-interrupts


Passed
")

add_test("03-op_sp,hl.gb" tests-blargg "${CMAKE_SOURCE_DIR}/gb-test-roms/cpu_instrs/individual/03-op sp,hl.gb" "1200000" "03-op sp,hl


Passed
")

add_test("04-op_r,imm.gb" tests-blargg "${CMAKE_SOURCE_DIR}/gb-test-roms/cpu_instrs/individual/04-op r,imm.gb" "1400000" "04-op r,imm


Passed
")

add_test("05-op_rp.gb" tests-blargg "${CMAKE_SOURCE_DIR}/gb-test-roms/cpu_instrs/individual/05-op rp.gb" "1900000" "05-op rp


Passed
")

add_test("06-ld_r,r.gb" tests-blargg "${CMAKE_SOURCE_DIR}/gb-test-roms/cpu_instrs/individual/06-ld r,r.gb" "290000" "06-ld r,r


Passed
")

add_test("07-jr,jp,call,ret,rst.gb" tests-blargg "${CMAKE_SOURCE_DIR}/gb-test-roms/cpu_instrs/individual/07-jr,jp,call,ret,rst.gb" "340000" "07-jr,jp,call,ret,rst


Passed
")

add_test("08-misc_instrs.gb" tests-blargg "${CMAKE_SOURCE_DIR}/gb-test-roms/cpu_instrs/individual/08-misc instrs.gb" "270000" "08-misc instrs


Passed
")

add_test("09-op_r,r.gb" tests-blargg "${CMAKE_SOURCE_DIR}/gb-test-roms/cpu_instrs/individual/09-op r,r.gb" "4500000" "09-op r,r


Passed
")

add_test("10-bit_ops.gb" tests-blargg "${CMAKE_SOURCE_DIR}/gb-test-roms/cpu_instrs/individual/10-bit ops.gb" "6800000" "10-bit ops


Passed
")

add_test("11-op_a,hl.gb" tests-blargg "${CMAKE_SOURCE_DIR}/gb-test-roms/cpu_instrs/individual/11-op a,(hl).gb" "7500000" "11-op a,(hl)


Passed
")

# TODO: Check how much instructions is needed to run
add_test("instr_timing" tests-blargg "${CMAKE_SOURCE_DIR}/gb-test-roms/instr_timing/instr_timing.gb" "300000" "instr_timing


Passed
")
