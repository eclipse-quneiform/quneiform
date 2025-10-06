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

#include "i18n_review.h"

namespace i18n_check
    {
    const std::wregex i18n_review::m_file_filter_regex{ LR"(([*][.][[:alnum:]\*]{1,5}[;]?)+$)" };

    const std::wregex i18n_review::m_halfwidth_range_regex{ LR"([\uFF61-\uFFDC]+)" };

    const std::wregex i18n_review::m_url_email_regex{
        LR"(((http|ftp)s?:\/\/)?(www\.)[-a-zA-Z0-9@:%._\+~#=]{1,256}\.[a-zA-Z0-9()]{1,6}\b([-a-zA-Z0-9()@:%_\+.~#?&//=]*))"
    };

    const std::wregex i18n_review::m_us_phone_number_regex{
        LR"((\+\d{1,2}\s)?\(?\d{3}\)?[\s.-]\d{3}[\s.-]\d{4})"
    };

    const std::wregex i18n_review::m_non_us_phone_number_regex{
        LR"((\+0?1\s)?\(?\d{3}\)?[\s.-]\d{3}[\s.-]\d{4})"
    };

    const std::wregex i18n_review::m_malformed_html_tag_bad_amp{ LR"(&amp;[a-zA-Z]{3,5};)" };

    const std::wregex i18n_review::m_malformed_html_tag{ LR"(&(nbsp|amp|quot)[^;])" };

    const std::wregex i18n_review::m_sql_code{
        LR"(.*(SELECT \*|CREATE TABLE|CREATE INDEX|COLLATE NOCASE|ALTER TABLE|DROP TABLE|COLLATE DATABASE_DEFAULT).*)",
        std::regex_constants::icase
    };

    // UINT MENU_ID_PRINT = 1'000;
    const std::wregex i18n_review::m_id_assignment_regex{
        LR"((int|uint32_t|uint64_t|INT|UINT|wxWindowID|#define)( |\t|const)*([a-zA-Z0-9_]*ID[a-zA-Z0-9_]*)[ \t]*[=\({]?[ \t\({]*([a-zA-Z0-9_ \+\-\'<>:\.]+){1}(.?))"
    };

    const std::wregex i18n_review::m_diagnostic_function_regex{
        LR"(([a-zA-Z0-9_]*|^)(ASSERT|VERIFY|PROFILE|CHECK)([a-zA-Z0-9_]*|$))"
    };

    const std::wregex i18n_review::m_plural_regex{ LR"([[:alnum:]]{2,}[(]s[)])" };
    const std::wregex i18n_review::m_open_function_signature_regex{ LR"([[:alnum:]]{2,}[(])" };
    const std::wregex i18n_review::m_html_tag_regex{ LR"(&[a-zA-Z]{2,5};.*)" };
    const std::wregex i18n_review::m_html_tag_unicode_regex{ LR"(&#[[:digit:]]{2,4};.*)" };
    // contains Western European, Czech, Polish, and Russian extended ASCII characters:
    // ŽžŸÀ-ÖØ-öø-ÿżźćńółęąśŻŹĆĄŚĘŁÓŃěščřžýáíéóúůďťňĎŇŤŠČŘŽÝÁÍÉÚŮĚÓАБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯабвгдеёжзийклмнопрстуфхцчшщъыьэюяІі
    // [:alpha:] supports all languages with MSVC, but GCC and Clang limit this to 7-bit ASCII (even
    // when calling setlocale), so we need to include other charsets explicitly here when trying to
    // include them in source strings.
    // quneiform-suppress-begin
    const std::wregex i18n_review::m_2letter_regex{
        LR"([[:alpha:]ŽžŸÀ-ÖØ-öø-ÿżźćńółęąśŻŹĆĄŚĘŁÓŃěščřžýáíéóúůďťňĎŇŤŠČŘŽÝÁÍÉÚŮĚÓАБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯабвгдеёжзийклмнопрстуфхцчшщъыьэюяІі]{2,})"
    };
    const std::wregex i18n_review::m_1word_regex{
        LR"((\b|\s|^)([[:alpha:]_ŽžŸÀ-ÖØ-öø-ÿżźćńółęąśŻŹĆĄŚĘŁÓŃěščřžýáíéóúůďťňĎŇŤŠČŘŽÝÁÍÉÚŮĚÓАБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯабвгдеёжзийклмнопрстуфхцчшщъыьэюяІі&'\.@]+)(2D|3D)?(\b|\s|$))"
    };
    const std::wregex i18n_review::m_keyboard_accelerator_regex{ LR"((^|[^&])&[^\s&])" };
    // quneiform-suppress-end
    const std::wregex i18n_review::m_hashtag_regex{ LR"(#[[:alnum:]]{2,})" };
    const std::wregex i18n_review::m_key_shortcut_regex{
        LR"((CTRL|SHIFT|CMD|ALT)([+](CTRL|SHIFT|CMD|ALT))*([+][[:alnum:]])+)",
        std::regex_constants::icase
    };
    const std::wregex i18n_review::m_function_signature_regex{
        LR"([[:alnum:]]{2,}[(][[:alnum:]]+(,[[:space:]]*[[:alnum:]]+)*[)])"
    };

    // HTML, but also includes some GTK formatting tags
    const std::wregex i18n_review::m_html_regex{
        LR"([^[:alnum:]<]*<(span|object|property|div|p|ul|ol|li|img|html|[?]xml|meta|body|table|tbody|tr|td|thead|head|title|a[[:space:]]|!--|/|!DOCTYPE|br|center|dd|em|dl|dt|tt|font|form|h[[:digit:]]|hr|main|map|pre|script).*)",
        std::regex_constants::icase
    };
    // <doc-val>Some text</doc-val>
    const std::wregex i18n_review::m_html_element_with_content_regex{
        LR"(<[a-zA-Z0-9_\-]+>[[:print:][:cntrl:]]*</[a-zA-Z0-9_\-]+>)", std::regex_constants::icase
    };

    // <s:complex name=\"{{GetFunctionName}}_{{GetParameterName}}_Array\">
    const std::wregex i18n_review::m_xml_element_regex{
        LR"(<\/?[a-zA-Z0-9_:'"\.\[\]\/\{\}\-\\=][a-zA-Z0-9_:'"\.\[\]\/\{\}\- \\=]+\/?>)",
        std::regex_constants::icase
    };

    // not really XML/HTMl ("<No Name Specified>")
    const std::wregex i18n_review::m_not_xml_element_regex{
        LR"(<\/?(([a-zA-Z0-9]){2,}\s+){1,}([a-zA-Z0-9]){2,}[[:punct:]]?\/?>)",
        std::regex_constants::icase
    };

    // <results> or <Results>, which should be OK to translate
    const std::wregex i18n_review::m_angle_braced_one_word_regex{ LR"(<[a-zA-Z][a-z\-]+>)" };

    // allowable HTML-like elements that are known
    const std::wregex i18n_review::m_html_known_elements_regex{
        LR"(<(span|object|property|div|p|ul|ol|li|img|html|xml|meta|body|table|tbody|tr|td|thead|head|title|br|center|dd|em|dl|dt|tt|font|form|hr|main|map|pre|script)>)"
    };

    // first capture group ensures that printf command is not proceeded by a negating '%'
    // second capture group is the actual printf command
    //
    // Note: a space specifier is OK for numbers, but not strings and pointers:
    //
    // "space: if the result of a signed conversion does not start with a sign character,
    //  or is empty, space is prepended to the result. It is ignored if + flag is present."
    //
    // Note: '<PRId64>' type specifiers are written as part of a printf string
    // (it's a macro outside of the string that the preprocessor maps to something else),
    // but PO files will embed these into the translations and source strings.
    const std::wregex i18n_review::m_printf_cpp_int_regex{
        LR"((^|\b|[%]{2}|[^%])([%]([[:digit:]]+[$])?([+]|[-] #0)?(([*]|[[:digit:]]+)*[.]?[[:digit:]]*)?(l)?(d|i|o|u|zu|c|C|e|E|x|X|l|I|I32|I64|<PRI(d|i|u|x)(32|64)>)))"
    };

    const std::wregex i18n_review::m_printf_cpp_float_regex{
        LR"((^|\b|[%]{2}|[^%])([%]([[:digit:]]+[$])?([+]|[-] #0)?(([*]|[[:digit:]]+)*[.]?[[:digit:]]*)?(l|L)?(f|F)))"
    };

    const std::wregex i18n_review::m_printf_cpp_string_regex{
        LR"((^|\b|[%]{2}|[^%])([%]([[:digit:]]+[$])?[-]?(([*]|[[:digit:]]+)*[.][[:digit:]]*)?s))"
    };

    const std::wregex i18n_review::m_printf_cpp_pointer_regex{
        LR"((^|\b|[%]{2}|[^%])[%]([%]([[:digit:]]+[$])?p))"
    };

    // %1, %L1, %n, %Ln
    const std::wregex i18n_review::m_positional_command_regex{ LR"([%](n|[L]?[0-9]+|Ln))" };

    // common font faces that we would usually ignore (client can add to this)
    std::set<string_util::case_insensitive_wstring> i18n_review::m_font_names = { // NOLINT
        L"Arial",
        L"Seaford",
        L"Skeena",
        L"Tenorite",
        L"Courier New",
        L"Garamond",
        L"Calibri",
        L"Gabriola",
        L".Helvetica Neue DeskInterface",
        L".Lucida Grande UI",
        L"Times New Roman",
        L"Georgia",
        L"Segoe UI",
        L"Segoe Script",
        L"Century Gothic",
        L"Century",
        L"Cascadia Mono",
        L"URW Bookman L",
        L"AR Berkley",
        L"Brush Script",
        L"Consolas",
        L"Century Schoolbook L",
        L"Lucida Grande",
        L"Helvetica Neue",
        L"Liberation Serif",
        L"Sans Serif",
        L"Luxi Serif",
        L"MS Sans Serif",
        L"Microsoft Sans Serif",
        L"Ms Shell Dlg",
        L"Ms Shell Dlg 2",
        L"Bitstream Vera Serif",
        L"URW Palladio L",
        L"URW Chancery L",
        L"Comic Sans MS",
        L"DejaVu Serif",
        L"DejaVu LGC Serif",
        L"Nimbus Sans L",
        L"URW Gothic L",
        L"Lucida Sans",
        L"Andale Mono",
        L"Luxi Sans",
        L"Liberation Sans",
        L"Bitstream Vera Sans",
        L"DejaVu LGC Sans",
        L"DejaVu Sans",
        L"Nimbus Mono L",
        L"Lucida Sans Typewriter",
        L"Luxi Mono",
        L"DejaVu Sans Mono",
        L"DejaVu LGC Sans Mono",
        L"Bitstream Vera Sans Mono",
        L"Liberation Mono",
        L"Franklin Gothic",
        L"Aptos",
        L"Grandview",
        L"Bierstadt",
        L"Tahoma",
        L"MingLiU",
        L"MS PGothic",
        L"Gulim",
        L"NSimSun"
    };

    // documents
    std::set<string_util::case_insensitive_wstring> i18n_review::m_file_extensions = { // NOLINT
        L"xml", L"html", L"htm", L"xhtml", L"rtf", L"doc", L"docx", L"dot", L"docm", L"txt", L"ppt",
        L"pptx", L"pdf", L"ps", L"odt", L"ott", L"odp", L"otp", L"pptm", L"md", L"xaml",
        // Visual Studio files
        L"sln", L"csproj", L"json", L"pbxproj", L"apk", L"tlb", L"ocx", L"pdb", L"tlh", L"hlp",
        L"msi", L"rc", L"vcxproj", L"resx", L"appx", L"vcproj",
        // macOS
        L"dmg", L"proj", L"xbuild", L"xmlns",
        // Database
        L"mdb", L"db",
        // Markdown files
        L"md", L"Rmd", L"qmd", L"yml",
        // help files
        L"hhc", L"hhk", L"hhp",
        // spreadsheets
        L"xls", L"xlsx", L"ods", L"csv",
        // image formats
        L"gif", L"jpg", L"jpeg", L"jpe", L"bmp", L"tiff", L"tif", L"png", L"tga", L"svg", L"xcf",
        L"ico", L"psd", L"hdr", L"pcx",
        // webpages
        L"asp", L"aspx", L"cfm", L"cfml", L"php", L"php3", L"php4", L"sgml", L"wmf", L"js",
        // style sheets
        L"css",
        // movies
        L"mov", L"qt", L"rv", L"rm", L"wmv", L"mpg", L"mpeg", L"mpe", L"avi",
        // music
        L"mp3", L"wav", L"wma", L"midi", L"ra", L"ram",
        // programs
        L"exe", L"swf", L"vbs",
        // source files
        L"cpp", L"h", L"c", L"idl", L"cs", L"hpp", L"po",
        // compressed files
        L"gzip", L"bz2"
    };

    std::set<std::wstring> i18n_review::m_untranslatable_exceptions = { L"PhD" };

    std::vector<std::wregex> i18n_review::m_variable_name_patterns_to_ignore;

    // variables whose CTORs take a string that should never be translated
    std::set<std::wstring> i18n_review::m_variable_types_to_ignore = {
        L"wxUxThemeHandle",
        L"wxRegKey",
        L"wxXmlNode",
        L"wxLoadedDLL",
        L"wxConfigPathChanger",
        L"wxWebViewEvent",
        L"wxFileSystemWatcherEvent",
        L"wxStdioPipe",
        L"wxCMD_LINE_CHARS_ALLOWED_BY_SHORT_OPTION",
        L"vmsWarningHandler",
        L"vmsErrorHandler",
        L"wxFFileOutputStream",
        L"wxFFile",
        L"wxFileName",
        L"QFileInfo",
        L"QDir",
        L"QTemporaryFile",
        L"wxColor",
        L"wxColour",
        L"wxFont",
        L"LOGFONTW",
        L"Font",
        L"SecretSchema",
        L"GtkTypeInfo",
        L"QKeySequence",
        L"QRegExp",
        L"wxRegEx",
        L"wregex",
        L"std::wregex",
        L"regex",
        L"std::regex",
        L"Regex",
        L"ifstream",
        L"ofstream",
        L"FileStream",
        L"StreamWriter",
        L"CultureInfo",
        L"TagHelperAttribute",
        L"QRegularExpression",
        L"wxDataViewRenderer",
        L"wxDataViewBitmapRenderer",
        L"wxDataViewDateRenderer",
        L"wxDataViewTextRenderer",
        L"wxDataViewIconTextRenderer",
        L"wxDataViewCustomRenderer",
        L"wxDataViewToggleRenderer",
        L"wxDataObjectSimple"
    };

    //--------------------------------------------------
    i18n_review::i18n_review(const bool verbose) : m_verbose(verbose)
        {
        m_deprecated_string_macros = {
            { L"wxT", _WXTRANS_WSTR(L"wxT() macro can be removed.") },
            { L"wxT_2", _WXTRANS_WSTR(L"wxT_2() macro can be removed.") },
            // wxWidgets can convert ANSI strings to double-byte, but Win32/MFC can't
            // and will need an 'L' prefixed to properly replace _T like macros.
            { L"_T",
              _WXTRANS_WSTR(L"_T() macro can be removed. Prefix with 'L' to make string wide.") },
            { L"__T",
              _WXTRANS_WSTR(L"__T() macro can be removed. Prefix with 'L' to make string wide.") },
            { L"TEXT",
              _WXTRANS_WSTR(L"TEXT() macro can be removed. Prefix with 'L' to make string wide.") },
            { L"_TEXT",
              _WXTRANS_WSTR(
                  L"_TEXT() macro can be removed. Prefix with 'L' to make string wide.") },
            { L"__TEXT",
              _WXTRANS_WSTR(
                  L"__TEXT() macro can be removed. Prefix with 'L' to make string wide.") },
            { L"_WIDE",
              _WXTRANS_WSTR(
                  L"_WIDE() macro can be removed. Prefix with 'L' to make string wide.") }
        };

        // Whole file needs to be scanned for these, as string variables can be passed to these
        // as well as hard-coded strings.
        m_deprecated_string_functions = {
            // Win32 TCHAR functions (which mapped between _MBCS and _UNICODE builds).
            // Nowadays, you should always be compiling as _UNICODE (i.e., UTF-16).
            { L"_tcsftime", _WXTRANS_WSTR(L"Use wcsftime instead of _tcsftime().") },
            { L"_tfopen", _WXTRANS_WSTR(L"Use fopen() instead of _tfopen().") },
            { L"__targv", _WXTRANS_WSTR(L"Use __wargv instead of __targv.") },
            { L"__tcserror", _WXTRANS_WSTR(L"Use __wcserror() instead of __tcserror().") },
            { L"__tcserror_s", _WXTRANS_WSTR(L"Use __wcserror_s() instead of __tcserror_s().") },
            { L"_tcscat", _WXTRANS_WSTR(L"Use std::wcscat() instead of _tcscat().") },
            { L"_tcscat_s", _WXTRANS_WSTR(L"Use wcscat_s() instead of _tcscat_s().") },
            { L"_tcschr", _WXTRANS_WSTR(L"Use std::wcschr() instead of _tcschr().") },
            { L"_tcsclen", _WXTRANS_WSTR(L"Use std::wcslen() instead of _tcsclen().") },
            { L"_tcscmp", _WXTRANS_WSTR(L"Use std::wcscmp() instead of _tcscmp().") },
            { L"_tcscnlen", _WXTRANS_WSTR(L"Use std::wcsnlen() instead of _tcscnlen().") },
            { L"_tcscoll", _WXTRANS_WSTR(L"Use std::wcscoll() instead of _tcscoll().") },
            { L"_tcscoll_l", _WXTRANS_WSTR(L"Use _wcscoll_l() instead of _tcscoll_l().") },
            { L"_tcscpy", _WXTRANS_WSTR(L"Use std::wcscpy() instead of _tcscpy().") },
            { L"_tcscpy_s", _WXTRANS_WSTR(L"Use wcscpy_s() instead of _tcscpy_s().") },
            { L"_tcscspn", _WXTRANS_WSTR(L"Use std::wcscspn() instead of _tcscspn().") },
            { L"_tcsdup", _WXTRANS_WSTR(L"Use _wcsdup() instead of _tcsdup().") },
            { L"_tcserror", _WXTRANS_WSTR(L"Use _wcserror() instead of _tcserror().") },
            { L"_tcserror_s", _WXTRANS_WSTR(L"Use _wcserror_s() instead of _tcserror_s().") },
            { L"_tcsicmp", _WXTRANS_WSTR(L"Use _wcsicmp() instead of _tcsicmp().") },
            { L"_tcsicmp_l", _WXTRANS_WSTR(L"Use _wcsicmp_l() instead of _tcsicmp_l().") },
            { L"_tcsicoll", _WXTRANS_WSTR(L"Use _wcsicoll() instead of _tcsicoll().") },
            { L"_tcsicoll_l", _WXTRANS_WSTR(L"Use _wcsicoll_l() instead of _tcsicoll_l().") },
            { L"_tcslen", _WXTRANS_WSTR(L"Use std::wcslen() instead of _tcslen().") },
            { L"_tcsncat", _WXTRANS_WSTR(L"Use std::wcsncat() instead of _tcsncat().") },
            { L"_tcsncat_l", _WXTRANS_WSTR(L"Use _wcsncat_l() instead of _tcsncat_l().") },
            { L"_tcsncat_s", _WXTRANS_WSTR(L"Use std::wcsncat_s() instead of _tcsncat_s().") },
            { L"_tcsncat_s_l", _WXTRANS_WSTR(L"Use _wcsncat_s_l() instead of _tcsncat_s_l().") },
            { L"_tcsnccmp", _WXTRANS_WSTR(L"Use std::wcsncmp() instead of _tcsnccmp().") },
            { L"_tcsnccoll", _WXTRANS_WSTR(L"Use _wcsncoll() instead of _tcsnccoll().") },
            { L"_tcsnccoll_l", _WXTRANS_WSTR(L"Use _wcsncoll_l() instead of _tcsnccoll_l().") },
            { L"_tcsncicmp", _WXTRANS_WSTR(L"Use _wcsnicmp() instead of _tcsncicmp().") },
            { L"_tcsncicmp_l", _WXTRANS_WSTR(L"Use _wcsnicmp_l() instead of _tcsncicmp_l().") },
            { L"_tcsncicoll", _WXTRANS_WSTR(L"Use _wcsnicoll() instead of _tcsncicoll().") },
            { L"_tcsncicoll_l", _WXTRANS_WSTR(L"Use _wcsnicoll_l() instead of _tcsncicoll_l().") },
            { L"_tcsncmp", _WXTRANS_WSTR(L"Use std::wcsncmp() instead of _tcsncmp().") },
            { L"_tcsncoll", _WXTRANS_WSTR(L"Use _wcsncoll() instead of _tcsncoll().") },
            { L"_tcsncoll_l", _WXTRANS_WSTR(L"Use _wcsncoll_l() instead of _tcsncoll_l().") },
            { L"_tcsncpy", _WXTRANS_WSTR(L"Use std::wcsncpy() instead of _tcsncpy().") },
            { L"_tcsncpy_l", _WXTRANS_WSTR(L"Use _wcsncpy_l() instead of _tcsncpy_l().") },
            { L"_tcsncpy_s", _WXTRANS_WSTR(L"Use wcsncpy_s() instead of _tcsncpy_s().") },
            { L"_tcsncpy_s_l", _WXTRANS_WSTR(L"Use _wcsncpy_s_l() instead of _tcsncpy_s_l().") },
            { L"_tcsnicmp", _WXTRANS_WSTR(L"Use _wcsnicmp() instead of _tcsnicmp().") },
            { L"_tcsnicmp_l", _WXTRANS_WSTR(L"Use _wcsnicmp_l() instead of _tcsnicmp_l().") },
            { L"_tcsnicoll", _WXTRANS_WSTR(L"Use _wcsnicoll() instead of _tcsnicoll().") },
            { L"_tcsnicoll_l", _WXTRANS_WSTR(L"Use _wcsnicoll_l() instead of _tcsnicoll_l().") },
            { L"_tcsnlen", _WXTRANS_WSTR(L"Use std::wcsnlen() instead of _tcsnlen().") },
            { L"_tcsnset", _WXTRANS_WSTR(L"Use _wcsnset() instead of _tcsnset().") },
            { L"_tcsnset_l", _WXTRANS_WSTR(L"Use _wcsnset_l() instead of _tcsnset_l().") },
            { L"_tcsnset_s", _WXTRANS_WSTR(L"Use _wcsnset_s() instead of _tcsnset_s().") },
            { L"_tcsnset_s_l", _WXTRANS_WSTR(L"Use _wcsnset_s_l() instead of _tcsnset_s_l().") },
            { L"_tcspbrk", _WXTRANS_WSTR(L"Use std::wcspbrk() instead of _tcspbrk().") },
            { L"_tcsrchr", _WXTRANS_WSTR(L"Use std::wcsrchr() instead of _tcsrchr().") },
            { L"_tcsrev", _WXTRANS_WSTR(L"Use _wcsrev() instead of _tcsrev().") },
            { L"_tcsset", _WXTRANS_WSTR(L"Use _wcsset() instead of _tcsset().") },
            { L"_tcsset_l", _WXTRANS_WSTR(L"Use _wcsset_l() instead of _tcsset_l().") },
            { L"_tcsset_s", _WXTRANS_WSTR(L"Use _wcsset_s() instead of _tcsset_s().") },
            { L"_tcsset_s_l", _WXTRANS_WSTR(L"Use _wcsset_s_l() instead of _tcsset_s_l().") },
            { L"_tcsspn", _WXTRANS_WSTR(L"Use std::wcsspn() instead of _tcsspn().") },
            { L"_tcsstr", _WXTRANS_WSTR(L"Use std::wcsstr() instead of _tcsstr().") },
            { L"_tcstod", _WXTRANS_WSTR(L"Use std::wcstod() instead of _tcstod().") },
            { L"_tcstof", _WXTRANS_WSTR(L"Use std::wcstof() instead of _tcstof().") },
            { L"_tcstoimax", _WXTRANS_WSTR(L"Use std::wcstoimax() instead of _tcstoimax().") },
            { L"_tcstok", _WXTRANS_WSTR(L"Use _wcstok() instead of _tcstok().") },
            { L"_tcstok_l", _WXTRANS_WSTR(L"Use _wcstok_l() instead of _tcstok_l().") },
            { L"_tcstok_s", _WXTRANS_WSTR(L"Use wcstok_s() instead of _tcstok_s().") },
            { L"_tcstok_s_l", _WXTRANS_WSTR(L"Use _wcstok_s_l() instead of _tcstok_s_l().") },
            { L"_tcstol", _WXTRANS_WSTR(L"Use std::wcstol() instead of _tcstol().") },
            { L"_tcstold", _WXTRANS_WSTR(L"Use std::wcstold() instead of _tcstold().") },
            { L"_tcstoll", _WXTRANS_WSTR(L"Use std::wcstoll() instead of _tcstoll().") },
            { L"_tcstoul", _WXTRANS_WSTR(L"Use std::wcstoul() instead of _tcstoul().") },
            { L"_tcstoull", _WXTRANS_WSTR(L"Use std::wcstoull() instead of _tcstoull().") },
            { L"_tcstoumax", _WXTRANS_WSTR(L"Use std::wcstoumax() instead of _tcstoumax().") },
            { L"_tcsxfrm", _WXTRANS_WSTR(L"Use std::wcsxfrm() instead of _tcsxfrm()") },
            { L"_tenviron", _WXTRANS_WSTR(L"Use _wenviron() instead of _tenviron().") },
            { L"_tmain", _WXTRANS_WSTR(L"Use wmain() instead of _tmain().") },
            { L"_tprintf", _WXTRANS_WSTR(L"Use wprintf() instead of _tprintf().") },
            { L"_tprintf_l", _WXTRANS_WSTR(L"Use _wprintf_l() instead of _tprintf_l().") },
            { L"_tprintf_s", _WXTRANS_WSTR(L"Use wprintf_s() instead of _tprintf_s().") },
            { L"_tWinMain", _WXTRANS_WSTR(L"Use wWinMain() instead of _tWinMain().") },
            { L"wsprintf", _WXTRANS_WSTR(L"Use std::swprintf() instead of wsprintf().") },
            { L"_stprintf", _WXTRANS_WSTR(L"Use std::swprintf() instead of _stprintf().") },
            { L"TCHAR", _WXTRANS_WSTR(L"Use wchar_t instead of TCHAR.") },
            { L"_TCHAR", _WXTRANS_WSTR(L"Use wchar_t instead of _TCHAR.") },
            { L"WCHAR", _WXTRANS_WSTR(L"Use wchar_t instead of WCHAR.") },
            { L"_MBCS",
              _WXTRANS_WSTR(L"Multibyte code should be replaced with Unicode ready code.") },
            { L"SBCS", _WXTRANS_WSTR(L"ANSI code should be replaced with Unicode ready code.") },
            { L"PTCHAR", _WXTRANS_WSTR(L"Use wchar_t* instead of PTCHAR.") },
            { L"LPTSTR", _WXTRANS_WSTR(L"Use LPWSTR (or wchar_t*) instead of LPTSTR.") },
            { L"LPCTSTR", _WXTRANS_WSTR(L"Use LPCWSTR (or const wchar_t*) instead of LPCTSTR.") },
            // wxWidgets
            { L"wxStrlen",
              _WXTRANS_WSTR(
                  L"Use std::wcslen() or (wrap in a std::wstring_view) instead of wxStrlen().") },
            { L"wxStrstr", _WXTRANS_WSTR(L"Use std::wcsstr() instead of wxStrstr().") },
            { L"wxStrchr", _WXTRANS_WSTR(L"Use std::wcschr() instead of wxStrchr().") },
            { L"wxStrdup", _WXTRANS_WSTR(L"Use std::wcsdup() instead of wxStrdup().") },
            { L"wxStrcpy",
              _WXTRANS_WSTR(L"Use std::wcscpy() instead of wxStrcpy() "
                            "(or prefer safer functions that process N number of characters).") },
            { L"wxStrncpy",
              _WXTRANS_WSTR(L"Use std::wcsncpy() (or wxStrlcpy) instead of wxStrncpy().") },
            { L"wxStrcat ",
              _WXTRANS_WSTR(L"Use std::wcscat() instead of wxStrcat() "
                            "(or prefer safer functions that process N number of characters).") },
            { L"wxStrncat", _WXTRANS_WSTR(L"Use std::wcsncat() instead of wxStrncat().") },
            { L"wxStrtok", _WXTRANS_WSTR(L"Use std::wcstok() instead of wxStrtok().") },
            { L"wxStrrchr", _WXTRANS_WSTR(L"Use std::wcsrchr() instead of wxStrrchr().") },
            { L"wxStrpbrk", _WXTRANS_WSTR(L"Use std::wcspbrk() instead of wxStrpbrk().") },
            { L"wxStrxfrm", _WXTRANS_WSTR(L"Use std::wcsxfrm() instead of wxStrxfrm.") },
            { L"wxIsEmpty",
              _WXTRANS_WSTR(L"Use wxString's empty() member instead of wxIsEmpty().") },
            { L"wxIsdigit", _WXTRANS_WSTR(L"Use std::iswdigit() instead of wxIsdigit().") },
            { L"wxIsalnum", _WXTRANS_WSTR(L"Use std::iswalnum() instead of wxIsalnum().") },
            { L"wxIsalpha", _WXTRANS_WSTR(L"Use std::iswalpha() instead of wxIsalpha().") },
            { L"wxIsctrl", _WXTRANS_WSTR(L"Use std::iswctrl() instead of wxIsctrl().") },
            { L"wxIspunct", _WXTRANS_WSTR(L"Use std::iswpunct() instead of wxIspunct().") },
            { L"wxIsspace", _WXTRANS_WSTR(L"Use std::iswpspace() instead of wxIsspace().") },
            { L"wxChar", _WXTRANS_WSTR(L"Use wchar_t instead of wxChar.") },
            { L"wxSChar", _WXTRANS_WSTR(L"Use wchar_t instead of wxSChar.") },
            { L"wxUChar", _WXTRANS_WSTR(L"Use wchar_t instead of wxUChar.") },
            { L"wxStrftime",
              _WXTRANS_WSTR(L"Use wxDateTime's formatting functions instead of wxStrftime().") },
            { L"wxStrtod", _WXTRANS_WSTR(L"Use wxString::ToDouble() instead of wxStrtod.") },
            { L"wxStrtol", _WXTRANS_WSTR(L"Use wxString::ToLong() instead of wxStrtol.") },
            { L"wxW64", _WXTRANS_WSTR(L"wxW64 is obsolete; remove it.") },
            { L"__WXFUNCTION__",
              _WXTRANS_WSTR(L"Use __func__ or __WXFUNCTION_SIG__ (requires wxWidgets 3.3) "
                            "instead of __WXFUNCTION__.") },
            { L"wxTrace",
              _WXTRANS_WSTR(L"Use one of the wxLogTrace() functions or one of the wxVLogTrace() "
                            "functions instead of wxTrace.") },
            { L"WXTRACE",
              _WXTRANS_WSTR(L"Use one of the wxLogTrace() functions or one of the wxVLogTrace() "
                            "functions instead of WXTRACE.") },
            { L"wxTraceLevel",
              _WXTRANS_WSTR(L"Use one of the wxLogTrace() functions or one of the wxVLogTrace() "
                            "functions instead of wxTraceLevel.") },
            { L"wxUnix2DosFilename",
              _WXTRANS_WSTR(
                  L"Construct a wxFileName with wxPATH_UNIX and then use "
                  "wxFileName::GetFullPath(wxPATH_DOS) instead of using wxUnix2DosFilename.") },
            { L"wxSplitPath",
              _WXTRANS_WSTR(
                  L"wxSplitPath is obsolete, please use wxFileName::SplitPath() instead.") },
            { L"wxConvCurrent", _WXTRANS_WSTR(L"Prefer using a wxConvAuto object instead.") },
            { L"mb_str()", _WXTRANS_WSTR(L"Relying on wxConvLibc can be unpredictable on some "
                                         "platforms. Prefer calling utf8_str() instead.") },
            { L"wxConvLibc", _WXTRANS_WSTR(L"Relying on wxConvLibc can be unpredictable on some "
                                           "platforms. Prefer calling utf8_str() instead.") }
        };

        if (verbose)
            {
            // not i18n related, just legacy wx functions that can be modernized
            if (m_min_cpp_version >= 2011)
                {
                m_deprecated_string_functions.insert(
                    { L"_STATIC_ASSERT",
                      _WXTRANS_WSTR(L"Use static_assert() instead of _STATIC_ASSERT().") });
                m_deprecated_string_functions.insert(
                    { L"wxMEMBER_DELETE",
                      _WXTRANS_WSTR(L"Use '= delete' instead of wxMEMBER_DELETE.") });
                m_deprecated_string_functions.insert(
                    { L"wxOVERRIDE",
                      _WXTRANS_WSTR(L"Use override or final instead of wxOVERRIDE.") });
                }
            if (m_min_cpp_version >= 2017)
                {
                m_deprecated_string_functions.insert(
                    { L"wxNODISCARD",
                      _WXTRANS_WSTR(L"Use [[nodiscard]] instead of wxNODISCARD.") });
                m_deprecated_string_functions.insert(
                    { L"WXSIZEOF", _WXTRANS_WSTR(L"Use std::size() instead of WXSIZEOF().") });
                m_deprecated_string_functions.insert(
                    { L"wxUnusedVar",
                      _WXTRANS_WSTR(L"Use [[maybe_unused]] instead of wxUnusedVar.") });
                m_deprecated_string_functions.insert(
                    { L"WXUNUSED", _WXTRANS_WSTR(L"Use [[maybe_unused]] instead of WXUNUSED().") });
                m_deprecated_string_functions.insert(
                    { L"Q_UNUSED", _WXTRANS_WSTR(L"Use [[maybe_unused]] instead of Q_UNUSED().") });
                }
            m_deprecated_string_functions.insert(
                { L"wxEXPAND",
                  _WXTRANS_WSTR(L"Call wxSizer::Add() with a wxSizerFlags object using Expand() "
                                "instead of wxEXPAND.") });
            m_deprecated_string_functions.insert(
                { L"wxGROW",
                  _WXTRANS_WSTR(L"Call wxSizer::Add() with a wxSizerFlags object using Expand() "
                                "instead of wxGROW.") });
            m_deprecated_string_functions.insert(
                { L"DECLARE_NO_COPY_CLASS",
                  _WXTRANS_WSTR(L"Delete the copy CTOR and assignment operator "
                                "instead of DECLARE_NO_COPY_CLASS.") });
            m_deprecated_string_functions.insert(
                { L"wxDECLARE_NO_COPY_CLASS",
                  _WXTRANS_WSTR(L"Delete the copy CTOR and assignment operator instead of "
                                "wxDECLARE_NO_COPY_CLASS.") });
            m_deprecated_string_functions.insert(
                { L"wxMin", _WXTRANS_WSTR(L"Use std::min() instead of wxMin().") });
            m_deprecated_string_functions.insert(
                { L"wxMax", _WXTRANS_WSTR(L"Use std::max() instead of wxMax().") });
            m_deprecated_string_functions.insert(
                { L"wxRound", _WXTRANS_WSTR(L"Use std::lround() instead of wxRound().") });
            m_deprecated_string_functions.insert(
                { L"wxIsNan", _WXTRANS_WSTR(L"Use std::isnan() instead of wxIsNan().") });
            m_deprecated_string_functions.insert(
                { L"wxNOEXCEPT", _WXTRANS_WSTR(L"Use noexcept instead of wxNOEXCEPT.") });
            m_deprecated_string_functions.insert(
                { L"__WXMAC__", _WXTRANS_WSTR(L"Use __WXOSX__ instead of __WXMAC__.") });
            }

        m_translatable_regexes = { std::wregex(LR"(Q[0-9](F|A)Y.*)") };

        m_untranslatable_regexes = {
            // nothing but numbers, punctuation, or control characters?
            std::wregex(LR"(([[:digit:][:space:][:punct:][:cntrl:]]|\\[rnt])+)"),
            // placeholder text
            std::wregex(LR"(Lorem ipsum.*)", std::regex_constants::icase),
            std::wregex(LR"(The quick brown fox.*)", std::regex_constants::icase),
            // webpage content type
            std::wregex(
                LR"([A-Za-z0-9\-]+/[A-Za-z0-9\-]+;[[:space:]]*[A-Za-z0-9\-]+=[A-Za-z0-9\-]+)"),
            // SQL code
            m_sql_code,
            std::wregex(LR"(^(INSERT INTO|DELETE ([*] )?FROM).*)", std::regex_constants::icase),
            std::wregex(LR"(^ORDER BY.*)"), // more strict
            std::wregex(LR"([(]*SELECT[[:space:]]+(COUNT|MIN|MAX|SUM|AVG|DISTINCT)[(].*)"),
            std::wregex(LR"([(]*SELECT[[:space:]]+[A-Z_0-9\.]+,.*)"), std::wregex(LR"(^DSN=.*)"),
            std::wregex(LR"(^Provider=(SQLOLEDB|Search).*)"),
            std::wregex(LR"(^Connection: Keep-Alive$)"), std::wregex(LR"(ODBC;DSN=.*)"),
            // C++ code
            std::wregex(LR"([a-zA-Z0-9]+\:\:[a-zA-Z0-9]+(\:\:[a-zA-Z0-9]+)?)"),
            // gettext formats
            std::wregex(LR"((possible\-)?(c|cpp|kde|qt)\-(plural\-)?format)"),
            // a regex expression
            std::wregex(LR"([(][?]i[)].*)"),
            // single file filter that just has a file extension as its "name"
            // PNG (*.png)
            // PNG (*.png)|*.png
            // TIFF (*.tif;*.tiff)|*.tif;*.tiff
            // special case for the word "bitmap" also, wouldn't normally translate that
            std::wregex(LR"(([A-Z]+|[bB]itmap) [(]([*][.][A-Za-z0-9]{1,7}[)]))"),
            std::wregex(
                LR"((([A-Z]+|[bB]itmap) [(]([*][.][A-Za-z0-9]{1,7})(;[*][.][A-Za-z0-9]{1,7})*[)][|]([*][.][A-Za-z0-9]{1,7})(;[*][.][A-Za-z0-9]{1,7})*[|]{0,2})+)"),
            // multiple file filters next to each other
            std::wregex(LR"(([*][.][A-Za-z0-9]{1,7}[;]?[[:space:]]*)+)"),
            // clang-tidy commands
            std::wregex(LR"(\-checks=.*)"),
            // generic measuring string (or regex expression)
            std::wregex(LR"([[:space:]]*(ABCDEFG|abcdefg|AEIOU|aeiou).*)"),
            // debug messages
            std::wregex(LR"(Assert(ion)? (f|F)ail.*)"), std::wregex(LR"(ASSERT *)"),
            // HTML doc start
            std::wregex(LR"(<!DOCTYPE html)"),
            // HTML entities
            std::wregex(LR"(&[#]?[xX]?[A-Za-z0-9]+;)"), std::wregex(LR"(<a href=.*)"),
            // HTML comment
            std::wregex(LR"(<![-]{2,}.*)"),
            // anchor
            std::wregex(LR"(#[a-zA-Z0-9\-]{3,})"),
            // CSS
            std::wregex(LR"(a[:](hover|link))", std::regex_constants::icase),
            std::wregex(LR"((width|height)[[:space:]]*\:[%]?[a-z]{2,4};)",
                        std::regex_constants::icase),
            std::wregex(
                LR"([\s\S]*(\{[[:space:]]*[a-zA-Z\-]+[[:space:]]*[:][[:space:]]*[0-9a-zA-Z\- \(\)\\;\:%#'",]+[[:space:]]*\})+[\s\S]*)"),
            std::wregex(
                LR"((margin[-](top|bottom|left|right)|text[-]indent)[:][[:space:]]*[[:alnum:]%]+;)"),
            // single hyphenated word that starts lowercase
            std::wregex(LR"([a-z][a-z0-9_]*\-[a-z0-9_]+(\-[a-z0-9_]+)*)"),
            // JS
            std::wregex(LR"(class[[:space:]]*=[[:space:]]*['"][A-Za-z0-9\- _]*['"])"),
            // An opening HTML element
            std::wregex(LR"(<(body|html|img|head|meta|style|span|p|tr|td))"),
            // PostScript element
            std::wregex(LR"(%%[a-zA-Z]+:.*)"),
            std::wregex(LR"((<< [\/()A-Za-z0-9[:space:]]*(\\n|[[:space:]])*)+)"),
            std::wregex(
                LR"((\/[A-Za-z0-9[:space:]]* \[[A-Za-z0-9[:space:]%]+\](\\n|[[:space:]])*)+)"),
            // C
            std::wregex(
                LR"(^#(include|define|if|ifdef|ifndef|endif|elif|pragma|warning)[[:space:]].*)"),
            // C++
            std::wregex(LR"([a-zA-Z0-9_]+([-][>]|::)[a-zA-Z0-9_]+([(][)];)?)"),
            std::wregex(LR"(#(define|pragma) .*)"),
            // command lines
            std::wregex(LR"(.*\b(rm|rmdir|mv) .*)"),
            std::wregex(LR"(\-\-[a-z0-9]+([\-\=][a-z0-9]+)*)"),
            std::wregex(LR"([-]D [A-Z_]{2,}[ =].*)"), std::wregex(LR"([-]dynamiclib .*)"),
            std::wregex(LR"([-]{2}[a-z]{2,}[ :].*)"),
            // registry keys
            std::wregex(LR"(SOFTWARE[\\]{1,2}(Policies|Microsoft|Classes).*)",
                        std::regex_constants::icase),
            std::wregex(LR"(SYSTEM[\\]{1,2}(CurrentControlSet).*)", std::regex_constants::icase),
            std::wregex(LR"(HKEY_.*)"),
            // web query
            std::wregex(LR"(search.aspx\?.*)"),
            // XML elements
            std::wregex(LR"(version[ ]?=\\"[0-9\.]+\\")"),
            std::wregex(LR"(<([A-Za-z])+([A-Za-z0-9_/\\\-\.'"=;:#[:space:]])+[>]?)"),
            std::wregex(LR"(xml[ ]*version[ ]*=[ ]*\\["'][0-9\.]+\\["'][>]?)"), // partial header
            std::wregex(LR"(<[\\]?\?xml[ a-zA-Z0-9=\\"'%\.\-]*[\?]?>.*)"),      // full header and
                                                                                // content after it
            std::wregex(
                LR"(<[A-Za-z]+[A-Za-z0-9_/\\\-\.'"=;:[:space:]]+>[[:space:][:digit:][:punct:]]*<[A-Za-z0-9_/\-.']*>)"),
            std::wregex(LR"(<[A-Za-z]+([A-Za-z0-9_\-\.]+[[:space:]]*){1,2}=[[:punct:]A-Za-z0-9]*)"),
            std::wregex(LR"(^[[:space:]]*xmlns(:[[:alnum:]]+)?=.*)"),
            std::wregex(LR"(^[[:space:]]*<soap(\.udp)?\:[[:alnum:]]+.*)"),
            std::wregex(LR"(^[[:space:]]*<port\b.*)"), std::wregex(LR"(ms-app(data|x))"),
            std::wregex(LR"(^\{\{.*)"),                      // soap syntax
            std::wregex(LR"(&[a-zA-Z0-9]+=[a-zA-Z0-9]+.*)"), // args passed to an URL
            std::wregex(LR"([cC]ontent-[tT]ype: [a-zA-Z]{3,}\/.*)"),
            std::wregex(LR"([cC]ontent-[dD]isposition: [a-zA-Z\-]{3,};.*)"),
            // <image x=%d y=\"%d\" width = '%dpx' height="%dpx"
            std::wregex(
                LR"(<[A-Za-z0-9_\-\.]+[[:space:]]*([A-Za-z0-9_\-\.]+[[:space:]]*=[[:space:]]*[\"'\\]{0,2}[a-zA-Z0-9\-]*[\"'\\]{0,2}[[:space:]]*)+)"),
            std::wregex(L"charset[[:space:]]*=.*", std::regex_constants::icase),
            // all 'X'es, spaces, and commas are usually a placeholder of some sort
            std::wregex(LR"((([\+\-]?[xX\.]+)[ ,]*)+)"),
            // placeholders
            std::wregex(LR"(asdfs.*)"),
            // program version string
            std::wregex(LR"([a-zA-Z\-]+ v(ersion)?[ ]?[0-9\.]+)"),
            // bash command (e.g., "lpstat -p") and system variables
            std::wregex(LR"([a-zA-Z]{3,} [\-][a-zA-Z]+)"), std::wregex(LR"(sys[$].*)"),
            // Pascal-case words (e.g., "GetValueFromUser");
            // surrounding punctuation is stripped first.
            std::wregex(LR"([[:punct:]]*[A-Z]+[a-z0-9]+([A-Z]+[a-z0-9]+)+[[:punct:]]*)"),
            // camel-case words (e.g., "getValueFromUser", "unencodedExtASCII");
            // surrounding punctuation is stripped first.
            std::wregex(LR"([[:punct:]]*[a-z]+[[:digit:]]*([A-Z]+[a-z0-9]*)+[[:punct:]]*)"),
            std::wregex(LR"([[:punct:]]*[a-z]+[[:digit:]]*_([A-Z]+[a-z0-9]*)+[[:punct:]]*)"),
            // reverse camel-case (e.g., "UTF8FileWithBOM")
            std::wregex(LR"([[:punct:]]*[A-Z]+[[:digit:]]*([a-z0-9]+[A-Z]+)+[[:punct:]]*)"),
            // formulas (e.g., ABS(-2.7), POW(-4, 2), =SUM(1; 2) )
            std::wregex(LR"((=)?[A-Za-z0-9_]{3,}[(]([RC0-9\-\.,;:\[\] ])*[)])"),
            // formulas (e.g., ComputeNumbers() )
            std::wregex(LR"([A-Za-z0-9_]{3,}[(][)])"),
            std::wregex(LR"([A-Za-z0-9_]{3,}[:]{2}[A-Za-z0-9_]{3,}[(][)])"),
            // equal sign followed by a single word is probably some sort of
            // config file tag or formula.
            std::wregex(LR"(=[A-Za-z0-9_]+)"),
            // character encodings
            std::wregex(
                LR"((utf[-]?[[:digit:]]+|Shift[-_]JIS|us-ascii|windows-[[:digit:]]{4}|KOI8-R|Big5|GB2312|iso-[[:digit:]]{4}-[[:digit:]]+))",
                std::regex_constants::icase),
            // wxWidgets constants
            std::wregex(LR"((wx|WX)[A-Z_0-9]{2,})"),
            // ODCTask --surrounding punctuation is stripped first
            std::wregex(LR"([[:punct:]]*[A-Z]{3,}[a-z_0-9]{2,}[[:punct:]]*)"),
            // snake case words
            // Note that "P_rinter" would be OK, as the '_' may be a hot-key accelerator
            std::wregex(LR"([_]*[a-z0-9]+(_[a-z0-9]+)+[_]*)"), // user_level_permission
            std::wregex(LR"([_]*[A-Z0-9]+(_[A-Z0-9]+)+[_]*)"), // __HIGH_SCORE__
            std::wregex(LR"([_]*[A-Z0-9][a-z0-9]+(_[A-Z0-9][a-z0-9]+)+[_]*)"), // Config_File_Path
            // CSS strings
            std::wregex(
                LR"(font-(style|weight|family|size|face-name|underline|point-size|variant)[[:space:]]*[:]?.*)",
                std::regex_constants::icase),
            std::wregex(
                LR"(border-(block|bottom|color|collapse|right|left|top|collapse|image|inline|start|end|width|style)[[:space:]]*[:]?.*)",
                std::regex_constants::icase),
            std::wregex(
                LR"(background-(clip|color|image|origin|position|repeat|size)[[:space:]]*[:]?.*)",
                std::regex_constants::icase),
            std::wregex(LR"(padding-(block|inline|left|right|top|bottom)[[:space:]]*[:]?.*)",
                        std::regex_constants::icase),
            std::wregex(LR"(page-break[[:space:]]*[:]?.*)", std::regex_constants::icase),
            std::wregex(LR"(line-(break|height|style|through)[[:space:]]*[:]?.*)",
                        std::regex_constants::icase),
            std::wregex(LR"((vertical|horizontal)-align[[:space:]]*[:]?.*)",
                        std::regex_constants::icase),
            std::wregex(LR"(flex-(basis|direction|flow|grow|shrink|wrap)[[:space:]]*[:]?.*)",
                        std::regex_constants::icase),
            std::wregex(
                LR"(text-(color|background|decoration|align|size|layout|transform|indent|justify|orientation|overflow|underline|shadow|emphasis)[[:space:]]*[:]?.*)",
                std::regex_constants::icase),
            std::wregex(LR"((background-)?color[[:space:]]*:.*)", std::regex_constants::icase),
            std::wregex(LR"(style[[:space:]]*=["']?.*)", std::regex_constants::icase),
            // local file paths & file names
            std::wregex(LR"((WINDIR|Win32|System32|Kernel32|/etc|/tmp))",
                        std::regex_constants::icase),
            std::wregex(LR"((so|dll|exe|dylib|jpg|bmp|png|gif|txt|doc))",
                        std::regex_constants::icase), // common file extension that might be missing
                                                      // the period
            std::wregex(LR"([.][a-zA-Z0-9]{1,5})"),   // file extension
            std::wregex(LR"([.]DS_Store)"),           // macOS file
            // file name (supports multiple extensions)
            std::wregex(LR"([\\/]?[[:alnum:]_~!@#$%&;',+={}().^\[\]\-]+([.][a-zA-Z0-9]{1,4})+)"),
            // ultra simple relative file path (e.g., "shaders/player1.vert")
            std::wregex(LR"(([[:alnum:]_-]+[\\/]){1,2}[[:alnum:]_-]+([.][a-zA-Z0-9]{1,4})+)"),
            std::wregex(LR"(\*[.][a-zA-Z0-9]{1,5})"), // wild card file extension
            // UNIX or web folder (needs at least 1 folder in path)
            std::wregex(LR"((\/{1,2}[[:alnum:]_~!@#$%&;',+={}().^\[\]\-]+){2,}/?)"),
            // Windows folder
            std::wregex(LR"([a-zA-Z][:]([\\]{1,2}[[:alnum:]_~!@#$%&;',+={}().^\[\]\-]*)+)"),
            std::wregex(LR"(\/?sys\$.*)"),
            // Debug message
            std::wregex(LR"(^DEBUG:[\s\S].*)"),
            // mail protocols
            std::wregex(LR"(^(RCPT TO|MAIL TO|MAIL FROM):.*)"), std::wregex(LR"(^(mailto)$)"),
            // GUIDs
            std::wregex(
                LR"(^(CLSID[\\]{1,2})?[\{]?[a-fA-F0-9]{8}\-[a-fA-F0-9]{4}\-[a-fA-F0-9]{4}\-[a-fA-F0-9]{4}\-[a-fA-F0-9]{12}[\}]?$)"),
            std::wregex(LR"(CLSID[\\]{1,2}.*)"),
            // encoding
            std::wregex(LR"(^(base[0-9]+|uuencode|quoted-printable)$)"),
            std::wregex(LR"(^(250\-AUTH)$)"),
            // MIME types
            std::wregex(LR"((application|text)\/(x\-)?[a-z\-]+)"),
            std::wregex(LR"(application\/(x\-)?[a-z\-]+\+[a-z\-]+)"),
            std::wregex(LR"(image\/(x\-)?[a-z\-]+)"), std::wregex(LR"(video\/(x\-)?[a-z\-]+)"),
            // MIME headers
            std::wregex(LR"(^MIME-Version:.*)"), std::wregex(LR"(^X-Priority:.*)"),
            std::wregex(
                LR"(^(application\/octet-stream|video\/([:alnum:]+)|audio\/([:alnum:]+)|image\/([:alnum:]+)|text\/(css|csv|plain|html|xml|rawdata|javascript))$)"),
            std::wregex(LR"(.*\bContent-Type:[[:space:]]*[[:alnum:]]+/[[:alnum:]]+;.*)"),
            std::wregex(LR"(.*\bContent-Transfer-Encoding:[[:space:]]*[[:alnum:]]+.*)"),
            // URL
            std::wregex(
                LR"(((http|ftp)s?:\/\/)?(www\.)[-a-zA-Z0-9@:%._\+~#=]{1,256}\.[a-zA-Z0-9()]{1,6}\b([-a-zA-Z0-9()@:%_\+.~#?&//=]*))"),
            // HTTP requests
            std::wregex(LR"(Sec[-]Fetch[-]Mode|User[-]Agent)"),
            // email address
            std::wregex(
                LR"(^[a-zA-Z0-9.!#$%&'*+/=?^_`{|}~-]+@[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?(?:\.[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?)*$)"),
            std::wregex(
                LR"(^[\w ]*<[a-zA-Z0-9.!#$%&'*+/=?^_`{|}~-]+@[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?(?:\.[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?)*>$)"),
            std::wregex(LR"(urn[:][a-zA-Z0-9]+.*)"),
            // Windows HTML clipboard data
            std::wregex(LR"(.*(End|Start)(HTML|Fragment)[:]?[[:digit:]]*.*)"),
            // printer commands (e.g., @PAGECOUNT@)
            std::wregex(LR"(@[A-Z0-9]+@)"),
            // [CMD]
            std::wregex(LR"(\[[A-Z0-9]+\])"),
            // Windows OS names
            std::wregex(
                LR"((Microsoft )?Windows (95|98|NT|ME|2000|Server|Vista|Longhorn|XP|[[:digit:]]{1,2}[.]?[[:digit:]]{0,2})[[:space:]]*[[:digit:]]{0,4}[[:space:]]*(R|SP)?[[:digit:]]{0,2})"),
            // products and standards
            std::wregex(LR"(UTF[-](8|16|32)(BE|LE)?)"),
            std::wregex(LR"(Pentium( (II|III|M|Xeon|Pro))?)"),
            std::wregex(LR"((Misra|MISRA) C( [0-9]+)?)"),
            std::wregex(LR"(Borland C\+\+ Builder( [0-9]+)?)"), std::wregex(LR"(Qt Creator)"),
            std::wregex(LR"((Microsoft )VS Code)"), std::wregex(LR"((Microsoft )?Visual Studio)"),
            std::wregex(LR"((Microsoft )?Visual C\+\+)"),
            std::wregex(LR"((Microsoft )?Visual Basic)"), std::wregex(LR"(GNU gdb debugger)"),
            std::wregex(LR"(Clang\-(Format|Tidy))", std::regex_constants::icase),
            std::wregex(LR"(GDB)"),
            // culture language tags
            std::wregex(LR"([a-z]{2,3}[\-_][A-Z]{2,3})"),
            // image formats
            std::wregex(LR"(TARGA|PNG|JPEG|JPG|BMP|GIF)")
        };

        // functions/macros that indicate that a string will be localizable
        m_localization_functions = {
            // GNU's gettext C/C++ functions
            L"_", L"gettext", L"dgettext", L"ngettext", L"dngettext", L"pgettext", L"dpgettext",
            L"npgettext", L"dnpgettext", L"dcgettext",
            // GNU's propername module
            L"proper_name", L"proper_name_utf8",
            // wxWidgets gettext wrapper functions
            L"wxPLURAL", L"wxGETTEXT_IN_CONTEXT", L"wxGETTEXT_IN_CONTEXT_PLURAL", L"wxTRANSLATE",
            L"wxTRANSLATE_IN_CONTEXT", L"wxGetTranslation",
            // Qt (note that NOOP functions actually do load something for translation,
            // just not in-place)
            L"tr", L"trUtf8", L"translate", L"QT_TR_NOOP", L"QT_TRANSLATE_NOOP",
            L"QApplication::translate", L"QApplication::tr", L"QApplication::trUtf8",
            // KDE (ki18n)
            L"i18n", L"i18np", L"i18ncp", L"i18nc", L"xi18n", L"xi18nc", L"ki18n", L"ki18np",
            L"ki18ncp", L"ki18nc",
            // our own functions
            L"_WXTRANS_WSTR"
        };

        // note that tr (in Qt) takes an optional disambiguation argument, but because its
        // optional we don't include it in this list
        m_localization_with_context_functions = { _DT(L"translate"),
                                                  L"i18nc",
                                                  L"i18ncp",
                                                  L"ki18ncp",
                                                  L"ki18nc",
                                                  L"QApplication::translate",
                                                  L"QCoreApplication::translate",
                                                  L"QT_TRANSLATE_NOOP",
                                                  L"wxTRANSLATE_IN_CONTEXT",
                                                  L"wxGETTEXT_IN_CONTEXT_PLURAL",
                                                  L"wxGETTEXT_IN_CONTEXT",
                                                  L"wxGetTranslation" };

        // functions that indicate that a string is explicitly marked to not be translatable
        m_non_localizable_functions = {
            L"_DT", L"DONTTRANSLATE",
            // these are not defined explicitly in gettext, but their documentation suggests
            // that you can add them as defines in your code and use them
            L"gettext_noop", L"N_"
        };

        // Constructors and macros that should be ignored
        // (when backtracing, these are skipped over, and the parser moves to the
        //  function/variable assignment to the left of these).
        m_ctors_to_ignore = {
            // Win32 text macros that should be skipped over
            L"_T", L"TEXT", L"_TEXT", L"__TEXT", L"_WIDE", L"W",
            // macOS
            L"CFSTR", L"CFStringRef",
            // similar macros from other libraries
            L"T",
            // wxWidgets
            L"wxT", L"wxT_2", L"wxS", L"wxString", L"wxBasicString", L"wxCFStringRef",
            L"wxASCII_STR", L"wxFile",
            // Qt
            L"QString", L"QLatin1String", L"QStringLiteral", L"setStyleSheet", L"QFile",
            // standard string objects
            L"basic_string", L"string", L"wstring", L"u8string", L"u16string", L"u32string",
            L"std::basic_string", L"std::string", L"std::wstring", L"std::u8string",
            L"std::u16string", L"std::u32string", L"std::pmr::basic_string", L"std::pmr::string",
            L"std::pmr::wstring", L"std::pmr::u8string", L"std::pmr::u16string",
            L"std::pmr::u32string", L"pmr::basic_string", L"pmr::string", L"pmr::wstring",
            L"pmr::u8string", L"pmr::u16string", L"pmr::u32string", L"std::ifstream",
            L"std::ofstream",
            // MFC, ATL, COM
            L"CString", L"_bstr_t", L"OLESTR", L"T2COLE", L"T2OLE", L"OLE2CT", L"OLE2T",
            L"CComBSTR", L"SysAllocString",
            // Java
            L"Locale",
            // formatting functions (not actually a CTOR) that should be skipped over
            L"wxString::Format", L"string.Format"
        };

        // Debugging, system call, and other internal functions that should never have
        // their string parameters translated. This can also include resource
        // loading functions that take a string ID.
        m_internal_functions = {
            // Java resource/key functions
            L"getBundle", L"getObject", L"handleGetObject", L"getString", L"getStringArray",
            L"containsKey",
            // attributes
            L"deprecated", L"nodiscard", L"_Pragma",
            // assert functions
            L"check_assertion", L"static_assert", L"assert", L"Assert", L"__android_log_assert",
            L"Assert.True",
            // wxWidgets functions and macros
            L"wxDEPRECATED_MSG", L"wxSTC_DEPRECATED_MACRO_VALUE", L"wxPG_DEPRECATED_MACRO_VALUE",
            L"GetExt", L"SetExt", L"XRCID", L"wxSystemOptions::GetOptionInt", L"WXTRACE",
            L"wxTrace", L"wxDATETIME_CHECK", L"wxASSERT", L"wxASSERT_MSG", L"wxASSERT_LEVEL_2",
            L"wxASSERT_LEVEL_2_MSG", L"wxOnAssert", L"wxCHECK", L"wxCHECK2", L"wxCHECK2_MSG",
            L"wxCHECK_MSG", L"wxCHECK_RET", L"wxCOMPILE_TIME_ASSERT", L"wxPROPERTY_FLAGS",
            L"wxPROPERTY", L"wxMISSING_IMPLEMENTATION", L"wxCOMPILE_TIME_ASSERT2", L"wxFAIL_MSG",
            L"wxFAILED_HRESULT_MSG", L"ExecCommand", L"CanExecCommand", L"IgnoreAppSubDir",
            L"put_designMode", L"SetExtension", L"wxSystemOptions::SetOption",
            L"wxFileName::CreateTempFileName", L"wxExecute", L"SetFailedWithLastError",
            L"wxIconHandler", L"wxBitmapHandler", L"OutputDumpLine", L"wxFileTypeInfo",
            L"TAG_HANDLER_BEGIN", L"FDEBUG", L"MDEBUG", L"wxVersionInfo", L"Platform::DebugPrintf",
            L"wxGetCommandOutput", L"SetKeyWords", L"AddDeveloper", L"AddDocWriter", L"AddArtist",
            L"AddTranslator", L"MarkerSetBackground", L"SetProperty", L"SetAppName",
            L"SetPrintToFile", L"GetAttribute", L"SetAuthor", L"GetPropertyAsSize",
            L"GetPropertyAsInteger", L"FoundSwitch",
            // Qt
            L"Q_ASSERT", L"Q_ASSERT_X", L"qSetMessagePattern", L"qmlRegisterUncreatableMetaObject",
            L"addShaderFromSourceCode", L"QStandardPaths::findExecutable", L"QDateTime::fromString",
            L"QFileInfo", L"qCDebug", L"qDebug", L"Q_MOC_INCLUDE", L"Q_CLASSINFO",
            L"setApplicationName", L"QApplication::setApplicationName",
            // l10n functions that take a string ID, not a literal string meant for translation
            L"QT_TRID_NOOP", L"QT_TRID_N_NOOP", L"qtTrId",
            // Catch2
            L"TEST_CASE", L"BENCHMARK", L"TEMPLATE_TEST_CASE", L"SECTION", L"DYNAMIC_SECTION",
            L"REQUIRE", L"REQUIRE_THROWS_WITH", L"REQUIRE_THAT", L"CHECK", L"CATCH_ENFORCE",
            L"INFO", L"SUCCEED", L"SCENARIO", L"GIVEN", L"AND_GIVEN", L"WHEN", L"THEN",
            L"SCENARIO_METHOD", L"WARN", L"TEST_CASE_METHOD", L"Catch::Clara::Arg",
            L"Catch::TestCaseInfo", L"GENERATE", L"CATCH_INTERNAL_ERROR", L"CATCH_ERROR",
            L"CATCH_MAKE_MSG", L"INTERNAL_CATCH_DYNAMIC_SECTION", L"CATCH_RUNTIME_ERROR",
            L"CATCH_INTERNAL_ERROR", L"CATCH_REQUIRE_THROWS_WIT", L"CATCH_SUCCEED", L"CATCH_INFO",
            L"CATCH_UNSCOPED_INFO", L"CATCH_WARN", L"CATCH_SECTION",
            // CppUnit
            L"CPPUNIT_ASSERT", L"CPPUNIT_ASSERT_EQUAL", L"CPPUNIT_ASSERT_DOUBLES_EQUAL",
            // Google Test
            L"EXPECT_STREQ", L"EXPECT_STRNE", L"EXPECT_STRCASEEQ", L"EXPECT_STRCASENE",
            L"EXPECT_TRUE", L"EXPECT_THAT", L"EXPECT_FALSE", L"EXPECT_EQ", L"EXPECT_NE",
            L"EXPECT_LT", L"EXPECT_LE", L"EXPECT_GT", L"EXPECT_GE", L"ASSERT_STREQ",
            L"ASSERT_STRNE", L"ASSERT_STRCASEEQ", L"ASSERT_STRCASENE", L"ASSERT_TRUE",
            L"ASSERT_THAT", L"ASSERT_FALSE", L"ASSERT_EQ", L"ASSERT_NE", L"ASSERT_LT", L"ASSERT_LE",
            L"ASSERT_GT", L"ASSERT_GE",
            // JUnit asserts
            L"assertEquals", L"assertNotEquals", L"assertArrayEquals", L"assertTrue", L"assertNull",
            L"assertNotNull", L"assertThat", L"assertNotEquals", L"assertNotSame", L"assertSame",
            L"assertThrows", L"fail",
            // other testing frameworks
            L"do_test", L"run_check", L"GNC_TEST_ADD_FUNC", L"GNC_TEST_ADD", L"g_test_message",
            L"check_binary_op", L"check_binary_op_equal", L"MockProvider",
            // MAME
            L"TEST_INSTRUCTION", L"ASIO_CHECK", L"ASIO_ERROR", L"ASIO_HANDLER_CREATION",
            L"ASMJIT_DEPRECATED",
            // low-level printf functions
            L"wprintf", L"printf", L"sprintf", L"snprintf", L"fprintf", L"wxSnprintf",
            // KDE
            L"getDocumentProperty", L"setDocumentProperty",
            // GTK
            L"gtk_assert_dialog_append_text_column", L"gtk_assert_dialog_add_button_to",
            L"gtk_assert_dialog_add_button", L"g_object_set_property", L"gdk_atom_intern",
            L"g_object_class_override_property", L"g_object_get", L"g_assert_cmpstr",
            L"gtk_rc_parse_string", L"g_param_spec_enum", L"g_error_new",
            L"g_dbus_method_invocation_return_error", L"GTKApplyCssStyle",
            // TCL
            L"Tcl_PkgRequire", L"Tcl_PkgRequireEx",
            // debugging functions from various open-source projects
            L"check_assertion", L"print_debug", L"DPRINTF", L"print_warning", L"perror",
            L"LogDebug", L"DebugMsg",
            // system functions that don't process user messages
            L"fopen", L"getenv", L"setenv", L"system", L"run", L"exec", L"execute", L"_tfopen",
            L"_wfopen", L"_fdopen", L"_sopen", L"_wsopen",
            // Unix calls
            L"popen", L"dlopen", L"dlsym", L"g_signal_connect", L"handle_system_error",
            // macOS calls
            L"CFBundleCopyResourceURL", L"sysctlbyname",
            // Windows/MFC/COM/ATL
            L"OutputDebugString", L"OutputDebugStringA", L"OutputDebugStringW", L"QueryValue",
            L"OutputFormattedDebugString", L"dbgprint", L"ASSERT", L"_ASSERTE", L"TRACE",
            L"ATLTRACE", L"TRACE0", L"ATLTRACE2", L"ATLENSURE", L"ATLASSERT", L"VERIFY",
            L"LoadLibrary", L"LoadLibraryEx", L"LoadModule", L"GetModuleHandle", L"QueryDWORDValue",
            L"GetTempFileName", L"QueryMultiStringValue", L"SetMultiStringValue",
            L"GetTempDirectory", L"FormatGmt", L"GetProgIDVersion", L"RegCreateKeyEx",
            L"RegCreateKey", L"GetProfileInt", L"WriteProfileInt", L"RegOpenKeyEx",
            L"RegOpenKeyExW", L"RegOpenKeyExA", L"QueryStringValue", L"lpVerb", L"Invoke",
            L"Invoke0", L"ShellExecute", L"GetProfileString", L"GetProcAddress",
            L"RegisterClipboardFormat", L"CreateIC", L"_makepath", L"_splitpath", L"VerQueryValue",
            L"CLSIDFromProgID", L"StgOpenStorage", L"InvokeN", L"CreateStream", L"DestroyElement",
            L"CreateStorage", L"OpenStream", L"CallMethod", L"PutProperty", L"GetProperty",
            L"HasProperty", L"SetRegistryKey", L"CreateDC", L"GetModuleFileName",
            L"GetModuleFileNameEx", L"GetProcessImageFileName", L"GetMappedFileName",
            L"GetDeviceDriverFileName", L"GetDeviceDriverBaseName", L"DECLARE_WND_SUPERCLASS",
            L"DECLARE_WND_CLASS_EX", L"DECLARE_WND_CLASS2", L"DECLARE_WND_CLASS", L"SHGetFileInfo",
            L"WFCTRACE", L"WFCTRACEVAL", L"WFCTRACEVARIANT", L"WFCLTRACEINIT", L"TRACE", L"TRACE0",
            L"TRACE1", L"TRACE2", L"TRACE3", L"TRACE4", L"TRACE5", L"TRACEERROR", L"_RPT0",
            L"_RPT1", L"_RPT2", L"_RPT3", L"_RPT4", L"_RPT5", L"_RPTF0", L"_RPTF1", L"_RPTF2",
            L"_RPTF3", L"_RPTF4", L"_RPTF5", L"_RPTW0", L"_RPTW1", L"_RPTW2", L"_RPTW3", L"_RPTW4",
            L"_RPTW5", L"_RPTFW0", L"_RPTFW1", L"_RPTFW2", L"_RPTFW3", L"_RPTFW4", L"_RPTFW5",
            L"OpenFromInitializationString", L"CreateADOCommand", L"ExecuteSql",
            L"com_interface_entry", L"uuid", L"idl_quote", L"threading", L"vi_progid", L"progid",
            L"CreatePointFont", L"CreateFont", L"FindWindow", L"RegisterServer",
            L"UnregisterServer", L"MIDL_INTERFACE", L"DECLSPEC_UUID", L"DebugPrintfW",
            L"DebugPrintfA", L"DebugPrintfW", L"DEBUGLOGRESULT", L"CreateTextFormat", L"DbgLog",
            L"GetPrivateProfileString", L"WritePrivateProfileString", L"RegDeleteKey",
            L"RegDeleteKeyEx", L"RegDeleteKeyValue", L"RegDeleteTree", L"RegLoadAppKey",
            L"RegOpenKey", L"RegRenameKey", L"RegSaveKey", L"RegSaveKeyEx", L"RegSetKeyValue",
            L"RegSetKeyValueEx", L"RegOpenKeyTransactedA", L"GetDataSource", L"TraceMsg",
            // .NET
            L"FindSystemTimeZoneById", L"CreateSpecificCulture", L"DebuggerDisplay", L"Debug.Fail",
            L"DeriveKey", L"Assert.Fail", L"Debug.Assert", L"Debug.Print", L"Debug.WriteLine",
            L"Debug.Write", L"Debug.WriteIf", L"Debug.WriteLineIf", L"Assert.Equal", L"DEBUGARG",
            L"noway_assert", L"DISASM_DUMP", L"NO_WAY", L"printfAlloc", L"Directory.GetFiles",
            L"Directory.EnumerateFiles", L"Utils.RunProcess", L"Utils.TryRunProcess",
            L"System.Diagnostics.Debug.Print",
            // zlib
            L"Tracev", L"Trace", L"Tracevv",
            // libpng
            L"png_debug", L"png_debug1", L"png_debug2", L"png_error", L"png_warning",
            L"png_chunk_warning", L"png_chunk_error",
            // Lua
            L"luaL_error", L"lua_pushstring", L"lua_setglobal",
            // more functions from various apps
            L"trace", L"ActionFormat", L"ErrorFormat", L"DEBUG", L"setParameters", L"getopt",
            L"_PrintEnter", L"_PrintExit", L"ERROR0", L"ERROR1", L"ERROR2", L"ERROR3",
            L"TraceString",
            // assembly calls
            L"asm"
        };

        m_log_functions = {
            // wxWidgets
            L"wxLogLastError", L"wxLogError", L"wxLogFatalError", L"wxLogGeneric", L"wxLogMessage",
            L"wxLogStatus", L"wxLogStatus", L"wxLogSysError", L"wxLogTrace", L"wxLogTrace",
            L"wxLogVerbose", L"wxLogWarning", L"wxLogDebug", L"wxLogApiError", L"LogTraceArray",
            L"DoLogRecord", L"DoLogText", L"DoLogTextAtLevel", L"LogRecord", L"DDELogError",
            L"LogTraceLargeArray",
            // Qt
            L"qDebug", L"qInfo", L"qWarning", L"qCritical", L"qFatal", L"qCDebug", L"qCInfo",
            L"qCWarning", L"qCCritical",
            // GLIB
            L"g_error", L"g_info", L"g_log", L"g_message", L"g_debug", L"g_message", L"g_warning",
            L"g_log_structured", L"g_critical",
            // SDL
            L"SDL_Log", L"SDL_LogCritical", L"SDL_LogDebug", L"SDL_LogError", L"SDL_LogInfo",
            L"SDL_LogMessage", L"SDL_LogMessageV", L"SDL_LogVerbose", L"SDL_LogWarn",
            // GnuCash
            L"PERR", L"PWARN", L"PINFO", L"ENTER", L"LEAVE",
            // actual console (or file) functions
            // (we will consider that most console apps are not localized, or if being used
            //  in a GUI then the message is meant for developers)
            L"printf", L"Console.WriteLine", L"dprintf", L"WriteLine", L"System.Console.WriteLine",
            L"_tprintf", L"wprintf",
            // .NET
            L"LoggerMessage", L"JITDUMP", L"LOG", L"LogSpew", L"LOG_HANDLE_OBJECT_CLASS",
            L"LOG_HANDLE_OBJECT", L"CorDisToolsLogERROR", L"LOG_ERROR", L"LOG_INFO", L"LogError",
            L"LogMessage", L"LogAsErrorException", L"LogVerbose", L"LogEvent", L"LogLine", L"Log",
            // TinyXML
            L"TIXML_LOG",
            // other programs
            L"log_message", L"outLog", L"Error", L"AppendLog", L"DBG_PRINT"
        };

        m_exceptions = {
            // std exceptions
            L"logic_error", L"std::logic_error", L"domain_error", L"std::domain_error",
            L"length_error", L"std::length_error", L"out_of_range", L"std::out_of_range",
            L"runtime_error", L"std::runtime_error", L"overflow_error", L"std::overflow_error",
            L"underflow_error", L"std::underflow_error", L"range_error", L"std::range_error",
            L"invalid_argument", L"std::invalid_argument", L"exception", L"std::exception",
            // MFC
            L"AfxThrowOleDispatchException", L"Win32Exception",
            // Qt
            L"QException",
            // .NET
            L"NotImplementedException", L"ArgumentException", L"InvalidOperationException",
            L"OptionException", L"NotSupportedException", L"Exception", L"BadImageFormatException",
            L"JsonException", L"ArgumentOutOfRangeException", L"ArgumentNullException",
            L"InvalidCastException"
        };

        // known strings to ignore
        m_known_internal_strings = { L"size-points", L"background-gdk", L"foreground-gdk",
                                     L"foreground-set", L"background-set", L"weight-set",
                                     L"style-set", L"underline-set", L"size-set", L"charset",
                                     L"xml", L"gdiplus", L"Direct2D", L"DirectX", L"localhost",
                                     L"32 bit", L"32-bit", L"64 bit", L"64-bit", L"NULL",
                                     // build types
                                     L"NDEBUG",
                                     // RTF font families
                                     L"fnil", L"fdecor", L"froman", L"fscript", L"fswiss",
                                     L"fmodern", L"ftech",
                                     // common UNIX names (Windows versions are handled by more
                                     // complex regex expressions elsewhere)
                                     L"UNIX", L"macOS", L"Apple Mac OS", L"Apple Mac OS X", L"OSX",
                                     L"Linux", L"FreeBSD", L"POSIX", L"NetBSD" };

        // keywords in the language that can appear in front of a string only
        m_keywords = { L"return", L"else", L"if", L"goto", L"new", L"delete", L"throw" };

        add_variable_name_pattern_to_ignore(
            std::wregex(LR"(^debug.*)", std::regex_constants::icase));
        add_variable_name_pattern_to_ignore(
            std::wregex(LR"(^stacktrace.*)", std::regex_constants::icase));
        add_variable_name_pattern_to_ignore(
            std::wregex(LR"(([[:alnum:]_\-])*xpm)", std::regex_constants::icase));
        add_variable_name_pattern_to_ignore(
            std::wregex(LR"(xpm([[:alnum:]_\-])*)", std::regex_constants::icase));
        add_variable_name_pattern_to_ignore(
            std::wregex(LR"((sql|db|database)(Table|Update|Query|Command|Upgrade)?[[:alnum:]_\-]*)",
                        std::regex_constants::icase));
        add_variable_name_pattern_to_ignore(std::wregex(LR"(log)"));
        add_variable_name_pattern_to_ignore(std::wregex(LR"([Cc]ommand(_)?[Ss]tring)"));
        add_variable_name_pattern_to_ignore(std::wregex(LR"(wxColourDialogNames)"));
        add_variable_name_pattern_to_ignore(std::wregex(LR"(wxColourTable)"));
        add_variable_name_pattern_to_ignore(std::wregex(LR"(QT_MESSAGE_PATTERN)"));
        // console objects
        add_variable_name_pattern_to_ignore(std::wregex(LR"((std::)?[w]?(cout|cerr|qout|qerr))"));
        }

    //--------------------------------------------------
    bool i18n_review::is_gettext_translator_comment(std::wstring_view commentBlock)
        {
        const std::wstring_view translatorKey{ _DT(L"TRANSLATORS:") };

        const size_t firstNonSpace = commentBlock.find_first_not_of(L" \t\n\r");
        if (firstNonSpace == std::wstring_view::npos)
            {
            return false;
            }
        commentBlock.remove_prefix(firstNonSpace);
        return (commentBlock.length() > translatorKey.length() &&
                commentBlock.compare(0, translatorKey.length(), translatorKey) == 0);
        }

    //--------------------------------------------------
    bool i18n_review::is_qt_translator_comment(std::wstring_view commentBlock)
        {
        return (!commentBlock.empty() && commentBlock[0] == L':');
        }

    //--------------------------------------------------
    std::pair<bool, size_t> i18n_review::is_block_suppressed(std::wstring_view commentBlock)
        {
        const std::wstring_view SUPPRESS_BEGIN{ L"quneiform-suppress-begin" };
        const std::wstring_view SUPPRESS_END{ L"quneiform-suppress-end" };

        const size_t firstNonSpace = commentBlock.find_first_not_of(L" \t\n\r");
        if (firstNonSpace == std::wstring_view::npos)
            {
            return std::make_pair(false, std::wstring_view::npos);
            }
        commentBlock.remove_prefix(firstNonSpace);
        if (commentBlock.length() > SUPPRESS_BEGIN.length() &&
            commentBlock.compare(0, SUPPRESS_BEGIN.length(), SUPPRESS_BEGIN) == 0)
            {
            const size_t endOfBlock = commentBlock.find(SUPPRESS_END);
            if (endOfBlock == std::wstring_view::npos)
                {
                return std::make_pair(false, std::wstring_view::npos);
                }
            return std::make_pair(true, firstNonSpace + endOfBlock + SUPPRESS_END.length());
            }
        else
            {
            return std::make_pair(false, std::wstring_view::npos);
            }
        }

    //--------------------------------------------------
    void i18n_review::review_strings([[maybe_unused]] analyze_callback_reset resetCallback,
                                     [[maybe_unused]] analyze_callback callback)
        {
        process_strings();

        std::wsmatch results;
        for (const auto& str : m_localizable_strings)
            {
            const auto [isunTranslatable, translatableContentLength] =
                is_untranslatable_string(str.m_string, false);
            if ((m_review_styles & check_l10n_contains_excessive_nonl10n_content) &&
                !isunTranslatable && str.m_string.length() > (translatableContentLength * 3) &&
                !str.m_usage.m_hasContext)
                {
                m_localizable_strings_with_unlocalizable_content.push_back(str);
                }
            if ((m_review_styles & check_l10n_strings) && str.m_string.length() > 0 &&
                isunTranslatable)
                {
                m_unsafe_localizable_strings.push_back(str);
                }
            if ((m_review_styles & check_multipart_strings) && is_string_multipart(str.m_string))
                {
                m_multipart_strings.push_back(str);
                }
            if ((m_review_styles & check_pluaralization) && is_string_resource_faux_plural(str))
                {
                m_faux_plural_strings.push_back(str);
                }
            if ((m_review_styles & check_articles_proceeding_placeholder) &&
                (is_string_article_issue(str.m_string) || is_string_pronoun(str.m_string)))
                {
                m_article_issue_strings.push_back(str);
                }
            if ((m_review_styles & check_l10n_contains_url) &&
                (std::regex_search(str.m_string, results, m_url_email_regex) ||
                 std::regex_search(str.m_string, results, m_us_phone_number_regex) ||
                 std::regex_search(str.m_string, results, m_non_us_phone_number_regex)))
                {
                m_localizable_strings_with_urls.push_back(str);
                }
            if ((m_review_styles & check_needing_context) && !str.m_usage.m_hasContext &&
                is_string_ambiguous(str.m_string))
                {
                m_localizable_strings_ambiguous_needing_context.push_back(str);
                }
            if ((m_review_styles & check_l10n_concatenated_strings) &&
                (has_surrounding_spaces(str.m_string) || is_concatenated_localizable_operator(str)))
                {
                m_localizable_strings_being_concatenated.push_back(str);
                }
            if ((m_review_styles & check_literal_l10n_string_comparison) &&
                has_comparison_operator(str))
                {
                m_literal_localizable_strings_being_compared.push_back(str);
                }
            if ((m_review_styles & check_halfwidth) &&
                !load_matches(str.m_string, m_halfwidth_range_regex).empty())
                {
                m_localizable_strings_with_halfwidths.push_back(str);
                }
            }

        if ((m_review_styles & check_l10n_concatenated_strings))
            {
            for (const auto& str : m_internal_strings)
                {
                // Hard coding a percent to a number at runtime should be avoided,
                // as some locales put the % at the front of the string.
                // Same with a dollar sign, where it is probably a currency string
                // being pieced together.
                if (is_concatenated_localizable_operator(str))
                    {
                    m_localizable_strings_being_concatenated.push_back(str);
                    }
                }
            }

        if (m_review_styles & check_malformed_strings)
            {
            const auto& classifyMalformedStrings = [this](const auto& strings)
            {
                for (const auto& str : strings)
                    {
                    if (std::regex_search(str.m_string, m_malformed_html_tag) ||
                        std::regex_search(str.m_string, m_malformed_html_tag_bad_amp))
                        {
                        m_malformed_strings.push_back(str);
                        }
                    }
            };

            classifyMalformedStrings(m_localizable_strings);
            classifyMalformedStrings(m_marked_as_non_localizable_strings);
            classifyMalformedStrings(m_internal_strings);
            classifyMalformedStrings(m_not_available_for_localization_strings);
            }

        if (m_review_styles & check_unencoded_ext_ascii)
            {
            const auto& classifyUnencodedStrings = [this](const auto& strings)
            {
                for (const auto& str : strings)
                    {
                    for (const auto& ch : str.m_string)
                        {
                        if (ch >= 128)
                            {
                            m_unencoded_strings.push_back(str);
                            break;
                            }
                        }
                    }
            };

            classifyUnencodedStrings(m_localizable_strings);
            classifyUnencodedStrings(m_marked_as_non_localizable_strings);
            classifyUnencodedStrings(m_internal_strings);
            classifyUnencodedStrings(m_not_available_for_localization_strings);
            }

        if (m_review_styles & check_printf_single_number)
            {
            // only looking at integral values (i.e., no floating-point precision)
            std::wregex intPrintf{ LR"([%]([+]|[-] #0)?(l)?(d|i|o|u|zu|c|C|e|E|x|X|l|I|I32|I64))" };
            std::wregex floatPrintf{ LR"([%]([+]|[-] #0)?(l|L)?(f|F))" };
            const auto& classifyPrintfIntStrings = [&, this](const auto& strings)
            {
                for (const auto& str : strings)
                    {
                    if (std::regex_match(str.m_string, intPrintf) ||
                        std::regex_match(str.m_string, floatPrintf))
                        {
                        m_printf_single_numbers.push_back(str);
                        }
                    }
            };
            classifyPrintfIntStrings(m_internal_strings);
            classifyPrintfIntStrings(m_localizable_strings_in_internal_call);
            }

        if (static_cast<bool>(m_review_styles & check_suspect_i18n_usage))
            {
            static std::set<std::wstring_view> strftimeFunctions{ L"strftime", L"_strftime_l",
                                                                  L"wcsftime", L"_wcsftime_l",
                                                                  L"_tcsftime" };
            const auto& classifyYearIssueStrings = [&, this](const auto& strings)
            {
                for (const auto& str : strings)
                    {
                    // quneiform-suppress-begin
                    if ((str.m_string.find(L"%g") != std::wstring::npos ||
                         str.m_string.find(L"%C") != std::wstring::npos ||
                         str.m_string.find(L"%y") != std::wstring::npos) &&
                        strftimeFunctions.find(str.m_usage.m_value) != strftimeFunctions.cend())
                        {
                        auto expandedStr{ str };
                        expandedStr.m_usage.m_value =
                            _WXTRANS_WSTR(L"Don't use two-digit year specifiers ('%g', '%y', '%C') "
                                          "in strftime-like functions.");
                        m_suspect_i18n_usage.push_back(expandedStr);
                        }
                    // quneiform-suppress-end
                    }
            };
            classifyYearIssueStrings(m_localizable_strings);
            classifyYearIssueStrings(m_not_available_for_localization_strings);
            classifyYearIssueStrings(m_internal_strings);
            }

        // if this is wxWidgets code, see if they initialized the locale framework
        if (static_cast<bool>(m_review_styles & check_suspect_i18n_usage) &&
            !m_wx_info.m_app_init_info.m_file_name.empty())
            {
            if (!m_wx_info.m_wxuilocale_initialized)
                {
                m_wx_info.m_app_init_info.m_usage.m_value =
                    _WXTRANS_WSTR(L"wxUILocale::UseDefault() should be called "
                                  "from your OnInit() function.");
                m_suspect_i18n_usage.push_back(m_wx_info.m_app_init_info);
                }
            if (is_verbose() && !m_wx_info.m_wxlocale_initialized)
                {
                m_wx_info.m_app_init_info.m_usage.m_value =
                    _WXTRANS_WSTR(L"A wxLocale object should be constructed from your OnInit() "
                                  "function if you rely on C runtime functions to be localized.");
                m_suspect_i18n_usage.push_back(m_wx_info.m_app_init_info);
                }
            }

        // log any parsing errors
        run_diagnostics();
        }

    //--------------------------------------------------
    void i18n_review::classify_non_localizable_string(string_info str)
        {
        if (m_review_styles & check_not_available_for_l10n)
            {
            if (!should_exceptions_be_translatable() &&
                (m_exceptions.find(str.m_usage.m_value) != m_exceptions.cend() ||
                 m_exceptions.find(str.m_usage.m_variableInfo.m_type) != m_exceptions.cend()))
                {
                return;
                }
            if (m_log_functions.find(str.m_usage.m_value) != m_log_functions.cend())
                {
                return;
                }

            if (is_untranslatable_string(str.m_string, true).first)
                {
                m_internal_strings.push_back(str);
                }
            else
                {
                m_not_available_for_localization_strings.push_back(str);
                }
            }
        }

    //--------------------------------------------------
    std::wstring_view i18n_review::extract_base_function(std::wstring_view str)
        {
        if (str.empty() || !is_valid_name_char(str.back()))
            {
            return std::wstring_view{};
            }
        if (str.length() == 1)
            {
            return is_valid_name_char(str[0]) ? str : std::wstring_view{};
            }
        for (int64_t i = static_cast<int64_t>(str.length() - 1); i >= 0; --i)
            {
            if (!is_valid_name_char(str[static_cast<size_t>(i)]))
                {
                return str.substr(static_cast<size_t>(i) + 1, str.length());
                }
            }
        return str;
        }

    //--------------------------------------------------
    void i18n_review::load_deprecated_functions(const std::wstring_view fileText,
                                                const std::filesystem::path& fileName)
        {
        if (!static_cast<bool>(m_review_styles & check_deprecated_macros))
            {
            return;
            }

        for (size_t i = 0; i < fileText.length(); /* in loop*/)
            {
            for (const auto& func : m_deprecated_string_functions)
                {
                if (fileText.substr(i, func.first.length()).compare(func.first) == 0 &&
                    // ensure function is a whole-word match and has something after it
                    (i + func.first.length() < fileText.length() &&
                     !is_valid_name_char(fileText[i + func.first.length()])) &&
                    (i == 0 || !is_valid_name_char(fileText[i - 1])))
                    {
                    m_deprecated_macros.push_back(string_info(
                        std::wstring{ func.first.data(), func.first.length() },
                        string_info::usage_info(string_info::usage_info::usage_type::function,
                                                std::wstring(func.second), std::wstring{},
                                                std::wstring{}),
                        fileName, get_line_and_column(i, fileText.data())));
                    i += func.first.length();
                    continue;
                    }
                }
            ++i;
            }
        }

    //--------------------------------------------------
    void i18n_review::load_suspect_i18n_usage(const std::wstring_view fileText,
                                              const std::filesystem::path& fileName)
        {
        if (!static_cast<bool>(m_review_styles & check_suspect_i18n_usage))
            {
            return;
            }

            // ::LoadString() being called
            {
            const std::wregex loadStringRegEx{
                LR"(([:]{2,2})?LoadString(A|W)?[(](\s*[a-zA-Z0-9_]+\s*,){3}\s*[a-zA-Z0-9_]+[)])"
            };
            auto currentTextBlock{ fileText };
            std::match_results<decltype(currentTextBlock)::const_iterator> stPositions;
            size_t currentBlockOffset{ 0 };
            while (std::regex_search(currentTextBlock.cbegin(), currentTextBlock.cend(),
                                     stPositions, loadStringRegEx))
                {
                currentTextBlock = currentTextBlock.substr(stPositions.position());
                currentBlockOffset += stPositions.position();
                if (currentBlockOffset == 0 ||
                    !(i18n_string_util::is_alpha_7bit(fileText[currentBlockOffset - 1]) ||
                      fileText[currentBlockOffset - 1] == L'.'))
                    {
                    m_suspect_i18n_usage.push_back(string_info(
                        std::wstring{ currentTextBlock.substr(0, stPositions.length()) },
                        string_info::usage_info(
                            string_info::usage_info::usage_type::function,
                            _(L"Prefer using CString::LoadString() (if using MFC) "
                              "or a different framework's string "
                              "loading function. Calling ::LoadString() requires a "
                              "fixed-size buffer and may result "
                              "in truncating translated strings.")
#ifdef wxVERSION_NUMBER

                                .wc_string(),
#else
                                ,
#endif
                            std::wstring{}, std::wstring{}),
                        fileName, get_line_and_column(currentBlockOffset, fileText.data())));
                    }
                currentTextBlock = currentTextBlock.substr(stPositions.length());
                currentBlockOffset += stPositions.length();
                }
            }

            // _(), wxPLURAL() not taking a literal string
            {
            const std::wregex l10nStringNonStringLiteralArgRegEx{
                LR"(\b(_|wxPLURAL)\([\(\s]*([a-zA-Z0-9]+)([[:punct:]]))"
            };
            auto currentTextBlock{ fileText };
            std::match_results<decltype(currentTextBlock)::const_iterator> stPositions;
            size_t currentBlockOffset{ 0 };
            while (std::regex_search(currentTextBlock.cbegin(), currentTextBlock.cend(),
                                     stPositions, l10nStringNonStringLiteralArgRegEx))
                {
                currentTextBlock = currentTextBlock.substr(stPositions.position(1));
                currentBlockOffset += stPositions.position(1);
                if (stPositions.size() >= 3 &&
                    // only something like LR, L, u8, etc. can be in front of a quote
                    stPositions[2].length() > 2 && stPositions[3].str() != L"\"")
                    {
                    m_suspect_i18n_usage.push_back(string_info(
                        stPositions[2].str(),
                        string_info::usage_info(string_info::usage_info::usage_type::function,
                                                _(L"Only string literals should be passed to _() "
                                                  "and wxPLURAL() functions.")
#ifdef wxVERSION_NUMBER
                                                    .wc_string(),
#else
                                                    ,
#endif
                                                std::wstring{}, std::wstring{}),
                        fileName, get_line_and_column(currentBlockOffset, fileText.data())));
                    }
                currentTextBlock = currentTextBlock.substr(stPositions.length());
                currentBlockOffset += stPositions.length();
                }
            }
        }

    //--------------------------------------------------
    void i18n_review::load_id_assignments(const std::wstring_view fileText,
                                          const std::filesystem::path& fileName)
        {
        if (!(static_cast<bool>(m_review_styles & check_duplicate_value_assigned_to_ids) ||
              static_cast<bool>(m_review_styles & check_number_assigned_to_id)))
            {
            return;
            }
        std::vector<std::pair<size_t, std::wstring>> matches;

        auto currentTextBlock{ fileText };
        std::match_results<decltype(currentTextBlock)::const_iterator> stPositions;
        size_t currentBlockOffset{ 0 };
        while (std::regex_search(currentTextBlock.cbegin(), currentTextBlock.cend(), stPositions,
                                 m_id_assignment_regex))
            {
            currentBlockOffset += stPositions.position();
            currentTextBlock = currentTextBlock.substr(stPositions.position());

            matches.emplace_back(currentBlockOffset,
                                 currentTextBlock.substr(0, stPositions.length()));

            currentBlockOffset += stPositions.length();

            currentTextBlock = currentTextBlock.substr(stPositions.length());
            }

        const std::wregex varNamePartsRE{ L"([a-zA-Z0-9_]*)(ID)([a-zA-Z0-9_]*)" };
        const std::wregex varNameIDPartsRE{ L"([a-zA-Z0-9_]*)(ID[A-Z]?[_]?)([a-zA-Z0-9_]*)" };
        // no std::from_chars for wchar_t :(
        const std::wregex numRE{ LR"(^[\-0-9']+$)" };
        if (!matches.empty())
            {
            std::vector<std::tuple<size_t, std::wstring, std::wstring>> idAssignments;
            idAssignments.reserve(matches.size());
            std::vector<std::wstring> subMatches;
            std::vector<std::wstring> idNameParts;
            // ID and variable name assigned to
            std::map<std::wstring, std::wstring> assignedIds;
            for (const auto& match : matches)
                {
                subMatches.clear();
                idNameParts.clear();
                auto varAssignment{ match.second };
                // get the var name and ID
                std::copy(std::regex_token_iterator<
                              std::remove_reference_t<decltype(varAssignment)>::const_iterator>(
                              varAssignment.cbegin(), varAssignment.cend(), m_id_assignment_regex,
                              { 3, 4, 5 }),
                          std::regex_token_iterator<
                              std::remove_reference_t<decltype(varAssignment)>::const_iterator>{},
                          std::back_inserter(subMatches));
                // ignore function calls or constructed objects assigning an ID
                if (subMatches[2] == L"(" || subMatches[2] == L"{")
                    {
                    continue;
                    }
                // clean up ID (e.g., 1'000 -> 1000)
                string_util::remove_all(subMatches[1], L'\'');
                string_util::trim(subMatches[1]);
                string_util::remove_all(subMatches[1], L' ');
                // break the ID into parts and see what's around "ID";
                // we don't want "ID" if it is part of a word like "WIDTH"
                std::copy(std::regex_token_iterator<
                              std::remove_reference_t<decltype(subMatches[0])>::const_iterator>(
                              subMatches[0].cbegin(), subMatches[0].cend(), varNamePartsRE,
                              { 1, 2, 3, 4, 5 }),
                          std::regex_token_iterator<
                              std::remove_reference_t<decltype(subMatches[0])>::const_iterator>{},
                          std::back_inserter(idNameParts));
                // MFC IDs
                if ((idNameParts[0].empty() ||
                     (idNameParts[0].length() > 0 &&
                      !static_cast<bool>(std::iswupper(idNameParts[0].back())))) &&
                    (idNameParts[2].starts_with(L"R_") || idNameParts[2].starts_with(L"D_") ||
                     idNameParts[2].starts_with(L"C_") || idNameParts[2].starts_with(L"I_") ||
                     idNameParts[2].starts_with(L"B_") || idNameParts[2].starts_with(L"S_") ||
                     idNameParts[2].starts_with(L"M_") || idNameParts[2].starts_with(L"P_")))
                    {
                    idAssignments.push_back({ match.first, subMatches[0], subMatches[1] });
                    continue;
                    }
                if ((idNameParts[0].length() > 0 &&
                     static_cast<bool>(std::iswupper(idNameParts[0].back()))) ||
                    (idNameParts[2].length() > 0 &&
                     static_cast<bool>(std::iswupper(idNameParts[2].front()))))
                    {
                    continue;
                    }

                idAssignments.push_back({ match.first, subMatches[0], subMatches[1] });
                }
            for (const auto& [position, string1, string2] : idAssignments)
                {
                idNameParts.clear();
                std::copy(std::regex_token_iterator<
                              std::remove_reference_t<decltype(string1)>::const_iterator>(
                              string1.cbegin(), string1.cend(), varNameIDPartsRE, { 1, 2, 3 }),
                          std::regex_token_iterator<
                              std::remove_reference_t<decltype(string1)>::const_iterator>(),
                          std::back_inserter(idNameParts));
                const auto idVal = [&string2 = string2]()
                {
                    try
                        {
                        if (string2.starts_with(L"0x"))
                            {
                            return std::optional<int32_t>(std::stol(string2, nullptr, 16));
                            }
                        return std::optional<int32_t>(std::stol(string2));
                        }
                    catch (...)
                        {
                        return std::optional<int32_t>{ std::nullopt };
                        }
                }();
                const int32_t idRangeStart{ 1 };
                const int32_t menuIdRangeEnd{ 0x6FFF };
                const int32_t stringIdRangeEnd{ 0x7FFF };
                const int32_t dialogIdRangeStart{ 8 };
                const int32_t dialogIdRangeEnd{ 0xDFFF };
                if (static_cast<bool>(m_review_styles & check_number_assigned_to_id) && idVal &&
                    !(idVal.value() >= idRangeStart && idVal.value() <= menuIdRangeEnd) &&
                    (idNameParts[1] == L"IDR_" || idNameParts[1] == L"IDD_" ||
                     idNameParts[1] == L"IDM_" || idNameParts[1] == L"IDC_" ||
                     idNameParts[1] == L"IDI_" || idNameParts[1] == L"IDB_"))
                    {
                    m_ids_assigned_number.push_back(string_info(
#ifdef wxVERSION_NUMBER
                        wxString::Format(_(L"%s assigned to %s; value should be between 1 and "
                                           "0x6FFF if this is an MFC project."),
                                         string2, string1)
                            .wc_string(),
#else
                        string2 + _DT(L" assigned to ") + string1 +
                            _DT(L"; value should be between 1 and 0x6FFF if "
                                "this is an MFC project."),
#endif
                        string_info::usage_info{}, fileName,
                        std::make_pair(get_line_and_column(position, fileText).first,
                                       std::wstring::npos)));
                    }
                else if (static_cast<bool>(m_review_styles & check_number_assigned_to_id) &&
                         idVal &&
                         !(idVal.value() >= idRangeStart && idVal.value() <= stringIdRangeEnd) &&
                         (idNameParts[1] == L"IDS_" || idNameParts[1] == L"IDP_"))
                    {
                    m_ids_assigned_number.emplace_back(
#ifdef wxVERSION_NUMBER
                        wxString::Format(_(L"%s assigned to %s; value should be between 1 and "
                                           "0x7FFF if this is an MFC project."),
                                         string2, string1)
                            .wc_string(),
#else
                        string2 + _DT(L" assigned to ") + string1 +
                            _DT(L"; value should be between 1 and 0x7FFF if "
                                "this is an MFC project."),
#endif
                        string_info::usage_info{}, fileName,
                        std::make_pair(get_line_and_column(position, fileText).first,
                                       std::wstring::npos));
                    }
                else if (static_cast<bool>(m_review_styles & check_number_assigned_to_id) &&
                         idVal &&
                         !(idVal.value() >= dialogIdRangeStart &&
                           idVal.value() <= dialogIdRangeEnd) &&
                         idNameParts[1] == L"IDC_")
                    {
                    m_ids_assigned_number.emplace_back(
#ifdef wxVERSION_NUMBER
                        wxString::Format(_(L"%s assigned to %s; value should be between 8 and "
                                           "0xDFFF if this is an MFC project."),
                                         string2, string1)
                            .wc_string(),
#else
                        string2 + _DT(L" assigned to ") + string1 +
                            _DT(L"; value should be between 8 and 0xDFFF if "
                                "this is an MFC project."),
#endif
                        string_info::usage_info{}, fileName,
                        std::make_pair(get_line_and_column(position, fileText).first,
                                       std::wstring::npos));
                    }
                else if (static_cast<bool>(m_review_styles & check_number_assigned_to_id) &&
                         idNameParts[1].length() <= 3 && // ignore MFC IDs (handled above)
                         std::regex_match(string2, numRE) &&
                         // -1 or 0 are usually generic IDs for the framework or
                         // temporary init values
                         string2 != L"-1" && string2 != L"0")
                    {
                    m_ids_assigned_number.emplace_back(
#ifdef wxVERSION_NUMBER
                        wxString::Format(_(L"%s assigned to %s"), string2, string1).wc_string(),
#else
                        string2 + _DT(L" assigned to ") + string1,
#endif
                        string_info::usage_info{}, fileName,
                        std::make_pair(get_line_and_column(position, fileText).first,
                                       std::wstring::npos));
                    }

                const auto [pos, inserted] = assignedIds.insert(std::make_pair(string2, string1));

                if (static_cast<bool>(m_review_styles & check_duplicate_value_assigned_to_ids) &&
                    !inserted && string2.length() > 0 &&
                    // ignore if same ID is assigned to variables with the same name
                    string1 != pos->second && string2 != L"wxID_ANY" && string2 != L"wxID_NONE" &&
                    string2 != L"-1" && string2 != L"0")
                    {
                    m_duplicates_value_assigned_to_ids.push_back(string_info(
#ifdef wxVERSION_NUMBER
                        wxString::Format(_(L"%s has been assigned to multiple ID variables."),
                                         string2)
                            .wc_string(),
#else
                        string2 + _DT(L" has been assigned to multiple ID variables."),
#endif
                        string_info::usage_info{}, fileName,
                        std::make_pair(get_line_and_column(position, fileText).first,
                                       std::wstring::npos)));
                    }
                }
            }
        }

    //--------------------------------------------------
    bool i18n_review::is_string_resource_faux_plural(const string_info& str)
        {
        // Qt functions (and possibly other frameworks) dynamically detect "(s")
        // and make separate strings if you provide a number in its context arguments,
        // so we will ignore anything with a context here.
        if (str.m_usage.m_hasContext)
            {
            return false;
            }
        return is_string_faux_plural(str.m_string);
        }

    //--------------------------------------------------
    bool i18n_review::is_string_faux_plural(std::wstring_view str)
        {
        static const std::wregex multiFauxPlurals{ LR"(\b\(s\))" };
        return load_matches(str, multiFauxPlurals).size() > 0;
        }

    //--------------------------------------------------
    bool i18n_review::is_string_multipart(std::wstring_view str)
        {
        static const std::wregex multiConsecSpaces{ LR"(([ ]{2,}|\\t))" };
        return load_matches(str, multiConsecSpaces).size() > 2;
        }

    //--------------------------------------------------
    bool i18n_review::is_string_pronoun(std::wstring_view str)
        {
        static const std::wregex pronouns{ LR"(^\s*(he|she|him|her|they)\s*$)",
                                           std::regex_constants::icase };

        return std::regex_match(str.cbegin(), str.cend(), pronouns);
        }

    //--------------------------------------------------
    bool i18n_review::is_string_article_issue(std::wstring_view str)
        {
        // assuming % or { is a dynamic placeholder may yield false positives,
        // but them proceeding an article when not a placeholder would be very rare
        static const std::wregex articleAndPlaceholderRegex{ LR"(\b(a|an|the)\s+[%{])",
                                                             std::regex_constants::icase };

        return (load_matches(str, articleAndPlaceholderRegex).size() > 0);
        }

    //--------------------------------------------------
    bool i18n_review::is_string_ambiguous(std::wstring_view str)
        {
        // quneiform-suppress-begin
        static std::set<std::wstring_view> common_acronyms = { L"N/A",    L"NA",       L"OK",
                                                               L"ASCII",  L"US-ASCII", L"CD",
                                                               L"CD-ROM", L"DVD",      L"URL" };
        static std::set<std::wstring_view> allowable_values = {
            _DT(L" of "), _DT(L"Page "), _DT(L"Column "),
            _DT(L"Row "), _DT(L"Line "), _DT(L"Page  of ")
        };
        // quneiform-suppress-end
        // Just one word?
        if (str.find_first_of(L" \t\n\r") == std::wstring::npos &&
            str.find(L"\\t") == std::wstring::npos)
            {
            // this is probably some sort of complex syntactical string
            // if it's abnormally long and has no spaces
            if (str.length() >= 32)
                {
                return true;
                }
            // trim punctuation from string that we would like to ignore
            // before a final review
            if ((str.starts_with(L'\'') && str.ends_with(L'\'')) ||
                (str.starts_with(L'"') && str.ends_with(L'"')) ||
                (str.starts_with(L'<') && str.ends_with(L'>')) ||
                (str.starts_with(L'(') && str.ends_with(L')')) ||
                (str.starts_with(L'[') && str.ends_with(L']')))
                {
                str.remove_prefix(1);
                str.remove_suffix(1);
                }
            if (str.ends_with(L':'))
                {
                str.remove_suffix(1);
                }
            if (str.starts_with(LR"(\")") || str.starts_with(LR"(\')"))
                {
                str.remove_prefix(2);
                }
            if (str.ends_with(LR"(\")") || str.ends_with(LR"(\')"))
                {
                str.remove_suffix(2);
                }
            // quneiform-suppress-begin
            if (str.ends_with(L"..."))
                {
                str.remove_suffix(3);
                }
            // quneiform-suppress-end
            if (str.ends_with(L"(s)"))
                {
                str.remove_suffix(3);
                }
            if (str.starts_with(L"&"))
                {
                str.remove_prefix(1);
                }
            if (str.starts_with(L"<br/>"))
                {
                str.remove_prefix(5);
                }
            if (str.ends_with(L"<br/>"))
                {
                str.remove_suffix(5);
                }
            if (str.starts_with(L"<br>"))
                {
                str.remove_prefix(4);
                }
            if (str.ends_with(L"<br>"))
                {
                str.remove_suffix(4);
                }

            if (str.empty())
                {
                return false;
                }
            // some acronyms are self explanatory, so ignore them
            if (common_acronyms.find(str) != common_acronyms.cend())
                {
                return false;
                }
            // Single word with multiple punctuation marks?
            const size_t punctCount = std::count_if(str.cbegin(), str.cend(),
                                                    [](const auto chr)
                                                    {
                                                        return std::iswpunct(chr) && chr != L'-' &&
                                                               chr != L'/' && chr != L'\\' &&
                                                               chr != L'&' && chr != L'.';
                                                    });
            if (punctCount > 1)
                {
                return true;
                }
            // All CAPS and/or punctuation?
            const size_t cappedOrPunctCount =
                std::count_if(str.cbegin(), str.cend(), [](const auto chr)
                              { return std::iswupper(chr) || std::iswpunct(chr); });
            if (cappedOrPunctCount == str.length())
                {
                return true;
                }

            return false;
            }

        // placeholders that may need an explanation
        if (str.find(L"###") != std::wstring_view::npos ||
            str.find(L"XXXX") != std::wstring_view::npos)
            {
            return true;
            }

        // we will ignore something like "Name: %s", where '%s'
        // would obviously be a name of some sort
        static const std::wregex colonAndPrintf{ LR"(\:( |\\t)%)" };
        const auto colonAndPrintfs = load_matches(str, colonAndPrintf);

        // String with many printf commands or a short string with at least one?
        // That could use a context also.
        std::wstring errorInfo;
        const auto printfCmds = load_cpp_printf_commands(str, errorInfo);
        size_t nonObviousCommands = (colonAndPrintfs.size() > printfCmds.size()) ?
                                        0 :
                                        (printfCmds.size() - colonAndPrintfs.size());
        if (nonObviousCommands >= 3 || (nonObviousCommands >= 2 && str.length() < 16) ||
            (nonObviousCommands >= 1 && str.length() < 10))
            {
            std::wstring filteredStr{ str };
            i18n_string_util::remove_printf_commands(filteredStr);
            // although this meets the criteria, "%1 of %2", "Page %d", etc.
            // are probably self explanatory
            if (allowable_values.find(filteredStr) != allowable_values.cend())
                {
                return false;
                }
            return true;
            }

        // String with many "%1" commands or a short string with at least one?
        const auto posCmds = load_positional_commands(str);
        nonObviousCommands = (colonAndPrintfs.size() > posCmds.size()) ?
                                 0 :
                                 (posCmds.size() - colonAndPrintfs.size());
        if (nonObviousCommands >= 3 || (nonObviousCommands >= 2 && str.length() < 16) ||
            (nonObviousCommands >= 1 && str.length() < 10))
            {
            std::wstring filteredStr{ str };
            i18n_string_util::remove_positional_commands(filteredStr);
            if (allowable_values.find(filteredStr) != allowable_values.cend())
                {
                return false;
                }
            return true;
            }

        // more than one abbreviation in string makes it difficult to understand
        // for a translator (or anyone, really)
        static const std::wregex abbreviationRegex{ LR"([[:alpha:]]\. [a-z])" };
        const auto abbrevs = load_matches(str, abbreviationRegex);
        if (abbrevs.size() > 1)
            {
            return true;
            }

        return false;
        }

    //--------------------------------------------------
    void i18n_review::process_quote(wchar_t* currentTextPos, const wchar_t* quoteEnd,
                                    const wchar_t* functionVarNamePos,
                                    const std::wstring& functionName,
                                    const variable_info& variableInfo,
                                    const std::wstring& deprecatedMacroEncountered,
                                    const size_t parameterPosition, const bool isFollowedByComma)
        {
        // Note that we always pass 'variableInfo.m_operator' as the operator value, even if this
        // quote is really being sent to a function. This will pick up any + or ?: operations in
        // front of this quote (inside of the arguments to the parent function) that may be relevant
        // later.
        if (deprecatedMacroEncountered.length() > 0 &&
            static_cast<bool>(m_review_styles & check_deprecated_macros))
            {
            const auto foundMessage = m_deprecated_string_macros.find(deprecatedMacroEncountered);
            m_deprecated_macros.emplace_back(
                deprecatedMacroEncountered,
                string_info::usage_info(string_info::usage_info::usage_type::function,
                                        (foundMessage != m_deprecated_string_macros.cend()) ?
                                            std::wstring{ foundMessage->second } :
                                            std::wstring{},
                                        std::wstring{}, variableInfo.m_operator),
                m_file_name, get_line_and_column(currentTextPos - m_file_start));
            }

        if (variableInfo.m_name.length() > 0)
            {
            process_variable(
                variableInfo,
                std::wstring_view{ currentTextPos, static_cast<size_t>(quoteEnd - currentTextPos) },
                (currentTextPos - m_file_start));
            }
        else if (functionName.length() > 0)
            {
            if (is_diagnostic_function(functionName))
                {
                m_internal_strings.emplace_back(
                    std::wstring(currentTextPos, quoteEnd - currentTextPos),
                    string_info::usage_info(string_info::usage_info::usage_type::function,
                                            functionName, std::wstring{}, variableInfo.m_operator),
                    m_file_name, get_line_and_column(currentTextPos - m_file_start));
                }
            else if (is_i18n_function(functionName))
                {
                // i18n functions that have multiple params, some of which
                // can be context strings that should not be translatable
                if ( // Qt
                    (functionName == _DT(L"translate") && parameterPosition == 0) ||
                    (functionName == L"QApplication::translate" && parameterPosition == 0) ||
                    (functionName == L"QApplication::tr" && parameterPosition == 0) ||
                    (functionName == L"QApplication::trUtf8" && parameterPosition == 0) ||
                    (functionName == L"QCoreApplication::translate" && parameterPosition == 0) ||
                    (functionName == L"QCoreApplication::tr" && parameterPosition == 0) ||
                    (functionName == L"QCoreApplication::trUtf8" && parameterPosition == 0) ||
                    (functionName == L"tr" && parameterPosition == 1) ||
                    (functionName == L"trUtf8" && parameterPosition == 1) ||
                    (functionName == L"QT_TRANSLATE_NOOP" && parameterPosition == 0) ||
                    // wxWidgets
                    (functionName == L"wxTRANSLATE_IN_CONTEXT" && parameterPosition == 0) ||
                    (functionName == L"wxGETTEXT_IN_CONTEXT_PLURAL" && parameterPosition == 0) ||
                    (functionName == L"wxGETTEXT_IN_CONTEXT" && parameterPosition == 0) ||
                    (functionName == L"i18nc" && parameterPosition == 0) ||
                    (functionName == L"i18ncp" && parameterPosition == 0) ||
                    (functionName == L"ki18nc" && parameterPosition == 0) ||
                    (functionName == L"ki18ncp" && parameterPosition == 0) ||
                    (functionName == L"i18n" && parameterPosition > 0) ||  // acts like printf
                    (functionName == L"ki18n" && parameterPosition > 0) || // acts like printf
                    (functionName == L"wxGetTranslation" && parameterPosition >= 1))
                    {
                    m_internal_strings.emplace_back(
                        std::wstring(currentTextPos, quoteEnd - currentTextPos),
                        string_info::usage_info(string_info::usage_info::usage_type::function,
                                                functionName, std::wstring{},
                                                variableInfo.m_operator),
                        m_file_name, get_line_and_column(currentTextPos - m_file_start));

                    const auto contextLength{ quoteEnd - currentTextPos };
                    if (static_cast<bool>(m_review_styles & check_suspect_i18n_usage) &&
                        contextLength > 32 &&
                        // although these are called context parameters,
                        // they really are comments in practice
                        !functionName.starts_with(L"i18n") && !functionName.starts_with(L"ki18n"))
                        {
                        m_suspect_i18n_usage.emplace_back(
                            std::wstring{ currentTextPos,
                                          static_cast<size_t>(quoteEnd - currentTextPos) },
                            string_info::usage_info(
                                string_info::usage_info::usage_type::function,
#ifdef wxVERSION_NUMBER
                                wxString::Format(
                                    _(L"Context string is considerably long. Are the context and "
                                      "string arguments to %s possibly transposed?"),
                                    functionName)
                                    .wc_string(),
#else
                                _(L"Context string is considerably long. Are the context and "
                                  "string arguments possibly transposed?"),
#endif
                                std::wstring{}, variableInfo.m_operator, true),
                            m_file_name, get_line_and_column(currentTextPos - m_file_start));
                        }
                    }
                else if (static_cast<bool>(m_review_styles & check_suspect_i18n_usage) &&
                         (functionName == L"QT_TRID_NOOP" || functionName == L"QT_TRID_N_NOOP" ||
                          functionName == L"qtTrId"))
                    {
                    const auto contextLength{ quoteEnd - currentTextPos };
                    if (static_cast<bool>(m_review_styles & check_suspect_i18n_usage) &&
                        contextLength > 32)
                        {
                        m_suspect_i18n_usage.emplace_back(
                            functionName,
                            string_info::usage_info(
                                string_info::usage_info::usage_type::function,
#ifdef wxVERSION_NUMBER
                                _(L"This function is meant for string IDs, "
                                  "not translatable strings."
                                  "Are you sure the provided argument is an ID?")
                                    .wc_string(),
#else
                                _(L"This function is meant for string IDs, "
                                  "not translatable strings."
                                  "Are you sure the provided argument is an ID?"),
#endif
                                std::wstring{}, variableInfo.m_operator, true),
                            m_file_name, get_line_and_column(currentTextPos - m_file_start));
                        }
                    }
                else
                    {
                    m_localizable_strings.emplace_back(
                        std::wstring(currentTextPos, quoteEnd - currentTextPos),
                        string_info::usage_info(
                            string_info::usage_info::usage_type::function, functionName,
                            std::wstring{}, variableInfo.m_operator,
                            (is_i18n_with_context_function(functionName) ||
                             functionName == L"wxPLURAL" ||
                             (isFollowedByComma && extract_base_function(functionName) == L"tr") ||
                             m_context_comment_active)),
                        m_file_name, get_line_and_column(currentTextPos - m_file_start));

                    assert(functionVarNamePos);
                    if (functionVarNamePos != nullptr &&
                        static_cast<bool>(m_review_styles & check_suspect_l10n_string_usage))
                        {
                        std::wstring functionNameOuter;
                        variable_info outerVariable;
                        std::wstring deprecatedMacroOuterEncountered;
                        size_t parameterPositionOuter{ 0 };
                        read_var_or_function_name(
                            functionVarNamePos, m_file_start, functionNameOuter, outerVariable,
                            deprecatedMacroOuterEncountered, parameterPositionOuter);
                        if (deprecatedMacroOuterEncountered.length() > 0 &&
                            static_cast<bool>(m_review_styles & check_deprecated_macros))
                            {
                            m_deprecated_macros.emplace_back(
                                deprecatedMacroOuterEncountered,
                                string_info::usage_info(
                                    string_info::usage_info::usage_type::function, std::wstring{},
                                    std::wstring{}, variableInfo.m_operator),
                                m_file_name, get_line_and_column(currentTextPos - m_file_start));
                            }
                        // internal functions
                        if (is_diagnostic_function(functionNameOuter) ||
                            // CTORs whose arguments should not be translated
                            m_variable_types_to_ignore.find(functionNameOuter) !=
                                m_variable_types_to_ignore.cend())
                            {
                            m_localizable_strings_in_internal_call.emplace_back(
                                std::wstring(currentTextPos, quoteEnd - currentTextPos),
                                string_info::usage_info(
                                    string_info::usage_info::usage_type::function,
                                    functionNameOuter, std::wstring{}, variableInfo.m_operator),
                                m_file_name, get_line_and_column(currentTextPos - m_file_start));
                            }
                        // untranslatable variable types
                        else if (m_variable_types_to_ignore.find(outerVariable.m_type) !=
                                 m_variable_types_to_ignore.cend())
                            {
                            m_localizable_strings_in_internal_call.emplace_back(
                                std::wstring(currentTextPos, quoteEnd - currentTextPos),
                                string_info::usage_info(
                                    string_info::usage_info::usage_type::variable,
                                    outerVariable.m_name, outerVariable.m_type,
                                    outerVariable.m_operator),
                                m_file_name, get_line_and_column(currentTextPos - m_file_start));
                            }
                        // untranslatable variable names (e.g., debugMsg)
                        else if (outerVariable.m_name.length() > 0)
                            {
                            try
                                {
                                for (const auto& reg : get_ignored_variable_patterns())
                                    {
                                    if (std::regex_match(outerVariable.m_name, reg))
                                        {
                                        m_localizable_strings_in_internal_call.emplace_back(
                                            std::wstring(currentTextPos, quoteEnd - currentTextPos),
                                            string_info::usage_info(
                                                string_info::usage_info::usage_type::variable,
                                                outerVariable.m_name, outerVariable.m_type,
                                                outerVariable.m_operator),
                                            m_file_name,
                                            get_line_and_column(currentTextPos - m_file_start));
                                        break;
                                        }
                                    }
                                }
                            catch (const std::exception& exp)
                                {
                                log_message(outerVariable.m_name,
                                            i18n_string_util::lazy_string_to_wstring(exp.what()),
                                            (currentTextPos - m_file_start));
                                }
                            }
                        }
                    }
                // we processed the active translation function, so switch this state back off
                m_context_comment_active = false;
                }
            else if (is_non_i18n_function(functionName))
                {
                m_marked_as_non_localizable_strings.emplace_back(
                    std::wstring(currentTextPos, quoteEnd - currentTextPos),
                    string_info::usage_info(string_info::usage_info::usage_type::function,
                                            functionName, std::wstring{}, variableInfo.m_operator),
                    m_file_name, get_line_and_column(currentTextPos - m_file_start));
                }
            else if (m_variable_types_to_ignore.find(functionName) !=
                     m_variable_types_to_ignore.cend())
                {
                m_internal_strings.emplace_back(
                    std::wstring(currentTextPos, quoteEnd - currentTextPos),
                    string_info::usage_info(string_info::usage_info::usage_type::function,
                                            functionName, std::wstring{}, variableInfo.m_operator),
                    m_file_name, get_line_and_column(currentTextPos - m_file_start));
                }
            else if (is_keyword(functionName))
                {
                classify_non_localizable_string(
                    string_info(std::wstring(currentTextPos, quoteEnd - currentTextPos),
                                string_info::usage_info(string_info::usage_info::usage_type::orphan,
                                                        std::wstring{}, std::wstring{},
                                                        variableInfo.m_operator),
                                m_file_name, get_line_and_column(currentTextPos - m_file_start)));
                }
            else
                {
                classify_non_localizable_string(string_info(
                    std::wstring(currentTextPos, quoteEnd - currentTextPos),
                    string_info::usage_info(string_info::usage_info::usage_type::function,
                                            functionName, std::wstring{}, variableInfo.m_operator),
                    m_file_name, get_line_and_column(currentTextPos - m_file_start)));
                }
            }
        else
            {
            classify_non_localizable_string(string_info(
                std::wstring(currentTextPos, quoteEnd - currentTextPos),
                string_info::usage_info(string_info::usage_info::usage_type::orphan, std::wstring{},
                                        std::wstring{}, variableInfo.m_operator),
                m_file_name, get_line_and_column(currentTextPos - m_file_start)));
            }
        clear_section(currentTextPos, std::next(quoteEnd));
        }

    //--------------------------------------------------
    void i18n_review::process_variable(const variable_info& variableInfo,
                                       const std::wstring_view value, const size_t quotePosition)
        {
#ifndef NDEBUG
        if (variableInfo.m_type.length() > 0 &&
            get_ignored_variable_types().find(variableInfo.m_type) ==
                get_ignored_variable_types().cend() &&
            m_ctors_to_ignore.find(variableInfo.m_type) == m_ctors_to_ignore.cend() &&
            (variableInfo.m_type.length() < 5 ||
             std::wstring_view(variableInfo.m_type.c_str(), 5)
                     .compare(std::wstring_view{ L"std::", 5 }) != 0) &&
            (variableInfo.m_type.length() < 2 ||
             std::wstring_view(variableInfo.m_type.c_str(), 2)
                     .compare(std::wstring_view{ L"wx", 2 }) != 0) &&
            (variableInfo.m_type.length() < 3 ||
             std::wstring_view(variableInfo.m_type.c_str(), 3)
                     .compare(std::wstring_view{ L"_wx", 3 }) != 0) &&
            (variableInfo.m_type.length() < 2 ||
             std::wstring_view(variableInfo.m_type.c_str(), 2)
                     .compare(std::wstring_view{ L"My", 2 }) != 0) &&
            variableInfo.m_type != L"auto" && variableInfo.m_type != L"Bar" &&
            variableInfo.m_type != L"ifstream" && variableInfo.m_type != L"ofstream" &&
            variableInfo.m_type != L"const" && variableInfo.m_type != L"char" &&
            variableInfo.m_type != L"int" && variableInfo.m_type != L"UINT" &&
            variableInfo.m_type != L"size_t" && variableInfo.m_type != L"float" &&
            variableInfo.m_type != L"double" && variableInfo.m_type != L"static_cast" &&
            variableInfo.m_type != L"StyleInfo" && variableInfo.m_type != L"Utf8CharBuffer" &&
            variableInfo.m_type != L"rgbRecord" && variableInfo.m_type != L"LPCTSTR" &&
            variableInfo.m_type != L"CDialog" && variableInfo.m_type != L"string_view" &&
            variableInfo.m_type != L"wstring_view" && variableInfo.m_type != L"LanguageInfo" &&
            variableInfo.m_type != L"MessageParameters")
            {
            log_message(variableInfo.m_type, L"New variable type detected.", quotePosition);
            }
#endif
        // For large string values, a 1024 substring will suffice for classifying it.
        // This is more optimal and will prevent memory exhaustion with regex comparisons.
        std::wstring clippedValue{ value.length() >= 1024 ? value.substr(0, 1024) : value };
        if (get_ignored_variable_types().find(variableInfo.m_type) !=
            get_ignored_variable_types().cend())
            {
            m_internal_strings.emplace_back(
                std::move(clippedValue),
                string_info::usage_info(string_info::usage_info::usage_type::variable,
                                        variableInfo.m_name, variableInfo.m_type,
                                        variableInfo.m_operator),
                m_file_name, get_line_and_column(quotePosition));
            return;
            }
        if (!get_ignored_variable_patterns().empty())
            {
            try
                {
                bool matchedInternalVar(false);
                for (const auto& reg : get_ignored_variable_patterns())
                    {
                    if (std::regex_match(variableInfo.m_name, reg))
                        {
                        m_internal_strings.emplace_back(
                            std::move(clippedValue),
                            string_info::usage_info(string_info::usage_info::usage_type::variable,
                                                    variableInfo.m_name, variableInfo.m_type,
                                                    variableInfo.m_operator),
                            m_file_name, get_line_and_column(quotePosition));
                        matchedInternalVar = true;
                        break;
                        }
                    }
                // didn't match any known internal variable name provided by user?
                if (!matchedInternalVar)
                    {
                    classify_non_localizable_string(string_info(
                        std::move(clippedValue),
                        string_info::usage_info(string_info::usage_info::usage_type::variable,
                                                variableInfo.m_name, variableInfo.m_type,
                                                variableInfo.m_operator),
                        m_file_name, get_line_and_column(quotePosition)));
                    }
                }
            catch (const std::exception& exp)
                {
                log_message(variableInfo.m_name,
                            i18n_string_util::lazy_string_to_wstring(exp.what()), quotePosition);
                }
            }
        else
            {
            classify_non_localizable_string(
                string_info(std::move(clippedValue),
                            string_info::usage_info(string_info::usage_info::usage_type::variable,
                                                    variableInfo.m_name, variableInfo.m_type,
                                                    variableInfo.m_operator),
                            m_file_name, get_line_and_column(quotePosition)));
            }
        }

    //--------------------------------------------------
    void i18n_review::reserve(const size_t fileCount)
        {
        try
            {
            m_error_log.reserve(std::min<size_t>(fileCount, 10));
            m_localizable_strings.reserve(std::min<size_t>(fileCount, 10));
            m_not_available_for_localization_strings.reserve(std::min<size_t>(fileCount, 10));
            m_marked_as_non_localizable_strings.reserve(std::min<size_t>(fileCount, 10));
            m_internal_strings.reserve(std::min<size_t>(fileCount, 10));
            m_unsafe_localizable_strings.reserve(std::min<size_t>(fileCount, 10));
            m_deprecated_macros.reserve(std::min<size_t>(fileCount, 10));
            }
        catch (const std::bad_alloc&)
            {
            }
        }

    //--------------------------------------------------
    void i18n_review::clear_results()
        {
        m_localizable_strings.clear();
        m_localizable_strings_with_unlocalizable_content.clear();
        m_localizable_strings_with_urls.clear();
        m_localizable_strings_ambiguous_needing_context.clear();
        m_localizable_strings_in_internal_call.clear();
        m_localizable_strings_being_concatenated.clear();
        m_literal_localizable_strings_being_compared.clear();
        m_localizable_strings_with_halfwidths.clear();
        m_multipart_strings.clear();
        m_faux_plural_strings.clear();
        m_article_issue_strings.clear();
        m_not_available_for_localization_strings.clear();
        m_marked_as_non_localizable_strings.clear();
        m_internal_strings.clear();
        m_unsafe_localizable_strings.clear();
        m_deprecated_macros.clear();
        m_unencoded_strings.clear();
        m_printf_single_numbers.clear();
        m_ids_assigned_number.clear();
        m_duplicates_value_assigned_to_ids.clear();
        m_malformed_strings.clear();
        m_trailing_spaces.clear();
        m_tabs.clear();
        m_wide_lines.clear();
        m_comments_missing_space.clear();
        m_suspect_i18n_usage.clear();

        m_wx_info = wx_project_info{};
        }

    //--------------------------------------------------
    bool i18n_review::is_diagnostic_function(const std::wstring& functionName) const
        {
        try
            {
            return (std::regex_match(functionName, m_diagnostic_function_regex) ||
                    (m_internal_functions.find(functionName) != m_internal_functions.cend()) ||
                    (m_internal_functions.find(extract_base_function(functionName)) !=
                     m_internal_functions.cend()) ||
                    functionName.ends_with(L"_TRACE") || functionName.ends_with(L"_DEBUG") ||
                    (!can_log_messages_be_translatable() &&
                     m_log_functions.find(functionName) != m_log_functions.cend()));
            }
        catch (const std::exception& exp)
            {
            log_message(functionName, i18n_string_util::lazy_string_to_wstring(exp.what()),
                        std::wstring::npos);
            return true;
            }
        }

    //--------------------------------------------------
    std::pair<bool, size_t> i18n_review::is_untranslatable_string(std::wstring strToReview,
                                                                  const bool limitWordCount) const
        {
        // if no spaces but lengthy, then this is probably some sort of GUID
        if (strToReview.length() >= 32 &&
            strToReview.find_first_of(L" \n\r") == std::wstring::npos &&
            strToReview.find(L"\\n") == std::wstring::npos &&
            strToReview.find(L"\\r") == std::wstring::npos &&
            strToReview.find(L"\\t") == std::wstring::npos)
            {
            return std::make_pair(true, strToReview.length());
            }

        static const std::wregex loremIpsum(L"Lorem ipsum.*", std::regex_constants::icase);
        static const std::wregex brownFox(L"the quick brown fox.*", std::regex_constants::icase);
        static const std::wregex percentageRegEx(LR"(([0-9]+|\{[a-z0-9]\}|%[udil]{1,2})%)");
        if (std::regex_match(strToReview, percentageRegEx))
            {
            return std::make_pair(false, strToReview.length());
            }

        i18n_string_util::replace_escaped_control_chars(strToReview);
        string_util::trim(strToReview);
        // see if a function signature before stripping printf commands and whatnot
        if ((std::regex_match(strToReview, m_function_signature_regex) ||
             std::regex_match(strToReview, m_open_function_signature_regex)) &&
            // but allow something like "Item(s)"
            !std::regex_match(strToReview, m_plural_regex))
            {
            return std::make_pair(true, strToReview.length());
            }

        i18n_string_util::remove_hex_color_values(strToReview);
        i18n_string_util::remove_printf_commands(strToReview);
        i18n_string_util::remove_escaped_unicode_values(strToReview);
        string_util::trim(strToReview);
        bool allPunctOrSpaces{ true };
        // strip control characters (these wreak havoc with the regex parser)
        for (auto& chr : strToReview)
            {
            if (chr == L'\n' || chr == L'\t' || chr == L'\r')
                {
                chr = L' ';
                }
            if (allPunctOrSpaces && !std::iswdigit(chr) && !std::iswpunct(chr) &&
                !std::iswspace(chr))
                {
                allPunctOrSpaces = false;
                }
            }
        string_util::trim(strToReview);
        // quneiform-suppress-begin
        // something like "%d%%" should be translatable
        if (allPunctOrSpaces &&
            (strToReview.find(_DT(L"%%")) != std::wstring::npos || strToReview == L"..."))
            {
            return std::make_pair(false, strToReview.length());
            }
        // quneiform-suppress-end

        try
            {
            // Handle HTML syntax that is hard coded in the source file.
            // Strip it down and then see if what's left contains translatable content.
            // Note that we skip any punctuation (not word characters, excluding '<')
            // in front of the initial '<' (sometimes there are braces and brackets
            // in front of the HTML tags).
            strToReview =
                std::regex_replace(strToReview, std::wregex(LR"(<br[[:space:]]*\/>)"), L"\n");
            string_util::trim(strToReview);
            if (std::regex_match(strToReview, m_xml_element_regex) ||
                std::regex_match(strToReview, m_html_regex) ||
                std::regex_match(strToReview, m_html_element_with_content_regex) ||
                std::regex_match(strToReview, m_html_tag_regex) ||
                std::regex_match(strToReview, m_html_tag_unicode_regex))
                {
                // it's really something like "<enter comment.>", which can be translatable
                if (std::regex_match(strToReview, m_not_xml_element_regex))
                    {
                    return std::make_pair(false, strToReview.length());
                    }

                // Avoid a false positive for single words in braces.
                // It may be an HTML/XML element, but it may also be a user-facing string,
                // so error on the side of that.
                if (std::regex_match(strToReview, m_angle_braced_one_word_regex) &&
                    !std::regex_match(strToReview, m_html_known_elements_regex))
                    {
                    if (limitWordCount)
                        {
                        // see if it has enough words
                        const auto matchCount{ std::distance(
                            std::wsregex_iterator(strToReview.cbegin(), strToReview.cend(),
                                                  m_1word_regex),
                            std::wsregex_iterator()) };
                        if (static_cast<size_t>(matchCount) <
                            get_min_words_for_classifying_unavailable_string())
                            {
                            return std::make_pair(true, strToReview.length());
                            }
                        }
                    else
                        {
                        return std::make_pair(false, strToReview.length());
                        }
                    }
                strToReview = std::regex_replace(
                    strToReview, std::wregex(LR"(<script[\d\D]*?>[\d\D]*?</script>)"), L"");
                strToReview = std::regex_replace(
                    strToReview, std::wregex(LR"(<style[\d\D]*?>[\d\D]*?</style>)"), L"");
                strToReview = std::regex_replace(
                    strToReview,
                    std::wregex(L"<[?]?[A-Za-z0-9+_/\\-\\.'\"=;:!%[:space:]\\\\,()]+[?]?>"), L"");
                strToReview = std::regex_replace(strToReview, m_xml_element_regex, L"");
                // strip things like &ldquo;
                strToReview = std::regex_replace(strToReview, std::wregex(L"&[a-zA-Z]{2,5};"), L"");
                strToReview =
                    std::regex_replace(strToReview, std::wregex(L"&#[[:digit:]]{2,4};"), L"");
                }

            // strings that may look like they should not be translatable, but are actually OK
            for (const auto& reg : m_translatable_regexes)
                {
                if (std::regex_match(strToReview, reg))
                    {
#ifndef NDEBUG
                    if (strToReview.length() > m_longest_internal_string.first.length())
                        {
                        m_longest_internal_string.first = strToReview;
                        m_longest_internal_string.second = reg;
                        }
#endif
                    return std::make_pair(false, strToReview.length());
                    }
                }

            // see if it has enough words
            const auto matchCount{ std::distance(
                std::wsregex_iterator(strToReview.cbegin(), strToReview.cend(), m_1word_regex),
                std::wsregex_iterator()) };
            if (!is_allowing_translating_punctuation_only_strings() && matchCount == 0)
                {
                return std::make_pair(true, strToReview.length());
                }
            else if (limitWordCount)
                {
                if (static_cast<size_t>(matchCount) <
                    get_min_words_for_classifying_unavailable_string())
                    {
                    return std::make_pair(true, strToReview.length());
                    }
                }

            // Nothing but punctuation? If that's OK to allow, then let it through.
            if (is_allowing_translating_punctuation_only_strings() &&
                std::regex_match(strToReview, std::wregex(L"[[:punct:]]+")))
                {
                return std::make_pair(false, strToReview.length());
                }

            // "N/A", "O&n", and "O&K" are OK to translate, but it won't meet the criterion of at
            // least two consecutive letters, so check for that first.
            if (strToReview.length() == 3 &&
                ((string_util::is_either(strToReview[0], L'N', L'n') && strToReview[1] == L'/' &&
                  string_util::is_either(strToReview[2], L'A', L'a')) ||
                 (string_util::is_either(strToReview[0], L'O', L'o') && strToReview[1] == L'&' &&
                  (string_util::is_either(strToReview[2], L'N', L'n') ||
                   string_util::is_either(strToReview[2], L'K', L'k')))))
                {
                return std::make_pair(false, strToReview.length());
                }
            constexpr size_t maxWordSize{ 20 };
            if (strToReview.length() <= 1 ||
                // not at least two letters together
                !std::regex_search(strToReview, m_2letter_regex) ||
                // single word (no spaces or word separators) and more than 20 characters--
                // doesn't seem like a real word meant for translation
                (strToReview.length() > maxWordSize &&
                 strToReview.find_first_of(L" \n\t\r/-") == std::wstring::npos &&
                 strToReview.find(L"\\n") == std::wstring::npos &&
                 strToReview.find(L"\\r") == std::wstring::npos &&
                 strToReview.find(L"\\t") == std::wstring::npos) ||
                m_known_internal_strings.find(strToReview.c_str()) !=
                    m_known_internal_strings.end() ||
                // a string like "_tcscoll" be odd to be in string, but just in case it
                // should not be localized
                m_deprecated_string_functions.find(strToReview.c_str()) !=
                    m_deprecated_string_functions.end() ||
                m_deprecated_string_macros.find(strToReview.c_str()) !=
                    m_deprecated_string_macros.end())
                {
                return std::make_pair(true, strToReview.length());
                }
            // RTF text
            if (strToReview.compare(0, 3, LR"({\\)") == 0)
                {
                return std::make_pair(true, strToReview.length());
                }
            // social media hashtag (or formatting code of some sort),
            // keyboard shortcuts, code, placeholder text, etc.
            if (std::regex_match(strToReview, m_hashtag_regex) ||
                std::regex_match(strToReview, m_key_shortcut_regex) ||
                std::regex_match(strToReview, loremIpsum) ||
                std::regex_match(strToReview, brownFox) ||
                std::regex_match(strToReview, m_sql_code))
                {
                return std::make_pair(true, strToReview.length());
                }

            constexpr size_t minMessageLength{ 200 };
            // if we know it has at least one word (and spaces) at this point,
            // then it being more than 200 characters means that it probably is
            // a real user-message (not an internal string)
            if (strToReview.length() > minMessageLength)
                {
                return std::make_pair(false, strToReview.length());
                }

            if (m_untranslatable_exceptions.find(strToReview) != m_untranslatable_exceptions.cend())
                {
                return std::make_pair(false, strToReview.length());
                }

            for (const auto& reg : m_untranslatable_regexes)
                {
                if (std::regex_match(strToReview, reg))
                    {
#ifndef NDEBUG
                    if (strToReview.length() > m_longest_internal_string.first.length())
                        {
                        m_longest_internal_string.first = strToReview;
                        m_longest_internal_string.second = reg;
                        }
#endif
                    return std::make_pair(true, strToReview.length());
                    }
                }
            return std::make_pair((is_font_name(strToReview.c_str()) ||
                                   is_file_extension(strToReview.c_str()) ||
                                   i18n_string_util::is_file_address(strToReview)),
                                  strToReview.length());
            }
        catch (const std::exception& exp)
            {
            log_message(strToReview, i18n_string_util::lazy_string_to_wstring(exp.what()),
                        std::wstring::npos);
            return std::make_pair(false, strToReview.length());
            }
        }

    //--------------------------------------------------
    std::wstring i18n_review::collapse_multipart_string(std::wstring str) const
        {
        // for strings that span multiple lines, remove the start/end quotes and newlines
        // between them, combining this into one string
        const std::wregex multilineRegex(LR"(([^\\])("[\s]+"))");
        str = std::regex_replace(str, multilineRegex, L"$1");
        // replace any doubled-up quotes with single
        // (C# does this for raw strings)
        if (m_collapse_double_quotes)
            {
            string_util::replace_all<std::wstring>(str, LR"("")", LR"(")");
            }
        return str;
        }

    //--------------------------------------------------
    void i18n_review::process_strings()
        {
        const auto processStrings = [this](auto& strings)
        {
            std::for_each(strings.begin(), strings.end(), [this](auto& val)
                          { val.m_string = i18n_review::collapse_multipart_string(val.m_string); });
        };
        processStrings(m_localizable_strings);
        processStrings(m_localizable_strings_in_internal_call);
        processStrings(m_not_available_for_localization_strings);
        processStrings(m_marked_as_non_localizable_strings);
        processStrings(m_internal_strings);
        processStrings(m_unsafe_localizable_strings);
        }

    //--------------------------------------------------
    void i18n_review::run_diagnostics()
        {
        for (const auto& str : m_localizable_strings)
            {
            if (str.m_usage.m_value.empty() &&
                str.m_usage.m_type != string_info::usage_info::usage_type::orphan)
                {
                log_message(str.m_string,
                            L"Unknown function or variable assignment for this string.",
                            std::wstring::npos);
                }
            }
        for (const auto& str : m_not_available_for_localization_strings)
            {
            if (str.m_usage.m_value.empty() &&
                str.m_usage.m_type != string_info::usage_info::usage_type::orphan)
                {
                log_message(str.m_string,
                            L"Unknown function or variable assignment for this string.",
                            std::wstring::npos);
                }
            }
        for (const auto& str : m_marked_as_non_localizable_strings)
            {
            if (str.m_usage.m_value.empty() &&
                str.m_usage.m_type != string_info::usage_info::usage_type::orphan)
                {
                log_message(str.m_string,
                            L"Unknown function or variable assignment for this string.",
                            std::wstring::npos);
                }
            }
        for (const auto& str : m_internal_strings)
            {
            if (str.m_usage.m_value.empty() &&
                str.m_usage.m_type != string_info::usage_info::usage_type::orphan)
                {
                log_message(str.m_string,
                            L"Unknown function or variable assignment for this string.",
                            std::wstring::npos);
                }
            }
        for (const auto& str : m_unsafe_localizable_strings)
            {
            if (str.m_usage.m_value.empty() &&
                str.m_usage.m_type != string_info::usage_info::usage_type::orphan)
                {
                log_message(str.m_string,
                            L"Unknown function or variable assignment for this string.",
                            std::wstring::npos);
                }
            }
        }

    //--------------------------------------------------
    const wchar_t* i18n_review::read_var_or_function_name(const wchar_t* startPos,
                                                          const wchar_t* const startSentinel,
                                                          std::wstring& functionName,
                                                          variable_info& variableInfo,
                                                          std::wstring& deprecatedMacroEncountered,
                                                          size_t& parameterPosition)
        {
        functionName.clear();
        variableInfo.reset();
        parameterPosition = 0;
        deprecatedMacroEncountered.clear();
        int32_t closeParenCount{ 0 };
        int32_t closeBraseCount{ 0 };
        bool quoteWrappedInCTOR{ false };
        const wchar_t* functionOrVarNamePos = startPos;

        const auto readOperator =
            [&variableInfo](const wchar_t* startingPos, const wchar_t* operatorEnd)
        {
            if (startingPos < operatorEnd)
                {
                const wchar_t* operatorStart{ startingPos };
                while (std::next(operatorStart) < operatorEnd &&
                       static_cast<bool>(std::iswspace(*operatorStart)))
                    {
                    std::advance(operatorStart, 1);
                    }
                variableInfo.m_operator.assign(operatorStart,
                                               static_cast<size_t>(operatorEnd - operatorStart));
                string_util::trim(variableInfo.m_operator);
                }
        };

        /// Reads the variable type from a variable constructed from a string.
        const auto readVarType = [&]()
        {
            const auto loadVarType = [&]()
            {
                variableInfo.m_type.clear();
                if (functionOrVarNamePos == startSentinel)
                    {
                    return;
                    }
                std::advance(functionOrVarNamePos, -1);
                while (functionOrVarNamePos > startSentinel &&
                       static_cast<bool>(std::iswspace(*functionOrVarNamePos)))
                    {
                    std::advance(functionOrVarNamePos, -1);
                    }
                const auto* typeEnd = std::next(functionOrVarNamePos);
                // if a template, then step over (going backwards) the template arguments
                // to get to the root type
                if (std::prev(typeEnd) > startSentinel && *std::prev(typeEnd) == L'>')
                    {
                    // if a pointer accessor (->) then bail as it won't be a variable assignment
                    if (std::prev(typeEnd, 2) > startSentinel && *std::prev(typeEnd, 2) == L'-')
                        {
                        return;
                        }
                    assert(functionOrVarNamePos >= startSentinel);
                    const auto openingAngle = std::wstring_view{ startSentinel }.find_last_of(
                        L'<', functionOrVarNamePos - startSentinel);
                    if (openingAngle == std::wstring::npos)
                        {
                        log_message(L"Template parse error",
                                    L"Unable to find opening < for template variable.",
                                    functionOrVarNamePos - startSentinel);
                        return;
                        }
                    functionOrVarNamePos =
                        std::next(startSentinel, static_cast<ptrdiff_t>(openingAngle));
                    }
                while (
                    functionOrVarNamePos > startSentinel &&
                    (is_valid_name_char_ex(*functionOrVarNamePos) || *functionOrVarNamePos == L'&'))
                    {
                    std::advance(functionOrVarNamePos, -1);
                    }
                if (!is_valid_name_char_ex(*functionOrVarNamePos))
                    {
                    std::advance(functionOrVarNamePos, 1);
                    }
                variableInfo.m_type.assign(functionOrVarNamePos, typeEnd - functionOrVarNamePos);
                // make sure the variable type is a word, not something like "<<"
                if (variableInfo.m_type.length() > 0 &&
                    !static_cast<bool>(std::iswalpha(variableInfo.m_type.front())))
                    {
                    variableInfo.m_type.clear();
                    }
                remove_decorations(variableInfo.m_type);
            };

            loadVarType();
            if (is_variable_type_decorator(variableInfo.m_type))
                {
                loadVarType();
                }

            // ignore case labels, else commands, etc.
            if (is_keyword(variableInfo.m_type) ||
                (variableInfo.m_type.length() > 0 && variableInfo.m_type.back() == L':'))
                {
                variableInfo.m_type.clear();
                }
        };

        while (startPos > startSentinel)
            {
            if (*startPos == L')')
                {
                ++closeParenCount;
                std::advance(startPos, -1);
                }
            else if (*startPos == L'}')
                {
                ++closeBraseCount;
                std::advance(startPos, -1);
                }
            else if (*startPos == L'(' || *startPos == L'{')
                {
                const auto currentOpeningChar{ *startPos };
                std::advance(startPos, -1);
                // if just closing the terminating parenthesis for a function
                // call in the list of parameters, then skip it and keep going
                // to find the outer function call that this string really belongs to.
                if (currentOpeningChar == L'(')
                    {
                    --closeParenCount;
                    }
                else if (currentOpeningChar == L'{')
                    {
                    --closeBraseCount;
                    }
                if (closeParenCount >= 0 && closeBraseCount >= 0)
                    {
                    continue;
                    }
                // skip whitespace between open parenthesis and function name
                while (startPos > startSentinel && static_cast<bool>(std::iswspace(*startPos)))
                    {
                    std::advance(startPos, -1);
                    }
                functionOrVarNamePos = startPos;
                while (functionOrVarNamePos > startSentinel &&
                       is_valid_name_char_ex(*functionOrVarNamePos))
                    {
                    std::advance(functionOrVarNamePos, -1);
                    }
                // If we are on the start of the text, then see if we need to
                // include that character too. We may have short circuited because
                // we reached the start of the stream.
                if (!is_valid_name_char_ex(*functionOrVarNamePos))
                    {
                    std::advance(functionOrVarNamePos, 1);
                    }
                functionName.assign(functionOrVarNamePos,
                                    std::next(startPos) - functionOrVarNamePos);
                const bool hasExtraneousParens{ functionName.empty() };
                remove_decorations(functionName);
                // If wrapped in a string CTOR (e.g., std::wstring), then skip it
                // and keep going backwards.
                // Or, if no function name probably means extraneous parentheses, so keep going.
                if (hasExtraneousParens ||
                    m_ctors_to_ignore.find(functionName) != m_ctors_to_ignore.cend())
                    {
                    startPos = std::min(startPos, functionOrVarNamePos);
                    // reset, the current open parenthesis isn't relevant
                    if (currentOpeningChar == L'(')
                        {
                        closeParenCount = 0;
                        }
                    else if (currentOpeningChar == L'{')
                        {
                        closeBraseCount = 0;
                        }
                    if (m_deprecated_string_macros.find(functionName) !=
                        m_deprecated_string_macros.cend())
                        {
                        deprecatedMacroEncountered = functionName;
                        }
                    functionName.clear();
                    // now we should be looking for a + operator, comma, or ( or { proceeding this
                    // (unless we are already on it because we stepped back too far
                    //      due to the string being inside of an empty parenthesis)
                    if (*startPos != L',' && *startPos != L'+' && *startPos != L'&' &&
                        *startPos != L'=')
                        {
                        quoteWrappedInCTOR = true;
                        }
                    if (!hasExtraneousParens)
                        {
                        std::advance(startPos, -1);
                        }
                    continue;
                    }
                // construction of a variable type that takes
                // non-localizable strings, just skip it entirely
                if (m_variable_types_to_ignore.find(functionName) !=
                    m_variable_types_to_ignore.cend())
                    {
                    break;
                    }

                if (functionName.length() > 0)
                    {
                    // see if function is actually a CTOR
                    if (variableInfo.m_name.empty() &&
                        m_localization_functions.find(functionName) ==
                            m_localization_functions.cend() &&
                        m_non_localizable_functions.find(functionName) ==
                            m_non_localizable_functions.cend() &&
                        m_internal_functions.find(functionName) == m_internal_functions.cend() &&
                        m_log_functions.find(functionName) == m_log_functions.cend() &&
                        functionOrVarNamePos >= startSentinel && !is_keyword(functionName))
                        {
                        readVarType();

                        if (variableInfo.m_type.length() > 0)
                            {
                            variableInfo.m_name = functionName;
                            functionName.clear();
                            }
                        }
                    // if function is being compared or assigned, then record that
                    // for later analyses
                    if (std::prev(startPos) > startSentinel)
                        {
                        std::advance(startPos, -1);
                        const wchar_t* operatorEnd{ std::next(startPos) };
                        // skip spaces (and "==" and "!=" tokens)
                        while (startPos > startSentinel &&
                               (static_cast<bool>(std::iswspace(*startPos)) || *startPos == L'=' ||
                                *startPos == L'!'))
                            {
                            std::advance(startPos, -1);
                            }
                        readOperator(std::next(startPos), operatorEnd);
                        }
                    if (is_i18n_function(functionName))
                        {
                        std::wstring tmpFunctionName;
                        variable_info tmpVariableInfo;
                        std::wstring tmpDeprecatedMacroEncountered;
                        size_t tmpParameterPosition;
                        read_var_or_function_name(startPos, startSentinel, tmpFunctionName,
                                                  tmpVariableInfo, tmpDeprecatedMacroEncountered,
                                                  tmpParameterPosition);
                        if ((m_review_styles & check_literal_l10n_string_comparison) &&
                            is_search_or_comparison_function(tmpFunctionName))
                            {
                            m_literal_localizable_strings_being_compared.emplace_back(
                                tmpFunctionName,
                                string_info::usage_info(
                                    string_info::usage_info::usage_type::function, tmpFunctionName,
                                    std::wstring{}, std::wstring{}),
                                m_file_name, get_line_and_column(startPos - m_file_start));
                            }
                        }
                    break;
                    }
                }
            // deal with variable assignments here
            // (note that comparisons (>=, <=, ==, !=) are handled as though this string
            //      is a parameter to a function.)
            else if (*startPos == L'=' && *std::next(startPos) != L'=' &&
                     startPos > startSentinel && *std::prev(startPos) != L'=' &&
                     *std::prev(startPos) != L'!' && *std::prev(startPos) != L'>' &&
                     *std::prev(startPos) != L'<')
                {
                const wchar_t* operatorEnd{ std::next(startPos) };
                std::advance(startPos, -1);
                // skip spaces (and "+=" tokens)
                while (startPos > startSentinel &&
                       (static_cast<bool>(std::iswspace(*startPos)) || *startPos == L'+'))
                    {
                    std::advance(startPos, -1);
                    }
                readOperator(std::next(startPos), operatorEnd);
                // skip array info
                if (startPos > startSentinel && *startPos == L']')
                    {
                    while (startPos > startSentinel && *startPos != L'[')
                        {
                        std::advance(startPos, -1);
                        }
                    std::advance(startPos, -1);
                    while (startPos > startSentinel && static_cast<bool>(std::iswspace(*startPos)))
                        {
                        std::advance(startPos, -1);
                        }
                    }
                functionOrVarNamePos = startPos;
                while (functionOrVarNamePos > startSentinel &&
                       is_valid_name_char_ex(*functionOrVarNamePos))
                    {
                    std::advance(functionOrVarNamePos, -1);
                    }
                // If we are on the start of the text, then see if we need to include that
                // character too. We may have short circuited because we reached the start
                // of the stream.
                if (!is_valid_name_char_ex(*functionOrVarNamePos))
                    {
                    std::advance(functionOrVarNamePos, 1);
                    }
                variableInfo.m_name.assign(functionOrVarNamePos,
                                           std::next(startPos) - functionOrVarNamePos);

                readVarType();

                if (variableInfo.m_name.length() > 0)
                    {
                    break;
                    }
                }
            else if (static_cast<bool>(std::iswspace(*startPos)))
                {
                std::advance(startPos, -1);
                }
            else if (quoteWrappedInCTOR &&
                     (*startPos == L',' || *startPos == L'+' || *startPos == L'&'))
                {
                quoteWrappedInCTOR = false;
                }
            else if (quoteWrappedInCTOR && *startPos != L',' && *startPos != L'+' &&
                     *startPos != L'&')
                {
                break;
                }
            // << stream operator in some languages.
            // Skip over it and skip over ')' in front of it if there is one
            // to allow things like:
            //     gDebug() << "message"
            else if (*startPos == L'<')
                {
                std::advance(startPos, -1);
                if (startPos > startSentinel && *startPos == L'<')
                    {
                    bool isFunctionCall{ false };
                    std::advance(startPos, -1);
                    while (startPos > startSentinel && static_cast<bool>(std::iswspace(*startPos)))
                        {
                        std::advance(startPos, -1);
                        }
                    // step over arguments to streamable function
                    if (startPos > startSentinel && *startPos == L')')
                        {
                        std::advance(startPos, -1);
                        while (startPos > startSentinel && *startPos != L'(')
                            {
                            std::advance(startPos, -1);
                            }
                        if (startPos > startSentinel)
                            {
                            std::advance(startPos, -1);
                            }
                        isFunctionCall = true;
                        }
                    functionOrVarNamePos = startPos;
                    while (functionOrVarNamePos > startSentinel &&
                           is_valid_name_char_ex(*functionOrVarNamePos))
                        {
                        std::advance(functionOrVarNamePos, -1);
                        }
                    // move back to valid starting character and return
                    if (!is_valid_name_char_ex(*functionOrVarNamePos))
                        {
                        std::advance(functionOrVarNamePos, 1);
                        }
                    if (isFunctionCall)
                        {
                        functionName.assign(functionOrVarNamePos,
                                            std::next(startPos) - functionOrVarNamePos);
                        // ignore localization related functions; in this case, it is the
                        // (temporary) string objects << operator being called,
                        // not the localization function
                        if (m_localization_functions.find(functionName) !=
                                m_localization_functions.cend() ||
                            m_non_localizable_functions.find(functionName) !=
                                m_non_localizable_functions.cend())
                            {
                            functionName.clear();
                            }
                        }
                    else
                        {
                        variableInfo.m_name.assign(functionOrVarNamePos,
                                                   std::next(startPos) - functionOrVarNamePos);
                        }

                    return functionOrVarNamePos;
                    }
                }
            else
                {
                if (*startPos == L',')
                    {
                    ++parameterPosition;
                    }
                else if (*startPos == L'+' || *startPos == L'?' || *startPos == L':')
                    {
                    const wchar_t* operatorEnd{ std::next(startPos) };
                    readOperator(startPos, operatorEnd);
                    }
                std::advance(startPos, -1);
                }
            }

        return functionOrVarNamePos;
        }

    //------------------------------------------------
    std::vector<std::wstring>
    i18n_review::convert_positional_cpp_printf(const std::vector<std::wstring>& printfCommands,
                                               std::wstring& errorInfo)
        {
        errorInfo.clear();

        const std::wregex positionalRegex{ L"^%([[:digit:]]+)[$](.*)" };

        std::map<long, std::wstring> positionalCommands;
        std::vector<std::wstring> adjustedCommands{ printfCommands };

        size_t nonPositionalCommands{ 0 };
        for (const auto& pc : printfCommands)
            {
            std::match_results<std::wstring::const_iterator> matches;
            if (std::regex_search(pc, matches, positionalRegex))
                {
                if (matches.size() >= 3)
                    {
                    // position will need to be zero-indexed
                    long position = std::wcstol(matches[1].str().c_str(), nullptr, 10) - 1;
                    const auto [insertionPos, inserted] = positionalCommands.insert(
                        std::make_pair(position, std::format(L"%{}", matches[2].str())));
                    // if positional argument is used more than once, make sure they are consistent
                    if (!inserted)
                        {
                        if (insertionPos->second != std::format(L"%{}", matches[2].str()))
                            {
#ifdef wxVERSION_NUMBER
                            errorInfo =
                                wxString::Format(_(L"('%s': positional argument provided more than "
                                                   "once, but with different data types.)"),
                                                 matches[0].str())
                                    .wc_string();
#else
                            errorInfo =
                                L"('" + matches[0].str() +
                                _DT(L"': positional argument provided more than once, but with "
                                    "different data types.)");
#endif
                            return std::vector<std::wstring>{};
                            }
                        }
                    };
                }
            else
                {
                ++nonPositionalCommands;
                }
            }

        // Fill output commands from positional arguments.
        // Note that you cannot mix positional and non-positional arguments
        // in the same printf string. If that is happening here, then the
        // non-positional ones will be thrown out and be recorded as an error later.
        if (positionalCommands.size())
            {
            if (nonPositionalCommands > 0)
                {
                errorInfo = _WXTRANS_WSTR(
                    L"(Positional and non-positional commands mixed in the same printf string.)");
                }
            adjustedCommands.clear();
            for (auto& posArg : positionalCommands)
                {
                adjustedCommands.push_back(std::move(posArg.second));
                }
            }

        return adjustedCommands;
        }

    //------------------------------------------------
    std::vector<std::wstring> i18n_review::load_numbers(std::wstring_view resource)
        {
        std::vector<std::wstring> results;

        // This will grab numbers like "36 600" (non-breaking space only, not regular space),
        // "36,600", and "36.600".
        // Also, 7-bit, full-width, and Hindi numbers will be extracted.
        const std::wregex numberRegex{
            LR"([[:digit:]\u0966-\u096F\uFF10-\uFF19]+([\u00A0,\.][[:digit:]\u0966-\u096F\uFF10-\uFF19]+)*)"
        };
        // this will then normalize them all to 36600
        const std::wregex separatorsRegex{ LR"([\u00A0,\.])" };
        std::wstring_view::const_iterator searchStart{ resource.cbegin() };
        std::match_results<std::wstring_view::const_iterator> res;
        while (std::regex_search(searchStart, resource.cend(), res, numberRegex))
            {
            searchStart += res.position();
            if (searchStart == resource.cbegin() || *std::prev(searchStart) != L'%')
                {
                const bool isPrintfOrPositional{ (
                    std::distance(resource.cbegin(), searchStart) >= 2 &&
                    // ignore what may be printf or positional commands, not user-facing numbers
                    *std::prev(searchStart) == L'.' &&
                    (*std::prev(searchStart, 2) == L'$' || *std::prev(searchStart, 2) == L'%')) };
                const bool isHexPrefix{
                    (std::next(searchStart) < resource.cend()) && *searchStart == L'0' &&
                    (*std::next(searchStart) == L'x' || *std::next(searchStart) == L'X')
                };
                if (!isPrintfOrPositional && !isHexPrefix)
                    {
                    // remove all decimal and thousand separators and
                    // convert full-width numbers to 7-bit numbers
                    std::wstring strippedText{ std::regex_replace(res.str(0), separatorsRegex,
                                                                  L"") };
                    std::for_each(strippedText.begin(), strippedText.end(),
                                  [](wchar_t& chr)
                                  {
                                      chr = i18n_string_util::devanagari_number_to_7bit(
                                          i18n_string_util::full_width_number_to_7bit(chr));
                                  });
                    results.push_back(std::move(strippedText));
                    }
                }
            searchStart += res.length();
            }
        std::sort(results.begin(), results.end());
        return results;
        }

    //------------------------------------------------
    std::vector<std::wstring> i18n_review::load_matches(std::wstring_view resource,
                                                        const std::wregex& regEx)
        {
        std::vector<std::wstring> results;

        std::wstring_view::const_iterator searchStart{ resource.cbegin() };
        std::match_results<std::wstring_view::const_iterator> res;
        while (std::regex_search(searchStart, resource.cend(), res, regEx))
            {
            searchStart += res.position() + res.length();
            results.push_back(res.str(0));
            }
        std::sort(results.begin(), results.end());
        return results;
        }

    //------------------------------------------------
    std::vector<std::wstring> i18n_review::load_positional_commands(std::wstring_view resource)
        {
        std::vector<std::wstring> results;

        static const std::wregex positionalRegex{ L"[%][L]?[0-9]{1,}" };
        std::wstring_view::const_iterator searchStart{ resource.cbegin() };
        std::match_results<std::wstring_view::const_iterator> res;
        while (std::regex_search(searchStart, resource.cend(), res, positionalRegex))
            {
            searchStart += res.position() + res.length();
            results.push_back(res.str(0));
            }
        std::sort(results.begin(), results.end());
        return results;
        }

    //------------------------------------------------
    std::vector<std::wstring> i18n_review::load_cpp_printf_commands(std::wstring_view resource,
                                                                    std::wstring& errorInfo)
        {
        std::vector<std::pair<size_t, std::wstring>> results;

        // we need to do this multi-pass because a single regex command for all printf
        // commands is too complex and will cause the regex library to randomly throw exceptions
        std::wstring_view::const_iterator searchStart(resource.cbegin());
        std::match_results<std::wstring_view::const_iterator> res;
        size_t commandPosition{ 0 };
        size_t previousLength{ 0 };
        while (std::regex_search(searchStart, resource.cend(), res, m_printf_cpp_int_regex))
            {
            searchStart += res.position() + res.length();
            commandPosition += res.position() + previousLength;
            previousLength = res.length();

            results.push_back(std::make_pair(commandPosition, res.str(2)));
            }

        searchStart = resource.cbegin();
        commandPosition = previousLength = 0;
        while (std::regex_search(searchStart, resource.cend(), res, m_printf_cpp_float_regex))
            {
            searchStart += res.position() + res.length();
            commandPosition += res.position() + previousLength;
            previousLength = res.length();

            results.push_back(std::make_pair(commandPosition, res.str(2)));
            }

        searchStart = resource.cbegin();
        commandPosition = previousLength = 0;
        while (std::regex_search(searchStart, resource.cend(), res, m_printf_cpp_string_regex))
            {
            searchStart += res.position() + res.length();
            commandPosition += res.position() + previousLength;
            previousLength = res.length();

            results.push_back(std::make_pair(commandPosition, res.str(2)));
            }

        searchStart = resource.cbegin();
        commandPosition = previousLength = 0;
        while (std::regex_search(searchStart, resource.cend(), res, m_printf_cpp_pointer_regex))
            {
            searchStart += res.position() + res.length();
            commandPosition += res.position() + previousLength;
            previousLength = res.length();

            results.push_back(std::make_pair(commandPosition, res.str(2)));
            }

        // sort by position
        std::sort(results.begin(), results.end(),
                  [](const auto& lhv, const auto& rhv) noexcept { return lhv.first < rhv.first; });

        std::vector<std::wstring> finalStrings;
        finalStrings.reserve(results.size());
        for (auto& result : results)
            {
            finalStrings.push_back(std::move(result.second));
            }

        return convert_positional_cpp_printf(finalStrings, errorInfo);
        }

    //--------------------------------------------------
    std::vector<std::pair<size_t, size_t>>
    i18n_review::load_file_filter_positions(const std::wstring& resource)
        {
        std::vector<std::pair<size_t, size_t>> results;

        std::wstring::const_iterator searchStart{ resource.cbegin() };
        std::wsmatch res;
        size_t commandPosition{ 0 };
        size_t previousLength{ 0 };
        while (std::regex_search(searchStart, resource.cend(), res, m_file_filter_regex))
            {
            searchStart += res.position() + res.length();
            commandPosition += res.position() + previousLength;
            previousLength = res.length();

            results.push_back(std::make_pair(commandPosition, res.length()));
            }

        // sort by position
        std::sort(results.begin(), results.end(),
                  [](const auto& lhv, const auto& rhv) noexcept { return lhv.first < rhv.first; });

        return results;
        }

    //--------------------------------------------------
    std::vector<std::pair<size_t, size_t>>
    i18n_review::load_cpp_printf_command_positions(const std::wstring& resource)
        {
        std::vector<std::pair<size_t, size_t>> results;

        // we need to do this multi-pass because a single regex command for all printf
        // commands is too complex and will cause the regex library to randomly throw exceptions
        std::wstring::const_iterator searchStart{ resource.cbegin() };
        std::wsmatch res;
        size_t commandPosition{ 0 };
        size_t previousLength{ 0 };
        while (std::regex_search(searchStart, resource.cend(), res, m_printf_cpp_int_regex))
            {
            // the position and length of the actual specifier, not the junk in front of it
            searchStart += res.position(2) + res.length(2);
            commandPosition += res.position(2) + previousLength;
            previousLength = res.length(2);

            results.push_back(std::make_pair(commandPosition, res.length(2)));
            }

        searchStart = resource.cbegin();
        commandPosition = previousLength = 0;
        while (std::regex_search(searchStart, resource.cend(), res, m_printf_cpp_float_regex))
            {
            searchStart += res.position(2) + res.length(2);
            commandPosition += res.position(2) + previousLength;
            previousLength = res.length(2);

            results.push_back(std::make_pair(commandPosition, res.length(2)));
            }

        searchStart = resource.cbegin();
        commandPosition = previousLength = 0;
        while (std::regex_search(searchStart, resource.cend(), res, m_printf_cpp_string_regex))
            {
            searchStart += res.position(2) + res.length(2);
            commandPosition += res.position(2) + previousLength;
            previousLength = res.length(2);

            results.push_back(std::make_pair(commandPosition, res.length(2)));
            }

        searchStart = resource.cbegin();
        commandPosition = previousLength = 0;
        while (std::regex_search(searchStart, resource.cend(), res, m_printf_cpp_pointer_regex))
            {
            searchStart += res.position(2) + res.length(2);
            commandPosition += res.position(2) + previousLength;
            previousLength = res.length(2);

            results.push_back(std::make_pair(commandPosition, res.length(2)));
            }

        // sort by position
        std::sort(results.begin(), results.end(),
                  [](const auto& lhv, const auto& rhv) noexcept { return lhv.first < rhv.first; });

        return results;
        }

    //--------------------------------------------------
    std::vector<std::pair<size_t, size_t>>
    i18n_review::load_positional_command_positions(const std::wstring& resource)
        {
        std::vector<std::pair<size_t, size_t>> results;

        std::wstring::const_iterator searchStart{ resource.cbegin() };
        std::wsmatch res;
        size_t commandPosition{ 0 };
        size_t previousLength{ 0 };
        while (std::regex_search(searchStart, resource.cend(), res, m_positional_command_regex))
            {
            searchStart += res.position() + res.length();
            commandPosition += res.position() + previousLength;
            previousLength = res.length();

            results.push_back(std::make_pair(commandPosition, res.length()));
            }

        // sort by position
        std::sort(results.begin(), results.end(),
                  [](const auto& lhv, const auto& rhv) noexcept { return lhv.first < rhv.first; });

        return results;
        }

    //------------------------------------------------
    std::tuple<bool, std::wstring_view, size_t>
    i18n_review::read_po_catalog_entry(std::wstring_view& poFileText)
        {
        const size_t entryPos = poFileText.find(L"\n#");
        if (entryPos == std::wstring_view::npos)
            {
            return { false, std::wstring_view{}, std::wstring_view::npos };
            }
        poFileText.remove_prefix(entryPos);

        // find the next blank line, which is the separator between catalog entries
        size_t endOfEntryPos{ 0 };
        while (endOfEntryPos != std::wstring_view::npos)
            {
            endOfEntryPos = poFileText.find(L'\n', endOfEntryPos);
            // we must be at the last entry
            if (endOfEntryPos == std::wstring_view::npos ||
                endOfEntryPos == poFileText.length() - 1)
                {
                return { true, poFileText, entryPos };
                }
            ++endOfEntryPos;
            // eat up whitespace on line
            while (endOfEntryPos < poFileText.length() - 1 &&
                   string_util::is_either(poFileText[endOfEntryPos], L'\t', L' '))
                {
                ++endOfEntryPos;
                }
            // stop if we encountered a blank line (with or without empty whitespace in it)
            if (endOfEntryPos == poFileText.length() - 1 ||
                string_util::is_either(poFileText[endOfEntryPos], L'\r', L'\n'))
                {
                break;
                }
            }
        return { true, poFileText.substr(0, endOfEntryPos), entryPos };
        }

    //------------------------------------------------
    std::tuple<bool, std::wstring, size_t, size_t>
    i18n_review::read_po_msg(std::wstring_view& poCatalogEntry, const std::wstring_view msgTag)
        {
        const size_t idPos = poCatalogEntry.find(msgTag);
        if (idPos == std::wstring_view::npos)
            {
            return { false, std::wstring{}, std::wstring::npos, std::wstring::npos };
            }
        // Step back to see if this is a previous translation (#|) or commented
        // out translation (#~).
        size_t lookBehindIndex{ idPos };
        while (lookBehindIndex > 0 &&
               string_util::is_neither(poCatalogEntry[lookBehindIndex], L'\r', L'\n'))
            {
            --lookBehindIndex;
            }
        if (poCatalogEntry[++lookBehindIndex] == L'#')
            {
            return { false, std::wstring{}, std::wstring::npos, std::wstring::npos };
            }

        poCatalogEntry.remove_prefix(idPos + msgTag.length());

        size_t idEndPos{ 0 };
        while (true)
            {
            idEndPos = poCatalogEntry.find(L'\"', idEndPos);
            if (idEndPos == std::wstring_view::npos)
                {
                return { false, std::wstring{}, std::wstring::npos, std::wstring::npos };
                }
            // skip escaped quotes
            if (idEndPos > 0 && poCatalogEntry[idEndPos - 1] == L'\\')
                {
                ++idEndPos;
                continue;
                }
            else
                {
                size_t lookAheadIndex{ idEndPos + 1 };
                // jump to next line
                while (lookAheadIndex < poCatalogEntry.length() &&
                       string_util::is_either(poCatalogEntry[lookAheadIndex], L'\r', L'\n'))
                    {
                    ++lookAheadIndex;
                    }
                // eat up leading spaces
                while (lookAheadIndex < poCatalogEntry.length() &&
                       string_util::is_either(poCatalogEntry[lookAheadIndex], L'\t', L' '))
                    {
                    ++lookAheadIndex;
                    }
                // if a quote, then this is still be part of the same string
                if (lookAheadIndex < poCatalogEntry.length() &&
                    poCatalogEntry[lookAheadIndex] == L'"')
                    {
                    idEndPos = lookAheadIndex + 1;
                    continue;
                    }
                break;
                }
            }
        const std::wstring msgId{ process_po_msg(poCatalogEntry.substr(0, idEndPos)) };

        poCatalogEntry.remove_prefix(idEndPos);

        return { true, msgId, idPos, idEndPos };
        }

    //--------------------------------------------------
    std::pair<size_t, size_t>
    i18n_review::get_line_and_column(size_t position,
                                     std::wstring_view fileStart /*= std::wstring_view*/) const
        {
        if (position == std::wstring::npos)
            {
            return std::make_pair(std::wstring::npos, std::wstring::npos);
            }

        const auto* startSentinel = (!fileStart.empty() ? fileStart.data() : m_file_start);
        if (startSentinel == nullptr)
            {
            return std::make_pair(std::wstring::npos, std::wstring::npos);
            }
        size_t nextLinePosition{ 0 };
        size_t lineCount{ 0 };
        while ((nextLinePosition = std::wcscspn(startSentinel, L"\r\n")) < position)
            {
            ++lineCount;
            if (nextLinePosition + 1 < position &&
                *std::next(startSentinel, static_cast<ptrdiff_t>(nextLinePosition)) == L'\r' &&
                *std::next(startSentinel, static_cast<ptrdiff_t>(nextLinePosition + 1)) == L'\n')
                {
                std::advance(startSentinel, nextLinePosition + 2);
                position -= nextLinePosition + 2;
                }
            else
                {
                std::advance(startSentinel, nextLinePosition + 1);
                position -= nextLinePosition + 1;
                }
            }
        // make one-indexed
        return std::make_pair(lineCount + 1, position + 1);
        }
    } // namespace i18n_check
