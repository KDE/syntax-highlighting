// Inside the opened session:
a = sciargs()
i = find(a=="-args")
a(i+1)
evstr(a(i+2))*2
"This session has been launched from " + a(i+3)
