#include "HCTree.hpp"

/* TODO: Delete all objects on the heap to avoid memory leaks. */
HCTree::~HCTree() {
    if (root) {
        deleteNodeRecursive(root);
    }
}

void HCTree::deleteNodeRecursive(HCNode* node) {
    if (node == nullptr) return;
    deleteNodeRecursive(node->c0);
    deleteNodeRecursive(node->c1);
    delete node;
}

/**
 * TODO: Build the HCTree from the given frequency vector. You can assume the
 * vector must have size 256 and each value at index i represents the frequency
 * of char with ASCII value i. Only non-zero frequency symbols should be used to
 * build the tree. The leaves vector must be updated so that it can be used in
 * encode() to improve performance.
 *
 * When building the HCTree, you should use the following tie-breaking rules to
 * match the output from reference solution in checkpoint:
 *
 *    1. HCNode with lower count should have higher priority. If count is the
 * same, then HCNode with a larger ascii value symbol should have higher
 * priority. (This should be already defined properly in the comparator in
 * HCNode.hpp)
 *    2. When popping two highest priority nodes from PQ, the higher priority
 * node will be the ‘c0’ child of the new parent HCNode.
 *    3. The symbol of any parent node should be taken from its 'c0' child.
 */
void HCTree::build(const vector<unsigned int>& freqs) {
    priority_queue<HCNode*, vector<HCNode*>, HCNodePtrComp> leafNodes;
    leaves.resize(256, nullptr);
    for (int i = 0; i < freqs.size(); ++i) {
        if (freqs[i] > 0) {
            HCNode* node = new HCNode(freqs[i], (byte)i);
            leafNodes.push(node);
            leaves[i] = node;
        }
    }
    if (leafNodes.size() == 1) {
        HCNode* c0 = leafNodes.top();
        leafNodes.pop();
        HCNode* parent = new HCNode(c0->count, c0->symbol, c0, nullptr);
        c0->p = parent;
        root = parent;
    } else {
        while (1) {
            HCNode* c0 = leafNodes.top();
            leafNodes.pop();
            HCNode* c1 = leafNodes.top();
            leafNodes.pop();
            HCNode* parent =
                new HCNode(c0->count + c1->count, c0->symbol, c0, c1);
            c0->p = parent;
            c1->p = parent;
            if (leafNodes.empty()) {
                root = parent;
                break;
            }
            leafNodes.push(parent);
        }
    }
}

void HCTree::dump(ostream& out) const {
    // level order traversal
    vector<HCNode*> frontier;
    vector<HCNode*> detectedleaves;
    if (root != nullptr) frontier.push_back(root);
    unsigned char idx_counter;
    unsigned char idx_latest_layer;

    byte bytecounter = 0;
    byte bytebuffer = 0;
    while (1) {
        vector<HCNode*> newfrontier;
        for (HCNode* eachFrontier : frontier) {
            // check left node
            bytecounter++;
            if (eachFrontier->c0 != nullptr) {
                newfrontier.push_back(eachFrontier->c0);
                bytebuffer |= 1 << (8 - bytecounter);
            }

            // check right node
            bytecounter++;
            if (eachFrontier->c1 != nullptr) {
                newfrontier.push_back(eachFrontier->c1);
                bytebuffer |= 1 << (8 - bytecounter);
            }

            // detect leaves
            if (eachFrontier->c0 == nullptr && eachFrontier->c1 == nullptr)
                detectedleaves.push_back(eachFrontier);

            // append if bytebuffer full
            if (bytecounter == 8) {
                out.put(bytebuffer);
                bytebuffer = 0;
                bytecounter = 0;
            }
        }

        // last layer must be fully logged, then break
        if (frontier.empty()) break;

        frontier = newfrontier;
    }

    // append if bytebuffer incomplete
    if (bytecounter != 0) out.put(bytebuffer);

    for (HCNode* leaf : detectedleaves) {
        out.put(leaf->symbol);  // leaf node symbols
    }
}

void HCTree::load(istream& in) {
    char c;
    bool next_new_layer_flag = false;
    vector<HCNode*> frontier;
    vector<HCNode*> newfrontier;
    leaves.clear();
    root = new HCNode(' ', 0);
    frontier.push_back(root);

    int num_this_layer = 2 * frontier.size();
    bool done = false;
    while (1) {
        in.get(c);
        for (int i = 0; i < 8; i++) {
            if ((c >> (7 - i)) & 1) {
                int thidIdx = 2 * frontier.size() - num_this_layer;
                HCNode* parent = frontier[thidIdx / 2];
                HCNode* child = new HCNode(' ', 0);
                if (thidIdx % 2 == 0) parent->c0 = child;
                if (thidIdx % 2 == 1) parent->c1 = child;
                child->p = parent;
                newfrontier.push_back(child);
            }
            num_this_layer--;

            if (num_this_layer == 0) {
                // collect leaves
                for (HCNode* eachFrontier : frontier) {
                    if (eachFrontier->c0 == nullptr &&
                        eachFrontier->c1 == nullptr)
                        leaves.push_back(eachFrontier);
                }
                // move to new layer
                frontier = newfrontier;
                newfrontier.clear();
                num_this_layer = 2 * frontier.size();
                if (num_this_layer == 0) {
                    done = true;
                }
            }
            if (done) break;
        }
        if (done) break;
    }

    for (HCNode* leaf : leaves) {
        leaf->symbol = in.get();
    }
    char dd;
}

/**
 * TODO: Write the encoding bits of the given symbol to ostream. You should
 * write each encoding bit as ascii char either '0' or '1' to the ostream. You
 * must not perform a comprehensive search to find the encoding bits of the
 * given symbol, and you should use the leaves vector instead to achieve
 * efficient encoding. For this function to work, build() must have been called
 * beforehand to create the HCTree.
 */
void HCTree::encode(byte symbol, ostream& out) const {
    HCNode* thisNode = leaves[symbol];
    vector<char> traverseUp;
    while (thisNode != root) {
        traverseUp.insert(traverseUp.begin(),
                          thisNode == (thisNode->p->c0) ? '0' : '1');
        thisNode = thisNode->p;
    }

    for (auto it = traverseUp.begin(); it != traverseUp.end(); ++it) {
        out.put(*it);
    }
}

/**
 * TODO: Decode the sequence of bits (represented as a char of either '0' or
 * '1') from the istream to return the coded symbol. For this function to work,
 * build() must have been called beforehand to create the HCTree.
 */
// byte HCTree::decode(BitInputStream& in) const { return ' '; }

/**
 * TODO: Decode the sequence of bits (represented as char of either '0' or '1')
 * from istream to return the coded symbol. For this function to work, build()
 * must have been called beforehand to create the HCTree.
 */
byte HCTree::decode(istream& in) const {
    char c;
    HCNode* curr = root;
    while (1) {
        in.get(c);
        HCNode* next = c == '0' ? curr->c0 : curr->c1;
        if (next->c0 == nullptr && next->c1 == nullptr) return next->symbol;
        curr = next;
    }
    return ' ';
}