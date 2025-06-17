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

#include "projectdlg.h"

//-------------------------------------------------------------
NewProjectDialog::NewProjectDialog(
    wxWindow* parent, wxWindowID id /*= wxID_ANY*/, const wxString& caption /*= _(L"New Project")*/,
    const EditorPageOptions extraPages /*= NoExtraPages*/,
    const wxPoint& pos /*= wxDefaultPosition*/, const wxSize& size /*= wxDefaultSize*/,
    long style /*= wxDEFAULT_DIALOG_STYLE | wxCLIP_CHILDREN | wxRESIZE_BORDER*/)
    : m_extraPages(extraPages)
    {
    SetExtraStyle(GetExtraStyle() | wxWS_EX_BLOCK_EVENTS | wxWS_EX_CONTEXTHELP);
    wxDialog::Create(parent, id, caption, pos, size, style);

    m_fontEncodings = {
        { wxFONTENCODING_SYSTEM, _(L"System Default") },
        { wxFONTENCODING_ISO8859_6, _(L"Arabic") },
        { wxFONTENCODING_MACARABIC, _(L"Arabic (Mac)") },
        { wxFONTENCODING_CP1256, _(L"Arabic (Windows)") },
        { wxFONTENCODING_MACARABICEXT, _(L"Arabic extended (Mac)") },
        { wxFONTENCODING_MACARMENIAN, _(L"Armenian (Mac)") },
        { wxFONTENCODING_ISO8859_4, _(L"Baltic (Latin 4)") },
        { wxFONTENCODING_ISO8859_13, _(L"Baltic (Latin 7)") },
        { wxFONTENCODING_CP1257, _(L"Baltic (Windows)") },
        { wxFONTENCODING_MACBENGALI, _(L"Bengali (Mac)") },
        { wxFONTENCODING_MACBURMESE, _(L"Burmese (Mac)") },
        { wxFONTENCODING_MACCELTIC, _(L"Celtic (Mac)") },
        { wxFONTENCODING_ISO8859_2, _(L"Central and East European (Latin 2)") },
        { wxFONTENCODING_CP1250, _(L"Central European (Windows)") },
        { wxFONTENCODING_MACCROATIAN, _(L"Croatian (Mac)") },
        { wxFONTENCODING_ISO8859_5, _(L"Cyrillic") },
        { wxFONTENCODING_MACCYRILLIC, _(L"Cyrillic (Mac)") },
        { wxFONTENCODING_CP855, _(L"Cyrillic (primarily Russian)") },
        { wxFONTENCODING_CP1251, _(L"Cyrillic (Windows)") },
        { wxFONTENCODING_MACDEVANAGARI, _(L"Devanagari (Mac)") },
        { wxFONTENCODING_MACDINGBATS, _(L"Dingbats (Mac)") },
        { wxFONTENCODING_ISO8859_3, _(L"Esperanto (Latin 3)") },
        { wxFONTENCODING_MACETHIOPIC, _(L"Ethiopic (Mac)") },
        { wxFONTENCODING_EUC_JP, _(L"Extended Unix Codepage for Japanese") },
        { wxFONTENCODING_MACGAELIC, _(L"Gaelic (Mac)") },
        { wxFONTENCODING_MACGEORGIAN, _(L"Georgian (Mac)") },
        { wxFONTENCODING_ISO8859_7, _(L"Greek") },
        { wxFONTENCODING_MACGREEK, _(L"Greek (Mac)") },
        { wxFONTENCODING_CP1253, _(L"Greek (Windows) (8859-7)") },
        { wxFONTENCODING_MACGUJARATI, _(L"Gujarati (Mac)") },
        { wxFONTENCODING_MACGURMUKHI, _(L"Gurmukhi (Mac)") },
        { wxFONTENCODING_ISO8859_8, _(L"Hebrew") },
        { wxFONTENCODING_MACHEBREW, _(L"Hebrew (Mac)") },
        { wxFONTENCODING_CP1255, _(L"Hebrew (Windows)") },
        { wxFONTENCODING_MACICELANDIC, _(L"Icelandic (Mac)") },
        { wxFONTENCODING_ISO2022_JP, _(L"ISO-2022-JP JIS encoding") },
        { wxFONTENCODING_MACJAPANESE, _(L"Japanese (Mac)") },
        { wxFONTENCODING_CP932, _(L"Japanese (Shift-JIS)") },
        { wxFONTENCODING_SHIFT_JIS, _(L"Japanese Shift JIS") },
        { wxFONTENCODING_MACKANNADA, _(L"Kannada (Mac)") },
        { wxFONTENCODING_MACKHMER, _(L"Khmer (Mac)") },
        { wxFONTENCODING_KOI8, _(L"KOI8 Russian") },
        { wxFONTENCODING_KOI8_U, _(L"KOI8 Ukrainian") },
        { wxFONTENCODING_EUC_KR, _(L"Korean") },
        { wxFONTENCODING_JOHAB, _(L"Korean (Johab)") },
        { wxFONTENCODING_CP1361, _(L"Korean (Johab) (Windows)") },
        { wxFONTENCODING_MACKOREAN, _(L"Korean (Mac)") },
        { wxFONTENCODING_CP949, _(L"Korean (Unified Hangul Code)") },
        { wxFONTENCODING_MACLAOTIAN, _(L"Laotian (Mac)") },
        { wxFONTENCODING_CP1252, _(L"Latin 1; Western European (Windows)") },
        { wxFONTENCODING_CP852, _(L"Latin 2; Central European (DOS)") },
        { wxFONTENCODING_MACCENTRALEUR, _(L"Latin 2; Central European (Mac)") },
        { wxFONTENCODING_ISO8859_14, _(L"Latin 8") },
        { wxFONTENCODING_ISO8859_15, _(L"Latin 9 (Latin 0, includes Euro symbol)") },
        { wxFONTENCODING_MACMALAJALAM, _(L"Malajalam (Mac)") },
        { wxFONTENCODING_MACMONGOLIAN, _(L"Mongolian (Mac)") },
        { wxFONTENCODING_CP850, _(L"Multilingual Latin 1; Western European (DOS)") },
        { wxFONTENCODING_MACORIYA, _(L"Oriya (Mac)") },
        { wxFONTENCODING_MACROMAN, _(L"Roman; Western European (Mac)") },
        { wxFONTENCODING_MACROMANIAN, _(L"Romanian (Mac)") },
        { wxFONTENCODING_CP866, _(L"Russian; Cyrillic (DOS)") },
        { wxFONTENCODING_GB2312, _(L"Simplified Chinese") },
        { wxFONTENCODING_MACCHINESESIMP, _(L"Simplified Chinese (GB 2312, Mac)") },
        { wxFONTENCODING_CP936, _(L"Simplified Chinese (GB2312)") },
        { wxFONTENCODING_MACSINHALESE, _(L"Sinhalese (Mac)") },
        { wxFONTENCODING_MACSYMBOL, _(L"Symbol (Mac)") },
        { wxFONTENCODING_MACTAMIL, _(L"Tamil (Mac)") },
        { wxFONTENCODING_MACTELUGU, _(L"Telugu (Mac)") },
        { wxFONTENCODING_ISO8859_11, _(L"Thai") },
        { wxFONTENCODING_MACTHAI, _(L"Thai (Mac)") },
        { wxFONTENCODING_CP874, _(L"Thai (Windows)") },
        { wxFONTENCODING_MACTIBETAN, _(L"Tibetan (Mac)") },
        { wxFONTENCODING_BIG5, _(L"Traditional Chinese") },
        { wxFONTENCODING_MACCHINESETRAD, _(L"Traditional Chinese (Big5, Mac)") },
        { wxFONTENCODING_CP950, _(L"Traditional Chinese (Big5)") },
        { wxFONTENCODING_ISO8859_9, _(L"Turkish (Latin 5)") },
        { wxFONTENCODING_MACTURKISH, _(L"Turkish (Mac)") },
        { wxFONTENCODING_CP1254, _(L"Turkish (Windows)") },
        { wxFONTENCODING_CP437, _(L"United States (DOS)") },
        { wxFONTENCODING_UTF16, _DT(L"UTF-16") },
        { wxFONTENCODING_UTF16BE, _(L"UTF-16 Big Endian Unicode encoding") },
        { wxFONTENCODING_UTF16LE, _(L"UTF-16 Little Endian Unicode encoding") },
        { wxFONTENCODING_UTF32, _DT(L"UTF-32") },
        { wxFONTENCODING_UTF32BE, _(L"UTF-32 Big Endian Unicode encoding") },
        { wxFONTENCODING_UTF32LE, _(L"UTF-32 Little Endian Unicode encoding") },
        { wxFONTENCODING_UTF7, _(L"UTF-7 Unicode encoding") },
        { wxFONTENCODING_UTF8, _(L"UTF-8 Unicode encoding") },
        { wxFONTENCODING_VIETNAMESE, _(L"Vietnamese") },
        { wxFONTENCODING_MACVIATNAMESE, _(L"Vietnamese (Mac)") },
        { wxFONTENCODING_CP1258, _(L"Vietnamese (Windows)") },
        { wxFONTENCODING_ISO8859_1, _(L"Western European (Latin 1)") }
    };

    CreateControls();
    Centre();

    // bind events
    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &NewProjectDialog::OnHelpClicked, this, wxID_HELP);
    Bind(wxEVT_HELP, &NewProjectDialog::OnContextHelp, this);
    Bind(wxEVT_BUTTON, &NewProjectDialog::OnFolderButtonClick, this,
         NewProjectDialog::ID_FOLDER_BROWSE_BUTTON);
    Bind(wxEVT_BUTTON, &NewProjectDialog::OnFileButtonClick, this,
         NewProjectDialog::ID_FILE_BROWSE_BUTTON);
    Bind(wxEVT_BUTTON, &NewProjectDialog::OnExcludedFolderButtonClick, this,
         NewProjectDialog::ID_EXCLUDED_FOLDERS_BROWSE_BUTTON);
    Bind(wxEVT_BUTTON, &NewProjectDialog::OnExcludedFileButtonClick, this,
         NewProjectDialog::ID_EXCLUDED_FILES_BROWSE_BUTTON);
    Bind(wxEVT_BUTTON, &NewProjectDialog::OnOK, this, wxID_OK);
    Bind(wxEVT_TEXT, &NewProjectDialog::OnSampleTextChanged, this, ID_SAMPLE_TEXT);
    Bind(
        wxEVT_CHECKBOX,
        [this](wxCommandEvent& evt)
        {
            if (m_transLongerThresholdLabel != nullptr)
                {
                m_transLongerThresholdLabel->Enable(evt.IsChecked());
                }
            if (m_transLongerThresholdMinLabel != nullptr)
                {
                m_transLongerThresholdMinLabel->Enable(evt.IsChecked());
                }
            if (m_transLongerThresholdMaxLabel != nullptr)
                {
                m_transLongerThresholdMaxLabel->Enable(evt.IsChecked());
                }
            if (m_transLongerThresholdSlider != nullptr)
                {
                m_transLongerThresholdSlider->Enable(evt.IsChecked());
                }
        },
        ID_CHECK_TRANS_LONGER_CHECK);
    Bind(
        wxEVT_CHECKBOX, [this]([[maybe_unused]] wxCommandEvent&)
        { UpdateEmbeddedStringsOptions(); }, ID_CHECK_STRINGS_NOT_AVAILABLE);
    Bind(
        wxEVT_CHECKBOX, [this]([[maybe_unused]] wxCommandEvent&) { UpdateConsistencyOptions(); },
        ID_CHECK_TRANS_CONSISTENCY);
    Bind(
        wxEVT_CHOICE, [this]([[maybe_unused]] wxCommandEvent&) { UpdatePseudoTransOptions(); },
        ID_PSEUDO_METHODS);
    Bind(
        wxEVT_CHECKBOX, [this]([[maybe_unused]] wxCommandEvent&) { UpdatePseudoTransOptions(); },
        ID_PSEUDO_BRACKETS_CHECK);
    Bind(
        wxEVT_CHECKBOX, [this]([[maybe_unused]] wxCommandEvent&) { UpdatePseudoTransOptions(); },
        ID_PSEUDO_TRACK_IDS_CHECK);
    Bind(
        wxEVT_SLIDER, [this]([[maybe_unused]] wxCommandEvent&) { UpdatePseudoTransOptions(); },
        ID_PSEUDO_WIDTH_SLIDER);
    }

//-------------------------------------------------------------
void NewProjectDialog::UpdateEmbeddedStringsOptions()
    {
    TransferDataFromWindow();
    if (m_ignoredVarsList != nullptr)
        {
        m_ignoredVarsList->Enable(m_notL10NAvailable);
        }
    }

//-------------------------------------------------------------
void NewProjectDialog::UpdateConsistencyOptions()
    {
    TransferDataFromWindow();
    if (m_untranslatableNamesList != nullptr)
        {
        m_untranslatableNamesList->Enable(m_transConsistency);
        }
    }

//-------------------------------------------------------------
void NewProjectDialog::OnSampleTextChanged([[maybe_unused]] wxCommandEvent&)
    {
    UpdatePseudoTransOptions();
    }

//-------------------------------------------------------------
void NewProjectDialog::UpdatePseudoTransOptions()
    {
    TransferDataFromWindow();
    if (m_pseudoSurroundingBracketsCheckbox != nullptr)
        {
        m_pseudoSurroundingBracketsCheckbox->Enable(m_pseudoTranslationMethod != 0);
        }
    if (m_pseudoTrackCheckbox != nullptr)
        {
        m_pseudoTrackCheckbox->Enable(m_pseudoTranslationMethod != 0);
        }
    if (m_pseudoIncreaseSlider != nullptr)
        {
        m_pseudoIncreaseSlider->Enable(m_pseudoTranslationMethod != 0);
        }
    if (m_pseudoSliderLabel != nullptr)
        {
        m_pseudoSliderLabel->Enable(m_pseudoTranslationMethod != 0);
        }
    if (m_pseudoSliderPercentMinLabel != nullptr)
        {
        m_pseudoSliderPercentMinLabel->Enable(m_pseudoTranslationMethod != 0);
        }
    if (m_pseudoSliderPercentMaxLabel != nullptr)
        {
        m_pseudoSliderPercentMaxLabel->Enable(m_pseudoTranslationMethod != 0);
        }
    if (m_previewSizer != nullptr)
        {
        m_previewSizer->GetStaticBox()->Enable(m_pseudoTranslationMethod != 0);
        }
    // do not call TransferDataToWindow() in here, as that will cause
    // this event handler to be called in an infinite loop
    if (m_previewTextWindow != nullptr && m_sampleTextWindow != nullptr)
        {
        i18n_check::pseudo_translater ptrans;
        ptrans.set_pseudo_method(
            static_cast<i18n_check::pseudo_translation_method>(m_pseudoTranslationMethod));
        ptrans.change_width(m_widthPseudoChange);
        ptrans.enable_tracking(m_pseudoTrack);
        ptrans.add_surrounding_brackets(m_addPseudoTransBrackets);
        const wxString mutatedValue =
            ptrans.mutate_message(m_sampleTextWindow->GetValue().wc_str());
        m_previewTextWindow->SetValue(mutatedValue);
        }
    }

//-------------------------------------------------------------
void NewProjectDialog::SetOptions(const i18n_check::review_style style)
    {
    m_options = static_cast<int64_t>(style);

    m_notL10NAvailable = (m_options & i18n_check::review_style::check_not_available_for_l10n);
    m_suspectL10NString = (m_options & i18n_check::review_style::check_l10n_strings);
    m_suspectL10NUsage = (m_options & i18n_check::review_style::check_suspect_l10n_string_usage);
    m_suspectI18NUsage = (m_options & i18n_check::review_style::check_suspect_i18n_usage);
    m_printfMismatch = (m_options & i18n_check::review_style::check_mismatching_printf_commands);
    m_acceleratorMismatch = (m_options & i18n_check::review_style::check_accelerators);
    m_transConsistency = (m_options & i18n_check::review_style::check_consistency);
    m_halfWidth = (m_options & i18n_check::review_style::check_halfwidth);
    m_numberInconsistency = (m_options & i18n_check::review_style::check_numbers);
    m_lengthInconsistency = (m_options & i18n_check::review_style::check_length);
    m_needsContext = (m_options & i18n_check::review_style::check_needing_context);
    m_urlInL10NString = (m_options & i18n_check::review_style::check_l10n_contains_url);
    m_multipartString = (m_options & i18n_check::review_style::check_multipart_strings);
    m_pluralization = (m_options & i18n_check::review_style::check_pluaralization);
    m_articles = (m_options & i18n_check::review_style::check_articles_proceeding_placeholder);
    m_excessiveNonTranslatableContentInL10NString =
        (m_options & i18n_check::review_style::check_l10n_contains_excessive_nonl10n_content);
    m_concatenatedStrings = (m_options & i18n_check::review_style::check_l10n_concatenated_strings);
    m_literalL10NStrings =
        (m_options & i18n_check::review_style::check_literal_l10n_string_comparison);
    m_deprecatedMacro = (m_options & i18n_check::review_style::check_deprecated_macros);
    m_nonUTF8File = (m_options & i18n_check::review_style::check_utf8_encoded);
    m_UTF8FileWithBOM = (m_options & i18n_check::review_style::check_utf8_with_signature);
    m_unencodedExtASCII = (m_options & i18n_check::review_style::check_unencoded_ext_ascii);
    m_printfSingleNumber = (m_options & i18n_check::review_style::check_printf_single_number);
    m_numberAssignedToId = (m_options & i18n_check::review_style::check_number_assigned_to_id);
    m_dupValAssignedToIds =
        (m_options & i18n_check::review_style::check_duplicate_value_assigned_to_ids);
    m_malformedString = (m_options & i18n_check::review_style::check_malformed_strings);
    m_trailingSpaces = (m_options & i18n_check::review_style::check_trailing_spaces);
    m_fontIssue = (m_options & i18n_check::review_style::check_fonts);
    m_tabs = (m_options & i18n_check::review_style::check_tabs);
    m_wideLine = (m_options & i18n_check::review_style::check_line_width);
    m_commentMissingSpace = (m_options & i18n_check::review_style::check_space_after_comment);

    TransferDataToWindow();

    UpdateEmbeddedStringsOptions();
    UpdateConsistencyOptions();
    UpdatePseudoTransOptions();

    if (m_transLongerThresholdLabel != nullptr)
        {
        m_transLongerThresholdLabel->Enable(m_lengthInconsistency);
        }
    if (m_transLongerThresholdMinLabel != nullptr)
        {
        m_transLongerThresholdMinLabel->Enable(m_lengthInconsistency);
        }
    if (m_transLongerThresholdMaxLabel != nullptr)
        {
        m_transLongerThresholdMaxLabel->Enable(m_lengthInconsistency);
        }
    if (m_transLongerThresholdSlider != nullptr)
        {
        m_transLongerThresholdSlider->Enable(m_lengthInconsistency);
        }
    }

//-------------------------------------------------------------
void NewProjectDialog::OnOK([[maybe_unused]] wxCommandEvent&)
    {
    TransferDataFromWindow();

    if (static_cast<bool>(m_extraPages & FilePage) &&
        (m_filePath.empty() ||
         (!wxFileName::DirExists(m_filePath) && !wxFileName::FileExists(m_filePath))))
        {
        wxMessageBox(_(L"Please select a valid file folder."), _(L"Invalid Input"),
                     wxICON_EXCLAMATION | wxOK, this);
        return;
        }

    m_options = i18n_check::review_style::no_checks;

    if (m_notL10NAvailable)
        {
        m_options |= i18n_check::review_style::check_not_available_for_l10n;
        }
    if (m_suspectL10NString)
        {
        m_options |= i18n_check::review_style::check_l10n_strings;
        }
    if (m_suspectL10NUsage)
        {
        m_options |= i18n_check::review_style::check_suspect_l10n_string_usage;
        }
    if (m_suspectI18NUsage)
        {
        m_options |= i18n_check::review_style::check_suspect_i18n_usage;
        }
    if (m_printfMismatch)
        {
        m_options |= i18n_check::review_style::check_mismatching_printf_commands;
        }
    if (m_acceleratorMismatch)
        {
        m_options |= i18n_check::review_style::check_accelerators;
        }
    if (m_transConsistency)
        {
        m_options |= i18n_check::review_style::check_consistency;
        }
    if (m_halfWidth)
        {
        m_options |= i18n_check::review_style::check_halfwidth;
        }
    if (m_numberInconsistency)
        {
        m_options |= i18n_check::review_style::check_numbers;
        }
    if (m_lengthInconsistency)
        {
        m_options |= i18n_check::review_style::check_length;
        }
    if (m_needsContext)
        {
        m_options |= i18n_check::review_style::check_needing_context;
        }
    if (m_urlInL10NString)
        {
        m_options |= i18n_check::review_style::check_l10n_contains_url;
        }
    if (m_excessiveNonTranslatableContentInL10NString)
        {
        m_options |= i18n_check::review_style::check_l10n_contains_excessive_nonl10n_content;
        }
    if (m_multipartString)
        {
        m_options |= i18n_check::review_style::check_multipart_strings;
        }
    if (m_pluralization)
        {
        m_options |= i18n_check::review_style::check_pluaralization;
        }
    if (m_articles)
        {
        m_options |= i18n_check::review_style::check_articles_proceeding_placeholder;
        }
    if (m_concatenatedStrings)
        {
        m_options |= i18n_check::review_style::check_l10n_concatenated_strings;
        }
    if (m_literalL10NStrings)
        {
        m_options |= i18n_check::review_style::check_literal_l10n_string_comparison;
        }
    if (m_deprecatedMacro)
        {
        m_options |= i18n_check::review_style::check_deprecated_macros;
        }
    if (m_nonUTF8File)
        {
        m_options |= i18n_check::review_style::check_utf8_encoded;
        }
    if (m_UTF8FileWithBOM)
        {
        m_options |= i18n_check::review_style::check_utf8_with_signature;
        }
    if (m_unencodedExtASCII)
        {
        m_options |= i18n_check::review_style::check_unencoded_ext_ascii;
        }
    if (m_printfSingleNumber)
        {
        m_options |= i18n_check::review_style::check_printf_single_number;
        }
    if (m_numberAssignedToId)
        {
        m_options |= i18n_check::review_style::check_number_assigned_to_id;
        }
    if (m_dupValAssignedToIds)
        {
        m_options |= i18n_check::review_style::check_duplicate_value_assigned_to_ids;
        }
    if (m_malformedString)
        {
        m_options |= i18n_check::review_style::check_malformed_strings;
        }
    if (m_trailingSpaces)
        {
        m_options |= i18n_check::review_style::check_trailing_spaces;
        }
    if (m_fontIssue)
        {
        m_options |= i18n_check::review_style::check_fonts;
        }
    if (m_tabs)
        {
        m_options |= i18n_check::review_style::check_tabs;
        }
    if (m_wideLine)
        {
        m_options |= i18n_check::review_style::check_line_width;
        }
    if (m_commentMissingSpace)
        {
        m_options |= i18n_check::review_style::check_space_after_comment;
        }

    m_minCppVersion = MinCppVersion();

    if (m_exclusionList != nullptr)
        {
        m_exclusionList->GetStrings(m_excludedPaths);
        }
    m_ignoredVarsList->GetStrings(m_varsToIgnore);

    m_untranslatableNamesList->GetStrings(m_untranslatableNames);

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
void NewProjectDialog::OnFolderButtonClick([[maybe_unused]] wxCommandEvent&)
    {
    TransferDataFromWindow();
    wxDirDialog dirDlg(this, _(L"Select Folder to Analyze"));
    const wxFileName fn{ m_filePath };
    // wxFileName::IsDir doesn't work under Unix, so look at the extension to see if it a dir
    dirDlg.SetPath(fn.GetExt().empty() ? m_filePath : fn.GetPath());
    if (dirDlg.ShowModal() != wxID_OK)
        {
        return;
        }
    m_filePath = dirDlg.GetPath();
    TransferDataToWindow();
    SetFocus();
    }

//-------------------------------------------------------------
void NewProjectDialog::OnFileButtonClick([[maybe_unused]] wxCommandEvent&)
    {
    TransferDataFromWindow();
    const wxFileName fn{ m_filePath };
    wxFileDialog dialog(this, _(L"Select Files to Analyze"),
                        (fn.GetExt().empty() ? m_filePath : fn.GetPath()), wxString{},
                        _(L"All Supported Files|*.cpp;*.c;*.h;*.hpp;*.po;*.pot;*.rc;*.plist|"
                          "Source Files (*.cpp; *.c; *.h; *.hpp)|*.cpp;*.c;*.h;*.hpp|"
                          "gettext Catalogs (*.po; *.pot)|*.po;*.pot|"
                          "macOS Info.plist (Info.plist)|Info.plist|"
                          "Windows Resource Files (*.rc)|*.rc"),
                        wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_PREVIEW);
    if (dialog.ShowModal() != wxID_OK)
        {
        return;
        }

    m_filePath = dialog.GetPath();
    TransferDataToWindow();
    SetFocus();
    }

//-------------------------------------------------------------
void NewProjectDialog::OnExcludedFolderButtonClick([[maybe_unused]] wxCommandEvent&)
    {
    TransferDataFromWindow();
    wxDirDialog dirDlg(this, _(L"Select Subfolders to Ignore"), wxString{},
                       wxDD_DEFAULT_STYLE | wxDD_MULTIPLE | wxDD_DIR_MUST_EXIST);
    const wxFileName fn{ m_filePath };
    dirDlg.SetPath(fn.GetExt().empty() ? m_filePath : fn.GetPath());
    if (dirDlg.ShowModal() != wxID_OK)
        {
        return;
        }

    wxArrayString paths;
    dirDlg.GetPaths(paths);
    m_excludedPaths.insert(m_excludedPaths.end(), paths.begin(), paths.end());
    m_exclusionList->SetStrings(m_excludedPaths);

    TransferDataToWindow();
    SetFocus();
    }

//-------------------------------------------------------------
void NewProjectDialog::OnExcludedFileButtonClick([[maybe_unused]] wxCommandEvent&)
    {
    TransferDataFromWindow();
    const wxFileName fn{ m_filePath };
    wxFileDialog dialog(this, _(L"Select Files to Ignore"),
                        (fn.GetExt().empty() ? m_filePath : fn.GetPath()), wxString{},
                        _(L"All Files (*.*)|*.*"),
                        wxFD_OPEN | wxFD_MULTIPLE | wxFD_FILE_MUST_EXIST | wxFD_PREVIEW);
    if (dialog.ShowModal() != wxID_OK)
        {
        return;
        }

    wxArrayString paths;
    dialog.GetPaths(paths);
    m_excludedPaths.insert(m_excludedPaths.end(), paths.begin(), paths.end());
    m_exclusionList->SetStrings(m_excludedPaths);

    TransferDataToWindow();
    SetFocus();
    }

//-------------------------------------------------------------
void NewProjectDialog::SetAllOptions(const I18NOptions& options)
    {
    SetOptions(static_cast<i18n_check::review_style>(options.m_options));
    m_filePath = options.m_filePath;
    m_excludedPaths = options.m_excludedPaths;
    m_varsToIgnore = options.m_varsToIgnore;
    m_untranslatableNames = options.m_untranslatableNames;
    m_fuzzyTranslations = options.m_fuzzyTranslations;
    m_widthPseudoChange = options.m_widthPseudoChange;
    m_maxTranslationLongerThreshold = options.m_maxTranslationLongerThreshold;
    m_addPseudoTransBrackets = options.m_addPseudoTransBrackets;
    m_pseudoTrack = options.m_pseudoTrack;
    m_pseudoTranslationMethod = static_cast<int>(options.m_pseudoTranslationMethod);
    m_logMessagesCanBeTranslated = options.m_logMessagesCanBeTranslated;
    m_allowTranslatingPunctuationOnlyStrings = options.m_allowTranslatingPunctuationOnlyStrings;
    m_exceptionsShouldBeTranslatable = options.m_exceptionsShouldBeTranslatable;
    m_verbose = options.m_verbose;
    m_minWordsForClassifyingUnavailableString = options.m_minWordsForClassifyingUnavailableString;
    if (m_exclusionList != nullptr)
        {
        m_exclusionList->SetStrings(m_excludedPaths);
        }
    if (m_untranslatableNamesList != nullptr)
        {
        m_untranslatableNamesList->SetStrings(m_untranslatableNames);
        }
    MinCppVersion(options.m_minCppVersion);
    // editor options
    m_fallbackEncoding = options.m_fallbackEncoding;
    const auto foundEncoding = m_fontEncodings.find(m_fallbackEncoding);
    if (foundEncoding != m_fontEncodings.cend())
        {
        m_fallbackEncodingStr = foundEncoding->second;
        }

    TransferDataToWindow();

    UpdateEmbeddedStringsOptions();
    UpdateConsistencyOptions();
    UpdatePseudoTransOptions();
    }

//-------------------------------------------------------------
void NewProjectDialog::CreateControls()
    {
    const auto buildCodeLabel = [](const wxString& label, wxWindow* parent) -> wxWindow*
    {
        if (wxGetMouseState().ShiftDown())
            {
            wxStaticText* statLabel = new wxStaticText(parent, wxID_STATIC, L"[" + label + L"]");
            statLabel->SetFont(statLabel->GetFont().Bold());
            statLabel->SetForegroundColour(wxSystemSettings::GetAppearance().IsDark() ?
                                               wxColour{ L"#89CFF0" } :
                                               wxColour{ L"#0095B6" });
            return statLabel;
            }
        else
            {
            return new wxStaticText(parent, wxID_STATIC, wxString{}, wxDefaultPosition,
                                    wxDefaultSize);
            }
    };

    wxListbook* listBook = new wxListbook(this, wxID_ANY);
    const wxSize imageSize{ 32, 32 };
    wxBookCtrlBase::Images images;
    images.push_back(wxArtProvider::GetBitmapBundle(wxART_FILE_OPEN, wxART_OTHER, imageSize));
    images.push_back(wxArtProvider::GetBitmapBundle(L"ID_CODE", wxART_OTHER, imageSize));
    images.push_back(wxArtProvider::GetBitmapBundle(L"ID_TRANSLATIONS", wxART_OTHER, imageSize));
    images.push_back(wxArtProvider::GetBitmapBundle(L"ID_CHECK", wxART_OTHER, imageSize));
    images.push_back(wxArtProvider::GetBitmapBundle(wxART_EDIT, wxART_OTHER, imageSize));
    listBook->SetImages(images);

    wxBoxSizer* mainDlgSizer = new wxBoxSizer(wxVERTICAL);

    // input options
    if (static_cast<bool>(m_extraPages & FilePage))
        {
        wxPanel* inputPage =
            new wxPanel(listBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

            {
            wxStaticBoxSizer* fileBrowseBoxSizer =
                new wxStaticBoxSizer(wxHORIZONTAL, inputPage, _(L"Folder/file to analyze"));

            mainSizer->Add(fileBrowseBoxSizer, wxSizerFlags{}.Expand().Border());

            wxTextCtrl* filePathEdit = new wxTextCtrl(
                fileBrowseBoxSizer->GetStaticBox(), wxID_ANY, wxString{}, wxDefaultPosition,
                wxSize{ FromDIP(400), -1 }, wxTE_RICH2 | wxBORDER_THEME | wxTE_BESTWRAP,
                wxGenericValidator(&m_filePath));
            filePathEdit->AutoCompleteFileNames();
            filePathEdit->AutoCompleteDirectories();
            fileBrowseBoxSizer->Add(filePathEdit, wxSizerFlags{ 1 }.Expand());

            wxBitmapButton* folderBrowseButton =
                new wxBitmapButton(fileBrowseBoxSizer->GetStaticBox(), ID_FOLDER_BROWSE_BUTTON,
                                   wxArtProvider::GetBitmapBundle(wxART_FOLDER_OPEN, wxART_BUTTON));
            fileBrowseBoxSizer->Add(folderBrowseButton, wxSizerFlags{}.CenterVertical());

            wxBitmapButton* fileBrowseButton =
                new wxBitmapButton(fileBrowseBoxSizer->GetStaticBox(), ID_FILE_BROWSE_BUTTON,
                                   wxArtProvider::GetBitmapBundle(wxART_NEW, wxART_BUTTON));
            fileBrowseBoxSizer->Add(fileBrowseButton, wxSizerFlags{}.CenterVertical());

            filePathEdit->SetFocus();
            }

            // files/folders to ignore
            {
            wxBoxSizer* fileBrowseBoxSizer = new wxBoxSizer(wxHORIZONTAL);

            mainSizer->Add(fileBrowseBoxSizer, wxSizerFlags{ 1 }.Expand().Border());

            m_exclusionList =
                new wxEditableListBox(inputPage, wxID_ANY, _(L"Subfolders/files to ignore"));
            fileBrowseBoxSizer->Add(m_exclusionList, wxSizerFlags{ 1 }.Expand());

            wxBitmapButton* folderExcludeBrowseButton =
                new wxBitmapButton(inputPage, ID_EXCLUDED_FOLDERS_BROWSE_BUTTON,
                                   wxArtProvider::GetBitmapBundle(wxART_FOLDER_OPEN, wxART_BUTTON));
            fileBrowseBoxSizer->Add(folderExcludeBrowseButton);

            wxBitmapButton* fileExcludeBrowseButton =
                new wxBitmapButton(inputPage, ID_EXCLUDED_FILES_BROWSE_BUTTON,
                                   wxArtProvider::GetBitmapBundle(wxART_NEW, wxART_BUTTON));
            fileBrowseBoxSizer->Add(fileExcludeBrowseButton);
            }

        inputPage->SetSizer(mainSizer);
        listBook->AddPage(inputPage, _(L"Input"), true, 0);
        }

        // source code options
        {
        wxPanel* generalSettingsPage =
            new wxPanel(listBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

        wxStaticBoxSizer* checkOptionsSizer =
            new wxStaticBoxSizer(wxVERTICAL, generalSettingsPage, _(L"Run the following checks:"));

        wxGridBagSizer* gbSizer = new wxGridBagSizer(wxSizerFlags::GetDefaultBorder());
        size_t currentRow{ 0 };

        gbSizer->Add(new wxCheckBox(checkOptionsSizer->GetStaticBox(),
                                    ID_CHECK_STRINGS_NOT_AVAILABLE,
                                    _(L"Strings not exposed for translation"), wxDefaultPosition,
                                    wxDefaultSize, 0, wxGenericValidator(&m_notL10NAvailable)),
                     wxGBPosition(currentRow, 0), wxGBSpan{});
        gbSizer->Add(buildCodeLabel(L"notL10NAvailable", checkOptionsSizer->GetStaticBox()),
                     wxGBPosition(currentRow++, 1), wxGBSpan{});

        m_ignoredVarsList = new wxEditableListBox(checkOptionsSizer->GetStaticBox(), wxID_ANY,
                                                  _(L"Ignore strings assigned to variables named:"),
                                                  wxDefaultPosition, wxSize{ -1, FromDIP(100) });
        gbSizer->Add(m_ignoredVarsList, wxGBPosition(currentRow++, 0), wxGBSpan{ 1, 2 }, wxLEFT,
                     wxSizerFlags::GetDefaultBorder() * 3);

        gbSizer->Add(new wxCheckBox(checkOptionsSizer->GetStaticBox(), wxID_ANY,
                                    _(L"Translatable strings that shouldn't be"), wxDefaultPosition,
                                    wxDefaultSize, 0, wxGenericValidator(&m_suspectL10NString)),
                     wxGBPosition(currentRow, 0), wxGBSpan{});
        gbSizer->Add(buildCodeLabel(L"suspectL10NString", checkOptionsSizer->GetStaticBox()),
                     wxGBPosition(currentRow++, 1), wxGBSpan{});

        gbSizer->Add(new wxCheckBox(checkOptionsSizer->GetStaticBox(), wxID_ANY,
                                    _(L"Translatable strings being used "
                                      "in debug functions"),
                                    wxDefaultPosition, wxDefaultSize, 0,
                                    wxGenericValidator(&m_suspectL10NUsage)),
                     wxGBPosition(currentRow, 0), wxGBSpan{});
        gbSizer->Add(buildCodeLabel(L"suspectL10NUsage", checkOptionsSizer->GetStaticBox()),
                     wxGBPosition(currentRow++, 1), wxGBSpan{});

        gbSizer->Add(new wxCheckBox(checkOptionsSizer->GetStaticBox(), wxID_ANY,
                                    _(L"Translatable strings that contain URLs, "
                                      "email addresses, or phone numbers"),
                                    wxDefaultPosition, wxDefaultSize, 0,
                                    wxGenericValidator(&m_urlInL10NString)),
                     wxGBPosition(currentRow, 0), wxGBSpan{});
        gbSizer->Add(buildCodeLabel(L"urlInL10NString", checkOptionsSizer->GetStaticBox()),
                     wxGBPosition(currentRow++, 1), wxGBSpan{});

        gbSizer->Add(
            new wxCheckBox(checkOptionsSizer->GetStaticBox(), wxID_ANY,
                           _(L"Translatable strings that contain an excessive amount "
                             "of non-translatable content"),
                           wxDefaultPosition, wxDefaultSize, 0,
                           wxGenericValidator(&m_excessiveNonTranslatableContentInL10NString)),
            wxGBPosition(currentRow, 0), wxGBSpan{});
        gbSizer->Add(buildCodeLabel(L"excessiveNonL10NContent", checkOptionsSizer->GetStaticBox()),
                     wxGBPosition(currentRow++, 1), wxGBSpan{});

        gbSizer->Add(new wxCheckBox(checkOptionsSizer->GetStaticBox(), wxID_ANY,
                                    _(L"Concatenated strings"), wxDefaultPosition, wxDefaultSize, 0,
                                    wxGenericValidator(&m_concatenatedStrings)),
                     wxGBPosition(currentRow, 0), wxGBSpan{});
        gbSizer->Add(buildCodeLabel(L"concatenatedStrings", checkOptionsSizer->GetStaticBox()),
                     wxGBPosition(currentRow++, 1), wxGBSpan{});

        gbSizer->Add(
            new wxCheckBox(checkOptionsSizer->GetStaticBox(), wxID_ANY,
                           _(L"Literal, localizable strings being compared or searched for"),
                           wxDefaultPosition, wxDefaultSize, 0,
                           wxGenericValidator(&m_literalL10NStrings)),
            wxGBPosition(currentRow, 0), wxGBSpan{});
        gbSizer->Add(buildCodeLabel(L"literalL10NStringCompare", checkOptionsSizer->GetStaticBox()),
                     wxGBPosition(currentRow++, 1), wxGBSpan{});

        gbSizer->Add(new wxCheckBox(checkOptionsSizer->GetStaticBox(), wxID_ANY,
                                    _(LR"(Multipart strings ("mega strings"))"), wxDefaultPosition,
                                    wxDefaultSize, 0, wxGenericValidator(&m_multipartString)),
                     wxGBPosition(currentRow, 0), wxGBSpan{});
        gbSizer->Add(buildCodeLabel(L"multipartString", checkOptionsSizer->GetStaticBox()),
                     wxGBPosition(currentRow++, 1), wxGBSpan{});

        gbSizer->Add(new wxCheckBox(checkOptionsSizer->GetStaticBox(), wxID_ANY,
                                    _(L"Strings used for both singular and plural"),
                                    wxDefaultPosition, wxDefaultSize, 0,
                                    wxGenericValidator(&m_pluralization)),
                     wxGBPosition(currentRow, 0), wxGBSpan{});
        gbSizer->Add(buildCodeLabel(L"pluralization", checkOptionsSizer->GetStaticBox()),
                     wxGBPosition(currentRow++, 1), wxGBSpan{});

        gbSizer->Add(new wxCheckBox(checkOptionsSizer->GetStaticBox(), wxID_ANY,
                                    _(L"Articles/pronouns mixed with dynamic content"),
                                    wxDefaultPosition, wxDefaultSize, 0,
                                    wxGenericValidator(&m_articles)),
                     wxGBPosition(currentRow, 0), wxGBSpan{});
        gbSizer->Add(buildCodeLabel(L"articleOrPronoun", checkOptionsSizer->GetStaticBox()),
                     wxGBPosition(currentRow++, 1), wxGBSpan{});

        gbSizer->Add(new wxCheckBox(checkOptionsSizer->GetStaticBox(), wxID_ANY,
                                    _(L"Suspect i18n usage"), wxDefaultPosition, wxDefaultSize, 0,
                                    wxGenericValidator(&m_suspectI18NUsage)),
                     wxGBPosition(currentRow, 0), wxGBSpan{});
        gbSizer->Add(buildCodeLabel(L"suspectI18NUsage", checkOptionsSizer->GetStaticBox()),
                     wxGBPosition(currentRow++, 1), wxGBSpan{});

        gbSizer->Add(new wxCheckBox(
                         checkOptionsSizer->GetStaticBox(), wxID_ANY,
                         _(L"Check for ambiguous strings that lack a translator comment"),
                         wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_needsContext)),
                     wxGBPosition(currentRow, 0), wxGBSpan{});
        gbSizer->Add(buildCodeLabel(L"L10NStringNeedsContext", checkOptionsSizer->GetStaticBox()),
                     wxGBPosition(currentRow++, 1), wxGBSpan{});

        gbSizer->Add(new wxCheckBox(checkOptionsSizer->GetStaticBox(), wxID_ANY,
                                    _(L"Deprecated macros and functions"), wxDefaultPosition,
                                    wxDefaultSize, 0, wxGenericValidator(&m_deprecatedMacro)),
                     wxGBPosition(currentRow, 0), wxGBSpan{});
        gbSizer->Add(buildCodeLabel(L"deprecatedMacro", checkOptionsSizer->GetStaticBox()),
                     wxGBPosition(currentRow++, 1), wxGBSpan{});

        gbSizer->Add(new wxCheckBox(checkOptionsSizer->GetStaticBox(), wxID_ANY,
                                    _(L"printf functions being used to just "
                                      "format a number"),
                                    wxDefaultPosition, wxDefaultSize, 0,
                                    wxGenericValidator(&m_printfSingleNumber)),
                     wxGBPosition(currentRow, 0), wxGBSpan{});
        gbSizer->Add(buildCodeLabel(L"printfSingleNumber", checkOptionsSizer->GetStaticBox()),
                     wxGBPosition(currentRow++, 1), wxGBSpan{});

        gbSizer->Add(new wxCheckBox(checkOptionsSizer->GetStaticBox(), wxID_ANY,
                                    _(L"Malformed syntax in strings"), wxDefaultPosition,
                                    wxDefaultSize, 0, wxGenericValidator(&m_malformedString)),
                     wxGBPosition(currentRow, 0), wxGBSpan{});
        gbSizer->Add(buildCodeLabel(L"malformedString", checkOptionsSizer->GetStaticBox()),
                     wxGBPosition(currentRow++, 1), wxGBSpan{});

        checkOptionsSizer->Add(gbSizer, wxSizerFlags{}.Border());

        mainSizer->Add(checkOptionsSizer, wxSizerFlags{}.Expand().Border());

        mainSizer->Add(new wxCheckBox(generalSettingsPage, wxID_ANY,
                                      _(L"Strings passed to logging functions can be translatable"),
                                      wxDefaultPosition, wxDefaultSize, 0,
                                      wxGenericValidator(&m_logMessagesCanBeTranslated)),
                       wxSizerFlags{}.Border().Left());
        mainSizer->Add(new wxCheckBox(generalSettingsPage, wxID_ANY,
                                      _(L"Exception messages should be available for translation"),
                                      wxDefaultPosition, wxDefaultSize, 0,
                                      wxGenericValidator(&m_exceptionsShouldBeTranslatable)),
                       wxSizerFlags{}.Border().Left());
        mainSizer->Add(
            new wxCheckBox(generalSettingsPage, wxID_ANY,
                           _(L"Punctuation only strings can be translatable"), wxDefaultPosition,
                           wxDefaultSize, 0,
                           wxGenericValidator(&m_allowTranslatingPunctuationOnlyStrings)),
            wxSizerFlags{}.Border().Left());

        wxBoxSizer* minWordSizer = new wxBoxSizer(wxHORIZONTAL);

        minWordSizer->Add(new wxStaticText(generalSettingsPage, wxID_STATIC,
                                           _(L"Minimum words for a string to "
                                             "be considered translatable:"),
                                           wxDefaultPosition, wxDefaultSize),
                          wxSizerFlags{}.CenterVertical().Border());

        wxSpinCtrl* minWordCtrl =
            new wxSpinCtrl(generalSettingsPage, wxID_ANY,
                           std::to_wstring(m_minWordsForClassifyingUnavailableString),
                           wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 1024);
        minWordCtrl->SetValidator(wxGenericValidator(&m_minWordsForClassifyingUnavailableString));
        minWordSizer->Add(minWordCtrl, wxSizerFlags{}.Border(wxLEFT));

        mainSizer->Add(minWordSizer, wxSizerFlags{}.Expand().Border());

        wxBoxSizer* cppVersionSizer = new wxBoxSizer(wxHORIZONTAL);

        cppVersionSizer->Add(new wxStaticText(generalSettingsPage, wxID_STATIC,
                                              _(L"C++ standard when issuing deprecation warnings:"),
                                              wxDefaultPosition, wxDefaultSize),
                             wxSizerFlags{}.CenterVertical().Border());

        wxArrayString cppVersions;
        cppVersions.Add(L"2011");
        cppVersions.Add(L"2014");
        cppVersions.Add(L"2017");
        cppVersions.Add(L"2020");
        cppVersions.Add(L"2023");
        wxChoice* cppVersionRadioBox =
            new wxChoice(generalSettingsPage, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                         cppVersions, 0, wxGenericValidator(&m_minCppVersion));
        cppVersionSizer->Add(cppVersionRadioBox, wxSizerFlags{}.Border(wxLEFT).Left());

        mainSizer->Add(cppVersionSizer, wxSizerFlags{}.Expand().Border());

        mainSizer->Add(new wxCheckBox(generalSettingsPage, wxID_ANY, _(L"Include verbose warnings"),
                                      wxDefaultPosition, wxDefaultSize, 0,
                                      wxGenericValidator(&m_verbose)),
                       wxSizerFlags{}.Border().Left());

        generalSettingsPage->SetSizer(mainSizer);
        listBook->AddPage(generalSettingsPage, _(L"Source Code"),
                          !(static_cast<bool>(m_extraPages & FilePage)), 1);
        }

        // resource files
        {
        wxPanel* transPage =
            new wxPanel(listBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

        wxStaticBoxSizer* poOptionsSizer =
            new wxStaticBoxSizer(wxVERTICAL, transPage, _(L"Translation catalogs"));

            {
            wxGridBagSizer* gbSizer = new wxGridBagSizer(wxSizerFlags::GetDefaultBorder());
            size_t currentRow{ 0 };

            gbSizer->Add(
                new wxCheckBox(poOptionsSizer->GetStaticBox(), wxID_ANY,
                               _(L"Check for inconsistent printf && positional format specifiers"),
                               wxDefaultPosition, wxDefaultSize, 0,
                               wxGenericValidator(&m_printfMismatch)),
                wxGBPosition(currentRow, 0), wxGBSpan{});
            gbSizer->Add(buildCodeLabel(L"printfMismatch", poOptionsSizer->GetStaticBox()),
                         wxGBPosition(currentRow++, 1), wxGBSpan{});

            gbSizer->Add(new wxCheckBox(poOptionsSizer->GetStaticBox(), wxID_ANY,
                                        _(L"Check for inconsistent keyboard accelerators"),
                                        wxDefaultPosition, wxDefaultSize, 0,
                                        wxGenericValidator(&m_acceleratorMismatch)),
                         wxGBPosition(currentRow, 0), wxGBSpan{});
            gbSizer->Add(buildCodeLabel(L"acceleratorMismatch", poOptionsSizer->GetStaticBox()),
                         wxGBPosition(currentRow++, 1), wxGBSpan{});

            gbSizer->Add(
                new wxCheckBox(poOptionsSizer->GetStaticBox(), ID_CHECK_TRANS_CONSISTENCY,
                               _(L"Check for inconsistent punctuation, untranslatable names, etc."),
                               wxDefaultPosition, wxDefaultSize, 0,
                               wxGenericValidator(&m_transConsistency)),
                wxGBPosition(currentRow, 0), wxGBSpan{});
            gbSizer->Add(buildCodeLabel(L"transInconsistency", poOptionsSizer->GetStaticBox()),
                         wxGBPosition(currentRow++, 1), wxGBSpan{});

            m_untranslatableNamesList =
                new wxEditableListBox(poOptionsSizer->GetStaticBox(), wxID_ANY,
                                      _(L"Untranslatable words and phrases:"),
                                      wxDefaultPosition, wxSize{ -1, FromDIP(100) });
            gbSizer->Add(m_untranslatableNamesList, wxGBPosition(currentRow++, 0), wxGBSpan{ 1, 2 },
                         wxLEFT, wxSizerFlags::GetDefaultBorder() * 3);

            gbSizer->Add(new wxCheckBox(poOptionsSizer->GetStaticBox(), wxID_ANY,
                                        _(L"Check for inconsistent numbers"), wxDefaultPosition,
                                        wxDefaultSize, 0,
                                        wxGenericValidator(&m_numberInconsistency)),
                         wxGBPosition(currentRow, 0), wxGBSpan{});
            gbSizer->Add(buildCodeLabel(L"numberInconsistency", poOptionsSizer->GetStaticBox()),
                         wxGBPosition(currentRow++, 1), wxGBSpan{});

            gbSizer->Add(new wxCheckBox(poOptionsSizer->GetStaticBox(), wxID_ANY,
                                        _(L"Check for halfwidth characters"), wxDefaultPosition,
                                        wxDefaultSize, 0, wxGenericValidator(&m_halfWidth)),
                         wxGBPosition(currentRow, 0), wxGBSpan{});
            gbSizer->Add(buildCodeLabel(L"halfWidth", poOptionsSizer->GetStaticBox()),
                         wxGBPosition(currentRow++, 1), wxGBSpan{});

            gbSizer->Add(new wxCheckBox(poOptionsSizer->GetStaticBox(), ID_CHECK_TRANS_LONGER_CHECK,
                                        _(L"Check for suspect translation lengths"),
                                        wxDefaultPosition, wxDefaultSize, 0,
                                        wxGenericValidator(&m_lengthInconsistency)),
                         wxGBPosition(currentRow, 0), wxGBSpan{});
            gbSizer->Add(buildCodeLabel(L"lengthInconsistency", poOptionsSizer->GetStaticBox()),
                         wxGBPosition(currentRow++, 1), wxGBSpan{});

            wxBoxSizer* transLongerThresholdSizer = new wxBoxSizer(wxHORIZONTAL);
            m_transLongerThresholdLabel = new wxStaticText(
                poOptionsSizer->GetStaticBox(), wxID_STATIC,
                _(L"How much translations can expand:"), wxDefaultPosition, wxDefaultSize);
            m_transLongerThresholdSlider =
                new wxSlider(poOptionsSizer->GetStaticBox(), ID_PSEUDO_WIDTH_SLIDER,
                             m_maxTranslationLongerThreshold, 0, 1000, wxDefaultPosition,
                             wxDefaultSize, wxSL_HORIZONTAL | wxSL_VALUE_LABEL,
                             wxGenericValidator(&m_maxTranslationLongerThreshold));
            m_transLongerThresholdSlider->SetPageSize(25);
            m_transLongerThresholdSlider->SetTickFreq(25);
            m_transLongerThresholdMinLabel = new wxStaticText(
                poOptionsSizer->GetStaticBox(), wxID_STATIC,
                wxString::Format(
                    // TRANSLATORS: number and then percent symbol
                    _("%s%%"), wxNumberFormatter::ToString(
                                   m_transLongerThresholdSlider->GetMin(), 0,
                                   wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                       wxNumberFormatter::Style::Style_WithThousandsSep)),
                wxDefaultPosition, wxDefaultSize);
            m_transLongerThresholdMaxLabel = new wxStaticText(
                poOptionsSizer->GetStaticBox(), wxID_STATIC,
                wxString::Format(
                    // TRANSLATORS: number and then percent symbol
                    _("%s%%"), wxNumberFormatter::ToString(
                                   m_transLongerThresholdSlider->GetMax(), 0,
                                   wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                       wxNumberFormatter::Style::Style_WithThousandsSep)),
                wxDefaultPosition, wxDefaultSize);
            transLongerThresholdSizer->Add(m_transLongerThresholdLabel,
                                           wxSizerFlags{}.Left().CenterVertical().Border(
                                               wxLEFT, wxSizerFlags::GetDefaultBorder() * 3));
            transLongerThresholdSizer->Add(m_transLongerThresholdMinLabel,
                                           wxSizerFlags{}.Border(wxLEFT).Left().Bottom());
            transLongerThresholdSizer->Add(m_transLongerThresholdSlider,
                                           wxSizerFlags{ 1 }.Expand().Border(wxLEFT));
            transLongerThresholdSizer->Add(m_transLongerThresholdMaxLabel,
                                           wxSizerFlags{}.Border(wxLEFT).Left().Bottom());
            gbSizer->Add(transLongerThresholdSizer, wxGBPosition(currentRow++, 0), wxGBSpan{ 1, 2 },
                         wxEXPAND);

            gbSizer->Add(new wxCheckBox(poOptionsSizer->GetStaticBox(), wxID_ANY,
                                        _(L"Review fuzzy translations"), wxDefaultPosition,
                                        wxDefaultSize, 0, wxGenericValidator(&m_fuzzyTranslations)),
                         wxGBPosition(currentRow, 0), wxGBSpan{});

            wxStaticBoxSizer* pseudoTransSizer = new wxStaticBoxSizer(
                wxVERTICAL, poOptionsSizer->GetStaticBox(), _(L"Pseudo-translation"));

            wxBoxSizer* pseudoTransMethodSizer = new wxBoxSizer(wxHORIZONTAL);

            pseudoTransMethodSizer->Add(new wxStaticText(pseudoTransSizer->GetStaticBox(),
                                                         wxID_STATIC, _(L"Method:"),
                                                         wxDefaultPosition, wxDefaultSize),
                                        wxSizerFlags{}.CenterVertical());

            wxArrayString pseudoOptions;
            pseudoOptions.Add(_(L"None (do not generate anything)"));
            pseudoOptions.Add(
                /* TRANSLATORS: Only in uppercase to show what uppercase looks like.
                   Translation should be uppercased also (when possible).*/
                _(L"UPPERCASE"));
            pseudoOptions.Add(_(L"Fill with 'X'es"));
            pseudoOptions.Add(_(L"European"));
            pseudoOptions.Add(_(L"Cherokee"));
            pseudoTransMethodSizer->Add(
                new wxChoice(pseudoTransSizer->GetStaticBox(), ID_PSEUDO_METHODS, wxDefaultPosition,
                             wxDefaultSize, pseudoOptions, 0,
                             wxGenericValidator(&m_pseudoTranslationMethod)),
                wxSizerFlags{}.Border(wxLEFT | wxBOTTOM).Left().CenterVertical());

            pseudoTransSizer->Add(pseudoTransMethodSizer, wxSizerFlags{}.Expand().Border());

            m_pseudoSurroundingBracketsCheckbox =
                new wxCheckBox(pseudoTransSizer->GetStaticBox(), ID_PSEUDO_BRACKETS_CHECK,
                               _(L"Add surrounding brackets"), wxDefaultPosition, wxDefaultSize, 0,
                               wxGenericValidator(&m_addPseudoTransBrackets));
            m_pseudoSurroundingBracketsCheckbox->Enable(m_pseudoTranslationMethod != 0);
            pseudoTransSizer->Add(m_pseudoSurroundingBracketsCheckbox,
                                  wxSizerFlags{}.Expand().Border(wxLEFT | wxBOTTOM));

            m_pseudoTrackCheckbox = new wxCheckBox(
                pseudoTransSizer->GetStaticBox(), ID_PSEUDO_TRACK_IDS_CHECK, _(L"Add tracking IDs"),
                wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_pseudoTrack));
            m_pseudoTrackCheckbox->Enable(m_pseudoTranslationMethod != 0);
            pseudoTransSizer->Add(m_pseudoTrackCheckbox,
                                  wxSizerFlags{}.Expand().Border(wxLEFT | wxBOTTOM));

            wxBoxSizer* pseudoWidthSizer = new wxBoxSizer(wxHORIZONTAL);

            m_pseudoSliderLabel =
                new wxStaticText(pseudoTransSizer->GetStaticBox(), wxID_STATIC,
                                 _(L"Expand/contract width:"), wxDefaultPosition, wxDefaultSize);

            m_pseudoIncreaseSlider = new wxSlider(
                pseudoTransSizer->GetStaticBox(), ID_PSEUDO_WIDTH_SLIDER, m_widthPseudoChange, -50,
                100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_VALUE_LABEL,
                wxGenericValidator(&m_widthPseudoChange));
            m_pseudoIncreaseSlider->SetPageSize(10);
            m_pseudoIncreaseSlider->SetTickFreq(10);

            m_pseudoSliderPercentMinLabel = new wxStaticText(
                pseudoTransSizer->GetStaticBox(), wxID_STATIC,
                wxString::Format(
                    // TRANSLATORS: number and then percent symbol
                    _("%s%%"), wxNumberFormatter::ToString(
                                   m_pseudoIncreaseSlider->GetMin(), 0,
                                   wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                       wxNumberFormatter::Style::Style_WithThousandsSep)),
                wxDefaultPosition, wxDefaultSize);
            m_pseudoSliderPercentMaxLabel = new wxStaticText(
                pseudoTransSizer->GetStaticBox(), wxID_STATIC,
                wxString::Format(
                    // TRANSLATORS: number and then percent symbol
                    _("%s%%"), wxNumberFormatter::ToString(
                                   m_pseudoIncreaseSlider->GetMax(), 0,
                                   wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                       wxNumberFormatter::Style::Style_WithThousandsSep)),
                wxDefaultPosition, wxDefaultSize);

            pseudoWidthSizer->Add(m_pseudoSliderLabel, wxSizerFlags{}.Left().CenterVertical());
            pseudoWidthSizer->Add(m_pseudoSliderPercentMinLabel,
                                  wxSizerFlags{}.Border(wxLEFT).Left().Bottom());
            pseudoWidthSizer->Add(m_pseudoIncreaseSlider,
                                  wxSizerFlags{ 1 }.Expand().Border(wxLEFT));
            pseudoWidthSizer->Add(m_pseudoSliderPercentMaxLabel,
                                  wxSizerFlags{}.Border(wxLEFT).Left().Bottom());

            pseudoTransSizer->Add(pseudoWidthSizer,
                                  wxSizerFlags{}.Expand().Border(wxLEFT | wxBOTTOM | wxRIGHT));

            wxFlexGridSizer* gbPreviewSizer = new wxFlexGridSizer(
                2, 2, wxSize{ wxSizerFlags::GetDefaultBorder(), wxSizerFlags::GetDefaultBorder() });
            gbPreviewSizer->AddGrowableCol(1, 1);
            gbPreviewSizer->SetFlexibleDirection(wxHORIZONTAL);

            m_previewSizer =
                new wxStaticBoxSizer(wxVERTICAL, pseudoTransSizer->GetStaticBox(), _(L"Preview"));

            gbPreviewSizer->Add(new wxStaticText(m_previewSizer->GetStaticBox(), wxID_STATIC,
                                                 _(L"Sample Text:"), wxDefaultPosition,
                                                 wxDefaultSize),
                                wxSizerFlags{}.CenterVertical());
            m_sampleTextWindow = new wxTextCtrl(m_previewSizer->GetStaticBox(), ID_SAMPLE_TEXT,
                                                _(L"Sample Text"), wxDefaultPosition, wxDefaultSize,
                                                wxTE_RICH2 | wxBORDER_THEME | wxTE_BESTWRAP);
            gbPreviewSizer->Add(m_sampleTextWindow, wxSizerFlags{}.Expand());

            gbPreviewSizer->Add(new wxStaticText(m_previewSizer->GetStaticBox(), wxID_STATIC,
                                                 _(L"Pseudo-translation:"), wxDefaultPosition,
                                                 wxDefaultSize),
                                wxSizerFlags{}.CenterVertical());
            m_previewTextWindow = new wxTextCtrl(
                m_previewSizer->GetStaticBox(), wxID_ANY, wxString{}, wxDefaultPosition,
                wxDefaultSize, wxTE_RICH2 | wxBORDER_THEME | wxTE_BESTWRAP | wxTE_READONLY);
            gbPreviewSizer->Add(m_previewTextWindow, wxSizerFlags{}.Expand());

            m_previewSizer->Add(gbPreviewSizer, wxSizerFlags{ 1 }.Expand().Border());

            pseudoTransSizer->Add(m_previewSizer,
                                  wxSizerFlags{}.Expand().Border(wxLEFT | wxBOTTOM | wxRIGHT));

            poOptionsSizer->Add(gbSizer, wxSizerFlags{}.Expand().Border());
            poOptionsSizer->Add(pseudoTransSizer, wxSizerFlags{}.Expand().Border());
            }

        wxStaticBoxSizer* rcOptionsSizer =
            new wxStaticBoxSizer(wxVERTICAL, transPage, _(L"Windows RC files"));

            {
            wxGridBagSizer* gbSizer = new wxGridBagSizer(wxSizerFlags::GetDefaultBorder());
            size_t currentRow{ 0 };

            gbSizer->Add(new wxCheckBox(rcOptionsSizer->GetStaticBox(), wxID_ANY,
                                        _(L"Check for font issues"), wxDefaultPosition,
                                        wxDefaultSize, 0, wxGenericValidator(&m_fontIssue)),
                         wxGBPosition(currentRow, 0), wxGBSpan{});
            gbSizer->Add(buildCodeLabel(L"fontIssue", rcOptionsSizer->GetStaticBox()),
                         wxGBPosition(currentRow++, 1), wxGBSpan{});
            rcOptionsSizer->Add(gbSizer, wxSizerFlags{}.Expand().Border());
            }

        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
        mainSizer->Add(poOptionsSizer, wxSizerFlags{}.Expand().Border());
        mainSizer->Add(rcOptionsSizer, wxSizerFlags{}.Expand().Border());
        mainSizer->AddStretchSpacer();
        mainSizer->Add(
            new wxStaticText(transPage, wxID_STATIC,
                             _(L"Note: additional checks will be performed on source messages.")),
            wxSizerFlags{}.Expand().Border());

        transPage->SetSizer(mainSizer);
        listBook->AddPage(transPage, _(L"Resource Files"), false, 2);
        }

        // extra checks
        {
        wxPanel* extraChecksPage =
            new wxPanel(listBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

        wxStaticBoxSizer* formattingOptionsSizer =
            new wxStaticBoxSizer(wxVERTICAL, extraChecksPage, _(L"Formatting && encoding checks:"));

            {
            wxGridBagSizer* gbSizer = new wxGridBagSizer(wxSizerFlags::GetDefaultBorder());
            size_t currentRow{ 0 };

            gbSizer->Add(new wxCheckBox(formattingOptionsSizer->GetStaticBox(), wxID_ANY,
                                        _(L"Non-UTF8 encoded files"), wxDefaultPosition,
                                        wxDefaultSize, 0, wxGenericValidator(&m_nonUTF8File)),
                         wxGBPosition(currentRow, 0), wxGBSpan{});
            gbSizer->Add(buildCodeLabel(L"nonUTF8File", formattingOptionsSizer->GetStaticBox()),
                         wxGBPosition(currentRow++, 1), wxGBSpan{});

            gbSizer->Add(new wxCheckBox(formattingOptionsSizer->GetStaticBox(), wxID_ANY,
                                        _(L"BOM/UTF-8 signatures"), wxDefaultPosition,
                                        wxDefaultSize, 0, wxGenericValidator(&m_UTF8FileWithBOM)),
                         wxGBPosition(currentRow, 0), wxGBSpan{});
            gbSizer->Add(buildCodeLabel(L"UTF8FileWithBOM", formattingOptionsSizer->GetStaticBox()),
                         wxGBPosition(currentRow++, 1), wxGBSpan{});

            gbSizer->Add(new wxCheckBox(formattingOptionsSizer->GetStaticBox(), wxID_ANY,
                                        _(L"Unencoded extended ASCII characters"),
                                        wxDefaultPosition, wxDefaultSize, 0,
                                        wxGenericValidator(&m_unencodedExtASCII)),
                         wxGBPosition(currentRow, 0), wxGBSpan{});
            gbSizer->Add(
                buildCodeLabel(L"unencodedExtASCII", formattingOptionsSizer->GetStaticBox()),
                wxGBPosition(currentRow++, 1), wxGBSpan{});

            gbSizer->Add(new wxCheckBox(formattingOptionsSizer->GetStaticBox(), wxID_ANY,
                                        _(L"Trailing spaces"), wxDefaultPosition, wxDefaultSize, 0,
                                        wxGenericValidator(&m_trailingSpaces)),
                         wxGBPosition(currentRow, 0), wxGBSpan{});
            gbSizer->Add(buildCodeLabel(L"trailingSpaces", formattingOptionsSizer->GetStaticBox()),
                         wxGBPosition(currentRow++, 1), wxGBSpan{});

            gbSizer->Add(new wxCheckBox(formattingOptionsSizer->GetStaticBox(), wxID_ANY,
                                        _(L"Tabs"), wxDefaultPosition, wxDefaultSize, 0,
                                        wxGenericValidator(&m_tabs)),
                         wxGBPosition(currentRow, 0), wxGBSpan{});
            gbSizer->Add(buildCodeLabel(L"tabs", formattingOptionsSizer->GetStaticBox()),
                         wxGBPosition(currentRow++, 1), wxGBSpan{});

            gbSizer->Add(new wxCheckBox(formattingOptionsSizer->GetStaticBox(), wxID_ANY,
                                        _(L"Overly long lines"), wxDefaultPosition, wxDefaultSize,
                                        0, wxGenericValidator(&m_wideLine)),
                         wxGBPosition(currentRow, 0), wxGBSpan{});
            gbSizer->Add(buildCodeLabel(L"wideLine", formattingOptionsSizer->GetStaticBox()),
                         wxGBPosition(currentRow++, 1), wxGBSpan{});

            gbSizer->Add(new wxCheckBox(formattingOptionsSizer->GetStaticBox(), wxID_ANY,
                                        _(L"Comments not starting with a space"), wxDefaultPosition,
                                        wxDefaultSize, 0,
                                        wxGenericValidator(&m_commentMissingSpace)),
                         wxGBPosition(currentRow, 0), wxGBSpan{});
            gbSizer->Add(
                buildCodeLabel(L"commentMissingSpace", formattingOptionsSizer->GetStaticBox()),
                wxGBPosition(currentRow++, 1), wxGBSpan{});

            formattingOptionsSizer->Add(gbSizer, wxSizerFlags{}.Expand().Border());
            }

        wxStaticBoxSizer* codeOptionsSizer =
            new wxStaticBoxSizer(wxVERTICAL, extraChecksPage, _(L"Code checks:"));

            {
            wxGridBagSizer* gbSizer = new wxGridBagSizer(wxSizerFlags::GetDefaultBorder());
            size_t currentRow{ 0 };

            gbSizer->Add(new wxCheckBox(codeOptionsSizer->GetStaticBox(), wxID_ANY,
                                        _(L"Hard-coded ID numbers"), wxDefaultPosition,
                                        wxDefaultSize, 0,
                                        wxGenericValidator(&m_numberAssignedToId)),
                         wxGBPosition(currentRow, 0), wxGBSpan{});
            gbSizer->Add(buildCodeLabel(L"numberAssignedToId", codeOptionsSizer->GetStaticBox()),
                         wxGBPosition(currentRow++, 1), wxGBSpan{});

            gbSizer->Add(new wxCheckBox(codeOptionsSizer->GetStaticBox(), wxID_ANY,
                                        _(L"ID variables assigned the same value"),
                                        wxDefaultPosition, wxDefaultSize, 0,
                                        wxGenericValidator(&m_dupValAssignedToIds)),
                         wxGBPosition(currentRow, 0), wxGBSpan{});
            gbSizer->Add(buildCodeLabel(L"dupValAssignedToIds", codeOptionsSizer->GetStaticBox()),
                         wxGBPosition(currentRow++, 1), wxGBSpan{});

            codeOptionsSizer->Add(gbSizer, wxSizerFlags{}.Border());
            }

        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
        mainSizer->Add(formattingOptionsSizer, wxSizerFlags{}.Expand().Border());
        mainSizer->Add(codeOptionsSizer, wxSizerFlags{}.Expand().Border());
        mainSizer->AddStretchSpacer();

        extraChecksPage->SetSizer(mainSizer);
        listBook->AddPage(extraChecksPage, _(L"Additional Checks"), false, 3);
        }

    // editor options
    if (static_cast<bool>(m_extraPages & EditorPage))
        {
        wxPanel* editorPage =
            new wxPanel(listBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

        wxArrayString encodings;
        for (const auto& encoding : m_fontEncodings)
            {
            encodings.Add(encoding.second);
            }
        encodings.Sort();

        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

        wxBoxSizer* encodingSizer = new wxBoxSizer(wxHORIZONTAL);

        encodingSizer->Add(new wxStaticText(editorPage, wxID_STATIC, _(L"Fallback encoding:"),
                                            wxDefaultPosition, wxDefaultSize),
                           wxSizerFlags{}.CenterVertical().Border());

        wxChoice* encodingRadioBox =
            new wxChoice(editorPage, wxID_ANY, wxDefaultPosition, wxDefaultSize, encodings, 0,
                         wxGenericValidator(&m_fallbackEncodingStr));
        encodingSizer->Add(encodingRadioBox, wxSizerFlags{}.Border(wxLEFT).Left());

        mainSizer->Add(encodingSizer, wxSizerFlags{}.Expand().Border());

        editorPage->SetSizer(mainSizer);
        listBook->AddPage(editorPage, _(L"Editor"), false, 4);
        }

    mainDlgSizer->Add(listBook, wxSizerFlags{ 1 }.Expand().Border());
    mainDlgSizer->Add(CreateSeparatedButtonSizer(wxOK | wxCANCEL | wxHELP),
                      wxSizerFlags{}.Expand().Border());

    SetSizerAndFit(mainDlgSizer);

    TransferDataToWindow();
    UpdateEmbeddedStringsOptions();
    UpdateConsistencyOptions();
    UpdatePseudoTransOptions();
    }
