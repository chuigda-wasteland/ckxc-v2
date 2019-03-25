#ifndef DECLFWD_HPP
#define DECLFWD_HPP

namespace ckx {
namespace AST {

class Decl;
class DeclContext;

class EmptyDecl;
/// @todo implement them once we can
// class ImportDecl;
// class ModuleDecl;
class TransUnitDecl; // : public DeclContext
class NamedDecl;
class LabelDecl;
class TypeDecl;
class TagDecl;
class ClassDecl; // : public DeclContext
class EnumDecl;  // : public DeclContext
class EnumClassDecl;
class EnumClassInternDecl;
class UsingDecl;
class ValuedDecl;
class FuncDecl; // : public DeclContext
class VarDecl;
class FieldDecl;
class ParamDecl;
class EnumeratorDecl;

/// @todo and_then fix DeclBase.hpp

} // namespace AST
} // namespace ckx

#endif // DECLFWD_HPP
