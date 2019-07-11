set(LLVM_LINK_COMPONENTS support)

add_clang_executable(clang-export-types
  TypeExport.cpp
)

target_link_libraries(clang-export-types
  PRIVATE
  clangTooling
  clangBasic
  clangASTMatchers
)