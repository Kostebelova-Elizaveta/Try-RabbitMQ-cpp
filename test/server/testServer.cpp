#include <gtest/gtest.h>
#include "rabbitserver.h"

TEST(ServerCheck, DefaultSettingsCheck) {
    RabbitServer *server = new RabbitServer();

    EXPECT_EQ(server->getHostname(), "localhost");
    EXPECT_EQ(server->getPort(), 5672);
    EXPECT_EQ(server->getExchange(), "amq.direct");
    EXPECT_EQ(server->getBingkey(), "test");

    delete server;
}

TEST(ServerCheck, RequestToDoubleNormal) {
    RabbitServer *server = new RabbitServer();
    TestTask::Messages::Request request;
    request.set_id("testNormal");
    request.set_req(54);

    EXPECT_EQ(server->doubleReq(request).res(), 108);

    delete server;
}

TEST(ServerCheck, RequestToDoubleZero) {
    RabbitServer *server = new RabbitServer();
    TestTask::Messages::Request request;
    request.set_id("testZero");
    request.set_req(0);

    EXPECT_EQ(server->doubleReq(request).res(), 0);

    delete server;
}

TEST(ServerCheck, RequestToDoubleNegative) {
    RabbitServer *server = new RabbitServer();
    TestTask::Messages::Request request;
    request.set_id("testNegative");
    request.set_req(-9);

    EXPECT_EQ(server->doubleReq(request).res(), -18);

    delete server;
}

TEST(ServerCheck, InvalidRequest) {
    RabbitServer *server = new RabbitServer();
    TestTask::Messages::Request request;
    std::string m_str = "nothing";
    request.ParseFromArray(m_str.c_str(), m_str.length());

    EXPECT_THROW(server->doubleReq(request), std::runtime_error);

    delete server;
}
