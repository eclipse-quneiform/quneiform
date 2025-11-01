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

#include "quarto_review.h"

namespace i18n_check
    {
    //--------------------------------------------------
    void quarto_review::operator()(std::wstring_view srcText, const std::filesystem::path& fileName)
        {
        m_file_name = fileName;
        m_file_start = nullptr;

        if (srcText.empty())
            {
            return;
            }

        std::wstring filteredContent{ srcText };

        // remove code blocks
        auto startPos = filteredContent.find(L"```");
        while (startPos != std::wstring::npos)
            {
            auto endPos = filteredContent.find(L"```", startPos + 3);
            if (endPos == std::wstring::npos)
                {
                break;
                }
            endPos += 3;
            clear_section(filteredContent, startPos, endPos);
            startPos = filteredContent.find(L"```", endPos);
            }

        std::wregex multiSentenceLineRE(
            LR"(([A-Za-zÀ-ÖØ-öø-ÿ]+(?:\.[A-Za-zÀ-ÖØ-öø-ÿ]+)*\.)[”’"'»)\]]*[ \t]+[“"'(]*[A-ZÀ-ÖØ-Þ0-9])",
            std::regex_constants::ECMAScript);
        std::wregex abbrevRE(
            L"\\b(?:(?:"
            // Country / organization abbreviations
            "(?:U\\.S\\.A|U\\.S|U\\.K|E\\.U|U\\.N|N\\.A\\.T\\.O|F\\.B\\.I|C\\.I\\.A|A\\.T\\.M)"
            "|"
            // English
            "(?:Mr|Mrs|Ms|Dr|Prof|Sr|Jr|St|Mt|vs|etc|e\\.g|i\\.e|Jan|Feb|Mar|Apr|"
            "Jun|Jul|Aug|Sep|Oct|Nov|Dec|a\\.m|p\\.m)"
            "|"
            // French
            "(?:Mme|Mlle|M\\.|p\\.ex|c\\.à\\.d|env|av|apr|approx|No|n°|vol|ch)"
            "|"
            // Spanish
            "(?:Sr|Sra|Srta|Ud|Uds|Dña|p\\.ej|etc)"
            "|"
            // German
            "(?:Hr|Fr|Dr|Prof|bzw|z\\.B|u\\.a|usw|ca)"
            ")\\.)",
            std::regex_constants::ECMAScript | std::regex_constants::icase);

        if ((get_style() & check_l10n_strings) != 0U)
            {
            auto currentTextBlock{ filteredContent };

            std::vector<std::pair<size_t, std::wstring>> multiSentenceEntries;
            std::match_results<decltype(currentTextBlock)::const_iterator> stPositions;
            size_t currentBlockOffset{ 0 };
            while (std::regex_search(currentTextBlock.cbegin(), currentTextBlock.cend(),
                                     stPositions, multiSentenceLineRE))
                {
                currentBlockOffset += stPositions.position();

                const std::wstring lastWord = (stPositions.size() > 1) ? stPositions[1].str() : L"";

                // flag if not an abbreviation, meaning that this is really the end of a sentence
                if (!std::regex_match(lastWord, abbrevRE))
                    {
                    multiSentenceEntries.emplace_back(
                        currentBlockOffset,
                        currentTextBlock.substr(stPositions.position(), stPositions.length()));
                    }

                currentBlockOffset += stPositions.length();

                currentTextBlock =
                    currentTextBlock.substr(stPositions.position() + stPositions.length());
                }

            for (const auto& mlEntry : multiSentenceEntries)
                {
                const auto filePos = get_line_and_column(mlEntry.first, filteredContent);
                m_sentencesOnSameLine.emplace_back(
                    mlEntry.second.length() > 32 ? mlEntry.second.substr(0, 29) + _DT(L"...") :
                                                   mlEntry.second,
                    string_info::usage_info(string_info::usage_info::usage_type::orphan,
                                            std::wstring{}, std::wstring{}, std::wstring{}),
                    m_file_name, filePos);
                }
            }

        std::wregex splitLineSentenceRE(LR"(([A-Za-z]+)\r?\n([A-Za-z])([A-Za-z]| ))",
                                        std::regex_constants::ECMAScript);

        if ((get_style() & check_l10n_strings) != 0U)
            {
            auto currentTextBlock{ filteredContent };

            std::vector<std::pair<size_t, std::wstring>> splitSentenceEntries;
            std::match_results<decltype(currentTextBlock)::const_iterator> stPositions;
            size_t currentBlockOffset{ 0 };
            while (std::regex_search(currentTextBlock.cbegin(), currentTextBlock.cend(),
                                     stPositions, splitLineSentenceRE))
                {
                currentBlockOffset += stPositions.position();

                splitSentenceEntries.emplace_back(
                    currentBlockOffset,
                    currentTextBlock.substr(stPositions.position(), stPositions.length()));

                currentBlockOffset += stPositions.length();

                currentTextBlock =
                    currentTextBlock.substr(stPositions.position() + stPositions.length());
                }

            for (const auto& ssEntry : splitSentenceEntries)
                {
                const auto filePos = get_line_and_column(ssEntry.first, filteredContent);
                m_sentencesSplitOnDifferentLines.emplace_back(
                    ssEntry.second.length() > 32 ? ssEntry.second.substr(0, 29) + _DT(L"...") :
                                                   ssEntry.second,
                    string_info::usage_info(string_info::usage_info::usage_type::orphan,
                                            std::wstring{}, std::wstring{}, std::wstring{}),
                    m_file_name, filePos);
                }
            }

        std::wregex malformedContentRE(
            LR"((\{\{\s*[<%]\s*(?:pagebreak|include|video|embed|placeholder|lipsum)\b[^}\r\n]*[>%]\s*\}\})(?=\r?\n(?!$)[^\S\r\n]*\S))",
            std::regex_constants::ECMAScript);

        if ((get_style() & check_malformed_strings) != 0U)
            {
            auto currentTextBlock{ filteredContent };

            std::vector<std::pair<size_t, std::wstring>> malformedEntries;
            std::match_results<decltype(currentTextBlock)::const_iterator> stPositions;
            size_t currentBlockOffset{ 0 };
            while (std::regex_search(currentTextBlock.cbegin(), currentTextBlock.cend(),
                                     stPositions, malformedContentRE))
                {
                currentBlockOffset += stPositions.position();

                malformedEntries.emplace_back(
                    currentBlockOffset,
                    currentTextBlock.substr(stPositions.position(), stPositions.length()));

                currentBlockOffset += stPositions.length();

                currentTextBlock =
                    currentTextBlock.substr(stPositions.position() + stPositions.length());
                }

            for (const auto& mcEntry : malformedEntries)
                {
                const auto filePos = get_line_and_column(mcEntry.first, filteredContent);
                m_malformedContent.emplace_back(
                    mcEntry.second,
                    string_info::usage_info(string_info::usage_info::usage_type::orphan,
                                            std::wstring{}, std::wstring{}, std::wstring{}),
                    m_file_name, filePos);
                }
            }
        }
    } // namespace i18n_check
