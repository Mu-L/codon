#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "codon/compiler/error.h"
#include "codon/dsl/plugins.h"
#include "codon/parser/cache.h"
#include "codon/sir/llvm/llvisitor.h"
#include "codon/sir/module.h"
#include "codon/sir/transform/manager.h"

namespace codon {

class Compiler {
public:
  enum Mode {
    DEBUG,
    RELEASE,
    JIT,
  };

private:
  std::string argv0;
  bool debug;
  std::string input;
  std::unique_ptr<PluginManager> plm;
  std::unique_ptr<ast::Cache> cache;
  std::unique_ptr<ir::Module> module;
  std::unique_ptr<ir::transform::PassManager> pm;
  std::unique_ptr<ir::LLVMVisitor> llvisitor;

  llvm::Error parse(bool isCode, const std::string &file, const std::string &code,
                    int startLine, int testFlags,
                    const std::unordered_map<std::string, std::string> &defines);

public:
  Compiler(const std::string &argv0, Mode mode,
           const std::vector<std::string> &disabledPasses = {}, bool isTest = false);

  explicit Compiler(const std::string &argv0, bool debug = false,
                    const std::vector<std::string> &disabledPasses = {},
                    bool isTest = false)
      : Compiler(argv0, debug ? Mode::DEBUG : Mode::RELEASE, disabledPasses, isTest) {}

  std::string getInput() const { return input; }
  PluginManager *getPluginManager() const { return plm.get(); }
  ast::Cache *getCache() const { return cache.get(); }
  ir::Module *getModule() const { return module.get(); }
  ir::transform::PassManager *getPassManager() const { return pm.get(); }
  ir::LLVMVisitor *getLLVMVisitor() const { return llvisitor.get(); }

  llvm::Error load(const std::string &plugin);
  llvm::Error
  parseFile(const std::string &file, int testFlags = 0,
            const std::unordered_map<std::string, std::string> &defines = {});
  llvm::Error
  parseCode(const std::string &file, const std::string &code, int startLine = 0,
            int testFlags = 0,
            const std::unordered_map<std::string, std::string> &defines = {});
  llvm::Error compile();
  llvm::Expected<std::string> docgen(const std::vector<std::string> &files);
};

} // namespace codon