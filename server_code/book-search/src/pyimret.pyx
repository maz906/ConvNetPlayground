# distutils: language = c++
# distutils: sources = imgretrieval.cpp

cdef extern from "<string>" namespace "std":
	cdef cppclass string:
		string()

cdef extern from "<vector>" namespace "std":
	cdef cppclass vector[T]:
		vector()
		T& operator[](int)
		T& operator[](string)

cdef extern from "imgretrieval.h": #namespace "std":
	cdef cppclass imgRetrieval:
		imgRetrieval() except +
		vector[string] m_trainNames
		void readTrainData()
		void loadHKM() #
		void buildIVF() #
		int predictImg(string) #
		#void predictImg(string,vector[int]&) #
		
cdef class PyImgRetrieval:
	cdef imgRetrieval *thisptr      # hold a C++ instance which we're wrapping
	def __cinit__(self):
		self.thisptr = new imgRetrieval()
	def __dealloc__(self):
		del self.thisptr
	def readTrainData(self):
		self.thisptr.readTrainData()
	def loadHKM(self):
		self.thisptr.loadHKM()
	def buildIVF(self):
		self.thisptr.buildIVF()
	def predictImg(self,imgFile):#
		return self.thisptr.predictImg(imgFile)#
	property m_trainNames:
		def __get__(self): return self.thisptr.m_trainNames
	#def predictImg(self,imgFile,Ids):
	#	self.thisptr.predictImg(imgFile,Ids)
