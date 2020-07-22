changequote([,])dnl
define([gl_STRING_MODULE_INDICATOR],
  [
    dnl comment
    GNULIB_]translit([$1],[a-z],[A-Z])[=1
  ])dnl
  gl_STRING_MODULE_INDICATOR([strcase])

divert(1)dnl
dnl
dnl The dnl macro causes m4 to discard the rest of the line, thus
dnl preventing unwanted blank lines from appearing in the output.
dnl
H2(First Section)
H2(Second Section)
H2(Conclusion)
dnl
divert(0)dnl
dnl
<HTML>
undivert(1)dnl One of the queues is being pushed to output.
</HTML>
