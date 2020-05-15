#include "rossubscriber.h"

RosSubscriber::RosSubscriber()
{
    this->topicName = "/robot_mobile_wheel_3_omni_open_base/pos";

    this->future = this->promise.get_future();
    this->runThread =
         std::thread(std::bind(&RosSubscriber::runRoutine,this));
    //th.detach();
}


RosSubscriber::~RosSubscriber()
{
    this->promise.set_value();
    this->runThread.join();
    QObject::deleteLater();
}


void RosSubscriber::runRoutine()
{
    // Subsribe
    std::unique_ptr<ros::NodeHandle> n;
    n.reset(new ros::NodeHandle("Client_subscriber"));
    auto sub = n->subscribe<CartConrolPlugin::Position>
            (topicName, 1000,
             boost::bind(&RosSubscriber::callBack,this,_1));

    auto loopRate = ros::Rate(1000);

    // While promise doesn't set it's value
    while(this->future.wait_for(std::chrono::microseconds(1)) == std::future_status::timeout)
    {
        ros::spinOnce();
        loopRate.sleep();
    }

    n->shutdown();
}

void RosSubscriber::callBack(const CartConrolPlugin::Position::ConstPtr &msg)
{
    QPointF p;
    p.setX(msg->x);
    p.setY(msg->y);
    emit getPos(p);
}