import pyimret


myIR = pyimret.PyImgRetrieval()
myIR.readTrainData()
myIR.loadHKM()
myIR.buildIVF()
imgFile = 'test.jpg'
id = myIR.predictImg(imgFile)
print id
print myIR.m_trainNames[id]

