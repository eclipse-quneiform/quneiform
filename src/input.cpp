/********************************************************************************
 * Copyright (c) 2021-2024 Blake Madden
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

#include "input.h"
#include "i18n_review.h"
#include <iostream>
#include <unordered_set>

namespace i18n_check
    {
    //------------------------------------------------------
    excluded_results get_paths_files_to_exclude(const std::filesystem::path& inputFolder,
                                                const std::vector<std::filesystem::path>& excluded)
        {
        excluded_results excResults;

        const auto combinePaths =
            [](const std::filesystem::path& input, const std::filesystem::path& excludedSubDir)
        {
            // combine paths, adding a dir separator between them
            if (std::filesystem::exists(input / excludedSubDir))
                {
                return input / excludedSubDir;
                }
            // ...or try combining without inserting a dir separator

            std::filesystem::path newPath{ input };
            newPath += excludedSubDir;
            return newPath;
        };

        for (const auto& excItem : excluded)
            {
            try
                {
                if (excItem.empty())
                    {
                    continue;
                    }
                if (std::filesystem::exists(excItem))
                    {
                    if (std::filesystem::is_directory(excItem))
                        {
                        excResults.m_excludedPaths.push_back(excItem);
                        const auto folderToRecurse{ excResults.m_excludedPaths.back() };
                        // add subdirectories
                        for (const auto& p :
                             std::filesystem::recursive_directory_iterator(folderToRecurse))
                            {
                            if (std::filesystem::exists(p) && p.is_directory())
                                {
                                excResults.m_excludedPaths.push_back(p.path());
                                }
                            }
                        }
                    else
                        {
                        excResults.m_excludedFiles.push_back(excItem);
                        }
                    }
                // if not a full path, just a subdirectory path
                else if (const auto relPath = combinePaths(inputFolder, excItem);
                         std::filesystem::exists(relPath))
                    {
                    if (std::filesystem::is_directory(relPath))
                        {
                        excResults.m_excludedPaths.push_back(relPath);
                        const auto folderToRecurse{ excResults.m_excludedPaths.back() };
                        // add subdirectories
                        for (const auto& p :
                             std::filesystem::recursive_directory_iterator(folderToRecurse))
                            {
                            if (std::filesystem::exists(p) && p.is_directory())
                                {
                                excResults.m_excludedPaths.push_back(p.path());
                                }
                            }
                        }
                    else
                        {
                        excResults.m_excludedFiles.push_back(relPath);
                        }
                    }
                else
                    {
                    continue;
                    }
                }
            catch (const std::exception& expt)
                {
                std::wcout << i18n_string_util::lazy_string_to_wstring(expt.what()) << L"\n";
                }
            }

        return excResults;
        }

    //------------------------------------------------------
    std::vector<std::filesystem::path>
    get_files_to_analyze(const std::filesystem::path& inputFolder,
                         const std::vector<std::filesystem::path>& excludedPaths,
                         const std::vector<std::filesystem::path>& excludedFiles)
        {
        std::vector<std::filesystem::path> filesToAnalyze;
        // wouldn't make sense to analyze anything in these folders
        static const std::unordered_set<std::wstring> defaultIgnoredDirs{
            L".vs",     L".git",   L".gitmodules", L".gitworktree", L".cache",
            L".clangd", L".cmake", L".gradle",     L".quarto",      L".github",
            L".vscode", L".idea",  L".svn"
        };

        std::vector<std::filesystem::path> excludedDirsAbs;
        excludedDirsAbs.reserve(excludedPaths.size());

        for (const auto& ePath : excludedPaths)
            {
            std::filesystem::path p(ePath, std::filesystem::path::format::native_format);
            if (p.is_relative())
                {
                p = inputFolder / p;
                }
            excludedDirsAbs.push_back(std::filesystem::weakly_canonical(p));
            }

        std::vector<std::filesystem::path> excludedFilesAbs;
        excludedFilesAbs.reserve(excludedFiles.size());

        for (const auto& eFile : excludedFiles)
            {
            std::filesystem::path p(eFile, std::filesystem::path::format::native_format);

            if (p.is_relative())
                {
                p = inputFolder / p;
                }

            excludedFilesAbs.push_back(std::filesystem::weakly_canonical(p));
            }

        if (std::filesystem::exists(inputFolder) && std::filesystem::is_regular_file(inputFolder))
            {
            filesToAnalyze.push_back(inputFolder);
            }
        else if (std::filesystem::exists(inputFolder) && std::filesystem::is_directory(inputFolder))
            {
            for (std::filesystem::recursive_directory_iterator
                     it(inputFolder, std::filesystem::directory_options::skip_permission_denied),
                 end;
                 it != end; ++it)
                {
                const std::filesystem::directory_entry& p = *it;

                // prune excluded directories immediately
                if (p.is_directory())
                    {
                    const std::wstring dirName = p.path().filename().wstring();
                    bool excludeDir = defaultIgnoredDirs.contains(dirName);

                    if (!excludeDir)
                        {
                        const std::filesystem::path curDir =
                            std::filesystem::weakly_canonical(p.path());

                        for (const auto& excDir : excludedDirsAbs)
                            {
                            if (curDir == excDir)
                                {
                                excludeDir = true;
                                break;
                                }
                            }
                        }

                    if (excludeDir)
                        {
                        // skip this directory entirely
                        it.disable_recursion_pending();
                        continue;
                        }
                    }
                const auto ext = p.path().extension();
                bool inExcludedPath{ false };
                try
                    {
                    if (p.is_regular_file())
                        {
                        const std::filesystem::path curFile =
                            std::filesystem::weakly_canonical(p.path());

                        for (const auto& excFile : excludedFilesAbs)
                            {
                            if (curFile == excFile)
                                {
                                inExcludedPath = true;
                                break;
                                }
                            }
                        }

                    if (p.is_regular_file() && !inExcludedPath &&
                        (ext == L".rc" || ext == L".plist" || ext == L".qmd" || ext == L".c" ||
                         ext == L".cs" || ext == L".cpp" || ext == L".h" || ext == L".hpp" ||
                         ext == L".po" || ext == L".pot") &&
                        // ignore CMake build files
                        p.path().filename() != L"CMakeCXXCompilerId.cpp" &&
                        p.path().filename() != L"CMakeCCompilerId.c" &&
                        // main catch2 files
                        p.path().filename() != L"catch.hpp" &&
                        p.path().filename() != L"catch_amalgamated.cpp" &&
                        p.path().filename() != L"catch_amalgamated.hpp" &&
                        // ignore pseudo-translated message catalogs
                        !p.path().filename().wstring().starts_with(L"pseudo_"))
                        {
                        filesToAnalyze.push_back(p.path());
                        }
                    }
                catch (const std::exception& expt)
                    {
                    std::wcout << i18n_string_util::lazy_string_to_wstring(expt.what()) << L"\n";
                    }
                }
            }

        return filesToAnalyze;
        }
    } // namespace i18n_check
