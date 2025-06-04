#pragma once
#include <wx/wx.h>
#include <wx/grid.h>
#include <wx/scrolwin.h>
#include <wx/choice.h>

template <typename T> struct Node;
template <typename T> class LinkedList;
template <typename T> struct TreeNode;  
struct AssetValueDetails;
struct SubAsset; 
struct TenderProject;  

class MainFrame : public wxFrame
{
public:
    MainFrame(const wxString& title);
    void ShowAssetGraphManagementPage();
    void OnSubmitConnection(wxCommandEvent& event);
    void OnDeleteConnection(wxCommandEvent& event);
    void OnViewAssetConnections(wxCommandEvent& event);
    void RefreshAssetDropdowns();

private:
    wxPanel* contentPanel;
    wxBoxSizer* contentSizer;

    wxTextCtrl* categoryNameInput;

    wxTextCtrl* assetNameInput;
    wxChoice* assetTypeDropdown;

    wxTextCtrl* searchInput;
    wxScrolledWindow* searchResultsPanel;
    wxBoxSizer* searchResultsSizer;

    wxChoice* parentAssetDropdown;
    wxChoice* parentSubAssetDropdown;
    wxTextCtrl* subAssetNameInput;
    wxTextCtrl* subAssetDescInput;
    wxScrolledWindow* treeDisplayPanel;
    wxBoxSizer* treeDisplaySizer;
    wxScrolledWindow* recentAssetsPanel;
    wxBoxSizer* recentAssetsSizer;

    wxChoice* fromAssetDropdown;
    wxChoice* toAssetDropdown;
    wxTextCtrl* weightInput;
    wxTextCtrl* descriptionInput;
    wxScrolledWindow* graphResultsPanel;
    wxBoxSizer* graphResultsSizer;
    wxChoice* selectedAssetDropdown;
    wxTextCtrl* tenderProjectNameInput;
    wxChoice* tenderProjectCategoryDropdown;
    wxTextCtrl* tenderProjectDescInput;
    wxTextCtrl* tenderProjectDateInput;
    wxTextCtrl* tenderProjectValueInput;
    wxTextCtrl* tenderProjectClientInput;
    wxChoice* tenderProjectPriorityDropdown;
    wxScrolledWindow* tenderProjectsPanel;
    wxBoxSizer* tenderProjectsSizer;

    void OnButtonClicked(wxCommandEvent& event);
    void OnSubmitCategory(wxCommandEvent& event);
    void OnDeleteCategory(wxCommandEvent& event);
    void OnSubmitAsset(wxCommandEvent& event);
    void OnAssetGridClick(wxGridEvent& event);
    void OnSearchAsset(wxCommandEvent& event);
    void OnSubmitSubAsset(wxCommandEvent& event);
    void OnEditSubAsset(wxCommandEvent& event);
    void OnDeleteSubAsset(wxCommandEvent& event);

    void ShowMenuPage(int menuNumber);
    void ShowCategoryManagementPage();
    void ShowAssetManagementPage();
    void ShowAssetListPage();
    void ShowAssetSearchPage();
    void ShowStatisticsPage();
    void ShowAssetValueManagementPanel();
    void ShowSubAssetManagementPage();

    void ShowEditAssetValueDialog(Node<AssetValueDetails>* node);

    void UpdateParentSubAssetDropdown();
    void DisplayAssetTrees();
    void DisplayTreeNode(TreeNode<SubAsset>* node, int depth);
    void ShowEditSubAssetDialog(SubAsset* subAsset);
    void ShowTenderProjectManagementPage();
    void OnSubmitTenderProject(wxCommandEvent& event);
    void OnDeleteTenderProject(wxCommandEvent& event);
    void OnUpdateTenderProjectStatus(wxCommandEvent& event);
    void OnProcessNextTenderProject(wxCommandEvent& event);
    void ShowEditTenderProjectDialog(TenderProject* project);
    void RefreshTenderProjectsDisplay();
    void ShowRecentAssetsPage();
    void OnClearHistory(wxCommandEvent& event);
    void OnOpenRecentAsset(wxCommandEvent& event);
    void RefreshRecentAssetsDisplay();


    wxDECLARE_EVENT_TABLE();
};
