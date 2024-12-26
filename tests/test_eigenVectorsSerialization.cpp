//
// Created by frongere on 28/02/19.
//

#include <gtest/gtest.h>
#include "hermes/hermes.h"

#include "Eigen/Dense"

TEST(Hermes, EigenVectorSerialization) {
    Eigen::Vector3d vector3d;
    int a = 0;

    hermes::Message msg;
    msg.SetNameAndDescription("Message test", "Message test with an int and a vector");
    msg.AddField("integer", "_", "my integer", &a);
    msg.AddField("vector", "m/s", "velocity", &vector3d);

//    msg.AddPrintSerializer();
    msg.AddCSVSerializer("test_eigen_serialization.csv");
    msg.Initialize();
    msg.Send();

    int i = 0;
    while (i < 1000) {
        i++;
        vector3d = Eigen::Vector3d::Random();
        msg.Serialize();
        msg.Send();
//        std::cout << vector3d << std::endl;
    }
}
