\Problem name: CPLEX solver

Minimize
 obj: 5 minDayViol(0) + 2 singletonCheck(0,0,0) + 2 singletonCheck(0,0,1)
      + 2 singletonCheck(0,1,0) + 2 singletonCheck(0,1,1)
      + 2 singletonCheck(0,2,0) + 2 singletonCheck(0,2,1)
      + 2 singletonCheck(0,3,0) + 2 singletonCheck(0,3,1)
      + 2 singletonCheck(0,4,0) + 2 singletonCheck(0,4,1)
      + 2 singletonCheck(0,0,2) + 2 singletonCheck(0,0,3)
      + 2 singletonCheck(0,0,4) + 2 singletonCheck(0,0,5)
      + 2 singletonCheck(0,1,2) + 2 singletonCheck(0,1,3)
      + 2 singletonCheck(0,1,4) + 2 singletonCheck(0,1,5)
      + 2 singletonCheck(0,2,2) + 2 singletonCheck(0,2,3)
      + 2 singletonCheck(0,2,4) + 2 singletonCheck(0,2,5)
      + 2 singletonCheck(0,3,2) + 2 singletonCheck(0,3,3)
      + 2 singletonCheck(0,3,4) + 2 singletonCheck(0,3,5)
      + 2 singletonCheck(0,4,2) + 2 singletonCheck(0,4,3)
      + 2 singletonCheck(0,4,4) + 2 singletonCheck(0,4,5) + id367
Subject To
 id68:  x(0,0,0) + x(1,0,0) + x(2,0,0) + x(3,0,0) + x(4,0,0) + x(5,0,0)
        + x(6,0,0) + x(7,0,0) + x(8,0,0) + x(9,0,0) + x(10,0,0) + x(11,0,0)
        + x(12,0,0) + x(13,0,0) + x(14,0,0) + x(15,0,0) + x(16,0,0) + x(17,0,0)
        + x(18,0,0) + x(19,0,0) + x(20,0,0) + x(21,0,0) + x(22,0,0) + x(23,0,0)
        + x(24,0,0) + x(25,0,0) + x(26,0,0) + x(27,0,0) + x(28,0,0) + x(29,0,0)
         = 6
 id70:  x(0,0,0) <= 1
 id72:  x(1,0,0) <= 1
 id74:  x(2,0,0) <= 1
 id76:  x(3,0,0) <= 1
 id78:  x(4,0,0) <= 1
 id80:  x(5,0,0) <= 1
 id82:  x(6,0,0) <= 1
 id84:  x(7,0,0) <= 1
 id86:  x(8,0,0) <= 1
 id88:  x(9,0,0) <= 1
 id90:  x(10,0,0) <= 1
 id92:  x(11,0,0) <= 1
 id94:  x(12,0,0) <= 1
 id96:  x(13,0,0) <= 1
 id98:  x(14,0,0) <= 1
 id100: x(15,0,0) <= 1
 id102: x(16,0,0) <= 1
 id104: x(17,0,0) <= 1
 id106: x(18,0,0) <= 1
 id108: x(19,0,0) <= 1
 id110: x(20,0,0) <= 1
 id112: x(21,0,0) <= 1
 id114: x(22,0,0) <= 1
 id116: x(23,0,0) <= 1
 id118: x(24,0,0) <= 1
 id120: x(25,0,0) <= 1
 id122: x(26,0,0) <= 1
 id124: x(27,0,0) <= 1
 id126: x(28,0,0) <= 1
 id128: x(29,0,0) <= 1
 id130: x(0,0,0) <= 6
 id132: x(1,0,0) <= 6
 id134: x(2,0,0) <= 6
 id136: x(3,0,0) <= 6
 id138: x(4,0,0) <= 6
 id140: x(5,0,0) <= 6
 id142: x(6,0,0) <= 6
 id144: x(7,0,0) <= 6
 id146: x(8,0,0) <= 6
 id148: x(9,0,0) <= 6
 id150: x(10,0,0) <= 6
 id152: x(11,0,0) <= 6
 id154: x(12,0,0) <= 6
 id156: x(13,0,0) <= 6
 id158: x(14,0,0) <= 6
 id160: x(15,0,0) <= 6
 id162: x(16,0,0) <= 6
 id164: x(17,0,0) <= 6
 id166: x(18,0,0) <= 6
 id168: x(19,0,0) <= 6
 id170: x(20,0,0) <= 6
 id172: x(21,0,0) <= 6
 id174: x(22,0,0) <= 6
 id176: x(23,0,0) <= 6
 id178: x(24,0,0) <= 6
 id180: x(25,0,0) <= 6
 id182: x(26,0,0) <= 6
 id184: x(27,0,0) <= 6
 id186: x(28,0,0) <= 6
 id188: x(29,0,0) <= 6
 id190: x(0,0,0) - courseDays(0,0) <= 0
 id193: x(1,0,0) - courseDays(0,0) <= 0
 id196: x(2,0,0) - courseDays(0,0) <= 0
 id199: x(3,0,0) - courseDays(0,0) <= 0
 id202: x(4,0,0) - courseDays(0,0) <= 0
 id205: x(5,0,0) - courseDays(0,0) <= 0
 id208: x(6,0,0) - courseDays(0,1) <= 0
 id211: x(7,0,0) - courseDays(0,1) <= 0
 id214: x(8,0,0) - courseDays(0,1) <= 0
 id217: x(9,0,0) - courseDays(0,1) <= 0
 id220: x(10,0,0) - courseDays(0,1) <= 0
 id223: x(11,0,0) - courseDays(0,1) <= 0
 id226: x(12,0,0) - courseDays(0,2) <= 0
 id229: x(13,0,0) - courseDays(0,2) <= 0
 id232: x(14,0,0) - courseDays(0,2) <= 0
 id235: x(15,0,0) - courseDays(0,2) <= 0
 id238: x(16,0,0) - courseDays(0,2) <= 0
 id241: x(17,0,0) - courseDays(0,2) <= 0
 id244: x(18,0,0) - courseDays(0,3) <= 0
 id247: x(19,0,0) - courseDays(0,3) <= 0
 id250: x(20,0,0) - courseDays(0,3) <= 0
 id253: x(21,0,0) - courseDays(0,3) <= 0
 id256: x(22,0,0) - courseDays(0,3) <= 0
 id259: x(23,0,0) - courseDays(0,3) <= 0
 id262: x(24,0,0) - courseDays(0,4) <= 0
 id265: x(25,0,0) - courseDays(0,4) <= 0
 id268: x(26,0,0) - courseDays(0,4) <= 0
 id271: x(27,0,0) - courseDays(0,4) <= 0
 id274: x(28,0,0) - courseDays(0,4) <= 0
 id277: x(29,0,0) - courseDays(0,4) <= 0
 id280: x(0,0,0) + x(1,0,0) + x(2,0,0) + x(3,0,0) + x(4,0,0) + x(5,0,0)
        - courseDays(0,0) >= 0
 id282: x(6,0,0) + x(7,0,0) + x(8,0,0) + x(9,0,0) + x(10,0,0) + x(11,0,0)
        - courseDays(0,1) >= 0
 id284: x(12,0,0) + x(13,0,0) + x(14,0,0) + x(15,0,0) + x(16,0,0) + x(17,0,0)
        - courseDays(0,2) >= 0
 id286: x(18,0,0) + x(19,0,0) + x(20,0,0) + x(21,0,0) + x(22,0,0) + x(23,0,0)
        - courseDays(0,3) >= 0
 id288: x(24,0,0) + x(25,0,0) + x(26,0,0) + x(27,0,0) + x(28,0,0) + x(29,0,0)
        - courseDays(0,4) >= 0
 id291: x(0,0,0) + x(1,0,0) + x(2,0,0) + x(3,0,0) + x(4,0,0) + x(5,0,0)
        - minDayViol(0) <= 3
 id293: x(6,0,0) + x(7,0,0) + x(8,0,0) + x(9,0,0) + x(10,0,0) + x(11,0,0)
        - minDayViol(0) <= 3
 id295: x(12,0,0) + x(13,0,0) + x(14,0,0) + x(15,0,0) + x(16,0,0) + x(17,0,0)
        - minDayViol(0) <= 3
 id297: x(18,0,0) + x(19,0,0) + x(20,0,0) + x(21,0,0) + x(22,0,0) + x(23,0,0)
        - minDayViol(0) <= 3
 id299: x(24,0,0) + x(25,0,0) + x(26,0,0) + x(27,0,0) + x(28,0,0) + x(29,0,0)
        - minDayViol(0) <= 3
 id301: courseDays(0,0) + courseDays(0,1) + courseDays(0,2) + courseDays(0,3)
        + courseDays(0,4) + minDayViol(0) >= 4
 id302: courseDays(0,0) + courseDays(0,1) + courseDays(0,2) + courseDays(0,3)
        + courseDays(0,4) >= 1
 id303: minDayViol(0) <= 3
 id307: x(0,0,0) - x(1,0,0) - singletonCheck(0,0,0) <= 0
 id309: - x(4,0,0) + x(5,0,0) - singletonCheck(0,0,1) <= 0
 id311: x(6,0,0) - x(7,0,0) - singletonCheck(0,1,0) <= 0
 id313: - x(10,0,0) + x(11,0,0) - singletonCheck(0,1,1) <= 0
 id315: x(12,0,0) - x(13,0,0) - singletonCheck(0,2,0) <= 0
 id317: - x(16,0,0) + x(17,0,0) - singletonCheck(0,2,1) <= 0
 id319: x(18,0,0) - x(19,0,0) - singletonCheck(0,3,0) <= 0
 id321: - x(22,0,0) + x(23,0,0) - singletonCheck(0,3,1) <= 0
 id323: x(24,0,0) - x(25,0,0) - singletonCheck(0,4,0) <= 0
 id325: - x(28,0,0) + x(29,0,0) - singletonCheck(0,4,1) <= 0
 id327: - x(0,0,0) + x(1,0,0) - x(2,0,0) - singletonCheck(0,0,2) <= 0
 id329: - x(1,0,0) + x(2,0,0) - x(3,0,0) - singletonCheck(0,0,3) <= 0
 id331: - x(2,0,0) + x(3,0,0) - x(4,0,0) - singletonCheck(0,0,4) <= 0
 id333: - x(3,0,0) + x(4,0,0) - x(5,0,0) - singletonCheck(0,0,5) <= 0
 id335: - x(6,0,0) + x(7,0,0) - x(8,0,0) - singletonCheck(0,1,2) <= 0
 id337: - x(7,0,0) + x(8,0,0) - x(9,0,0) - singletonCheck(0,1,3) <= 0
 id339: - x(8,0,0) + x(9,0,0) - x(10,0,0) - singletonCheck(0,1,4) <= 0
 id341: - x(9,0,0) + x(10,0,0) - x(11,0,0) - singletonCheck(0,1,5) <= 0
 id343: - x(12,0,0) + x(13,0,0) - x(14,0,0) - singletonCheck(0,2,2) <= 0
 id345: - x(13,0,0) + x(14,0,0) - x(15,0,0) - singletonCheck(0,2,3) <= 0
 id347: - x(14,0,0) + x(15,0,0) - x(16,0,0) - singletonCheck(0,2,4) <= 0
 id349: - x(15,0,0) + x(16,0,0) - x(17,0,0) - singletonCheck(0,2,5) <= 0
 id351: - x(18,0,0) + x(19,0,0) - x(20,0,0) - singletonCheck(0,3,2) <= 0
 id353: - x(19,0,0) + x(20,0,0) - x(21,0,0) - singletonCheck(0,3,3) <= 0
 id355: - x(20,0,0) + x(21,0,0) - x(22,0,0) - singletonCheck(0,3,4) <= 0
 id357: - x(21,0,0) + x(22,0,0) - x(23,0,0) - singletonCheck(0,3,5) <= 0
 id359: - x(24,0,0) + x(25,0,0) - x(26,0,0) - singletonCheck(0,4,2) <= 0
 id361: - x(25,0,0) + x(26,0,0) - x(27,0,0) - singletonCheck(0,4,3) <= 0
 id363: - x(26,0,0) + x(27,0,0) - x(28,0,0) - singletonCheck(0,4,4) <= 0
 id365: - x(27,0,0) + x(28,0,0) - x(29,0,0) - singletonCheck(0,4,5) <= 0
Bounds
 0 <= x(0,0,0) <= 1
 0 <= x(1,0,0) <= 1
 0 <= x(2,0,0) <= 1
 0 <= x(3,0,0) <= 1
 0 <= x(4,0,0) <= 1
 0 <= x(5,0,0) <= 1
 0 <= x(6,0,0) <= 1
 0 <= x(7,0,0) <= 1
 0 <= x(8,0,0) <= 1
 0 <= x(9,0,0) <= 1
 0 <= x(10,0,0) <= 1
 0 <= x(11,0,0) <= 1
 0 <= x(12,0,0) <= 1
 0 <= x(13,0,0) <= 1
 0 <= x(14,0,0) <= 1
 0 <= x(15,0,0) <= 1
 0 <= x(16,0,0) <= 1
 0 <= x(17,0,0) <= 1
 0 <= x(18,0,0) <= 1
 0 <= x(19,0,0) <= 1
 0 <= x(20,0,0) <= 1
 0 <= x(21,0,0) <= 1
 0 <= x(22,0,0) <= 1
 0 <= x(23,0,0) <= 1
 0 <= x(24,0,0) <= 1
 0 <= x(25,0,0) <= 1
 0 <= x(26,0,0) <= 1
 0 <= x(27,0,0) <= 1
 0 <= x(28,0,0) <= 1
 0 <= x(29,0,0) <= 1
 0 <= courseDays(0,0) <= 1
 0 <= courseDays(0,1) <= 1
 0 <= courseDays(0,2) <= 1
 0 <= courseDays(0,3) <= 1
 0 <= courseDays(0,4) <= 1
 0 <= minDayViol(0) <= 1
 0 <= singletonCheck(0,0,0) <= 1
 0 <= singletonCheck(0,0,1) <= 1
 0 <= singletonCheck(0,1,0) <= 1
 0 <= singletonCheck(0,1,1) <= 1
 0 <= singletonCheck(0,2,0) <= 1
 0 <= singletonCheck(0,2,1) <= 1
 0 <= singletonCheck(0,3,0) <= 1
 0 <= singletonCheck(0,3,1) <= 1
 0 <= singletonCheck(0,4,0) <= 1
 0 <= singletonCheck(0,4,1) <= 1
 0 <= singletonCheck(0,0,2) <= 1
 0 <= singletonCheck(0,0,3) <= 1
 0 <= singletonCheck(0,0,4) <= 1
 0 <= singletonCheck(0,0,5) <= 1
 0 <= singletonCheck(0,1,2) <= 1
 0 <= singletonCheck(0,1,3) <= 1
 0 <= singletonCheck(0,1,4) <= 1
 0 <= singletonCheck(0,1,5) <= 1
 0 <= singletonCheck(0,2,2) <= 1
 0 <= singletonCheck(0,2,3) <= 1
 0 <= singletonCheck(0,2,4) <= 1
 0 <= singletonCheck(0,2,5) <= 1
 0 <= singletonCheck(0,3,2) <= 1
 0 <= singletonCheck(0,3,3) <= 1
 0 <= singletonCheck(0,3,4) <= 1
 0 <= singletonCheck(0,3,5) <= 1
 0 <= singletonCheck(0,4,2) <= 1
 0 <= singletonCheck(0,4,3) <= 1
 0 <= singletonCheck(0,4,4) <= 1
 0 <= singletonCheck(0,4,5) <= 1
      id367 = 0
Generals
 x(0,0,0)  x(1,0,0)  x(2,0,0)  x(3,0,0)  x(4,0,0)  x(5,0,0)  x(6,0,0) 
 x(7,0,0)  x(8,0,0)  x(9,0,0)  x(10,0,0)  x(11,0,0)  x(12,0,0)  x(13,0,0) 
 x(14,0,0)  x(15,0,0)  x(16,0,0)  x(17,0,0)  x(18,0,0)  x(19,0,0)  x(20,0,0) 
 x(21,0,0)  x(22,0,0)  x(23,0,0)  x(24,0,0)  x(25,0,0)  x(26,0,0)  x(27,0,0) 
 x(28,0,0)  x(29,0,0)  courseDays(0,0)  courseDays(0,1)  courseDays(0,2) 
 courseDays(0,3)  courseDays(0,4)  minDayViol(0)  singletonCheck(0,0,0) 
 singletonCheck(0,0,1)  singletonCheck(0,1,0)  singletonCheck(0,1,1) 
 singletonCheck(0,2,0)  singletonCheck(0,2,1)  singletonCheck(0,3,0) 
 singletonCheck(0,3,1)  singletonCheck(0,4,0)  singletonCheck(0,4,1) 
 singletonCheck(0,0,2)  singletonCheck(0,0,3)  singletonCheck(0,0,4) 
 singletonCheck(0,0,5)  singletonCheck(0,1,2)  singletonCheck(0,1,3) 
 singletonCheck(0,1,4)  singletonCheck(0,1,5)  singletonCheck(0,2,2) 
 singletonCheck(0,2,3)  singletonCheck(0,2,4)  singletonCheck(0,2,5) 
 singletonCheck(0,3,2)  singletonCheck(0,3,3)  singletonCheck(0,3,4) 
 singletonCheck(0,3,5)  singletonCheck(0,4,2)  singletonCheck(0,4,3) 
 singletonCheck(0,4,4)  singletonCheck(0,4,5) 
End
