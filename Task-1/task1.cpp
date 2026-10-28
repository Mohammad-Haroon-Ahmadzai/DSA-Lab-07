#include <iostream>
#include <string>
#include <stack>
#include <vector>
#include <algorithm>
#include <cctype>
using namespace std;

// Node
struct Node
{
    string data;
    Node* left;
    Node* right;

    Node(const string& d) : data(d), left(NULL), right(NULL) {}
};

// remove spaces from a string
string removeSpaces(const string& s)
{
    string r;
    for (size_t i = 0; i < s.size(); i++)
        if (s[i] != ' ') r += s[i];
    return r;
}

// 1. Build tree from string
Node* parseNode(const string& s, size_t& pos)
{
    // read the label (letters / digits)
    size_t start = pos;
    while (pos < s.size() && isalnum((unsigned char)s[pos])) pos++;
    if (start == pos) return NULL; // no label -> empty child

    Node* node = new Node(s.substr(start, pos - start));

    // optional "( left , right )"
    if (pos < s.size() && s[pos] == '(')
    {
        pos++;
        node->left = parseNode(s, pos);
        pos++;
        node->right = parseNode(s, pos);
        pos++;
    }
    return node;
}

Node* buildTree(const string& input)
{
    string s = removeSpaces(input);
    size_t pos = 0;
    return parseNode(s, pos);
}

// 2. Preorder (recursive) + string
void preorder(Node* root)
{
    if (root == NULL) return;
    cout << root->data << " ";
    preorder(root->left);
    preorder(root->right);
}

// prints the preorder string form a(b(d,e),c(,f(g,)))
void preorderString(Node* root)
{
    if (root == NULL) return;
    cout << root->data;
    if (root->left == NULL && root->right == NULL) return; // leaf
    cout << "(";
    preorderString(root->left);
    cout << ",";
    preorderString(root->right);
    cout << ")";
}

// 3. Inorder (iterative, stack)
void inorderIterative(Node* root)
{
    stack<Node*> st;
    Node* cur = root;
    while (cur != NULL || !st.empty())
    {
        while (cur != NULL)
        {
            st.push(cur);
            cur = cur->left;
        }
        cur = st.top();
        st.pop();
        cout << cur->data << " ";
        cur = cur->right;
    }
}

// 4. Postorder (iterative, stack)
void postorderIterative(Node* root)
{
    stack<Node*> st;
    Node* cur = root;
    Node* lastVisited = NULL;
    while (cur != NULL || !st.empty())
    {
        if (cur != NULL)
        {
            st.push(cur);
            cur = cur->left;
        }
        else
        {
            Node* top = st.top();
            // go right if a right child exists and was not visited yet
            if (top->right != NULL && lastVisited != top->right)
            {
                cur = top->right;
            }
            else
            {
                cout << top->data << " ";
                lastVisited = top;
                st.pop();
            }
        }
    }
}

// 5. Validate string WITHOUT building the tree
bool isValidTreeString(const string& input, string& reason)
{
    string s = removeSpaces(input);
    if (s.empty()) { reason = "empty string"; return false; }
    if (!isalnum((unsigned char)s[0]))
    {
        reason = "string must start with a root label";
        return false;
    }

    vector<int> commaCount;
    char prev = '\0';
    for (size_t i = 0; i < s.size(); i++)
    {
        char ch = s[i];
        if (isalnum((unsigned char)ch))
        {
            if (prev == ')')
            {
                reason = string("label '") + ch + "' at index " + to_string(i) + " cannot follow ')'";
                return false;
            }
        }
        else if (ch == '(')
        {
            if (!isalnum((unsigned char)prev))
            {
                reason = "'(' at index " + to_string(i) + " must follow a node label";
                return false;
            }
            commaCount.push_back(0);
        }
        else if (ch == ',')
        {
            if (commaCount.empty())
            {
                reason = "comma at index " + to_string(i) + " is outside parentheses";
                return false;
            }
            commaCount.back()++;
            if (commaCount.back() > 1)
            {
                reason = "more than 2 children (extra comma at index " + to_string(i) + ")";
                return false;
            }
        }
        else if (ch == ')')
        {
            if (commaCount.empty())
            {
                reason = "unmatched ')' at index " + to_string(i);
                return false;
            }
            if (commaCount.back() != 1)
            {
                reason = "')' at index " + to_string(i) + ": node needs exactly one comma";
                return false;
            }
            commaCount.pop_back();
        }
        else
        {
            reason = string("illegal character '") + ch + "' at index " + to_string(i);
            return false;
        }
        prev = ch;
    }

    if (!commaCount.empty())
    {
        reason = "unbalanced parentheses: missing ')'";
        return false;
    }
    reason = "ok";
    return true;
}

// 6. Height and depth
// Height in EDGES (leaf = 0, empty tree = -1)
int height(Node* root)
{
    if (root == NULL) return -1;
    return 1 + max(height(root->left), height(root->right));
}

// Recursively print the depth of each node (root depth = 0)
void printDepths(Node* root, int d = 0)
{
    if (root == NULL) return;
    cout << "  Node " << root->data << ": depth = " << d << endl;
    printDepths(root->left, d + 1);
    printDepths(root->right, d + 1);
}

void deleteTree(Node* root)
{
    if (root == NULL) return;
    deleteTree(root->left);
    deleteTree(root->right);
    delete root;
}

// 7. BST from integer list
struct BSTNode
{
    int key;
    BSTNode* left;
    BSTNode* right;

    BSTNode(int k) : key(k), left(NULL), right(NULL) {}
};

BSTNode* bstInsert(BSTNode* root, int key)
{
    if (root == NULL) return new BSTNode(key);
    if (key < root->key) root->left = bstInsert(root->left, key);
    else if (key > root->key) root->right = bstInsert(root->right, key);
    return root; // duplicates are ignored
}

// first element becomes the root, the rest are inserted in order
BSTNode* buildBST(const vector<int>& values)
{
    BSTNode* root = NULL;
    for (size_t i = 0; i < values.size(); i++)
        root = bstInsert(root, values[i]);
    return root;
}

void bstInorder(BSTNode* root)
{
    if (root == NULL) return;
    bstInorder(root->left);
    cout << root->key << " ";
    bstInorder(root->right);
}

void deleteBST(BSTNode* root)
{
    if (root == NULL) return;
    deleteBST(root->left);
    deleteBST(root->right);
    delete root;
}

// main
int main()
{
    string treeStr = "a(b(d,e),c(,f(g,)))";

    cout << "============================================================\n";
    cout << "Given string: " << treeStr << "\n";
    cout << "============================================================\n";

    // validate first (without building the tree)
    string reason;
    bool ok = isValidTreeString(treeStr, reason);
    cout << "\n[Validation] Is the string valid? " << (ok ? "true" : "false")
         << " - " << reason << "\n";
    if (!ok) return 0;

    // 1. build
    Node* root = buildTree(treeStr);
    cout << "\n[1] Tree built successfully. Root = " << root->data << "\n";

    // 2. preorder (recursive)
    cout << "\n[2] Preorder (recursive)\n";
    cout << "  Sequence    : ";
    preorder(root);
    cout << "\n  String form : ";
    preorderString(root);
    cout << "\n";

    // 3. inorder (iterative)
    cout << "\n[3] Inorder (iterative, stack): ";
    inorderIterative(root);
    cout << "\n";

    // 4. postorder (iterative)
    cout << "\n[4] Postorder (iterative, stack): ";
    postorderIterative(root);
    cout << "\n";

    // 5. more validation examples
    cout << "\n[5] Validation tests (no tree is built):\n";
    const char* tests[] = {
        "a(b(d,e),c(,f(g,)))",   // valid
        "a(b(d,e),c(,f(g,))",    // missing ')'
        "a(b(d,e),c(,f(g,))))",  // extra ')'
        "a(b,c,d)",              // 3 children
        "a(b(d,e) c)",           // bad structure
        "(b,c)",                 // no root
        "a(b,c)x",               // junk after the tree
        "a"                      // single node - valid
    };
    for (int i = 0; i < 8; i++)
    {
        string why;
        bool v = isValidTreeString(tests[i], why);
        cout << "  " << tests[i] << string(26 - string(tests[i]).size(), ' ')
             << "-> " << (v ? "VALID" : "INVALID") << " (" << why << ")\n";
    }

    // 6. height and depth
    cout << "\n[6] Height and depth\n";
    cout << "  Height of tree (edges) : " << height(root) << "\n";
    cout << "  Height of tree (levels): " << height(root) + 1 << "\n";
    cout << "  Depth of each node:\n";
    printDepths(root);

    // 7. BST from integers
    vector<int> nums;
    int arr[] = {50, 30, 70, 20, 40, 60, 80};
    for (int i = 0; i < 7; i++) nums.push_back(arr[i]);
    BSTNode* bst = buildBST(nums);
    cout << "\n[7] BST from [50, 30, 70, 20, 40, 60, 80]\n";
    cout << "  Inorder traversal: ";
    bstInorder(bst);
    cout << "\n";

    deleteTree(root);
    deleteBST(bst);
    return 0;
}
