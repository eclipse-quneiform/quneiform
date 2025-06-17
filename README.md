<img src="app-logo.svg" width="150" />

*Eclipse Quneiform*®
=============================

<a href="docs/manual/book/quneiform.pdf"><img alt="Manual" src="docs/manual/images/cover-thumnail.png" width="200" align="right"/></a>

Internationalization & localization analysis system for C++ code, *gettext* catalogs, and Windows&reg; RC files.

`Eclipse Quneiform` is a command-line utility and graphical user interface that scans source and resource files to check for
various i18n and l10n issues.

Additionally, the GUI version provides pseudo-translation support for
*gettext* catalogs (\*.po files).

Refer to the [manual](docs/manual/book/quneiform.pdf) for full documentation.

## File Support

`Eclipse Quneiform` supports static analysis for the following:

- C code
- C++ code ('98 and modern C++)
- Windows resource files (\*.rc)
- macOS application Info.plist files

Static analysis and pseudo-translation are available for:

- GNU *gettext* translation files (\*.po)

Additionally, it offers specialized support for the following frameworks:

- wxWidgets
- Qt
- KDE
- GTK+
- Win32
- MFC

¹ Strings are considered translatable if inside of [gettext](https://www.gnu.org/software/gettext/),
[wxWidgets](https://www.wxwidgets.org), [Qt](https://www.qt.io), or
[KDE](https://develop.kde.org/docs/plasma/widget/translations-i18n) (ki18n) i18n functions.
This includes functions and macros such as `gettext()`, `_()`, `tr()`, `translate()`,
`QT_TR_NOOP()`, `wxTRANSLATE()`, `i18n()`, etc.<br />
² Variables are determined to be ID variables if they are integral types with the whole word "ID" in their name.

Refer [here](Example.md) for example usage.

## Static Analysis

![](docs/manual/images/options-dialog-source.png)

The command line and GUI versions provide the following checks:

- Strings exposed for translation¹ that probably should not be. This includes (but not limited to):
  - Filenames
  - Strings only containing `printf()` commands
  - Numbers
  - Regular expressions
  - Strings inside of debug functions
  - Formulas
  - Code (used for code generators)
  - Strings that contain URLs, email addresses, or phone numbers
- Strings not available for translation that probably should be.
- Localizable strings being concatenated at runtime.
- Use of deprecated text macros (e.g., `wxT()` in wxWidgets, `_T()` in Win32).
- Use of deprecated string functions (e.g., `_tcsncpy()` in Win32).
- Suspect usage of i18n functions.
- Strings with malformed syntax (e.g., malformed HTML tags).
- Files that contain extended ASCII characters, but are not UTF-8 encoded.<br />
  (It is recommended that files be UTF-8 encoded for portability between compilers.)
- Strings that contain extended ASCII characters that are not encoded.
  ("Danke schön" instead of "Danke sch\U000000F6n".)<br />
  Encoding extended ASCII characters is recommended for
  best portability between compilers.
- UTF-8 encoded files which start with a BOM/UTF-8 signature.<br />
  It is recommended to save without the file signature for best compiler portability.
- `printf()`-like functions being used to just format an integer to a string.<br />
  It is recommended to use `std::to_string()` to do this instead.
- Pluralization and pronoun issues.
- Usage of half-width characters.
- Literal, localizable strings being used in search and comparison functions.
- `printf()` command mismatches between source and translation strings.
- Accelerator mismatches between source and translation strings.
- Number mismatches between source and translation strings.
- Complicated strings that should have translator comments added.
- Font issues in Windows resource files:
  - Dialogs not using "MS Shell Dlg" or "MS Shell Dlg 2."
  - Dialogs with non-standard font sizes.
- Info.plist files not including any `CFBundleLocalizations` keys.

Code formatting and other issues can also be checked for, such as:

- Trailing spaces at the end of a line.
- Tabs (instead of spaces).
- Lines longer than 120 characters.
- Spaces missing between "//" and their comments.
- ID variable² assignment issues:
  - The same value being assigned to different ID variables in the same source file
    (e.g., "wxID_HIGHEST + 1" being assigned to two menu ID constants).
  - Hard-coded numbers being assigned to ID variables.
  - Out-of-range values being assigned to MFC IDs.

## Pseudo-translation

(available in the GUI version)

![](docs/manual/images/options-dialog-resources.png)

Pseudo-translation includes features such as:
- Multiple methods for character replacement (e.g., replacing characters with accented variations or upper casing them).
- Increasing the width of the translations. This is useful for ensuring that strings are not being truncated at runtime.
- Wrapping the translations in braces. This is useful for ensuring that strings are not being pieced together at runtime.
- Appending a unique ID to each translation. This is useful for finding where a translation is being loaded from.

When pseudo-translating, a copy of all *gettext* \*.po and \*.pot files will be created and have their translations filled
with mutations of the respective source strings. These files (which will have a "pseudo_" prefix) can then be compiled into \*.mo files and
loaded by your application for integration testing.

[![Linux Build](https://github.com/eclipse-quneiform/quneiform/actions/workflows/linux-build.yml/badge.svg)](https://github.com/eclipse-quneiform/quneiform/actions/workflows/linux-build.yml)
[![macOS build](https://github.com/eclipse-quneiform/quneiform/actions/workflows/macOS%20build.yml/badge.svg)](https://github.com/eclipse-quneiform/quneiform/actions/workflows/macOS%20build.yml)
[![Windows build](https://github.com/eclipse-quneiform/quneiform/actions/workflows/msw-build.yml/badge.svg)](https://github.com/eclipse-quneiform/quneiform/actions/workflows/msw-build.yml)

[![unit-tests](https://github.com/eclipse-quneiform/quneiform/actions/workflows/unit-tests.yml/badge.svg)](https://github.com/eclipse-quneiform/quneiform/actions/workflows/unit-tests.yml)
[![macOS unit tests](https://github.com/eclipse-quneiform/quneiform/actions/workflows/macos-unit-test.yml/badge.svg)](https://github.com/eclipse-quneiform/quneiform/actions/workflows/macos-unit-test.yml)
[![Windows unit tests](https://github.com/eclipse-quneiform/quneiform/actions/workflows/msw-unit-tests.yml/badge.svg)](https://github.com/eclipse-quneiform/quneiform/actions/workflows/msw-unit-tests.yml)

[![cppcheck](https://github.com/eclipse-quneiform/quneiform/actions/workflows/cppcheck.yml/badge.svg)](https://github.com/eclipse-quneiform/quneiform/actions/workflows/cppcheck.yml)
[![CodeQL](https://github.com/eclipse-quneiform/quneiform/actions/workflows/codeql.yml/badge.svg)](https://github.com/eclipse-quneiform/quneiform/actions/workflows/codeql.yml)
[![Microsoft C++ Code Analysis](https://github.com/eclipse-quneiform/quneiform/actions/workflows/msvc.yml/badge.svg)](https://github.com/eclipse-quneiform/quneiform/actions/workflows/msvc.yml)

[![doxygen](https://github.com/eclipse-quneiform/quneiform/actions/workflows/doxygen.yml/badge.svg)](https://github.com/eclipse-quneiform/quneiform/actions/workflows/doxygen.yml)
[![Check Spelling](https://github.com/eclipse-quneiform/quneiform/actions/workflows/spell-check.yml/badge.svg)](https://github.com/eclipse-quneiform/quneiform/actions/workflows/spell-check.yml)
[![Quneiform](https://github.com/eclipse-quneiform/quneiform/actions/workflows/i18n-check.yml/badge.svg)](https://github.com/eclipse-quneiform/quneiform/actions/workflows/i18n-check.yml)

# Usage

`Eclipse Quneiform` accepts the following arguments:

```shellscript
[input]: The folder to analyze.

--enable: Which checks to perform. Can be any combination of:
  allI18N:                  Perform all internationalization checks (the default).
  allL10N:                  Perform all localization checks (the default).
  allCodeFormatting:        Check all code formatting issues.
                            These are not enabled by default.
  suspectL10NString:        Check for translatable strings that shouldn't be
                            (e.g., numbers, keywords, printf() commands).
  suspectL10NUsage:         Check for translatable strings being used in internal contexts
                            (e.g., debugging functions).
  suspectI18NUsage:         Check for suspect usage of i18n functions.
  urlInL10NString:          Check for translatable strings that contain URLs, email addresses,
                            or phone numbers.
                            It is recommended to dynamically format these into the string so that
                            translators don't have to manage them.
  concatenatedStrings:      Check for strings that may be concatenated at runtime.
  literalL10NStringCompare: Check for literal, localizable strings being compared or searched for.
  L10NStringNeedsContext:   Check for possibly ambiguous strings that lack a translator comment.
  notL10NAvailable:         Check for strings not exposed for translation.
  deprecatedMacro:          Check for deprecated text macros (e.g., wxT()).
  nonUTF8File:              Check that files containing extended ASCII characters are UTF-8 encoded.
  UTF8FileWithBOM:          Check for UTF-8 encoded files which start with a BOM/UTF-8 signature.
                            It is recommended to save without the file signature for
                            best compiler portability.
  unencodedExtASCII:        Check for strings containing extended ASCII characters that are not encoded.
  printfSingleNumber:       Check for printf()-like functions being used to just format a number.
  dupValAssignedToIds:      Check for the same value being assigned to different ID variables.
  numberAssignedToId:       Check for ID variables being assigned a hard-coded number.
                            It may be preferred to assign framework-defined constants to IDs.
  malformedString:          Check for malformed syntax in strings (e.g., malformed HTML tags).
  fontIssue:                Check for font issues (e.g., Windows *.RC dialogs not using MS Shell Dlg
                            or using unusual font sizes).
  trailingSpaces:           Check for trailing spaces at the end of each line.
                            This is a code formatting check and is not enabled by default.
  tabs:                     Check for tabs.
                            (Spaces are recommended as tabs may appear differently between editors.)
                            This is a code formatting check and is not enabled by default.
  wideLine:                 Check for overly long lines.
                            This is a code formatting check and is not enabled by default.
  commentMissingSpace:      Check that there is a space at the start of a comment.
                            This is a code formatting check and is not enabled by default.
  printfMismatch:           Check for mismatching printf() commands between source and
                            translation strings.
  acceleratorMismatch:      Check for mismatching keyboard accelerators between source and
                            translation strings.
  transInconsistency:       Check for inconsistent trailing punctuation, spaces, newlines,
                            and untranslatable names.
  numberInconsistency:      Check for mismatching numbers between the source and target strings.
  lengthInconsistency:      Check for suspect lengths of translations compared to their source strings.
  excessiveNonL10NContent:  Check for translatable strings that contain large blocks on
                            non-translatable content.
  halfWidth:                Check for halfwidth Kanas, Hanguls, and punctuation in source and
                            target strings.
  multipartString:          Check for strings that appear to contain multiple parts that are
                            being sliced at runtime.
  pluralization:            Check for strings being used for both singular and plural that
                            should be use different variations.
  articleOrPronoun:         Check for strings with an article (e.g., the, a) in front of a
                            formatting placeholder.
                            Also checks for pronouns being used as individual strings.

--disable: Which checks to not perform. (Refer to options available above.)
           This will override any options passed to "--enable".

--log-l10n-allowed: Whether it is acceptable to pass translatable strings to 
                    logging functions. Setting this to false will emit warnings
                    when a translatable string is passed to functions such as
                    wxLogMessage or SDL_Log.
                    (Default is true.)

--punct-l10n-allowed: Whether it is acceptable for punctuation only strings to be
                      translatable. Setting this to true will suppress warnings about
                      strings such as " - " being available for localization.
                      (Default is false.)

--exceptions-l10n-required: Whether to verify that exception messages are available
                            for translation. Setting this to true will emit warnings
                            when untranslatable strings are passed to various exception
                            constructors or functions (e.g., AfxThrowOleDispatchException).
                            (Default is true.)

--min-l10n-wordcount: The minimum number of words that a string must have to be
                      considered translatable. Higher values for this will result in less
                      strings being classified as a notL10NAvailable warning.
                      (Default is 2.)

--fuzzy:              Whether to review fuzzy translations.
                      (Default is false.)

--cpp-version: The C++ standard that should be assumed when issuing deprecated macro warnings.
               (Default is 2014.)

-i,--ignore: Folders and files to ignore (can be used multiple times).

-o,--output: The output report path (tab-delimited or CSV format).
             (Can either be a full path, or a file name within the current working directory.)

-q,--quiet: Only print errors and the final output.

-v,--verbose: Perform additional checks and display debug information.

-h,--help: Print usage.
```

The following example will analyze the folder "c:\src\wxWidgets\src"
(but ignore the subfolders "expat" and "zlib"). It will only check for
suspect translatable strings, and then send the output to "results.txt"
in the current working directory. 

```shellscript
quneiform C:\src\wxWidgets\src -i expat -i zlib --enable=suspectL10NString -o results.txt
```

This example will only check for `suspectL10NUsage` and `suspectL10NString` and not show
any progress messages.

```shellscript
quneiform C:\src\wxWidgets\samples -q --enable=suspectL10NUsage,suspectL10NString
```

This example will ignore multiple folders (and files) and output the results to "WDVResults.txt."

```shellscript
quneiform C:\src\Wisteria-dataviz\src --ignore=import,wxsimplejson,wxStartPage,math,easyexif,debug,utfcpp,CRCpp,base/colors.cpp,base/colors.h -o WDVresults.txt
```

Refer [here](Example.md) for more examples.

# Building (command line tool)

`Eclipse Quneiform` can be configured and built with *CMake*.

On Unix:

```shellscript
cmake . -DCMAKE_BUILD_TYPE=Release
cmake --build . --target all -j $(nproc) --config Release
```

On Windows, "CMakeLists.txt" can be opened and built directly in Visual Studio.

After building, "quneiform" will be available in the "bin" folder.

# Building (GUI)

![](docs/manual/images/main-window.png)

[wxWidgets](https://github.com/wxWidgets/wxWidgets) 3.3 or higher is required for building the graphical user interface version.

Download [wxWidgets](https://github.com/wxWidgets/wxWidgets), placing it at the same folder level as this project.
After building wxWidgets, *Quneiform* can be configured and built with *CMake*.

On Unix:

```shellscript
git clone https://github.com/wxWidgets/wxWidgets.git --recurse-submodules
cd quneiform/gui
cmake . -DCMAKE_BUILD_TYPE=Release
cmake --build . --target all --config Release
```

On Windows with Visual Studio, build wxWidgets with the defaults, except `wxBUILD_SHARED` should be set to "OFF"
(and `MAKE_BUILD_TYPE` set to "Release" for release builds).

Open "gui/CMakeLists.txt" in Visual Studio, setting the *CMake* setting's configuration type to "Release" for a release build.

After building, "quneiform" will be available in the "bin" folder.
