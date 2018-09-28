#!/usr/bin/env python3

# blurAndView.py
# Script for blurring and visualizing an image without touching the file on disk.
#

import cv2
import argparse
from utilsLib import FileSequence

parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
parser.add_argument("inputFiles", help="The input files to process", nargs='+')
parser.add_argument("-b", "--blur-radius", help="Radius of gaussian blur", dest="blurRadius", type=int, default=3)
args = parser.parse_args()

fileSequence = FileSequence.expandSequence(args.inputFiles)

for id, f in enumerate(fileSequence):
    print(">[%d] %s" %(id, f))
    img = cv2.imread(f)
    blurredImg = cv2.GaussianBlur(img, (args.blurRadius, args.blurRadius), 0)
    cv2.imshow("Blur(%d) : %d" %(args.blurRadius, id), blurredImg)
    cv2.waitKey(0)
    cv2.destroyAllWindows()

#eof
