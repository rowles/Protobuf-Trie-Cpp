#include <iostream>
#include <string_view>
#include <tuple>
#include <memory>
#include <unordered_map>
#include <optional>

#include "trie.pb.h"


class Trie {
public:
    struct Node;
    typedef std::unique_ptr<Node> NodePtr;

    struct Node {
        int32_t value;
        std::unordered_map<char, NodePtr> children_map{};
    };

    Trie() = default;

    virtual ~Trie() = default;

    void insert(const std::tuple<std::string, int32_t> &pair) {
        const std::string_view keyView{std::get<0>(pair)};

        if (root == nullptr) {
            root = std::make_unique<Node>();
        }

        auto node = root.get();

        for (auto it = keyView.cbegin(); it != keyView.cend(); ++it) {
            const char &cc = (*it);

            if (node->children_map.find(cc) == node->children_map.end()) {
                node->children_map[cc] = std::make_unique<Node>();
            }

            node = node->children_map[cc].get();

        }

        node->value = std::get<1>(pair);
    }

    std::optional<int32_t> find(const std::string &key) {
        if (root == nullptr) return {};

        auto node = std::move(root);

        for (const char &c : key) {
            if (node->children_map.find(c) == node->children_map.end()) {
                std::cout << "not found " << c << std::endl;
                return {};
            }
            node = std::move(node->children_map[c]);
        }

        return node->value;
    }


private:
    NodePtr root{nullptr};

};

int main() {
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    Trie trie{};

    auto t = std::make_tuple("test", 1);
    trie.insert(t);

    trie.insert({"tes", 100});
    trie.insert({"cat", 999});
    trie.insert({"doggo", 99});

    auto val = trie.find("test").value_or(-100);
    std::cout << val << std::endl;

    std::cout << trie.find("cat").value_or(-100) << std::endl;
    std::cout << trie.find("404").value_or(-100) << std::endl;

    return 0;
}