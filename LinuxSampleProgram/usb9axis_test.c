//------------------------------------------------------------------------------
// Copyright (c) 2018 RT Corporation
// All rights reserved.

// License: BSD

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:

// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.

// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.

// * Neither the name of RT Corporation nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>

#include <termios.h>
#include <fcntl.h>
#include <unistd.h>

volatile sig_atomic_t e_flag = 0;

void abrt_handler(int sig){
    e_flag = 1;
}

int read_data(int fd, unsigned char* buf, unsigned int buf_len) {
    if(fd < 0) {
        return -1;
    }

    unsigned int left_len = buf_len;
    unsigned char* p = buf;
    while(left_len > 0) {
        //Read device file and put the data into buf (max size is (buf_len) bytes)
        int size = read(fd, p, left_len);
        if(size <= 0) {
            return -1;
        }
        p += size;
        left_len -= size;
    }

    return (buf_len - left_len);
}


int main(int argc, char **argv){
    char serial_port[128]="/dev/ttyACM0";
    const int DATA_SIZE = 28;

    if(signal(SIGINT, abrt_handler) == SIG_ERR){
        exit(1);
    }

    errno = 0;
    // Port Open
    int fd = open(serial_port, O_RDWR | O_NOCTTY);

    if(fd < 0) {
        printf("Open Failed : %s\n", serial_port);
        perror("Error");
        exit(1);
    }

    // Port Settings
    struct termios settings;

    cfsetispeed(&settings, B57600);
    cfmakeraw(&settings);
    tcsetattr(fd, TCSANOW, &settings);

    // Outputs until Ctrl-c intrrupt
    while(!e_flag){
        unsigned char imu_data_buf[DATA_SIZE];

        int readDataSize = read_data(fd, imu_data_buf, DATA_SIZE);
        if(readDataSize != DATA_SIZE){
            if(readDataSize == -1){
                printf("Read Failed : %s\n", serial_port);
                exit(1);
            }
            continue;
        }

        printf("==========DATA_SIZE:%d==========\n", DATA_SIZE);
        for(int i=0; i<DATA_SIZE; i++){
            printf("%2d : %2x\n",i, imu_data_buf[i]);
        }
    }

    // Port Close
    if(close(fd) != 0){
        printf("Close Failed : %s\n", serial_port);
        perror("Error");
        exit(1);
    }else{
        printf("Finish\n");
    }

    return 0;
}
