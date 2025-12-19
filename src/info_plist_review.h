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

/** @addtogroup Internationalization
    @brief i18n classes.
@{*/

#ifndef INFO_PLIST_FILE_REVIEW_H
#define INFO_PLIST_FILE_REVIEW_H

#include "i18n_review.h"
#include <vector>

/// @brief Class for reviewing a macOS Info.plist file.
namespace i18n_check
    {
    /** @brief Class to extract and review info from a macOS Info.plist file.*/
    class info_plist_file_review : public i18n_review
        {
      public:
        /// @brief Constructor.
        /// @param verbose @c true to include verbose warnings.
        explicit info_plist_file_review(const bool verbose) : i18n_review(verbose) {}

        /** @brief Main interface for extracting info from an Info.plist.
            @param infoFileText The Info.plist's text to analyze.
            @param fileName The (optional) name of file being analyzed.*/
        void operator()(const std::wstring_view infoFileText,
                        const std::filesystem::path& fileName) final;

        /// @returns Files missing `CFBundleLocalizations` values.
        [[nodiscard]]
        const std::vector<string_info>& get_missing_localization_bundles() const noexcept
            {
            return m_noLocalizationBundles;
            }

        /// @brief Clears the results.
        void clear_results() final
            {
            i18n_review::clear_results();
            m_noLocalizationBundles.clear();
            }

      private:
        std::vector<string_info> m_noLocalizationBundles;
        };
    } // namespace i18n_check

/** @}*/

#endif // INFO_PLIST_FILE_REVIEW_H
