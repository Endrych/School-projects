#Projekt: 2. projekt ITY 2016/2017
#Autor:	David Endrych 
#Email: xendry02@stud.fit.vutbr.cz
#Datum:	03.03.2017
#Popis:	Makefile pro 2. projekt pro předmět ITY

NAME=proj2

all:
	latex $(NAME).tex
	latex $(NAME).tex # Potřeba kvůli labelům
	dvips -t a4 $(NAME).dvi
	ps2pdf $(NAME).ps
clean:
	-rm -f $(NAME).aux $(NAME).ps $(NAME).dvi $(NAME).log $(NAME).pdf
