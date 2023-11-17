#include <clang/AST/ASTConsumer.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/FrontendAction.h>
#include <clang/Tooling/Tooling.h>

using namespace clang;

class FunctionCallVisitor : public RecursiveASTVisitor<FunctionCallVisitor> {
public:
  explicit FunctionCallVisitor(ASTContext *Context) : Context(Context) {}

  bool VisitCallExpr(CallExpr *E) {
    if (FunctionDecl *FD = E->getDirectCallee()) {
      FullSourceLoc FullLocation = Context->getFullLoc(E->getBeginLoc());
      if (FullLocation.isValid())
        llvm::outs() << "In function: '" << FD->getNameInfo().getAsString()
                     << "' at "
                     << FullLocation.getSpellingLineNumber() << ":"
                     << FullLocation.getSpellingColumnNumber() << "\n";
    }

    return true;
  }

private:
  ASTContext *Context;
};

class FunctionCallConsumer : public clang::ASTConsumer {
public:
  explicit FunctionCallConsumer(ASTContext *Context) : Visitor(Context) {}

  virtual void HandleTranslationUnit(clang::ASTContext &Context) {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }

private:
  FunctionCallVisitor Visitor;
};

class FunctionCallAction : public clang::ASTFrontendAction {
public:
  virtual std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &Compiler, llvm::StringRef InFile) {
    return std::unique_ptr<clang::ASTConsumer>(
        new FunctionCallConsumer(&Compiler.getASTContext()));
  }
};

int main(int argc, char **argv) {
  if (argc > 1) {
    clang::tooling::runToolOnCode(new FunctionCallAction, argv[1]);
  }

  return 0;
}
