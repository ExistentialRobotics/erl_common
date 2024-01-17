#include "template_helper.hpp"
#include "assert.hpp"
#include <cmath>

namespace erl::common {

    template<class T, class Less>
    class FibonacciHeap {
    public:
        using Self = FibonacciHeap<T, Less>;

        struct Node {
            ERL_SMART_PTR_TYPEDEFS(Node);

            T key;
            Node::Ptr left = nullptr;
            Node::Ptr right = nullptr;
            Node::Ptr parent = nullptr;
            Node::Ptr child = nullptr;
            std::size_t degree = 0;
            bool mark = false;

            explicit Node(T key)
                : key(key) {}
        };

    private:
        typename Node::Ptr m_min_node_ = nullptr;
        std::size_t m_num_of_nodes_ = 0;
        Less m_less_;
        typename Node::Ptr m_node_to_delete_ = nullptr;

    public:
        [[deprecated("please use boost::heap::fibonacci_heap instead")]]
        ~FibonacciHeap() { ClearHeap(m_min_node_); }

        [[nodiscard]] inline const T&
        Min() const {
            return m_min_node_->key;
        }

        [[nodiscard]] inline typename Node::ConstPtr
        MinNode() const {
            return m_min_node_;
        }

        [[nodiscard]] inline std::size_t
        Size() const {
            return m_num_of_nodes_;
        }

        inline typename Node::Ptr
        Insert(T key) {
            // create a new node
            auto node = std::make_shared<Node>(key);
            node->left = node;
            node->right = node;
            m_min_node_ = MergeLists(m_min_node_, node);
            ++m_num_of_nodes_;
            return node;
        }

        inline void
        Merge(Self& other) {
            m_min_node_ = MergeLists(m_min_node_, other.m_min_node_);
            m_num_of_nodes_ += other.m_num_of_nodes_;
            other.m_min_node_ = nullptr;
            other.m_num_of_nodes_ = 0;
        }

        /**
         * @brief Extract the minimum node from the heap and delete it from the heap.
         * @return the minimum node
         */
        T
        ExtractMin() {
            typename Node::Ptr min_node = m_min_node_;
            ERL_ASSERTM(min_node != nullptr, "The heap is empty.");
            // make all nodes' parent null in a circular doubly linked list
            if (min_node->child != nullptr) {
                typename Node::Ptr child = min_node->child;
                do {
                    child->parent = nullptr;
                    child = child->right;
                } while (child != min_node->child);
            }
            // merge the children of the minimum node with the root list
            MergeLists(min_node, min_node->child);
            if (min_node == min_node->right) {
                m_min_node_ = nullptr;  // only one node in the heap
            } else {
                m_min_node_ = min_node->right;  // move to the right
            }
            RemoveFromList(min_node);
            if (m_min_node_ != nullptr) { Consolidate(); }
            --m_num_of_nodes_;
            return min_node->key;
        }

        /**
         * @brief Call this function when the key of a node is decreased.
         * @param node
         */
        inline void
        DecreaseKey(typename Node::Ptr node) {
            typename Node::Ptr parent = node->parent;
            if (parent != nullptr && IsLess(node, parent)) {
                // node's key is smaller than its parent's key
                Cut(node, parent);
                CascadingCut(parent);
            }
            if (IsLess(node, m_min_node_)) { m_min_node_ = node; }
        }

        inline void
        DeleteNode(typename Node::Ptr node) {
            m_node_to_delete_ = node;
            DecreaseKey(node);
            ExtractMin();
        }

        [[nodiscard]] inline bool
        IsEmpty() const {
            return m_num_of_nodes_ == 0;
        }

    private:
        inline bool
        IsLess(typename Node::Ptr node_a, typename Node::Ptr node_b) const {
            return m_less_(node_a->key, node_b->key) || node_a == m_node_to_delete_;
        }

        /**
         * @brief merge two circular doubly linked lists.
         * @param node_a node of circular doubly linked list A
         * @param node_b node of circular doubly linked list B
         * @return
         */
        inline typename Node::Ptr
        MergeLists(typename Node::Ptr node_a, typename Node::Ptr node_b) {
            if (node_a == nullptr) { return node_b; }
            if (node_b == nullptr) { return node_a; }
            // make sure node1->key <= node2->key
            if (IsLess(node_b, node_a)) { std::swap(node_b, node_a); }
            typename Node::Ptr a_right = node_a->right;
            typename Node::Ptr b_left = node_b->left;
            node_a->right = node_b;
            node_b->left = node_a;
            a_right->left = b_left;
            b_left->right = a_right;
            return node_a;
        }

        inline void
        RemoveFromList(typename Node::Ptr node) {
            if (node->right == node) { return; }  // only one node in the list
            auto left_sibling = node->left;
            auto right_sibling = node->right;
            left_sibling->right = right_sibling;
            right_sibling->left = left_sibling;
            node->left = node->right = node;  // self-loop
        }

        void
        Consolidate() {
            auto max_root_degree = std::size_t(std::log2(double(m_num_of_nodes_)) / std::log2(1.618));
            std::vector<typename Node::Ptr> root_list(max_root_degree + 1, nullptr);
            typename Node::Ptr node = m_min_node_;
            bool reach_end = node == node->right;
            do {
                typename Node::Ptr e = node;
                std::size_t d = e->degree;
                reach_end = node == node->right;
                node = node->right;  // move to the right
                // ERL_ASSERTM(node->key != e->key, "??");
                RemoveFromList(e);  // e is self-loop now
                                    // std::cout << ">> e(" << e << ")->key = " << e->key << ", e->degree = " << e->degree << std::endl;
                while (root_list[d] != nullptr) {
                    typename Node::Ptr tmp = root_list[d];
                    // std::cout << "e->key = " << e->key << ", tmp->key = " << tmp->key << ", e->degree = " << e->degree << ", tmp->degree = " << tmp->degree
                    // << std::endl;
                    if (IsLess(tmp, e)) {
                        // std::cout << "swap e and tmp(" << tmp << ")" << std::endl;
                        std::swap(tmp, e);
                    }
                    MakeChild(tmp, e);
                    root_list[d++] = nullptr;  // tmp has been merged to e's child list
                                               // std::cout << "e->degree = " << e->degree << ", d = " << d << std::endl;
                }
                root_list[e->degree] = e;
                m_min_node_ = e;
            } while (!reach_end);
            // merge all roots in the root list
            typename Node::Ptr flag = m_min_node_;  // node that has been merged
            for (auto& root: root_list) {
                if (root != nullptr && root != flag) { m_min_node_ = MergeLists(m_min_node_, root); }
            }
        }

        inline void
        Cut(typename Node::Ptr child, typename Node::Ptr parent) {
            parent->child = (child == child->right) ? nullptr : child->right;
            RemoveFromList(child);
            parent->degree--;
            MergeLists(m_min_node_, child);
            child->parent = nullptr;
            child->mark = false;
        }

        /**
         * @brief Continue to cut the parent node to the root until a node that is not marked is found.
         * @param node
         */
        inline void
        CascadingCut(typename Node::Ptr node) {
            typename Node::Ptr parent = node->parent;
            if (parent == nullptr) { return; }
            if (node->mark) {
                Cut(node, parent);
                CascadingCut(parent);
            } else {
                node->mark = true;  // mark the node that has lost a child
            }
        }

        inline void
        MakeChild(typename Node::Ptr child, typename Node::Ptr parent) {
            child->parent = parent;
            parent->child = MergeLists(parent->child, child);  // add child to the parent's child list
            ++parent->degree;                                  // degree is the number of children
            child->mark = false;
        }

        inline void
        ClearHeap(typename Node::Ptr node) {
            if (node == nullptr) { return; }

            typename Node::Ptr current = node;
            do {
                typename Node::Ptr next = current->right;
                ClearHeap(current->child);
                current->left.reset();
                current->right.reset();
                current->parent.reset();
                current->child.reset();
                current = next;
            } while (current != node);
            node.reset();
        }
    };

}  // namespace erl::common
