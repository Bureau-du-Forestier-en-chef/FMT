call conda activate replanning
python Replanner.py -pri ../../Models/TWD_land/TWD_land.pri -str Globalreplanning -tac Localreplanning -sto Globalfire -out OVOLREC,OSUPREC -fol replanning -rep 100 -len 10 -rel 10 -thr 2 -com 5,5 8,10
PAUSE