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

#include "insert_warning_suppression_dlg.h"
#include <wx/valgen.h>

//-------------------------------------------------------------
InsertWarningSuppressionDlg::InsertWarningSuppressionDlg(
    wxWindow* parent, wxString stringToFormat, wxWindowID id /*= wxID_ANY*/,
    const wxString& caption /*= _(L"Insert Translator Comment")*/,
    const wxPoint& pos /*= wxDefaultPosition*/, const wxSize& size /*= wxDefaultSize*/,
    long style /*= wxDEFAULT_DIALOG_STYLE | wxCLIP_CHILDREN | wxRESIZE_BORDER*/)
    : m_stringToFormat(std::move(stringToFormat))
    {
    wxNonOwnedWindow::SetExtraStyle(GetExtraStyle() | wxWS_EX_BLOCK_EVENTS | wxWS_EX_CONTEXTHELP);
    wxDialog::Create(parent, id, caption, pos, size, style);

    m_suppressionTags.Add(_DT(L"Quneiform (C/C++)"));
    m_suppressionTags.Add(_DT(L"Quneiform (Markdown/Quarto)"));
    m_suppressionTags.Add(_DT(L"md2po (translate-off/on)"));
    m_suppressionTags.Add(_DT(L"Clang-format"));
    // last two options are expected to use the warning text window,
    // and last one should be Cpp-check
    m_suppressionTags.Add(_DT(L"Clang-tidy"));
    m_suppressionTags.Add(_DT(L"Cpp-check"));

    // bind events
    Bind(wxEVT_HELP, &InsertWarningSuppressionDlg::OnContextHelp, this);
    Bind(wxEVT_BUTTON, &InsertWarningSuppressionDlg::OnHelpClicked, this, wxID_HELP);
    Bind(wxEVT_BUTTON, &InsertWarningSuppressionDlg::OnOK, this, wxID_OK);
    Bind(wxEVT_CHOICE, [this](wxCommandEvent& event) { OnApplicationChange(event); }, ID_APP_COMBO);

    CreateControls();
    Centre();
    }

//-------------------------------------------------------------
void InsertWarningSuppressionDlg::CreateControls()
    {
    auto* mainDlgSizer = new wxBoxSizer(wxVERTICAL);

    auto* functionComboSzr = new wxBoxSizer(wxHORIZONTAL);
    functionComboSzr->Add(
        new wxStaticText(this, wxID_STATIC, _(L"Program to suppress warnings from:")),
        wxSizerFlags{}.CenterVertical());

    functionComboSzr->Add(new wxChoice(this, ID_APP_COMBO, wxDefaultPosition, wxDefaultSize,
                                       m_suppressionTags, 0, wxGenericValidator(&m_selectedTag)),
                          wxSizerFlags{}.Left().CenterVertical().Border(wxLEFT));
    mainDlgSizer->Add(functionComboSzr, wxSizerFlags{}.Expand().Border());

    mainDlgSizer->Add(new wxStaticText(this, wxID_STATIC, _(L"Specific warnings to suppress:")),
                      wxSizerFlags{}.Border());
    m_warningsTextCtrl =
        new wxTextCtrl(this, wxID_ANY, wxString{}, wxDefaultPosition, FromDIP(wxSize{ 500, -1 }),
                       wxTE_RICH2 | wxBORDER_THEME, wxGenericValidator(&m_warnings));
    m_warningsTextCtrl->Enable(m_selectedTag >= 2);
    mainDlgSizer->Add(m_warningsTextCtrl, wxSizerFlags{ 1 }.Expand().Border());

    mainDlgSizer->Add(CreateSeparatedButtonSizer(wxOK | wxCANCEL | wxHELP),
                      wxSizerFlags{}.Expand().Border());

    TransferDataToWindow();

    SetSizerAndFit(mainDlgSizer);
    }

//-------------------------------------------------------------
void InsertWarningSuppressionDlg::OnApplicationChange([[maybe_unused]] wxCommandEvent& event)
    {
    TransferDataFromWindow();
    if (m_warningsTextCtrl != nullptr)
        {
        m_warningsTextCtrl->Enable(
            std::cmp_greater_equal(m_selectedTag, (m_suppressionTags.size() - 2)));
        }
    }

//-------------------------------------------------------------
void InsertWarningSuppressionDlg::OnOK([[maybe_unused]] wxCommandEvent& event)
    {
    TransferDataFromWindow();
    if (std::cmp_equal(m_selectedTag, (m_suppressionTags.size() - 1)) && m_warnings.empty())
        {
        wxMessageBox(_(L"Cpp-check requires a list of warning IDs to suppress."),
                     _(L"Missing Warnings"));
        return;
        }

    if (IsModal())
        {
        EndModal(wxID_OK);
        }
    else
        {
        Show(false);
        }
    }

//-------------------------------------------------------------
wxString InsertWarningSuppressionDlg::GetFormattedOutput()
    {
    TransferDataFromWindow();

    const wxString padding = wxString{}.Pad(m_indenting);

    if (m_selectedTag == 0)
        {
        return _DT(L"// quneiform-suppress-begin\n") + padding + m_stringToFormat + "\n" + padding +
               _DT(L"// quneiform-suppress-end");
        }
    if (m_selectedTag == 1)
        {
        return _DT(L"<!-- quneiform-suppress-begin -->\n") + padding + m_stringToFormat + "\n" +
               padding + _DT(L"<!-- quneiform-suppress-end -->");
        }
    if (m_selectedTag == 2)
        {
        return _DT(L"<!-- translate:off -->\n") + padding + m_stringToFormat + "\n" + padding +
               _DT(L"<!-- translate:on -->");
        }
    if (m_selectedTag == 3)
        {
        return _DT(L"// clang-format off\n") + padding + m_stringToFormat + "\n" + padding +
               _DT(L"// clang-format on");
        }
    if (m_selectedTag == 4)
        {
        return (m_warnings.empty() ?
                    _DT(L"// NOLINTBEGIN\n") + padding + m_stringToFormat + "\n" + padding +
                        _DT(L"// NOLINTEND") :
                    wxString::Format(_DT(L"// NOLINTBEGIN(%s)\n"), m_warnings) + padding +
                        m_stringToFormat + "\n" + padding + _DT(L"// NOLINTEND"));
        }
    if (m_selectedTag == 5)
        {
        return wxString::Format(_DT(L"// cppcheck-suppress %s\n"), m_warnings) + padding +
               m_stringToFormat;
        }
    // shouldn't happen
    return m_stringToFormat;
    }
