woth_unittests = executable(
    'woth_unittests',
    sources: [
        'src/main.cpp',
        'src/woth/mana/mana_type_tests.cpp',
        'src/woth/string/algorithms/join_tests.cpp',
        'src/woth/string/algorithms/split_tests.cpp',
        'src/woth/string/algorithms/trim_tests.cpp',
    ],
    dependencies: [wthlib_dep, jge_dep, unity_cpp_dep],
)

test('woth unittests', woth_unittests)
