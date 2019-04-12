import ctypes

import time

def AddOverTime():
    ctypes.CDLL('librt.so', mode=ctypes.RTLD_GLOBAL)
    TestLib = ctypes.cdll.LoadLibrary('./shared_lib.so')
    for i in range(0, 200):
        TestLib.AddToMemory()
        time.sleep(1)

def ReadOverTime():
    ctypes.CDLL('librt.so', mode=ctypes.RTLD_GLOBAL)
    TestLib = ctypes.cdll.LoadLibrary('./shared_lib.so')
    for i in range(0, 200):
        print(TestLib.SharedMem())
        time.sleep(1)


if __name__ == "__main__":
    ReadOverTime()
