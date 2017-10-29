#include <iostream>
#include <string_view>
#include <tuple>
#include <memory>
#include <unordered_map>
#include <optional>
#include <vector>
#include <fstream>

#include "trie.pb.h"


class Trie {
public:
    struct Node;
    typedef std::unique_ptr<Node> NodePtr;
    typedef std::vector<std::tuple<std::string, int32_t>> Dict;

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
            return {};
        }

        auto node = root.get();

        for (const char &c : key) {
            if (node->children_map.find(c) == node->children_map.end()) {
                return {};
            }
            node = node->children_map[c].get();
        }

        return node->value;
    }

    std::unique_ptr<test::TrieNode> serialize_tree() {
        auto tn = std::make_unique<test::TrieNode>();

        if (root == nullptr) {
            return tn;
        }

        std::string s = "";
        this->traverse_copy(s, root.get(), tn.get());

        return tn;
    }

    std::unique_ptr<Dict> get_entries() {
        auto vec = std::make_unique<Dict>();

        std::string word = "";
        this->dfs(word, root.get(), vec.get());

        return vec;
    };

    std::unique_ptr<test::Dictionary> serialize_dictionary() {
        auto entries = this->get_entries();
        auto dict = std::make_unique<test::Dictionary>();

        for (auto& e : *entries) {
            auto entry = dict->add_enties();
            entry->set_key(std::get<0>(e));
            entry->set_value(std::get<1>(e));
        }

        return dict;
    }

    void save_dictionary(const std::string& filename) {
        auto dict = this->serialize_dictionary();

        std::ofstream of(filename);
        of << dict->SerializeAsString();
        of.close();
    }

private:
    NodePtr root{nullptr};

    void traverse_copy(const std::string& let, Node* old_node, test::TrieNode* new_node) {
        if (!let.empty()) new_node->set_letter(let);
        if (old_node->value.has_value()) new_node->set_value(old_node->value.value());

        for (auto& p : old_node->children_map) {
            std::string l = std::string(1, p.first);
            this->traverse_copy(l, p.second.get(), new_node->add_children());
        }
    }

    /*
    void traverse_load(const std::string& let, Node* old_node, test::TrieNode* pb_node) {
        std::cout << "setting values" << std::endl;
        //if (!new_node->letter().empty()) old_node->value = new_node->letter();
        if (pb_node->value()) old_node->value = pb_node->value();
        std::cout << "set values" << std::endl;

        for (auto& p : pb_node->children()) {
            std::cout << "values" << std::endl;

            this->traverse_load(p.letter(), p.second.get(), new_node->add_children());
        }
    }*/

    // Depth First Search
    // Get all Key Value pairs
    //
    void dfs(const std::string word, Node* node, Dict* dict_vec) {
        if (node->value.has_value()) {
            auto t = std::make_tuple(word, node->value.value());
            dict_vec->push_back(t);
        }

        for (auto& p : node->children_map) {
            std::string w = word + std::string(1, p.first);
            this->dfs(w, p.second.get(), dict_vec);
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

    trie.get_entries();

    auto tpb = trie.serialize_tree();

    auto serial = tpb->SerializeAsString();
    // std::cout << serial << std::endl;

    auto entries = trie.get_entries();

    for (auto& e : *entries) {
        std::cout << "{" << std::get<0>(e) << ", " << std::get<1>(e) << "}" << std::endl;
    }

    trie.save_dictionary("dict.pb");

    return 0;
}