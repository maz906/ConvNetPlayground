from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext
from Cython.Build import cythonize


setup(
	ext_modules = [
		Extension("pyimret",
			sources=["pyimret.pyx","imgretrieval.cpp","ccInvertedFile.cpp","ccDistance.cpp","ccHKmeans.cpp","ccKdt.cpp","ccLsh.cpp","ccNormalize.cpp"],
			language="c++",             # generate C++ code
			include_dirs=[".","source","/usr/local/include/opencv","/usr/local/include"],
			library_dirs=['/usr/local/lib','source'],
			libraries=['opencv_core','opencv_imgproc','opencv_highgui','opencv_features2d','opencv_flann','opencv_nonfree'])
	],
	cmdclass = {'build_ext': build_ext},
)

