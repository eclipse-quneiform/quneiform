/********************************************************************************
 * Copyright (c) 2025 Blake Madden
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * https://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 *   Blake Madden - initial implementation
 ********************************************************************************/

/** @addtogroup Internationalization
    @brief i18n classes.
@{*/

#ifndef INSERT_SUPPRESS_WARNINGS_DLG_H
#define INSERT_SUPPRESS_WARNINGS_DLG_H

#include "../i18n_review.h"
#include "app_options.h"
#include <wx/combobox.h>
#include <wx/editlbox.h>
#include <wx/filename.h>
#include <wx/listbook.h>
#include <wx/spinctrl.h>
#include <wx/stdpaths.h>
#include <wx/string.h>
#include <wx/wx.h>

/** @brief Prompt for inserting warning suppression blocks into code.*/
class InsertWarningSuppressionDlg final : public wxDialog
    {
  public:
    /** @brief Constructor.
        @param parent The parent window.
        @param stringToFormat The original string to wrap in a function call.
        @param id The window ID.
        @param caption The title of the dialog.
        @param pos The screen position of the window.
        @param size The window size.
        @param style The window style (i.e., decorations and flags).*/
    explicit InsertWarningSuppressionDlg(wxWindow* parent, wxString stringToFormat,
                                         wxWindowID id = wxID_ANY,
                                         const wxString& caption = _(L"Insert Warning Suppression"),
                                         const wxPoint& pos = wxDefaultPosition,
                                         const wxSize& size = wxDefaultSize,
                                         long style = wxDEFAULT_DIALOG_STYLE | wxCLIP_CHILDREN |
                                                      wxRESIZE_BORDER);
    /// @private
    InsertWarningSuppressionDlg(const InsertWarningSuppressionDlg&) = delete;
    /// @private
    InsertWarningSuppressionDlg& operator=(const InsertWarningSuppressionDlg&) = delete;

    /// @returns The fully formatted results.
    [[nodiscard]]
    wxString GetFormattedOutput();

    /// @brief Sets how much to indent the suppression comments
    /// @param indenting Number of spaces to indent the comments.
    void SetIndenting(const int indenting) { m_indenting = indenting; }

  private:
    void CreateControls();

    void OnHelpClicked([[maybe_unused]] wxCommandEvent& event)
        {
        const wxString docPath = []()
        {
            if (wxFile::Exists(wxStandardPaths::Get().GetResourcesDir() +
                               wxFileName::GetPathSeparator() + L"quneiform.pdf"))
                {
                return wxStandardPaths::Get().GetResourcesDir() + wxFileName::GetPathSeparator() +
                       L"quneiform.pdf";
                }
            return wxFileName{ wxStandardPaths::Get().GetExecutablePath() }.GetPath() +
                   wxFileName::GetPathSeparator() + L"quneiform.pdf";
        }();
        if (wxFile::Exists(docPath))
            {
            wxLaunchDefaultApplication(docPath);
            }
        }

    void OnContextHelp([[maybe_unused]] wxHelpEvent& event)
        {
        wxCommandEvent cmd;
        OnHelpClicked(cmd);
        }

    void OnOK([[maybe_unused]] wxCommandEvent& event);

    void OnApplicationChange([[maybe_unused]] wxCommandEvent& event);

    int m_indenting{ 0 };

    wxString m_stringToFormat;
    int m_selectedTag{ 0 };
    wxString m_warnings;

    wxArrayString m_suppressionTags;

    wxTextCtrl* m_warningsTextCtrl{ nullptr };

    constexpr static int ID_APP_COMBO = wxID_HIGHEST;
    };

    /** @}*/

#endif // INSERT_SUPPRESS_WARNINGS_DLG_H
