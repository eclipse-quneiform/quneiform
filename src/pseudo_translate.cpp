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

#include "pseudo_translate.h"

namespace i18n_check
    {
    const std::map<wchar_t, wchar_t> pseudo_translator::m_euro_char_map = {
        { L'a', L'\u00E0' }, { L'A', L'\u00C0' }, { L'b', L'\u0180' }, { L'B', L'\u0181' },
        { L'c', L'\u00E7' }, { L'C', L'\u00C7' }, { L'd', L'\u010F' }, { L'D', L'\u010E' },
        { L'e', L'\u00EA' }, { L'E', L'\u00CA' }, { L'f', L'\u0192' }, { L'F', L'\u0191' },
        { L'g', L'\u01F5' }, { L'G', L'\u0193' }, { L'h', L'\u1E25' }, { L'H', L'\u1E24' },
        { L'i', L'\u00EC' }, { L'I', L'\u00CC' }, { L'j', L'\u0249' }, { L'J', L'\u0248' },
        { L'k', L'\u01E9' }, { L'K', L'\u01E8' }, { L'l', L'\u0142' }, { L'L', L'\u0141' },
        { L'm', L'\u1E41' }, { L'M', L'\u1E40' }, { L'n', L'\u0148' }, { L'N', L'\u0147' },
        { L'o', L'\u00F6' }, { L'O', L'\u00D6' }, { L'p', L'\u0440' }, { L'P', L'\u0420' },
        { L'q', L'\u024A' }, { L'Q', L'\u024A' }, { L'r', L'\u0213' }, { L'R', L'\u0212' },
        { L's', L'\u015B' }, { L'S', L'\u015A' }, { L't', L'\u021B' }, { L'T', L'\u021A' },
        { L'u', L'\u00FC' }, { L'U', L'\u00DC' }, { L'v', L'\u1E7F' }, { L'V', L'\u1E7E' },
        { L'w', L'\u1E87' }, { L'W', L'\u1E86' }, { L'x', L'\u0445' }, { L'X', L'\u0425' },
        { L'y', L'\u00FD' }, { L'Y', L'\u00DD' }, { L'z', L'\u01B6' }, { L'Z', L'\u01B5' },
        { L'0', L'\u2070' }, { L'1', L'\u2081' }, { L'2', L'\u01BB' }, { L'3', L'\u01B7' },
        { L'4', L'\u2463' }, { L'5', L'\u01BD' }, { L'6', L'\u2465' }, { L'7', L'\u247A' },
        { L'8', L'\u0223' }, { L'9', L'\u277E' }
    };

    const std::map<wchar_t, wchar_t> pseudo_translator::m_cherokee_char_map = {
        { L'a', L'\u13F9' }, { L'A', L'\u13AA' }, { L'b', L'\u13CF' }, { L'B', L'\u13F4' },
        { L'c', L'\u13E3' }, { L'C', L'\u13E3' }, { L'd', L'\u13E7' }, { L'D', L'\u13A0' },
        { L'e', L'\u13CB' }, { L'E', L'\u13AC' }, { L'f', L'\u13B8' }, { L'F', L'\u13C5' },
        { L'g', L'\u13FD' }, { L'G', L'\u13C0' }, { L'h', L'\u13C2' }, { L'H', L'\u13BB' },
        { L'i', L'\u13A5' }, { L'I', L'\u13C6' }, { L'j', L'\u13D7' }, { L'J', L'\u13AB' },
        { L'k', L'\u13E6' }, { L'K', L'\u13E6' }, { L'l', L'\u13DD' }, { L'L', L'\u13DE' },
        { L'm', L'\u13B7' }, { L'M', L'\u13B7' }, { L'n', L'\u13F2' }, { L'N', L'\u13F2' },
        { L'o', L'\u00F6' }, { L'O', L'\u13BE' }, { L'p', L'\u13AE' }, { L'P', L'\u13E2' },
        { L'q', L'\u13E1' }, { L'Q', L'\u13A4' }, { L'r', L'\u13B1' }, { L'R', L'\u13A1' },
        { L's', L'\u13A6' }, { L'S', L'\u13A6' }, { L't', L'\u13BF' }, { L'T', L'\u13A2' },
        { L'u', L'\u13E0' }, { L'U', L'\u13EC' }, { L'v', L'\u13E4' }, { L'V', L'\u13D9' },
        { L'w', L'\u13C7' }, { L'W', L'\u13B3' }, { L'x', L'\u13DC' }, { L'X', L'\u13DC' },
        { L'y', L'\u13BD' }, { L'Y', L'\u13A9' }, { L'z', L'\u13B9' }, { L'Z', L'\u13B9' },
        { L'0', L'\uFF10' }, { L'1', L'\uFF11' }, { L'2', L'\uFF12' }, { L'3', L'\uFF13' },
        { L'4', L'\u13CE' }, { L'5', L'\uFF15' }, { L'6', L'\uFF16' }, { L'7', L'\uFF17' },
        { L'8', L'\uFF18' }, { L'9', L'\u13EE' }
    };

    //------------------------------------------------
    void pseudo_translator::translate_po_file(std::wstring& poFileText) const
        {
        if (poFileText.empty())
            {
            return;
            }

        static const std::wstring_view MSGID{ L"msgid \"" };
        static const std::wstring_view MSGID_PLURAL{ L"msgid_plural \"" };
        static const std::wstring_view MSGSTR{ L"msgstr \"" };
        static const std::wstring_view MSGSTR0{ L"msgstr[0] \"" };
        static const std::wstring_view MSGSTR1{ L"msgstr[1] \"" };

        std::wstring_view fileContent{ poFileText };

        size_t currentPosition{ 0 };

        // find the first blank line so that we can skip over the header section
        while (true)
            {
            const size_t newLinePos = fileContent.find(L'\n', currentPosition);
            // if no blank lines, then bail as there will be nothing to load
            if (newLinePos == std::wstring_view::npos || newLinePos == fileContent.length() - 1)
                {
                return;
                }
            if (fileContent[newLinePos + 1] == L'\r' || fileContent[newLinePos + 1] == L'\n')
                {
                currentPosition = newLinePos;
                fileContent.remove_prefix(currentPosition);
                break;
                }
            currentPosition = newLinePos + 1;
            }

        auto [foundEntry, entryContent, entryPos] = i18n_review::read_po_catalog_entry(fileContent);
        while (foundEntry)
            {
            // step to start of catalog entry
            currentPosition += entryPos;
            fileContent = std::wstring_view{ poFileText }.substr(currentPosition);

            int64_t altertedLengthDiff{ 0 };

            // read the main source string
            std::wstring_view msgIdEntry{ entryContent };
            const auto [foundMsgId, msgIdContent, msgIdPos, msgIdLen] =
                i18n_review::read_po_msg(msgIdEntry, MSGID);
            // read the plural source string
            std::wstring_view msgIdPluralEntry{ entryContent };
            const auto [foundMsgPluralId, msgIdPluralContent, msgIdPluralPos, msgIdPluralLen] =
                i18n_review::read_po_msg(msgIdPluralEntry, MSGID_PLURAL);

            // read the plural translation...
            std::wstring_view msgStrPluralEntry{ entryContent };
            const auto [foundMsgPluralStr, msgStrPluralContent, msgStrPluralPos, msgStrPluralLen] =
                i18n_review::read_po_msg(msgStrPluralEntry, MSGSTR1);

            // if there is a plural source string, then pseudo-translate msgstr[0] based
            // on the singular form; otherwise, pseudo-translate msgstr.
            const std::wstring_view msgStrKey = (foundMsgPluralId ? MSGSTR0 : MSGSTR);
            // read the main translation
            std::wstring_view msgStrEntry{ entryContent };
            const auto [foundMsgStr, msgStrContent, msgStrPos, msgStrLen] =
                i18n_review::read_po_msg(msgStrEntry, msgStrKey);

            size_t adjustedMainTranslationLength{ 0 };

            if (foundMsgId)
                {
                if (foundMsgStr)
                    {
                    // replace main translation it with a pseudo-translation
                    auto mutatedStr{ mutate_message(msgIdContent) };
                    adjustedMainTranslationLength = mutatedStr.length() - msgStrLen;
                    altertedLengthDiff = mutatedStr.length() - msgStrLen;
                    poFileText.replace(currentPosition + msgStrPos + msgStrKey.length(), msgStrLen,
                                       std::move(mutatedStr));
                    }
                }
            // if a plural form of the source string exists, then pseudo-translate msgstr[1]
            // based on that...
            if (foundMsgPluralId)
                {
                if (foundMsgPluralStr)
                    {
                    // ...and replace it with a pseudo-translation
                    auto mutatedStr{ mutate_message(msgIdPluralContent) };
                    altertedLengthDiff += mutatedStr.length() - msgStrPluralLen;
                    poFileText.replace(currentPosition + msgStrPluralPos + MSGSTR1.length() +
                                           adjustedMainTranslationLength,
                                       msgStrPluralLen, std::move(mutatedStr));
                    }
                }

            // step to end of catalog entry and look for next one
            currentPosition += static_cast<size_t>(entryContent.length() + altertedLengthDiff);
            fileContent = std::wstring_view{ poFileText }.substr(currentPosition);
            std::tie(foundEntry, entryContent, entryPos) =
                i18n_review::read_po_catalog_entry(fileContent);
            }

        // remove any fuzzy specifiers
        const std::wstring_view FUZZY{ _DT(L"#, fuzzy") };
        size_t foundPos = poFileText.find(FUZZY);
        while (foundPos != std::wstring::npos && foundPos > 0)
            {
            size_t lastChar = poFileText.find_last_not_of(L"\r\n", foundPos - 1);
            if (lastChar == std::wstring::npos)
                {
                break;
                }
            // If line above ends in a quote, this it is probably a different entry.
            // That means this entry is missing references and is probably a commented
            // out section. In that case, skip over it.
            if (poFileText[lastChar] == L'"')
                {
                foundPos = poFileText.find(_DT(L"#, fuzzy"), foundPos + FUZZY.length());
                continue;
                }
            ++lastChar; // step forward to the first newline character
            size_t nextChar = poFileText.find_first_not_of(L"\r\n", foundPos + FUZZY.length());
            if (nextChar == std::wstring::npos)
                {
                break;
                }
            --nextChar; // step back to last newline
            poFileText.replace(lastChar, nextChar - lastChar, L"");
            foundPos = poFileText.find(_DT(L"#, fuzzy"), foundPos);
            }

        // mark the file's encoding as UTF-8
        const std::wregex CONTENT_TYPE_RE{
            LR"((\r|\n)\"Content-Type:[ ]*text/plain;[ ]*charset[ ]*=[ ]*([a-zA-Z0-9\-]*))"
        };
        std::wsmatch matches;
        if (std::regex_search(poFileText, matches, CONTENT_TYPE_RE) && matches.size() >= 3)
            {
            poFileText.replace(matches.position(2), matches.length(2), L"UTF-8");
            }

        // if target language is missing, then set to Esperanto
        const std::wregex LANG_RE{ LR"((\r|\n)\"Language:[ ]*([a-zA-Z0-9\-]*))" };
        if (std::regex_search(poFileText, matches, LANG_RE) && matches.size() >= 3 &&
            matches.length(2) == 0)
            {
            poFileText.replace(matches.position(2), matches.length(2), L"eo");
            }
        }

    //------------------------------------------------
    std::wstring pseudo_translator::mutate_message(const std::wstring& msg) const
        {
        if (msg.empty())
            {
            return msg;
            }

        // build the tracking prefix so that we can take its length into
        // account when increasing/decreasing the width of the message
        const std::wstring trackPrefix = [this]()
        {
            if (m_track)
                {
#if __cpp_lib_format > 201907L
                return L"[" + std::format(L"{:06X}", m_current_id++) + L"]";
#else
                return L"[" + std::to_wstring(m_current_id++) + L"]";
#endif
                }
            return std::wstring{};
        }();

        const auto printfSpecifiers = i18n_review::load_cpp_printf_command_positions(msg);
        const auto positionalSpecifiers = i18n_review::load_positional_command_positions(msg);
        const auto fileFilters = i18n_review::load_file_filter_positions(msg);

        // Get the position of the first character that is not a space or whitespace control
        // sequence. We will step over this and add them to the mutated string after the brackets
        // and dashes are added.
        const size_t startPos = [&msg]()
        {
            size_t i{ 0 };
            for (; i < msg.length(); /* handled in loop*/)
                {
                if (msg[i] == L' ')
                    {
                    ++i;
                    continue;
                    }
                else if (i < (msg.length() - 1) && msg[i] == L'\\' &&
                         (msg[i + 1] == L'r' || msg[i + 1] == L'n' || msg[i + 1] == L't'))
                    {
                    i += 2;
                    continue;
                    }
                break;
                }
            return i;
        }();

        // Do the same for the trailing spaces.
        const size_t endPos = [&msg]()
        {
            int32_t i{ static_cast<int32_t>(msg.length() - 1) };
            for (; i > 0; /* handled in loop*/)
                {
                if (msg[i] == L' ')
                    {
                    --i;
                    continue;
                    }
                else if ((msg[i] == L'r' || msg[i] == L'n' || msg[i] == L't') && i > 0 &&
                         msg[i - 1] == L'\\')
                    {
                    i -= 2;
                    continue;
                    }
                break;
                }
            return static_cast<size_t>(i + 1);
        }();

        // If nothing but spaces and newlines, then leave it alone.
        // A separate analysis will complain about this string,
        // depending on which checks are being performed.
        if (startPos == endPos)
            {
            return msg;
            }

        std::wstring newMsg;
        newMsg.reserve(msg.size() * 2);

        // If decreasing the width of the string, then we will copy only as much as necessary
        // from the translatable sections of the string and also take the length of ID tracking and
        // surrounding brackets into account.
        int64_t charCountToCopy{ 0 };
        if (m_width_change < 0)
            {
            int64_t charCountToRemove = static_cast<int64_t>(
                std::ceil(msg.length() * (static_cast<double>(-m_width_change) / 100)));
            charCountToRemove += trackPrefix.length();
            if (m_add_surrounding_brackets)
                {
                charCountToRemove += 2;
                }
            if (charCountToRemove >= static_cast<int64_t>(msg.length()))
                {
                charCountToRemove = (static_cast<int64_t>(msg.length()) - 1);
                }
            charCountToCopy = std::max(static_cast<int64_t>(1),
                                       static_cast<int64_t>(msg.length()) - charCountToRemove);
            }

        const auto appendChar = [&newMsg, &charCountToCopy, this](const auto chr)
        {
            if (m_width_change < 0)
                {
                if (charCountToCopy > 0)
                    {
                    newMsg += chr;
                    }
                --charCountToCopy;
                }
            else
                {
                newMsg += chr;
                }
        };

        for (size_t i = startPos; i < endPos; /* handled in loop*/)
            {
            // step over escaped characters
            if (msg[i] == L'\\')
                {
                newMsg += msg[i++];
                newMsg += msg[i++];
                continue;
                }
            // step over printf commands
            auto foundPos = std::find_if(printfSpecifiers.cbegin(), printfSpecifiers.cend(),
                                         [i](auto val) noexcept { return val.first == i; });
            if (foundPos != printfSpecifiers.cend())
                {
                newMsg += msg.substr(i, foundPos->second);
                i += foundPos->second;
                continue;
                }
            // step over file filters
            foundPos = std::find_if(fileFilters.cbegin(), fileFilters.cend(),
                                    [i](auto val) noexcept { return val.first == i; });
            if (foundPos != fileFilters.cend())
                {
                newMsg += msg.substr(i, foundPos->second);
                i += foundPos->second;
                continue;
                }
            // step over positional commands
            foundPos = std::find_if(positionalSpecifiers.cbegin(), positionalSpecifiers.cend(),
                                    [i](auto val) noexcept { return val.first == i; });
            if (foundPos != positionalSpecifiers.cend())
                {
                newMsg += msg.substr(i, foundPos->second);
                i += foundPos->second;
                continue;
                }

            if (std::iswalnum(msg[i]))
                {
                if (m_trans_type == pseudo_translation_method::all_caps)
                    {
                    appendChar(std::towupper(msg[i]));
                    }
                else if (m_trans_type == pseudo_translation_method::Xx_es)
                    {
                    if (std::iswupper(msg[i]))
                        {
                        appendChar(L'X');
                        }
                    else if (std::iswlower(msg[i]))
                        {
                        appendChar(L'x');
                        }
                    else
                        {
                        appendChar(msg[i]);
                        }
                    }
                else if (m_trans_type == pseudo_translation_method::european_characters)
                    {
                    const auto charPos = m_euro_char_map.find(msg[i]);
                    if (charPos != m_euro_char_map.cend())
                        {
                        appendChar(charPos->second);
                        }
                    else
                        {
                        appendChar(msg[i]);
                        }
                    }
                else if (m_trans_type == pseudo_translation_method::cherokee)
                    {
                    const auto charPos = m_cherokee_char_map.find(msg[i]);
                    if (charPos != m_cherokee_char_map.cend())
                        {
                        appendChar(charPos->second);
                        }
                    else
                        {
                        appendChar(msg[i]);
                        }
                    }
                else
                    {
                    appendChar(msg[i]);
                    }
                }
            else
                {
                appendChar(msg[i]);
                }
            ++i;
            }

        if (m_width_change > 0)
            {
            int64_t newCharCountToAdd = static_cast<int64_t>(
                std::ceil(msg.length() * (static_cast<double>(m_width_change) / 100)));
            if (m_add_surrounding_brackets && newCharCountToAdd >= 2)
                {
                newCharCountToAdd -= 2;
                }
            newCharCountToAdd -= trackPrefix.length();
            if (newCharCountToAdd > 0)
                {
                newMsg.insert(0, static_cast<size_t>(newCharCountToAdd) / 2, L'-');
                newMsg.append(static_cast<size_t>(newCharCountToAdd) / 2, L'-');
                }
            }

        if (m_add_surrounding_brackets)
            {
            newMsg.insert(0, L"[");
            newMsg.append(L"]");
            }

        if (m_track)
            {
            newMsg.insert(0, trackPrefix);
            }

        // prepend the leading whitespace from the source into the mutated string
        if (startPos > 0)
            {
            newMsg.insert(0, msg.substr(0, startPos));
            }
        // ...and append any trailing spaces
        if (endPos < msg.length())
            {
            newMsg.append(msg.substr(endPos));
            }

        return newMsg;
        }
    } // namespace i18n_check
