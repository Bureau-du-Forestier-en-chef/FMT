call conda activate replanning
python Replanner.py -pri ../../Models/TWD_land/TWD_land.pri -str Globalex1 -tac Localex1 -sto Globalnofire -out OVOLREC,OSUPREC,OSUPPL,VOLINVENT -fol replanning_EX1 -rep 10 -len 20 -rel 20 -thr 2
PAUSE