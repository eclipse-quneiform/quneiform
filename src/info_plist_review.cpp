/********************************************************************************
 * Copyright (c) 2023-2024 Blake Madden
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

#include "info_plist_review.h"

namespace i18n_check
    {
    void info_plist_file_review::operator()(const std::wstring_view infoFileText,
                                            const std::filesystem::path& fileName)
        {
        m_file_name = fileName;

        if (infoFileText.empty())
            {
            return;
            }

        static const std::wregex CFBundleLocalizationsRegEx{
            LR"(<key>CFBundleLocalizations<\/key>\s+<array>[\s<>[:alnum:]\/]+<\/array>)"
        };
        static const std::wregex CFBundleLocalStringRegEx{ LR"(<string>[[:alnum:]]+<\/string>)" };

        // check to see if any languages are supported
        if (static_cast<bool>(get_style() & check_suspect_i18n_usage))
            {
            auto currentTextBlock{ infoFileText };
            std::match_results<decltype(currentTextBlock)::const_iterator> stPositions;
            if (std::regex_search(currentTextBlock.cbegin(), currentTextBlock.cend(), stPositions,
                                  CFBundleLocalizationsRegEx))
                {
                currentTextBlock =
                    currentTextBlock.substr(stPositions.position(), stPositions.length());
                size_t langCount{ 0 };
                while (std::regex_search(currentTextBlock.cbegin(), currentTextBlock.cend(),
                                         stPositions, CFBundleLocalStringRegEx))
                    {
                    currentTextBlock = currentTextBlock.substr(stPositions.position());
                    currentTextBlock = currentTextBlock.substr(stPositions.length());
                    ++langCount;
                    }
                if (langCount == 0)
                    {
                    m_noLocalizationBundles.emplace_back(
                        std::wstring{},
                        string_info::usage_info(string_info::usage_info::usage_type::orphan,
                                                std::wstring{}, std::wstring{}, std::wstring{}),
                        m_file_name, std::make_pair(std::wstring::npos, std::wstring::npos));
                    }
                }
            else
                {
                m_noLocalizationBundles.emplace_back(
                    std::wstring{},
                    string_info::usage_info(string_info::usage_info::usage_type::orphan,
                                            std::wstring{}, std::wstring{}, std::wstring{}),
                    m_file_name, std::make_pair(std::wstring::npos, std::wstring::npos));
                }
            }
        }
    } // namespace i18n_check
