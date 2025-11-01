/********************************************************************************
 * Copyright (c) 2021-2025 Blake Madden
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

#ifndef QUARTO_I18N_REVIEW_H
#define QUARTO_I18N_REVIEW_H

#include "i18n_review.h"
#include <map>
#include <set>
#include <span>
#include <string>
#include <utility>
#include <vector>

/// @brief Classes for checking source code for internationalization/localization issues.
namespace i18n_check
    {
    /** @brief Class to review i18n issues from Quarto documents.*/
    class quarto_review : public i18n_review
        {
      public:
        /// @brief Constructor.
        quarto_review() : i18n_review(false) {}

        /** @brief Main interface for reviewing Quarto content.
            @param srcText The Quarto text.
            @param fileName The (optional) name of source file being analyzed.*/
        void operator()(std::wstring_view srcText, const std::filesystem::path& fileName) final;

        /// @returns Lines that contain multiple sentences.
        [[nodiscard]]
        const std::vector<string_info>& get_multi_sentence_lines() const noexcept
            {
            return m_sentencesOnSameLine;
            }

        /// @returns Lines where a sentence is split into multiple lines.
        [[nodiscard]]
        const std::vector<string_info>& get_multi_line_sentences() const noexcept
            {
            return m_sentencesSplitOnDifferentLines;
            }
        
        /// @returns Malformed Quarto content.
        [[nodiscard]]
        const std::vector<string_info>& get_malformed_content() const noexcept
            {
            return m_malformedContent;
            }

        /// @brief Clears the results.
        void clear_results() final
            {
            i18n_review::clear_results();
            m_sentencesOnSameLine.clear();
            m_sentencesSplitOnDifferentLines.clear();
            m_malformedContent.clear();
            }

      private:
        std::vector<string_info> m_sentencesOnSameLine;
        std::vector<string_info> m_sentencesSplitOnDifferentLines;
        std::vector<string_info> m_malformedContent;
        };

    } // namespace i18n_check

/** @}*/

#endif // QUARTO_I18N_REVIEW_H
