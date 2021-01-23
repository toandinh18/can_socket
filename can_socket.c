#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/can.h>
#include <sys/ioctl.h>
#include <net/if.h>

int main() {
    int s;
    struct sockaddr_can addr;
    struct ifreq ifr;
    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (s < 0) {
        perror("Can socket");
        return 1;
    }
    
    //declare socket interface we want to bind
    strcpy(ifr.ifr_name, "vcan0" );
    ioctl(s, SIOCGIFINDEX, &ifr);
    
    //create socketCAN
    memset(&addr, 0, sizeof(addr));
	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;
    

    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    	perror("Bind");
        return 1;
    } 
    //
    // Create can frame to write to socket
    struct can_frame frame;
    frame.can_id = 0x555;
    frame.can_dlc = 5;
    sprintf(frame.data, "Hello");
    if (write(s, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
           perror("Write");
           return 1;
    }

    // read can frame using read
    
    int nbytes;
    struct can_frame frame_read;
    nbytes = read(s, &frame_read, sizeof(struct can_frame));
    if (nbytes < 0) {
           perror("Read");
           return 1;
    }
    printf("0x%03X [%d] ",frame_read.can_id,frame_read.can_dlc);
    for (int i = 0; i < frame_read.can_dlc; i++)
           printf("%c ",frame_read.data[i]);
    printf("\r\n");
    
    // Close the socket
    if (close(s) < 0) {
           perror("Close");
           return 1;
    }
    // Using can-utils to create virtual can interface driver
    // sudo ip link add dev vcan0 type vcan 
    // sudo ifconfig vcan0 up

    return 0;
}

