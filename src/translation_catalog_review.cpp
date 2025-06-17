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

#include "translation_catalog_review.h"

namespace i18n_check
    {
    std::vector<std::wstring> translation_catalog_review::m_untranslatable_names = {
        L"Java", L"Jakarta", L"Jakarta EE", L"Eclipse Foundation"
    };

    //------------------------------------------------
    void translation_catalog_review::review_strings(analyze_callback_reset resetCallback,
                                                    analyze_callback callback)
        {
        std::vector<std::wstring> printfStrings1, printfStrings2;
        std::wstring errorInfo;

        std::vector<std::wstring> srcResults;
        std::vector<std::wstring> transResults;
        std::wsmatch reMatches;

        const auto unrollStrings = [](const auto& strs)
        {
            std::wstring result;
            for (const auto& str : strs)
                {
                result.append(str).append(L"; ");
                }
            if (result.length() >= 2)
                {
                result.erase(result.length() - 2);
                }
            return result;
        };

        resetCallback(m_catalog_entries.size());
        size_t currentCatalogIndex{ 0 };
        for (auto& catEntry : m_catalog_entries)
            {
            if (!callback(++currentCatalogIndex, std::wstring{}))
                {
                return;
                }
            if (static_cast<bool>(m_review_styles & check_l10n_strings))
                {
                if (is_untranslatable_string(catEntry.second.m_source, false).first)
                    {
                    catEntry.second.m_issues.emplace_back(translation_issue::suspect_source_issue,
                                                          catEntry.second.m_source);
                    }
                if (!catEntry.second.m_source_plural.empty() &&
                    is_untranslatable_string(catEntry.second.m_source_plural, false).first)
                    {
                    catEntry.second.m_issues.emplace_back(translation_issue::suspect_source_issue,
                                                          catEntry.second.m_source_plural);
                    }
                }
            if (static_cast<bool>(m_review_styles & check_l10n_contains_url))
                {
                std::wsmatch results;

                if (std::regex_search(catEntry.second.m_source, results, m_url_email_regex))
                    {
                    catEntry.second.m_issues.emplace_back(translation_issue::suspect_source_issue,
                                                          catEntry.second.m_source);
                    }
                if (!catEntry.second.m_source_plural.empty() &&
                    std::regex_search(catEntry.second.m_source_plural, results, m_url_email_regex))
                    {
                    catEntry.second.m_issues.emplace_back(translation_issue::suspect_source_issue,
                                                          catEntry.second.m_source_plural);
                    }
                }
            if (static_cast<bool>(m_review_styles & check_l10n_contains_excessive_nonl10n_content))
                {
                if (const auto [isUntranslatable, translatableContentLength] =
                        is_untranslatable_string(catEntry.second.m_source, false);
                    (m_review_styles & check_l10n_contains_excessive_nonl10n_content) &&
                    !isUntranslatable &&
                    catEntry.second.m_source.length() > (translatableContentLength * 3) &&
                    catEntry.second.m_comment.empty())
                    {
                    catEntry.second.m_issues.emplace_back(
                        translation_issue::excessive_nonl10n_content, catEntry.second.m_source);
                    }

                if (const auto [isUntranslatable, translatableContentLength] =
                        is_untranslatable_string(catEntry.second.m_source_plural, false);
                    (m_review_styles & check_l10n_contains_excessive_nonl10n_content) &&
                    !isUntranslatable &&
                    catEntry.second.m_source_plural.length() > (translatableContentLength * 3) &&
                    catEntry.second.m_comment.empty())
                    {
                    catEntry.second.m_issues.emplace_back(
                        translation_issue::excessive_nonl10n_content,
                        catEntry.second.m_source_plural);
                    }
                }
            if (static_cast<bool>(m_review_styles & check_l10n_concatenated_strings))
                {
                if (has_surrounding_spaces(catEntry.second.m_source))
                    {
                    catEntry.second.m_issues.emplace_back(translation_issue::concatenation_issue,
                                                          catEntry.second.m_source);
                    }
                if (!catEntry.second.m_source_plural.empty() &&
                    has_surrounding_spaces(catEntry.second.m_source_plural))
                    {
                    catEntry.second.m_issues.emplace_back(translation_issue::concatenation_issue,
                                                          catEntry.second.m_source_plural);
                    }
                }
            if (static_cast<bool>(m_review_styles & check_needing_context))
                {
                if (catEntry.second.m_comment.empty() &&
                    is_string_ambiguous(catEntry.second.m_source))
                    {
                    catEntry.second.m_issues.emplace_back(
                        translation_issue::source_needing_context_issue, catEntry.second.m_source);
                    }
                }
            if (static_cast<bool>(m_review_styles & check_multipart_strings))
                {
                if (is_string_multipart(catEntry.second.m_source))
                    {
                    catEntry.second.m_issues.emplace_back(translation_issue::multipart_string,
                                                          catEntry.second.m_source);
                    }
                }
            if (static_cast<bool>(m_review_styles & check_pluaralization))
                {
                if (is_string_faux_plural(catEntry.second.m_source))
                    {
                    catEntry.second.m_issues.emplace_back(translation_issue::pluralization,
                                                          catEntry.second.m_source);
                    }
                }
            if (static_cast<bool>(m_review_styles & check_articles_proceeding_placeholder))
                {
                if (is_string_article_issue(catEntry.second.m_source) ||
                    is_string_pronoun(catEntry.second.m_source))
                    {
                    catEntry.second.m_issues.emplace_back(translation_issue::article_issue,
                                                          catEntry.second.m_source);
                    }
                }
            if (static_cast<bool>(m_review_styles & check_mismatching_printf_commands))
                {
                if (catEntry.second.m_po_format == po_format_string::cpp_format)
                    {
                    // only look at strings that have a translation
                    if (!catEntry.second.m_translation.empty())
                        {
                        printfStrings1 =
                            load_cpp_printf_commands(catEntry.second.m_source, errorInfo);
                        printfStrings2 =
                            load_cpp_printf_commands(catEntry.second.m_translation, errorInfo);

                        if (printfStrings1.size() || printfStrings2.size())
                            {
                            if (printfStrings1 != printfStrings2)
                                {
                                catEntry.second.m_issues.emplace_back(
                                    translation_issue::printf_issue,
                                    L"'" + catEntry.second.m_source + _WXTRANS_WSTR(L"' vs. '") +
                                        catEntry.second.m_translation + L"'" + errorInfo);
                                }
                            }
                        }

                    if (!catEntry.second.m_translation_plural.empty())
                        {
                        printfStrings1 =
                            load_cpp_printf_commands(catEntry.second.m_source_plural, errorInfo);
                        printfStrings2 = load_cpp_printf_commands(
                            catEntry.second.m_translation_plural, errorInfo);

                        if (printfStrings1.size() || printfStrings2.size())
                            {
                            if (printfStrings1 != printfStrings2)
                                {
                                catEntry.second.m_issues.emplace_back(
                                    translation_issue::printf_issue,
                                    L"'" + catEntry.second.m_source_plural +
                                        _WXTRANS_WSTR(L"' vs. '") +
                                        catEntry.second.m_translation_plural + L"'" + errorInfo);
                                }
                            }
                        }
                    }

                if (catEntry.second.m_po_format == po_format_string::qt_format)
                    {
                    const auto reviewPositionals =
                        [&catEntry, &printfStrings1, &printfStrings2,
                         &unrollStrings](const auto& src, const auto& trans)
                    {
                        // only look at strings that have a translation
                        if (!trans.empty())
                            {
                            printfStrings1 = load_positional_commands(src);
                            printfStrings2 = load_positional_commands(trans);

                            if (printfStrings1.size() || printfStrings2.size())
                                {
                                if (printfStrings1 != printfStrings2)
                                    {
                                    catEntry.second.m_issues.emplace_back(
                                        translation_issue::printf_issue,
                                        _WXTRANS_WSTR(L"Positional values differences: '") +
                                            unrollStrings(printfStrings1) +
                                            _WXTRANS_WSTR(L"' vs. '") +
                                            unrollStrings(printfStrings2) + L"'");
                                    }
                                }
                            }
                    };

                    reviewPositionals(catEntry.second.m_source, catEntry.second.m_translation);
                    reviewPositionals(catEntry.second.m_source_plural,
                                      catEntry.second.m_translation_plural);
                    }
                }

            if (static_cast<bool>(m_review_styles & check_accelerators) ||
                static_cast<bool>(m_review_styles & check_malformed_strings))
                {
                const auto reviewAccelerators =
                    [&catEntry, &srcResults, &transResults, &reMatches, this](auto src, auto trans)
                {
                    if (!trans.empty())
                        {
                        srcResults.clear();
                        transResults.clear();
                        std::wstring::const_iterator searchSrcStart{ src.cbegin() };
                        std::wstring::const_iterator searchTransStart{ trans.cbegin() };
                        while (std::regex_search(searchSrcStart, src.cend(), reMatches,
                                                 m_keyboard_accelerator_regex))
                            {
                            srcResults.push_back(reMatches[0]);
                            searchSrcStart = reMatches.suffix().first;
                            }
                        while (std::regex_search(searchTransStart, trans.cend(), reMatches,
                                                 m_keyboard_accelerator_regex))
                            {
                            transResults.push_back(reMatches[0]);
                            searchTransStart = reMatches.suffix().first;
                            }

                        if ((srcResults.size() == 1 && transResults.size() != 1) ||
                            (srcResults.size() != 1 && transResults.size() == 1))
                            {
                            if (static_cast<bool>(m_review_styles & check_accelerators))
                                {
                                catEntry.second.m_issues.emplace_back(
                                    translation_issue::accelerator_issue,
                                    L"'" + src + _WXTRANS_WSTR(L"' vs. '") + trans + L"'");
                                }
                            // if source has an accelerator key but the translation does not
                            // but it does have a %, then that probably was meant to be an &
                            if (static_cast<bool>(m_review_styles & check_malformed_strings) &&
                                srcResults.size() == 1 && src.find(L'%') == std::wstring::npos &&
                                trans.find(L'%') != std::wstring::npos)
                                {
                                catEntry.second.m_issues.emplace_back(
                                    translation_issue::malformed_translation,
                                    L"'" + src + _WXTRANS_WSTR(L"' vs. '") + trans + L"'");
                                }
                            }
                        }
                };

                reviewAccelerators(catEntry.second.m_source, catEntry.second.m_translation);
                reviewAccelerators(catEntry.second.m_source_plural,
                                   catEntry.second.m_translation_plural);
                }

            if (static_cast<bool>(m_review_styles & check_length))
                {
                // the length that a translation can be longer than the source
                const double lengthFactor{ (get_translation_length_threshold() + 100) /
                                           static_cast<double>(100) };
                if (catEntry.second.m_source != L"translator-credits" &&
                    catEntry.second.m_translation.length() >
                        (catEntry.second.m_source.length() * lengthFactor))
                    {
                    catEntry.second.m_issues.emplace_back(translation_issue::length_issue,
                                                          L"'" + catEntry.second.m_source +
                                                              _WXTRANS_WSTR(L"' vs. '") +
                                                              catEntry.second.m_translation + L"'");
                    }

                if (catEntry.second.m_source_plural != L"translator-credits" &&
                    catEntry.second.m_translation_plural.length() >
                        (catEntry.second.m_source_plural.length() * lengthFactor))
                    {
                    catEntry.second.m_issues.emplace_back(
                        translation_issue::length_issue,
                        L"'" + catEntry.second.m_source_plural + _WXTRANS_WSTR(L"' vs. '") +
                            catEntry.second.m_translation_plural + L"'");
                    }
                }

            if (static_cast<bool>(m_review_styles & check_halfwidth))
                {
                const auto reviewHW =
                    [&catEntry, &srcResults, &transResults, &unrollStrings](auto src, auto trans)
                {
                    if (!src.empty())
                        {
                        srcResults = load_matches(src, m_halfwidth_range_regex);

                        if (!srcResults.empty())
                            {
                            catEntry.second.m_issues.emplace_back(translation_issue::halfwidth,
                                                                  unrollStrings(srcResults));
                            }
                        }

                    if (!trans.empty())
                        {
                        transResults = load_matches(trans, m_halfwidth_range_regex);

                        if (!transResults.empty())
                            {
                            catEntry.second.m_issues.emplace_back(translation_issue::halfwidth,
                                                                  unrollStrings(transResults));
                            }
                        }
                };

                reviewHW(catEntry.second.m_source, catEntry.second.m_translation);
                reviewHW(catEntry.second.m_source_plural, catEntry.second.m_translation_plural);
                }

            if (static_cast<bool>(m_review_styles & check_numbers))
                {
                const auto reviewNumbers = [&catEntry, &printfStrings1, &printfStrings2,
                                            &unrollStrings](auto src, auto trans)
                {
                    // only look at strings that have a translation
                    if (!trans.empty())
                        {
                        std::for_each(src.begin(), src.end(),
                                      [](wchar_t& chr) { chr = std::towlower(chr); });
                        std::for_each(trans.begin(), trans.end(),
                                      [](wchar_t& chr) { chr = std::towlower(chr); });
                        printfStrings1 = load_numbers(src);
                        printfStrings2 = load_numbers(trans);

                        if (printfStrings1.size() || printfStrings2.size())
                            {
                            if (printfStrings1 != printfStrings2)
                                {
                                if (src == L"translator-credits")
                                    {
                                    return;
                                    }
                                // ignore where source is an imperial measurement and
                                // translation is metric
                                if ((src.ends_with(L" in") || src.ends_with(L" inch") ||
                                     src.ends_with(L" inches")) &&
                                    (trans.ends_with(L" cm") || trans.ends_with(L" mm")))
                                    {
                                    return;
                                    }
                                // common word to number translations can be ignored
                                if (printfStrings2.size() == 1 && printfStrings2[0] == L"1" &&
                                    (src.find(L"once") != std::wstring::npos ||
                                     src.find(L"first") != std::wstring::npos ||
                                     src.find(L"home") != std::wstring::npos ||
                                     src.find(L"single") != std::wstring::npos))
                                    {
                                    return;
                                    }
                                if (printfStrings2.size() == 1 && printfStrings2[0] == L"2" &&
                                    (src.find(L"twice") != std::wstring::npos ||
                                     src.find(L"second") != std::wstring::npos ||
                                     src.find(L"half") != std::wstring::npos ||
                                     src.find(L"double") != std::wstring::npos))
                                    {
                                    return;
                                    }
                                if (printfStrings2.size() == 1 && printfStrings2[0] == L"3" &&
                                    (src.find(L"thrice") != std::wstring::npos ||
                                     src.find(L"third") != std::wstring::npos ||
                                     src.find(L"triple") != std::wstring::npos))
                                    {
                                    return;
                                    }
                                if (printfStrings2.size() == 2 && printfStrings2[0] == L"1" &&
                                    printfStrings2[1] == L"3" &&
                                    src.find(L"third") != std::wstring::npos)
                                    {
                                    return;
                                    }
                                if (printfStrings2.size() == 1 && printfStrings2[0] == L"4" &&
                                    (src.find(L"fourth") != std::wstring::npos ||
                                     src.find(L"quarter") != std::wstring::npos))
                                    {
                                    return;
                                    }
                                if (printfStrings2.size() == 2 && printfStrings2[0] == L"1" &&
                                    printfStrings2[1] == L"4" &&
                                    (src.find(L"fourth") != std::wstring::npos ||
                                     src.find(L"quarter") != std::wstring::npos))
                                    {
                                    return;
                                    }
                                if (printfStrings2.size() == 1 && printfStrings2[0] == L"5" &&
                                    src.find(L"fifth") != std::wstring::npos)
                                    {
                                    return;
                                    }
                                if (printfStrings2.size() == 1 && printfStrings2[0] == L"6" &&
                                    src.find(L"sixth") != std::wstring::npos)
                                    {
                                    return;
                                    }
                                if (printfStrings2.size() == 1 && printfStrings2[0] == L"7" &&
                                    src.find(L"seventh") != std::wstring::npos)
                                    {
                                    return;
                                    }
                                if (printfStrings2.size() == 1 && printfStrings2[0] == L"8" &&
                                    src.find(L"eighth") != std::wstring::npos)
                                    {
                                    return;
                                    }
                                if (printfStrings2.size() == 1 && printfStrings2[0] == L"9" &&
                                    src.find(L"ninth") != std::wstring::npos)
                                    {
                                    return;
                                    }
                                if (printfStrings2.size() == 1 && printfStrings2[0] == L"10" &&
                                    src.find(L"tenth") != std::wstring::npos)
                                    {
                                    return;
                                    }
                                if (printfStrings2.size() == 2 && printfStrings2[0] == L"1" &&
                                    printfStrings2[1] == L"10" &&
                                    src.find(L"tenths") != std::wstring::npos)
                                    {
                                    return;
                                    }
                                if (printfStrings2.size() == 1 && printfStrings2[0] == L"11" &&
                                    src.find(L"eleven") != std::wstring::npos)
                                    {
                                    return;
                                    }
                                if (printfStrings2.size() == 1 && printfStrings2[0] == L"12" &&
                                    (src.find(L"twelve") != std::wstring::npos ||
                                     src.find(L"twelfth") != std::wstring::npos))
                                    {
                                    return;
                                    }
                                if (printfStrings2.size() == 1 && printfStrings2[0] == L"13" &&
                                    src.find(L"thirteen") != std::wstring::npos)
                                    {
                                    return;
                                    }
                                if (printfStrings2.size() == 1 && printfStrings2[0] == L"14" &&
                                    src.find(L"fourteen") != std::wstring::npos)
                                    {
                                    return;
                                    }
                                if (printfStrings2.size() == 1 && printfStrings2[0] == L"15" &&
                                    src.find(L"fifteen") != std::wstring::npos)
                                    {
                                    return;
                                    }
                                if (printfStrings2.size() == 1 && printfStrings2[0] == L"16" &&
                                    src.find(L"sixteen") != std::wstring::npos)
                                    {
                                    return;
                                    }
                                if (printfStrings2.size() == 1 && printfStrings2[0] == L"17" &&
                                    src.find(L"seventeen") != std::wstring::npos)
                                    {
                                    return;
                                    }
                                if (printfStrings2.size() == 1 && printfStrings2[0] == L"18" &&
                                    src.find(L"eighteen") != std::wstring::npos)
                                    {
                                    return;
                                    }
                                if (printfStrings2.size() == 1 && printfStrings2[0] == L"19" &&
                                    src.find(L"nineteen") != std::wstring::npos)
                                    {
                                    return;
                                    }
                                if (printfStrings2.size() == 1 && printfStrings2[0] == L"20" &&
                                    src.find(L"twent") != std::wstring::npos)
                                    {
                                    return;
                                    }
                                catEntry.second.m_issues.emplace_back(
                                    translation_issue::number_issue,
                                    _WXTRANS_WSTR(L"Number differences: '") +
                                        unrollStrings(printfStrings1) + _WXTRANS_WSTR(L"' vs. '") +
                                        unrollStrings(printfStrings2) + L"'");
                                }
                            }
                        }
                };

                reviewNumbers(catEntry.second.m_source, catEntry.second.m_translation);
                reviewNumbers(catEntry.second.m_source_plural,
                              catEntry.second.m_translation_plural);
                }

            if (static_cast<bool>(m_review_styles & check_consistency))
                {
                const auto reviewConsistency =
                    [&catEntry, &srcResults, &transResults](auto src, auto trans)
                {
                    if (!src.empty() && !trans.empty())
                        {
                        const wchar_t lastSrcChar{ src.back() };
                        const wchar_t lastTransChar{ trans.back() };

                        const bool srcIsStop{ i18n_string_util::is_period(lastSrcChar) ||
                                              i18n_string_util::is_exclamation(lastSrcChar) ||
                                              i18n_string_util::is_question(lastSrcChar) };
                        const bool transIsStop{ i18n_string_util::is_period(lastTransChar) ||
                                                i18n_string_util::is_exclamation(lastTransChar) ||
                                                i18n_string_util::is_question(lastTransChar) };

                        if ((std::iswspace(lastSrcChar) && !std::iswspace(lastTransChar)) ||
                            (!std::iswspace(lastSrcChar) && std::iswspace(lastTransChar)) ||
                            // note that it is allowable for source to not have full stop, but for
                            // translation too
                            (srcIsStop && !transIsStop) ||
                            // mismatching colons
                            (i18n_string_util::is_colon(lastSrcChar) &&
                             !i18n_string_util::is_colon(lastTransChar)) ||
                            (i18n_string_util::is_colon(lastTransChar) &&
                             !i18n_string_util::is_colon(lastSrcChar)))
                            {
                            // if source is an exclamation and the translation is not,
                            // then that is OK
                            if (!(i18n_string_util::is_exclamation(lastSrcChar) && !transIsStop) &&
                                // translation ending with ')' is OK also if source has a full stop
                                !(srcIsStop &&
                                  i18n_string_util::is_close_parenthesis(lastTransChar)))
                                {
                                catEntry.second.m_issues.emplace_back(
                                    translation_issue::consistency_issue,
                                    _WXTRANS_WSTR(L"Different ending punctuation."));
                                }
                            }
                        else if (std::iswupper(src.front()) && std::iswlower(trans.front()))
                            {
                            catEntry.second.m_issues.emplace_back(
                                translation_issue::consistency_issue,
                                _WXTRANS_WSTR(L"Different casing of first word."));
                            }

                        // see if the number of pipe or tabs (not literal, but embedded 't'
                        // characters) match
                        static const std::wregex embeddedTabs{ LR"(\\t|\|)" };
                        srcResults = load_matches(src, embeddedTabs);
                        transResults = load_matches(trans, embeddedTabs);
                        if (srcResults.size() != transResults.size())
                            {
                            catEntry.second.m_issues.emplace_back(
                                translation_issue::consistency_issue,
                                _WXTRANS_WSTR(L"Different number of pipe or tab characters."));
                            }

                        // check for names that shouldn't be translated
                        for (const auto& unsafeWord : get_untranslatable_names())
                            {
                            size_t stcInstanceCount{ 0 };
                            size_t wordIndex{ 0 };
                            while (wordIndex != std::wstring::npos)
                                {
                                wordIndex =
                                    string_util::find_whole_word(src, unsafeWord, wordIndex);
                                if (wordIndex == std::wstring::npos)
                                    {
                                    break;
                                    }
                                ++stcInstanceCount;
                                wordIndex += unsafeWord.length();
                                }
                            size_t transInstanceCount{ 0 };
                            wordIndex = 0;
                            while (wordIndex != std::wstring::npos)
                                {
                                wordIndex =
                                    string_util::find_whole_word(trans, unsafeWord, wordIndex);
                                if (wordIndex == std::wstring::npos)
                                    {
                                    break;
                                    }
                                ++transInstanceCount;
                                wordIndex += unsafeWord.length();
                                }
                            if (stcInstanceCount > transInstanceCount)
                                {
                                catEntry.second.m_issues.emplace_back(
                                    translation_issue::consistency_issue,
                                    L"'" + unsafeWord + L"'" +
                                        _WXTRANS_WSTR(L" appears more times in the source "
                                                      "string than in the translation."));
                                }
                            }
                        }
                };

                reviewConsistency(catEntry.second.m_source, catEntry.second.m_translation);
                reviewConsistency(catEntry.second.m_source_plural,
                                  catEntry.second.m_translation_plural);
                }
            }
        }
    } // namespace i18n_check
