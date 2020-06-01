#include "ros/ros.h"
#include "can_msgs/Frame.h"
#include "std_msgs/Bool.h"

#include <stdint.h>


void recievedCallback(const can_msgs::Frame::ConstPtr& msg);


bool lidarsOK = true;
bool obsStop = false;
bool detectBR = false;
bool detectBL = false;
bool detectFR = false;
bool detectFL = false;

int main(int argc, char **argv)
{
    ros::init(argc, argv, "sick_plc_node");
    ros::NodeHandle n;
    ros::Rate loop_rate(10);


    ros::Subscriber sub = n.subscribe("received_messages", 1000, recievedCallback);
    ros::Publisher lidarsOK_pub = n.advertise<std_msgs::Bool>("safety/lidarsOK", 1000);
    ros::Publisher obsStop_pub = n.advertise<std_msgs::Bool>("safety/obsticleStop", 1000);


    std_msgs::Bool lidOK;
    std_msgs::Bool obsSt;

    while (ros::ok())
    {
        lidOK.data = lidarsOK;
        obsSt.data = obsStop;

        lidarsOK_pub.publish(lidOK);
        obsStop_pub.publish(obsSt);

        ros::spinOnce();
    }

    return 0;
}


void recievedCallback(const can_msgs::Frame::ConstPtr& msg)
{
    uint8_t data = 0;

    if (msg->id == 404)
    {
        data = msg->data[0];

        detectFL = data & 0b1;
        detectFR = (data>>1) & 0b1;
        detectBL = (data>>2) & 0b1;
        detectBR = (data>>3) & 0b1;

        obsStop = (data>>4) & 0b1;
        lidarsOK = (data>>5) & 0b1;

        ROS_INFO("ID: 20 - TxPDO1 Recieved, %d, %d", lidarsOK, obsStop);
    }

}
