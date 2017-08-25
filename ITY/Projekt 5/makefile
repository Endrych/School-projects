#Projekt: 5. projekt ITY 2016/2017
#Autor:	David Endrych 
#Email: xendry02@stud.fit.vutbr.cz
#Datum:	18.04.2017
#Popis:	Makefile pro 5. projekt pro předmět ITY

NAME=proj5

all:
	latex $(NAME).tex
	latex $(NAME).tex
	dvips $(NAME).dvi
	ps2pdf $(NAME).ps
clean:
	-rm -f $(NAME).aux $(NAME).ps $(NAME).dvi $(NAME).log $(NAME).pdf $(NAME).out $(NAME).nav $(NAME).snm $(NAME).toc
