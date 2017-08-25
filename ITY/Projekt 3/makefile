#Projekt: 3. projekt ITY 2016/2017
#Autor:	David Endrych 
#Email: xendry02@stud.fit.vutbr.cz
#Datum:	26.03.2017
#Popis:	Makefile pro 3. projekt pro předmět ITY

NAME=proj3

all:
	latex -sPAPERSIZE=a4 $(NAME).tex
	latex -sPAPERSIZE=a4 $(NAME).tex
	dvips -sPAPERSIZE=a4 -t a4 $(NAME).dvi
	ps2pdf -sPAPERSIZE=a4 $(NAME).ps
clean:
	-rm -f $(NAME).aux $(NAME).ps $(NAME).dvi $(NAME).log $(NAME).pdf
