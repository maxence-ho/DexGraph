dot -Tps2 graph.dot -o tmp.ps
pstoedit tmp.ps -f gs:pdfwrite  graphs.pdf
rm tmp.ps
