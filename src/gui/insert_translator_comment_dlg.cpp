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

#include "insert_translator_comment_dlg.h"

//-------------------------------------------------------------
InsertTransCommentDlg::InsertTransCommentDlg(
    wxWindow* parent, wxWindowID id /*= wxID_ANY*/,
    const wxString& selectedComment /*= _DT(L"// TRANSLATORS:")*/,
    const wxString& caption /*= _(L"Insert Translator Comment")*/,
    const wxPoint& pos /*= wxDefaultPosition*/, const wxSize& size /*= wxDefaultSize*/,
    long style /*= wxDEFAULT_DIALOG_STYLE | wxCLIP_CHILDREN | wxRESIZE_BORDER*/)
    : m_selectedTag(selectedComment)
    {
    SetExtraStyle(GetExtraStyle() | wxWS_EX_BLOCK_EVENTS | wxWS_EX_CONTEXTHELP);
    wxDialog::Create(parent, id, caption, pos, size, style);

    // wxWidgets
    m_translatorTags.Add(_DT(L"// TRANSLATORS:"));
    m_translatorTags.Add(_DT(L"/* TRANSLATORS: */"));
    // Qt
    m_translatorTags.Add(L"//:");
    m_translatorTags.Add(L"/*: */");

    // bind events
    Bind(wxEVT_HELP, &InsertTransCommentDlg::OnContextHelp, this);
    Bind(wxEVT_BUTTON, &InsertTransCommentDlg::OnHelpClicked, this, wxID_HELP);
    Bind(wxEVT_BUTTON, &InsertTransCommentDlg::OnOK, this, wxID_OK);

    CreateControls();
    Centre();
    }

//-------------------------------------------------------------
void InsertTransCommentDlg::CreateControls()
    {
    wxBoxSizer* mainDlgSizer = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer* functionComboSzr = new wxBoxSizer(wxHORIZONTAL);
    functionComboSzr->Add(new wxStaticText(this, wxID_STATIC, _(L"Translator comment style:")),
                          wxSizerFlags{}.CenterVertical());

    if (m_selectedTag.empty())
        {
        m_selectedTag = m_translatorTags[0];
        }
    functionComboSzr->Add(new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                       m_translatorTags, 0, wxGenericValidator(&m_selectedTag)),
                          wxSizerFlags{}.Left().CenterVertical().Border(wxLEFT));
    mainDlgSizer->Add(functionComboSzr, wxSizerFlags{}.Expand().Border());

    mainDlgSizer->Add(new wxStaticText(this, wxID_STATIC, _(L"Explanation for translators:")),
                      wxSizerFlags{}.Border());
    wxTextCtrl* commentTextCtrl =
        new wxTextCtrl(this, wxID_ANY, wxString{}, wxDefaultPosition, FromDIP(wxSize{ 500, 150 }),
                       wxTE_RICH2 | wxTE_MULTILINE | wxBORDER_THEME | wxTE_BESTWRAP,
                       wxGenericValidator(&m_comment));
#if wxUSE_SPELLCHECK
    commentTextCtrl->EnableProofCheck(
        wxTextProofOptions::Default().GrammarCheck(true).SpellCheck(true));
#endif
    mainDlgSizer->Add(commentTextCtrl, wxSizerFlags{ 1 }.Expand().Border());

    mainDlgSizer->Add(CreateSeparatedButtonSizer(wxOK | wxCANCEL | wxHELP),
                      wxSizerFlags{}.Expand().Border());

    commentTextCtrl->SetFocus();

    TransferDataToWindow();

    SetSizerAndFit(mainDlgSizer);
    }

//-------------------------------------------------------------
void InsertTransCommentDlg::OnOK([[maybe_unused]] wxCommandEvent&)
    {
    TransferDataFromWindow();

    if (m_comment.find(L"\n") != wxString::npos && !IsMultilineComment())
        {
        wxMessageBox(_(L"Please select a multiline comment style if using a multiline comment."),
                     _(L"Invalid Comment Style"));
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
wxString InsertTransCommentDlg::GetFormattedOutput()
    {
    TransferDataFromWindow();

    // if a multiline comment, push each subsequent line over to
    // line up with the start of the comment
    wxString formattedComment{ m_comment };
    formattedComment.Replace(L"\n",
                             L"\n" + wxString{}.Pad(static_cast<size_t>(m_linePosition) +
                                                    ((m_selectedTag == _DT(L"/*: */")) ? 4 : 3)));

    if (m_selectedTag == _DT(L"// TRANSLATORS:"))
        {
        return m_selectedTag + L" " + m_comment;
        }
    else if (m_selectedTag == _DT(L"/* TRANSLATORS: */"))
        {
        return _DT(L"/* TRANSLATORS: ") + formattedComment + L" */";
        }
    else if (m_selectedTag == _DT(L"//:"))
        {
        return m_selectedTag + L" " + m_comment;
        }
    else if (m_selectedTag == _DT(L"/*: */"))
        {
        return _DT(L"/*: ") + formattedComment + L" */";
        }
    else
        {
        // shouldn't happen
        return wxString{};
        }
    }
