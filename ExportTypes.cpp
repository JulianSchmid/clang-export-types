// Declares clang::SyntaxOnlyAction.
#include "clang/AST/ASTConsumer.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
// Declares llvm::cl::extrahelp.
#include "llvm/Support/CommandLine.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/AST/TypeVisitor.h"

using namespace clang;
using namespace clang::ast_matchers;

const char* getBuiltinName(const BuiltinType* builtinType) {
    switch(builtinType->getKind()) {
        
        case BuiltinType::Bool: // 'bool' in C++, '_Bool' in C99
            return "bool";
        case BuiltinType::Char_U: // 'char' for targets where it's unsigned
            return "char";
        case BuiltinType::UChar: // 'unsigned char', explicitly qualified
            return "unsigned char";
        case BuiltinType::WChar_U: // 'wchar_t' for targets where it's unsigned
            return "wchar_t";
        case BuiltinType::Char8: // 'char8_t' in C++20 (proposed)
            return "char8_t";
        case BuiltinType::Char16: // 'char16_t' in C++
            return "char16_t";
        case BuiltinType::Char32: // 'char32_t' in C++
            return "char32_t";
        case BuiltinType::UShort: // 'unsigned short'
            return "unsigned short";
        case BuiltinType::UInt: // 'unsigned int'
            return "unsigned int";
        case BuiltinType::ULong: // 'unsigned long'
            return "unsigned long";
        case BuiltinType::ULongLong: // 'unsigned long long'
            return "unsigned long long";
        case BuiltinType::UInt128: // '__uint128_t'
            return "__uint128_t";
        case BuiltinType::Char_S: // 'char' for targets where it's signed
            return "char";
        case BuiltinType::SChar: // 'signed char', explicitly qualified
            return "signed char";
        case BuiltinType::WChar_S: // 'wchar_t' for targets where it's signed
            return "wchar_t";
        case BuiltinType::Short: // 'short' or 'signed short'
            return "short";
        case BuiltinType::Int: // 'int' or 'signed int'
            return "int";
        case BuiltinType::Long: // 'long' or 'signed long'
            return "long";
        case BuiltinType::LongLong: // 'long long' or 'signed long long'
            return "long long";
        case BuiltinType::Int128: // '__int128_t'
            return "__int128_t";
        case BuiltinType::Float: // 'float'
            return "float";
        case BuiltinType::Double: // 'double'
            return "double";
        case BuiltinType::LongDouble: // 'long double'
            return "long double";

        case BuiltinType::Void:
        case BuiltinType::Half:
        case BuiltinType::Float16:
        case BuiltinType::Float128:
        default:
        {
            //TODO throw error -> unsupported type
            return "UNSUPPORTED";
        }
    }
}

struct TypedumpVisitor : public TypeVisitor<TypedumpVisitor,void> {

    size_t indent;
    ASTContext* context;

    TypedumpVisitor() = delete;
    TypedumpVisitor(ASTContext* context) :
        indent(0),
        context(context) {}

    void VisitBuiltinType(const BuiltinType* b) {
        Indent();
        llvm::outs() << getBuiltinName(b) << "\n";
    }

    void VisitRecordType(const RecordType* r) {
        const auto* decl = r->getDecl();
        switch(decl->getTagKind()) {
            case TTK_Struct:
            case TTK_Class:
            {
                Indent();
                llvm::outs() << "struct/class\n";
                indent += 1;
                for (auto it = decl->field_begin(); it != decl->field_end(); ++it)
                {
                    Indent();
                    llvm::outs() << it->getName();
                    if (it->isBitField()) {
                        Expr::EvalResult bitwidthEval;

                        if(it->getBitWidth()->EvaluateAsInt(bitwidthEval, *context)) {
                            llvm::outs() << " : " << bitwidthEval.Val.getInt();
                        } else {
                            //TODO: throw error
                            llvm::outs() << " : ERROR READING BITFIELD VALUE";
                        }
                    }
                    llvm::outs() << "\n";
                    indent += 1;

                    Visit(it->getType().getTypePtr());
                    indent -= 1;
                }
                indent -= 1;
            } break;
            case TTK_Union:
            {
                Indent();
                llvm::outs() << "union\n";
                indent += 1;
                for (auto it = decl->field_begin(); it != decl->field_end(); ++it)
                {
                    Indent();
                    llvm::outs() << it->getName() << "\n";
                    indent += 1;

                    Visit(it->getType().getTypePtr());
                    indent -= 1;
                }
                //Visit(decl->field_begin()->getType().getTypePtr());
                indent -= 1;

            } break;
            default:
            {
                Indent();
                llvm::outs() << "UNSUPPORTED RECORD TYPE\n";
                decl->dump();
            }
        }
    }

    void VisitConstantArrayType(const ConstantArrayType* arr) {
        Indent();
        llvm::outs() << "constant array[" << arr->getSize() << "]\n";
        indent += 1;
        Visit(arr->getElementType().getTypePtr());
        indent -= 1;
    }

    void VisitEnumType(const EnumType* t) {
        
        const auto* decl = t->getDecl();
        if (decl->isFixed()) {
            //fixed type enum (aka final type was manually defined)
            Indent();
            llvm::outs() << "enum : " << getBuiltinName(decl->getIntegerType().getTypePtr()->getAs<BuiltinType>()) << "\n";
        } else {
            //enum type defined by the compiler configuration
            Indent();
            llvm::outs() << "enum\n";
        }
    }

    void VisitTemplateSpecializationType(const TemplateSpecializationType* t) {
        Visit(t->desugar().getTypePtr());
    }

    void VisitElaboratedType(const ElaboratedType* e) {
        Visit(e->desugar().getTypePtr());
    }

    //not supported elements
    void VisitPointerType(const PointerType* p) {
        Indent();
        llvm::outs() << "POINTER -> not supported\n";
        //TODO error
    }

    void VisitLValueReferenceType(const LValueReferenceType* r) {
        Indent();
        llvm::outs() << "REFERENCE -> not supported\n";
        //TODO error
    }

    void VisitType(const Type* t) {
        Indent();
        llvm::outs() << "UNSUPPORTED TYPE\n";
        t->dump();
    }

    void Indent() const {
        for (size_t i = 0; i < indent; ++i) {
            llvm::outs() << "  ";
        }
    }
};

//TODO:
// - get type name

//WIP How to handle serialization based on the types:
// - padding issues -> different compilers & targets have different padding behaviours
//   solutions:
//     - enforce "non padded" structs & classes
//     - generate "serialize" methods that generate deterministic serialization functions
//        - potentially slower
//        - some platforms dont allow "non aligned" copies ... -> complicates code

DeclarationMatcher templateMatcher = functionTemplateDecl().bind("templatedFunc");

class FuncPrinter : public MatchFinder::MatchCallback {
public :
    virtual void run(const MatchFinder::MatchResult& result) {
        ASTContext* context = result.Context;
        if (const FunctionTemplateDecl* fs = result.Nodes.getNodeAs<clang::FunctionTemplateDecl>("templatedFunc")) {
            for (auto it = fs->spec_begin(); it < fs->spec_end(); ++it) {
                //it->dump();
                auto* list = it->getTemplateSpecializationArgs();
                for (size_t k = 0; k < list->size(); ++k) {
                    const auto& arg = list->get(k);
                    using ArgKind = clang::TemplateArgument::ArgKind;
                    switch (arg.getKind()) {
                        case ArgKind::Null:
                        {
                            llvm::outs() << "Null\n";
                            break;
                        }
                        case ArgKind::Type:
                        {
                            TypedumpVisitor visitor(context);
                            visitor.Visit(arg.getAsType().getTypePtr());
                            break;
                        }
                        case ArgKind::Declaration:
                        {
                            llvm::outs() << "Declaration\n";
                            break;
                        }
                        case ArgKind::NullPtr:
                        {
                            llvm::outs() << "NullPtr\n";
                            break;
                        }
                        case ArgKind::Integral:
                        {
                            llvm::outs() << "Integral: " << arg.getAsIntegral().getExtValue() << "\n";
                            break;
                        }
                        case ArgKind::Template:
                        {
                            llvm::outs() << "Template\n";
                            break;
                        }
                        case ArgKind::TemplateExpansion:
                        {
                            llvm::outs() << "TemplateExpansion\n";
                            break;
                        }
                        case ArgKind::Expression:
                        {
                            llvm::outs() << "Expression\n";
                            break;
                        }
                        case ArgKind::Pack:
                        {
                            llvm::outs() << "Pack\n";
                            break;
                        }
                    }
                    //list->get(k).dump();
                }
            }
        }
    }
};

using namespace clang::tooling;
using namespace llvm;

// Apply a custom category to all command-line options so that they are the
// only ones displayed.
static llvm::cl::OptionCategory myToolCategory("my-tool options");

// CommonOptionsParser declares HelpMessage with a description of the common
// command-line options related to the compilation database and input files.
// It's nice to have this help message in all tools.
static cl::extrahelp commonHelp(CommonOptionsParser::HelpMessage);

// A help message for this specific tool can be added afterwards.
static cl::extrahelp moreHelp("\nMore help text...\n");

int main(int argc, const char **argv) {
    CommonOptionsParser optionsParser(argc, argv, myToolCategory);
    ClangTool tool(optionsParser.getCompilations(),
                   optionsParser.getSourcePathList());

    FuncPrinter printer;
    MatchFinder finder;
    finder.addMatcher(templateMatcher, &printer);

    return tool.run(newFrontendActionFactory(&finder).get());
}