#include <gtest/gtest.h>
#include <cucumber-cpp/autodetect.hpp>

#include <thread>
#include <string>
#include <regex>
#include <map>

#include "soap_validator.h"
#include "onvif_server.h"
#include "device_service_handler.h"

using cucumber::ScenarioScope;

static int find_available_port()
{
    /* Use a range of ports for testing to avoid conflicts */
    static int test_port = 18080;
    return test_port++;
}

class OnvifServerTestCtx : public std::enable_shared_from_this<OnvifServerTestCtx>
{
public:
    ~OnvifServerTestCtx()
    {
        stop();
    }

    onvif_server_t server;

    // 启动服务器（安全版本）
    void start()
    {
        auto self = shared_from_this(); // 增加引用计数

        worker_thread = std::thread([this, self]()
                                    {
                                        // self 确保server对象在线程执行期间不被销毁
                                        onvif_server_start(&server);
                                        sleep(1); // 等待一秒，确保server启动完成
                                    });
    }

    // 停止服务器
    void stop()
    {
        onvif_server_destroy(&server);
        if (worker_thread.joinable())
        {
            worker_thread.join(); // 等待线程退出
        }
    }

public:
    char response[4096];
    std::string result_in_response;

private:
    std::thread worker_thread;
};

GIVEN("^the ONVIF server is configured on port (\\d+)$")
{
    REGEX_PARAM(int, port);

    cucumber::ScenarioScope<OnvifServerTestCtx> ctx;
    int result = onvif_server_init(&(ctx->server), port);

    EXPECT_EQ(result, ONVIF_SERVER_OK);
}

WHEN("^I start the ONVIF server$")
{
    cucumber::ScenarioScope<OnvifServerTestCtx> ctx;

    // server start 阻塞式，测试用例会卡住，在这里需要用独立线程方式启动
    ctx->start();
}

// 这个函数仅仅负责判断是否运行
THEN("^the server should be running successfully$")
{
    cucumber::ScenarioScope<OnvifServerTestCtx> ctx;

    bool result = onvif_server_is_running(&(ctx->server));
    EXPECT_EQ(result, true);
}

// 这个描述类似前置条件，需要保障服务器已经启动
GIVEN("^the ONVIF server is running$")
{
    cucumber::ScenarioScope<OnvifServerTestCtx> ctx;

    bool result = onvif_server_is_running(&(ctx->server));
    int count = 0;
    if (false == result)
    {
        while (true && count < 100)
        {
            count++;
            int port = find_available_port();
            bool ok = is_port_available(port, NULL);
            if (ok)
            {
                onvif_server_init(&(ctx->server), port);
                ctx->start();
                break;
            }
        }
        sleep(1);
    }

    result = onvif_server_is_running(&(ctx->server));
    EXPECT_EQ(result, true);
}

THEN("^the server should be listening on the configured port$")
{
    cucumber::ScenarioScope<OnvifServerTestCtx> ctx;

    std::cout << "检查服务端口占用情况" << std::endl;

    bool result = is_port_available(ctx->server.port, NULL);
    EXPECT_EQ(result, false);

    std::cout << "检查服务端口占用结束" << std::endl;
}

// 这里类似 集成|单元 的测试，不是系统级别的从客户端连接开始的方式
WHEN("^a client requests device information$")
{
    cucumber::ScenarioScope<OnvifServerTestCtx> ctx;

    ctx->response[0] = 0;
    handle_get_device_information(&(ctx->server), ctx->response, sizeof(ctx->response));
}

THEN("^the server should return (.*) information$")
{
    cucumber::ScenarioScope<OnvifServerTestCtx> ctx;

    // "manufacturer"      <---> "<tds:Manufacturer>%s</tds:Manufacturer>"
    // "model"             <---> "<tds:Model>%s</tds:Model>"
    // "firmware version"  <---> "<tds:FirmwareVersion>%s</tds:FirmwareVersion>"
    // "serial number"     <---> "<tds:SerialNumber>%s</tds:SerialNumber>"
    REGEX_PARAM(std::string, information_type);

    std::regex pattern;
    if (information_type == "manufacturer")
    {
        pattern = R"(<tds:Manufacturer>(.*?)</tds:Manufacturer>)";
    }
    else if (information_type == "model")
    {
        pattern = R"(<tds:Model>(.*?)</tds:Model>)";
    }
    else if (information_type == "firmware version")
    {
        pattern = R"(<tds:FirmwareVersion>(.*?)</tds:FirmwareVersion>)";
    }
    else if (information_type == "serial number")
    {
        pattern = R"(<tds:SerialNumber>(.*?)</tds:SerialNumber>)";
    }
    else
    {
        ASSERT_STREQ("Not a valid information type", information_type.c_str());
        return;
    }

    // 仅仅检查有无，并不对具体值进行确认，本书取值和具体设备相关，需要产品/系统级别测试
    std::smatch match;
    std::string text = std::string(ctx->response);
    if (std::regex_search(text, match, pattern))
    {
        ctx->result_in_response = std::string(match[1].str());

        // match[0] 是整个匹配的字符串
        // match[1] 是第一个捕获组，即标签间的内容
        ASSERT_STRNE("", match[1].str().c_str());
    }
    else
    {
        ASSERT_STREQ("No valid information", information_type.c_str());
    }
}

// 仅要求满足基础SOAP规范
THEN("^the response should be valid SOAP format$")
{
    cucumber::ScenarioScope<OnvifServerTestCtx> ctx;

    std::cout << "return SOAP data: " << ctx->response << std::endl;

    SOAPMessageValidator validator;
    SOAPMessageValidator::SOAPValidationDetail detail = validator.validate(ctx->response);
    validator.printValidationResult(detail);

    // 暂时没有做约束，只是作了检查结果输出
    // <TODO> 详细内容待确认
}

WHEN("^a client requests device capabilities$")
{
    cucumber::ScenarioScope<OnvifServerTestCtx> ctx;

    ctx->response[0] = 0;
    handle_get_capabilities(&(ctx->server), ctx->response, sizeof(ctx->response));
}

THEN("^the server should return (.*) capability$")
{
    cucumber::ScenarioScope<OnvifServerTestCtx> ctx;

    // "Device service" <---> "<tds:FirmwareVersion>%s</tds:FirmwareVersion>"
    // "Media service"  <---> "<tds:FirmwareVersion>%s</tds:FirmwareVersion>"
    REGEX_PARAM(std::string, capability_type);

    std::regex pattern;
    if (capability_type == "Device service")
    {
        pattern = R"(<tt:Device>(.*?)</tt:Device>)";
    }
    else if (capability_type == "Media service")
    {
        pattern = R"(<tt:Media>(.*?)</tt:Media>)";
    }
    else
    {
        ASSERT_STREQ("Not a valid capability type", capability_type.c_str());
        return;
    }

    // 仅仅检查有无，并不对具体值进行确认，本书取值和具体设备相关，需要产品/系统级别测试
    std::smatch match;
    std::string text = std::string(ctx->response);
    if (std::regex_search(text, match, pattern))
    {
        // match[0] 是整个匹配的字符串
        // match[1] 是第一个捕获组，即标签间的内容
        ASSERT_STRNE("", match[1].str().c_str());
    }
    else
    {
        ASSERT_STREQ("No valid capability", capability_type.c_str());
        return;
    }

    std::regex data_regex(R"(<tt:XAddr>(.*?)</tt:XAddr>)");
    std::string matched_text = match[0].str();
    if (std::regex_search(matched_text, match, data_regex))
    {
        ctx->result_in_response = std::string(match[1].str());

        // match[0] 是整个匹配的字符串
        // match[1] 是第一个捕获组，即标签间的内容
        ASSERT_STRNE("", match[1].str().c_str());
    }
    else
    {
        ASSERT_STREQ("No valid capability", capability_type.c_str());
        return;
    }
}

THEN("^the server should return service URLs$")
{
    cucumber::ScenarioScope<OnvifServerTestCtx> ctx;

    // 这里的service URLs是否有特指？
    // 没有做检查，当前返回的内容，如：
    // http://0.0.0.0:18080/onvif/media_service
    std::cout << "return service URLs: " << ctx->result_in_response << std::endl;
}
