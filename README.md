# ScrabbleSearch
Scrabble move enumerator using just C++17.

I wrote this code to help work on this challenge:
https://puzzling.stackexchange.com/questions/100614/largest-smallest-unattainable-score-in-scrabble

Writing code to make an efficient Scrabble move enumerator was tricker than I thought. I hacked this together, but it works really well. Hope it's useful for you.

Running the code as-is will print out a valid moves for every possible score for the next play in this configuration:  
![image](https://github.com/TomasSirgedas/ScrabbleSearch/assets/76455203/5eb6e3a1-2d80-405d-93d1-673a70294519)

Output:  
```
[x2373]  1  1M (E)a
[x7700]  2  1J aa
[x8765]  3  1I bAa
[x10631] 4  1I abA
[x12088] 5  1I aMa
[x11236] 6  1I iDE
[x8766]  7  1I aMA
[x8647]  8  1H aSEa
[x5387]  9  1H alAE
[x7373] 10  1I arEA(E)
[x5633] 11  1G cAESe
[x8443] 12  1G abASE
[x5242] 13  1I aSiD(E)
[x7435] 14  1G cADEe
[x4462] 15  1G alDEA
[x4763] 16  1H pESAd(E)
[x3025] 17  1F pESADe
[x3333] 18  1F mEDuSA
[x1686] 19  1H bEDAz(E)
[x2244] 20  1F cAMESe
[x1240] 21  1F aShAME
[x1908] 22  1H bEDAz(E)S
[x1502] 23  1F rEADMe
[x1355] 24  1F coMADE
[x 547] 25  1G SiAMEz(E)
[x 904] 26  1H MEdiA(E)
[x 406] 27  1F MEDuSA
[x 643] 28  1G bEDMAt(E)
[x 705] 29  1H bEjAD(E)
[x 555] 30 13J aMASs
[x 997] 31  1G sEASiD(E)
[x 819] 32  1G rESpAD(E)
[x 603] 33  1K aM(E)bA
[x 482] 34  1H dEMoD(E)
[x 494] 35  1H coMAD(E)
[x1161] 36  1H sESaM(E)
[x 669] 37  1H aShAM(E)
[x 177] 38  1G liMEAD(E)
[x 374] 39  1G wAESoM(E)
[x 293] 40  1G bEShAM(E)
[x 163] 41  1H rEMAD(E)S
[x 306] 42  1G EDAmaM(E)
[x 154] 43  1G acADEM(E)
[x 296] 44 13G A(L)DosES
[x 114] 45  O1 aMAS
[x 109] 46  1H DEfAM(E)S
[x 110] 47  O1 abASED
[x  75] 48 13G a(L)MuDES
[x 206] 49  O1 aSiDE
[x  50] 50 13G A(L)MnErS
[x  10] 51  O1 rEADdS
[x  78] 52 13G A(L)MonDS
[x  25] 53  O1 aMuSED
[x  16] 54 13G A(L)MuDES
[x  22] 55  O1 aMiDE
[x   5] 56  O1 AMuSED
[x  13] 57 15H d
[x  39] 58 15G ed
[x   2] 59  C4 ADMa(S)sES
[x  12] 60  C4 aDMA(S)sES
[x  32] 61 10B ADMirE(R)S
[x  97] 62 15G Ed
[x  61] 63 10B jEMaDA(R)S
[x  92] 64 13D MEDu(L)lAS
[x 172] 65 10B AMenDE(R)S
[x  45] 66 15E AbEd
[x 216] 67 10B MiDyEA(R)S
[x 325] 68 11B sESAMo(I)D
[x 144] 69 10B DefAME(R)S
[x 198] 70 13D MeDA(L)EtS
[x 220] 71 15E iDEs
[x 194] 72 15D bAbES
[x 279] 73 15D blAES
[x 355] 74 13A SoMEDeA(L)
[x 166] 75 13A SoMeDEA(L)
[x 226] 76  1E MEDuSAe
[x 350] 77  1E aMiDASE
[x 238] 78 15A MeDuSAE
[x 374] 79 15A MiSmADE
[x 401] 80  1G DAEMon(E)S
[x 266] 81  1G bEDMAt(E)S
[x 146] 82 13C DiSMA(L)Er
[x 176] 83 13B SEMibA(L)D
[x 236] 84 15A aMiDASE
[x 139] 85 15C bAsSED
[x 126] 86 11D diAM(I)DES
[x 109] 87 15C gAmMES
[x  88] 88 15A miSMADE
[x  83] 89 15C MASheD
[x 114] 90  1G MAEnaD(E)S
[x  49] 91  1F StAMpED(E)
[x  39] 92 15C bEAMeD
[x  74] 93 15C bAlMED
[x  28] 94 15C AMiDeS
[x  20] 95  1F SEADroM(E)
[x  38] 96  1F StEpDAM(E)
[x   7] 97 13F be(L)DAMES
[x   4] 98 15C AMiDES
[x  11] 99  O9 miSMADE
[x   3]100 15C AMuSED
[x   2]101 13F bE(L)DAMeS
[x   1]102  O1 MADdESt
[x   6]103 13F Do(L)MAdES
[x   2]104  O1 aMAsSED
[x  15]105  8B (PSYCHOANALYZE)d
[x   9]106  O1 SpAsMED
[x   1]107  O1 DAMSEls
[x 132]108  8B (PSYCHOANALYZE)S
[x  52]109  O1 gADSMEn
[x 117]110  O6 aAs
[x 119]111  8B (PSYCHOANALYZE)D
[x 138]112  O1 AMiDaSE
[x 198]113  O1 anADEMS
[x 162]114  O1 SeEDMAn
[x 174]115  O1 reADMES
[x 516]116  O5 aAhS
[x 488]117  O5 abAS
[x 203]118  O5 bADs
[x 488]119  O5 aDdS
[x 214]120  O5 abASE
[x 377]121  O5 adDS
[x 412]122  O5 aMaS
[x 247]123  O5 abED
[x 260]124  O5 bADS
[x 639]125  O5 aiMS
[x 352]126  O5 abASED
[x 196]127  O5 wADSEt
[x 189]128  O5 aMiD
[x 199]129  O5 blADES
[x 119]130  O5 ArEDeS
[x 207]131  O5 aMiDE
[x 147]132  O5 AMiD
[x 198]133 15H dEMAnDS
[x 159]134  O5 aMiDES
[x 119]135 15H diADEMS
[x 101]136 15H rAnDEMS
[x 264]137 15B MArDiES
[x 147]138 15H reMADES
[x 137]139 15B AMenDES
[x 144]140 15B AMaSsED
[x  76]141 15B eMbASED
[x  58]142 15B rEADMeS
[x  98]143 15B aMASsED
[x  28]144 15B aMAsSED
[x  27]145 15B StEAMeD
[x 105]146 15B chASMED
[x  40]147 15B rEMADeS
[x  67]148 15B noMADES
[x  45]149 15B SAMbaED
[x  33]150 15B vAMoSED
[x  38]151  O3 AsiDES
[x  30]152 15F SEDAriM
[x  30]153  O3 AdDiES
[x  19]154  O3 tSADEs
[x  44]155  O3 edEMAS
[x  81]156  1H SEMiD(E)Af
[x  49]157  1H SoMED(E)Al
[x  95]158  O3 ASMEar
[x  23]159  O3 aSMEAr
[x  14]160  O3 ADdErS
[x  22]161  O3 iDEAlS
[x  13]162  O3 aDAgES
[x   7]163  O4 DaMeS
[x   8]164  O3 EDeMAs
[x   4]165  O3 eDEMAs
[x   8]166  O3 ADeEMs
[x  18]167  O3 aDMASs
[x   3]168  1H DrEAM(E)rS
[x   9]169  O3 aDeEMS
[x   5]170  O3 aDEeMS
[x   3]171  O3 eDEMAS
[x   1]172  O3 ADeEMS
[x   1]173  O3 ADEeMS
[x   4]189  O5 aMAsSED
[x   8]190  O5 DiAdEMS
[x   7]191  O5 gADSMEn
[x  13]192  O5 aMASsED
[x   9]193  O5 aMiDASE
[x  22]194  O5 anADEMS
[x  15]195  O5 ApEDoMS
[x   7]196  O5 MAdDEnS
[x  12]197  O5 MiSDAtE
[x   2]198  O6 DErhAMS
[x  19]199  O6 miSMADE
[x   5]200  O6 DiSfAME
[x  16]201  O6 bEDAMnS
[x  12]202  O6 mADaMES
[x   6]203  O6 AdDeEMS
[x   4]204  O6 bEDlAMS
[x   3]205  O7 AdDEeMS
[x   6]206  O7 AdDeEMS
[x   4]207  O8 SiDEMAn
[x   3]208  O8 SeEDMAn
[x   7]210  O8 DefAMES
[x   4]211  O2 MAdDEnS
[x   3]212  O2 ADdeEMS
[x   2]213  O2 ADdEeMS
[x   1]214  O2 DESMAns
[x   2]215  O4 SMeArED
[x   1]217  O2 MADnESs
[x   4]218  O2 MiSlEAD
[x   6]219  O2 MADdEnS
[x   7]220  O2 bEDAMnS
[x   6]221  O2 maDAMES
[x   3]222  O2 bEDlAMS
[x   1]223  O2 AdDEeMS
```
