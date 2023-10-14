#!/usr/bin/python3
from pynq import Overlay
from pynq import Xlnk
import numpy as np
import time
xlnk=Xlnk()

ol=Overlay("20230308_Vit64x32_300MHz_WNS-1.284.bit")
for i in ol.ip_dict:
    print(i);
ol.download();
