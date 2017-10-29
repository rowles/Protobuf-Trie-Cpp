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
        std::optional<int32_t> value;
        // array would be better here
        std::unordered_map<char, NodePtr> children_map{};
    };

    Trie() = default;

    virtual ~Trie() = default;

    // Insert Key Value pair into Trie
    //
    //
    void insert(const std::tuple<std::string, int32_t> &pair) {
        const std::string_view keyView{std::get<0>(pair)};

        if (root == nullptr) {
            std::cout << "setting root" << std::endl;
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

    // Search for value within Trie
    //
    //
    std::optional<int32_t> find(const std::string &key) {
        if (root == nullptr) {
            std::cout << "null root" << std::endl;
            return {};
        }

        auto node = root.get();

        for (const char &c : key) {
            if (node->children_map.find(c) == node->children_map.end()) {
                std::cout << "not found " << c << std::endl;
                return {};
            }
            node = node->children_map[c].get();
        }

        return node->value;
    }

    std::unique_ptr<test::TrieNode> serialize() {
        auto tn = std::make_unique<test::TrieNode>();

        if (root == nullptr) {
            return tn;
        }

        std::string s = "";
        this->traverse_copy(s, root.get(), tn.get());

        return tn;
    }

private:
    NodePtr root{nullptr};

    void traverse_copy(const std::string& let, Node* old_node, test::TrieNode* new_node) {
        std::cout << "setting values" << std::endl;
        if (!let.empty()) new_node->set_letter(let);
        if (old_node->value.has_value()) new_node->set_value(old_node->value.value());
        std::cout << "set values" << std::endl;

        for (auto& p : old_node->children_map) {
            std::cout << "values" << std::endl;

            std::string l = std::string(1, p.first);
            this->traverse_copy(l, p.second.get(), new_node->add_children());
        }
    }
    
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
    assert(val == 1);

    assert(trie.find("tes").value_or(-100) == 100);
    assert(trie.find("te").value_or(-100) == -100);
    assert(trie.find("cat").value_or(-100) == 999);
    assert(trie.find("doggo").value_or(-100) == 99);
    assert(trie.find("snoopdog").value_or(-100) == -100);

    auto tpb = trie.serialize();

    auto serial = tpb->SerializeAsString();
    std::cout << serial << std::endl;

    return 0;
}