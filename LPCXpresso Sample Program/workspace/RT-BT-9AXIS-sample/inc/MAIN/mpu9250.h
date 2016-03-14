#ifndef MPU9250_H
#define MPU9250_H

void initOmegaRef(void);
void initMPU9250 (void);
void initAK8963 (void);

void getDataFromMPU9250 (uint8_t *dest_array);
void getDataFromAK8963 (uint8_t *dest_array);

void whoAmI(void);


void getOmega(float *vec);
void getMag(float *vec);
void getAcc(float *vec);
void getTemp(float *temp);

void getOmegaLPF(float *vec);
void getMagLPF(float *vec);
void getAccLPF(float *vec);

void getOmegaStrLPF(float *vec);
void getMagStrLPF(float *vec);
void getAccStrLPF(float *vec);

float getOmegaRef(float *vec);

void setOmegaRef_x(float ref_x);
void setOmegaRef_y(float ref_y);
void setOmegaRef_z(float ref_z);

void debugMPU9250(void);

#endif
