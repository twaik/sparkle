
#include "common/sparkle_client.h"
#include "common/sparkle_protocol.h"
#include "common/sparkle_packet.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "widget_host.h"
#include "widget_button.h"

const char *compositor = "/dev/shm/sparkle.socket";
const char *surface_name = "ui";
const char *surface_file = "/dev/shm/ui";


struct _tester_t
{
    int surface_fd;
    void *surface_data;
    sparkle_client_t *sparkle;
    int size;
};
typedef struct _tester_t tester_t;


void handle_data(unsigned char *data, int size, void *user)
{
    tester_t *tester = (tester_t *)user;
}

void damage(int x1, int y1, int x2, int y2, void *user)
{
    tester_t *tester = (tester_t *)user;
    
    fprintf(stdout, "dmg\n");
    
        sparkle_packet_t *packet = sparkle_packet_create(NULL, 64);
        sparkle_packet_add_uint32(packet, SPARKLE_ADD_SURFACE_DAMAGE);
        sparkle_packet_add_string(packet, surface_name);
        sparkle_packet_add_uint32(packet, x1);
        sparkle_packet_add_uint32(packet, y1);
        sparkle_packet_add_uint32(packet, x2);
        sparkle_packet_add_uint32(packet, y2);
        sparkle_client_send(tester->sparkle, sparkle_packet_data(packet), sparkle_packet_position(packet));
        sparkle_packet_destroy(packet);
}

void handle_connection(void *user)
{
    tester_t *tester = (tester_t *)user;
    
    fprintf(stderr, "Connected\n");
    
    sparkle_packet_t *packet = sparkle_packet_create(NULL, 64);
    sparkle_packet_add_uint32(packet, SPARKLE_REGISTER_SURFACE);
    sparkle_packet_add_string(packet, surface_name);
    sparkle_client_send(tester->sparkle, sparkle_packet_data(packet), sparkle_packet_position(packet));
    sparkle_packet_destroy(packet);
    
    packet = sparkle_packet_create(NULL, 64);
    sparkle_packet_add_uint32(packet, SPARKLE_SET_SURFACE_TYPE);
    sparkle_packet_add_string(packet, surface_name);
    sparkle_packet_add_uint32(packet, 1);
    sparkle_client_send(tester->sparkle, sparkle_packet_data(packet), sparkle_packet_position(packet));
    sparkle_packet_destroy(packet);
    
    packet = sparkle_packet_create(NULL, 64);
    sparkle_packet_add_uint32(packet, SPARKLE_SET_SURFACE_FILE);
    sparkle_packet_add_string(packet, surface_name);
    sparkle_packet_add_string(packet, surface_file);
    sparkle_client_send(tester->sparkle, sparkle_packet_data(packet), sparkle_packet_position(packet));
    sparkle_packet_destroy(packet);
                
    
    packet = sparkle_packet_create(NULL, 64);
    sparkle_packet_add_uint32(packet, SPARKLE_SET_SURFACE_SIZE);
    sparkle_packet_add_string(packet, surface_name);
    sparkle_packet_add_uint32(packet, 320);
    sparkle_packet_add_uint32(packet, 240);
    sparkle_client_send(tester->sparkle, sparkle_packet_data(packet), sparkle_packet_position(packet));
    sparkle_packet_destroy(packet);
    
    packet = sparkle_packet_create(NULL, 64);
    sparkle_packet_add_uint32(packet, SPARKLE_SET_SURFACE_POSITION);
    sparkle_packet_add_string(packet, surface_name);
    sparkle_packet_add_uint32(packet, 0);
    sparkle_packet_add_uint32(packet, 0);
    sparkle_packet_add_uint32(packet, 800);
    sparkle_packet_add_uint32(packet, 600);
    sparkle_client_send(tester->sparkle, sparkle_packet_data(packet), sparkle_packet_position(packet));
    sparkle_packet_destroy(packet);
    
        packet = sparkle_packet_create(NULL, 64);
        sparkle_packet_add_uint32(packet, SPARKLE_ADD_SURFACE_DAMAGE);
        sparkle_packet_add_string(packet, surface_name);
        sparkle_packet_add_uint32(packet, 0);
        sparkle_packet_add_uint32(packet, 0);
        sparkle_packet_add_uint32(packet, 320);
        sparkle_packet_add_uint32(packet, 240);
        sparkle_client_send(tester->sparkle, sparkle_packet_data(packet), sparkle_packet_position(packet));
        sparkle_packet_destroy(packet);
}

void handle_disconnection(void *user)
{
    tester_t *tester = (tester_t *)user;
}

int main(int argc, char *argv[])
{
    tester_t *tester = (tester_t *)malloc(sizeof(tester_t));
    
    
    tester->size = 320 * 240 * 4;
    
    
    tester->surface_fd = open(surface_file, O_CREAT | O_RDWR);
    if (tester->surface_fd == -1)
    {
	    return -1;
    }
    
    if (ftruncate(tester->surface_fd, tester->size) == -1)
    {
	    return -1;
    }
    
    if (fchmod(tester->surface_fd, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH) == -1)
    {
	    return -1;
    }
    
    tester->surface_data = mmap(NULL, tester->size, PROT_READ | PROT_WRITE, MAP_SHARED, tester->surface_fd, 0);
    if (tester->surface_data == MAP_FAILED)
    {
	    return -1;
    }
    
    //memset(tester->surface_data, 0xFF, tester->size);

    
    tester->sparkle = sparkle_client_create();
    if (tester->sparkle == NULL)
    {
	    return -1;
    }
    
    sparkle_client_add_compositor(tester->sparkle, compositor);
    sparkle_client_set_connection_handler(tester->sparkle, handle_connection, tester);
    sparkle_client_set_disconnection_handler(tester->sparkle, handle_disconnection, tester);
    sparkle_client_set_data_handler(tester->sparkle, handle_data, tester);
    
    
    
    WidgetHost *host = new WidgetHost((unsigned char *)tester->surface_data, 320, 320, 240);
    host->damageCallback = damage;
    host->damageCallbackUser = tester;
    
    WidgetButton *button = new WidgetButton();
    host->addWidget(button, 
        RectangleC(PointC(CoordinateC(0.0, 0), CoordinateC(0.0, 0)), PointC(CoordinateC(0.5, 0), CoordinateC(0.5, 0)))
    );
    
    
    for (;;)
    {
        sparkle_client_connect(tester->sparkle);
        sparkle_client_process(tester->sparkle);
        

        
        usleep(1000000);
    }
    
    
    delete button;
    delete host;
    
    sparkle_client_destroy(tester->sparkle);
    munmap(tester->surface_data, tester->size);
    close(tester->surface_fd);
    
    
    free(tester);
    
    fprintf(stdout, "Ok\n");
    
    
    return 0;
}

