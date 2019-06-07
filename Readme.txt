#######################################################
#Tes informations d'utilisateur
#Télécharger git pour windows
#https://git-scm.com/download/win
#Utiliser Git bash
#######################################################
git config --global user.name "Guillaume Cyr"
git config --global user.email "Guillaume.Cyr@fec.gouv.qc.ca"
#######################################################
#Pour pousser sur le git sur Artemis
#######################################################
#Aller dans cygwin
cd /C/Mrnmicro/Applic/Cygwin64/home/cyrgu3/FMT
#Création de ton repository
git init
#Sur quelle branche tu veux pousser
git checkout -b master
#Quand tu veux ajouter des changements (fichiers) à ton git (répertoir actuel)
git add Readme.txt
#Visual studio build stuff
git add BOOSTVS.bat
git add GDALVS.bat
git add Compile_zlib-1.2.11.txt
#Main source code
git add *.h
git add *.cpp
#Les info de ton commit
git commit -m "base"
#Finalement quand tu veux pousser ta pranche sur le repository sur Artemis
#Ici la branche master est spécifié
#Chaque version doit avoir sa propre branche / projets aussi
git push //Artemis/FECGEO/Donnees/Courant/Provincial/FMT master
#######################################################
#Pour prendre sur le git sur Artemis
#######################################################
#Ici on va chercher sur master
git pull //Artemis/FECGEO/Donnees/Courant/Provincial/FMT master 
#######################################################
#Pour le git sur Artemis ## ne pas faire rouler
#######################################################
cd //Artemis/FECGEO/Donnees/Courant/Provincial/FMT
#Création d'un GIT bare pour pouvoir pousser dessus
git init --bare
#Utile pour connaitre ce que tu envoi
git status
