#include "MainFrame.h"
#include <wx/wx.h>
#include <wx/sizer.h>
#include <wx/grid.h>
#include <iomanip> 
#include <wx/scrolwin.h>
#include <wx/statline.h>
#include <sstream>  
#include <vector>
using namespace std;

template <typename T>
struct Node {
    T data;
    Node<T>* next;
    Node<T>* prev;

    Node(const T& d) : data(d), next(nullptr), prev(nullptr) {}
};

template <typename T>
class LinkedList {
private:
    Node<T>* head;
    Node<T>* tail;

public:
    LinkedList() : head(nullptr), tail(nullptr) {}

    struct iterator {
        Node<T>* node;
        iterator(Node<T>* n) : node(n) {}

        T& operator*() { return node->data; }
        iterator& operator++() { node = node->next; return *this; }
        bool operator!=(const iterator& other) const { return node != other.node; }
    };

    iterator begin() { return iterator(head); }
    iterator end() { return iterator(nullptr); }

    void push_back(const T& dataBaru) {
        Node<T>* baru = new Node<T>(dataBaru);
        if (!head) {
            head = tail = baru;
        }
        else {
            tail->next = baru;
            baru->prev = tail;
            tail = baru;
        }
    }

    bool empty() const {
        return head == nullptr;
    }

    void tampilkanSemua() const {
        Node<T>* bantu = head;
        while (bantu) {
            bantu = bantu->next;
        }
    }

    template <typename Predicate>
    void remove_if(Predicate pred) {
        Node<T>* current = head;
        while (current) {
            if (pred(current->data)) {
                Node<T>* toDelete = current;

                if (toDelete->prev)
                    toDelete->prev->next = toDelete->next;
                else
                    head = toDelete->next;

                if (toDelete->next)
                    toDelete->next->prev = toDelete->prev;
                else
                    tail = toDelete->prev;

                current = toDelete->next;
                delete toDelete;
            }
            else {
                current = current->next;
            }
        }
    }

    size_t size() const {
        size_t count = 0;
        Node<T>* current = head;
        while (current) {
            ++count;
            current = current->next;
        }
        return count;
    }

    ~LinkedList() {
        Node<T>* current = head;
        while (current) {
            Node<T>* next = current->next;
            delete current;
            current = next;
        }
    }

    Node<T>* getHead() const { return head; }
};

// Struct definitions
struct Kategori {
    wxString nama;
    Kategori(const wxString& n) : nama(n) {}
};

struct Aset {
    wxString id;
    wxString nama;
    wxString kategori;

    Aset(const wxString& i, const wxString& n, const wxString& k)
        : id(i), nama(n), kategori(k) {
    }
};

struct AssetValueDetails {
    wxString assetId;
    int currentValue;
    int maintenanceCost;
    int propertyTax;

    AssetValueDetails(const wxString& id, int val, int maint = 0, int tax = 0)
        : assetId(id), currentValue(val), maintenanceCost(maint), propertyTax(tax) {
    }
};

struct AssetConnection {
    wxString fromAssetId;
    wxString toAssetId;
    int weight;
    wxString description;

    AssetConnection(const wxString& from, const wxString& to, int w, const wxString& desc = "")
        : fromAssetId(from), toAssetId(to), weight(w), description(desc) {
    }
};

struct GraphNode {
    wxString assetId;
    LinkedList<AssetConnection> connections;

    GraphNode(const wxString& id) : assetId(id) {}
};


struct CategoryCount {
    wxString kategori;
    int count;

    CategoryCount(const wxString& k, int c = 0) : kategori(k), count(c) {}
};

struct SubAsset {
    wxString id;
    wxString nama;
    wxString parentId;  // ID dari parent asset
    wxString description;
    bool isRented;
    wxString renterName;
    int rentalPrice;

    SubAsset(const wxString& i, const wxString& n, const wxString& parent,
        const wxString& desc = "", bool rented = false,
        const wxString& renter = "", int price = 0)
        : id(i), nama(n), parentId(parent), description(desc),
        isRented(rented), renterName(renter), rentalPrice(price) {
    }
};

template <typename T>
struct TreeNode {
    T data;
    LinkedList<TreeNode<T>*> children;
    TreeNode<T>* parent;

    TreeNode(const T& d) : data(d), parent(nullptr) {}

    void addChild(TreeNode<T>* child) {
        child->parent = this;
        children.push_back(child);
    }

    void removeChild(TreeNode<T>* child) {
        children.remove_if([child](TreeNode<T>* node) {
            return node == child;
            });
        if (child) {
            child->parent = nullptr;
        }
    }

    ~TreeNode() {
        // Clean up children
        Node<TreeNode<T>*>* current = children.getHead();
        while (current) {
            delete current->data;
            current = current->next;
        }
    }
};

template <typename T>
class AssetTree {
private:
    TreeNode<T>* root;

public:
    AssetTree() : root(nullptr) {}

    TreeNode<T>* getRoot() const { return root; }

    void setRoot(TreeNode<T>* newRoot) { root = newRoot; }

    TreeNode<T>* findNode(const wxString& id) {
        return findNodeRecursive(root, id);
    }

    TreeNode<T>* findNodeRecursive(TreeNode<T>* node, const wxString& id) {
        if (!node) return nullptr;

        if (node->data.id == id) return node;

        Node<TreeNode<T>*>* child = node->children.getHead();
        while (child) {
            TreeNode<T>* result = findNodeRecursive(child->data, id);
            if (result) return result;
            child = child->next;
        }
        return nullptr;
    }

    void getAllSubAssets(TreeNode<T>* node, LinkedList<T>& result) {
        if (!node) return;

        Node<TreeNode<T>*>* child = node->children.getHead();
        while (child) {
            result.push_back(child->data->data);
            getAllSubAssets(child->data, result);
            child = child->next;
        }
    }

    int getDepth(TreeNode<T>* node) {
        if (!node || !node->parent) return 0;
        return 1 + getDepth(node->parent);
    }

    ~AssetTree() {
        delete root;
    }
};

struct TenderProject {
    wxString id;
    wxString nama;
    wxString kategori;
    wxString description;
    wxString status; // "Pending", "In Review", "Approved", "Rejected"
    wxString tenderDate;
    int estimatedValue;
    wxString clientName;
    int priority; // 1-5, where 1 is highest priority

    TenderProject(const wxString& i, const wxString& n, const wxString& k,
        const wxString& desc = "", const wxString& stat = "Pending",
        const wxString& date = "", int value = 0, const wxString& client = "",
        int prio = 3)
        : id(i), nama(n), kategori(k), description(desc), status(stat),
        tenderDate(date), estimatedValue(value), clientName(client), priority(prio) {
    }
};

// QUEUE IMPLEMENTATION FOR TENDER PROJECTS
template <typename T>
class Queue {
private:
    Node<T>* front;
    Node<T>* rear;
    size_t count;

public:
    Queue() : front(nullptr), rear(nullptr), count(0) {}

    void enqueue(const T& data) {
        Node<T>* newNode = new Node<T>(data);
        if (rear == nullptr) {
            front = rear = newNode;
        }
        else {
            rear->next = newNode;
            newNode->prev = rear;
            rear = newNode;
        }
        count++;
    }

    bool dequeue() {
        if (front == nullptr) return false;

        Node<T>* temp = front;
        front = front->next;

        if (front != nullptr) {
            front->prev = nullptr;
        }
        else {
            rear = nullptr;
        }

        delete temp;
        count--;
        return true;
    }

    T* peek() {
        return front ? &(front->data) : nullptr;
    }

    bool empty() const { return front == nullptr; }
    size_t size() const { return count; }

    // Iterator for displaying all items
    struct iterator {
        Node<T>* node;
        iterator(Node<T>* n) : node(n) {}
        T& operator*() { return node->data; }
        iterator& operator++() { node = node->next; return *this; }
        bool operator!=(const iterator& other) const { return node != other.node; }
    };

    iterator begin() { return iterator(front); }
    iterator end() { return iterator(nullptr); }

    // Method to remove specific item by ID
    template <typename Predicate>
    void remove_if(Predicate pred) {
        Node<T>* current = front;
        while (current) {
            if (pred(current->data)) {
                Node<T>* toDelete = current;

                // Update links
                if (toDelete->prev) {
                    toDelete->prev->next = toDelete->next;
                }
                else {
                    front = toDelete->next;
                }

                if (toDelete->next) {
                    toDelete->next->prev = toDelete->prev;
                }
                else {
                    rear = toDelete->prev;
                }

                current = toDelete->next;
                delete toDelete;
                count--;
            }
            else {
                current = current->next;
            }
        }
    }

    ~Queue() {
        while (!empty()) {
            dequeue();
        }
    }
};

struct AssetHistory {
    wxString assetId;
    wxString assetName;
    wxString assetType; // "Asset", "SubAsset", dll
    wxString accessTime; // timestamp sederhana
    AssetHistory(const wxString& id, const wxString& name, const wxString& type, const wxString& time = "")
        : assetId(id), assetName(name), assetType(type), accessTime(time) {
    }
};

// NEW: Stack implementation
template <typename T>
class Stack {
private:
    Node<T>* top;
    size_t count;
    size_t maxSize; // Batasan maksimal item

public:
    Stack(size_t maxSize = 10) : top(nullptr), count(0), maxSize(maxSize) {}

    void push(const T& data) {
        Node<T>* newNode = new Node<T>(data);
        newNode->next = top;
        if (top != nullptr) top->prev = newNode;
        top = newNode;
        count++;

        // Batasi ukuran stack - hapus yang paling lama jika melebihi batas
        if (count > maxSize) {
            Node<T>* current = top;
            // Cari node terakhir
            while (current->next != nullptr) {
                current = current->next;
            }
            // Hapus node terakhir
            if (current->prev) current->prev->next = nullptr;
            delete current;
            count--;
        }
    }

    bool pop() {
        if (top == nullptr) return false;
        Node<T>* temp = top;
        top = top->next;
        if (top != nullptr) top->prev = nullptr;
        delete temp;
        count--;
        return true;
    }

    T* peek() { return top ? &(top->data) : nullptr; }
    bool empty() const { return top == nullptr; }
    size_t size() const { return count; }

    struct iterator {
        Node<T>* node;
        iterator(Node<T>* n) : node(n) {}
        T& operator*() { return node->data; }
        iterator& operator++() { node = node->next; return *this; }
        bool operator!=(const iterator& other) const { return node != other.node; }
    };
    iterator begin() { return iterator(top); }
    iterator end() { return iterator(nullptr); }

    ~Stack() { while (!empty()) { pop(); } }
};

// GLOBAL QUEUE FOR TENDER PROJECTS
Queue<TenderProject> queueTenderProjects;

Stack<AssetHistory> assetHistoryStack(15); 

LinkedList<SubAsset> daftarSubAset;
LinkedList<AssetTree<SubAsset>*> assetTrees;

// Global data structures
LinkedList<Kategori> daftarKategori;
LinkedList<Aset> daftarAset;
LinkedList<AssetValueDetails> daftarDetailNilaiAset;
LinkedList<AssetConnection> daftarKoneksiAset;

// Helper functions
Node<CategoryCount>* findCategoryNode(LinkedList<CategoryCount>& counts, const wxString& key) {
    Node<CategoryCount>* current = counts.getHead();
    while (current) {
        if (current->data.kategori == key) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

void initializeCategoryCounts(const LinkedList<Kategori>& daftarKategori, LinkedList<CategoryCount>& counts) {
    Node<Kategori>* current = daftarKategori.getHead();
    while (current) {
        counts.push_back(CategoryCount(current->data.nama, 0));
        current = current->next;
    }
}

void countAssetsByCategory(const LinkedList<Aset>& daftarAset, LinkedList<CategoryCount>& counts) {
    Node<Aset>* current = daftarAset.getHead();
    while (current) {
        Node<CategoryCount>* node = findCategoryNode(counts, current->data.kategori);
        if (node) {
            node->data.count++;
        }
        current = current->next;
    }
}

Node<AssetValueDetails>* findAssetValueById(LinkedList<AssetValueDetails>& list, const wxString& id) {
    Node<AssetValueDetails>* current = list.getHead();
    while (current) {
        if (current->data.assetId == id) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

void addOrUpdateAssetValue(LinkedList<AssetValueDetails>& list, const wxString& id, int currentValue, int maintenanceCost, int propertyTax) {
    Node<AssetValueDetails>* node = findAssetValueById(list, id);
    if (node) {
        node->data.currentValue = currentValue;
        node->data.maintenanceCost = maintenanceCost;
        node->data.propertyTax = propertyTax;
    }
    else {
        list.push_back(AssetValueDetails(id, currentValue, maintenanceCost, propertyTax));
    }
}

void initializeAssetValues(const LinkedList<Aset>& daftarAset, LinkedList<AssetValueDetails>& daftarDetailNilaiAset) {
    Node<Aset>* current = daftarAset.getHead();
    while (current) {
        if (!findAssetValueById(daftarDetailNilaiAset, current->data.id)) {
            daftarDetailNilaiAset.push_back(AssetValueDetails(current->data.id, 1000, 0, 0));
        }
        current = current->next;
    }
}

void printCategoryCounts(const LinkedList<CategoryCount>& counts) {
    Node<CategoryCount>* current = counts.getHead();
    while (current) {
        wxPrintf("Category: %s, Count: %d\n", current->data.kategori, current->data.count);
        current = current->next;
    }
}

void InitializeDefaultData() {
    if (daftarKategori.empty()) {
        daftarKategori.push_back(Kategori("Kost"));
        daftarKategori.push_back(Kategori("Gedung"));
        daftarKategori.push_back(Kategori("Rumah"));
        daftarKategori.push_back(Kategori("Emas"));
        daftarKategori.push_back(Kategori("Kendaraan"));
        daftarKategori.push_back(Kategori("Elektronik"));
    }
}

wxString GenerateUniqueAssetId(const wxString& type) {
    std::string prefix = type.SubString(0, 1).Upper().ToStdString();

    int count = 0;
    for (const auto& aset : daftarAset) {
        if (aset.id.StartsWith(prefix)) {
            count++;
        }
    }

    count++; // next ID

    std::ostringstream oss;
    oss << prefix << std::setw(4) << std::setfill('0') << count;
    return wxString(oss.str());
}

std::vector<wxString> GetCategoriesVector() {
    std::vector<wxString> categories;
    for (const auto& kategori : daftarKategori) {
        categories.push_back(kategori.nama);
    }
    return categories;
}

bool CategoryExists(const wxString& category) {
    for (const auto& kategori : daftarKategori) {
        if (kategori.nama.IsSameAs(category, false)) {
            return true;
        }
    }
    return false;
}

void AddCategory(const wxString& categoryName) {
    daftarKategori.push_back(Kategori(categoryName));
}

void DeleteCategory(const wxString& categoryName) {
    // Remove assets with this category first
    daftarAset.remove_if([&categoryName](const Aset& a) {
        return a.kategori.IsSameAs(categoryName, false);
        });

    // Remove asset values for deleted assets
    daftarDetailNilaiAset.remove_if([&categoryName](const AssetValueDetails& avd) {
        // Check if this asset still exists
        Node<Aset>* current = daftarAset.getHead();
        while (current) {
            if (current->data.id == avd.assetId) {
                return false; // Asset exists, don't remove
            }
            current = current->next;
        }
        return true; // Asset doesn't exist, remove its value details
        });

    // Remove the category
    daftarKategori.remove_if([&categoryName](const Kategori& k) {
        return k.nama.IsSameAs(categoryName, false);
        });
}

void AddAsset(const wxString& id, const wxString& nama, const wxString& kategori) {
    daftarAset.push_back(Aset(id, nama, kategori));
    // Initialize asset value with default values
    addOrUpdateAssetValue(daftarDetailNilaiAset, id, 1000, 0, 0);
}

void DeleteAsset(const wxString& assetId) {
    daftarAset.remove_if([&assetId](const Aset& a) {
        return a.id.IsSameAs(assetId, false);
        });

    // Remove asset value details
    daftarDetailNilaiAset.remove_if([&assetId](const AssetValueDetails& avd) {
        return avd.assetId.IsSameAs(assetId, false);
        });
}

std::vector<Aset> GetAssetsVector() {
    std::vector<Aset> assets;
    for (const auto& aset : daftarAset) {
        assets.push_back(aset);
    }
    return assets;
}

std::vector<Aset> SearchAssets(const wxString& searchTerm) {
    std::vector<Aset> results;
    for (const auto& aset : daftarAset) {
        if (aset.nama.Lower().Contains(searchTerm.Lower()) ||
            aset.id.Lower().Contains(searchTerm.Lower()) ||
            aset.kategori.Lower().Contains(searchTerm.Lower())) {
            results.push_back(aset);
        }
    }
    return results;
}

bool ConnectionExists(const wxString& fromId, const wxString& toId) {
    Node<AssetConnection>* current = daftarKoneksiAset.getHead();
    while (current) {
        if ((current->data.fromAssetId == fromId && current->data.toAssetId == toId) ||
            (current->data.fromAssetId == toId && current->data.toAssetId == fromId)) {
            return true;
        }
        current = current->next;
    }
    return false;
}

void AddAssetConnection(const wxString& fromId, const wxString& toId, int weight, const wxString& description = "") {
    if (!ConnectionExists(fromId, toId)) {
        daftarKoneksiAset.push_back(AssetConnection(fromId, toId, weight, description));
    }
}

void DeleteAssetConnection(const wxString& fromId, const wxString& toId) {
    daftarKoneksiAset.remove_if([&fromId, &toId](const AssetConnection& conn) {
        return (conn.fromAssetId == fromId && conn.toAssetId == toId) ||
            (conn.fromAssetId == toId && conn.toAssetId == fromId);
        });
}

std::vector<AssetConnection> GetAssetConnections() {
    std::vector<AssetConnection> connections;
    Node<AssetConnection>* current = daftarKoneksiAset.getHead();
    while (current) {
        connections.push_back(current->data);
        current = current->next;
    }
    return connections;
}

std::vector<AssetConnection> GetConnectionsForAsset(const wxString& assetId) {
    std::vector<AssetConnection> connections;
    Node<AssetConnection>* current = daftarKoneksiAset.getHead();
    while (current) {
        if (current->data.fromAssetId == assetId || current->data.toAssetId == assetId) {
            connections.push_back(current->data);
        }
        current = current->next;
    }
    return connections;
}

wxString GetAssetNameById(const wxString& assetId) {
    Node<Aset>* current = daftarAset.getHead();
    while (current) {
        if (current->data.id == assetId) {
            return current->data.nama;
        }
        current = current->next;
    }
    return assetId; // Return ID if name not found
}

wxString GenerateUniqueSubAssetId(const wxString& parentId, const wxString& type = "SUB") {
    int count = 0;
    Node<SubAsset>* current = daftarSubAset.getHead();
    while (current) {
        if (current->data.parentId == parentId) {
            count++;
        }
        current = current->next;
    }

    count++; // next ID
    std::ostringstream oss;
    oss << parentId << "-" << type << std::setw(3) << std::setfill('0') << count;
    return wxString(oss.str());
}

AssetTree<SubAsset>* getOrCreateAssetTree(const wxString& assetId) {
    // Find existing tree
    Node<AssetTree<SubAsset>*>* current = assetTrees.getHead();
    while (current) {
        if (current->data->getRoot() && current->data->getRoot()->data.parentId == assetId) {
            return current->data;
        }
        current = current->next;
    }

    // Create new tree
    AssetTree<SubAsset>* newTree = new AssetTree<SubAsset>();
    // Create root node for the main asset
    SubAsset rootData("ROOT-" + assetId, "Root of " + GetAssetNameById(assetId), assetId);
    TreeNode<SubAsset>* rootNode = new TreeNode<SubAsset>(rootData);
    newTree->setRoot(rootNode);
    assetTrees.push_back(newTree);

    return newTree;
}

void AddSubAsset(const wxString& parentAssetId, const wxString& parentSubAssetId,
    const wxString& nama, const wxString& description) {
    wxString subAssetId = GenerateUniqueSubAssetId(parentAssetId);
    SubAsset newSubAsset(subAssetId, nama, parentAssetId, description);

    daftarSubAset.push_back(newSubAsset);

    AssetTree<SubAsset>* tree = getOrCreateAssetTree(parentAssetId);
    TreeNode<SubAsset>* newNode = new TreeNode<SubAsset>(newSubAsset);

    if (parentSubAssetId.IsEmpty() || parentSubAssetId == "ROOT-" + parentAssetId) {
        // Add to root
        tree->getRoot()->addChild(newNode);
    }
    else {
        // Find parent sub-asset node
        TreeNode<SubAsset>* parentNode = tree->findNode(parentSubAssetId);
        if (parentNode) {
            parentNode->addChild(newNode);
        }
        else {
            // Fallback to root if parent not found
            tree->getRoot()->addChild(newNode);
        }
    }
}

void DeleteSubAsset(const wxString& subAssetId) {
    // Remove from linked list
    daftarSubAset.remove_if([&subAssetId](const SubAsset& sa) {
        return sa.id == subAssetId;
        });

    // Remove from tree
    Node<AssetTree<SubAsset>*>* treeNode = assetTrees.getHead();
    while (treeNode) {
        TreeNode<SubAsset>* nodeToDelete = treeNode->data->findNode(subAssetId);
        if (nodeToDelete && nodeToDelete->parent) {
            nodeToDelete->parent->removeChild(nodeToDelete);
            delete nodeToDelete;
            break;
        }
        treeNode = treeNode->next;
    }
}

std::vector<SubAsset> GetSubAssetsForAsset(const wxString& assetId) {
    std::vector<SubAsset> result;
    Node<SubAsset>* current = daftarSubAset.getHead();
    while (current) {
        if (current->data.parentId == assetId) {
            result.push_back(current->data);
        }
        current = current->next;
    }
    return result;
}

void UpdateSubAssetRental(const wxString& subAssetId, bool isRented,
    const wxString& renterName, int rentalPrice) {
    Node<SubAsset>* current = daftarSubAset.getHead();
    while (current) {
        if (current->data.id == subAssetId) {
            current->data.isRented = isRented;
            current->data.renterName = renterName;
            current->data.rentalPrice = rentalPrice;
            break;
        }
        current = current->next;
    }
}

wxString GenerateUniqueTenderProjectId() {
    static int counter = 1;
    std::ostringstream oss;
    oss << "TNR" << std::setfill('0') << std::setw(4) << counter++;
    return wxString(oss.str());
}

void AddTenderProject(const wxString& nama, const wxString& kategori,
    const wxString& description, const wxString& tenderDate,
    int estimatedValue, const wxString& clientName, int priority) {
    wxString id = GenerateUniqueTenderProjectId();
    TenderProject newProject(id, nama, kategori, description, "Pending",
        tenderDate, estimatedValue, clientName, priority);
    queueTenderProjects.enqueue(newProject);
}

void DeleteTenderProject(const wxString& projectId) {
    queueTenderProjects.remove_if([&projectId](const TenderProject& project) {
        return project.id == projectId;
        });
}

void UpdateTenderProjectStatus(const wxString& projectId, const wxString& newStatus) {
    for (auto it = queueTenderProjects.begin(); it != queueTenderProjects.end(); ++it) {
        if ((*it).id == projectId) {
            (*it).status = newStatus;
            break;
        }
    }
}

// NEW: Asset History Helper Functions
wxString GetCurrentTimeString() {
    wxDateTime now = wxDateTime::Now();
    return now.Format("%d/%m/%Y %H:%M");
}

void AddToAssetHistory(const wxString& assetId, const wxString& assetName, const wxString& assetType) {
    // Cek apakah aset yang sama sudah ada di top stack
    AssetHistory* topItem = assetHistoryStack.peek();
    if (topItem && topItem->assetId == assetId) {
        return; // Jangan tambah duplikat berturut-turut
    }

    // Hapus item yang sama dari stack (jika ada)
    Stack<AssetHistory> tempStack;
    while (!assetHistoryStack.empty()) {
        AssetHistory* item = assetHistoryStack.peek();
        if (item->assetId != assetId) {
            tempStack.push(*item);
        }
        assetHistoryStack.pop();
    }

    // Kembalikan item ke stack (kecuali yang duplikat)
    while (!tempStack.empty()) {
        AssetHistory* item = tempStack.peek();
        assetHistoryStack.push(*item);
        tempStack.pop();
    }

    // Tambahkan item baru ke top
    AssetHistory newHistory(assetId, assetName, assetType, GetCurrentTimeString());
    assetHistoryStack.push(newHistory);
}

std::vector<AssetHistory> GetAssetHistoryVector() {
    std::vector<AssetHistory> result;
    for (auto& history : assetHistoryStack) {
        result.push_back(history);
    }
    return result;
}

std::vector<TenderProject> GetTenderProjectsVector() {
    std::vector<TenderProject> result;
    for (auto it = queueTenderProjects.begin(); it != queueTenderProjects.end(); ++it) {
        result.push_back(*it);
    }
    return result;
}

TenderProject* GetNextTenderProject() {
    return queueTenderProjects.peek();
}

void ProcessNextTenderProject() {
    queueTenderProjects.dequeue();
}


MainFrame::MainFrame(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600))
{
    InitializeDefaultData();

    // Main panel
    wxPanel* panel = new wxPanel(this);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);

    // Left panel for buttons
    wxPanel* buttonPanel = new wxPanel(panel);
    wxBoxSizer* buttonSizer = new wxBoxSizer(wxVERTICAL);

    // Create styled buttons
    wxButton* btn1 = new wxButton(buttonPanel, 1001, "Manajemen Kategori");
    buttonSizer->Add(btn1, 0, wxEXPAND | wxALL, 5);
    btn1->Bind(wxEVT_BUTTON, &MainFrame::OnButtonClicked, this);

    wxButton* btn2 = new wxButton(buttonPanel, 1002, "Manajemen Aset");
    buttonSizer->Add(btn2, 0, wxEXPAND | wxALL, 5);
    btn2->Bind(wxEVT_BUTTON, &MainFrame::OnButtonClicked, this);

    wxButton* btn3 = new wxButton(buttonPanel, 1003, "Daftar Aset");
    buttonSizer->Add(btn3, 0, wxEXPAND | wxALL, 5);
    btn3->Bind(wxEVT_BUTTON, &MainFrame::OnButtonClicked, this);

    wxButton* btn4 = new wxButton(buttonPanel, 1004, "Pencarian Aset");
    buttonSizer->Add(btn4, 0, wxEXPAND | wxALL, 5);
    btn4->Bind(wxEVT_BUTTON, &MainFrame::OnButtonClicked, this);

    wxButton* btn5 = new wxButton(buttonPanel, 1005, "Statistik");
    buttonSizer->Add(btn5, 0, wxEXPAND | wxALL, 5);
    btn5->Bind(wxEVT_BUTTON, &MainFrame::OnButtonClicked, this);

    wxButton* btn6 = new wxButton(buttonPanel, 1006, "Manajemen Harga");
    buttonSizer->Add(btn6, 0, wxEXPAND | wxALL, 5);
    btn6->Bind(wxEVT_BUTTON, &MainFrame::OnButtonClicked, this);

    wxButton* btn7 = new wxButton(buttonPanel, 1007, "Hubungkan Aset");
    buttonSizer->Add(btn7, 0, wxEXPAND | wxALL, 5);
    btn7->Bind(wxEVT_BUTTON, &MainFrame::OnButtonClicked, this);

    wxButton* btn8 = new wxButton(buttonPanel, 1008, "Tree SubAset");
    buttonSizer->Add(btn8, 0, wxEXPAND | wxALL, 5);
    btn8->Bind(wxEVT_BUTTON, &MainFrame::OnButtonClicked, this);

    wxButton* btn9 = new wxButton(buttonPanel, 1009, "Manajemen Tender");
    buttonSizer->Add(btn9, 0, wxEXPAND | wxALL, 5);
    btn9->Bind(wxEVT_BUTTON, &MainFrame::OnButtonClicked, this);

    wxButton* btn10 = new wxButton(buttonPanel, 1010, "History Stack");
    buttonSizer->Add(btn10, 0, wxEXPAND | wxALL, 5);
    btn10->Bind(wxEVT_BUTTON, &MainFrame::OnButtonClicked, this);

    buttonPanel->SetSizer(buttonSizer);

    // Right panel for content
    contentPanel = new wxPanel(panel);
    contentSizer = new wxBoxSizer(wxVERTICAL);
    contentPanel->SetSizer(contentSizer);

    mainSizer->Add(buttonPanel, 0, wxEXPAND | wxALL, 10);
    mainSizer->Add(contentPanel, 1, wxEXPAND | wxALL, 10);

    panel->SetSizer(mainSizer);

    // Show default page
    ShowMenuPage(1);
}

void MainFrame::ShowSubAssetManagementPage() {
    contentSizer->Clear(true);

    wxStaticText* title = new wxStaticText(contentPanel, wxID_ANY, "Manajemen Sub-Aset");
    wxFont font = title->GetFont();
    font.SetPointSize(16);
    font.SetWeight(wxFONTWEIGHT_BOLD);
    title->SetFont(font);

    contentSizer->Add(title, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 15);

    // Get all assets for dropdown
    std::vector<Aset> assets = GetAssetsVector();
    if (assets.empty()) {
        wxStaticText* noAssets = new wxStaticText(contentPanel, wxID_ANY,
            "Tidak ada aset yang tersedia. Tambahkan aset terlebih dahulu.");
        contentSizer->Add(noAssets, 0, wxALL | wxALIGN_CENTER, 20);
        return;
    }

    // Section 1: Add new sub-asset
    wxStaticBox* addBox = new wxStaticBox(contentPanel, wxID_ANY, "Tambah Sub-Aset Baru");
    wxStaticBoxSizer* addSizer = new wxStaticBoxSizer(addBox, wxVERTICAL);

    // Parent Asset dropdown
    wxStaticText* parentAssetLabel = new wxStaticText(contentPanel, wxID_ANY, "Pilih Aset Utama:");
    parentAssetDropdown = new wxChoice(contentPanel, wxID_ANY);

    for (const auto& asset : assets) {
        wxString displayText = asset.id + " - " + asset.nama;
        parentAssetDropdown->Append(displayText);
    }
    if (!assets.empty()) {
        parentAssetDropdown->SetSelection(0);
    }

    // Parent Sub-Asset dropdown (for hierarchical structure)
    wxStaticText* parentSubAssetLabel = new wxStaticText(contentPanel, wxID_ANY, "Parent Sub-Aset (opsional):");
    parentSubAssetDropdown = new wxChoice(contentPanel, wxID_ANY);
    parentSubAssetDropdown->Append("-- Root Level --");
    parentSubAssetDropdown->SetSelection(0);

    // Update parent sub-asset dropdown when parent asset changes
    parentAssetDropdown->Bind(wxEVT_CHOICE, [this](wxCommandEvent&) {
        this->UpdateParentSubAssetDropdown();
        });

    // Sub-asset name input
    wxStaticText* nameLabel = new wxStaticText(contentPanel, wxID_ANY, "Nama Sub-Aset:");
    subAssetNameInput = new wxTextCtrl(contentPanel, wxID_ANY);

    // Description input
    wxStaticText* descLabel = new wxStaticText(contentPanel, wxID_ANY, "Deskripsi:");
    subAssetDescInput = new wxTextCtrl(contentPanel, wxID_ANY, "", wxDefaultPosition,
        wxDefaultSize, wxTE_MULTILINE);
    subAssetDescInput->SetMinSize(wxSize(-1, 60));

    wxButton* addSubAssetBtn = new wxButton(contentPanel, wxID_ANY, "Tambah Sub-Aset");
    addSubAssetBtn->Bind(wxEVT_BUTTON, &MainFrame::OnSubmitSubAsset, this);

    addSizer->Add(parentAssetLabel, 0, wxTOP, 5);
    addSizer->Add(parentAssetDropdown, 0, wxEXPAND | wxBOTTOM, 10);
    addSizer->Add(parentSubAssetLabel, 0, wxTOP, 5);
    addSizer->Add(parentSubAssetDropdown, 0, wxEXPAND | wxBOTTOM, 10);
    addSizer->Add(nameLabel, 0, wxTOP, 5);
    addSizer->Add(subAssetNameInput, 0, wxEXPAND | wxBOTTOM, 10);
    addSizer->Add(descLabel, 0, wxTOP, 5);
    addSizer->Add(subAssetDescInput, 0, wxEXPAND | wxBOTTOM, 10);
    addSizer->Add(addSubAssetBtn, 0, wxALIGN_CENTER | wxTOP, 10);

    contentSizer->Add(addSizer, 0, wxEXPAND | wxALL, 10);

    // Section 2: Display asset tree
    wxStaticBox* displayBox = new wxStaticBox(contentPanel, wxID_ANY, "Struktur Sub-Aset");
    wxStaticBoxSizer* displaySizer = new wxStaticBoxSizer(displayBox, wxVERTICAL);

    treeDisplayPanel = new wxScrolledWindow(contentPanel);
    treeDisplayPanel->SetScrollRate(5, 5);
    treeDisplayPanel->SetMinSize(wxSize(-1, 300));
    treeDisplaySizer = new wxBoxSizer(wxVERTICAL);
    treeDisplayPanel->SetSizer(treeDisplaySizer);

    displaySizer->Add(treeDisplayPanel, 1, wxEXPAND);
    contentSizer->Add(displaySizer, 1, wxEXPAND | wxALL, 10);

    // Initialize display
    UpdateParentSubAssetDropdown();
    DisplayAssetTrees();
}

void MainFrame::UpdateParentSubAssetDropdown() {
    if (!parentAssetDropdown || !parentSubAssetDropdown) return;

    int selection = parentAssetDropdown->GetSelection();
    if (selection == wxNOT_FOUND) return;

    wxString selectedText = parentAssetDropdown->GetString(selection);
    wxString assetId = selectedText.BeforeFirst(' ');

    parentSubAssetDropdown->Clear();
    parentSubAssetDropdown->Append("-- Root Level --");

    // Add existing sub-assets for this asset
    std::vector<SubAsset> subAssets = GetSubAssetsForAsset(assetId);
    for (const auto& subAsset : subAssets) {
        wxString displayText = subAsset.id + " - " + subAsset.nama;
        parentSubAssetDropdown->Append(displayText);
    }

    parentSubAssetDropdown->SetSelection(0);
}

void MainFrame::DisplayAssetTrees() {
    if (!treeDisplaySizer) return;

    treeDisplaySizer->Clear(true);

    std::vector<Aset> assets = GetAssetsVector();

    for (const auto& asset : assets) {
        std::vector<SubAsset> subAssets = GetSubAssetsForAsset(asset.id);
        if (!subAssets.empty()) {
            // Asset header
            wxString headerText = wxString::Format(" %s (%s)", asset.nama, asset.id);
            wxStaticText* header = new wxStaticText(treeDisplayPanel, wxID_ANY, headerText);
            wxFont headerFont = header->GetFont();
            headerFont.SetWeight(wxFONTWEIGHT_BOLD);
            headerFont.SetPointSize(headerFont.GetPointSize() + 1);
            header->SetFont(headerFont);
            treeDisplaySizer->Add(header, 0, wxALL, 5);

            // Display tree structure
            AssetTree<SubAsset>* tree = getOrCreateAssetTree(asset.id);
            if (tree->getRoot()) {
                DisplayTreeNode(tree->getRoot(), 1);
            }

            // Add separator
            wxStaticLine* separator = new wxStaticLine(treeDisplayPanel);
            treeDisplaySizer->Add(separator, 0, wxEXPAND | wxTOP | wxBOTTOM, 10);
        }
    }

    if (treeDisplaySizer->GetItemCount() == 0) {
        wxStaticText* noData = new wxStaticText(treeDisplayPanel, wxID_ANY,
            "Belum ada sub-aset yang dibuat.");
        treeDisplaySizer->Add(noData, 0, wxALL, 20);
    }

    treeDisplayPanel->Layout();
    treeDisplayPanel->FitInside();
}

void MainFrame::DisplayTreeNode(TreeNode<SubAsset>* node, int depth) {
    if (!node) return;

    // Skip root node display
    if (depth > 1 || !node->data.id.StartsWith("ROOT-")) {
        wxBoxSizer* nodeSizer = new wxBoxSizer(wxHORIZONTAL);

        // Indentation
        wxString indent = wxString(' ', (depth - 1) * 4);
        wxString prefix = depth > 1 ? "|-- " : "+-- ";
        // Node info
        wxString nodeText = indent + prefix;
        if (node->data.isRented) {
            nodeText += "[R] ";  // [R] for Rented
        }
        else {
            nodeText += "[A] ";  // [A] for Asset
        }
        nodeText += node->data.nama + " (" + node->data.id + ")";

        if (node->data.isRented) {
            nodeText += wxString::Format(" - Disewa: %s (Rp %d)",
                node->data.renterName, node->data.rentalPrice);
        }

        wxStaticText* nodeLabel = new wxStaticText(treeDisplayPanel, wxID_ANY, nodeText);

        // Action buttons
        wxButton* editBtn = new wxButton(treeDisplayPanel, wxID_ANY, "Edit",
            wxDefaultPosition, wxSize(50, 25));
        wxButton* deleteBtn = new wxButton(treeDisplayPanel, wxID_ANY, "Hapus",
            wxDefaultPosition, wxSize(50, 25));

        // Store node data in buttons
        editBtn->SetClientData(new wxString(node->data.id));
        deleteBtn->SetClientData(new wxString(node->data.id));

        editBtn->Bind(wxEVT_BUTTON, &MainFrame::OnEditSubAsset, this);
        deleteBtn->Bind(wxEVT_BUTTON, &MainFrame::OnDeleteSubAsset, this);

        nodeSizer->Add(nodeLabel, 1, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);
        nodeSizer->Add(editBtn, 0, wxRIGHT, 5);
        nodeSizer->Add(deleteBtn, 0);

        treeDisplaySizer->Add(nodeSizer, 0, wxEXPAND | wxALL, 2);
    }

    // Display children
    Node<TreeNode<SubAsset>*>* child = node->children.getHead();
    while (child) {
        DisplayTreeNode(child->data, depth + 1);
        child = child->next;
    }
}

void MainFrame::OnSubmitSubAsset(wxCommandEvent& event) {
    wxString subAssetName = subAssetNameInput->GetValue().Trim();
    wxString description = subAssetDescInput->GetValue().Trim();

    if (subAssetName.IsEmpty()) {
        wxMessageBox("Nama sub-aset tidak boleh kosong.", "Error", wxOK | wxICON_ERROR);
        return;
    }

    int assetSelection = parentAssetDropdown->GetSelection();
    if (assetSelection == wxNOT_FOUND) {
        wxMessageBox("Pilih aset utama.", "Error", wxOK | wxICON_ERROR);
        return;
    }

    wxString assetText = parentAssetDropdown->GetString(assetSelection);
    wxString assetId = assetText.BeforeFirst(' ');

    wxString parentSubAssetId = "";
    int subAssetSelection = parentSubAssetDropdown->GetSelection();
    if (subAssetSelection > 0) { // Not "-- Root Level --"
        wxString subAssetText = parentSubAssetDropdown->GetString(subAssetSelection);
        parentSubAssetId = subAssetText.BeforeFirst(' ');
    }

    AddSubAsset(assetId, parentSubAssetId, subAssetName, description);

    // Clear inputs
    subAssetNameInput->Clear();
    subAssetDescInput->Clear();
    parentSubAssetDropdown->SetSelection(0);

    wxMessageBox("Sub-aset berhasil ditambahkan.", "Sukses", wxOK | wxICON_INFORMATION);

    // Refresh display
    UpdateParentSubAssetDropdown();
    DisplayAssetTrees();
}

void MainFrame::OnEditSubAsset(wxCommandEvent& event) {
    wxButton* btn = dynamic_cast<wxButton*>(event.GetEventObject());
    if (!btn) return;

    wxString* subAssetId = static_cast<wxString*>(btn->GetClientData());
    if (!subAssetId) return;

    // Find the sub-asset
    Node<SubAsset>* current = daftarSubAset.getHead();
    SubAsset* targetSubAsset = nullptr;
    while (current) {
        if (current->data.id == *subAssetId) {
            targetSubAsset = &current->data;
            break;
        }
        current = current->next;
    }

    if (!targetSubAsset) {
        wxMessageBox("Sub-aset tidak ditemukan.", "Error", wxOK | wxICON_ERROR);
        return;
    }

    ShowEditSubAssetDialog(targetSubAsset);
}

void MainFrame::ShowEditSubAssetDialog(SubAsset* subAsset) {
    wxDialog dlg(this, wxID_ANY, "Edit Sub-Aset: " + subAsset->nama,
        wxDefaultPosition, wxSize(400, 350));
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    // Name
    sizer->Add(new wxStaticText(&dlg, wxID_ANY, "Nama:"), 0, wxALL, 5);
    wxTextCtrl* nameCtrl = new wxTextCtrl(&dlg, wxID_ANY, subAsset->nama);
    sizer->Add(nameCtrl, 0, wxEXPAND | wxALL, 5);

    // Description
    sizer->Add(new wxStaticText(&dlg, wxID_ANY, "Deskripsi:"), 0, wxALL, 5);
    wxTextCtrl* descCtrl = new wxTextCtrl(&dlg, wxID_ANY, subAsset->description,
        wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
    descCtrl->SetMinSize(wxSize(-1, 60));
    sizer->Add(descCtrl, 0, wxEXPAND | wxALL, 5);

    // Rental status
    wxCheckBox* rentalCheck = new wxCheckBox(&dlg, wxID_ANY, "Sedang Disewa");
    rentalCheck->SetValue(subAsset->isRented);
    sizer->Add(rentalCheck, 0, wxALL, 5);

    // Renter name
    sizer->Add(new wxStaticText(&dlg, wxID_ANY, "Nama Penyewa:"), 0, wxALL, 5);
    wxTextCtrl* renterCtrl = new wxTextCtrl(&dlg, wxID_ANY, subAsset->renterName);
    sizer->Add(renterCtrl, 0, wxEXPAND | wxALL, 5);

    // Rental price
    sizer->Add(new wxStaticText(&dlg, wxID_ANY, "Harga Sewa:"), 0, wxALL, 5);
    wxTextCtrl* priceCtrl = new wxTextCtrl(&dlg, wxID_ANY,
        wxString::Format("%d", subAsset->rentalPrice));
    sizer->Add(priceCtrl, 0, wxEXPAND | wxALL, 5);

    sizer->Add(dlg.CreateButtonSizer(wxOK | wxCANCEL), 0, wxALL | wxALIGN_CENTER, 10);
    dlg.SetSizer(sizer);
    dlg.CenterOnParent();

    if (dlg.ShowModal() == wxID_OK) {
        subAsset->nama = nameCtrl->GetValue().Trim();
        subAsset->description = descCtrl->GetValue().Trim();
        subAsset->isRented = rentalCheck->GetValue();
        subAsset->renterName = renterCtrl->GetValue().Trim();

        long price;
        if (priceCtrl->GetValue().ToLong(&price)) {
            subAsset->rentalPrice = (int)price;
        }

        wxMessageBox("Sub-aset berhasil diperbarui.", "Sukses", wxOK | wxICON_INFORMATION);
        DisplayAssetTrees();
    }
    AddToAssetHistory(subAsset->id, subAsset->nama, "SubAsset");
}

void MainFrame::OnDeleteSubAsset(wxCommandEvent& event) {
    wxButton* btn = dynamic_cast<wxButton*>(event.GetEventObject());
    if (!btn) return;

    wxString* subAssetId = static_cast<wxString*>(btn->GetClientData());
    if (!subAssetId) return;

    // Find sub-asset name for confirmation
    wxString subAssetName = *subAssetId;
    Node<SubAsset>* current = daftarSubAset.getHead();
    while (current) {
        if (current->data.id == *subAssetId) {
            subAssetName = current->data.nama;
            break;
        }
        current = current->next;
    }

    int result = wxMessageBox(
        wxString::Format("Apakah Anda yakin ingin menghapus sub-aset '%s'?\n"
            "Semua sub-aset turunannya juga akan dihapus.", subAssetName),
        "Konfirmasi Hapus", wxYES_NO | wxICON_QUESTION);

    if (result == wxYES) {
        DeleteSubAsset(*subAssetId);
        wxMessageBox("Sub-aset berhasil dihapus.", "Sukses", wxOK | wxICON_INFORMATION);
        UpdateParentSubAssetDropdown();
        DisplayAssetTrees();
    }
}


void MainFrame::ShowAssetValueManagementPanel() {
    contentPanel->DestroyChildren();

    wxBoxSizer* vSizer = new wxBoxSizer(wxVERTICAL);


    // Header
    wxStaticText* header = new wxStaticText(contentPanel, wxID_ANY, "Manajemen Harga Aset");
    wxFont headerFont = header->GetFont();
    headerFont.SetPointSize(16);
    headerFont.SetWeight(wxFONTWEIGHT_BOLD);
    header->SetFont(headerFont);
    vSizer->Add(header, 0, wxALL | wxALIGN_CENTER, 10);

    // Ensure asset values are initialized
    initializeAssetValues(daftarAset, daftarDetailNilaiAset);

    // Display asset values
    Node<AssetValueDetails>* current = daftarDetailNilaiAset.getHead();
    while (current) {
        wxString info = wxString::Format("ID: %s | Harga: %d | Maintenance: %d | Pajak: %d",
            current->data.assetId,
            current->data.currentValue,
            current->data.maintenanceCost,
            current->data.propertyTax);
        wxStaticText* stat = new wxStaticText(contentPanel, wxID_ANY, info);
        vSizer->Add(stat, 0, wxALL, 5);

        wxButton* editBtn = new wxButton(contentPanel, wxID_ANY, "Edit " + current->data.assetId);
        vSizer->Add(editBtn, 0, wxALL, 5);

        editBtn->Bind(wxEVT_BUTTON, [this, node = current](wxCommandEvent&) {
            this->ShowEditAssetValueDialog(node);
            });

        current = current->next;
    }

    contentPanel->SetSizer(vSizer);
    contentPanel->Layout();
}

void MainFrame::ShowEditAssetValueDialog(Node<AssetValueDetails>* node) {
    wxString assetName = node->data.assetId;

    for (auto it = daftarAset.begin(); it != daftarAset.end(); ++it) {
        if ((*it).id == node->data.assetId) {
            assetName = (*it).nama;
            break;
        }
    }

    for (auto& aset : daftarAset) {
        if (aset.id == node->data.assetId) {
            assetName = aset.nama;
            break;
        }
    }
    AddToAssetHistory(node->data.assetId, assetName, "Asset Value");


    wxDialog dlg(this, wxID_ANY, "Edit Harga Aset: " + assetName, wxDefaultPosition, wxSize(400, 300));
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    wxTextCtrl* txtHarga = new wxTextCtrl(&dlg, wxID_ANY, wxString::Format("%d", node->data.currentValue));
    wxTextCtrl* txtMaint = new wxTextCtrl(&dlg, wxID_ANY, wxString::Format("%d", node->data.maintenanceCost));
    wxTextCtrl* txtPajak = new wxTextCtrl(&dlg, wxID_ANY, wxString::Format("%d", node->data.propertyTax));

    sizer->Add(new wxStaticText(&dlg, wxID_ANY, "Harga Terkini:"), 0, wxALL, 5);
    sizer->Add(txtHarga, 0, wxALL | wxEXPAND, 5);
    sizer->Add(new wxStaticText(&dlg, wxID_ANY, "Biaya Maintenance:"), 0, wxALL, 5);
    sizer->Add(txtMaint, 0, wxALL | wxEXPAND, 5);
    sizer->Add(new wxStaticText(&dlg, wxID_ANY, "Pajak:"), 0, wxALL, 5);
    sizer->Add(txtPajak, 0, wxALL | wxEXPAND, 5);

    sizer->Add(dlg.CreateButtonSizer(wxOK | wxCANCEL), 0, wxALL | wxALIGN_CENTER, 10);
    dlg.SetSizer(sizer);
    dlg.SetMinSize(wxSize(400, 300)); // Set a minimum size for the dialog
    dlg.Fit(); // Adjust to contents
    dlg.CenterOnParent();

    if (dlg.ShowModal() == wxID_OK) {
        long valHarga, valMaint, valPajak;
        if (txtHarga->GetValue().ToLong(&valHarga) &&
            txtMaint->GetValue().ToLong(&valMaint) &&
            txtPajak->GetValue().ToLong(&valPajak)) {

            node->data.currentValue = (int)valHarga;
            node->data.maintenanceCost = (int)valMaint;
            node->data.propertyTax = (int)valPajak;

            wxMessageBox("Data harga aset berhasil diperbarui.", "Sukses", wxOK | wxICON_INFORMATION);
            ShowAssetValueManagementPanel();
        }
        else {
            wxMessageBox("Input tidak valid.", "Error", wxOK | wxICON_ERROR);
        }
    }

}


void MainFrame::ShowAssetGraphManagementPage() {
    wxStaticText* title = new wxStaticText(contentPanel, wxID_ANY, "Manajemen Graph Aset");
    wxFont font = title->GetFont();
    font.SetPointSize(16);
    font.SetWeight(wxFONTWEIGHT_BOLD);
    title->SetFont(font);

    contentSizer->Add(title, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 15);

    // Section 1: Add new connection
    wxStaticBox* addBox = new wxStaticBox(contentPanel, wxID_ANY, "Tambah Koneksi Baru");
    wxStaticBoxSizer* addSizer = new wxStaticBoxSizer(addBox, wxVERTICAL);

    // Get all assets for dropdowns
    std::vector<Aset> assets = GetAssetsVector();
    if (assets.empty()) {
        wxStaticText* noAssets = new wxStaticText(contentPanel, wxID_ANY,
            "Tidak ada aset yang tersedia. Tambahkan aset terlebih dahulu.");
        contentSizer->Add(noAssets, 0, wxALL | wxALIGN_CENTER, 20);
        return;
    }

    wxString* assetArray = new wxString[assets.size()];
    for (size_t i = 0; i < assets.size(); i++) {
        assetArray[i] = assets[i].id + " - " + assets[i].nama;
    }

    // From Asset dropdown
    wxStaticText* fromLabel = new wxStaticText(contentPanel, wxID_ANY, "Aset Asal:");
    fromAssetDropdown = new wxChoice(contentPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize,
        static_cast<int>(assets.size()), assetArray);
    if (assets.size() > 0) {
        fromAssetDropdown->SetSelection(0);
    }

    // To Asset dropdown
    wxStaticText* toLabel = new wxStaticText(contentPanel, wxID_ANY, "Aset Tujuan:");
    toAssetDropdown = new wxChoice(contentPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize,
        static_cast<int>(assets.size()), assetArray);
    if (assets.size() > 1) {
        toAssetDropdown->SetSelection(1);
    }

    delete[] assetArray;

    // Weight input
    wxStaticText* weightLabel = new wxStaticText(contentPanel, wxID_ANY, "Bobot Koneksi:");
    weightInput = new wxTextCtrl(contentPanel, wxID_ANY, "1");

    // Description input
    wxStaticText* descLabel = new wxStaticText(contentPanel, wxID_ANY, "Deskripsi (opsional):");
    descriptionInput = new wxTextCtrl(contentPanel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
    descriptionInput->SetMinSize(wxSize(-1, 60));

    wxButton* addConnectionBtn = new wxButton(contentPanel, wxID_ANY, "Tambah Koneksi");
    addConnectionBtn->Bind(wxEVT_BUTTON, &MainFrame::OnSubmitConnection, this);

    addSizer->Add(fromLabel, 0, wxTOP, 5);
    addSizer->Add(fromAssetDropdown, 0, wxEXPAND | wxBOTTOM, 10);
    addSizer->Add(toLabel, 0, wxTOP, 5);
    addSizer->Add(toAssetDropdown, 0, wxEXPAND | wxBOTTOM, 10);
    addSizer->Add(weightLabel, 0, wxTOP, 5);
    addSizer->Add(weightInput, 0, wxEXPAND | wxBOTTOM, 10);
    addSizer->Add(descLabel, 0, wxTOP, 5);
    addSizer->Add(descriptionInput, 0, wxEXPAND | wxBOTTOM, 10);
    addSizer->Add(addConnectionBtn, 0, wxALIGN_CENTER | wxTOP, 10);

    contentSizer->Add(addSizer, 0, wxEXPAND | wxALL, 10);

    // Section 2: View connections for specific asset
    wxStaticBox* viewBox = new wxStaticBox(contentPanel, wxID_ANY, "Lihat Koneksi Aset");
    wxStaticBoxSizer* viewSizer = new wxStaticBoxSizer(viewBox, wxVERTICAL);

    wxStaticText* selectLabel = new wxStaticText(contentPanel, wxID_ANY, "Pilih Aset:");

    // Recreate asset array for the third dropdown
    wxString* assetArray2 = new wxString[assets.size()];
    for (size_t i = 0; i < assets.size(); i++) {
        assetArray2[i] = assets[i].id + " - " + assets[i].nama;
    }

    selectedAssetDropdown = new wxChoice(contentPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize,
        static_cast<int>(assets.size()), assetArray2);
    if (assets.size() > 0) {
        selectedAssetDropdown->SetSelection(0);
    }

    delete[] assetArray2;

    wxButton* viewConnectionsBtn = new wxButton(contentPanel, wxID_ANY, "Lihat Koneksi");
    viewConnectionsBtn->Bind(wxEVT_BUTTON, &MainFrame::OnViewAssetConnections, this);

    viewSizer->Add(selectLabel, 0, wxTOP, 5);
    viewSizer->Add(selectedAssetDropdown, 0, wxEXPAND | wxBOTTOM, 10);
    viewSizer->Add(viewConnectionsBtn, 0, wxALIGN_CENTER | wxTOP, 10);

    contentSizer->Add(viewSizer, 0, wxEXPAND | wxALL, 10);

    // Section 3: Display all connections
    wxStaticBox* allBox = new wxStaticBox(contentPanel, wxID_ANY, "Semua Koneksi Aset");
    wxStaticBoxSizer* allSizer = new wxStaticBoxSizer(allBox, wxVERTICAL);

    graphResultsPanel = new wxScrolledWindow(contentPanel);
    graphResultsPanel->SetScrollRate(5, 5);
    graphResultsPanel->SetMinSize(wxSize(-1, 200));
    graphResultsSizer = new wxBoxSizer(wxVERTICAL);
    graphResultsPanel->SetSizer(graphResultsSizer);

    // Display existing connections
    std::vector<AssetConnection> connections = GetAssetConnections();

    if (connections.empty()) {
        wxStaticText* noConnections = new wxStaticText(graphResultsPanel, wxID_ANY,
            "Belum ada koneksi antar aset.");
        graphResultsSizer->Add(noConnections, 0, wxALL, 10);
    }
    else {
        for (const auto& conn : connections) {
            wxBoxSizer* connSizer = new wxBoxSizer(wxHORIZONTAL);

            wxString fromName = GetAssetNameById(conn.fromAssetId);
            wxString toName = GetAssetNameById(conn.toAssetId);

            wxString connText = wxString::Format("%s <-> %s (Bobot: %d)",
                fromName, toName, conn.weight);

            if (!conn.description.IsEmpty()) {
                connText += " - " + conn.description;
            }

            wxStaticText* connLabel = new wxStaticText(graphResultsPanel, wxID_ANY, connText);
            wxButton* deleteBtn = new wxButton(graphResultsPanel, wxID_ANY, "Hapus");

            // Store connection info in button's client data
            deleteBtn->SetClientData(new AssetConnection(conn));
            deleteBtn->Bind(wxEVT_BUTTON, &MainFrame::OnDeleteConnection, this);

            connSizer->Add(connLabel, 1, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);
            connSizer->Add(deleteBtn, 0);

            graphResultsSizer->Add(connSizer, 0, wxEXPAND | wxALL, 5);
        }
    }

    allSizer->Add(graphResultsPanel, 1, wxEXPAND);
    contentSizer->Add(allSizer, 1, wxEXPAND | wxALL, 10);
}

void MainFrame::OnSubmitConnection(wxCommandEvent& event) {
    int fromSelection = fromAssetDropdown->GetSelection();
    int toSelection = toAssetDropdown->GetSelection();

    if (fromSelection == wxNOT_FOUND || toSelection == wxNOT_FOUND) {
        wxMessageBox("Pilih aset asal dan tujuan.", "Error", wxOK | wxICON_ERROR);
        return;
    }

    if (fromSelection == toSelection) {
        wxMessageBox("Aset asal dan tujuan harus berbeda.", "Error", wxOK | wxICON_ERROR);
        return;
    }

    wxString weightStr = weightInput->GetValue().Trim();
    long weight;
    if (!weightStr.ToLong(&weight) || weight <= 0) {
        wxMessageBox("Bobot harus berupa angka positif.", "Error", wxOK | wxICON_ERROR);
        return;
    }

    // Extract asset IDs from dropdown selections
    wxString fromText = fromAssetDropdown->GetString(fromSelection);
    wxString toText = toAssetDropdown->GetString(toSelection);

    wxString fromId = fromText.BeforeFirst(' ');
    wxString toId = toText.BeforeFirst(' ');

    if (ConnectionExists(fromId, toId)) {
        wxMessageBox("Koneksi antara aset ini sudah ada.", "Error", wxOK | wxICON_ERROR);
        return;
    }

    wxString description = descriptionInput->GetValue().Trim();
    AddAssetConnection(fromId, toId, static_cast<int>(weight), description);

    // Clear inputs
    weightInput->SetValue("1");
    descriptionInput->Clear();

    wxMessageBox("Koneksi berhasil ditambahkan.", "Sukses", wxOK | wxICON_INFORMATION);

    // Refresh the page
    ShowMenuPage(7);
}

void MainFrame::ShowTenderProjectManagementPage() {
    // Title
    wxStaticText* title = new wxStaticText(contentPanel, wxID_ANY, "Manajemen Proyek Tender");
    wxFont titleFont = title->GetFont();
    titleFont.SetPointSize(16);
    titleFont.SetWeight(wxFONTWEIGHT_BOLD);
    title->SetFont(titleFont);
    contentSizer->Add(title, 0, wxALL | wxALIGN_CENTER, 10);

    // Add New Tender Project Section
    wxStaticBox* addBox = new wxStaticBox(contentPanel, wxID_ANY, "Tambah Proyek Tender Baru");
    wxStaticBoxSizer* addSizer = new wxStaticBoxSizer(addBox, wxVERTICAL);

    // Project Name
    wxBoxSizer* nameSizer = new wxBoxSizer(wxHORIZONTAL);
    nameSizer->Add(new wxStaticText(contentPanel, wxID_ANY, "Nama Proyek:"), 0,
        wxALL | wxALIGN_CENTER_VERTICAL, 5);
    tenderProjectNameInput = new wxTextCtrl(contentPanel, wxID_ANY, "",
        wxDefaultPosition, wxSize(300, -1));
    nameSizer->Add(tenderProjectNameInput, 1, wxALL | wxEXPAND, 5);
    addSizer->Add(nameSizer, 0, wxEXPAND | wxALL, 5);

    // Category
    wxBoxSizer* categorySizer = new wxBoxSizer(wxHORIZONTAL);
    categorySizer->Add(new wxStaticText(contentPanel, wxID_ANY, "Kategori:"), 0,
        wxALL | wxALIGN_CENTER_VERTICAL, 5);
    tenderProjectCategoryDropdown = new wxChoice(contentPanel, wxID_ANY,
        wxDefaultPosition, wxSize(200, -1));

    // Populate category dropdown
    std::vector<wxString> categories = GetCategoriesVector();
    for (const auto& category : categories) {
        tenderProjectCategoryDropdown->Append(category);
    }

    categorySizer->Add(tenderProjectCategoryDropdown, 0, wxALL, 5);
    addSizer->Add(categorySizer, 0, wxEXPAND | wxALL, 5);

    // Description
    wxBoxSizer* descSizer = new wxBoxSizer(wxHORIZONTAL);
    descSizer->Add(new wxStaticText(contentPanel, wxID_ANY, "Deskripsi:"), 0,
        wxALL | wxALIGN_CENTER_VERTICAL, 5);
    tenderProjectDescInput = new wxTextCtrl(contentPanel, wxID_ANY, "",
        wxDefaultPosition, wxSize(300, 60),
        wxTE_MULTILINE);
    descSizer->Add(tenderProjectDescInput, 1, wxALL | wxEXPAND, 5);
    addSizer->Add(descSizer, 0, wxEXPAND | wxALL, 5);

    // Tender Date
    wxBoxSizer* dateSizer = new wxBoxSizer(wxHORIZONTAL);
    dateSizer->Add(new wxStaticText(contentPanel, wxID_ANY, "Tanggal Tender:"), 0,
        wxALL | wxALIGN_CENTER_VERTICAL, 5);
    tenderProjectDateInput = new wxTextCtrl(contentPanel, wxID_ANY, "",
        wxDefaultPosition, wxSize(200, -1));
    tenderProjectDateInput->SetHint("DD/MM/YYYY");
    dateSizer->Add(tenderProjectDateInput, 0, wxALL, 5);
    addSizer->Add(dateSizer, 0, wxEXPAND | wxALL, 5);

    // Estimated Value
    wxBoxSizer* valueSizer = new wxBoxSizer(wxHORIZONTAL);
    valueSizer->Add(new wxStaticText(contentPanel, wxID_ANY, "Estimasi Nilai:"), 0,
        wxALL | wxALIGN_CENTER_VERTICAL, 5);
    tenderProjectValueInput = new wxTextCtrl(contentPanel, wxID_ANY, "",
        wxDefaultPosition, wxSize(200, -1));
    tenderProjectValueInput->SetHint("Dalam Rupiah");
    valueSizer->Add(tenderProjectValueInput, 0, wxALL, 5);
    addSizer->Add(valueSizer, 0, wxEXPAND | wxALL, 5);

    // Client Name
    wxBoxSizer* clientSizer = new wxBoxSizer(wxHORIZONTAL);
    clientSizer->Add(new wxStaticText(contentPanel, wxID_ANY, "Nama Klien:"), 0,
        wxALL | wxALIGN_CENTER_VERTICAL, 5);
    tenderProjectClientInput = new wxTextCtrl(contentPanel, wxID_ANY, "",
        wxDefaultPosition, wxSize(300, -1));
    clientSizer->Add(tenderProjectClientInput, 1, wxALL | wxEXPAND, 5);
    addSizer->Add(clientSizer, 0, wxEXPAND | wxALL, 5);

    // Priority
    wxBoxSizer* prioritySizer = new wxBoxSizer(wxHORIZONTAL);
    prioritySizer->Add(new wxStaticText(contentPanel, wxID_ANY, "Prioritas:"), 0,
        wxALL | wxALIGN_CENTER_VERTICAL, 5);
    tenderProjectPriorityDropdown = new wxChoice(contentPanel, wxID_ANY,
        wxDefaultPosition, wxSize(150, -1));
    tenderProjectPriorityDropdown->Append("1 - Sangat Tinggi");
    tenderProjectPriorityDropdown->Append("2 - Tinggi");
    tenderProjectPriorityDropdown->Append("3 - Sedang");
    tenderProjectPriorityDropdown->Append("4 - Rendah");
    tenderProjectPriorityDropdown->Append("5 - Sangat Rendah");
    tenderProjectPriorityDropdown->SetSelection(2); // Default to "Sedang"

    prioritySizer->Add(tenderProjectPriorityDropdown, 0, wxALL, 5);
    addSizer->Add(prioritySizer, 0, wxEXPAND | wxALL, 5);

    // Submit Button
    wxButton* submitBtn = new wxButton(contentPanel, 9001, "Tambah ke Queue Tender");
    submitBtn->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &MainFrame::OnSubmitTenderProject, this);
    addSizer->Add(submitBtn, 0, wxALL | wxALIGN_CENTER, 10);

    contentSizer->Add(addSizer, 0, wxEXPAND | wxALL, 10);

    // Queue Management Section
    wxStaticBox* queueBox = new wxStaticBox(contentPanel, wxID_ANY, "Manajemen Queue Tender");
    wxStaticBoxSizer* queueSizer = new wxStaticBoxSizer(queueBox, wxVERTICAL);

    // Scrolled Window for Queue
    wxScrolledWindow* queueScrolledPanel = new wxScrolledWindow(contentPanel, wxID_ANY,
        wxDefaultPosition, wxSize(-1, 150), wxVSCROLL);
    queueScrolledPanel->SetScrollRate(5, 5);

    wxBoxSizer* queueInnerSizer = new wxBoxSizer(wxVERTICAL);
    queueScrolledPanel->SetSizer(queueInnerSizer);

    // Queue Statistics
    wxString queueStats = wxString::Format("Total Proyek dalam Queue: %zu",
        queueTenderProjects.size());
    queueSizer->Add(new wxStaticText(contentPanel, wxID_ANY, queueStats), 0, wxALL, 5);

    // Next Project Info
    TenderProject* nextProject = GetNextTenderProject();
    if (nextProject) {
        wxString nextInfo = wxString::Format("Proyek Berikutnya: %s (%s)",
            nextProject->nama, nextProject->id);
        queueSizer->Add(new wxStaticText(contentPanel, wxID_ANY, nextInfo), 0, wxALL, 5);

        wxButton* processBtn = new wxButton(contentPanel, 9002, "Proses Proyek Berikutnya");
        processBtn->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &MainFrame::OnProcessNextTenderProject, this);
        queueSizer->Add(processBtn, 0, wxALL, 5);
    }
    else {
        queueSizer->Add(new wxStaticText(contentPanel, wxID_ANY, "Queue kosong"), 0, wxALL, 5);
    }

    contentSizer->Add(queueSizer, 0, wxEXPAND | wxALL, 10);

    // Projects Display Section
    wxStaticBox* displayBox = new wxStaticBox(contentPanel, wxID_ANY, "Daftar Proyek Tender");
    wxStaticBoxSizer* displaySizer = new wxStaticBoxSizer(displayBox, wxVERTICAL);

    tenderProjectsPanel = new wxScrolledWindow(contentPanel, wxID_ANY,
        wxDefaultPosition, wxSize(-1, 300));
    tenderProjectsPanel->SetScrollRate(5, 5);
    tenderProjectsSizer = new wxBoxSizer(wxVERTICAL);
    tenderProjectsPanel->SetSizer(tenderProjectsSizer);

    RefreshTenderProjectsDisplay();

    displaySizer->Add(tenderProjectsPanel, 1, wxEXPAND | wxALL, 5);
    contentSizer->Add(displaySizer, 1, wxEXPAND | wxALL, 10);
}

void MainFrame::OnSubmitTenderProject(wxCommandEvent& event) {
    // Validate inputs
    wxString name = tenderProjectNameInput->GetValue().Trim();
    if (name.IsEmpty()) {
        wxMessageBox("Nama proyek tidak boleh kosong!", "Error", wxOK | wxICON_ERROR);
        return;
    }

    int categorySelection = tenderProjectCategoryDropdown->GetSelection();
    if (categorySelection == wxNOT_FOUND) {
        wxMessageBox("Pilih kategori proyek!", "Error", wxOK | wxICON_ERROR);
        return;
    }

    wxString category = tenderProjectCategoryDropdown->GetString(categorySelection);
    wxString description = tenderProjectDescInput->GetValue().Trim();
    wxString tenderDate = tenderProjectDateInput->GetValue().Trim();
    wxString clientName = tenderProjectClientInput->GetValue().Trim();

    // Parse estimated value
    long estimatedValue = 0;
    wxString valueStr = tenderProjectValueInput->GetValue().Trim();
    if (!valueStr.IsEmpty() && !valueStr.ToLong(&estimatedValue)) {
        wxMessageBox("Estimasi nilai harus berupa angka!", "Error", wxOK | wxICON_ERROR);
        return;
    }

    int priority = tenderProjectPriorityDropdown->GetSelection() + 1; // 1-5

    // Add to queue
    AddTenderProject(name, category, description, tenderDate,
        (int)estimatedValue, clientName, priority);

    // Clear inputs
    tenderProjectNameInput->Clear();
    tenderProjectDescInput->Clear();
    tenderProjectDateInput->Clear();
    tenderProjectValueInput->Clear();
    tenderProjectClientInput->Clear();
    tenderProjectCategoryDropdown->SetSelection(wxNOT_FOUND);
    tenderProjectPriorityDropdown->SetSelection(2);

    wxMessageBox("Proyek tender berhasil ditambahkan ke queue!", "Sukses",
        wxOK | wxICON_INFORMATION);

    // Refresh display
    ShowMenuPage(9);
}

void MainFrame::OnDeleteTenderProject(wxCommandEvent& event) {
    wxButton* btn = static_cast<wxButton*>(event.GetEventObject());
    wxString projectId = *static_cast<wxString*>(btn->GetClientData());

    int result = wxMessageBox("Apakah Anda yakin ingin menghapus proyek tender ini?",
        "Konfirmasi Hapus", wxYES_NO | wxICON_QUESTION);

    if (result == wxYES) {
        DeleteTenderProject(projectId);
        wxMessageBox("Proyek tender berhasil dihapus!", "Sukses",
            wxOK | wxICON_INFORMATION);
        ShowMenuPage(9); // Refresh page
    }
}

void MainFrame::OnProcessNextTenderProject(wxCommandEvent& event) {
    TenderProject* nextProject = GetNextTenderProject();
    if (!nextProject) {
        wxMessageBox("Queue kosong!", "Info", wxOK | wxICON_INFORMATION);
        return;
    }

    wxString message = wxString::Format(
        "Memproses proyek: %s\nID: %s\nKlien: %s\n\nProyek akan dihapus dari queue. Lanjutkan?",
        nextProject->nama, nextProject->id, nextProject->clientName
    );

    int result = wxMessageBox(message, "Proses Proyek Berikutnya",
        wxYES_NO | wxICON_QUESTION);

    if (result == wxYES) {
        ProcessNextTenderProject();
        wxMessageBox("Proyek berhasil diproses dan dihapus dari queue!", "Sukses",
            wxOK | wxICON_INFORMATION);
        ShowMenuPage(9); // Refresh page
    }
}

void MainFrame::RefreshTenderProjectsDisplay() {
    tenderProjectsSizer->Clear(true);

    std::vector<TenderProject> projects = GetTenderProjectsVector();

    if (projects.empty()) {
        wxStaticText* emptyMsg = new wxStaticText(tenderProjectsPanel, wxID_ANY,
            "Belum ada proyek tender dalam queue.");
        tenderProjectsSizer->Add(emptyMsg, 0, wxALL, 10);
    }
    else {
        int position = 1;
        for (const auto& project : projects) {
            wxPanel* projectPanel = new wxPanel(tenderProjectsPanel);
            wxBoxSizer* projectSizer = new wxBoxSizer(wxVERTICAL);

            // Project Header
            wxString headerText = wxString::Format("#%d - %s (%s)",
                position++, project.nama, project.id);
            wxStaticText* header = new wxStaticText(projectPanel, wxID_ANY, headerText);
            wxFont headerFont = header->GetFont();
            headerFont.SetWeight(wxFONTWEIGHT_BOLD);
            header->SetFont(headerFont);
            projectSizer->Add(header, 0, wxALL, 5);

            // Project Details
            wxString details = wxString::Format(
                "Kategori: %s | Status: %s | Prioritas: %d\n"
                "Klien: %s | Estimasi: Rp %s\n"
                "Tanggal Tender: %s",
                project.kategori, project.status, project.priority,
                project.clientName.IsEmpty() ? "Tidak disebutkan" : project.clientName,
                project.estimatedValue > 0 ? wxString::Format("%d", project.estimatedValue) : "Tidak disebutkan",
                project.tenderDate.IsEmpty() ? "Tidak disebutkan" : project.tenderDate
            );

            wxStaticText* detailsText = new wxStaticText(projectPanel, wxID_ANY, details);
            projectSizer->Add(detailsText, 0, wxALL | wxEXPAND, 5);

            if (!project.description.IsEmpty()) {
                wxString descText = "Deskripsi: " + project.description;
                wxStaticText* desc = new wxStaticText(projectPanel, wxID_ANY, descText);
                projectSizer->Add(desc, 0, wxALL | wxEXPAND, 5);
            }

            // Action Buttons
            wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);

            wxButton* deleteBtn = new wxButton(projectPanel, 9003, "Hapus");
            deleteBtn->SetClientData(new wxString(project.id));
            deleteBtn->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &MainFrame::OnDeleteTenderProject, this);
            buttonSizer->Add(deleteBtn, 0, wxALL, 2);

            projectSizer->Add(buttonSizer, 0, wxALL, 5);

            projectPanel->SetSizer(projectSizer);
            tenderProjectsSizer->Add(projectPanel, 0, wxEXPAND | wxALL, 5);

            // Add separator
            wxStaticLine* separator = new wxStaticLine(tenderProjectsPanel);
            tenderProjectsSizer->Add(separator, 0, wxEXPAND | wxALL, 2);
        }
    }

    tenderProjectsPanel->Layout();
    tenderProjectsPanel->FitInside();
}


void MainFrame::OnDeleteConnection(wxCommandEvent& event) {
    wxButton* btn = dynamic_cast<wxButton*>(event.GetEventObject());
    if (btn) {
        AssetConnection* conn = static_cast<AssetConnection*>(btn->GetClientData());
        if (conn) {
            wxString fromName = GetAssetNameById(conn->fromAssetId);
            wxString toName = GetAssetNameById(conn->toAssetId);

            int result = wxMessageBox(
                wxString::Format("Apakah Anda yakin ingin menghapus koneksi antara '%s' dan '%s'?",
                    fromName, toName),
                "Konfirmasi Hapus", wxYES_NO | wxICON_QUESTION);

            if (result == wxYES) {
                DeleteAssetConnection(conn->fromAssetId, conn->toAssetId);
                wxMessageBox("Koneksi berhasil dihapus.", "Sukses", wxOK | wxICON_INFORMATION);
                ShowMenuPage(7); // Refresh the page
            }
            delete conn;
        }
    }
}

void MainFrame::OnViewAssetConnections(wxCommandEvent& event) {
    int selection = selectedAssetDropdown->GetSelection();
    if (selection == wxNOT_FOUND) {
        wxMessageBox("Pilih aset terlebih dahulu.", "Error", wxOK | wxICON_ERROR);
        return;
    }

    wxString selectedText = selectedAssetDropdown->GetString(selection);
    wxString assetId = selectedText.BeforeFirst(' ');
    wxString assetName = GetAssetNameById(assetId);

    std::vector<AssetConnection> connections = GetConnectionsForAsset(assetId);

    wxString message;
    if (connections.empty()) {
        message = wxString::Format("Aset '%s' tidak memiliki koneksi dengan aset lain.", assetName);
    }
    else {
        message = wxString::Format("Koneksi untuk aset '%s':\n\n", assetName);
        for (const auto& conn : connections) {
            wxString otherAssetId = (conn.fromAssetId == assetId) ? conn.toAssetId : conn.fromAssetId;
            wxString otherAssetName = GetAssetNameById(otherAssetId);

            message += wxString::Format("• %s (Bobot: %d)", otherAssetName, conn.weight);
            if (!conn.description.IsEmpty()) {
                message += " - " + conn.description;
            }
            message += "\n";
        }
    }

    wxMessageBox(message, "Koneksi Aset: " + assetName, wxOK | wxICON_INFORMATION);
}

void MainFrame::RefreshAssetDropdowns() {
    // This method can be called when assets are added/removed
    // to refresh all dropdown choices in the graph management page
    if (fromAssetDropdown && toAssetDropdown && selectedAssetDropdown) {
        std::vector<Aset> assets = GetAssetsVector();

        fromAssetDropdown->Clear();
        toAssetDropdown->Clear();
        selectedAssetDropdown->Clear();

        for (const auto& asset : assets) {
            wxString displayText = asset.id + " - " + asset.nama;
            fromAssetDropdown->Append(displayText);
            toAssetDropdown->Append(displayText);
            selectedAssetDropdown->Append(displayText);
        }

        if (!assets.empty()) {
            fromAssetDropdown->SetSelection(0);
            toAssetDropdown->SetSelection(assets.size() > 1 ? 1 : 0);
            selectedAssetDropdown->SetSelection(0);
        }
    }
}



void MainFrame::ShowMenuPage(int menuNumber) {
    contentSizer->Clear(true);

    if (menuNumber == 1) {
        ShowCategoryManagementPage();
    }
    else if (menuNumber == 2) {
        ShowAssetManagementPage();
    }
    else if (menuNumber == 3) {
        ShowAssetListPage();
    }
    else if (menuNumber == 4) {
        ShowAssetSearchPage();
    }
    else if (menuNumber == 5) {
        ShowStatisticsPage();
    }
    else if (menuNumber == 6) {
        ShowAssetValueManagementPanel();
    }
    else if (menuNumber == 7) {
        ShowAssetGraphManagementPage();
    }
    else if (menuNumber == 8) {
        ShowSubAssetManagementPage();
    }
    else if (menuNumber == 9) {
        ShowTenderProjectManagementPage();
    }
    else if (menuNumber == 10) {
        ShowRecentAssetsPage();
    }
    else {
        wxString labelText = wxString::Format("Menu %d - Coming Soon", menuNumber);
        wxStaticText* label = new wxStaticText(contentPanel, wxID_ANY, labelText);
        wxFont font = label->GetFont();
        font.SetPointSize(18);
        font.SetWeight(wxFONTWEIGHT_BOLD);
        label->SetFont(font);

        wxBoxSizer* centerSizer = new wxBoxSizer(wxVERTICAL);
        centerSizer->AddStretchSpacer(1);
        centerSizer->Add(label, 0, wxALIGN_CENTER);
        centerSizer->AddStretchSpacer(1);

        contentSizer->Add(centerSizer, 1, wxEXPAND);
    }

    contentPanel->Layout();
}

void MainFrame::ShowRecentAssetsPage() {
    // Title
    wxStaticText* title = new wxStaticText(contentPanel, wxID_ANY, "Catatan Aset Terakhir Dibuka");
    wxFont titleFont = title->GetFont();
    titleFont.SetPointSize(16);
    titleFont.SetWeight(wxFONTWEIGHT_BOLD);
    title->SetFont(titleFont);
    contentSizer->Add(title, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 10);

    // Statistics
    wxString statsText = wxString::Format("Total History: %zu item(s)", assetHistoryStack.size());
    wxStaticText* stats = new wxStaticText(contentPanel, wxID_ANY, statsText);
    contentSizer->Add(stats, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);

    // Clear History Button
    if (!assetHistoryStack.empty()) {
        wxButton* clearBtn = new wxButton(contentPanel, 10001, "Hapus Semua History");
        clearBtn->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &MainFrame::OnClearHistory, this);
        contentSizer->Add(clearBtn, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 5);
    }

    // Recent Assets List
    wxStaticBox* listBox = new wxStaticBox(contentPanel, wxID_ANY, "Daftar History Akses");
    wxStaticBoxSizer* listSizer = new wxStaticBoxSizer(listBox, wxVERTICAL);

    recentAssetsPanel = new wxScrolledWindow(contentPanel, wxID_ANY);
    recentAssetsPanel->SetScrollRate(0, 20);
    recentAssetsSizer = new wxBoxSizer(wxVERTICAL);
    recentAssetsPanel->SetSizer(recentAssetsSizer);

    RefreshRecentAssetsDisplay();

    listSizer->Add(recentAssetsPanel, 1, wxEXPAND | wxALL, 5);
    contentSizer->Add(listSizer, 1, wxEXPAND | wxALL, 10);
}

void MainFrame::RefreshRecentAssetsDisplay() {
    recentAssetsSizer->Clear(true);

    std::vector<AssetHistory> history = GetAssetHistoryVector();

    if (history.empty()) {
        wxStaticText* emptyText = new wxStaticText(recentAssetsPanel, wxID_ANY,
            "Belum ada history akses aset.\nHistory akan muncul ketika Anda membuka detail aset.");
        emptyText->SetForegroundColour(wxColour(128, 128, 128));
        recentAssetsSizer->Add(emptyText, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 20);
    }
    else {
        for (size_t i = 0; i < history.size(); i++) {
            const AssetHistory& item = history[i];

            // Panel untuk setiap item
            wxPanel* itemPanel = new wxPanel(recentAssetsPanel, wxID_ANY);
            itemPanel->SetBackgroundColour(wxColour(248, 248, 248));
            wxBoxSizer* itemSizer = new wxBoxSizer(wxVERTICAL);

            // Header: Ranking + Asset Name
            wxString headerText = wxString::Format("#%zu - %s (%s)",
                i + 1, item.assetName, item.assetId);
            wxStaticText* header = new wxStaticText(itemPanel, wxID_ANY, headerText);
            wxFont headerFont = header->GetFont();
            headerFont.SetWeight(wxFONTWEIGHT_BOLD);
            header->SetFont(headerFont);

            // Details
            wxString detailText = wxString::Format("Tipe: %s | Diakses: %s",
                item.assetType, item.accessTime);
            wxStaticText* details = new wxStaticText(itemPanel, wxID_ANY, detailText);
            details->SetForegroundColour(wxColour(64, 64, 64));

            // Open Button
            wxButton* openBtn = new wxButton(itemPanel, 10002, "Buka Detail");
            openBtn->SetClientData(new wxString(item.assetId)); // Store asset ID
            openBtn->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &MainFrame::OnOpenRecentAsset, this);

            itemSizer->Add(header, 0, wxALL, 5);
            itemSizer->Add(details, 0, wxLEFT | wxRIGHT | wxBOTTOM, 5);
            itemSizer->Add(openBtn, 0, wxALL, 5);

            itemPanel->SetSizer(itemSizer);
            recentAssetsSizer->Add(itemPanel, 0, wxEXPAND | wxALL, 3);

            // Separator
            if (i < history.size() - 1) {
                recentAssetsSizer->Add(new wxStaticLine(recentAssetsPanel), 0, wxEXPAND | wxLEFT | wxRIGHT, 10);
            }
        }
    }

    recentAssetsPanel->Layout();
    recentAssetsPanel->FitInside();
}

void MainFrame::OnClearHistory(wxCommandEvent& event) {
    int result = wxMessageBox("Apakah Anda yakin ingin menghapus semua history akses aset?",
        "Konfirmasi", wxYES_NO | wxICON_QUESTION);
    if (result == wxYES) {
        while (!assetHistoryStack.empty()) {
            assetHistoryStack.pop();
        }
        wxMessageBox("History berhasil dihapus!", "Sukses", wxOK | wxICON_INFORMATION);
        ShowMenuPage(10); // Refresh page
    }
}

void MainFrame::OnOpenRecentAsset(wxCommandEvent& event) {
    wxButton* btn = dynamic_cast<wxButton*>(event.GetEventObject());
    if (btn && btn->GetClientData()) {
        wxString* assetId = static_cast<wxString*>(btn->GetClientData());

        // Implementasi buka detail aset (sesuaikan dengan sistem Anda)
        // Contoh: redirect ke asset detail page atau search page
        wxMessageBox(wxString::Format("Membuka detail aset: %s", *assetId),
            "Info", wxOK | wxICON_INFORMATION);

        // Anda bisa redirect ke halaman yang sesuai, misalnya:
        // ShowAssetDetailPage(*assetId);
        // atau set search input dan lakukan pencarian
    }
}



void MainFrame::ShowCategoryManagementPage() {
    wxStaticText* title = new wxStaticText(contentPanel, wxID_ANY, "Manajemen Kategori");
    wxFont font = title->GetFont();
    font.SetPointSize(16);
    font.SetWeight(wxFONTWEIGHT_BOLD);
    title->SetFont(font);

    wxStaticText* nameLabel = new wxStaticText(contentPanel, wxID_ANY, "Nama Kategori:");
    categoryNameInput = new wxTextCtrl(contentPanel, wxID_ANY);

    wxButton* submitBtn = new wxButton(contentPanel, wxID_ANY, "Simpan Kategori");
    submitBtn->Bind(wxEVT_BUTTON, &MainFrame::OnSubmitCategory, this);

    wxStaticText* existingLabel = new wxStaticText(contentPanel, wxID_ANY, "Kategori yang Ada:");
    wxFont labelFont = existingLabel->GetFont();
    labelFont.SetWeight(wxFONTWEIGHT_BOLD);
    existingLabel->SetFont(labelFont);

    contentSizer->Add(title, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 15);
    contentSizer->Add(nameLabel, 0, wxTOP, 5);
    contentSizer->Add(categoryNameInput, 0, wxEXPAND | wxBOTTOM, 10);
    contentSizer->Add(submitBtn, 0, wxALIGN_CENTER | wxTOP, 10);
    contentSizer->Add(existingLabel, 0, wxTOP, 20);

    std::vector<wxString> categories = GetCategoriesVector();
    for (size_t i = 0; i < categories.size(); ++i) {
        wxBoxSizer* rowSizer = new wxBoxSizer(wxHORIZONTAL);

        wxStaticText* label = new wxStaticText(contentPanel, wxID_ANY, categories[i]);
        wxButton* deleteBtn = new wxButton(contentPanel, 2000 + static_cast<int>(i), "Hapus");

        deleteBtn->SetClientData(new wxString(categories[i]));
        deleteBtn->Bind(wxEVT_BUTTON, &MainFrame::OnDeleteCategory, this);

        rowSizer->Add(label, 1, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);
        rowSizer->Add(deleteBtn, 0);

        contentSizer->Add(rowSizer, 0, wxEXPAND | wxALL, 5);
    }
}

void MainFrame::ShowAssetManagementPage() {
    wxStaticText* title = new wxStaticText(contentPanel, wxID_ANY, "Manajemen Aset");
    wxFont font = title->GetFont();
    font.SetPointSize(16);
    font.SetWeight(wxFONTWEIGHT_BOLD);
    title->SetFont(font);

    wxStaticText* nameLabel = new wxStaticText(contentPanel, wxID_ANY, "Nama Aset:");
    assetNameInput = new wxTextCtrl(contentPanel, wxID_ANY);

    wxStaticText* typeLabel = new wxStaticText(contentPanel, wxID_ANY, "Kategori Aset:");

    std::vector<wxString> categories = GetCategoriesVector();
    wxString* categoryArray = new wxString[categories.size()];
    for (size_t i = 0; i < categories.size(); i++) {
        categoryArray[i] = categories[i];
    }

    assetTypeDropdown = new wxChoice(contentPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize,
        static_cast<int>(categories.size()), categoryArray);
    if (categories.size() > 0) {
        assetTypeDropdown->SetSelection(0);
    }

    delete[] categoryArray;

    wxButton* submitBtn = new wxButton(contentPanel, wxID_ANY, "Simpan Aset");
    submitBtn->Bind(wxEVT_BUTTON, &MainFrame::OnSubmitAsset, this);

    contentSizer->Add(title, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 15);
    contentSizer->Add(nameLabel, 0, wxTOP, 5);
    contentSizer->Add(assetNameInput, 0, wxEXPAND | wxBOTTOM, 10);
    contentSizer->Add(typeLabel, 0, wxTOP, 5);
    contentSizer->Add(assetTypeDropdown, 0, wxEXPAND | wxBOTTOM, 10);
    contentSizer->Add(submitBtn, 0, wxALIGN_CENTER | wxTOP, 10);
}

void MainFrame::ShowAssetListPage() {
    wxStaticText* title = new wxStaticText(contentPanel, wxID_ANY, "Daftar Aset");
    wxFont font = title->GetFont();
    font.SetPointSize(16);
    font.SetWeight(wxFONTWEIGHT_BOLD);
    title->SetFont(font);

    contentSizer->Add(title, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 15);

    wxGrid* assetGrid = new wxGrid(contentPanel, wxID_ANY);
    assetGrid->CreateGrid(0, 4);
    assetGrid->SetColLabelValue(0, "ID Aset");
    assetGrid->SetColLabelValue(1, "Nama Aset");
    assetGrid->SetColLabelValue(2, "Kategori");
    assetGrid->SetColLabelValue(3, "Aksi");

    assetGrid->SetColSize(0, 100);
    assetGrid->SetColSize(1, 200);
    assetGrid->SetColSize(2, 150);
    assetGrid->SetColSize(3, 100);

    std::vector<Aset> assets = GetAssetsVector();

    for (size_t i = 0; i < assets.size(); ++i) {
        assetGrid->AppendRows(1);
        assetGrid->SetCellValue(i, 0, assets[i].id);
        assetGrid->SetCellValue(i, 1, assets[i].nama);
        assetGrid->SetCellValue(i, 2, assets[i].kategori);
        assetGrid->SetCellValue(i, 3, "Hapus");

        assetGrid->SetReadOnly(i, 0);
        assetGrid->SetReadOnly(i, 1);
        assetGrid->SetReadOnly(i, 2);
    }

    assetGrid->Bind(wxEVT_GRID_CELL_LEFT_CLICK, &MainFrame::OnAssetGridClick, this);

    contentSizer->Add(assetGrid, 1, wxEXPAND | wxALL, 10);
}

void MainFrame::ShowAssetSearchPage() {
    wxStaticText* title = new wxStaticText(contentPanel, wxID_ANY, "Pencarian Aset");
    wxFont font = title->GetFont();
    font.SetPointSize(16);
    font.SetWeight(wxFONTWEIGHT_BOLD);
    title->SetFont(font);

    wxStaticText* searchLabel = new wxStaticText(contentPanel, wxID_ANY, "Kata Kunci Pencarian:");
    searchInput = new wxTextCtrl(contentPanel, wxID_ANY);

    wxButton* searchBtn = new wxButton(contentPanel, wxID_ANY, "Cari");
    searchBtn->Bind(wxEVT_BUTTON, &MainFrame::OnSearchAsset, this);

    contentSizer->Add(title, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 15);
    contentSizer->Add(searchLabel, 0, wxTOP, 5);
    contentSizer->Add(searchInput, 0, wxEXPAND | wxBOTTOM, 10);
    contentSizer->Add(searchBtn, 0, wxALIGN_CENTER | wxTOP, 10);

    searchResultsPanel = new wxScrolledWindow(contentPanel);
    searchResultsPanel->SetScrollRate(5, 5);
    searchResultsSizer = new wxBoxSizer(wxVERTICAL);
    searchResultsPanel->SetSizer(searchResultsSizer);

    contentSizer->Add(searchResultsPanel, 1, wxEXPAND | wxALL, 10);
}

// Continuation of the MainFrame.cpp file

void MainFrame::ShowStatisticsPage() {
    wxStaticText* title = new wxStaticText(contentPanel, wxID_ANY, "Statistik Aset");
    wxFont font = title->GetFont();
    font.SetPointSize(16);
    font.SetWeight(wxFONTWEIGHT_BOLD);
    title->SetFont(font);

    contentSizer->Add(title, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 15);

    // Count assets by category
    LinkedList<CategoryCount> categoryCounts;
    initializeCategoryCounts(daftarKategori, categoryCounts);
    countAssetsByCategory(daftarAset, categoryCounts);

    // Display statistics
    wxStaticText* categoryStatsLabel = new wxStaticText(contentPanel, wxID_ANY, "Jumlah Aset per Kategori:");
    wxFont labelFont = categoryStatsLabel->GetFont();
    labelFont.SetWeight(wxFONTWEIGHT_BOLD);
    categoryStatsLabel->SetFont(labelFont);
    contentSizer->Add(categoryStatsLabel, 0, wxTOP | wxBOTTOM, 10);

    Node<CategoryCount>* current = categoryCounts.getHead();
    while (current) {
        wxString statText = wxString::Format("%s: %d aset",
            current->data.kategori, current->data.count);
        wxStaticText* stat = new wxStaticText(contentPanel, wxID_ANY, statText);
        contentSizer->Add(stat, 0, wxALL, 5);
        current = current->next;
    }

    // Total assets
    size_t totalAssets = daftarAset.size();
    wxString totalText = wxString::Format("Total Aset: %zu", totalAssets);
    wxStaticText* totalStat = new wxStaticText(contentPanel, wxID_ANY, totalText);
    wxFont totalFont = totalStat->GetFont();
    totalFont.SetWeight(wxFONTWEIGHT_BOLD);
    totalStat->SetFont(totalFont);
    contentSizer->Add(totalStat, 0, wxTOP | wxBOTTOM, 15);

    // Asset value statistics
    if (!daftarDetailNilaiAset.empty()) {
        wxStaticText* valueStatsLabel = new wxStaticText(contentPanel, wxID_ANY, "Statistik Nilai Aset:");
        valueStatsLabel->SetFont(labelFont);
        contentSizer->Add(valueStatsLabel, 0, wxTOP | wxBOTTOM, 10);

        int totalValue = 0;
        int totalMaintenance = 0;
        int totalTax = 0;
        Node<AssetValueDetails>* valueNode = daftarDetailNilaiAset.getHead();
        while (valueNode) {
            totalValue += valueNode->data.currentValue;
            totalMaintenance += valueNode->data.maintenanceCost;
            totalTax += valueNode->data.propertyTax;
            valueNode = valueNode->next;
        }

        wxString valueText = wxString::Format("Total Nilai Aset: Rp %d", totalValue);
        wxString maintenanceText = wxString::Format("Total Biaya Maintenance: Rp %d", totalMaintenance);
        wxString taxText = wxString::Format("Total Pajak Properti: Rp %d", totalTax);

        contentSizer->Add(new wxStaticText(contentPanel, wxID_ANY, valueText), 0, wxALL, 5);
        contentSizer->Add(new wxStaticText(contentPanel, wxID_ANY, maintenanceText), 0, wxALL, 5);
        contentSizer->Add(new wxStaticText(contentPanel, wxID_ANY, taxText), 0, wxALL, 5);
    }
}

void MainFrame::OnButtonClicked(wxCommandEvent& event) {
    int id = event.GetId();
    int menuNumber = id - 1000;
    ShowMenuPage(menuNumber);
}

void MainFrame::OnSubmitCategory(wxCommandEvent& event) {
    wxString categoryName = categoryNameInput->GetValue().Trim();

    if (categoryName.IsEmpty()) {
        wxMessageBox("Nama kategori tidak boleh kosong.", "Error", wxOK | wxICON_ERROR);
        return;
    }

    if (CategoryExists(categoryName)) {
        wxMessageBox("Kategori sudah ada.", "Error", wxOK | wxICON_ERROR);
        return;
    }

    AddCategory(categoryName);
    categoryNameInput->Clear();
    wxMessageBox("Kategori berhasil ditambahkan.", "Sukses", wxOK | wxICON_INFORMATION);

    // Refresh the page
    ShowMenuPage(1);
}

void MainFrame::OnDeleteCategory(wxCommandEvent& event) {
    wxButton* btn = dynamic_cast<wxButton*>(event.GetEventObject());
    if (btn) {
        wxString* categoryName = static_cast<wxString*>(btn->GetClientData());
        if (categoryName) {
            int result = wxMessageBox("Apakah Anda yakin ingin menghapus kategori '" + *categoryName + "'?\n"
                "Semua aset dalam kategori ini juga akan dihapus.",
                "Konfirmasi Hapus", wxYES_NO | wxICON_QUESTION);

            if (result == wxYES) {
                DeleteCategory(*categoryName);
                wxMessageBox("Kategori berhasil dihapus.", "Sukses", wxOK | wxICON_INFORMATION);
                ShowMenuPage(1); // Refresh the page
            }
            delete categoryName;
        }
    }
}

void MainFrame::OnSubmitAsset(wxCommandEvent& event) {
    wxString assetName = assetNameInput->GetValue().Trim();

    if (assetName.IsEmpty()) {
        wxMessageBox("Nama aset tidak boleh kosong.", "Error", wxOK | wxICON_ERROR);
        return;
    }

    int selection = assetTypeDropdown->GetSelection();
    if (selection == wxNOT_FOUND) {
        wxMessageBox("Pilih kategori aset.", "Error", wxOK | wxICON_ERROR);
        return;
    }

    wxString selectedCategory = assetTypeDropdown->GetString(selection);
    wxString assetId = GenerateUniqueAssetId(selectedCategory);

    AddAsset(assetId, assetName, selectedCategory);

    assetNameInput->Clear();
    assetTypeDropdown->SetSelection(0);

    wxMessageBox(wxString::Format("Aset '%s' dengan ID '%s' berhasil ditambahkan.",
        assetName, assetId), "Sukses", wxOK | wxICON_INFORMATION);
}

void MainFrame::OnAssetGridClick(wxGridEvent& event) {
    int row = event.GetRow();
    int col = event.GetCol();

    if (col == 3) { // Delete column
        wxGrid* grid = dynamic_cast<wxGrid*>(event.GetEventObject());
        if (grid) {
            wxString assetId = grid->GetCellValue(row, 0);
            wxString assetName = grid->GetCellValue(row, 1);
            // Track history

            int result = wxMessageBox("Apakah Anda yakin ingin menghapus aset '" + assetName + "'?",
                "Konfirmasi Hapus", wxYES_NO | wxICON_QUESTION);
            AddToAssetHistory(assetId, assetName, "Asset");

            if (result == wxYES) {
                DeleteAsset(assetId);
                wxMessageBox("Aset berhasil dihapus.", "Sukses", wxOK | wxICON_INFORMATION);
                ShowMenuPage(3); // Refresh the asset list page
            }
        }
    }

    event.Skip();
}

void MainFrame::OnSearchAsset(wxCommandEvent& event) {
    wxString searchTerm = searchInput->GetValue().Trim();

    // Clear previous results
    searchResultsSizer->Clear(true);

    if (searchTerm.IsEmpty()) {
        wxStaticText* noResults = new wxStaticText(searchResultsPanel, wxID_ANY,
            "Masukkan kata kunci pencarian.");
        searchResultsSizer->Add(noResults, 0, wxALL, 10);
        searchResultsPanel->Layout();
        return;
    }

    std::vector<Aset> results = SearchAssets(searchTerm);

    if (results.empty()) {
        wxStaticText* noResults = new wxStaticText(searchResultsPanel, wxID_ANY,
            "Tidak ada aset yang ditemukan.");
        searchResultsSizer->Add(noResults, 0, wxALL, 10);
    }
    else {
        wxString headerText = wxString::Format("Ditemukan %zu aset:", results.size());
        wxStaticText* header = new wxStaticText(searchResultsPanel, wxID_ANY, headerText);
        wxFont headerFont = header->GetFont();
        headerFont.SetWeight(wxFONTWEIGHT_BOLD);
        header->SetFont(headerFont);
        searchResultsSizer->Add(header, 0, wxALL, 10);

        for (const auto& asset : results) {
            wxBoxSizer* resultSizer = new wxBoxSizer(wxHORIZONTAL);

            wxString resultText = wxString::Format("ID: %s | Nama: %s | Kategori: %s",
                asset.id, asset.nama, asset.kategori);
            wxStaticText* resultLabel = new wxStaticText(searchResultsPanel, wxID_ANY, resultText);

            // Get asset value details if available
            Node<AssetValueDetails>* valueNode = findAssetValueById(daftarDetailNilaiAset, asset.id);
            if (valueNode) {
                wxString valueText = wxString::Format(" | Nilai: Rp %d", valueNode->data.currentValue);
                resultText += valueText;
                resultLabel->SetLabel(resultText);
            }

            resultSizer->Add(resultLabel, 1, wxALIGN_CENTER_VERTICAL);
            searchResultsSizer->Add(resultSizer, 0, wxEXPAND | wxALL, 5);
        }
    }

    searchResultsPanel->Layout();
    searchResultsPanel->FitInside();
}

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
wxEND_EVENT_TABLE()
