#include "netswitch/base/prefix_tree.h"

#include <sstream>

namespace netswitch {
namespace base {

class PrefixNode {
 public:
  Status AddChild(std::list<std::string> node_path);

  Status DelChild(std::list<std::string> node_path);

  Status Match(std::list<std::string> path, std::list<std::string> &node_path);

  bool IsValid();

  bool IsEmpty();

 private:
  bool is_valid_{false};
  std::unordered_map<std::string, std::shared_ptr<PrefixNode>> children_;
};

Status PrefixNode::AddChild(std::list<std::string> node_path) {
  if (node_path.empty()) {
    if (is_valid_) {
      return STATUS_EXIST;
    }

    is_valid_ = true;
    return STATUS_OK;
  }

  auto child_name = node_path.front();
  node_path.pop_front();
  std::shared_ptr<PrefixNode> child;
  auto item = children_.find(child_name);
  if (item == children_.end()) {
    child = std::make_shared<PrefixNode>();
    children_[child_name] = child;
  } else {
    child = item->second;
  }

  return child->AddChild(std::move(node_path));
}

Status PrefixNode::DelChild(std::list<std::string> node_path) {
  if (node_path.empty()) {
    is_valid_ = false;
    return STATUS_OK;
  }

  auto child_name = node_path.front();
  node_path.pop_front();
  auto item = children_.find(child_name);
  if (item == children_.end()) {
    return STATUS_NOTFOUND;
  }

  auto &child = item->second;
  auto ret = child->DelChild(std::move(node_path));
  if (ret != STATUS_OK) {
    return ret;
  }

  if (child->IsValid() || !child->IsEmpty()) {
    return STATUS_OK;
  }

  children_.erase(child_name);
  return STATUS_OK;
}

Status PrefixNode::Match(std::list<std::string> path,
                         std::list<std::string> &node_path) {
  Status default_ret = is_valid_ ? STATUS_OK : STATUS_NOTFOUND;
  if (path.empty()) {
    return default_ret;
  }

  auto child_name = path.front();
  path.pop_front();
  auto item = children_.find(child_name);
  if (item == children_.end()) {
    return default_ret;
  }

  node_path.push_back(child_name);
  auto ret = item->second->Match(std::move(path), node_path);
  if (ret == STATUS_OK) {
    return ret;
  }

  node_path.pop_back();
  return default_ret;
}

bool PrefixNode::IsValid() { return is_valid_; }

bool PrefixNode::IsEmpty() { return children_.empty(); }

PrefixTree::PrefixTree(const std::string &split_token)
    : root_(std::make_shared<PrefixNode>()), split_token_(split_token) {}

Status PrefixTree::AddNode(const std::string &node_path) {
  auto name_list = SplitPath(node_path);
  return root_->AddChild(name_list);
}

void PrefixTree::DelNode(const std::string &node_path) {
  auto name_list = SplitPath(node_path);
  root_->DelChild(name_list);
}

Status PrefixTree::Match(const std::string &path, std::string &match_result) {
  std::list<std::string> node_path_list;
  auto name_list = SplitPath(path);
  auto ret = root_->Match(name_list, node_path_list);
  if (ret != STATUS_OK) {
    match_result.clear();
    return ret;
  }

  if (node_path_list.empty()) {
    match_result = split_token_;
    return STATUS_OK;
  }

  std::stringstream node_path_buffer;
  for (auto &path : node_path_list) {
    node_path_buffer << split_token_ << path;
  }

  match_result = std::move(node_path_buffer.str());
  return STATUS_OK;
}

std::list<std::string> PrefixTree::SplitPath(const std::string &node_path) {
  std::list<std::string> node_name_list;
  auto name_start_pos = node_path.find(split_token_) + 1;
  while (true) {
    if (name_start_pos >= node_path.size()) {
      break;
    }

    auto name_end_pos = node_path.find(split_token_, name_start_pos);
    if (name_end_pos == node_path.npos) {
      node_name_list.push_back(node_path.substr(name_start_pos));
      break;
    }

    node_name_list.push_back(
        node_path.substr(name_start_pos, name_end_pos - name_start_pos));
    name_start_pos = name_end_pos + 1;
  }

  return std::move(node_name_list);
}

}  // namespace base
}  // namespace netswitch