# Add a selection of warnings to a target
macro(cppsort_add_warnings target)
    target_compile_options(${target} PRIVATE
        -Wall -Wextra -Wcast-align -Winline -Wmissing-declarations -Wmissing-include-dirs
        -Wnon-virtual-dtor -Wodr -Wpedantic -Wredundant-decls -Wundef -Wunreachable-code
        -Wdouble-promotion
        $<$<CXX_COMPILER_ID:GNU>:-Wlogical-op -Wuseless-cast -Wzero-as-null-pointer-constant>
        $<$<CXX_COMPILER_ID:GNU>:-Wduplicated-cond -Wduplicated-branches>
    )
endmacro()
