set(LLVM_LINK_COMPONENTS support)

add_clang_executable(clang-typeexport
  TypeExport.cpp
)

target_link_libraries(clang-typeexport
  PRIVATE
  clangTooling
  clangBasic
  clangASTMatchers
)