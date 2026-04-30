#pragma once
/*

    //------------------------------------------------------------------------------
    constexpr uint32_t DIALOG_OK = 0x00000000u;
    constexpr uint32_t DIALOG_OKCANCEL = 0x00000001u;
    constexpr uint32_t DIALOG_YESNO = 0x00000002u;
    constexpr uint32_t DIALOG_YESNOCANCEL = 0x00000003u;
    constexpr uint32_t DIALOG_OPENDIALOG = 0x00000004u;
    constexpr uint32_t DIALOG_SAVEDIALOG = 0x00000005u;

    constexpr size_t MAX_DIALOG_FILTER = 20;

    struct FDialogDesc
    {
        uint32_t DialogType;
        char* szTitle;
        char* szMessage;
        char* szOKLabel;

        bool OnlySelectFolder; // for OpenDialog

        int NumFilters;
        char* FilterNames[MAX_DIALOG_FILTER];
        char* FilterExts[MAX_DIALOG_FILTER];

        FDialogDesc()
            : DialogType(0)
            , szTitle(nullptr)
            , szMessage(nullptr)
            , szOKLabel(nullptr)
            , NumFilters(0)
            , OnlySelectFolder(false)
        {
            memset(FilterNames, 0, sizeof(char*) * MAX_DIALOG_FILTER);
            memset(FilterExts, 0, sizeof(char*) * MAX_DIALOG_FILTER);
        }
    };



namespace mark
{
	MARKENGINE_API int ShowMessageBox(const FDialogDesc& DialogDesc);

	MARKENGINE_API bool ShowOpenDialog(const FDialogDesc& DialogDesc, char* OutputPath, size_t Length);

	MARKENGINE_API bool ShowSaveDialog(const FDialogDesc& DialogDesc, char* OutputPath, size_t Length);
}
*/
