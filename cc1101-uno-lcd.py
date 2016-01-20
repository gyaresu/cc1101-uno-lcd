from rflib import *
import sys

def pwm_decode(p):
        biginteger = 0
        for byte in p:
                biginteger <<= 8
                biginteger |= ord(byte)
        biginteger >>= 12
        out = 0
        for i in range(28, (len(p)*8-12)/3, 1):
                out <<= 1
                out |= ((biginteger & 1) ^ 1)
                biginteger >>=3
        return out

d.setFreq(433e6)
d.setMdmModulation(MOD_ASK_OOK)
d.setPktPQT(0)
d.setMdmSyncMode(2)
d.setMdmSyncWord(0xEEEE)
d.setMdmNumPreamble(0)
d.setMdmDRate(115200)
d.setMaxPower()
d.makePktFLEN(5)

while not keystop():
    try:
        pkt, ts = d.RFrecv()
        #print pkt.encode('hex')#"0x%012x" % pwm_decode(pkt)
        #print "0x%012x" % pwm_decode(pkt)
        print "Received: %s" % pkt.encode("hex")
    except ChipconUsbTimeoutException:
        pass
sys.stdin.read(1)

