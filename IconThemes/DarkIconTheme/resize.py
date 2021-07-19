import cv2
import os
import cairosvg
import numpy as np

directory = "/home/trackingsetup/Documents/AnnotationTool/IconThemes/DarkIconTheme"

categories = ["actions", "emblems", "panels", "status"]
resolutions = [16,22,24,32,48,128,256,512]

for resolution in resolutions:
	os.makedirs(os.path.join(directory, str(resolution)+ "x" + str(resolution)), exist_ok=True)
	for category in categories:
		os.makedirs(os.path.join(directory, str(resolution)+ "x" + str(resolution), category),  exist_ok=True)
		for filename in os.listdir(os.path.join(directory, "scalable", category)):
			if filename.endswith(".svg"):
				cairosvg.svg2png(url=os.path.join(directory, "scalable", category, filename), write_to='temp.png', scale = 2)
				img  = cv2.imread("temp.png", cv2.IMREAD_UNCHANGED)
				if (img.shape[0] > img.shape[1]):
					diff = img.shape[0] - img.shape[1]
					img = cv2.copyMakeBorder(img, 0, 0, int(np.ceil(diff/2.0)), int(np.floor(diff/2.0)), cv2.BORDER_CONSTANT)
				else:
					diff = img.shape[1] - img.shape[0]
					img = cv2.copyMakeBorder(img, int(np.ceil(diff/2.0)), int(np.floor(diff/2.0)), 0, 0, cv2.BORDER_CONSTANT)
				img = cv2.resize(img, (resolution, resolution))
				filename = filename.split(".")[0]
				cv2.imwrite(os.path.join(directory, str(resolution)+ "x" + str(resolution), category, filename + ".png"), img)
