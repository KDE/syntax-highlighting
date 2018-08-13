% Single-line comment
\name{foo}
#ifdef unix
\alias{bar}
#endif ignored
\title{Foo}
\description{
    We have a list
    \itemize error{
        \item foo,
        \item bar.
    }
    Also a table: \tabular{lr}{
        foo \tab \code{foo} \cr
        bar \tab \preformatted{bar}
    }
}
\section{Section}{
    Escaped braces don't need balance \}
    \subsection{Subsection}{
        \subsection{Subsubsection}{
            Sections can be nested.
        }
    }
}
\usage{
foo(a, b = default)
}
\newcommand{\macro}{Test#1}
\arguments{
  \item{a}{\if{html}{&pi; =} \Sexpr[stage=build]error{4*atan(1)}}
  \item{b}{\macro{text} \link[pkg:bar]{fooo}}
}
\seealso{
  \code{\link{bar}}
}
\examples{
# R comment
% Rdoc comment
foo(1)
for (i in 1:10)
    foo(i)
}
\keyword{foo}
