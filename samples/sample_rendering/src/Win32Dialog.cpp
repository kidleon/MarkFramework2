#include "pch.h"
/*
#include "MarkWin32Dialog.h"
#include <codecvt>
#include <locale>
#include <dwmapi.h>
#include <ShObjIdl.h>
#include <windowsx.h>


namespace mark
{
	int ShowMessageBox(const FDialogDesc& DialogDesc)
	{
		if (DialogDesc.DialogType == DIALOG_OPENDIALOG || DialogDesc.DialogType == DIALOG_SAVEDIALOG)
			return -1;

		UINT flags = MB_APPLMODAL | MB_SETFOREGROUND | MB_ICONINFORMATION;
		switch(DialogDesc.DialogType)
		{
		case DIALOG_OK:
			flags |= MB_OK;
			break;

		case DIALOG_OKCANCEL:
			flags |= MB_OKCANCEL;
			break;

		case DIALOG_YESNO:
			flags |= MB_YESNO;
			break;

		case DIALOG_YESNOCANCEL:
			flags |= MB_YESNOCANCEL;
			break;
		}

		int Result = MessageBox(nullptr,
								!DialogDesc.szMessage ? "" : DialogDesc.szMessage,
								!DialogDesc.szTitle ? "MessageBox" : DialogDesc.szTitle,
								flags);
		return Result;
	}

	bool ShowOpenDialog(const FDialogDesc& DialogDesc, char* OutputPath, size_t Length)
	{
        if (DialogDesc.DialogType != DIALOG_OPENDIALOG)
            return false;

        bool success = false;
        HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        if (SUCCEEDED(hr))
        {
            IFileOpenDialog* pFileOpen;

            // Create the FileOpenDialog object.
            hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
                                  IID_IFileOpenDialog,
                                  reinterpret_cast<void**>(&pFileOpen));

            if (SUCCEEDED(hr))
            {
                // Show the Open dialog box.
                std::string strTitle(!DialogDesc.szTitle ? "MessageBox" : DialogDesc.szTitle);

                std::wstring title = std::wstring(strTitle.begin(), strTitle.end());
                pFileOpen->SetTitle(title.c_str());

                std::string strOKLabel(!DialogDesc.szOKLabel ? "Open" : DialogDesc.szOKLabel);
                std::wstring okLabel = std::wstring(strOKLabel.begin(), strOKLabel.end());
                if (!okLabel.empty())
                {
                    pFileOpen->SetOkButtonLabel(okLabel.c_str());
                }

                if (0 < DialogDesc.NumFilters)
                {
                    COMDLG_FILTERSPEC FilterSpecs[MAX_DIALOG_FILTER];
                    memset(FilterSpecs, 0, sizeof(COMDLG_FILTERSPEC) * MAX_DIALOG_FILTER);

                    const UINT NumFilters = std::min((UINT)DialogDesc.NumFilters, (UINT)MAX_DIALOG_FILTER);

                    for (int i = 0; i < DialogDesc.NumFilters; ++i)
                    {
                        std::string strName(DialogDesc.FilterNames[i]);
                        std::string strSpec;
                        strSpec += "*.";
                        strSpec += DialogDesc.FilterExts[i];
                        if (i < DialogDesc.NumFilters - 1)
                            strSpec += ";";

                        std::wstring wstrName(strName.begin(), strName.end());
                        std::wstring wstrSpec(strSpec.begin(), strSpec.end());

                        FilterSpecs[i].pszName = wstrName.c_str();
                        FilterSpecs[i].pszSpec = wstrSpec.c_str();
                    }

                    pFileOpen->SetFileTypes((UINT)DialogDesc.NumFilters, FilterSpecs);
                }

                FILEOPENDIALOGOPTIONS opts;
                opts = pFileOpen->GetOptions(&opts);
                if (DialogDesc.OnlySelectFolder)
                {
                    pFileOpen->SetOptions(opts | FOS_PICKFOLDERS);
                }

                hr = pFileOpen->Show(NULL);

                // Get the file name from the dialog box.
                if (SUCCEEDED(hr))
                {
                    IShellItem* pItem;
                    hr = pFileOpen->GetResult(&pItem);
                    if (SUCCEEDED(hr))
                    {
                        PWSTR pszFilePath = L"";
                        hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                        if (pszFilePath != 0x0)
                        {
                            char Utf8Path[MAX_PATH] = { '\0' };
                            std::wstring outPathW = pszFilePath;
                            wchar_to_utf8(outPathW.c_str(), outPathW.size(), Utf8Path, MAX_PATH);
                            fstrlcpy(OutputPath, Utf8Path, Length);
                            success = true;
                        }

                        pItem->Release();
                    }
                }
                pFileOpen->Release();
            }
            CoUninitialize();
        }

        return success;
	}

	bool ShowSaveDialog(const FDialogDesc& DialogDesc, char* OutputPath, size_t Length)
    {
        if (DialogDesc.DialogType != DIALOG_SAVEDIALOG)
            return false;

        bool success = false;
        HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        if (SUCCEEDED(hr))
        {
            IFileSaveDialog* pFileSave;

            // Create the FileOpenDialog object.
            hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL,
                                  CLSID_FileSaveDialog,
                                  reinterpret_cast<void**>(&pFileSave));

            if (SUCCEEDED(hr))
            {
                // Show the Open dialog box.
                std::string strTitle(!DialogDesc.szTitle ? "MessageBox" : DialogDesc.szTitle);

                std::wstring title = std::wstring(strTitle.begin(), strTitle.end());
                pFileSave->SetTitle(title.c_str());

                std::string strOKLabel(!DialogDesc.szOKLabel ? "Open" : DialogDesc.szOKLabel);
                std::wstring okLabel = std::wstring(strOKLabel.begin(), strOKLabel.end());
                if (!okLabel.empty())
                {
                    pFileSave->SetOkButtonLabel(okLabel.c_str());
                }

                if (0 < DialogDesc.NumFilters)
                {
                    COMDLG_FILTERSPEC FilterSpecs[MAX_DIALOG_FILTER];
                    memset(FilterSpecs, 0, sizeof(COMDLG_FILTERSPEC) * MAX_DIALOG_FILTER);

                    const UINT NumFilters = std::min((UINT)DialogDesc.NumFilters, (UINT)MAX_DIALOG_FILTER);

                    for (int i = 0; i < DialogDesc.NumFilters; ++i)
                    {
                        std::string strName(DialogDesc.FilterNames[i]);
                        std::string strSpec;
                        strSpec += "*.";
                        strSpec += DialogDesc.FilterExts[i];
                        if (i < DialogDesc.NumFilters - 1)
                            strSpec += ";";

                        std::wstring wstrName(strName.begin(), strName.end());
                        std::wstring wstrSpec(strSpec.begin(), strSpec.end());

                        FilterSpecs[i].pszName = wstrName.c_str();
                        FilterSpecs[i].pszSpec = wstrSpec.c_str();
                    }

                    pFileSave->SetFileTypes((UINT)DialogDesc.NumFilters, FilterSpecs);
                }
                
                hr = pFileSave->Show(NULL);

                // Get the file name from the dialog box.
                if (SUCCEEDED(hr))
                {
                    IShellItem* pItem;
                    hr = pFileSave->GetResult(&pItem);
                    if (SUCCEEDED(hr))
                    {
                        PWSTR pszFilePath = L"";
                        hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                        if (pszFilePath != 0x0)
                        {
                            char Utf8Path[MAX_PATH] = { '\0' };
                            std::wstring outPathW = pszFilePath;
                            wchar_to_utf8(outPathW.c_str(), outPathW.size(), Utf8Path, MAX_PATH);
                            fstrlcpy(OutputPath, Utf8Path, Length);
                            success = true;
                        }

                        pItem->Release();
                    }
                }
                pFileSave->Release();
            }
            CoUninitialize();
        }

        return success;
    }
}

*/