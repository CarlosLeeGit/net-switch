#ifndef NET_SWITCH_PREFIX_TREE_H_
#define NET_SWITCH_PREFIX_TREE_H_

#include <list>
#include <memory>
#include <unordered_map>

#include "status.h"

namespace netswitch {
namespace base {

class PrefixNode;

class PrefixTree {
 public:
  PrefixTree(const std::string &split_token);

  virtual ~PrefixTree() = default;

  Status AddNode(const std::string &node_path);

  void DelNode(const std::string &node_path);

  Status Match(const std::string &path, std::string &node_path);

 private:
  std::list<std::string> SplitPath(const std::string &match_result);
  std::shared_ptr<PrefixNode> root_;
  std::string split_token_;
};
}  // namespace base
}  // namespace netswitch

#endif  // NET_SWITCH_PREFIX_TREE_H_