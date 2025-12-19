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

#include "convert_string_dlg.h"
#include <algorithm>
#include <wx/valgen.h>

//-------------------------------------------------------------
ConvertStringDlg::ConvertStringDlg(
    wxWindow* parent, wxWindowID id /*= wxID_ANY*/,
    const wxString& caption /*= _(L"Convert String")*/, const wxPoint& pos /*= wxDefaultPosition*/,
    const wxSize& size /*= wxDefaultSize*/,
    long style /*= wxDEFAULT_DIALOG_STYLE | wxCLIP_CHILDREN | wxRESIZE_BORDER*/)
    {
    wxNonOwnedWindow::SetExtraStyle(GetExtraStyle() | wxWS_EX_BLOCK_EVENTS | wxWS_EX_CONTEXTHELP);
    wxDialog::Create(parent, id, caption, pos, size, style);

    // bind events
    Bind(wxEVT_BUTTON, &ConvertStringDlg::OnHelpClicked, this, wxID_HELP);
    Bind(wxEVT_HELP, &ConvertStringDlg::OnContextHelp, this);
    Bind(wxEVT_TEXT, &ConvertStringDlg::OnTextChanged, this, ID_SOURCE_TEXT);
    Bind(wxEVT_CHOICE, &ConvertStringDlg::OnTextChanged, this, ID_SELECTIONS);
    Bind(
        wxEVT_BUTTON,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            TransferDataFromWindow();
            if (wxTheClipboard->Open() && m_outputTextCtrl != nullptr)
                {
                wxTheClipboard->Clear();
                auto* obj = new wxDataObjectComposite();
                obj->Add(new wxTextDataObject(m_outputTextCtrl->GetValue()));
                wxTheClipboard->SetData(obj);
                wxTheClipboard->Close();
                }
        },
        wxID_COPY);

    CreateControls();

    if (m_inputTextCtrl != nullptr)
        {
        m_inputTextCtrl->SetFocus();
        }
    Centre();
    }

//-------------------------------------------------------------
void ConvertStringDlg::CreateControls()
    {
    auto* mainDlgSizer = new wxBoxSizer(wxVERTICAL);

    auto* functionComboSzr = new wxBoxSizer(wxHORIZONTAL);
    functionComboSzr->Add(new wxStaticText(this, wxID_STATIC, _(L"Convert:")),
                          wxSizerFlags{}.CenterVertical());

    const wxArrayString choices = { _(L"7-bit numbers to full-width numbers"),
                                    _(L"7-bit numbers to Devanagari numbers"),
                                    _(L"Full-width numbers to 7-bit numbers"),
                                    _(L"Devanagari numbers to 7-bit numbers"),
                                    _(L"Encode extended ASCII characters") };

    functionComboSzr->Add(new wxChoice(this, ID_SELECTIONS, wxDefaultPosition, wxDefaultSize,
                                       choices, 0, wxGenericValidator(&m_selectedConversion)),
                          wxSizerFlags{}.Left().CenterVertical().Border(wxLEFT));
    mainDlgSizer->Add(functionComboSzr, wxSizerFlags{}.Expand().Border());

    mainDlgSizer->Add(new wxStaticText(this, wxID_STATIC, _(L"Source string:")),
                      wxSizerFlags{}.Border());
    m_inputTextCtrl = new wxTextCtrl(
        this, ID_SOURCE_TEXT, wxString{}, wxDefaultPosition, FromDIP(wxSize{ 500, 150 }),
        wxTE_RICH2 | wxTE_MULTILINE | wxBORDER_THEME | wxTE_BESTWRAP, wxGenericValidator(&m_input));
    mainDlgSizer->Add(m_inputTextCtrl, wxSizerFlags{ 1 }.Expand().Border());

    mainDlgSizer->Add(new wxStaticText(this, wxID_STATIC, _(L"Conversion:")),
                      wxSizerFlags{}.Border());
    auto* outputSizer = new wxBoxSizer(wxHORIZONTAL);
    m_outputTextCtrl = new wxTextCtrl(
        this, wxID_ANY, wxString{}, wxDefaultPosition, FromDIP(wxSize{ 500, 150 }),
        wxTE_RICH2 | wxTE_MULTILINE | wxBORDER_THEME | wxTE_BESTWRAP | wxTE_READONLY);
    outputSizer->Add(m_outputTextCtrl, wxSizerFlags{ 1 }.Expand());
    auto* copyButton = new wxBitmapButton(
        this, wxID_COPY,
        wxArtProvider::GetBitmap(wxART_COPY, wxART_OTHER, FromDIP(wxSize{ 16, 16 })));
    copyButton->SetToolTip(_("Copy"));
    outputSizer->Add(copyButton, wxSizerFlags{}.Top());
    mainDlgSizer->Add(outputSizer, wxSizerFlags{ 1 }.Expand().Border());

    mainDlgSizer->Add(CreateSeparatedButtonSizer(wxCLOSE | wxHELP),
                      wxSizerFlags{}.Expand().Border());

    TransferDataToWindow();

    SetSizerAndFit(mainDlgSizer);
    }

//-------------------------------------------------------------
void ConvertStringDlg::OnTextChanged([[maybe_unused]] wxCommandEvent& event)
    {
    TransferDataFromWindow();

    std::wstring tempStr = m_input.wc_string();

    if (m_selectedConversion == 0)
        {
        std::ranges::for_each(tempStr, [](auto& chr)
                              { chr = i18n_string_util::seven_bit_number_to_full_width(chr); });
        }
    else if (m_selectedConversion == 1)
        {
        std::ranges::for_each(tempStr, [](auto& chr)
                              { chr = i18n_string_util::seven_bit_number_to_devanagari(chr); });
        }
    else if (m_selectedConversion == 2)
        {
        std::ranges::for_each(tempStr, [](auto& chr)
                              { chr = i18n_string_util::full_width_number_to_7bit(chr); });
        }
    else if (m_selectedConversion == 3)
        {
        std::ranges::for_each(tempStr, [](auto& chr)
                              { chr = i18n_string_util::devanagari_number_to_7bit(chr); });
        }
    else if (m_selectedConversion == 4)
        {
        std::wstring encoded;
        encoded.reserve(tempStr.size() * 10); // worst case: every char → \UXXXXXXXX

        for (const wchar_t ch : tempStr)
            {
            if (ch > 0x7F)
                {
                encoded += std::format(L"\\U{:08X}", static_cast<std::uint32_t>(ch));
                }
            else
                {
                encoded.push_back(ch);
                }
            }
        tempStr = std::move(encoded);
        }

    m_outputTextCtrl->SetValue(tempStr);
    }
