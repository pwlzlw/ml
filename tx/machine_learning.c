/* Author: Pawel Zalewski
 * Date:   29/11/201
 *
 * The random forest implementation and feature extractor. 
 *
 */
#include "machine_learning.h"
#include <math.h>
#include <time.h>
//static uint32_t t0,t1,time,freq;
static Types_FreqHz frequency;


uint8_t IMA = 0, maX = 1, miY = 2, muY = 3, muX =4, meY= 5, meX=6, muZ = 7, meZ = 8, miZ = 9,
        vaZ =10, maY = 11, gmuX = 12;


static uint16_t* (*tr1[10])(float* ) = {stage1_t1,stage1_t2,stage1_t3,stage1_t4,stage1_t5,
                                         stage1_t6,stage1_t7,stage1_t8,stage1_t9,stage1_t10};

static uint16_t* (*tr2[10])(float* ) = {stage2_S_t1,stage2_S_t2,stage2_S_t3,stage2_S_t4,stage2_S_t5,
                                         stage2_S_t6,stage2_S_t7,stage2_S_t8,stage2_S_t9,stage2_S_t10};

static uint16_t* (*tr3[10])(float* ) = {stage2_MA_t1,stage2_MA_t2,stage2_MA_t3,stage2_MA_t4,stage2_MA_t5,
                                         stage2_MA_t6,stage2_MA_t7,stage2_MA_t8,stage2_MA_t9,stage2_MA_t10};

static uint16_t* (*tr3b[10])(float* ) = {stage2_MG_t1,stage2_MG_t2,stage2_MG_t3,stage2_MG_t4,stage2_MG_t5,
                                         stage2_MG_t6,stage2_MG_t7,stage2_MG_t8,stage2_MG_t9,stage2_MG_t10};

static uint16_t*  (*tr4[10])(float* ) = {stage2_R_t1,stage2_R_t2,stage2_R_t3,stage2_R_t4,stage2_R_t5,
                                         stage2_R_t6,stage2_R_t7,stage2_R_t8,stage2_R_t9,stage2_R_t10};

//assign probabilities
static void assignProb(uint16_t a, uint16_t b ,uint16_t c, uint16_t* hist) {
    hist[0] = a;
    hist[1] = b;
    hist[2] = c;
}
/* @fn computeFeatures - computes the features over the window, can observe 3 windows at a time
 * but this was not explored, co uld potentially do simple markov chains
 *  0 - first stage, 1 - rigorous, 2 - moderate */
void computeFV(uint8_t *windowKN1, float *output, uint8_t handle) {
    int16_t KN1_b16[SAMPLES_COUNT];
    intoInteger16(windowKN1 ,KN1_b16,0);
        float muz;
        if(handle == 0) {
            output[IMA] = getIMA(KN1_b16);     //IMA
            output[maX] = getMAX(KN1_b16, 0);  //maX
            output[miY] = getMIN(KN1_b16, 1);  //minY
            output[muY] = getMU(KN1_b16,1);   //muY
        }

        else if(handle == 1 ) {
            output[muX] = getMU(KN1_b16,0);  //muX
            output[meY] = getMED(KN1_b16,1);  //medY
            output[meX] = getMED(KN1_b16,0);  //medX
        }

        else if(handle == 2 ) {
            output[muZ] = getMU(KN1_b16,2); //muZ
            output[meZ] = getMED(KN1_b16,2);  //medZ
            output[miZ] = getMIN(KN1_b16,2);  //minZ
        }

        else if (handle == 3 ) {
            muz = getMU(KN1_b16,2);
            output[vaZ] = getVAR(KN1_b16,2,muz); //vaZ
            output[maY] = getMAX(KN1_b16, 1); //maY
        }
}

/* @fn stage1_RF*/
void stage1_RF(float* fv, uint16_t* ret) {
    //compute 10 tress
    uint8_t j;
    static uint32_t s = 0,m=0,r=0;
    static uint16_t* t1[10];

    //each tree returns a probability
    for(j=0;j<10;j++) {
        t1[j] = tr1[j](fv);
        m = m + (uint32_t)t1[j][0];
        r = r + (uint32_t)t1[j][1];
        s = s + (uint32_t)t1[j][2];
    }
    //we can tolerate some quantization
    s = (uint32_t)s/10;
    m = (uint32_t)m/10;
    r = (uint32_t)r/10;

    ret[0] = (uint16_t)s;
    ret[1] = (uint16_t)m;
    ret[2] = (uint16_t)r;
}
activity_t evalS1(uint16_t s, uint16_t m, uint16_t r) {
    if( s>m && s>r ) {
           return Sed;
       } else if( m > r && m > s) {
           return Mod;
       } else if( r> s && r> m){
           return Rig;
       } else if( r==s) {
           return Sed;
       } else if (m==r ) {
           return Mod;
       } else if(s==m) {
           srand(time(NULL));
           int i = rand() % 2;
           if(i == 0) {
               return Mod;
           } else {
              return Sed;
           }
       } else if(s==r) {
           srand(time(NULL));
           int i = rand() % 2;
           if(i == 0) {
               return Mod;
           } else {
               return Rig;
           }
       } else {
           return Sed;
       }
}
uint16_t* stage1_t1(float* fv) {
    static uint16_t hist[3];
    if(fv[IMA] < 5.40107f) {
          if(fv[IMA] < 3.51501f) {
              assignProb(93,0,9910, hist);
              return hist;
          } else {
              if(fv[miY] < 53.0f) {
                  assignProb(5970,0,4030, hist);
                  return hist;
              } else {
                  assignProb(247,0,9750, hist);
                  return hist;
              }
          }
    } else {
        if(fv[maX] < -96.5f) {
            if(fv[miY] < -112.5f) {
                assignProb(5970,4030,0, hist);
                return hist;
            } else {
                assignProb(9420,143,440, hist);
                return hist;
            }
        } else {
            assignProb(184,9570,249, hist);
            return hist;
        }
    }
}
uint16_t* stage1_t2(float* fv) {
    static uint16_t hist[3];
    if(fv[IMA] < 5.02559f) {
          if(fv[IMA] < 3.36779f) {
              assignProb(84,0,9920, hist);
              return hist;
          } else {
              assignProb(3200,0,6800, hist);
              return hist;
          }
    } else {
        if(fv[miY] < -88.5f) {
            if(fv[IMA] < 22.8025f) {
                assignProb(9220,522,261, hist);
                return hist;
            } else {
                assignProb(125,9870,0, hist);
                return hist;
            }
        } else {
            if(fv[maX] < -88.0f){
                assignProb(9450,110,442, hist);
                return hist;
            } else {
                assignProb(308,9250,447, hist);
                return hist;
            }
        }
    }
}
uint16_t* stage1_t3(float* fv) {
    static uint16_t hist[3];
    if(fv[miY] < -83.5f) {
          if(fv[maX] < -148.0f) {
              assignProb(9420,578,0, hist);
              return hist;
          } else {
              if(fv[IMA] < 30.5162f) {
                  assignProb(141,7320,2540, hist);
                  return hist;
              } else {
                  assignProb(52,9950,0, hist);
                  return hist;
              }
          }
    } else {
        if(fv[IMA] < 5.03995f) {
             assignProb(453,0,9550, hist);
            return hist;
        } else {
            if(fv[IMA] < 97.244f){
                assignProb(8510,852,640, hist);
                return hist;
            } else {
                assignProb(179,9820,3, hist);
                return hist;
            }
        }
    }
}
uint16_t* stage1_t4(float* fv) {
    static uint16_t hist[3];
    if(fv[miY] < -83.5f) {
          if(fv[miY] < -120.5f) {
              assignProb(161,9750,84, hist);
              return hist;
          } else {
              assignProb(3050,6840,113, hist);
              return hist;
          }
    } else {
        if(fv[maX] < -80.5f) {
            if(fv[IMA] < 4.11464f) {
                assignProb(806,0,9190, hist);
                return hist;
            } else {
                assignProb(9240,143,619, hist);
                return hist;
            }
        } else {
            if(fv[IMA] < 15.2902f){
                assignProb(0,15,9990, hist);
                return hist;
            } else {
                assignProb(354,9550,94, hist);
                return hist;
            }
        }
    }
}
uint16_t* stage1_t5(float* fv) {
    static uint16_t hist[3];
    if(fv[maX] < -102.0f) {
          if(fv[maX] < -231.5f) {
              if(fv[IMA] < 3.44547f) {
                  assignProb(354,0,9650, hist);
                  return hist;
              } else {
                  assignProb(6710,435,2850, hist);
                  return hist;
              }
          } else {
              if(fv[IMA] < 4.98987f) {
                assignProb(4080,0,5920, hist);
                return hist;
            } else {
                assignProb(9460,169,376, hist);
                return hist;
            }
          }
    } else {
        if(fv[IMA] < 15.0502f) {
                assignProb(0,0,10000, hist);
                return hist;
            } else {
                assignProb(196,9780,19, hist);
                return hist;
            }
    }
}
uint16_t* stage1_t6(float* fv) {
    static uint16_t hist[3];
    if(fv[IMA] < 5.39867f) {
          if(fv[IMA] < 3.65887f) {
              assignProb(152,0,9850, hist);
              return hist;
          } else {
              if(fv[miY] < 53.0f) {
                  assignProb(6140,0,3860, hist);
                  return hist;
              } else {
                  assignProb(133,0,9870, hist);
                  return hist;
              }

          }
    } else {
        if(fv[maX] < -96.5f) {
            if(fv[IMA] < 85.3761f) {
                assignProb(9490,130,384, hist);
                return hist;
            } else {
                assignProb(6390,3610,3, hist);
                return hist;
            }
        } else {
            assignProb(171,9590,239, hist);
            return hist;
        }
    }
}
uint16_t* stage1_t7(float* fv) {
    static uint16_t hist[3];
    if(fv[IMA] < 5.10852f) {
          if(fv[IMA] < 3.677580f) {
              assignProb(103,0,9900, hist);
              return hist;
          } else {
              assignProb(4190,0,5810, hist);
              return hist;
          }
    } else {
        if(fv[miY] < -88.5f) {
            if(fv[IMA] < 21.9529f) {
                assignProb(9460,272,272, hist);
                return hist;
            } else {
                assignProb(106,9890,0, hist);
                return hist;
            }
        } else {
            if(fv[muY] < 112.047f){
                assignProb(8420,1200,375, hist);
                return hist;
            } else {
                assignProb(992,8370,637, hist);
                return hist;
            }
        }
    }
}
uint16_t* stage1_t8(float* fv) {
    static uint16_t hist[3];
    if(fv[maX] < -96.5f) {
          if(fv[IMA] < 3.65436f) {
              assignProb(396,0,9600, hist);
              return hist;
          } else {
              if(fv[IMA] < 5.93216f) {
                  assignProb(6190,0,3810, hist);
                  return hist;
              } else {
                  assignProb(9510,170,317, hist);
                  return hist;
              }
          }
    } else {
        if(fv[IMA] < 15.2176f) {

                assignProb(0,10,9990, hist);
                return hist;

        } else {
            if(fv[miY] < 77.5f){
                assignProb(93,9890,20, hist);
                return hist;
            } else {
                assignProb(4860,4580,556, hist);
                return hist;
            }
        }
    }
}
uint16_t* stage1_t9(float* fv) {
    static uint16_t hist[3];
    if(fv[miY] < -94.5f) {
          if(fv[maX] < -132.0f) {
              assignProb(8660,1340,0, hist);
              return hist;
          } else {
              assignProb(25,9910,69, hist);
              return hist;
          }
    } else {
        if(fv[maX] < -80.5f) {
            if(fv[IMA] < 5.25302f) {
                assignProb(1230,0,8770, hist);
                return hist;
            } else {
                assignProb(9410,147,442, hist);
                return hist;
            }
        } else {
            if(fv[maX] < 81.5f){
                assignProb(185,1570,8240, hist);
                return hist;
            } else {
                assignProb(88,8610,1300, hist);
                return hist;
            }
        }
    }
}
uint16_t* stage1_t10(float* fv) {
    static uint16_t hist[3];
    if(fv[IMA] < 5.49369f) {
          if(fv[IMA] < 3.64359f) {
              assignProb(103,0,9900, hist);
              return hist;
          } else {
              if(fv[muY] < 61.8594f) {
                  assignProb(6750,0,3250, hist);
                  return hist;
              } else {
                  assignProb(593,0,9410, hist);
                  return hist;
              }
          }
    } else {
        if(fv[IMA] < 83.3393f) {
            if(fv[IMA] < 24.4877f) {
                assignProb(8950,316,738, hist);
                return hist;
            } else {
                assignProb(5500,4450,52, hist);
                return hist;
            }
        } else {

                assignProb(239,9760,0, hist);
                return hist;

        }
    }
}

/* @fn stage2_Sed_RF */
void stage2_S(float* fv, uint16_t* ret) {
    //compute 5 tress
    uint8_t j;
    static uint32_t s = 0,m=0,r=0;
    static uint16_t* t2[10];

    //each tree returns a probability
    for(j=0;j<10;j++) {
        t2[j] = tr2[j](fv);
        m = m + (uint32_t)t2[j][0]; //liy
        r = r + (uint32_t)t2[j][1]; //sit
        s = s + (uint32_t)t2[j][2]; //sta
    }

    //we can tolerate some quantization
    s = (uint32_t)s/10;
    m = (uint32_t)m/10;
    r = (uint32_t)r/10;

    ret[0] = s;
    ret[1] = m;
    ret[2] = r;
}
activity_t evalS2 (uint16_t s, uint16_t m, uint16_t r) {

    if( s>m && s>r ) {
        return Sta;
    } else if( m > r && m > s) {
        return Liy;
    } else if( r>m && r> s) {
        return Sit;
    } else if (s==m) {
        srand(time(NULL));
        int i = rand() % 2;
        if(i == 0) {
            return Liy;
        } else {
            return Sta;
        }
    } else if (s==r) {
        srand(time(NULL));
        int i = rand() % 2;
        if(i == 0) {
            return Sta;
        } else {
            return Sit;
        }
    } else if (m==r) {
        srand(time(NULL));
        int i = rand() % 2;
        if(i == 0) {
            return Liy;
        } else {
            return Sit;
        }
    } else {
        return Sit;
    }
}
uint16_t* stage2_S_t1(float* fv) {
    static uint16_t hist[3];
    if(fv[meX] < -130.f) {
        assignProb(0,0,10000, hist);
        return hist;
    } else {
        if(fv[muX] < 5.576563f) {
            if(fv[meY] < -33.75f) {
                assignProb(114,8860,0, hist);
                return hist;
            } else {
                assignProb(22,9980,0, hist);
                return hist;
            }
        } else {
            if(fv[meY] < 122.75f) {
                assignProb(9400,597,0, hist);
                return hist;
            } else {
                if(fv[muY] < 239.313f) {
                    assignProb(3120,6880,0, hist);
                    return hist;
                } else {
                    assignProb(10000,0,0, hist);
                    return hist;
                }
            }
        }
    }
}
uint16_t* stage2_S_t2(float* fv) {
    static uint16_t hist[3];
    if(fv[meX] < -138.f) {
        assignProb(0,0,10000, hist);
        return hist;
    } else {
        if(fv[meX] < 5.5f) {
            if(fv[meY] < -33.75f) {
                assignProb(629,9370,0, hist);
                return hist;
            } else {
                assignProb(0,10000,0, hist);
                return hist;
            }
        } else {
            if(fv[muY] < 122.156f) {
                assignProb(9220,776,0, hist);
                return hist;
            } else {
                if(fv[meX] < 17.25f) {
                    assignProb(9120,875,0, hist);
                    return hist;
                } else {
                    assignProb(761,9240,0, hist);
                    return hist;
                }
            }
        }
    }
}
uint16_t* stage2_S_t3(float* fv) {
    static uint16_t hist[3];
    if(fv[muX] < -122.391f) {
        assignProb(0,0,10000, hist);
        return hist;
    } else {
        if(fv[muX] < 5.67188f) {
            if(fv[meY] < -30.75f) {
                assignProb(329,9670,0, hist);
                return hist;
            } else {
                assignProb(0,10000,0, hist);
                return hist;
            }
        } else {
            if(fv[meY] < 121.75f) {
                assignProb(9410,558,0, hist);
                return hist;
            } else {
                if(fv[muY] < 236.25f) {
                    assignProb(3250,675,0, hist);
                    return hist;
                } else {
                    assignProb(10000,0,0, hist);
                    return hist;
                }
            }
        }
    }
}
uint16_t* stage2_S_t4(float* fv) {
    static uint16_t hist[3];
    if(fv[meX] < -122.391f) {
        assignProb(0,0,10000, hist);
        return hist;
    } else {
        if(fv[meX] < 5.75f) {
            if(fv[meY] < -32.5f) {
                assignProb(839,9160,0, hist);
                return hist;
            } else {
                assignProb(0,10000,0, hist);
                return hist;
            }
        } else {
            if(fv[muY] < 122.328f) {
                assignProb(9370,628,0, hist);
                return hist;
            } else {
                if(fv[muX] < 17.3594f) {
                    assignProb(9310,689,0, hist);
                    return hist;
                } else {
                    assignProb(784,9220,0, hist);
                    return hist;
                }
            }
        }
    }
}
uint16_t* stage2_S_t5(float* fv){
    static uint16_t hist[3];
    if(fv[meX] < -130.0f) {
        assignProb(0,0,10000, hist);
        return hist;
    } else {
        if(fv[meX] < 5.75f) {
            if(fv[muY] < -33.8594f) {
                assignProb(833,9170,0, hist);
                return hist;
            } else {
                assignProb(0,10000,0, hist);
                return hist;
            }
        } else {
            if(fv[muY] < 122.109f) {
                assignProb(9160,839,0, hist);
                return hist;
            } else {
                if(fv[meX] < 17.25f) {
                    assignProb(9340,664,0, hist);
                    return hist;
                } else {
                    assignProb(711,9290,0, hist);
                    return hist;
                }
            }
        }
    }
}
uint16_t* stage2_S_t6(float* fv) {
    static uint16_t hist[3];
    if(fv[meX] < -128.0f) {
        assignProb(0,0,10000, hist);
        return hist;
    } else {
        if(fv[muY] < 236.484f) {
            if(fv[muX] < 13.7656f) {
                if(fv[meY] < -33.75f) {
                    assignProb(745,9250,0, hist);
                    return hist;
                } else {
                    assignProb(0,10000,0, hist);
                    return hist;
                }
            } else {
                if(fv[meY] < 129.5f) {
                    assignProb(9550,454,0, hist);
                    return hist;
                } else {
                    assignProb(2660,7340,0, hist);
                    return hist;
                }
            }
        } else {

                assignProb(10000,0,0, hist);
                return hist;

        }
    }
}
uint16_t* stage2_S_t7(float* fv) {
    static uint16_t hist[3];
    if(fv[meX] < -130.0f) {
        assignProb(0,0,10000, hist);
        return hist;
    } else {
        if(fv[muX] < 5.8125f) {
            if(fv[meY] < -33.75f) {
                    assignProb(1030,8970,0, hist);
                    return hist;

            } else {
                    assignProb(0,10000,0, hist);
                    return hist;

            }
        } else {
            if(fv[muX] < 17.6563f) {
                assignProb(9310,687,0, hist);
                return hist;
            } else {
                if(fv[meX] < 67.5f) {
                    assignProb(2860,7140,0, hist);
                    return hist;
                } else {
                    assignProb(10000,0,0, hist);
                    return hist;
                }
            }
        }
    }
}
uint16_t* stage2_S_t8(float* fv) {
    static uint16_t hist[3];
    if(fv[muX] < -122.156f) {
        assignProb(0,0,10000, hist);
        return hist;
    } else {
        if(fv[muX] < 5.79688f) {
            if(fv[muY] < -33.2656f) {
                    assignProb(1120,8880,0, hist);
                    return hist;

            } else {
                    assignProb(22,9980,0, hist);
                    return hist;

            }
        } else {
            if(fv[muY] < 122.234f) {
                assignProb(9380,620,0, hist);
                return hist;
            } else {
                if(fv[muX] < 17.3906f) {
                    assignProb(9210,792,0, hist);
                    return hist;
                } else {
                    assignProb(582,9420,0, hist);
                    return hist;
                }
            }
        }
    }
}
uint16_t* stage2_S_t9(float* fv) {
    static uint16_t hist[3];
    if(fv[meX] < -128.0f) {
        assignProb(0,0,10000, hist);
        return hist;
    } else {
        if(fv[meX] < 5.75f) {
            if(fv[meY] < -32.5f) {
                    assignProb(702,9300,0, hist);
                    return hist;

            } else {
                    assignProb(0,10000,0, hist);
                    return hist;

            }
        } else {
            if(fv[muX] < 17.5781f) {
                assignProb(9100,899,0, hist);
                return hist;
            } else {
                if(fv[muY] < 123.203f) {
                    assignProb(9210,791,0, hist);
                    return hist;
                } else {
                    assignProb(248,9750,0, hist);
                    return hist;
                }
            }
        }
    }
}
uint16_t* stage2_S_t10(float* fv) {
    static uint16_t hist[3];
    if(fv[muY] < 109.531f) {
          if(fv[meX] < -135.0f) {
              assignProb(0,0,10000, hist);
              return hist;
          } else {
              if(fv[muX] < 9.65625f) {
                  assignProb(898,9910,0, hist);
                  return hist;
              } else {
                  assignProb(9780,215,0, hist);
                  return hist;
              }
          }
    } else {
        if(fv[muY] < 239.313f) {
            if(fv[meX] < 66.75f) {
                assignProb(2290,7610,106, hist);
                return hist;
            } else {
                assignProb(9650,350,0, hist);
                return hist;
            }
        } else {
            assignProb(10000,0,0, hist);
            return hist;
        }
    }
}
/* @fn stage2_ModA_RF  */
void stage2_MA(float* fv, uint16_t* ret) {
    //compute 5 tress
    uint8_t j;
    static uint32_t s = 0,m=0,r=0;
    static uint16_t* t3[10];

    //each tree returns a probability
    for(j=0;j<10;j++) {
        t3[j] = tr3[j](fv);
        m = m + (uint32_t)t3[j][0]; //tul
        r = r + (uint32_t)t3[j][1]; //tur
        s = s + (uint32_t)t3[j][2]; //wal
    }

    //we can tolerate some quantization
    s = (uint32_t)s/10;
    m = (uint32_t)m/10;
    r = (uint32_t)r/10;

    ret[0] = s;
    ret[1] = m;
    ret[2] = r;
}
activity_t evalMA(uint16_t s, uint16_t m, uint16_t r){
//biased towards the most likely from the statistical point of view..
    if( s>m && s>r ) {
        return Wal;
    } else if( m > r && m>s) {
        return TuL;
    } else if (r> m && r>s){
        return TuR;
    } else if (r == m) {
        srand(time(NULL));
        int i = rand() % 2;
        if(i == 0) {
            return TuL;
        } else {
            return TuR;
        }
    } else {
        return Wal;
    }
}
uint16_t* stage2_MA_t1(float* fv) { //check this one
    static uint16_t hist[3];
    if(fv[IMA] < 21.4196f) {
          if(fv[muZ] < 13.6094f) {
              assignProb(1730,8270,0, hist);
              return hist;
          } else {
              if(fv[muY] < -49.4063f) {
                  assignProb(2130,651,7220, hist);
                  return hist;
              } else {
                  assignProb(4780,4080,1140, hist);
                  return hist;
              }
          }
    } else {
        if(fv[miZ] < -37.0f) {
            if(fv[IMA] < 36.8197f) {
                assignProb(4830,4330,833, hist);
                return hist;
            } else {
                assignProb(19,381,9430, hist);
                return hist;
            }
        } else {
            assignProb(248,569,9180, hist);
            return hist;
        }
    }
}
uint16_t* stage2_MA_t2(float* fv) {
    static uint16_t hist[3];
    if(fv[IMA] < 22.7451f) {
          if(fv[muZ] < 13.3906f) {
              if(fv[muY] < 86.6719f){
                  assignProb(420,9580,0, hist);
                  return hist;
              } else {
                  assignProb(4000,6000,0, hist);
                  return hist;
              }
          } else {
              if(fv[muZ] < 40.7656f) {
                  assignProb(6650,1230,2130, hist);
                  return hist;
              } else {
                  assignProb(3860,4660,1480, hist);
                  return hist;
              }
          }
    } else {
        if(fv[miZ] < -37.5f) {
            assignProb(1890,1970,6140, hist);
            return hist;
        } else {
            assignProb(307,436,9260, hist);
            return hist;
        }
    }
}
uint16_t* stage2_MA_t3(float* fv) {
    static uint16_t hist[3];
    if(fv[IMA] < 21.4915f) {
          if(fv[muZ] < 16.0781f) {
              assignProb(2530,7440,29, hist);
              return hist;
          } else {
              if(fv[muY] < -51.15630f) {
                  assignProb(1640,246,8110, hist);
                  return hist;
              } else {
                  assignProb(5020,3800,1180, hist);
                  return hist;
              }
          }
    } else {
        if(fv[IMA] < 28.7147f) {
            if(fv[miZ] < 15.5f) {
                assignProb(2990,4110,2900, hist);
                return hist;
            } else {
                assignProb(112,1340,8550, hist);
                return hist;
            }
        } else {
            assignProb(214,142,9640, hist);
            return hist;
        }
    }
}
uint16_t* stage2_MA_t4(float* fv) {
    static uint16_t hist[3];
    if(fv[miZ] < 51.5f) {
          if(fv[IMA] < 25.1358f) {
              if(fv[muZ] < 11.5156f) {
                    assignProb(1870,8130,0, hist);
                    return hist;
                } else {
                    assignProb(4110,3810,2080, hist);
                    return hist;
                }

          } else {
              assignProb(306,306,939, hist);
              return hist;
          }
    } else {
        if(fv[IMA] < 10.1436f) {
            if(fv[muY] < 0.75f) {
                assignProb(3080,6920,0, hist);
                return hist;
            } else {
                assignProb(8970,1030,0, hist);
                return hist;
            }
        } else {
            assignProb(3740,5530,732, hist);
            return hist;
        }
    }
}
uint16_t* stage2_MA_t5(float* fv) {
    static uint16_t hist[3];
    if(fv[IMA] < 24.1333f) {
          if(fv[muY] < -52.9375f) {
              if(fv[muZ] < 33.6719f) {
                    assignProb(3620,3830,2550, hist);
                    return hist;
                } else {
                    assignProb(250,0,9750, hist);
                    return hist;
                }
          } else {
              if(fv[meZ] < 2.25f) {
                  assignProb(556,9440,0, hist);
                  return hist;
              } else {
                  assignProb(4820,3910,1270, hist);
                  return hist;
              }
          }
    } else {
        if(fv[miZ] < -37.5f) {
                assignProb(1620,1470,6910, hist);
                return hist;
            } else {
                assignProb(241,412,9350, hist);
                return hist;
            }
    }
}
uint16_t* stage2_MA_t6(float* fv) {
    static uint16_t hist[3];
    if(fv[IMA] < 21.5694f) {
          if(fv[muY] < -51.4844f) {
              if(fv[muZ] < 27.4375f) {
                    assignProb(2080,7920,0, hist);
                    return hist;
                } else {
                    assignProb(603,0,9400, hist);
                    return hist;
                }
          } else {
              if(fv[muZ] < 1.0625f) {
                  assignProb(250,9750,0, hist);
                  return hist;
              } else {
                  assignProb(4800,4230,971, hist);
                  return hist;
              }
          }
    } else {

            if(fv[IMA] < 28.5425f) {
                assignProb(1600,2600,5800, hist);
                return hist;
            } else {
                assignProb(198,162,9640, hist);
                return hist;
            }
    }
}
uint16_t* stage2_MA_t7(float* fv) {
    static uint16_t hist[3];
    if(fv[IMA] < 21.5694f) {
          if(fv[muZ] < 11.4688f) {
              assignProb(1810,8190,0, hist);
              return hist;
          } else {
              if(fv[muZ] < 40.2344f) {
                  assignProb(6470,1430,2100, hist);
                  return hist;
              } else {
                  assignProb(4130,4460,1400, hist);
                  return hist;
              }
          }
    } else {
        if(fv[IMA] < 26.9722f) {
            if(fv[muY] < 61.7969f) {
                assignProb(676,1490,7840, hist);
                return hist;
            } else {
                assignProb(5350,3520,1130, hist);
                return hist;
            }
        } else {
            assignProb(326,233,9440, hist);
            return hist;
        }
    }
}
uint16_t* stage2_MA_t8(float* fv) {
    static uint16_t hist[3];
    if(fv[IMA] < 25.136f) {
          if(fv[meZ] < -4.25f) {
              assignProb(84,9920,0, hist);
              return hist;
          } else {
              if(fv[IMA] < 16.2522f) {
                  assignProb(4900,4140,957, hist);
                  return hist;
              } else {
                  assignProb(2410,3570,4020, hist);
                  return hist;
              }
          }
    } else {
        if(fv[IMA] < 33.8361f) {
            if(fv[muZ] < 57.8281f) {
                assignProb(2400,3200,4400, hist);
                return hist;
            } else {
                assignProb(120,359,9520, hist);
                return hist;
            }
        } else {
            assignProb(84,84,9830, hist);
            return hist;
        }
    }
}
uint16_t* stage2_MA_t9(float* fv) {
    static uint16_t hist[3];
    if(fv[IMA] < 26.7043f) {
          if(fv[muZ] < 13.2813f) {
              if(fv[muZ] < -2.26563f) {
                    assignProb(73,9930,0, hist);
                    return hist;
                } else {
                    assignProb(3240,6760,0, hist);
                    return hist;
                }
          } else {
              if(fv[miZ] < 2.5f) {
                  assignProb(7660,1790,557, hist);
                  return hist;
              } else {
                  assignProb(3550,4120,2330, hist);
                  return hist;
              }
          }
    } else {

            if(fv[miZ] < -19.5f) {
                assignProb(1180,647,8180, hist);
                return hist;
            } else {
                assignProb(44,131,9830, hist);
                return hist;
            }

    }
}
uint16_t* stage2_MA_t10(float* fv) {
    static uint16_t hist[3];
    if(fv[IMA] < 24.1333f) {
          if(fv[muZ] < 16.5469f) {
              assignProb(2080,7850,63, hist);
              return hist;
          } else {
              if(fv[muY] < -46.8906f) {
                  assignProb(2190,428,7380, hist);
                  return hist;
              } else {
                  assignProb(4720,4040,1240, hist);
                  return hist;
              }
          }
    } else {
        if(fv[miZ] < -37.5f) {
            if(fv[muZ] < 5.25f) {
                assignProb(263,789,8950, hist);
                return hist;
            } else {
                assignProb(5630,2500,1880, hist);
                return hist;
            }
        } else {
            assignProb(222,381,9400, hist);
            return hist;
        }
    }
}
/* @fn stage2_ModG_RF */
void stage2_MG(float* fv, uint16_t * ret) {
    //compute 5 tress
    uint8_t j;
    static uint32_t s = 0,m=0,r=0;
    static uint16_t* t3[10];

    //each tree returns a probability
    for(j=0;j<10;j++) {
        t3[j] = tr3b[j](fv);
        m = m + (uint32_t)t3[j][0]; //tul
        r = r + (uint32_t)t3[j][1]; //tur
        s = s + (uint32_t)t3[j][2]; //wal
    }

    //we can tolerate some quantization
    s = (uint32_t)s/10;
    m = (uint32_t)m/10;
    r = (uint32_t)r/10;

    ret[0] = s;
    ret[1] = m;
    ret[2] = r;
}

activity_t evalMG(uint16_t s, uint16_t m, uint16_t r) {

    //biased towards the most likely from the statistical point of view..
    if( s>m && s>r ) {
        return Wal;
    } else if( m > r && m>s) {
        return TuL;
    } else if (r> m && r>s){
        return TuR;
    } else if (r == m) {
        srand(time(NULL));
        int i = rand() % 2;
        if(i == 0) {
            return TuL;
        } else {
            return TuR;
        }
    } else {
        return Wal;
    }
}
uint16_t* stage2_MG_t1(float* fv) {
    static uint16_t hist[3];
    if(fv[gmuX] < 2414.3f) {
          if(fv[gmuX] < -1091.06f) {
              if(fv[muZ] < 4.40625f) {
                assignProb(131,9150,719, hist);
                return hist;
            } else {
                assignProb(9780,62,160, hist);
                return hist;
            }
          } else {
              if(fv[gmuX] < -571.313f) {
                  assignProb(3810,0,6190, hist);
                  return hist;
              } else {
                  assignProb(431,507,9060, hist);
                  return hist;
              }
          }
    } else {

            if(fv[muY] < 84.7813f) {
                assignProb(46,9720,232, hist);
                return hist;
            } else {
                assignProb(8170,793,1040, hist);
                return hist;
            }
    }
}
uint16_t* stage2_MG_t2(float* fv) {
    static uint16_t hist[3];
    if(fv[gmuX] < 2391.13f) {
          if(fv[IMA] < 21.633f) {
              if(fv[gmuX] < -567.422f){
                  assignProb(8520,1270,211, hist);
                  return hist;
              } else {
                  assignProb(1030,1130,7840, hist);
                  return hist;
              }
          } else {
                  assignProb(443,235,9320, hist);
                  return hist;
          }
    } else {
        if(fv[muZ] < 39.0313f) {
            if(fv[muY] < 75.1563f) {
                assignProb(0,9810,194, hist);
                return hist;
            } else {
                assignProb(8670,301,1020, hist);
                return hist;
            }
        } else {
            assignProb(44,9660,294, hist);
            return hist;
        }
    }
}
uint16_t* stage2_MG_t3(float* fv) {
    static uint16_t hist[3];
    if(fv[gmuX] < -1076.3f) {
          if(fv[muZ] < 4.10938f) {
                  assignProb(74,8970,956, hist);
                  return hist;
              } else {
                  assignProb(9610,129,258, hist);
                  return hist;
              }

    } else {
        if(fv[muY] < 46.3281f) {
            if(fv[gmuX] < 2200.39f) {
                assignProb(569,921,8510, hist);
                return hist;
            } else {
                assignProb(0,9740,265, hist);
                return hist;
            }
        } else {
            if(fv[gmuX] < 3017.13f) {
                assignProb(831,262,8910, hist);
                return hist;
            } else {
                assignProb(3460,6280,254, hist);
                return hist;
            }
        }
    }
}
uint16_t* stage2_MG_t4(float* fv) {
    static uint16_t hist[3];
    if(fv[miZ] < 50.5f) {
          if(fv[muZ] < 59.6525f) {
              if(fv[IMA] < 29.6052f) {
                    assignProb(4290,4230,1470, hist);
                    return hist;
                } else {
                    assignProb(569,268,9160, hist);
                    return hist;
                }

          } else {
              if(fv[IMA] < 16.2113f) {
                  assignProb(3720,5460,816, hist);
                  return hist;
              } else {
                  assignProb(407,1120,8480, hist);
                  return hist;
              }
          }
    } else {

            if(fv[gmuX] < 1061.2f) {
                assignProb(9800,0,199, hist);
                return hist;
            } else {
                assignProb(0,9360,644, hist);
                return hist;
            }
    }
}
uint16_t* stage2_MG_t5(float* fv) {
    static uint16_t hist[3];
    if(fv[gmuX] < 2414.3f) {
          if(fv[gmuX] < -1091.06f) {
              if(fv[muZ] < 10.125f) {
                    assignProb(127,8920,955, hist);
                    return hist;
                } else {
                    assignProb(9720,109,169, hist);
                    return hist;
                }
          } else {
              if(fv[IMA] < 6.78427f) {
                  assignProb(6360,2470,1170, hist);
                  return hist;
              } else {
                  assignProb(235,306,9460, hist);
                  return hist;
              }
          }
    } else {
        if(fv[muY] < 85.3125f) {
                assignProb(62,9750,185, hist);
                return hist;
            } else {
                assignProb(8480,861,662, hist);
                return hist;
            }
    }
}
uint16_t* stage2_MG_t6(float* fv) {
    static uint16_t hist[3];
    if(fv[miZ] < 49.5f) {
          if(fv[gmuX] < 3089.39f) {
              if(fv[gmuX] < -2067.36f) {
                    assignProb(7850,2070,76, hist);
                    return hist;
                } else {
                    assignProb(341,467,9190, hist);
                    return hist;
                }
          } else {
              if(fv[miZ] < -22.5f) {
                  assignProb(4910,5030,57, hist);
                  return hist;
              } else {
                  assignProb(48,9410,111, hist);
                  return hist;
              }
          }
    } else {
        if(fv[IMA] < 11.5747f) {
            assignProb(7150,2850,0, hist);
            return hist;
        } else {
            assignProb(3000,5770,1220, hist);
            return hist;
        }
    }
}
uint16_t* stage2_MG_t7(float* fv) {
    static uint16_t hist[3];
    if(fv[gmuX] < 2445.45f) {
          if(fv[gmuX] < -1091.06f) {
              if(fv[muZ] < 17.6719f) {
                    assignProb(261,8560,1180, hist);
                    return hist;
                } else {
                    assignProb(9800,13,191, hist);
                    return hist;
                }
          } else {
              if(fv[IMA] < 6.48001f) {
                  assignProb(7120,2470,411, hist);
                  return hist;
              } else {
                  assignProb(344,344,9310, hist);
                  return hist;
              }
          }
    } else {

            if(fv[muZ] < 36.0625f) {
                assignProb(4080,5390,526, hist);
                return hist;
            } else {
                assignProb(75,9570,359, hist);
                return hist;
            }
        }

}
uint16_t* stage2_MG_t8(float* fv) {
    static uint16_t hist[3];
    if(fv[gmuX] < 2518.63f) {
          if(fv[IMA] < 21.2908f) {
              if(fv[muZ] < 3.3125f) {
              assignProb(102,9900,0, hist);
              return hist;
              } else {
                  assignProb(7100,589,2310, hist);
                  return hist;
              }
          } else {
              if(fv[gmuX] < -2659.73f) {
                  assignProb(6000,4000,0, hist);
                  return hist;
              } else {
                  assignProb(0,15,9990, hist);
                  return hist;
              }
          }
    } else {

            if(fv[muY] < 86.4531f) {
                assignProb(48,9680,275, hist);
                return hist;
            } else {
                assignProb(8300,741,963, hist);
                return hist;
            }

    }
}
uint16_t* stage2_MG_t9(float* fv) {
    static uint16_t hist[3];
    if(fv[miZ] < 51.5f) {
          if(fv[IMA] < 21.5764f) {
              if(fv[muY] < 13.26563f) {
                    assignProb(2440,3430,4120, hist);
                    return hist;
                } else {
                    assignProb(4640,4870,483, hist);
                    return hist;
                }
          } else {
              if(fv[gmuX] < -1834.14f) {
                  assignProb(6440,2050,1510, hist);
                  return hist;
              } else {
                  assignProb(134,697,9170, hist);
                  return hist;
              }
          }
    } else {

            if(fv[muY] < -2.875f) {
                assignProb(541,9050,405, hist);
                return hist;
            } else {
                assignProb(6670,2920,410, hist);
                return hist;
            }

    }
}
uint16_t* stage2_MG_t10(float* fv) {
    static uint16_t hist[3];
    if(fv[IMA] < 22.3761f) {
          if(fv[gmuX] < -156.203f) {
              if(fv[muZ] < 10.3438f) {
                    assignProb(81,9920,0, hist);
                    return hist;
                } else {
                    assignProb(9470,184,345, hist);
                    return hist;
                }
          } else {
              if(fv[gmuX] < 2194.23f) {
                  assignProb(789,1450,7760, hist);
                  return hist;
              } else {
                  assignProb(1520,8420,56, hist);
                  return hist;
              }
          }
    } else {
        if(fv[gmuX] < -1869.23f) {
            assignProb(6300,2190,1510, hist);
            return hist;
        } else {
            assignProb(151,549,9300, hist);
            return hist;
        }
    }
}

/* @fn stage2_Rig_RF*/

void stage2_R(float* fv, uint16_t * ret) {
    //compute 5 tress
    uint8_t j;
    static uint32_t s = 0,m=0,r=0;
    static uint16_t* t4[10];

    //each tree returns a probability
    for(j=0;j<10;j++) {
        t4[j] = tr4[j](fv);
        m = m + (uint32_t)t4[j][0]; //exe
        r = r + (uint32_t)t4[j][1]; //jum
        s = s + (uint32_t)t4[j][2]; //run
    }

    //we can tolerate some quantization
    s = (uint32_t)s/10;
    m = (uint32_t)m/10;
    r = (uint32_t)r/10;
    ret[0] = s;
    ret[1] = m;
    ret[2] = r;


    //biased towards the most likely from the statistical point of view..
    if( s>m && s>r ) {
        return Run;
    } else if( m > r && m>s) {
        return Exe;
    } else if( r>m && r>s){
        return Jum;
    } else if(r ==m) {
        int i = rand() % 2;
        if(i == 0) {
            return Jum;
        } else {
            return Exe;
        }
    } else if(r ==s) {
            srand(time(NULL));
            int i = rand() % 2;
            if(i == 0) {
                return Jum;
            } else {
                return Run;
            }
    } else if(s == m) {
        srand(time(NULL));
        int i = rand() % 2;
        if(i == 0) {
            return Run;
        } else {
            return Exe;
        }
    } else {
        return Exe;
    }
}

activity_t evalR(uint16_t s, uint16_t m, uint16_t r) {
    //biased towards the most likely from the statistical point of view..
    if( s>m && s>r ) {
        return Run;
    } else if( m > r && m>s) {
        return Exe;
    } else if( r>m && r>s){
        return Jum;
    } else if(r ==m) {
        int i = rand() % 2;
        if(i == 0) {
            return Jum;
        } else {
            return Exe;
        }
    } else if(r ==s) {
            srand(time(NULL));
            int i = rand() % 2;
            if(i == 0) {
                return Jum;
            } else {
                return Run;
            }
    } else if(s == m) {
        srand(time(NULL));
        int i = rand() % 2;
        if(i == 0) {
            return Run;
        } else {
            return Exe;
        }
    } else {
        return Exe;
    }
}



uint16_t*  stage2_R_t1(float* fv) {
    static uint16_t hist[3];
    if(fv[maY] < 439.5f) {
          if(fv[miY] < -709.0f) {
              assignProb(363,9640,0, hist);
              return hist;
          } else {
              if(fv[muY] < -86.3125f) {
                  assignProb(4300,423,5280, hist);
                  return hist;
              } else {
                  assignProb(6740,2550,712, hist);
                  return hist;
              }
          }
    } else {
        if(fv[vaZ] < 27296.3f) {
            if(fv[miY] < -51.5f) {
                assignProb(1050,3420,5530, hist);
                return hist;
            } else {
                assignProb(252,350,9400, hist);
                return hist;
            }
        } else {
            assignProb(891,8720,388, hist);
            return hist;
        }
    }
}
uint16_t*  stage2_R_t2(float* fv) {
    static uint16_t hist[3];
    if(fv[IMA] < 83.5631f) {

              if(fv[IMA] < 61.7826f) {
                  assignProb(9710,168,120, hist);
                  return hist;
              } else {
                  assignProb(7980,1440,577, hist);
                  return hist;
              }

    } else {
        if(fv[muY] < 158.969f) {
            if(fv[maY] < 100.5f) {
                assignProb(175,702,9120, hist);
                return hist;
            } else {
                assignProb(3500,5280,1220, hist);
                return hist;
            }
        } else {
            if(fv[miY] < -98.5f) {
                assignProb(714,6430,2860, hist);
                return hist;
            } else {
                assignProb(300,658,9040, hist);
                return hist;
            }
        }
    }
}
uint16_t*  stage2_R_t3(float* fv) {
    static uint16_t hist[3];
    if(fv[IMA] < 81.84f) {

              if(fv[maY] < 312.0f) {
                  assignProb(9820,131,44, hist);
                  return hist;
              } else {
                  assignProb(7570,1760,676, hist);
                  return hist;
              }

    } else {
        if(fv[muY] < 143.578f) {
            if(fv[miY] < -768.5f) {
                assignProb(331,9670,0, hist);
                return hist;
            } else {
                assignProb(4250,3720,2040, hist);
                return hist;
            }
        } else {
            if(fv[miY] < -106.5f) {
                assignProb(714,6830,2460, hist);
                return hist;
            } else {
                assignProb(494,951,8560, hist);
                return hist;
            }
        }
    }
}
uint16_t* stage2_R_t4(float* fv) {
    static uint16_t hist[3];
    if(fv[IMA] < 85.3261f) {
          if(fv[muY] < 143.781f) {
                  assignProb(9820,178,0, hist);
                  return hist;
              } else {
                  assignProb(6760,2030,1120, hist);
                  return hist;
              }
    } else {
        if(fv[IMA] < 229.144f) {
            if(fv[miY] < -653.0f) {
                assignProb(516,9480,0, hist);
                return hist;
            } else {
                assignProb(2460,1400,6140, hist);
                return hist;
            }
        } else {
            if(fv[vaZ] < 34982.0f) {
                assignProb(1430,6900,1670, hist);
                return hist;
            } else {
                assignProb(421,9580,0, hist);
                return hist;
            }
        }
    }
}
uint16_t*  stage2_R_t5(float* fv) {
    static uint16_t hist[3];
    if(fv[muY] < 152.563f) {
          if(fv[muY] < -113.5331f) {
              if(fv[IMA] < 103.035f) {
                    assignProb(9070,722,206, hist);
                    return hist;
                } else {
                    assignProb(182,6910,2910, hist);
                    return hist;
                }
          } else {
              if(fv[vaZ] < 49783.2f) {
                  assignProb(6440,2500,1060, hist);
                  return hist;
              } else {
                  assignProb(737,9260,0, hist);
                  return hist;
              }
          }
    } else {
        if(fv[miY] < -84.0f) {
                assignProb(1040,5950,3010, hist);
            return hist;

        } else {
            assignProb(242,861,8900, hist);
            return hist;
        }
    }
}
uint16_t* stage2_R_t6(float* fv) {
    static uint16_t hist[3];
    if(fv[IMA] < 85.3491f) {

              if(fv[muY] < 149.453f) {
                  assignProb(9730,190,84, hist);
                  return hist;
              } else {
                  assignProb(6800,2130,1070, hist);
                  return hist;
              }
    } else {
        if(fv[muY] < 158.969f) {
            if(fv[miY] < -709.0f) {
                assignProb(267,9730,0, hist);
                return hist;
            } else {
                assignProb(4010,3830,2170, hist);
                return hist;
            }
        } else {
            if(fv[miY] < -106.5f) {
                assignProb(355,7040,2600, hist);
                return hist;
            } else {
                assignProb(300,950,8740, hist);
                return hist;
            }
        }
    }
}
uint16_t* stage2_R_t7(float* fv) {
    static uint16_t hist[3];
    if(fv[IMA] < 88.8122f) {

              if(fv[maY] < 345.0f) {
                  assignProb(9610,308,77, hist);
                  return hist;
              } else {
                  assignProb(6890,2160,946, hist);
                  return hist;
              }

    } else {
        if(fv[muY] < 143.563f) {
            if(fv[miY] < -709.0f) {
                assignProb(356,9640,0, hist);
                return hist;
            } else {
                assignProb(4070,3780,2150, hist);
                return hist;
            }
        } else {
            if(fv[vaZ] < 30726.0f) {
                assignProb(545,1350,8100, hist);
                return hist;
            } else {
                assignProb(108,9510,378, hist);
                return hist;
            }
        }
    }
}
uint16_t*  stage2_R_t8(float* fv) {
    static uint16_t hist[3];
    if(fv[muY] < 147.016f) {
          if(fv[miY] < -754.5f) {
              assignProb(310,9690,0, hist);
              return hist;
          } else {
              if(fv[IMA] < 201.028f) {
                  assignProb(7690,517,1790, hist);
                  return hist;
              } else {
                  assignProb(2390,7000,613, hist);
                  return hist;
              }
          }
    } else {
        if(fv[IMA] < 239.703f) {
            if(fv[miY] < -123.5f) {
                assignProb(2570,3760,3660, hist);
                return hist;
            } else {
                assignProb(561,586,8850, hist);
                return hist;
            }
        } else {
            assignProb(47,8440,1520, hist);
            return hist;
        }
    }
}
uint16_t*  stage2_R_t9(float* fv) {
    static uint16_t hist[3];
    if(fv[muY] < 160.578f) {
          if(fv[IMA] < 96.2526f) {
              assignProb(9400,395,207, hist);
              return hist;
          } else {
              if(fv[maY] < 93.5f) {
                  assignProb(0,495,9500, hist);
                  return hist;
              } else {
                  assignProb(3340,5400,1260, hist);
                  return hist;
              }
          }
    } else {
        if(fv[vaZ] < 27414.3f) {
            if(fv[miY] < -103.5f) {
                assignProb(1050,4860,4090, hist);
                return hist;
            } else {
                assignProb(348,508,9140, hist);
                return hist;
            }
        } else {
            assignProb(1290,8310,398, hist);
            return hist;
        }
    }
}
uint16_t*  stage2_R_t10(float* fv) {
    static uint16_t hist[3];
    if(fv[IMA] < 85.2834f) {

              if(fv[maY] < 345.0f) {
                  assignProb(9710,187,104, hist);
                  return hist;
              } else {
                  assignProb(6320,1580,2110, hist);
                  return hist;
              }

    } else {
        if(fv[muY] < 162.891f) {
            if(fv[IMA] < 196.752f) {
                assignProb(4360,2710,2920, hist);
                return hist;
            } else {
                assignProb(1930,7580,483, hist);
                return hist;
            }
        } else {
            if(fv[miY] < -98.5f) {
            assignProb(777,5740,3490, hist);
            return hist;
            } else {
                assignProb(392,624,8980, hist);
                return hist;
            }

        }
    }
}
/* @fn intoInteger16 - turns byte array into byte array/2 16bit array, format is x,y,z ,
 *  direction: 0 for MSB last, 1 for MSB first  */
void intoInteger16(uint8_t *input, int16_t *output, uint8_t d) {
    //t0 = Timestamp_get32();
    //re-shuffle to get 32 x y z samples
    uint8_t j,n;
    for (n=0;n<SAMPLES_COUNT;n++) {
        j = n*2;
        // MSB | LSB : only 4 bits are used from the MSB though, could do with 8 bit value instead
        output[n] = (short)((((unsigned short) (int)input[j+1-d]) << 8) | (int)input[j+d]);
    }
    //t1 = Timestamp_get32();
    //time = t1 - t0;
}

/* @fn getIMA - computes IMA for a fixed window */
float getIMA (int16_t *input) {
    //t0 = Timestamp_get32();
    uint8_t n,j;
    float sum = 0,ima=0,partial=0;
    for (n=0;n<WINDOW_SIZE;n++) {
        j = n*3;
        partial =  (float)sqrt( (uint32_t)(input[j]*input[j]) + (uint32_t)(input[j+1]*input[j+1]) +
                        (uint32_t)(input[j+2]*input[j+2]) );
        //minus the gravity
        partial = (float)fabs(partial - 255);
        sum = sum + partial;
    }
    ima = (sum/(float)WINDOW_SIZE);
    //t1 = Timestamp_get32();
    //time = t1 - t0;
    return ima;
}
/* @fn getMU - computes MU for a fixed window, axis = 0 => x, 1=> y, 2=> z */
float getMU (int16_t *input, uint8_t axis) {
    //t0 = Timestamp_get32();
    uint8_t n,j;
    float mu=0,partial=0;
    for (n=0;n<WINDOW_SIZE;n++) {
        j = (n*3) + axis;
        partial += (float)input[j];
    }
    mu = (partial/(float)WINDOW_SIZE);
    //t1 = Timestamp_get32();
    //time = t1 - t0;
    return mu;
}
/* @fn getMIN - computes MIN for a fixed window, axis = 0 => x, 1=> y, 2=> z */
float getMIN (int16_t *input, uint8_t axis) {
    //t0 = Timestamp_get32();
    uint8_t n,j;
    int16_t min= input[0];
    for (n=1;n<WINDOW_SIZE;n++) {
        j = (n*3) + axis;
        if (input[j] < min) {
            min = input[j];
        }
    }
    //t1 = Timestamp_get32();
    //time = t1 - t0;
    return (float)min;
}
/* @fn getMAX - computes MAX for a fixed window, axis = 0 => x, 1=> y, 2=> z */
float getMAX (int16_t *input, uint8_t axis) {
    //t0 = Timestamp_get32();
    uint8_t n,j;
    int16_t max=input[0];
    for (n=1;n<WINDOW_SIZE;n++) {
        j = (n*3) + axis;
        if (input[j] > max) {
            max = input[j];
        }
    }
    //t1 = Timestamp_get32();
    //time = t1 - t0;
    return (float)max;
}
/* @fn getVAR - computes VAR for a fixed window, axis = 0 => x, 1=> y, 2=> z */
float getVAR(int16_t *input, uint8_t axis, float mu) {
    uint8_t n;
    float s=0.0;
    for (n=0;n<WINDOW_SIZE;n++) {
            n = (n*3) + axis;
            s = s + pow(input[n] - mu, 2);
        }
    return (s/ ( (float)WINDOW_SIZE-1 ) );
}
/* @fn getMED - computes MEDIAN for a fixed window, axis = 0 => x, 1=> y, 2=> z */
float getMED (int16_t *input, uint8_t axis) {
    uint8_t i,j,n;
    int16_t partial[WINDOW_SIZE], regA;
    float med;
    for (n=0;n<WINDOW_SIZE;n++) {
        j = (n*3) + axis;
        //get our single axis array
        partial[n] = input[j]; //j
    }
    n = WINDOW_SIZE;
    for(i=0; i<n-1; i++) {
            for(j=i+1; j<n; j++) {
                if(partial[j] < partial[i]) {
                    // swap elements
                    regA = partial[i];
                    partial[i] = partial[j];
                    partial[j] = regA;
                }
            }
        }
    med = ((float)(partial[n/2] + partial[n/2 - 1]) / 2.0);
    return med;
}
