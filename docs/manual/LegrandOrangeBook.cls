%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% The Legrand Orange Book
% LaTeX Class
% Version 3.1 (February 18, 2022)
%
% Authors:
% Vel (vel@latextemplates.com)
% Mathias Legrand (legrand.mathias@gmail.com)
% 
% This class originates from:
% https://www.LaTeXTemplates.com
%
% License:
% CC BY-NC-SA 4.0 (https://creativecommons.org/licenses/by-nc-sa/4.0/)
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% This file has been modified for Readability Studio manuals (August, 2024)
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% https://www.latextemplates.com/license-explanation
%
% LaTeX Templates License Explanation
%
% LaTeX Templates uses the CC BY-NC-SA 4.0 license for templates created exclusively for the website.
% It is not immediately obvious how this license should be applied to LaTeX templates where
% there are two main components: the template code and the compiled document PDF.
% This page aims to clarify any confusion and clearly explain what you can and can't do with LaTeX
% templates licensed using this license.
%
% What You Can't Do
%
% You may not use the template code for any commercial purpose.
% This may include (but is not limited to) receiving money for modifying it, including it as part
% of an automated document creation process/product and hosting it with the purpose of monetizing visitors/users.
% Essentially, the template code cannot be used to add significant value to a business activity.
%
% You may not change the license of the template.
%
% What You Can Do
%
% You may use the compiled document (i.e. PDF) resulting from typesetting the code for any purposes,
% including commercial, and you do not need an attribution in the document text to LaTeX Templates,
% although it is appreciated. Examples include writing your CV in a CV template and using it to apply
% for jobs or writing a book using a book template and selling the book online or physically.
% This clause does not override the first one above.
%
% You may modify the template code in any way you wish for your personal use and you do not need to
% make your changes publicly available.
%
% You may redistribute the code, whether as-is or including your changes, but you must attribute
% it to the LaTeX Templates website as the source and retain the template license.
% In practice, this means you need to keep the notice at the top of the template code and
% broadly list what changes you have made.
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%----------------------------------------------------------------------------------------
%	CLASS CONFIGURATION
%----------------------------------------------------------------------------------------

\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{LegrandOrangeBook}[2022/01/26 The Legrand Orange Book Class v3.0]

\DeclareOption*{\PassOptionsToClass{\CurrentOption}{book}} % Pass through any options to the base class
\ProcessOptions\relax % Process given options

\LoadClass{book} % Load the base class

%----------------------------------------------------------------------------------------
%	 REQUIRED PACKAGES And CONFIGURATIONS
%----------------------------------------------------------------------------------------

\usepackage{graphicx} % Required for including pictures
\graphicspath{{images/}} % Specifies the directory where pictures are stored

\usepackage{float} % Allows more precisely positioning floats e.g. \begin{figure}[H]

\usepackage[usenames, svgnames, table]{xcolor} % Required for defining and using custom colors

\usepackage{calc} % For simpler calculations - used for spacing the index letter headings correctly

\usepackage{changepage} % Required for temporarily indenting text blocks

\usepackage[toc]{appendix} % Required for the appendices chapters

\usepackage{etoolbox} % Required for conditional logic And easily changing commands

%----------------------------------------------------------------------------------------
%	FONTS
%----------------------------------------------------------------------------------------

\usepackage[utf8]{inputenc} % Required for inputting international characters
\usepackage[T1]{fontenc} % Output font encoding for international characters

\usepackage{microtype} % Improve typography

%----------------------------------------------------------------------------------------
%	HEADERS AND FOOTERS
%----------------------------------------------------------------------------------------

\usepackage{fancyhdr} % Required for customizing headers and footers
\pagestyle{fancy} % Enable the custom headers and footers

\renewcommand{\headrulewidth}{0.5pt} % Top horizontal rule thickness

\renewcommand{\chaptermark}[1]{\markboth{\sffamily\normalsize\bfseries\chaptername\ \thechapter.~#1}{}} % Redefine the styling of the current chapter name in the header
\renewcommand{\sectionmark}[1]{\markright{\sffamily\normalsize\thesection~#1}{}} % Redefine the styling of the current section name in the header

\fancyhf{} % Clear default headers and footers

\fancyhead[LE, RO]{\sffamily\thepage} % Header for left even pages and right odd pages

\fancyhead[LO]{\rightmark} % Header for left odd pages

\fancyhead[RE]{\leftmark} % Header for right even pages

\fancypagestyle{plain}{ % Style for when a plain pagestyle is specified
	\fancyhead{} % Clear headers
	\renewcommand{\headrulewidth}{0pt} % Remove header rule
}

\usepackage{emptypage} % This package removes headers and footers on empty pages between chapters

%----------------------------------------------------------------------------------------
%	SECTION STYLING
%----------------------------------------------------------------------------------------

\setcounter{secnumdepth}{3} % Number sections down to subsubsections, a value of 2 will number to subsections only and 1 to sections only

\usepackage[
	explicit, % Specify section names explicitly in \titleformat with #1
	newparttoc, % Required for writing the part toc entry number in a standard way (overriding default LaTeX behavior) so the titletoc package can style it
]{titlesec} % Required for modifying sections

%-------------------------------------------

% Command to create and output the parts pages
\newcommand{\outputpartpage}[2]{
	\begin{tikzpicture}[remember picture, overlay]
		\node[anchor=center, rectangle, fill=ocre!20, inner sep=0pt, minimum height=\paperheight, minimum width=\paperwidth] at (current page.center) {}; % Background color
		\node[anchor=north west, inner sep=0pt, xshift=\Gm@lmargin+0.5cm, yshift=-(\Gm@tmargin+0.5cm)] at (current page.north west) {\fontsize{220pt}{220pt}\selectfont\sffamily\bfseries\textcolor{ocre!40}{#1}}; % Part number
		\node[anchor=north east, inner sep=0pt, xshift=-(\Gm@rmargin+0.5cm), yshift=-(\Gm@tmargin+0.5cm)] at (current page.north east) {\fontsize{30pt}{30pt}\selectfont\sffamily\bfseries\textcolor{white}{\strut #2}}; % Part title
		\node[anchor=south east, inner sep=0pt, xshift=-(\Gm@rmargin+0.5cm), yshift=\Gm@bmargin+0.5cm] at (current page.south east) { % Mini table of contents
			\parbox[t][][t]{8.5cm}{ % Width of box holding the mini ToC
				\printcontents[part]{l}{0}{\setcounter{tocdepth}{0}} % Display the mini table of contents showing chapters and sections, change tocdepth to 2 to also show subsections or 0 to only show chapters
			}
		};
	\end{tikzpicture}
}

\titleformat
	{\part} % Section type being modified
	[block] % Shape type, can be: hang, block, display, runin, leftmargin, rightmargin, drop, wrap, frame
	{\startcontents[part]\outputpartpage{\thepart}{#1}} % Format of the whole section
	{} % Format of the section label
	{0em} % Space between the title and label
	{} % Code before the label
	[] % Code after the label

%-------------------------------------------

% Command to store the current chapter image to use for all future chapters, can be changed any time in the document and can be empty
\newcommand{\thechapterimage}{}
\newcommand{\chapterimage}[1]{\renewcommand{\thechapterimage}{#1}}

% Command to store the current whitespace above the chapter title box for all future chapters, can be changed at any time in the document
\newcommand{\thechapterspaceabove}{}
\newcommand{\chapterspaceabove}[1]{\renewcommand{\thechapterspaceabove}{#1}}

% Command to store the current whitespace below the chapter title box for all future chapters, can be changed at any time in the document
\newcommand{\thechapterspacebelow}{}
\newcommand{\chapterspacebelow}[1]{\renewcommand{\thechapterspacebelow}{#1}}

% Command to output the chapter title with optional background image
\newcommand{\outputchaptertitle}[2]{
	\begin{tikzpicture}[remember picture,overlay]
		% If a chapter image has been specified
		\expandafter\ifstrequal\expandafter{\thechapterimage}{}{}{
			% Output the chapter image
			\node[
				anchor=north west, % Anchor point on the image
				inner sep=0pt, % Inner padding
			] at (current page.north west) {\includegraphics[width=\paperwidth]{\thechapterimage}};
		}
		% Box with the chapter title
		\draw[anchor=west] (current page.north west) node [
			anchor=north west, % Anchor point of the chapter box
			xshift=\Gm@lmargin, % Horizontal distance from the left side of the page
			yshift=-\thechapterspaceabove, % Vertical distance from the top of the page
			line width=2pt, % Border line thickness
			rounded corners=10pt, % Rounded corners amount
			draw=ocre, % Border line color
			fill=black!5, % Box background color
			fill opacity=0.6, % Background transparency
			text opacity=1, % Text transparency
			inner ysep=12.5pt, % Inner vertical padding
			inner xsep=20pt, % Inner horizontal padding
			align=flush left, % Text alignment inside box
			text width=1.2\linewidth, % Box width (wider than page to go off the right side)
			font=\huge\sffamily\bfseries, % Font styling for chapter title
		] {\parbox{0.85\linewidth}{\strut\raggedright\expandafter\ifstrequal\expandafter{#1}{}{}{#1. }#2\vspace{-3pt}}}; % Chapter title in a fixed width box for wrapping
	\end{tikzpicture}
}

% Numbered chapters
\titleformat
	{\chapter} % Section type being modified
	[block] % Shape type, can be: hang, block, display, runin, leftmargin, rightmargin, drop, wrap, frame
	{\outputchaptertitle{\thechapter}{#1}} % Format of the whole section
	{} % Format of the section label
	{0cm} % Space between the title and label
	{} % Code before the label
	[] % Code after the label

% Unnumbered chapters
\titleformat
	{name=\chapter, numberless} % Section type being modified
	[block] % Shape type, can be: hang, block, display, runin, leftmargin, rightmargin, drop, wrap, frame
	{\outputchaptertitle{}{#1}} % Format of the whole section
	{} % Format of the section label
	{0cm} % Space between the title and label
	{} % Code before the label
	[] % Code after the label

\titlespacing*{\chapter}{0pt}{0pt}{\thechapterspacebelow} % Spacing around section titles, the order is: left, before and after

%-------------------------------------------

\titleformat
	{\section} % Section type being modified
	[block] % Shape type, can be: hang, block, display, runin, leftmargin, rightmargin, drop, wrap, frame
	{\bfseries\sffamily\Large} % Format of the whole section
	{\llap{\textcolor{ocre}{\thesection}\hspace{1em}}#1} % Format of the section label
	{0cm} % Space between the title and label
	{} % Code before the label
	[] % Code after the label
	
\titleformat
	{name=\section, numberless} % Section type being modified
	[block] % Shape type, can be: hang, block, display, runin, leftmargin, rightmargin, drop, wrap, frame
	{\bfseries\sffamily\Large} % Format of the whole section
	{#1} % Format of the section label
	{0cm} % Space between the title and label
	{} % Code before the label
	[] % Code after the label

\titlespacing*{\section}{0pt}{12pt}{6pt} % Spacing around section titles, the order is: left, before and after

%-------------------------------------------

\titleformat
	{\subsection} % Section type being modified
	[block] % Shape type, can be: hang, block, display, runin, leftmargin, rightmargin, drop, wrap, frame
	{\bfseries\sffamily\large} % Format of the whole section
	{\llap{\textcolor{ocre}{\thesubsection}\hspace{1em}}#1} % Format of the section label
	{0cm} % Space between the title and label
	{} % Code before the label
	[] % Code after the label
	
\titleformat
	{name=\subsection, numberless} % Section type being modified
	[block] % Shape type, can be: hang, block, display, runin, leftmargin, rightmargin, drop, wrap, frame
	{\bfseries\sffamily\large} % Format of the whole section
	{#1} % Format of the section label
	{0cm} % Space between the title and label
	{} % Code before the label
	[] % Code after the label

\titlespacing*{\subsection}{0pt}{12pt}{4pt} % Spacing around section titles, the order is: left, before and after

%-------------------------------------------

\titleformat
	{\subsubsection} % Section type being modified
	[block] % Shape type, can be: hang, block, display, runin, leftmargin, rightmargin, drop, wrap, frame
	{\bfseries\sffamily} % Format of the whole section
	{\llap{{\thesubsubsection}\hspace{1em}}#1} % Format of the section label
	{0cm} % Space between the title and label
	{} % Code before the label
	[] % Code after the label
	
\titleformat
	{name=\subsubsection, numberless} % Section type being modified
	[block] % Shape type, can be: hang, block, display, runin, leftmargin, rightmargin, drop, wrap, frame
	{\bfseries\sffamily} % Format of the whole section
	{#1} % Format of the section label
	{0cm} % Space between the title and label
	{} % Code before the label
	[] % Code after the label

\titlespacing*{\subsubsection}{0pt}{8pt}{4pt} % Spacing around section titles, the order is: left, before and after

%-------------------------------------------

\titleformat
	{\paragraph} % Section type being modified
	[runin] % Shape type, can be: hang, block, display, runin, leftmargin, rightmargin, drop, wrap, frame
	{\bfseries\sffamily} % Format of the whole section
	{} % Format of the section label
	{0cm} % Space between the title and label
	{#1} % Code before the label
	[] % Code after the label

\titlespacing*{\paragraph}{0pt}{0pt}{6pt} % Spacing around section titles, the order is: left, before and after

%----------------------------------------------------------------------------------------
%	TABLE OF CONTENTS
%----------------------------------------------------------------------------------------

\usepackage{titletoc} % Required for manipulating the table of contents

\setcounter{tocdepth}{2} % Display down to subsections in the table of contents

\contentsmargin{0cm} % Removes the default right margin

%------------------------------------------------

% Styling of numbered parts in the table of contents
\newcommand{\tocentrypartnumbered}[1]{%
	\setlength\fboxsep{0pt}% Remove box padding
	\contentslabel[%
		% Part number box
		\colorbox{ocre!20}{% Background color
			\strut\parbox[c][.7cm]{1.1cm}{% Box size
				\color{ocre!70}\Large\sffamily\bfseries\centering\thecontentslabel% Part number
			}%
		}%
		\hspace{4pt}%
		% Part title box
		\colorbox{ocre!40}{% Background color
			\strut\parbox[c][.7cm]{\linewidth-1.25cm}{% Box size
				\centering\Large\sffamily #1% Part title
			}%
		}%
	]{1.25cm}
}

% Styling of unnumbered parts in the table of contents
\newcommand{\tocentrypartunnumbered}[1]{%
	\setlength\fboxsep{0pt}% Remove box padding
	\contentslabel[%
		% Part title box
		\colorbox{ocre!40}{% Background color
			\strut\parbox[c][.7cm]{\linewidth}{% Box size
				\centering\Large\sffamily #1% Part title
			}%
		}%
	]{1.25cm}
}

\titlecontents{part} % Section type being modified
	[1.25cm] % Left indentation
	{\addvspace{20pt}\Large\sffamily\bfseries\hypersetup{linkcolor=black}} % Before code
	{\tocentrypartnumbered} % Formatting of numbered sections of this type
	{\tocentrypartunnumbered} % Formatting of numberless sections of this type
	{} % Formatting of the filler to the right of the heading and the page number
	[] % After code

%------------------------------------------------

\titlecontents{chapter} % Section type being modified
	[1.25cm] % Left indentation
	{\addvspace{12pt}\large\sffamily\bfseries\hypersetup{linkcolor=ocre}} % Before code
	{\color{ocre}\contentslabel[\Large\thecontentslabel]{1.25cm}} % Formatting of numbered sections of this type
	{} % Formatting of numberless sections of this type
	{\color{ocre!60}\normalsize\;\titlerule*[6pt]{.}\;\color{ocre}\thecontentspage} % Formatting of the filler to the right of the heading and the page number
	[] % After code

%------------------------------------------------

\titlecontents{section} % Section type being modified
	[1.25cm] % Left indentation
	{\addvspace{3pt}\sffamily\bfseries} % Before code
	{\contentslabel[\thecontentslabel]{1.25cm}} % Formatting of numbered sections of this type
	{} % Formatting of numberless sections of this type
	{~{\normalfont\titlerule*[6pt]{.}}~\thecontentspage} % Formatting of the filler to the right of the heading and the page number
	[] % After code

%------------------------------------------------

\titlecontents{subsection} % Section type being modified
	[1.25cm] % Left indentation
	{\addvspace{1pt}\sffamily\small} % Before code
	{\contentslabel[\thecontentslabel]{1.25cm}} % Formatting of numbered sections of this type
	{} % Formatting of numberless sections of this type
	{~\titlerule*[6pt]{.}~\thecontentspage} % Formatting of the filler to the right of the heading and the page number
	[] % After code
	
%------------------------------------------------

\titlecontents{subsubsection} % Section type being modified
	[1.75cm] % Left indentation
	{\sffamily\small} % Before code
	{\contentslabel[\thecontentslabel]{1.75cm}} % Formatting of numbered sections of this type
	{} % Formatting of numberless sections of this type
	{~\titlerule*[6pt]{.}~\thecontentspage} % Formatting of the filler to the right of the heading and the page number
	[] % After code

%------------------------------------------------

% List of figures entries
\titlecontents{figure} % Section type being modified
	[0cm] % Left indentation
	{\addvspace{2pt}\sffamily\small} % Before code
	{\thecontentslabel\hspace*{1em}} % Formatting of numbered sections of this type
	{} % Formatting of numberless sections of this type
	{~\titlerule*[6pt]{.}~\thecontentspage} % Formatting of the filler to the right of the heading and the page number
	[] % After code

%------------------------------------------------

% List of tables entries
\titlecontents{table} % Section type being modified
	[0cm] % Left indentation
	{\addvspace{2pt}\sffamily\small} % Before code
	{\thecontentslabel\hspace*{1em}} % Formatting of numbered sections of this type
	{} % Formatting of numberless sections of this type
	{~\titlerule*[6pt]{.}~\thecontentspage} % Formatting of the filler to the right of the heading and the page number
	[] % After code

%------------------------------------------------

\titlecontents{lchapter} % Chapter headings on part pages
	[0cm] % Left indentation
	{\addvspace{12pt}\large\sffamily\bfseries\hypersetup{linkcolor=ocre}} % Before code
	{\color{ocre}\contentslabel[\Large\thecontentslabel]{1.25cm}} % Formatting of numbered sections of this type
	{\color{ocre}} % Formatting of numberless sections of this type
	{~\color{ocre!60}\normalsize\titlerule*[6pt]{.}~\color{ocre}\thecontentspage} % Formatting of the filler to the right of the heading and the page number

%------------------------------------------------

\titlecontents{lsection} % Section headings on part pages
	[0cm] % Left indentation
	{\sffamily\small} % Before code
	{\contentslabel[\thecontentslabel]{1.25cm}} % Formatting of numbered sections of this type
	{} % Formatting of numberless sections of this type
	{~{\normalfont\titlerule*[6pt]{.}}~\thecontentspage} % Formatting of the filler to the right of the heading and the page number

%------------------------------------------------

\titlecontents{lsubsection} % Subsection headings on part pages
	[0cm] % Left indentation
	{\sffamily\footnotesize} % Before code
	{\contentslabel[\thecontentslabel]{1.25cm}\hspace{0.5cm}} % Formatting of numbered sections of this type
	{} % Formatting of numberless sections of this type
	{~{\normalfont\titlerule*[6pt]{.}}~\thecontentspage} % Formatting of the filler to the right of the heading and the page number

%----------------------------------------------------------------------------------------
%	BOOKMARKS
%----------------------------------------------------------------------------------------

\usepackage{bookmark} % Required for customizing PDF bookmarks

\bookmarksetup{
	open % Open all bookmark drawers automatically
}
